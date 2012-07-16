<?php

/**
 * @file
 * Generic gallery pages.
 */

define('GALLERY_COLUMNS', 5);
define('GALLERY_IMAGE_COUNT', 15);

/**
 * Menu callback; Listing of all images for a given context.
 */
function gallery_list($type, $id, $backlink = true, $title = true, $mediatype) {
	$types = _gallery_types();
	$type_info = $types[$type];
	if ($title && array_key_exists($type, $types)
			&& array_key_exists('image_caption', $types[$type]))
		drupal_set_title(
			call_user_func_array($types[$type]['image_caption'], array($id)),
			PASS_THROUGH);
	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/gallery.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
	$page = array();
	$editable = gallery_edit_access_callback($type, $id);
	//	$editable = false;
	$params = _gallery_list_parameters($type, $id);
	$images = gallery_images(
		$mediatype,
		$type,
		$id,
		$params['crosslinked'],
		true,
		$params['show_all'] ? false : GALLERY_IMAGE_COUNT,
		$params['subtype']);
	$links = _gallery_list_links($params, $images['count'], $mediatype);
	if ($images['count']) {
		$page['anchor'] = array(
				'#markup' => '<a name="' . $params['key'] . '"></a>'
		);
		$page['gallery_list'] = array(
				'#theme' => 'gallery_list',
				'#images' => $images,
				'#rating_types' => gallery_available_rating_types()
		);
	} else if ($editable) {
		$page['gallery_list'] = array(
				'#markup' => '<p>'
						. t(
							'There are no @mediatype available yet.',
							array('@mediatype' => t($mediatype))) . '</p>',
		);
	}
	if ($editable)
		array_push(
			$links,
			array('title' => t(
						'Manage @mediatype',
						array('@mediatype' => t($mediatype))),
					'href' => 'gallery/' . $type . '/' . $id . '/' . $mediatype
							. '/edit' //'href' => 'gallery/'.$type.'/'.$id.'/edit'
			));
	if ($backlink) {
		$back = _gallery_back_link($type, $id);
		if ($back)
			array_push($links, $back);
	}
	if ($links)
		$page['actions'] = array(
				'#theme' => 'links',
				'#links' => $links,
				'#attributes' => array(
						'class' => 'gallery-action-links'
				),
				'#weight' => 9
		);

	return $page;
}

/**
 * Renders the medias of a given object type and id
 *
 * @return
 * @param object $type
 * @param object $id
 * @param object $title[optional]
 * @param object $weight[optional]
 * @param object $collapsed[optional]
 */
function gallery_list_renderer($mediatype, $type, $id, $title = false,
		$weight = 10, $collapsed = false) {
	if (!gallery_access_callback($type))
		return '';
	$gallery = gallery_list($type, $id, false, false, $mediatype);
	if (!$gallery['gallery_list'])
		return '';
	return _gallery_list_fieldset(
		$gallery,
		$title,
		$weight,
		$collapsed && !array_key_exists(md5($type . $id) . '_show_all', $_GET)
				&& !array_key_exists(md5($type . $id) . '_crosslinked', $_GET),
		$mediatype);
}

/**
 * Renders the medias of a given object type and multiple ids
 *
 * @return
 * @param object $type
 * @param object $ids
 * @param object $title[optional]
 * @param object $weight[optional]
 * @param object $collapsed[optional]
 */
function gallery_multiple_list_renderer($type, $ids, $title = false,
		$weight = 10, $collapsed = false, $mediatype) {
	if (!gallery_access_callback($type))
		return '';
	$params = _gallery_list_parameters($type, $id);
	$images = gallery_images(
		$mediatype,
		$type,
		$ids,
		$params['crosslinked'],
		true,
		$params['show_all'] ? false : GALLERY_IMAGE_COUNT,
		$params['subtype']);
	$links = _gallery_list_links($params, $images['count'], $mediatype);
	if ($images['count']) {
		$page = _gallery_list_fieldset(
			array('anchor' => array('#markup' => '<a name="' . $params['key']
									. '"></a>'
					),
					'list' => array('#theme' => 'gallery_list',
							'#images' => $images,
							'#rating_types' => gallery_available_rating_types()
					)
			),
			$title,
			$weight,
			$collapsed
					&& !array_key_exists($params['key'] . '_show_all', $_GET)
					&& !array_key_exists($params['key'] . '_crosslinked', $_GET)
					&& !array_key_exists($params['key'] . '_subtype', $_GET));
		if (isset($info))
			$page['gallery_info'] = $info;
		if ($links)
			$page['actions'] = array(
					'#theme' => 'links',
					'#links' => $links,
					'#attributes' => array(
							'class' => 'gallery-action-links'
					),
					'#weight' => 9
			);
		return $page;
	}
	return '';
}

function _gallery_list_parameters($type, $id) {
	$params = array();
	$types = _gallery_types();
	$key = md5($type . (is_array($id) ? implode('.', $id) : $id));
	if (array_key_exists($key . '_show_all', $_GET))
		$_SESSION[$key . '_show_all'] = $_GET[$key . '_show_all'] ? true : false;
	$params['show_all'] = array_key_exists($key . '_show_all', $_SESSION)
			? $_SESSION[$key . '_show_all'] : false;
	$params['crosslinking'] = array_key_exists('crosslinked', $types[$type]);
	if ($params['crosslinking']
			&& array_key_exists($key . '_crosslinked', $_GET))
		$_SESSION[$key . '_crosslinked'] = $_GET[$key . '_crosslinked'] ? true
				: false;
	$params['crosslinked'] = array_key_exists($key . '_crosslinked', $_SESSION)
			? $_SESSION[$key . '_crosslinked'] : null;
	$params['subtypes'] = _gallery_get_subtypes($type, $params['crosslinked']);
	if ($params['subtypes'] && array_key_exists($key . '_subtype', $_GET))
		$_SESSION[$key . '_subtype'] = $_GET[$key . '_subtype'];
	$params['subtype'] = array_key_exists($key . '_subtype', $_SESSION)
			? $_SESSION[$key . '_subtype'] : '';
	$params['key'] = $key;
	return $params;
}

function _gallery_list_links($params, $count, $mediatype) {
	$url = drupal_get_destination();
	$url = drupal_parse_url($url['destination']);
	$url = $url['path'];
	$links = array();
	if ($count > GALLERY_IMAGE_COUNT) {
		if (!$params['show_all']) {
			$info = array(
					'#markup' => '<p class="info">'
							. t(
								'Showing first !showing @mediatype of !total',
								array('!showing' => GALLERY_IMAGE_COUNT,
										'!total' => $count,
										'@mediatype' => t($mediatype)
								)),
					'#weight' => 10
			);
			array_push(
				$links,
				array('title' => t(
							'Show all @mediatype',
							array('@mediatype' => t($mediatype))),
						'href' => $url,
						'query' => array(
								$params['key'] => '1'
						),
						'fragment' => $params['key']
				));
		} else
			array_push(
				$links,
				array('title' => t(
							'Show top !number',
							array('!number' => GALLERY_IMAGE_COUNT)),
						'href' => $url,
						'query' => array(
								$params['key'] => ''
						),
						'fragment' => $params['key']
				));
	}
	if ($params['crosslinking']) {
		if ($params['crosslinked'])
			array_push(
				$links,
				array('title' => t(
							'Hide linked @mediatype',
							array('@mediatype' => t($mediatype))),
						'href' => $url,
						'query' => array(
								$params['key'] . '_crosslinked' => ''
						),
						'fragment' => $params['key']
				));
		else
			array_push(
				$links,
				array('title' => t(
							'Show linked @mediatype',
							array('@mediatype' => t($mediatype))),
						'href' => $url,
						'query' => array(
								$params['key'] . '_crosslinked' => '1'
						),
						'fragment' => $params['key']
				));
	}
	if ($params['subtypes']) {
		$links[(!$params['subtype'] ? 'selected' : count($links))] = array(
				'title' => t(
					'Show all @mediatype types',
					array('@mediatype' => t($mediatype))),
				'href' => $url,
				'query' => array(
						$params['key'] . '_subtype' => ''
				),
				'fragment' => $params['key']
		);
		foreach ($params['subtypes'] as $value => $label)
			$links[($value == $params['subtype'] ? 'selected' : count($links))] = array(
					'title' => $label,
					'href' => $url,
					'query' => array(
							$params['key'] . '_subtype' => $value
					),
					'fragment' => $params['key']
			);
	}
	return $links;
}

