/**
 * @author Roger Wolfer, Roman Schaller
 */
 
var INVENTORY_SERVER_PATH = Drupal.settings.basePath + '../inventoryServer/web_root/public/';

function AreaSelect() {
	
	//////////////////////// Method declarations ///////////////////////
	/**
	 * This function will be called if the user clicks on the plus or minus symbol at the beginning
	 * of a row.
	 */
	AreaSelect.prototype.onTableExpanderClicked = function () {
	    var nTr = this.parentNode.parentNode;
	    if ( this.src.match('details_close') ) {
	        /* This row is already open - close it */
	        this.src = "modules/area/images/details_open.png";
	        me.area_table.fnClose( nTr );
	    } else {
	        /* Open this row */
	        this.src = "modules/area/images/details_close.png";
	        me.area_table.fnOpen( nTr, me.formatDetails(nTr), 'details' );
	    }
	}

	/**
	 * This function is called when the user clicks on a row
	 */
	AreaSelect.prototype.onTableRowClicked = function(e) {
		if (!e) var e = window.event;
		if (e.target) targ = e.target;
		else if (e.srcElement) targ = e.srcElement;
		if (targ.nodeType == 3) // defeat Safari bug
			targ = targ.parentNode;
		var overlayId = targ.parentNode.id.split('_');
		me.selectArea(overlayId[1], targ);
	}
	
	/**
	 * This is called if the user hovers over a table row
	 */
	AreaSelect.prototype.onTableRowHover = function(event) {
		var overlayid = jQuery(event.srcElement).parent().attr('overlay_id');
		var overlay = me.mapOverlays.overlays[overlayid];
		if (overlay) {
			var parent = event.target.parentNode;
			if (event.type == 'mouseover') {
				jQuery(parent).addClass('row_highlighted');
				overlay.setStyle('highlighted');
			} else {
				jQuery(parent).removeClass('row_highlighted');
				overlay.setStyle('highlighted-disable');
			}
		}
	}
	
	/**
	 * This is called if an area is selected from any source (table, dropdown, map)
	 */
	AreaSelect.prototype.selectArea = function(overlayId, targ) {
		// deselect area if it was previously selected
		if(areaselect.selected_area){
			areaselect.mapOverlays.overlays[areaselect.selected_area].setStyle('selected-disable');
		}

		// handle map highlighting
		overlay = areaselect.mapOverlays.overlays[overlayId];
		overlay.setStyle('selected');
		areaselect.map.panTo(overlay.getCenter());
		areaselect.showAreaInfoBubble(overlay);
		// remember selected area
		areaselect.selected_area = overlayId;
		// set area id, used when we just assign an area to a already existing inventory
		jQuery('#edit-id-area').val(overlayId);
		
		if(jQuery('#show_areas')) {
			if(targ == null) targ = jQuery('#area_'+overlayId+' td');
			jQuery("td.selected_row", oTable.fnGetNodes()).removeClass('selected_row');
			jQuery(targ).parent().find("td").addClass('selected_row');
			jQuery('#show_areas').parent().scrollTo(jQuery('#area_'+overlayId));
		}
	}

	/**
	 * Creates the google maps object and attaches it to the element with the id
	 * 'map_canvas'. Returns a google.maps.Map
	 */
	AreaSelect.prototype.createGoogleMaps = function() {
		var bruggCenter = new google.maps.LatLng(47.487084, 8.207273);
		var mapsOptions = {
				zoom : 15,
				center : bruggCenter,
				mapTypeId : google.maps.MapTypeId.ROADMAP,
				scrollwheel: false
			};
		var map = new google.maps.Map(document.getElementById('map_canvas'), mapsOptions);
		util.geocode('map_search', 'map_search_button', map);
		return map;
	}

	/**
	 * Creates the bubble which shows information on an area when the user clicks on a area.
	 * Returns an InfoBubble
	 */
	AreaSelect.prototype.createAreaInfo = function(map) {
		areaInfo = new InfoBubble({
			map : map,
			shadowStyle : 1,
			padding : 8,
			borderRadius : 8,
			arrowSize : 40,
			borderWidth : 1,
			borderColor : '#2c2c2c',
			disableAutoPan : true,
			arrowPosition : 20,
			arrowStyle : 2
		});
		areaInfo.addTab('Details', '');
		areaInfo.addTab('Inventories', '');
		areaInfo.addTab('Habitats', '');
		return areaInfo;
	}

	/**
	 * Creates the overlay representing areas
	 * Returns MapGeometryOverlays
	 */
	AreaSelect.prototype.createOverlays = function(map) {
		var overlays = new MapGeometryOverlays(map);
		return overlays;
	}
	
	/**
	 * Is called if new area data is needed. This makes a call to the server by ajax.
	 */
	AreaSelect.prototype.onRequestServerData = function ( sSource, aoData, fnCallback ) {
        /* Add some data to send to the source, and send as 'POST' */
        jQuery.ajax( {
            "dataType": 'json',
            "type": "POST",
            "url": sSource,
            "data": aoData,
            "success": function (json) {
                var aaData = [];
                var areas = json.areas;
                me.mapOverlays.clear();

                for(var i in areas) {
                    aaData.push([
                      '<img src="modules/area/images/details_open.png"></img>',
                      areas[i]['field_name'], 
                      areas[i]['locality'],
                      areas[i]['township'],
                      areas[i]['canton'],
                      areas[i]['surface_area'],    
                      areas[i]['altitude'], 
                      areas[i]['type'],
                      areas[i]['parcel_nr'],
                      me.formatHabitats(areas[i]['habitats']),   
                      areas[i]['comment'], 
                      areas[i]['id']]);
                }
                var newJson = {
                    'sEcho' : json.sEcho,
                    'iTotalRecords' : json.iTotalRecords,
                    'iTotalDisplayRecords': json.iTotalDisplayRecords,
                    'aaData': aaData
                }
                me.mapOverlays.addOverlaysJson(json.areas);

                for(var i in me.mapOverlays.overlays) {
                    var overlay = me.mapOverlays.overlays[i];
                    me.addOverlayListener(overlay);
                }
                fnCallback(newJson);
                jQuery(me.area_table.fnSettings().aoData).each( function () {
                    this.nTr.setAttribute('overlay_id', this._aData[11]);
                });
            }
        } );
    }
	
	/**
	 * new getarea function which fetches all area data with a json request
	 */
	function getareasJSON() {
		var aoData;
        jQuery.ajax( {
            "dataType": 'json',
            "type": "GET",
            "url": Drupal.settings.basePath + "area/getareas",
            
            "success": function (json) {
                var aaData = [];
                var areas = json;
                me.mapOverlays.clear();
                me.mapOverlays.addOverlaysJson(areas);

                for(var i in me.mapOverlays.overlays) {
                    var overlay = me.mapOverlays.overlays[i];
                    me.addOverlayListener(overlay);
                }
            }
        } );
    }

	/**
	 * adds a listener for selecting and hover to geometry overlays
	 */
	AreaSelect.prototype.addOverlayListener = function(overlay) {
	    //hover
	    google.maps.event.addListener(overlay.gOverlay, 'mouseover', function(event) {
	        jQuery('tr[overlay_id="'+overlay.id+'"]').addClass('row_highlighted');
	        overlay.setStyle('highlighted');
	    });
	    google.maps.event.addListener(overlay.gOverlay, 'mouseout', function(event) {
	        jQuery('tr[overlay_id="'+overlay.id+'"]').removeClass('row_highlighted');
	        overlay.setStyle('highlighted-disable');
	    });

	    //select
	    google.maps.event.addListener(overlay.gOverlay, 'click', function(event) {
	    	me.selectArea(overlay.id);
	    });
	};
	
	/**
	 * Shows the info bubble of an area.
	 * It first closes the bubble, moves it's position, makes a ajax request and if that returns, updates and shows the bubble again.
	 */
	AreaSelect.prototype.showAreaInfoBubble = function(overlay) {
		// close and move
		me.areaInfo.close();
		me.areaInfo.setPosition(overlay.getCenter());
		
		// update and show area info
		jQuery.getJSON(Drupal.settings.basePath + 'area/json/' + overlay.id, function(data, textStatus, jqXHR) {
			// area details
			var details = '<div>';
			details += '<strong>' + data.field_name + '</strong>';
			details += '<div>' + data.creator + '<br>';
			details += data.locality + '<br>';
			details += data.zip + ' ' + data.township + '<br>';
			details += data.altitude + '<br>';
			details += data.surface_area + '<br>';
			details += '</div>';
			details += '<div>' + data.comment + '</div>';
			me.areaInfo.updateTab(0, 'Details', details);

			// inventories
			inventories = '';
			for ( var i = 0, len = data.inventories.invs.length; i < len; ++i) {
				inventory = data.inventories.invs[i];
				inventories += '<div><a href="inventory/' + inventory.id + '"><strong>' + inventory.name + ':</strong></a></div><div>';
				for ( var ie = 0, ieLen = inventory.types.length; ie < ieLen; ++ie) {
					type = data.inventories.invs[i].types[ie];
					inventories += type.name + ' (' + type.count + ' x)';
					if (ieLen > ie + 1) {
						inventories += ', ';
					}
				}
				inventories += '</div>';
			}
			me.areaInfo.updateTab(1, data.inventories.title, inventories);

			// habitats
			habitats = '<div><ul>';
			if (data.habitats.habs) {
				for ( var i = 0, len = data.habitats.habs.length; i < len; ++i) {
					habitats += '<li>' +data.habitats.habs[i] + '</li>';
				}
			}
			habitats += '</ul></div>';
			me.areaInfo.updateTab(2, data.habitats.title, habitats);

			// and finally show the fancy popup bubble:
			me.areaInfo.open(me.map);
		});
	}

	AreaSelect.prototype.formatDetails = function ( nTr ){
	  var aData = me.area_table.fnGetData( nTr );
	  var sOut = '<table cellpadding="5" cellspacing="0" border="0" style="padding-left:50px;">';
	  sOut += '<tr><td>Parzellen Nr.:</td><td>'+aData[8]+'</td></tr>';
	  sOut += '<tr><td>Lebensräume:</td><td>'+aData[9]+'</td></tr>';
	  sOut += '<tr><td>Beschreibung:</td><td>'+aData[10]+'</td></tr>';
	  sOut += '</table>';
	  
	  return sOut;
	};

	AreaSelect.prototype.formatHabitats = function(habitats){
	  var sHabitats = '<table>';
	  for(var hi in habitats){
	    sHabitats += '<tr><td>'+habitats[hi]['label'] + '</td><td>' + habitats[hi]['name_de']+'</td></tr>';
	  }
	  sHabitats += '</table>';
	  return sHabitats;
	};
	
	AreaSelect.prototype.onControlAreaChooseClicked = function(event) {
		jQuery('#controlAreaChoose').addClass('selected');
		jQuery('#controlAreaCreate').removeClass('selected');
		areaselect.overlayControl.stopDigitizing();
		jQuery('#area-new-form').addClass('hidden');
		jQuery('#area-choose-form').removeClass('hidden');
		jQuery('#area-field-name-form').removeClass('hidden');
	}

	AreaSelect.prototype.onControlAreaCreateClicked = function(event) {
		jQuery('#controlAreaChoose').removeClass('selected');
		jQuery('#controlAreaCreate').addClass('selected');
		jQuery('#area-new-form').removeClass('hidden');
		jQuery('#area-choose-form').addClass('hidden');
		jQuery('#area-field-name-form').addClass('hidden');
		areaselect.overlayControl.startDigitizing();
	}

	//////////////////////// Class initialisation //////////////////////
	
	// Member variable initialisation
	var me = this; // references to the instance of AreaSelect
	me.map = this.createGoogleMaps();
	me.mapOverlays = this.createOverlays(me.map); // Overlays representing areas
	me.selected_area = null; // index of currently selected area
	me.areaInfo = this.createAreaInfo(me.map); // popup bubble shown if user clicks on an area
	me.overlayControl = new GeometryOverlayControl(me.map); // class to control drawing of new areas
	getareasJSON();

	// register events
	jQuery("#show_areas").live('click', this.onTableRowClicked);
	jQuery('tbody > tr').live('mouseover mouseout', this.onTableRowHover);
    jQuery('#area_table tbody td img').live( 'click', this.onTableExpanderClicked);
    jQuery('#controlAreaChoose').click(this.onControlAreaChooseClicked);
    jQuery('#controlAreaCreate').click(this.onControlAreaCreateClicked);
};

