<?php
/**
 * Queries Wikipedia for organism entry and displays content on local page
 */
function organism_lexika_show($organism_id) {
	$organism = db_query('
		SELECT o.organism_type, fo."Gattung" gattung, fo."Art" art, fa.genus, fa.species, fo.name_de floraname, fa.name_de faunaname
		FROM organism o
		LEFT JOIN flora_organism fo ON o.organism_id = fo.id
		LEFT JOIN fauna_organism fa ON o.organism_id = fa.id
		WHERE o.id = :id',
		array(':id' => $organism_id)
	);
	
	$organism = $organism->fetch();
	if($organism->organism_type == '1') {
		$organism_name = $organism->genus.'_'.$organism->species;
		$organism_name_de = $organism->faunaname;
		$title = $organism_name_de.' ['.$organism->genus.' '.$organism->species.']';
	}
	if($organism->organism_type == '2') {
		$organism_name = $organism->gattung.'_'.$organism->art;
		$organism_name_de = $organism->floraname;
		$title = $organism_name_de.' ['.$organism->gattung.' '.$organism->art.']';
	}
	
	drupal_set_title($title);
	$host = 'de.wikipedia.org';
	$url = '/w/api.php?';
	$postdata = 'action=query&prop=revisions&titles='.$organism_name.'&rvprop=content&rvparse&format=json&redirects';
	
	$fp = pfsockopen($host, 80, &$errno, &$errstr, 60);
	if(!$fp) return "$errstr ($errno)<br>\n";
	fputs($fp,"POST $url.$postdata HTTP/1.1\n");
	fputs($fp,"Host: $host\n");
	fputs($fp,"User-Agent: Post\n");
	fputs($fp,"Accept: */*\n");
	fputs($fp,"Content-type: application/x-www-form-urlencoded\n");
	fputs($fp,"Content-length: ".strlen($postdata)."\n\n");
	fputs($fp,"$postdata\n\n");
	$o = "";
	while(!feof($fp))
		$o .= fgets($fp, 1024);
	fclose($fp);
	 
	$pos = strpos($o, '{"query":');
	$o = substr($o, $pos);
	$lexika = drupal_json_decode($o);
	$rev_key = key($lexika['query']['pages']);
	$content = str_replace('/wiki/', 'http://de.wikipedia.org/wiki/', $lexika['query']['pages'][$rev_key]['revisions']['0']['*']); 
	$content = str_replace('h2', 'h3', $content);
	$content = str_replace('<a href="http://de.wikipedia.org/wiki', '<a target="blank" href="http://de.wikipedia.org/wiki', $content);
	
	$pattern = array();
	$pattern1 = '/^.*(<table\s+id="toc" class="toc">)/i';
	$pattern2 = '/(<span class="editsection">.+?)+(<\/span>)/i';
	$content = str_replace(array("\n", "\r"), '', $content);
	$content = preg_replace($pattern1, '$2', $content);
	$content = preg_replace($pattern2, '', $content);
	if(!empty($content)) {
		$content = '<div class="wikientry">'.$content.'</div><div  class="wiki_source">Obenstehender Artikel basiert auf <a target="_blank" href="http://de.wikipedia.org/wiki/'.$organism_name.'">diesem Artikel</a> der freien Enzyklopädie <a target="_blank" href="http://de.wikipedia.org">Wikipedia</a> und steht unter der <a target="_blank" href="http://de.wikipedia.org/wiki/Wikipedia:Lizenzbestimmungen_Commons_Attribution-ShareAlike_3.0_Unported">„Creative Commons Attribution/Share Alike“</a>.Hier finden Sie <a target="_blank" href="http://de.wikipedia.org/w/index.php?title='.$organism_name.'&action=history">Versionen/Autoren</a>.</div>';
	}
	else {
		$content = t('We are sorry, but Wikipedia does not have any german information about this organism.');
	}
	
	return array(
		'#markup' => $content
		);
}

?>