function _gallery_list_fieldset($list, $title = false, $weight = 10,
		$collapsed = false, $mediatype) {

	if ($mediatype == "images")
		$media_title = "Images";
	elseif ($mediatype == "sounds")
		$media_title = "Sounds";
	elseif ($mediatype == "videos")
		$media_title = "Videos";
	elseif ($mediatype == "medias")
		$media_title = "Medias";

	$output = array(
			'#type' => 'fieldset',
			'#title' => $title ? $title : t($media_title),
			'#weight' => $weight,
			'#collapsible' => TRUE,
			'#collapsed' => $collapsed,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible gallery'
									. ($collapsed ? ' collapsed' : '')
					),
			),
	);
	$output['edit'] = $list;
	return $output;
}

/**
 * Renders the highest rated media of the given items
 *
 * @return
 * @param array $items
 *   Array of arrays having the following keys:
 *   - "type": The type of object to display
 *   - "id": The id of the object to display
 *   - "subtype": [Optional] The subtype of the media to display
 *   - "content": [Optional] drupal render array of the content to be displayed for the given item
 *   - "alternatives": [Optional] List of arrays with the following keys defining an alternative way of getting the media
 *     - "type": The type of object to display
 *     - "id": The id of the object to display
 * @param string $imgsize
 *   The size of the images to be displayed
 * @param string $cssClass
 *   Additional css classes to be set on the container
 * @param int $columns
 *   The number of columns to show the items in
 * @param string $msgPrimary
 *   Additional message to be displayed if an media is found by using the primary source
 * @param string $msgAlternative
 *   Additional message to be displayed if an media is found by using the alternative source
 */
function gallery_presentation_renderer($mediatype, $items,
		$imgsize = 'gallery_presentation', $cssClass = '',
		$columns = GALLERY_COLUMNS, $msgPrimary = false,
		$msgAlternative = false) {
	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/gallery.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js',
		array(
				'weight' => 100
		));
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js',
		array(
				'weight' => 110
		));
	$page = array();

	$i = 1;
	$images = array();
	foreach ($items as $weight => $item) {
		$image = gallery_images(
			$mediatype,
			$item['type'],
			$item['id'],
			true,
			false,
			1,
			isset($item['subtype']) ? $item['subtype'] : false);
		$msg = $msgPrimary;
		if (!$image['images'] && isset($item['alternatives'])
				&& $item['alternatives']) {
			$msg = $msgAlternative;
			foreach ($item['alternatives'] as $alternative) {
				$image = gallery_images(
					$mediatype,
					$alternative['type'],
					$alternative['id'],
					true,
					false,
					1);
				if ($image['images'])
					break;
			}
		}
		$image = $image['count'] ? $image['images'][0] : false;
		$cls = _gallery_get_class($i - 1, $columns);
		if (!$image)
			array_push($cls, 'noimage');

		$images['image' . $i]['start'] = array(
				'#markup' => '<li class="' . join(' ', $cls) . '">'
		);
		if ($image) {
			$images['image' . $i]['image'] = array(
					'#markup' => '<a class="image" href="'
							. url(
								'gallery/' . $image['item_type'] . '/'
										. $image['item_id'] . '/thumb/'
										. $image['id'] . '/gallery_large')
							. '">' . '<img src="'
							. url(
								'gallery/' . $image['item_type'] . '/'
										. $image['item_id'] . '/thumb/'
										. $image['id'] . '/' . $imgsize)
							. '" alt="' . $image['title'] . '" />' . '</a>'
			);
		} else {
			$content = t('No media available');
			$ids = $item['id'];
			if (!is_array($ids))
				$ids = array(
						$ids
				);
			foreach ($ids as $id) {
				if (gallery_edit_access_callback($item['type'], $id)) {
					$content .= '<br>'
							. l(
								t('Add one'),
								'gallery/' . $item['type'] . '/' . $id . '/'
										. $mediatype . '/edit');
					break;
				}
			}
			$images['image' . $i]['image'] = array(
					'#markup' => '<span class="image">' . $content . '</span>'
			);
		}
		if ($item['content']) {
			$images['image' . $i]['content_start'] = array(
					'#markup' => '<div class="caption">'
			);
			if ($image) {
				$caption = '';
				if ($image['author'] || $image['location'])
					$caption .= '<span class="author">&copy; '
							. $image['author']
							. ($image['location'] ? ', ' . $image['location']
									: '') . '</span>';
				if ($msg)
					$caption .= '<small class="msg">' . $msg . '</small>';
				if ($caption)
					$images['image' . $i]['caption'] = array(
							'#markup' => '<p class="author-msg">' . $caption
									. '</p>'
					);
			}
			$images['image' . $i]['content'] = $item['content'];
			$images['image' . $i]['content_end'] = array(
					'#markup' => '</div>'
			);
		}
		$images['image' . $i]['end'] = array(
				'#markup' => '</li>'
		);

		$i++;
	}
	if ($images) {
		$page['start'] = array(
				'#markup' => '<ul class="gallery presentation clearfix '
						. $cssClass . '">'
		);
		$page['images'] = $images;
		$page['end'] = array(
				'#markup' => '</ul>'
		);
	}
	return $page;
}

/**
 * Renders a link to view the medias of a given object type and id
 * if the user has view permissions and there are medias available
 *
 * @return
 * @param object $type
 * @param object $id
 * @param object $url[Optional]
 */
function gallery_view_link($type, $id, $url = false, $check = true,
		$count_images = 0, $count_sounds = 0, $count_videos = 0) {
	if (!gallery_access_callback($type))
		return false;
	$params = _gallery_list_parameters($type, $id);
	if ($check) {
		$count_images = gallery_images_count($type, $id);
		$count_sounds = gallery_sounds_count($type, $id);
		$count_videos = gallery_videos_count($type, $id);

		if (!$count_images && !$count_sounds && !$count_videos) {
			return false;
		}

	}
	$gallery_img_localize = theme(
		'image',
		array('path' => drupal_get_path('module', 'gallery')
						. '/images/image.png',
				'alt' => t('View images'),
				'title' => t('View images'),
		));
	$gallery_sound_localize = theme(
		'image',
		array('path' => drupal_get_path('module', 'gallery')
						. '/images/music_super_mini_icon.png',
				'alt' => t('View sounds'),
				'title' => t('View sounds'),
		));
	$gallery_video_localize = theme(
		'image',
		array('path' => drupal_get_path('module', 'gallery')
						. '/images/video_super_mini_icon.png',
				'alt' => t('View videos'),
				'title' => t('View videos'),
		));
	$return_link = "";
	if ($count_images > 0) {
		$return_link .= ''
				. l(
					$gallery_img_localize,
					$url ? $url : 'gallery/' . $type . '/' . $id . '/images',
					array('html' => true));
	}
	if ($count_sounds > 0) {
		$return_link .= ''
				. l(
					$gallery_sound_localize,
					$url ? $url : 'gallery/' . $type . '/' . $id . '/sounds',
					array('html' => true));
	}
	if ($count_videos > 0) {
		$return_link .= ''
				. l(
					$gallery_video_localize,
					$url ? $url : 'gallery/' . $type . '/' . $id . '/videos',
					array('html' => true));
	}

	return $return_link;
	//return (l($gallery_img_localize, $url ? $url : 'gallery/'.$type.'/'.$id.'/images', array('html'=>true)).''.l($gallery_sound_localize, $url ? $url : 'gallery/'.$type.'/'.$id.'/videos', array('html'=>true)).''.l($gallery_video_localize, $url ? $url : 'gallery/'.$type.'/'.$id.'/images', array('html'=>true)));
}

/**
 * Menu callback; Serving a gallery media
 */
function gallery_image($type, $id, $image) {
	$uri = explode('://', $image['uri']);
	return image_style_deliver(
		image_style_load('gallery_large'),
		$uri[0],
		$uri[1]);
}

/**
 * Menu callback; Serving a gallery media thumb
 */
function gallery_thumb($type, $id, $image, $size) {
	$uri = explode('://', $image['uri']);
	return image_style_deliver(image_style_load($size), $uri[0], $uri[1]);
}

/**
 * Menu callback; Gallery edit view
 * @param $mediatype
 * 	 String which represents the media type ('images', 'sounds', 'videos', 'medias'
 * @param $type
 *   The type of the object the medias are to be edited.
 * @param $id
 *   The id of the object the medias are to be edited
 */
