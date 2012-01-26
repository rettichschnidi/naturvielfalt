<?php

/**
 * @file SchemaGenerator.php
 * @author Reto Schneider, 2011, github@reto-schneider.ch
 * @copyright 2012 Naturwerk, Brugg
 *
 * Converts a DLL-Schema (*.sql, PostgreSQL), generated by ERMaster (ermaster.sf.net)
 * into a Drupal Schema (Schema API)
 *
 * WARNING: its a quick hack and does just what I need (hopefully) and that's it - its
 * not nice, it's not flexible and there are no comments. Take it or leave it.
 */
$arguments = getopt("i:o:m:");

if (!file_exists($arguments['i'])) {
	die("File " . $arguments['i'] . " does not exist!\n");
}
$filename = $arguments['i'];

if (!isset($arguments['m'])) {
	die("No modulname (-m) given!\n");
}
$modulname = $arguments['m'];

function getNextLine(&$input) {
	if ($line = fgets($input)) {
		return trim($line);
	}
	return false;
}

function getComment(&$line) {
	if (preg_match('/^--(.+)/', $line, $match)) {
		return $match[1];
	}
	return false;
}

function getColumnName(&$line) {
	if (preg_match('/([a-z_]+)\ ([a-z])+.*/', $line, $match)) {
		return $match[1];
	}
	return false;
}

function pruneTypesAndSize(&$column) {
	$type = $column['type'];
	switch($type) {
		case 'boolean' :
			$column['type'] = 'int';
			$column['size'] = 'tiny';
			break;
		case 'varchar' :
			$column['size'] = 'normal';
			break;
		case 'bigint' :
			$column['type'] = 'int';
			$column['size'] = 'big';
			break;
		case 'text' :
			if (isset($column['default'])) {
				fwrite(STDERR, "Warning: text fields can not have a default value, dropping '" . $column['default'] . "'.\n");
				unset($column['default']);
			}
			break;
	}
}

function getColumnType(&$table, $lastcolumn, &$line) {
	if (preg_match('/([a-z_]+) ([a-z]+)\(?([0-9]*)\)?\.*/', $line, $match)) {
		$table['fields'][$lastcolumn]['type'] = $match[2];
		if ($match[3]) {
			$table['fields'][$lastcolumn]['size'] = $match[3];
		}
		if (preg_match('/^.*NOT NULL.*/', $line)) {
			$table['fields'][$lastcolumn]['not null'] = 'TRUE';
		}
		if (preg_match('/^.*UNIQUE.*/', $line)) {
			$table['unique keys'][$lastcolumn] = array($lastcolumn);
		}
		if (preg_match('/^.*DEFAULT \'(.+)\'.*/', $line, $matchdef)) {
			$default = $matchdef[1];
			if ($default != "''''::character varying") {
				$table['fields'][$lastcolumn]['default'] = $default;
			}
		}
		if (preg_match('/^.*DEFAULT ([0-9]+).*/', $line, $matchdef)) {
			if (isset($matchdef[1])) {
				$default = $matchdef[1];
				$table['fields'][$lastcolumn]['default'] = $default;
			}
		}
		pruneTypesAndSize($table['fields'][$lastcolumn]);
		return true;
	}
	return false;
}

function getUnique(&$line) {
	if (preg_match('/^UNIQUE\ \(([a-z0-9_,\ ]+)\),?$/', $line, $match)) {
		$found = $match[1];
		$keys = preg_split('/,/', $found);
		return $keys;
	}
	return false;
}

function getPrimaryKey(&$line) {
	if (preg_match('/^PRIMARY KEY\ \(([a-z0-9_,\ ]*)\),?$/', $line, $match)) {
		$found = $match[1];
		$keys = preg_split('/,/', $found);
		return $keys;
	}
	return false;
}

function handleDropStatement(&$schema, &$input, &$line) {
	if (preg_match('/^DROP\ .*$/', $line, $match) == 1) {
		$line = getNextLine($input);
		return true;
	}
	return false;
}

function handleComment(&$schema, &$input, &$line) {
	if (preg_match('/^\/\*.*\*\/$/', $line, $match) == 1) {
		$line = getNextLine($input);
		return true;
	}
	return false;
}

function handleAlterTable(&$schema, &$input, &$line) {
	if (preg_match('/^ALTER TABLE ([a-z]*\.)?([a-z0-9_]+)$/', $line, $match) == 1) {
		$tablename = $match[2];
		$line = getNextLine($input);
		if (preg_match('/^ADD FOREIGN KEY \(([a-z0-9_]+)\)$/', $line, $matchFK) == 1) {
			$columnname = $matchFK[1];
			$line = getNextLine($input);
		} else {
			die("Unsupported statement within ALTER TABLE: '" . $line . "'");
		}
		if (preg_match('/^REFERENCES ([a-z]+\.)?([a-z0-9_]+) \(([0-9a-z_]+)\)$/', $line, $matchREF) == 1) {
			$foreigntablename = $matchREF[2];
			$foreigncolumnname = $matchREF[3];
			$line = getNextLine($input);
		} else {
			die("Unsupported statement within ALTER TABLE: '" . $line . "'");
		}
		while (preg_match('/^ON (DELETE|UPDATE) (RESTRICT|NO ACTION|CASCADE)$/', $line) == 1) {
			$line = getNextLine($input);
		}
		if (!isset($schema[$tablename]['foreign keys'])) {
			$schema[$tablename]['foreign keys'] = array();
		}
		$fkdescription = $tablename . '_2_' . $foreigntablename;
		$schema[$tablename]['foreign keys'][$fkdescription] = array(
				'table' => $foreigntablename,
				'columns' => array($columnname => $foreigncolumnname),
		);
		if (preg_match('/^;$/', $line) == 1) {
			$line = getNextLine($input);
		}
		return true;
	}
	return false;
}

