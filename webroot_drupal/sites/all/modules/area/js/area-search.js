/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-search.js
 *
 * Create a searchbar using autocomplete-feature by google places api.
 */

jQuery(document).ready(function() {
	Area.prototype.createSearchbar = function() {
		var googlemap = this.googlemap;
		// create a new div element to hold everything needed for the searchbar
		var searchdiv = document.createElement('div');
		// create new input field
		var searchinput = document.createElement('input');
		// create new text field
		// add searchinput to searchdiv
		searchdiv.appendChild(searchinput);
		// set id's for both elements
		searchdiv.setAttribute('id', 'search_container');
		searchinput.setAttribute('id', 'search_input');

		// push the search element on the google map in the top left corner
		googlemap.controls[google.maps.ControlPosition.TOP_LEFT]
				.push(searchdiv);

		var autocomplete = new google.maps.places.Autocomplete(searchinput, {
			// available types: 'geocode' for places, 'establishment' for
			// companies
			types : [ 'geocode' ]
		});

		// listen do pressed suggestions and center map on them
		autocomplete.bindTo('bounds', googlemap);

		google.maps.event.addListener(autocomplete,	'place_changed', function() {
			var place = autocomplete.getPlace();
			console.log(place);
			if (typeof place.geometry !== 'undefined') {
				if (place.geometry.viewport) {
					googlemap.fitBounds(place.geometry.viewport);
				} else {
					console.log(googlemap);
					googlemap.setCenter(place.geometry.location);
					googlemap.setZoom(17);
				}
			} else {
				console.error("Search by pressing enter not yet implemented.");
				console.error("Query was: " + searchinput.value);
			}
		});

		// remove focus from searchinput when map moved
		google.maps.event.addListener(googlemap, 'bounds_changed', function() {
			searchinput.blur();
		});

		// select all text when user focus the searchinput field
		searchinput.onfocus = function() {
			searchinput.select();
		};
		
		// do not submit the form when pressing enter
		searchinput.onkeypress = function(evt) {
		    evt = evt || window.event;
		    var charCode = evt.keyCode || evt.which;
		    if (charCode == 13) {
		        evt.returnValue = false;
		        if (evt.preventDefault) {
		            evt.preventDefault();
		        }
		    }
		};
	};
	
	areabasic.createSearchbar();
});