function gallery_images_edit($type, $id, $mediatype) {

	$back = _gallery_back_link($type, $id);
	if ($back)
		$page['back'] = array(
				'#theme' => 'links',
				'#links' => array(
						$back
				),
				'#attributes' => array(
						'class' => 'action-links'
				)
		);

	$page['upload'] = array(
			'#type' => 'fieldset',
			'#title' => t(
				'Upload new @mediatype',
				array('@mediatype' => t($mediatype))),
			'#weight' => 1,
			'#collapsible' => true,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
			),
	);
	$page['upload']['edit'] = drupal_get_form(
		'gallery_image_form',
		$type,
		$id,
		$mediatype,
		array(),
		false,
		1);

	$page['edit'] = array(
			'#type' => 'fieldset',
			'#title' => t(
				'Edit existing @mediatype',
				array('@mediatype' => t($mediatype))),
			'#weight' => 2,
			'#collapsible' => true,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
									. (isset($_GET['saved']) ? '' : ' collapsed')
					),
			),
	);
	$page['edit']['edit'] = drupal_get_form(
		'gallery_images_form',
		$mediatype,
		$type,
		$id,
		false,
		'gallery/' . $type . '/' . $id . '/' . $mediatype . '');

	$title = t('Manage @mediatype', array('@mediatype' => t($mediatype)));
	drupal_set_title($title, PASS_THROUGH);

	$info = _gallery_get_title_description($type, $id, $mediatype);
	if ($info['title'])
		drupal_set_title($info['title'], PASS_THROUGH);
	if ($info['description'])
		$page['description'] = array(
				'#markup' => '<p>' . $info['description'] . '</p>',
				'#weight' => -1
		);
	return $page;
}

function _gallery_get_title_description($type, $id, $mediatype) {
	$types = _gallery_types();
	$type = $types[$type];
	$info = array(
			'title' => false,
			'description' => false
	);
	if ($type['manage_title'] && function_exists($type['manage_title']))
		$info['title'] = call_user_func_array(
			$type['manage_title'],
			array($id,
					$mediatype
			));
	if ($type['manage_description']
			&& function_exists($type['manage_description']))
		$info['description'] = call_user_func_array(
			$type['manage_description'],
			array($id,
					$mediatype
			));
	return $info;
}

/**
 * Form builder; Edit gallery medias
 *
 * @param $form_state
 *   An associative array containing the current state of the form.
 * @param $mediatype
 * 	 String which represents the media type ('images', 'sounds', 'videos', 'medias'
 * @param $image
 *   Array with all data in an image
 * @param $type
 *   The type of the object the medias are to be edited.
 * @param $id
 *   The id of the object the medias are to be edited
 * @ingroup forms
 * @see gallery_medias_form_submit()
 */
function gallery_images_form($form, &$form_state, $mediatype, $type = false,
		$id = false, $images = false, $camefrom = false) {
	assert(is_string($type));
	assert($id == false || $id > 0);
	assert(is_string($mediatype));

	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js');
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js');
	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/divbox.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
	drupal_add_js(drupal_get_path('module', 'gallery') . '/js/divbox.js');
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/gallery.divbox.js');
	$form_state['gallery_type'] = $type;
	$form_state['gallery_id'] = $id;
	$types = _gallery_types();

	$subtypes = $type ? _gallery_get_subtypes($type, false) : false;

	$header = array(
			'',
			t('Filename'),
			t('Title')
	);
	if ($subtypes)
		$header[] = t('Type');
	$header[] = t('Author');
	$header[] = t('Location');
	$header[] = t('Caption');
	$header[] = t('Operations');
	$header[] = t('Delete');

	if (!$images)
		$images = gallery_images($mediatype, $type, $id, 'edit');

	$rows = array();
	foreach ($images[$mediatype] as $image) {

		if ($mediatype == "images"
				|| ($mediatype == "medias"
						&& ($image['filemime'] == "image/png"
								|| $image['filemime'] == "image/jpg"
								|| $image['filemime'] == "image/jpeg"
								|| $image['filemime'] == "image/gif"))) {

			$url_href = url(
				'gallery/' . $image['item_type'] . '/' . $image['item_id']
						. '/thumb/' . $image['id'] . '/gallery_large');
			$url_image = url(
				'gallery/' . $image['item_type'] . '/' . $image['item_id']
						. '/thumb/' . $image['id'] . '/gallery_mini');
			$extras = 'rel="lightbox"';
		} elseif ($mediatype == "sounds"
				|| ($mediatype == "medias"
						&& ($image['filemime'] == "audio/mpeg"))) {

			$url_href = file_uri_target($image['uri']);
			$url_href = url('sites/default/files/' . $url_href);
			$url_image = url(
				drupal_get_path('module', 'gallery')
						. '/images/music_icon_mini.png');
			$extras = 'class="divbox_sound"';
		} elseif ($mediatype == "videos"
				|| ($mediatype == "medias"
						&& ($image['filemime'] == "video/mp4"))) {

			$url_href = file_uri_target($image['uri']);
			$url_href = url('sites/default/files/' . $url_href);
			$url_image = url(
				drupal_get_path('module', 'gallery')
						. '/images/video_icon_mini.png');
			$extras = 'class="divbox_video"';
		}

		$caption = '';
		if (array_key_exists($image['item_type'], $types)
				&& array_key_exists(
					'image_caption',
					$types[$image['item_type']]))
			$caption = call_user_func_array(
				$types[$image['item_type']]['image_caption'],
				array($image['item_id']));
		$editable = gallery_edit_access_callback(
			$image['item_type'],
			$image['item_id'],
			$image);
		$row = array();
		$row[] = '<a ' . $extras . ' href="' . $url_href . '"><img src="'
				. $url_image . '" alt="' . $image['title'] . '" /></a>';
		$row[] = $image['filename'];
		$row[] = $image['title'];
		if ($subtypes) {
			$stypes = array();
			foreach ($image['subtypes'] as $subtype)
				$stypes[] = $subtypes[$subtype];
			$row[] = implode(', ', $stypes);
		}
		$row[] = $image['author'];
		$row[] = $image['location'];
		$row[] = $caption;
		$row[] = $editable
				? l(
					t('edit'),
					'gallery/' . $image['item_type'] . '/' . $image['item_id']
							. '/' . $mediatype . '/edit/' . $image['id'],
					$camefrom
							? array(
									'query' => array(
											'camefrom' => $camefrom
									)
							) : array()) : '&nbsp;';
		$row[] = $editable
				? '<input type="checkbox" name="delete[' . $image['id']
						. ']" value="1" />' : '&nbsp;';
		$rows[] = $row;
	}

	if (!$rows)
		$form['empty'] = array(
				'#markup' => '<p>'
						. t(
							'There are no @mediatype available yet.',
							array('@mediatype' => t($mediatype))) . '</p>',
		);
	else
		$form['overview'] = array(
				'#theme' => 'table',
				'#header' => $header,
				'#rows' => $rows,
		);

	if (array_key_exists('overview', $form)) {
		// Show the Override or Submit button for this style.
		$form['actions'] = array(
				'#type' => 'actions'
		);
		$form['actions']['delete_submit'] = array(
				'#type' => 'submit',
				'#value' => t(
					'Delete selected @mediatype',
					array('@mediatype' => t($mediatype))),
				'#submit' => array(
						'gallery_images_form_delete_submit'
				),
		);
	}

	return $form;
}

/**
 * Form builder; Edit media
 *
 * @param $form_state
 *   An associative array containing the current state of the form.
 * @param $type
 *   String: The type of the object the medias are to be edited.
 * @param $id
 *   The id of the object the medias are to be edited
 * @param $media
 *   An media array.
 * @param $mediatype
 * 	 String which represents the media type ('images', 'sounds', 'videos', 'medias'
 * @param $image
 *   Array with all data in an image
 * @param $i
 *   Integer to mark the fields with... what!?
 * @ingroup forms
 * @see gallery_media_form_submit()
 */