function handleCommentOnColumn(&$schema, &$input, &$line) {
	if (preg_match('/^COMMENT ON COLUMN ([a-z]*\.)?([a-z0-9_]+)\.([a-z0-9_]+) IS \'(.+)\';$/', $line, $match) == 1) {
		$table = $match[2];
		$column = $match[3];
		$description = $match[4];
		$schema[$table]['fields'][$column]['description'] = $description;
		$line = getNextLine($input);
		return true;
	}
	return false;
}

/**
 * CREATE INDEX organism_type ON public.organism USING BTREE ();
 * CREATE INDEX left_value ON organism_classification (left_value);
 */
function handleCreateIndex(&$schema, &$input, &$line) {
	if (preg_match('/^CREATE INDEX ([a-zA-Z0-9_]+) ON ([a-z]*\.)?([a-zA-Z0-9_]+).*\(([a-zA-Z0-9_]+)\);$/', $line, $match) == 1) {
		$indexname = $match[1];
		$table = $match[3];
		$column = $match[4];

		if (!isset($schema[$table]['indexes'])) {
			$schema[$table]['indexes'] = array();
		}
		$schema[$table]['indexes'][$indexname] = array($column);
		$line = getNextLine($input);
		return true;
	}
	return false;
}

function handleCreateTable(&$schema, &$input, &$line) {
	if (preg_match('/^CREATE\ TABLE\ ([a-z]*\.)?([a-z_]+)$/', $line, $match) == 1) {
		$tabName = $match[2];
		// eat (
		getNextLine($input);
		$lastcolumn = false;
		while ($line = getNextLine($input)) {
			$comment = false;
			while ($commentLine = getComment($line)) {
				$comment = $comment . "\n" . $commentLine;
				$line = getNextLine($input);
			}
			$column = getColumnName($line);
			if ($column)
				$lastcolumn = $column;
			if (!isset($schema[$tabName]['fields'][$lastcolumn])) {
				$schema[$tabName]['fields'][$lastcolumn] = array('description' => "No description for column $lastcolumn available, please fix");
			}
			if ($comment) {
				$schema[$tabName]['fields'][$lastcolumn]['description'] = $comment;
			}
			if (getColumnType($schema[$tabName], $lastcolumn, $line)) {
				continue;
			}
			if (preg_match('/^\) WITHOUT OIDS;/', $line)) {
				continue;
			}
			if ($uniquekeys = getUnique($line)) {
				$keyname = str_replace(' ', '_', implode('', $uniquekeys));
				$schema[$tabName]['unique keys'][$keyname] = $uniquekeys;
				continue;
			}
			if ($primaryKey = getPrimaryKey($line)) {
				$schema[$tabName]['primary key'] = $primaryKey;
				continue;
			}
			fwrite(STDERR, "Unknown statements within CREATE TABLE: $line\n");
		}
		return true;
	}
	return false;
}

if (!isset($arguments['o'])) {
	print "Using " . $filename . "\n";
}

$input = fopen($filename, 'r');
$schema = array();
$line = getNextLine($input);
while (!($line === false)) {
	$line = trim($line);
	if ($line == "") {

		$line = getNextLine($input);
		continue;
	}
	if (handleCreateTable($schema, $input, $line)) {
		continue;
	}
	if (handleDropStatement($schema, $input, $line)) {
		continue;
	}
	if (handleAlterTable($schema, $input, $line)) {
		continue;
	}
	if (handleCommentOnColumn($schema, $input, $line)) {
		continue;
	}
	if (handleCreateIndex($schema, $input, $line)) {
		continue;
	}
	if (handleComment($schema, $input, $line)) {
		continue;
	}
	fwrite(STDERR, "Could not handle: " . $line . "\n");
	$line = getNextLine($input);
}
if (isset($arguments['o'])) {
	$outputhandle = fopen($arguments['o'], "w");
	if (!$outputhandle) {
		die("Could not open file '" . $arguments['o'] . "' for writing.\n");
	}
	fwrite($outputhandle, "<?php\n/**\n * do not modify this file, it is generated! \n */\n\nfunction " . $modulname . "_schema() {\n\treturn ");
	fwrite($outputhandle, var_export($schema, true) . ";\n");
	fwrite($outputhandle, "}\n?>");
} else {
	print "Schema:\nreturn " . var_export($schema, true) . ";\n";
}
?>
