jQuery(function($) {

	// make columns selection draggable
	$('body.page-find .columns-overlay ul').sortable();

	// show/hide columns selection
	$('body.page-find .columns-select').click(function() {
		$('body.page-find .columns-overlay').toggle();
		return false;
	});
	// activate filter after click on <li>
	$('body.page-find .filters li span').click(function() {
		var input = $(this).prev('input[type="checkbox"]');
		if(input.attr('checked')) {
			input.removeAttr('checked');
		} else {
			input.attr('checked', 'checked');
		}
	});
	// show filter after click on <select>
	$('body.page-find .filter-selector').change(function() {
		$('body.page-find .filter-' + $(this).val()).slideDown();
		$('body.page-find .filter-selector option[value="' + $(this).val() + '"]').attr('disabled', 'disabled');
		$(this).val('');
	});
	// hide filter on clear
	$('body.page-find .clear').click(function() {
		var div = $(this).closest('div');
		div.slideUp('fast');
		$('input', div).attr('disabled', 'disabled');
		$('#search-form').submit();
		return false;
	});
	var map;
	var latlng = new google.maps.LatLng(46.8, 8.233333);
	// CH
	var myOptions = {
		center : latlng,
		mapTypeId : google.maps.MapTypeId.ROADMAP,
		maxZoom : 18,
		minZoom : 5,
		mapTypeControl : false,
		zoom : 8
	};
	var overlay;
	var control;

	// show map
	$('body.page-find .map-select').click(function() {
		$('body.page-find .map-overlay').css('left', '50%');

		// init map for area selection
		map = new google.maps.Map(document.getElementById('map-canvas'), myOptions);

		// geometry control to draw on map
		control = new GeometryControl(map, function() {
			if(overlay) {
				overlay.overlay.setMap(null);
			}
		});
		//create searchbar
		var searchBar = document.createElement('div');
		var inputSearch = document.createElement('input');
		searchBar.appendChild(inputSearch);
		searchBar.setAttribute('id', 'search_container');
		inputSearch.setAttribute('id', 'search_input');
		map.controls[google.maps.ControlPosition.TOP_LEFT].push(searchBar);
		var ac = new google.maps.places.Autocomplete(inputSearch, {
			types : ['geocode']
		});
		ac.bindTo('bounds', map);

		google.maps.event.addListener(ac, 'place_changed', function() {
			var place = ac.getPlace();
			if(place.geometry.viewport) {
				map.fitBounds(place.geometry.viewport);
			} else {
				map.setCenter(place.geometry.location);
				map.setZoom(17);
			}
		});

		inputSearch.onfocus = function() {
			inputSearch.value = '';
		}

		google.maps.event.addListener(map, 'bounds_changed', function() {
			inputSearch.blur();
		});

		google.maps.event.addDomListener(inputSearch, 'keydown', function(e) {
			var key = e.keyCode ? e.keyCode : e.charCode;
			if(key == 13) {
				if(e.preventDefault) {
					e.preventDefault();
				} else {
					e.cancelBubble = true;
					e.returnValue = false;
				}
			}
		});

		control.addPolygon(function() {
			// don't hide polygon
			control.overlay.overlay.setMap(map);
			control.overlay.overlay.getPath().forEach(function(position) {
				$('#search-form').append('<input type="hidden" name="geo[]" value="' + position.toUrlValue() + '" />')
			});
			$('#search-form').submit();
		});
		// add overlay if geo is set
		if(Drupal.settings.find.geo.length > 1) {

			var path = [];
			for(var i = 0, l = Drupal.settings.find.geo.length; i < l; i++) {
				var values = Drupal.settings.find.geo[i].split(',');
				path.push(new google.maps.LatLng(values[0], values[1]));
			}
			overlay = new Polygon(map);
			overlay.overlay.setPath(path);
			map.fitBounds(overlay.getBounds());
		}
	});
	$('body.page-find .map-overlay .map-close').click(function() {
		$('.map-overlay').css('left', '-9000px');
	});
});