function gallery_image_form($form, &$form_state, $type, $id, $mediatype, $image,
		$new_open_identification = false, $i = 1) {
	global $user;
	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/gallery.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js');
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js');
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/gallery.upload.js');

	if ($mediatype == "images") {
		$label = 'image';
		$label_caps = 'Image';
		$validate_function = '_gallery_images_form_upload_validate';
	} elseif ($mediatype == "sounds") {
		$label = 'sound';
		$label_caps = 'Sound';
		$validate_function = '_gallery_sounds_form_upload_validate';
	} elseif ($mediatype == "videos") {
		$label = 'video';
		$label_caps = 'Video';
		$validate_function = '_gallery_videos_form_upload_validate';
	} elseif ($mediatype == "medias") {
		$label = 'media';
		$label_caps = 'Medias';
		$validate_function = '_gallery_medias_form_upload_validate';
	}
	if (isset($image['id'])) {
		$title = t(
			'Edit %name',
			array('%name' => ($image['title'] ? $image['title']
							: $image['filename'])
			));
		$form_state['gallery_image_id'] = $image['id'];
	} else
		$title = t('Add new ' . $label . '');
	drupal_set_title($title, PASS_THROUGH);

	$types = _gallery_types();
	$item_provider = array_key_exists('item_provider', $types[$type])
			? $types[$type]['item_provider'] : false;
	$form_state['item_provider'] = $item_provider;
	$item_provider_autocomplete = array_key_exists(
		'item_provider_autocomplete',
		$types[$type]) ? $types[$type]['item_provider_autocomplete'] : false;
	if ($item_provider_autocomplete) {
		drupal_add_library('system', 'ui.autocomplete');
	} else if ($item_provider) {
		$items = call_user_func_array($item_provider, array($id));
	}
	$subtypes = _gallery_get_subtypes($type, false);

	if (isset($_REQUEST['camefrom'])) {
		$_SESSION[md5('camefrom_' . $type . '_' . $id)] = check_plain($_REQUEST['camefrom']);
	}

	$form_state['gallery_type'] = $type;
	$form_state['gallery_id'] = $id;
	$form_state['gallery_image'] = $image;
	$form_state['mediatype'] = $mediatype;
	$url = drupal_get_destination();
	$url = drupal_parse_url($url['destination']);
	$form_state['camefrom'] = $url['path'];
	$form['#tree'] = TRUE;

	if (isset($image['file_managed_fid'])) {
		$form['thumb'] = array(
				'#markup' => '<a class="user-picture" rel="lightbox" href="'
						. url(
							'gallery/' . $type . '/' . $id . '/thumb/'
									. $image['id'] . '/gallery_large')
						. '"><img src="'
						. url(
							'gallery/' . $type . '/' . $id . '/thumb/'
									. $image['id'] . '/gallery_mini')
						. '" alt="' . $image['title'] . '" /></a>'
		);

	}
	if (!$new_open_identification)
		$form_state['mode'] = 'edit';

	if (!isset($image['item_type']))
		$image['item_type'] = $type;
	if (!isset($image['item_id']))
		$image['item_id'] = $id;

	$default_location = _gallery_get_default_location($type, $id);

	$num = 1;
	while (array_key_exists('images_' . ($i + $num), $form_state['input']))
		$num++;
	for ($n = $i; $n < $i + $num; $n++) {
		$prefix = 'images_' . $n;

		$form[$prefix . '_file'] = array(
				'#type' => 'file',
				'#title' => t($label_caps),
				'#size' => 48,
				'#element_validate' => array(
						$validate_function
				),
				'#file_required' => false,//!$image['id'],
		);

		$form[$prefix] = array();
		if ($subtypes)
			$form[$prefix]['subtypes'] = array(
					'#type' => 'checkboxes',
					'#title' => t('Type'),
					'#options' => $subtypes,
					'#default_value' => $image['subtypes'],
					'#required' => false,
			);
		if ($item_provider) {
			if ($item_provider_autocomplete) {
				$search = call_user_func_array(
					$item_provider,
					array($image['item_type'],
							$image['item_id']
					));
				$form[$prefix]['item'] = array(
						'#type' => 'hidden',
						'#default_value' => $search
								? $image['item_type'] . ':' . $image['item_id']
								: '',
						'#attributes' => array(
								'class' => array(
										'item'
								)
						)
				);
				$form[$prefix]['item_search'] = array(
						'#type' => 'textfield',
						'#title' => t('Related item'),
						'#default_value' => $search,
						'#required' => true,
						'#attributes' => array(
								'class' => array(
										'search'
								),
								'rel' => url(
									'gallery/' . $item_provider_autocomplete
											. '/' . $type . '/' . $id)
						)
				);
			} else {
				$form[$prefix]['item'] = array(
						'#type' => 'select',
						'#title' => t('Related item'),
						'#options' => $items,
						'#default_value' => $image['item_type'] . ':'
								. $image['item_id'],
						'#required' => true,
						'#attributes' => array(
								'class' => array(
										'item'
								)
						)
				);
			}
		}
		if (isset($image['id'])) {
			$collapsed = false;
		} else if ($type == "open_identification") {
			$collapsed = true;
		} else {
			$collapsed = true;
		}

		$form[$prefix]['metadata'] = array(
				'#type' => 'fieldset',
				'#title' => t('Metadata'),
				'#weight' => 1,
				'#collapsible' => true,
				'#collapsed' => $collapsed,
				'#attached' => array(
						'js' => array(
								'misc/form.js',
								'misc/collapse.js',
						),
				),
				'#attributes' => array(
						'class' => array(
								'metadata collapsible'
						),
				),
		);
		$form[$prefix]['metadata']['title'] = array(
				'#type' => 'textfield',
				'#title' => t('Title'),
				'#size' => 100,
				'#maxlength' => 128,
				'#default_value' => isset($image['title']) ? $image['title']
						: '',
				'#description' => t('The title of the ' . $label . '.'),
				'#required' => FALSE,
		);
		$form[$prefix]['metadata']['description'] = array(
				'#type' => 'textarea',
				'#title' => t('Description'),
				'#default_value' => isset($image['description'])
						? $image['description'] : '',
				'#description' => t('The description of the ' . $label . '.'),
				'#required' => FALSE,
		);
		$form[$prefix]['metadata']['location'] = array(
				'#type' => 'textfield',
				'#title' => t('Location'),
				'#size' => 100,
				'#maxlength' => 255,
				'#default_value' => isset($image['location'])
						? $image['location'] : $default_location,
				'#description' => t(
					'The location the ' . $label . ' was taken at.'),
				'#required' => FALSE,
		);
		$form[$prefix]['metadata']['author'] = array(
				'#type' => 'textfield',
				'#title' => t('Author'),
				'#size' => 100,
				'#maxlength' => 255,
				'#default_value' => isset($image['author']) ? $image['author']
						: $user->name,
				'#description' => t('The author of the ' . $label . '.'),
				'#required' => FALSE,
		);

		// Openidentifiaction stuff, irrelevant for everytthing else
		if ($new_open_identification) {
			$form['description'] = array(
					'#type' => 'textarea',
					'#title' => t('Comment'),
					'#default_value' => t('no comment'),
					'#description' => t('Comment to the open identification'),
					'#required' => false,
					'#weight' => 1,
			);
			$form['type'] = array(
					'#name' => 'type',
					'#type' => 'select',
					'#title' => t('Type'),
					'#options' => array(
							'habitat' => t('habitat'),
							'organism' => t('Species')
					),
					'#weight' => 2,
					'#required' => false, /*'#ajax' => array(
										  'callback' => 'open_identification_new_add_organismgroup',
										  'wrapper' => 'organismgroup',
										  )*/
			);
			$result = db_select('inventory_type', 'it')->fields(
					'it',
					array('id',
							'name'
					))
				->orderBy('it.id', 'DESC')
				->execute();

			$options = array(
					'' => t('- Please choose -')
			);
			$options['unknown'] = t('unknown');
			foreach ($result as $record)
				$options[$record->id] = $record->name;
			$form['organismgroupid'] = array(
					'#name' => 'organismgroupid',
					'#type' => 'select',
					'#title' => t('Organismgroup'),
					'#options' => $options,
					'#weight' => 3,
					'#required' => false,
			);
		}
		// End of: Openidentifiaction stuff, irrelevant for everytthing else

		if (isset($form_state['input'][$prefix]['item'])) {
			$item = explode(':', $form_state['input'][$prefix]['item']);
			$categories = _gallery_image_form_categories(
				$mediatype,
				$item[0],
				$item[1],
				$image);
		} else
			$categories = _gallery_image_form_categories(
				$mediatype,
				$image['item_type'],
				$image['item_id'],
				$image);
		$form[$prefix]['categories'] = array(
				'#type' => 'fieldset',
				'#title' => t('Categories'),
				'#weight' => 2,
				'#collapsible' => true,
				'#collapsed' => isset($image['id']) ? false : true,
				'#attached' => array(
						'js' => array(
								'misc/form.js',
								'misc/collapse.js',
						),
				),
				'#attributes' => array(
						'class' => array(
								'categories collapsible'
						),
				),
		);
		if ($categories)
			$form[$prefix]['categories'] = array_merge(
				$form[$prefix]['categories'],
				$categories);
		else
			$form[$prefix]['categories']['#attributes']['style'] = 'display:none';
	}

	// Show the Override or Submit button for this style.
	$form['actions'] = array(
			'#type' => 'actions'
	);
	$form['actions']['upload_submit'] = array(
			'#type' => 'submit',
			'#value' => isset($image['id']) ? t('Change ' . $label . '')
					: t('Upload ' . $label . ''),
			'#submit' => array(
					'gallery_images_form_upload_submit'
			),
			'#attributes' => isset($image['id']) ? array()
					: array(
							'data-add' => t('Add another ' . $label . ''),
							'data-remove' => t('Remove ' . $label . '')
					)
	);

	return $form;
}

