<?php
/**
 * @file organism.lexika.inc
 * @author Marco Zimmerli, 2011
 * @author Reto Schneider, 2012
 * @copyright 2011-2012 Naturwerk, Brugg
 */

module_load_include('inc', 'organism', 'organism');

/**
 * Queries Wikipedia for organism entry and displays content on local page.
 * @param $organism_id
 * 	String with an integer-Id of requested organism.
 * @return
 * 	Drupal render array
 */
function organism_lexika_show($organism_id) {
	drupal_add_css(
		drupal_get_path('module', 'organism') . '/css/organism.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
	$organism_name_array = organism_get_single_name_of_organism($organism_id);
	$organism_name = $organism_name_array['name'];
	if (preg_match('/([a-zA-Z0-9]+\ [a-zA-Z0-9]+).*/', $organism_name, $matches)) {
		$organism_name = $matches[1];
	}

	drupal_set_title($organism_name);
	$organism_name = str_replace(' ', '_', $organism_name);
	global $user;
	global $language;
	$langcode = isset($user->language) ? $user->language : $language->language;

	$host = $langcode . '.wikipedia.org';
	$url = '/w/api.php?';
	$postdata = 'action=query&prop=revisions&titles=' . $organism_name
			. '&rvprop=content&rvparse&format=json&redirects';

	$fp = pfsockopen($host, 80, &$errno, &$errstr, 60);
	if (!$fp)
		return "$errstr ($errno)<br>\n";
	fputs($fp, "POST $url$postdata HTTP/1.1\n");
	fputs($fp, "Host: $host\n");
	fputs($fp, "User-Agent: Post\n");
	fputs($fp, "Accept: */*\n");
	fputs($fp, "Content-type: application/x-www-form-urlencoded\n");
	fputs($fp, "Content-length: " . strlen($postdata) . "\n\n");
	fputs($fp, "$postdata\n\n");
	$o = "";
	while (!feof($fp))
		$o .= fgets($fp, 1024);
	fclose($fp);

	$pos = strpos($o, '{"query":');
	$o = substr($o, $pos);
	$lexika = drupal_json_decode($o);
	$rev_key = key($lexika['query']['pages']);
	$content = str_replace(
		'/wiki/',
		"http://$langcode.wikipedia.org/wiki/",
		$lexika['query']['pages'][$rev_key]['revisions']['0']['*']);
	$content = str_replace('h2', 'h3', $content);
	$content = str_replace(
		'h2',
		'h3',
		$lexika['query']['pages'][$rev_key]['revisions']['0']['*']);
	$content = str_replace(
		"<a href=\"http://de.wikipedia.org/wiki",
		"<a target=\"blank\" href=\"http://$langcode.wikipedia.org/wiki",
		$content);

	$pattern = array();
	$pattern1 = '/^.*(<table\s+id="toc" class="toc">)/i';
	$pattern2 = '/(<span class="editsection">.+?)+(<\/span>)/i';
	$content = str_replace(array("\n",
			"\r"
	), '', $content);
	$content = preg_replace($pattern1, '$2', $content);
	$content = preg_replace($pattern2, '', $content);
	if (!empty($content)) {
		$content = '<div class="wikientry">' . $content
				. '</div><div  class="wiki_source">'
				. t('The article above is based on')
				. "<a target=\"_blank\" href=\"http://$langcode.wikipedia.org/wiki/"
				. $organism_name . '"> ' . t('this article') . '</a>'
				. t(' of the free encyclopedia')
				. "<a target=\"_blank\" href=\"http://$langcode.wikipedia.org\"> Wikipedia</a> "
				. t('and it is licensed under')
				. ' <a target="_blank" href="http://de.wikipedia.org/wiki/Wikipedia:Lizenzbestimmungen_Commons_Attribution-ShareAlike_3.0_Unported">„Creative Commons Attribution/Share Alike“</a>. '
				. t('Here you find')
				. " <a target=\"_blank\" href=\"http://$langcode.wikipedia.org/w/index.php?title="
				. $organism_name . '&action=history">' . t('versions/authors')
				. '</a>.</div>';
	} else {
		$content = t(
			'Wikipedia does not have any @langcode information about the organism.',
			array('@langcode' => $langcode));
	}

	return array(
			'#markup' => $content
	);
}

?>
