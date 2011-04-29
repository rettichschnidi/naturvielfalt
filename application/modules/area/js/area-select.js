/**
 * @author Roger Wolfer
 */
 
var INVENTORY_SERVER_PATH = '../inventoryServer/web_root/public/';



function AreaSelect() {
	
	// Member variable declarations
	var me = this; // references to the instance of AreaSelect
	var bruggCenter = new google.maps.LatLng(47.487084, 8.207273);
	var selected_area = null; // index of currently selected area
	var areaInfo = null; // popup bubble shown if user clicks on an area
	var map = null; // google maps object
	var mapOverlays = null; // Overlays representing areas
	
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
    AreaSelect.prototype.onTableRowClicked = function(event) {
    	jQuery(areaselect.area_table.fnSettings().aoData).each(function() {
    		jQuery(this.nTr).removeClass('row_selected');
    		if (this.nTr.getAttribute('overlay_id')) {
    			areaselect.mapOverlays.overlays[this.nTr.getAttribute('overlay_id')].setStyle('selected-disable');
    		}
    	});
    	jQuery(event.target.parentNode).addClass('row_selected');
    	var overlayId = event.target.parentNode.getAttribute('overlay_id');
    	areaselect.mapOverlays.overlays[overlayId].setStyle('selected');
    	areaselect.selected_area = overlayId;
    	areaselect.map.panTo(areaselect.mapOverlays.overlays[overlayId].getCenter());
    }
    
    /**
     * This is called if the user hovers over a table row
     */
    AreaSelect.prototype.onTableRowHover = function(event) {
    	var parent = event.target.parentNode;
        if(event.type == 'mouseover') {
            jQuery(parent).addClass( 'row_highlighted' );
            me.mapOverlays.overlays[parent.getAttribute('overlay_id')].setStyle('highlighted');
        } else {
            jQuery(parent).removeClass('row_highlighted');
            me.mapOverlays.overlays[parent.getAttribute('overlay_id')].setStyle('highlighted-disable');
        }
    }
    
    /**
     * Creates the google maps object and attaches it to the element with the id 'map_canvas'.
     * Returns a google.maps.Map
     */
    AreaSelect.prototype.createGoogleMaps = function() {
    	var mapsOptions = { // Google maps options
    			zoom : 15,
    			center : bruggCenter,
    			mapTypeId : google.maps.MapTypeId.ROADMAP
    		};
    	return new google.maps.Map(document.getElementById('map_canvas'), mapsOptions);
    }
    
    /**
     * Creates the bubble which shows information on an area when the user clicks on a area.
     * Returns an InfoBubble
     */
    AreaSelect.prototype.createAreaInfo = function() {
    	areaInfo = new InfoBubble({
    		map : me.map,
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
    AreaSelect.prototype.createOverlays = function() {
    	var overlays = new MapGeometryOverlays(me.map);
    	return overlays;
    }
    
    this.map = this.createGoogleMaps();
	this.areaInfo = this.createAreaInfo();
	// all overlays shown in the map
	this.mapOverlays = this.createOverlays();

	util.geocode('map_search', 'map_search_button', me.map);

	jQuery('#create-inventory').click(onCreateInventoryClicked);


	// select row in table
	jQuery("#area_table tbody").live('click', this.onTableRowClicked);
	// hover row in table
	jQuery('tbody > tr').live('mouseover mouseout', this.onTableRowHover);
        
    // create table
    me.area_table = jQuery('#area_table').dataTable( {
        "oLanguage" : {
            "sUrl" : "modules/area/languages/de_DE/DataTables.txt"
        },
        "bScrollInfinite": true,
        "bScrollCollapse": true,
        "sScrollY": "400px",
        "bProcessing": true,
        "bServerSide": true,
        "sAjaxSource": INVENTORY_SERVER_PATH + "area-select/get-area",
        "fnServerData": function ( sSource, aoData, fnCallback ) {
            /* Add some data to send to the source, and send as 'POST' */
            //aoData.push( { "name": "my_field", "value": "my_value" } );
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
        },
        "aoColumns": [
        {"sClass": "center", "bSortable": false,  "bSearchable" : false},
        {"sTitle": "Flurname"},
        {"sTitle": "Ortschaft"},
        {"sTitle": "Gemeinde"},
        {"sTitle": "Kanton"},
        {"sTitle": "Fläche (m&#178;)"},
        {"sTitle": "M.ü.M"},
        {"sTitle": "Typ", "bSearchable" : false, "bSortable": false},
        {"sTitle": "ParzellenNr.", "bSearchable" : false, "bVisible": false},
        {"sTitle": "Lebensräume", "bSearchable" : false, "bVisible": false},
        {"sTitle": "Beschreibung", "bSearchable" : false, "bVisible": false},
        {"sTitle": "area_id", "bSearchable" : false, "bVisible": false}
        ],
        "aaSorting": [[1, 'asc']],
        'fnInitComplete': function (){
          this.fnSetFilteringDelay(250);
        }
    } );
        
    // detail view of table rows
    jQuery('#area_table tbody td img').live( 'click', this.onTableExpanderClicked );
};

/*
 * This is called if the user clicks on the button "create inventory".
 */
function onCreateInventoryClicked() {
	if (areaselect.selected_area != null) {
		window.location.href = 'inventory/new/' + areaselect.selected_area;
	} else {
		alert('Bitte wählen Sie ein Gebiet aus.');
	}
}

/**
 * adds a listener for selecting and hover to geometry overlays
 */
AreaSelect.prototype.addOverlayListener = function(overlay) {
    var me = this;
    var info = new Object({
        'id' : overlay.id
    });

    //hover
    google.maps.event.addListener(overlay.gOverlay, 'mouseover', function(event) {
        jQuery('tr[overlay_id="'+info.id+'"]').addClass('row_highlighted');
        me.mapOverlays.overlays[info.id].setStyle('highlighted');
    });
    google.maps.event.addListener(overlay.gOverlay, 'mouseout', function(event) {
        jQuery('tr[overlay_id="'+info.id+'"]').removeClass('row_highlighted');
        me.mapOverlays.overlays[info.id].setStyle('highlighted-disable');
    });

    //select
    google.maps.event.addListener(overlay.gOverlay, 'click', function(event) {
        var overlays = me.mapOverlays.overlays;
        for(id in overlays) {
            if(overlays[id].style == overlays[id].type+'-selected') {
                jQuery('tr[overlay_id="'+id+'"]').removeClass('row_selected');
                overlays[id].setStyle('selected-disable');
            }
        }
        
        // update and show area info
		jQuery.getJSON(Drupal.settings.basePath + 'area/json/' + info.id, function(data, textStatus, jqXHR) {
			// area details
			var details = '<div>';
			details += '<strong>' + data.field_name + '</strong>';
			details += '<div>' + data.locality + '<br>';
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
			habitats = '<div>';
			if (data.habitats.habs) {
				for ( var i = 0, len = data.habitats.habs.length; i < len; ++i) {
					habitats += data.habitats.habs[i] + '<br/>';
				}
			}
			habitats += '</div>';
			me.areaInfo.updateTab(2, data.habitats.title, habitats);

			// and finally show the fancy popup bubble:
			me.areaInfo.open(me.map);
		});
		me.areaInfo.close();
		me.areaInfo.setPosition(overlays[info.id].getCenter());
		
        jQuery('tr[overlay_id="'+info.id+'"]').addClass('row_selected');
        overlays[info.id].setStyle('selected');
        me.selected_area = info.id;
        me.map.panTo(overlays[info.id].getCenter());
    });
};

AreaSelect.prototype.formatDetails = function ( nTr ){
  var me = this;
  
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

var areaselect = null;
jQuery(document).ready(function() {
	areaselect = new AreaSelect();
});