function gallery_image_form_categories($mediatype, $type, $id) {
	$prefix = filter_xss($_REQUEST['prefix']);
	$type = filter_xss($_REQUEST['item_type']);
	$id = filter_xss($_REQUEST['item_id']);
	$data = array(
			'categories' => array(),
			'item_type' => $type,
			'item_id' => $id,
	);
	if (is_array($_REQUEST[$prefix]['categories']))
		foreach ($_REQUEST[$prefix]['categories'] as $cat) {
			if (is_array($cat)) {
				foreach ($cat as $value) {
					$value = filter_xss($value);
					if ($value)
						$data['categories'][substr($value, 4)] = substr(
							$value,
							4);
				}
			} else {
				$cat = filter_xss($cat);
				$data['categories'][substr($cat, 4)] = substr($cat, 4);
			}
		}
	$base_form = drupal_get_form(
		'gallery_image_form',
		$type,
		$id,
		$mediatype,
		$data,
		false,
		intval(substr($prefix, 7)));
	$form = drupal_render(
		drupal_get_form(
			'gallery_image_form_categories_form',
			$mediatype,
			$type,
			$id,
			$data,
			$prefix));
	$form = preg_replace(
		'/<form[^>]*>\s*<div>(.*)(<input[^>]*type="hidden"[^>]*>\s*){3}<\/div>\s*<\/form>/',
		'$1',
		str_replace("\n", '', $form));
	return drupal_json_output(
		array('request' => $_REQUEST[$prefix]['categories'],
				'values' => $data['categories'],
				'form_build_id' => $base_form['form_build_id']['#value'],
				'form_token' => $base_form['form_token']['#value'],
				'categories' => $form
		));
}

function gallery_image_form_categories_form($form, &$form_state, $mediatype,
		$type, $id, $image, $prefix) {
	$form['#tree'] = true;
	$form[$prefix]['categories'] = _gallery_image_form_categories(
		$mediatype,
		$type,
		$id,
		$image);
	return $form;
}

function _gallery_image_form_categories($mediatype, $type, $id, $image) {
	$available = gallery_available_categories($type, $id);
	$categories = array();
	foreach ($available as $category) {
		if (!$category['options']) // no options available for this category
			continue;
		$options = array();
		foreach ($category['options'] as $option)
			$options['opt_' . $option['id']] = $option['name'];
		$categories['cat_' . $category['id']] = array(
				'#type' => $category['multiple'] ? 'checkboxes' : 'select',
				'#title' => $category['name'],
				'#options' => $options,
				'#description' => $category['description'],
				'#category' => $category,
		);
		if ($category['required']) {
			$categories['cat_' . $category['id']]['#needs_validation'] = true;
			$categories['cat_' . $category['id']]['#title'] .= ' '
					. theme(
						'form_required_marker',
						array('element' => $categories['cat_' . $category['id']]));
			$categories['cat_' . $category['id']]['#element_validate'] = array(
					'_gallery_images_form_upload_category_validate'
			);
		}
		if ($category['multiple'])
			$categories['cat_' . $category['id']]['#default_value'] = array();
		foreach ($options as $value => $option) {
			if (isset($image['categories']) && is_array($image['categories'])
					&& array_key_exists(substr($value, 4), $image['categories']))
				if ($category['multiple'])
					array_push(
						$categories['cat_' . $category['id']]['#default_value'],
						$value);
				else {
					$categories['cat_' . $category['id']]['#default_value'] = $value;
					break;
				}
		}
	}
	return $categories;
}

/**
 * Validator for gallery image files
 */
function _gallery_images_form_upload_validate($element, &$form_state, $form) {
	$name = substr($element['#name'], 6, strlen($element['#name']) - 7);

	$fileName = $_FILES['files']['name'][$name];
	$fileMimeType = $_FILES['files']['type'][$name];
	$fileSize = $_FILES['files']['size'][$name];
	$mediaInfo = gallery_get_MediaInfo_by_filename($_FILES['files']['name'][$name]);
	
	// validate file extension
	if (!gallery_form_upload_validate_file_extension($element, GALLERY_MEDIATYPE_IMAGE, $fileName, $fileMimeType)) {
		return;
	}
	
	// validate file size
	if (!gallery_form_upload_validate_file_size($element, $fileName, $fileSize, $mediaInfo)) {
		return;
	}
}
/**
 * Validator for gallery sound files
 */
function _gallery_sounds_form_upload_validate($element, &$form_state, $form) {
	$name = substr($element['#name'], 6, strlen($element['#name']) - 7);

	$fileName = $_FILES['files']['name'][$name];
	$fileMimeType = $_FILES['files']['type'][$name];
	$fileSize = $_FILES['files']['size'][$name];
	$mediaInfo = gallery_get_MediaInfo_by_filename($_FILES['files']['name'][$name]);
	
	// validate file extension
	if (!gallery_form_upload_validate_file_extension($element, GALLERY_MEDIATYPE_SOUND, $fileName, $fileMimeType)) {
		return;
	}
	
	// validate file size
	if (!gallery_form_upload_validate_file_size($element, $fileName, $fileSize, $mediaInfo)) {
		return;
	}
}
/**
 * Validator for gallery video files
 */
function _gallery_videos_form_upload_validate($element, &$form_state, $form) {
$name = substr($element['#name'], 6, strlen($element['#name']) - 7);

	$fileName = $_FILES['files']['name'][$name];
	$fileMimeType = $_FILES['files']['type'][$name];
	$fileSize = $_FILES['files']['size'][$name];
	$mediaInfo = gallery_get_MediaInfo_by_filename($_FILES['files']['name'][$name]);
	
	// validate file extension
	if (!gallery_form_upload_validate_file_extension($element, GALLERY_MEDIATYPE_VIDEO, $fileName, $fileMimeType)) {
		return;
	}
	
	// validate file size
	if (!gallery_form_upload_validate_file_size($element, $fileName, $fileSize, $mediaInfo)) {
		return;
	}
}
function _gallery_medias_form_upload_validate($element, &$form_state, $form) {
	$name = substr($element['#name'], 6, strlen($element['#name']) - 7);

	$fileName = $_FILES['files']['name'][$name];
	$fileMimeType = $_FILES['files']['type'][$name];
	$fileSize = $_FILES['files']['size'][$name];
	$mediaInfo = gallery_get_MediaInfo_by_filename($_FILES['files']['name'][$name]);
	
	// validate file extension
	if (!gallery_form_upload_validate_file_extension($element, GALLERY_MEDIATYPE_ALL, $fileName, $fileMimeType)) {
		return;
	}

	// validate file size
	if (!gallery_form_upload_validate_file_size($element, $fileName, $fileSize, $mediaInfo)) {
		return;
	}
}

function _gallery_images_form_upload_category_validate($element, &$form_state,
		$form) {
	$value = $element['#value'];
	$image = $form_state['gallery_image'];
	$name = substr($element['#name'], 0, strpos($element['#name'], '['))
			. '_file';
	if (!$image['id'] && !array_key_exists($name, $_FILES['files']['name'])
			|| !$_FILES['files']['name'][$name])
		return;
	if ($element['#multiple'] && is_array($value))
		$value = array_filter($value);
	if (!$value)
		return form_error(
			$element,
			t(
				'!name field is required.',
				array('!name' => $element['#category']['name'])));
	return;
}

function gallery_form_upload_validate_file_extension($element, $mediaType, $fileName, $fileMimeType) {
	$mediaInfo = gallery_get_MediaInfo_by_filename($fileName);
	if ($mediaInfo == FALSE || ($mediaType != GALLERY_MEDIATYPE_ALL && $mediaType != $mediaInfo->mediaType)) {
		$allowedTypes = implode(', ', _gallery_getMediaInfo($mediaType)->fileExtensions);
		form_error(
			$element,
			t('Files of type @type are not allowed. Try to upload one of these: @allowedTypes.',
				array('@type' => $fileMimeType,
				'@allowedTypes' => $allowedTypes,
				)
			)
		);
		return FALSE;
	}
	return TRUE;
}

function gallery_form_upload_validate_file_size($element, $fileName, $fileSize, $mediaInfo) {
	$max_file_size = $mediaInfo->maxFileSize;
	$max_file_size = $max_file_size * 1024; // make bytes
	if ($fileSize > $max_file_size) {
		form_error(
			$element,
			t('The file in @name is exceeding the maximum allowed size of @size.',
				array('@name' => $fileName,
					'@size' => format_size(parse_size($max_file_size))
				)
			)
		);
		return FALSE;
	}
	return TRUE;
}

/**
 * Submit handler for saving multiple gallery medias.
 */
