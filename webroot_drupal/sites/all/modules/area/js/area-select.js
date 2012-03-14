/**
 * @author Roger Wolfer
 * @author Roman Schaller
 */

function AreaSelect(map_id) {
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
  
  var lastHoverId = 0;
  AreaSelect.prototype.showStaticImage = function(event) {
    var pos = jQuery(event.target).position();
    if (event.type == 'mouseout'){
      jQuery('#static_image').css('display', 'none');
    }
  }
  
  /**
   * This is called if an area is selected from any source (table, dropdown, map)
   */
  AreaSelect.prototype.selectArea = function(overlayId, targ) {
    // deselect area if it was previously selected
    if(this.selected_area){
      this.mapOverlays.overlays[this.selected_area].setStyle('selected-disable');
    }

    // handle map highlighting
    overlay = this.mapOverlays.overlays[overlayId];
    if(!overlay)
      return;
    overlay.setStyle('selected');
    var bounds = new google.maps.LatLngBounds();
    var points = overlay.getLatLngs().getArray();
    for(var n=0; n<points.length; n++) {
      bounds.extend(points[n]);
    }
    this.map.fitBounds(bounds);
    if (this.map.getZoom() > 17) {
        this.map.setZoom(17);
    }
    this.showAreaInfoBubble(overlay);
    // remember selected area
    this.selected_area = overlayId;
    // set area id, used when we just assign an area to a already existing inventory
    jQuery('#edit-id-area').val(overlayId);
    
    if(jQuery('#show_areas').size()) {
      if(targ == null) targ = jQuery('#area_'+overlayId+' td');
      jQuery("#show_areas td.selected_row").removeClass('selected_row');
      jQuery(targ).parent().find("td").addClass('selected_row');
      jQuery('#show_areas').parent().scrollTo(jQuery('#area_'+overlayId));
    }
  }

  /**
   * Creates the google maps object and attaches it to the element with the id
   * 'map_canvas'. Returns a google.maps.Map
   */
  AreaSelect.prototype.createGoogleMaps = function() {
    var mapcenter = [46.77373, 8.25073];
    var canvas = jQuery('#'+me.map_id);
    if(!canvas.size())
      return false;
    zoom = canvas.data('zoom') ? parseInt(canvas.data('zoom')) : 15;
    if(canvas.data('center'))
      mapcenter = canvas.data('center').split(',');
    else
      zoom = 8;
    me.center = new google.maps.LatLng(parseFloat(mapcenter[0]), parseFloat(mapcenter[1]));
    var mapsOptions = {
        zoom : zoom,
        center : me.center,
        mapTypeId : google.maps.MapTypeId.ROADMAP,
        maxZoom: 18,
        minZoom: 5,
        scrollwheel: true
      };
    var map = new google.maps.Map(document.getElementById(me.map_id), mapsOptions);

    // switch to satellite for near zoom
    google.maps.event.addListener(map, 'zoom_changed', function() {
        if (map.getMapTypeId() == google.maps.MapTypeId.ROADMAP && map.getZoom() > 13) {
            map.setMapTypeId(google.maps.MapTypeId.HYBRID);
        }
        if (map.getMapTypeId() == google.maps.MapTypeId.HYBRID && map.getZoom() < 14) {
            map.setMapTypeId(google.maps.MapTypeId.ROADMAP);
        }
    });

    if(canvas.data('layers')) {
      var layers = canvas.data('layers').split(';');
      for(var i=0; i<layers.length; i++) {
        var parser = new geoXML3.parser({
          map: map, 
          singleInfoWindow: true
        });
        parser.parse(layers[i]);
      }
    }
    return map;
  }

  /**
   * Creates the bubble which shows information on an area when the user clicks on a area.
   * Returns an InfoBubble
   */
  AreaSelect.prototype.createAreaInfo = function(map) {
    if(typeof InfoBubble == "undefined")
      return;
    areaInfo = new InfoBubble({
      map : map,
      shadowStyle : 1,
      padding : 10,
      borderRadius : 0,
      arrowSize : 10,
      borderWidth : 1,
      borderColor : '#ccc',
      disableAutoPan : true,
      disableAnimation : true,
      arrowPosition : 50,
      arrowStyle : 0,
      maxHeight : 200,
      maxWidth : 300
    });
    areaInfo.addTab('Details', '');
    areaInfo.addTab('Inventories', '');
    areaInfo.addTab('Habitats', '');
    return areaInfo;
  }

  /**
   * Creates the overlay representing areas
   * Returns GeometryOverlays
   */
  AreaSelect.prototype.createOverlays = function(map) {
    var overlays = new GeometryOverlays(map);
    return overlays;
  }

  /**
   * Creates the toolbar for the map controls
   * Returns GeometryControl
   */
  AreaSelect.prototype.createControl = function(map) {

      var that = this;

      var control = new GeometryControl(map, function () {
          bubble.close();
      });
      
      var bubble = new InfoBubble({
          map : this.map,
          shadowStyle : 1,
          padding : 10,
          borderRadius : 0,
          arrowSize : 10,
          borderWidth : 1,
          borderColor : '#ccc',
          disableAutoPan : true,
          disableAnimation : true,
          arrowPosition : 50,
          arrowStyle : 0,
          maxHeight : 200,
          maxWidth : 300
      });
      var html = '<div><div style="width: 275px; padding: 10px 0;"><label>Gebiet / Flurname: <span class="form-required" title="Diese Angabe ist erforderlich">*</span></label><input class="areaname" size="39" /></div><div style="padding: 10px 0;"><input class="submit" type="submit" value="Gebiet erstellen" style="font-weight: bold;" /></div></div>';
      var tab = jQuery(html);
      jQuery('.submit', tab).click(function (e) {
          var name = jQuery.trim(jQuery('.areaname', tab).val());
          if (name) {
              jQuery('#edit-field_name').val(name);

              jQuery(this).val('Gebiet wird erstellt ...');
              jQuery(this).attr('disabled', 'disabled');

              jQuery.post(Drupal.settings.basePath + 'area/new', jQuery("#area-create-form").serialize(), function (json) {

                  bubble.close();
                  control.overlay.clear();

                  areaselect.mapOverlays.addOverlaysJson(json);

                  var overlay = areaselect.mapOverlays.overlays[json[0].id];
                  areaselect.addOverlayListener(overlay);
                  areaselect.selectArea(overlay.id);
              });
          } else {
              alert('Bitte Gebietsnamen eingeben, um das Gebiet zu erstellen!')
          }
      });
      bubble.addTab('Gebiet erstellen', tab.get(0));

      google.maps.event.addListener(bubble, 'closeclick', function() {
          control.overlay.clear();
      });

      var display = function () {
          refresh_map_info();
          jQuery('.areaname', tab).val('');
          jQuery('.submit', tab).val('Gebiet erstellen');
          jQuery('.submit', tab).attr('disabled', '');
          bubble.setPosition(control.overlay.getCenter());
          bubble.open();
      };

      control.addHand(display);
      control.addMarker(display);
      control.addPath(display);
      control.addPolygon(display);
      
      return control;
  }
  
  /**
	 * Is called if new area data is needed. This makes a call to the server by
	 * ajax.
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
            "url": Drupal.settings.basePath + "area/getareas" + (jQuery('#'+me.map_id).data('areaid') ? '/'+jQuery('#'+me.map_id).data('areaid') : ''),
            
            "success": function (json) {
                if(!json || !json.length)
                  return;
                var aaData = [];
                me.areas = json;
                me.mapOverlays.clear();
                me.mapOverlays.addOverlaysJson(me.areas);
                
                if(typeof InfoBubble == "undefined")
                  return;
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
      //select
      if(typeof InfoBubble != "undefined")
        google.maps.event.addListener(overlay.overlay, 'click', function(event) {
          me.selectArea(overlay.id);
        });
      else
        overlay.setClickable(false);
      
      if(!jQuery('tr[overlay_id="'+overlay.id+'"]').size())
        return;
      //hover
      google.maps.event.addListener(overlay.gOverlay, 'mouseover', function(event) {
          jQuery('tr[overlay_id="'+overlay.id+'"]').addClass('row_highlighted');
          overlay.setStyle('highlighted');
      });
      google.maps.event.addListener(overlay.gOverlay, 'mouseout', function(event) {
          jQuery('tr[overlay_id="'+overlay.id+'"]').removeClass('row_highlighted');
          overlay.setStyle('highlighted-disable');
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
      var details = '<div style="position: relative;">';
      details += '<div style="position: absolute; right: 0; bottom: 0;"><a href="' + Drupal.settings.basePath + 'inventory/new/area/' + overlay.id + '">+ Inventar erfassen</a></div>';
      if (data.edit) {
          details += '<strong>' + data.field_name + ' <a href="' + Drupal.settings.basePath + 'area/' + overlay.id + '/edit">(bearbeiten)</a></strong>';
      } else {
          details += '<strong><a href="' + Drupal.settings.basePath + 'area/' + overlay.id + '">' + data.field_name + '</a></strong>';
      }
      details += '<div>' + data.comment + '</div>';
      details += '<div>' + data.creator + '<br>';
      details += data.locality + '<br>';
      details += data.zip + ' ' + data.township + '<br>';
      details += data.altitude + '<br>';
      details += data.surface_area + '<br>';
      details += '</div>';
      me.areaInfo.updateTab(0, 'Gebiet', details);

      // inventories
      inventories = '';
      for ( var i = 0, len = data.inventories.invs.length; i < len; ++i) {
        inventory = data.inventories.invs[i];
        inventories += '<div><a href="' + Drupal.settings.basePath + 'inventory/' + inventory.id + '"><strong>' + inventory.name + ' [' + inventory.owner + ']:</strong></a></div><div>';
        for ( var ie = 0, ieLen = inventory.types.length; ie < ieLen; ++ie) {
          type = data.inventories.invs[i].types[ie];
          inventories += type.name + ' (' + type.count + ' x)';
          if (ieLen > ie + 1) {
            inventories += ', ';
          }
        }
        inventories += '</div>';
      }
      inventories += '<div><a href="' + Drupal.settings.basePath + 'inventory/new/area/' + overlay.id + '">+ Inventar erfassen</a></div>';
      me.areaInfo.updateTab(1, data.inventories.title, inventories);

      // habitats
      habitats = '<div>';
      if (data.habitats.habs) {
        habitats += '<ul>';
        for ( var i = 0, len = data.habitats.habs.length; i < len; ++i) {
          habitats += '<li><a href="' + Drupal.settings.basePath + 'habitat/'+data.habitats.habs[i].id+'">' +data.habitats.habs[i].name + '</a></li>';
        }
        habitats += '</ul>';
      }
      habitats += '</div>';
      habitats += '<div><a href="' + Drupal.settings.basePath + 'area/' + overlay.id + '/edit#area-habitats">+ Lebensräume bearbeiten</a></div>';
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
  
  /**
   * Create a Searchbar using autocomplete-feature by google places api
   */
  AreaSelect.prototype.createSearchbar = function() {
	    var control = document.createElement('div');
	    var input = document.createElement('input');
		control.appendChild(input);
		control.setAttribute('id', 'search_container');
		input.setAttribute('id', 'search_input');
		this.map.controls[google.maps.ControlPosition.TOP_LEFT].push(control);
		var ac = new google.maps.places.Autocomplete(input, {
			types : [ 'geocode' ]
		});
		ac.bindTo('bounds', this.map);
		var me = this;
		google.maps.event.addListener(ac, 'place_changed', function() {
			var place = ac.getPlace();
			if (place.geometry.viewport) {
				me.map.fitBounds(place.geometry.viewport);
			} else {
				me.map.setCenter(place.geometry.location);
				me.map.setZoom(17);
			}
		});
		google.maps.event.addListener(this.map, 'bounds_changed', function() {
			input.blur();
		});	
		
		input.onfocus = function(){
			input.value = '';
		}

		google.maps.event.addDomListener(input, 'keydown', function(e) {
			var key = e.keyCode ? e.keyCode : e.charCode;
            if (key == 13) { 
            	if (e.preventDefault) { 
            		e.preventDefault(); 
            	} else { 
            		e.cancelBubble = true; 
            		e.returnValue = false; 
            	}
            } 
		}); 
  }
  
  AreaSelect.prototype.initLocation = function() {
	  if (window.localStorage) {
		  var bounds = window.localStorage.getItem('swissmon_ne_lat');
		  if (bounds != null) {
			  this.mapOverlays.fitBoundsOnLoad = false;
			  var ls = window.localStorage;
			  var ne = new google.maps.LatLng(ls.getItem('swissmon_ne_lat'), ls.getItem('swissmon_ne_lng'));
			  var sw = new google.maps.LatLng(ls.getItem('swissmon_sw_lat'), ls.getItem('swissmon_sw_lng'));
			  bounds = new google.maps.LatLngBounds(sw,ne);
			  this.map.fitBounds(bounds);
		  }
	  }
  }

  //////////////////////// Class initialisation //////////////////////
  // Member variable initialisation
  var me = this; // references to the instance of AreaSelect
  me.map_id = map_id;
  me.map = this.createGoogleMaps();
  if(!me.map)
    return false;
  me.mapOverlays = this.createOverlays(me.map); // Overlays representing areas
  me.selected_area = null; // index of currently selected area
  if(typeof InfoBubble != "undefined") {
    me.areaInfo = this.createAreaInfo(me.map); // popup bubble shown if user clicks on an area
  }
  if (jQuery("#area-create-form").length) {
    me.overlayControl = this.createControl(me.map); // class to control drawing of new areas
  }
  
  getareasJSON();

  // register events
  jQuery("#show_areas").bind('click', this.onTableRowClicked);
  jQuery('#show_areas_wrapper tbody > tr').bind('mouseover mouseout', this.onTableRowHover);
  jQuery('.show_static_image').bind('mouseover mouseout', this.showStaticImage);
  jQuery('#area_table tbody td img').bind( 'click', this.onTableExpanderClicked);
  
  //store location whenever bounds change
  if (window.localStorage){
	  google.maps.event.addListener(me.map, 'bounds_changed', function() {
		  var b = me.map.getBounds();
		  var ne = b.getNorthEast();
		  var sw = b.getSouthWest();
		  window.localStorage.setItem('swissmon_ne_lat', ne.lat());
		  window.localStorage.setItem('swissmon_ne_lng', ne.lng());
		  window.localStorage.setItem('swissmon_sw_lat', sw.lat());
		  window.localStorage.setItem('swissmon_sw_lng', sw.lng());
	  });  
  }
};


function refresh_map_info(){
  if(areaselect.overlayControl.overlay != null) {

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
      areaselect.overlayControl.overlay.getLatLngs().forEach(function (position) {
          area_coords.push([position.lat(), position.lng()]);
      });
      area_coords = JSON.stringify(area_coords);
      
      jQuery('#edit-area-coords').val(area_coords);
      jQuery('#edit-area-type').val(areaselect.overlayControl.overlay.type);

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
  if(jQuery('#map_canvas').size()){
	  areaselect = new AreaSelect('map_canvas')
	  // tlevy 2011-11-30: bugfix: Suchfeld in der Map war bei einigen nicht sichtbar. if-Abfrage auskommentiert
//	  if (!jQuery('#map_canvas').data('areaid')) {
		  areaselect.createSearchbar();
		  areaselect.initLocation();
//	  } 
  }
    
  /* clear edit-id-area field. Solves the problem that if a user navigates back problem that
   * he can continue to new inv without actually having selected an area */
  jQuery('#edit-id-area').val('');
});