var lastQuery = null;
function refresh_map_info(){
    if(areaselect.overlayControl.overlay != null) {
    	var area_coords_new = new Array();
		var markers_new = areaselect.overlayControl.overlay.markers;
		for (var i in markers_new) {
			area_coords_new.push(new Array(markers_new[i].position.lat(), markers_new[i].position.lng()));
		}
		area_coords_new = JSON.stringify(area_coords_new);
		var centGLatLng1 = areaselect.overlayControl.overlay.getCenter();
		area_coords_new = area_coords_new + centGLatLng1.lat() + centGLatLng1.lng()

    	if (lastQuery!=area_coords_new){
			jQuery('#edit-surface-area').val(areaselect.overlayControl.overlay.getArea());
			areaselect.overlayControl.overlay.getAltitude(function(altitude) {
				jQuery('#edit-altitude').val(altitude);
			});
			
			areaselect.overlayControl.overlay.getAddress(function(address) {
				jQuery('#edit-canton').val(address.canton);
				jQuery('#edit-township').val(address.township);
				jQuery('#edit-locality').val(address.locality);
				jQuery('#edit-zip').val(address.zip);
				jQuery('#edit-country').val(address.country);
			});
	
			var centGLatLng = areaselect.overlayControl.overlay.getCenter();
			jQuery('#edit-latitude').val(centGLatLng.lat());
			jQuery('#edit-longitude').val(centGLatLng.lng());
			var area_coords = new Array();
			var markers = areaselect.overlayControl.overlay.markers;
			for (var i in markers) {
				area_coords.push(new Array(markers[i].position.lat(), markers[i].position.lng()));
			}
			area_coords = JSON.stringify(area_coords);
			
			jQuery('#edit-area-coords').val(area_coords);
			jQuery('#edit-area-type').val(areaselect.overlayControl.overlay.type);
			lastQuery = area_coords + centGLatLng.lat() + centGLatLng.lng()
    	} else {
    		console.info("identical query");
    	}
    } else {
    	console.info("overlay is null, cleaning up stuff");
    	jQuery('#edit-surface-area').val('');
		jQuery('#edit-altitude').val('');
		jQuery('#edit-canton').val('');
		jQuery('#edit-township').val('');
		jQuery('#edit-locality').val('');
		jQuery('#edit-zip').val('');
		jQuery('#edit-country').val('');		
		jQuery('#edit-latitude').val('');
		jQuery('#edit-longitude').val('');
		jQuery('#edit-area-coords').val('');
		jQuery('#edit-area-type').val('');
    };
};

var areaselect = null;
jQuery(document).ready(function() {
	areaselect = new AreaSelect();
	/* clear edit-id-area field. Solves the problem that if a user navigates back problem that
	 * he can continue to new inv without actually having selected an area */
	jQuery('#edit-id-area').val('');
});