function gallery_images_form_upload_submit($form, &$form_state) {
	global $GALLERY_MAX_SIZE;
	$type = $form_state['gallery_type'];
	$id = $form_state['gallery_id'];
	$orig_image = $form_state['gallery_image'];
	$item_provider = $form_state['item_provider'];
	$mediatype = $form_state['mediatype'];
	$mode = $form_state['mode'];

	if ($mediatype == "images") {
		$extensions = 'jpg jpeg gif png';
		$file_size = variable_get('gallery_image_size', 2048);
	} elseif ($mediatype == "sounds") {
		$extensions = 'mp3';
		$file_size = variable_get('gallery_sound_size', 2048);
	} elseif ($mediatype == "videos") {
		$extensions = 'mp4';
		$file_size = variable_get('gallery_video_size', 8192);
	}
	$file_size = $file_size * 1024;
	$validators = array(
			'file_validate_extensions' => array(
					$extensions
			),
			'file_validate_size' => array(
					parse_size($file_size),
					0
			),
	);

	$destination = array();
	$categories = array();

	$saved = false;

	// Openidentifiaction stuff, irrelevant for everytthing else
	if ($type == "open_identification" && ($mode != 'edit')) {
		$open_identification_description = $form_state['values']['description'];
		$open_identification_type = $form_state['values']['type'];
		$open_identification_organismgroup = $form_state['values']['organismgroup'];
		$open_identification_organismgroupid = $form_state['values']['organismgroupid'];

		$id = open_identification_save(
			$open_identification_description,
			$open_identification_type,
			$open_identification_organismgroup,
			$open_identification_organismgroupid);
	}
	// End of: Openidentifiaction stuff, irrelevant for everytthing else

	foreach ($form_state['values'] as $prefix => $values) {
		if (!isset($values['metadata']) || !is_array($values['metadata'])) {
			continue;
		}

		$name = $prefix . '_file';
		if ((!isset($orig_image['id']) || !$orig_image['id'])
				&& (!array_key_exists($name, $_FILES['files']['name'])
						|| !$_FILES['files']['name'][$name])) {}

		if ($item_provider) {
			assert(isset($values['item']));
			$item = explode(':', $values['item']);
		}
		$image = array(
				'item_type' => $item_provider ? $item[0] : $type,
				'item_id' => $item_provider ? $item[1] : $id,
				'title' => $values['metadata']['title'],
				'description' => $values['metadata']['description'],
				'author' => $values['metadata']['author'],
				'location' => $values['metadata']['location'],
				'categories' => array(),
				'subtypes' => isset($values['subtypes']) ? $values['subtypes']
						: array()
		);
		$key = $image['item_type'] . ':' . $image['item_id'];
		if (!array_key_exists($key, $destination)) {
			$destination[$key] = 'public://naturvielfalt/gallery/'
					. $image['item_type'] . '/' . $image['item_id'];
			file_prepare_directory($destination[$key], FILE_CREATE_DIRECTORY);
		}
		if (!array_key_exists($key, $categories))
			$categories[$key] = gallery_available_categories(
				$image['item_type'],
				$image['item_id']);

		if (isset($orig_image['id']) && $orig_image['id']) {
			$image['id'] = $orig_image['id'];
		}

		foreach ($categories[$key] as $category) {
			if (!$category['options']) // no options available for this category
				continue;
			if (!$values['categories']['cat_' . $category['id']])
				continue;
			$options = $values['categories']['cat_' . $category['id']];
			if (!$category['multiple'])
				$options = array(
						$options
				);
			foreach ($options as $value) {
				if (!$value)
					continue;
				$cat = array(
						'gallery_category_option_id' => substr($value, 4),
				);
				if (is_array($orig_image['categories'])
						&& array_key_exists($value, $orig_image['categories']))
					$cat['value_id'] = $orig_image['categories'][substr(
						$value,
						4)]['value_id'];
				array_push($image['categories'], $cat);
			}
		}
		$file = file_save_upload($name, $validators, $destination[$key]);
		if ($file) {
			$file->status |= FILE_STATUS_PERMANENT;
			$file = file_save($file);
			$image['file_managed_fid'] = $file->fid;
		} else if (!$orig_image['id'])
			return drupal_set_message(
				t(
					'Image %file could not be saved!',
					array('%file' => $file->filename)),
				'error');
		$image = gallery_image_save($image);
		drupal_set_message(
			t('Image %file saved', array('%file' => $file->filename)));

		if ($type == "open_identification") {
			open_identification_link_media_with_open_identification($id, $image);
		}
		$saved = true;
		$orig_image = array();
	}

	if ($type == "open_identification") {
		$camefrom_md5_key = md5('camefrom_' . $type . '_' . 'new');
		$camefrom = 'open_identification';
		if ($_SESSION[$camefrom_md5_key]) {
			$camefrom = $_SESSION[$camefrom_md5_key];
			unset($_SESSION[$camefrom_md5_key]);
		}
	} else {
		$camefrom_md5_key = md5('camefrom_' . $type . '_' . $id);
		$camefrom = 'gallery/' . $type . '/' . $id . '/' . $mediatype . '/edit';
		if (isset($_SESSION[$camefrom_md5_key]) && $_SESSION[$camefrom_md5_key]) {
			$camefrom = $_SESSION[$camefrom_md5_key];
			unset($_SESSION[$camefrom_md5_key]);
		}
	}
	drupal_goto(
		$camefrom,
		($saved ? array('query' => array('saved' => '1')) : array()));
}

/**
 * Submit handler for deleting multiple gallery medias.
 */
function gallery_images_form_delete_submit($form, &$form_state) {
	$type = $form_state['gallery_type'];
	$id = $form_state['gallery_id'];

	$delete = $form_state['input']['delete'];

	if (is_array($delete)) {
		foreach ($delete as $iid => $del) {
			if (!$del)
				continue;
			$image = gallery_image_load($iid);
			if (!gallery_edit_access_callback(
				$image['item_type'],
				$image['item_id'],
				$image)) {
				drupal_set_message(
					t(
						'You do not have the permission to delete the image %file',
						array('%file' => $image['filename']),
						'error'));
				continue;
			}
			if (function_exists('open_identification_gallery_delete'))
				open_identification_gallery_delete($image);

			gallery_image_delete($image);
			drupal_set_message(
				t('File %file deleted', array('%file' => $image['filename'])));
		}
	}
	$url = drupal_get_destination();
	$url = drupal_parse_url($url['destination']);
	$url = $url['path'];
	drupal_goto($url, array('query' => array('saved' => '1')));
}

/**
 * Menu callback; Show a gallery media
 */
function gallery_image_show($image) {
	gallery_image_visibility_save($image, '1');
	if ($_REQUEST['ajax'])
		return drupal_json_output(
			array('label' => t('Hide'),
					'action' => url(
						'gallery/' . $image['item_type'] . '/'
								. $image['item_id'] . '/hide/' . $image['id'])
			));
	$camefrom = filter_xss($_REQUEST['camefrom']);
	return drupal_goto($camefrom);
}

/**
 * Menu callback; Hide a gallery media
 */
function gallery_image_hide($image) {
	gallery_image_visibility_save($image, '0');
	if ($_REQUEST['ajax'])
		return drupal_json_output(
			array('label' => t('Show'),
					'action' => url(
						'gallery/' . $image['item_type'] . '/'
								. $image['item_id'] . '/show/' . $image['id'])
			));
	$camefrom = filter_xss($_REQUEST['camefrom']);
	return drupal_goto($camefrom);
}

/**
 * Menu callback; Rate a gallery media
 */
function gallery_image_rate($image) {
	$type_id = $_POST['type'];
	$types = gallery_available_rating_types();
	foreach ($types as $type) {
		if ($type['id'] != $type_id)
			continue;
		gallery_image_rating_save(
			$image['id'],
			$type['id'],
			$_POST['rating_' . $type['id']]);
		break;
	}
	if ($_REQUEST['ajax']) {
		$select = db_select('gallery_image_rating', 'gir');
		$select->addField('gir', 'gallery_rating_type_id', 'id');
		$select->addExpression('AVG(gir.rating)', 'average');
		$select->addExpression('COUNT(gir.rating)', 'number');
		$select->condition('gir.gallery_image_id', $image['id']);
		$select->groupBy('gir.gallery_rating_type_id');
		$result = $select->execute();
		$image['ratings'] = array();
		foreach ($result as $record) {
			$image['ratings'][$record->id] = array(
					'rating_type' => $record->id,
					'average' => $record->average,
					'count' => $record->number
			);
		}

		return drupal_json_output(
			array('rating' => theme(
						'gallery_image_rating',
						array('image' => $image,
								'rating_types' => gallery_available_rating_types()
						)),
					'message' => t(
						'Your rating has successfully been registered')
			));
	}
	drupal_set_message(t('Your rating was successfully placed.'));
	$camefrom = $_POST['camefrom'];
	return drupal_goto($camefrom);
}

