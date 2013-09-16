<?php
/**
 * do not modify this file, it is generated!
 * To make modifications on the schema alter the file in /doc/module/<modulename> and regenerate it. 
 * See also description in wiki at 
 * http://www.naturwerk.info/wiki/naturvielfalt:entwicklungsumgebung?s[]=ermaster#drupal_schema_api_beschreibungen_generieren 
 */

function vote_get_schema() {
	return array(
			'vote' => array(
					'fields' => array(
							'vote_id' => array(
									'description' => 'ID of this vote entry',
									'type' => 'serial',
									'not null' => 'TRUE'
							),
							'user_id' => array(
									'description' => 'ID of the user that voted',
									'type' => 'int',
									'not null' => 'TRUE'
							),
							'observation_id' => array(
									'description' => 'ID of the observation the vote is made for',
									'type' => 'int',
									'not null' => 'TRUE'
							),
							'organism_id' => array(
									'description' => 'ID of the proposed organism',
									'type' => 'int',
									'not null' => 'TRUE'
							),
							'vote_timestamp' => array(
									'description' => 'Date and time, when the vote was entered',
									'type' => 'datetime',
									'not null' => 'TRUE',
									'pgsql_type' => 'timestamp'
							)
					),
					'primary key' => array(
							0 => 'vote_id'
					)
			),
			'vote_comment' => array(
					'fields' => array(
							'id' => array(
									'description' => 'ID of this entry',
									'type' => 'serial',
									'not null' => 'TRUE'
							),
							'vote_entry' => array(
									'description' => 'ID of the vote',
									'type' => 'int',
									'not null' => 'TRUE'
							),
							'comment_id' => array(
									'description' => 'ID of the comment',
									'type' => 'int',
									'not null' => 'TRUE'
							)
					),
					'primary key' => array(
							0 => 'id'
					)
			)
	);
}
?>