/**
 * Menu callback; Listing of all medias added by a given user.
 */
function gallery_own_list($user, $mediatype = "images") {
	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/gallery.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
	$page = array();

	if ($mediatype == "images" || $mediatype == "medias")
		$images = gallery_images(
			"images",
			false,
			false,
			false,
			true,
			false,
			false,
			array(),
			array('gi.users_uid = ' . $user->uid));
	if ($mediatype == "sounds" || $mediatype == "medias")
		$sounds = gallery_images(
			"sounds",
			false,
			false,
			false,
			true,
			false,
			false,
			array(),
			array('gi.users_uid = ' . $user->uid));
	if ($mediatype == "videos" || $mediatype == "medias")
		$videos = gallery_images(
			"videos",
			false,
			false,
			false,
			true,
			false,
			false,
			array(),
			array('gi.users_uid = ' . $user->uid));

	$links = array();
	if ($images['count'] || $sounds['count'] || $videos['count']) {

		if ($videos['count']) {
			$page['anchor_videos'] = array(
					'#markup' => '<a name="' . $params['key'] . '"></a>'
			);
			$page['gallery_list_videos'] = array(
					'#theme' => 'gallery_list',
					'#images' => $videos,
					'#rating_types' => gallery_available_rating_types()
			);
		}
		if ($sounds['count']) {
			$page['anchor_sounds'] = array(
					'#markup' => '<a name="' . $params['key'] . '"></a>'
			);
			$page['gallery_list_sounds'] = array(
					'#theme' => 'gallery_list',
					'#images' => $sounds,
					'#rating_types' => gallery_available_rating_types()
			);
		}
		if ($images['count']) {
			$page['anchor_images'] = array(
					'#markup' => '<a name="' . $params['key'] . '"></a>'
			);
			$page['gallery_list_images'] = array(
					'#theme' => 'gallery_list',
					'#images' => $images,
					'#rating_types' => gallery_available_rating_types()
			);
		}
	} else
		$page['gallery_list'] = array(
				'#markup' => '<p>'
						. t(
							'You have not added any @mediatype',
							array('@mediatype' => t($mediatype))) . '</p>',
		);
	array_push(
		$links,
		array('title' => t(
					'Manage @mediatype',
					array('@mediatype' => t($mediatype))),
				'href' => 'user/' . $user->uid . '/gallery/edit'
		));
	if ($backlink) {
		$back = _gallery_back_link('user', $user->uid);
		if ($back)
			array_push($links, $back);
	}
	if ($links)
		$page['actions'] = array(
				'#theme' => 'links',
				'#links' => $links,
				'#attributes' => array(
						'class' => 'gallery-action-links'
				),
				'#weight' => 9
		);
	drupal_set_title(
		t(
			'@mediatype added by %user',
			array('%user' => $user->name,
					'@mediatype' => t($mediatype)
			)),
		PASS_THROUGH);
	return $page;
}

function gallery_own_edit($user, $mediatype = "images") {
	$images = gallery_images(
		$mediatype,
		false,
		false,
		false,
		true,
		false,
		false,
		array(),
		array('gi.users_uid = ' . $user->uid));
	$form = drupal_get_form(
		'gallery_images_form',
		$mediatype,
		false,
		false,
		$images,
		'user/' . $user->uid . '/gallery/edit');
	drupal_set_title(
		t(
			'@mediatype added by %user',
			array('%user' => $user->name,
					'@mediatype' => t($mediatype)
			)),
		PASS_THROUGH);
	return $form;
}

/**
 * Returns HTML for the page containing the list of gallery medias
 *
 * @param $variables
 *   An associative array containing:
 *   - medias: An array of the medias to display
 *   - rating_types: An array of rating types
 *
 * @ingroup themeable
 */
function theme_gallery_list($variables) {
	$mediatype = $variables['images']['mediatype'];
	if ($mediatype == "images" || $mediatype == "medias") {

		drupal_add_css(
			drupal_get_path('module', 'gallery') . '/css/gallery.css',
			array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
			));
		drupal_add_css(
			drupal_get_path('module', 'gallery') . '/css/jquery.lightbox.css',
			array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
			));
		drupal_add_js(
			drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js',
			array(
					'weight' => 100
			));
		drupal_add_js(
			drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js',
			array(
					'weight' => 110
			));
	}
	if ($mediatype == "videos" || $mediatype == "sounds"
			|| $mediatype == "medias") {

		drupal_add_css(
			drupal_get_path('module', 'gallery') . '/css/divbox.css',
			array(
					'group' => CSS_DEFAULT,
					'every_page' => TRUE
			));
		drupal_add_js(
			drupal_get_path('module', 'gallery') . '/js/divbox.js',
			array(
					'weight' => 101
			));
		drupal_add_js(
			drupal_get_path('module', 'gallery') . '/js/gallery.divbox.js',
			array(
					'weight' => 111
			));
	}

	$images = $variables['images'][$mediatype];
	$rating_types = $variables['rating_types'];
	$types = _gallery_types();

	if (!$images)
		return;

	$url = drupal_get_destination();
	$url = drupal_parse_url($url['destination']);
	$url = $url['path'];
	$items = array();
	$i = 0;
	foreach ($images as $image) {
		$i++;
		$editable = gallery_edit_access_callback(
			$image['item_type'],
			$image['item_id'],
			$image);
		$caption = '';
		if ($image['title'])
			$caption .= '<strong>' . $image['title'] . '</strong>';
		if ($image['description'])
			$caption .= ' <small>' . $image['description'] . '</small>';
		if ($caption)
			$caption = '<span class="title">' . $caption . '</span>';
		if ($image['location'] || $image['author'])
			$caption .= '<span class="author">&copy; ' . $image['author']
					. ($image['location'] ? ', ' . $image['location'] : '')
					. '</span>';

		$image['mediatype'] = $mediatype;

		if ($mediatype == "images"
				|| ($mediatype == "medias" && $image['filemime'] == 'image/png'
						|| $image['filemime'] == 'image/jpg'
						|| $image['filemime'] == 'image/jpeg'
						|| $image['filemime'] == 'image/gif')) {

			if (array_key_exists($image['item_type'], $types)
					&& array_key_exists(
						'image_caption',
						$types[$image['item_type']]))
				$caption .= ' <em>'
						. call_user_func_array(
							$types[$image['item_type']]['image_caption'],
							array($image['item_id'])) . '</em>';

			$url_href = url(
				'gallery/' . $image['item_type'] . '/' . $image['item_id']
						. '/thumb/' . $image['id'] . '/gallery_large');
			$url_image = url(
				'gallery/' . $image['item_type'] . '/' . $image['item_id']
						. '/thumb/' . $image['id'] . '/gallery_thumbnail');
			$extras = '';
			$captionclass = "caption";

			if ($image['item_type'] == 'inventory_entry'
					|| $image['item_type'] == 'head_inventory')
				$media_label = '<div id="media_label"><span>Belegbild</span></div>';
			else
				$media_label = "";
		} elseif ($mediatype == "sounds"
				|| ($mediatype == "medias"
						&& $image['filemime'] == 'audio/mpeg')) {

			if ($image['filename'] || $image['timestamp'])
				$caption .= ' <em>' . $image['filename'] . ' / '
						. date("d.m.y", $image['timestamp']) . '</em>';

			$url_href = file_uri_target($image['uri']);
			$url_href = url('sites/default/files/' . $url_href);
			$url_image = url(
				drupal_get_path('module', 'gallery') . '/images/music_icon.png');
			$extras = 'class="divbox_sound"';
			$captionclass = "caption";

			if ($image['item_type'] == 'inventory_entry'
					|| $image['item_type'] == 'head_inventory')
				$media_label = '<div id="media_label"><span>Hörbeleg</span></div>';
			else
				$media_label = "";
		} elseif ($mediatype == "videos"
				|| ($mediatype == "medias" && $image['filemime'] == 'video/mp4')) {

			if ($image['filename'] || $image['timestamp'])
				$caption .= ' <em>' . $image['filename'] . ' / '
						. date("d.m.y", $image['timestamp']) . '</em>';

			$url_href = file_uri_target($image['uri']);
			$url_href = url('sites/default/files/' . $url_href);
			$url_image = url(
				drupal_get_path('module', 'gallery') . '/images/video_icon.png');
			$extras = 'class="divbox_video"';
			$captionclass = "caption";

			if ($image['item_type'] == 'inventory_entry'
					|| $image['item_type'] == 'head_inventory')
				$media_label = '<div id="media_label"><span>Belegvideo</span></div>';
			else
				$media_label = "";
		}

		array_push(
			$items,
			'<li class="' . join(' ', _gallery_get_class($i - 1)) . '">'
					. '<a ' . $extras . ' href="' . $url_href . '">'
					. '<img src="' . $url_image . '" alt="' . $image['title']
					. '" />' . $media_label . '</a>'
					. (user_access('manage gallery medias') && false /* disables the button */
							? '<form class="show_hide" method="post" action="'
									. url(
										'gallery/' . $image['item_type'] . '/'
												. $image['item_id'] . '/'
												. ($image['visible'] ? 'hide'
														: 'show') . '/'
												. $image['id']) . '">'
									. '<input type="hidden" name="camefrom" value="'
									. $url . '" />'
									. '<input type="submit" name="submit" value="'
									. ($image['visible'] ? t('Hide') : t('Show'))
									. '" />' . '</form>' : '') . '<div class="'
					. $captionclass . '">'
					. ($caption ? '<p class="captionText">' . $caption . '</p>'
							: '')
					. ($rating_types
							? theme(
								'gallery_image_rating',
								array('image' => $image,
										'rating_types' => $rating_types
								)) : '') . '</div>' . '</li>');
	}
	return '<ul class="gallery clearfix">' . join('', $items) . '</ul>';
}

function _gallery_get_class($i, $columns = GALLERY_COLUMNS) {
	$cls = array();
	$i = $i % $columns;
	if ($i == 0)
		array_push($cls, 'first');
	if ($i == $columns - 1)
		array_push($cls, 'last');
	if ($i % 2 == 0)
		array_push($cls, 'odd');
	if ($i % 2 == 1)
		array_push($cls, 'even');
	return $cls;
}

/**
 * Returns HTML for an medias rating types
 *
 * @param $variables
 *   An associative array containing:
 *   - media: A gallery media array
 *   - rating_types: An array of rating types
 *
 * @ingroup themeable
 */
function theme_gallery_image_rating($variables) {
	global $base_url;
	$mediatype = $variables['image']['mediatype'];
	drupal_add_library('system', 'ui.widget');
	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/jquery.ui.stars.min.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
	drupal_add_css(
		drupal_get_path('module', 'gallery') . '/css/gallery.css',
		array(
				'group' => CSS_DEFAULT,
				'every_page' => TRUE
		));
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/jquery.ui.stars.min.js',
		array(
				'weight' => 101
		));
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/jquery.lightbox.js',
		array(
				'weight' => 110
		));
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/gallery.rating.js',
		array(
				'weight' => 111
		));
	drupal_add_js(
		drupal_get_path('module', 'gallery') . '/js/gallery.lightbox.js',
		array(
				'weight' => 100
		));
	$image = $variables['image'];
	$rating_types = $variables['rating_types'];
	if (!$rating_types)
		return '';
	$url = drupal_get_destination();
	$url = drupal_parse_url($url['destination']);
	$url = $url['path'];
	$allowed = user_access('rate gallery medias');
	$ratings = '';
	foreach ($rating_types as $rating_type) {
		$rating = is_array($image['ratings'])
				&& array_key_exists($rating_type['id'], $image['ratings'])
				? $image['ratings'][$rating_type['id']]
				: array(
						'count' => 0,
						'average' => 0
				);
		$avg = intval($rating['count']) ? (floatval($rating['average']) + 3.0)
				: 0;
		$ratings .= '<form method="post" action="'
				. url(
					'gallery/' . $image['item_type'] . '/' . $image['item_id']
							. '/rate/' . $image['id']) . '" data-average="'
				. $avg . '">' . '<input type="hidden" name="type" value="'
				. $rating_type['id'] . '" />'
				. '<input type="hidden" name="camefrom" value="' . $url
				. '" />' . '<label for="rating_' . $rating_type['id']
				. '" title="' . $rating_type['description'] . '">'
				. $rating_type['name'] . ' <small>('
				. t(
					'Average: %avg; %num votes',
					array('%avg' => $avg,
							'%num' => intval($rating['count'])
					)) . ')</small></label>' . '<select name="rating_'
				. $rating_type['id'] . '" id="rating_' . $rating_type['id']
				. '" size="1"' . (!$allowed ? ' disabled="disabled"' : '')
				. '>' . '<option value="-2">' . t('Poor') . '</option>'
				. '<option value="-1">' . t('Not that bad') . '</option>'
				. '<option value="0" selected="selected">' . t('Average')
				. '</option>' . '<option value="1">' . t('Good') . '</option>'
				. '<option value="2">' . t('Perfect') . '</option>'
				. '</select>'
				. ($allowed
						? '<input type="submit" name="rate" value="'
								. t('Rate') . '" />' : '') . '</form>';
	}
	return '<div class="rating">' . $ratings . '</div>';
}

/**
 * If $_SERVER['HTTP_REFERER'] available and valid, generate an array with the elements
 *  'title' => t('Back')
 *  'href' => $lastVisitedPage
 *
 *
 * @param $type
 * 	String with the type of the gallery: 'images', 'videos', etc.
 * @param $id
 * 	Integer with the Id of the managed object (Habitat Id, Organism Id, etc.)
 * @return
 * 	On success, return an array with the elements 'title' and 'href'
 */
function _gallery_back_link($type, $id) {
	global $base_url;
	$gallery_back_url_key = 'gallery_back_url_' . $type . '_' . $id;
	if ((strlen($_SERVER['HTTP_REFERER']) < strlen($base_url) + 8
			|| substr($_SERVER['HTTP_REFERER'], strlen($base_url) + 1, 7)
					!= 'gallery')
			&& substr($_SERVER['HTTP_REFERER'], 0, strlen($base_url))
					== $base_url)
		$_SESSION[$gallery_back_url_key] = substr(
			$_SERVER['HTTP_REFERER'],
			strlen($base_url) + 1);
	$url = drupal_parse_url($_SESSION[$gallery_back_url_key]);
	$_SESSION[$gallery_back_url_key] = $url['path'];
	if ($_SESSION[$gallery_back_url_key])
		return array(
				'title' => t('Back'),
				'href' => filter_xss($_SESSION[$gallery_back_url_key])
		);
	return false;
}

function gallery_metadata_popup() {
	$output['form'] = drupal_render(drupal_get_form('gallery_metadata_form'));
	return drupal_json_output($output);
}

function gallery_metadata_form() {
	$file_name = check_plain($_REQUEST['fn']);
	$div_id = check_plain($_REQUEST['di']);

	$form['metadata'] = array(
			'#type' => 'fieldset',
			'#title' => t('Metadata'),
			'#weight' => 1,
			'#collapsible' => true,
			'#collapsed' => $collapsed,
			'#attached' => array(
					'js' => array(
							'misc/form.js',
							'misc/collapse.js',
					),
			),
			'#attributes' => array(
					'class' => array(
							'collapsible'
					),
			),
	);

	if (gallery_validate_file_extension($file_name)) {
		$form['metadata']['title'] = array(
				'#type' => 'textfield',
				'#title' => t('Title'),
				'#size' => 50,
				'#maxlength' => 128,
				'#required' => FALSE,
		);
		$form['metadata']['description'] = array(
				'#type' => 'textfield',
				'#title' => t('Description'),
				'#size' => 50,
				'#maxlength' => 255,
				'#required' => FALSE,
		);
		$form['metadata']['location'] = array(
				'#type' => 'textfield',
				'#title' => t('Location'),
				'#size' => 50,
				'#maxlength' => 255,
				'#required' => FALSE,
		);
		$form['metadata']['author'] = array(
				'#type' => 'textfield',
				'#title' => t('Author'),
				'#size' => 50,
				'#maxlength' => 255,
				'#description' => t('The author or copyright of the file'),
				'#required' => FALSE,
		);
		$form['metadata']['fomr'] = array(
				'#type' => 'textfield',
				'#attributes' => array(
						'id' => 'uploadform',
						'name' => 'uploadform',
						'value' => $div_id,
						'style' => array(
								'display: none;',
								'height: 0px;'
						)
				),
		);
	} else {
		$form['metadata'] = t('File not allowed');
	}
	$form['actions'] = array(
			'#type' => 'actions'
	);
	$form['actions']['close'] = array(
			'#markup' => l(t('Cancel'), 'observations')
	);
	$form['actions']['save'] = array(
			'#type' => 'submit',
			'#value' => t('Save'),
			'#attributes' => array(
					'style' => array(
							'float:right;'
					)
			)
	);

	return $form;
}
