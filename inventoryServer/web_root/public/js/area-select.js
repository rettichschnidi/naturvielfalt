/**
 * @author Roger Wolfer
 */
 
//var map;



function AreaSelect() {
    var me = this;

    google.maps.event.addDomListener(window,"load", function() {
        //create map
        var bruggCenter = new google.maps.LatLng(47.487084, 8.207273);
        var myOptions = {
            zoom: 15,
            center: bruggCenter,
            mapTypeId: google.maps.MapTypeId.ROADMAP
        };
        me.selected_area = null;
        me.map = new google.maps.Map(document.getElementById('map_canvas'), myOptions);
        // all overlays shown in the map
        me.mapOverlays = new MapGeometryOverlays(me.map);

        util.geocode('map_search', 'map_search_button', me.map);
        
        $('#new-area').click(function(){
            window.location.href = 'area-create';
        });

        $('#create-inventory').click(function(){
            if(me.selected_area != null){
               window.location.href = 'inventory?area_id='+me.selected_area;
            } else {
                //TODO: Change!
                alert('Bitte wählen Sie ein Gebiet aus.');
            }
        });

        // select row in table
        $("#area_table tbody").live('click', function(event) {
            $(me.area_table.fnSettings().aoData).each( function () {
                $(this.nTr).removeClass('row_selected');
                if(this.nTr.getAttribute('overlay_id')) {
                    me.mapOverlays.overlays[this.nTr.getAttribute('overlay_id')].setStyle('selected-disable');
                }
            });
            $(event.target.parentNode).addClass('row_selected');
            me.mapOverlays.overlays[event.target.parentNode.getAttribute('overlay_id')].setStyle('selected');
            me.selected_area = event.target.parentNode.getAttribute('overlay_id');
            me.map.panTo(me.mapOverlays.overlays[event.target.parentNode.getAttribute('overlay_id')].getCenter());
        });
        // hover row in table
        $('tbody > tr').live('mouseover mouseout', function(event) {
            if(event.type == 'mouseover') {
                $(event.target.parentNode).addClass( 'row_highlighted' );
                me.mapOverlays.overlays[event.target.parentNode.getAttribute('overlay_id')].setStyle('highlighted');
            } else {
                $(event.target.parentNode).removeClass('row_highlighted');
                me.mapOverlays.overlays[event.target.parentNode.getAttribute('overlay_id')].setStyle('highlighted-disable');
            }

        });
        
        // create table
        me.area_table = $('#area_table').dataTable( {
            "oLanguage" : {
                "sUrl" : "languages/de_DE/DataTables.txt"
            },
            "bScrollInfinite": true,
            "bScrollCollapse": true,
            "sScrollY": "400px",
            "bProcessing": true,
            "bServerSide": true,
            "sAjaxSource": "area-select/get-area",
            "fnServerData": function ( sSource, aoData, fnCallback ) {
                /* Add some data to send to the source, and send as 'POST' */
                //aoData.push( { "name": "my_field", "value": "my_value" } );
                $.ajax( {
                    "dataType": 'json',
                    "type": "POST",
                    "url": sSource,
                    "data": aoData,
                    "success": function (json) {
                        var aaData = [];
                        console.info(json);
                        var areas = json.areas;
                        me.mapOverlays.clear();
                        
                        for(var i in areas) {
                            aaData.push([
                              '<img src="images/details_open.png"></img>',
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
                        $(me.area_table.fnSettings().aoData).each( function () {
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
        $('#area_table tbody td img').live( 'click', function () {
            var nTr = this.parentNode.parentNode;
            if ( this.src.match('details_close') ) {
                /* This row is already open - close it */
                this.src = "images/details_open.png";
                me.area_table.fnClose( nTr );
            } else {
                /* Open this row */
                this.src = "images/details_close.png";
                me.area_table.fnOpen( nTr, me.formatDetails(nTr), 'details' );
            }
        } );
        
    });
};

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
        $('tr[overlay_id="'+info.id+'"]').addClass('row_highlighted');
        me.mapOverlays.overlays[info.id].setStyle('highlighted');
    });
    google.maps.event.addListener(overlay.gOverlay, 'mouseout', function(event) {
        $('tr[overlay_id="'+info.id+'"]').removeClass('row_highlighted');
        me.mapOverlays.overlays[info.id].setStyle('highlighted-disable');
    });

    //select
    google.maps.event.addListener(overlay.gOverlay, 'click', function(event) {
        var overlays = me.mapOverlays.overlays;
        for(id in overlays) {
            if(overlays[id].style == overlays[id].type+'-selected') {
                $('tr[overlay_id="'+id+'"]').removeClass('row_selected');
                overlays[id].setStyle('selected-disable');
            }
        }

        $('tr[overlay_id="'+info.id+'"]').addClass('row_selected');
        overlays[info.id].setStyle('selected');
        me.selected_area = info.id;
        me.map.panTo(overlays[info.id].getCenter());
    });
};

/**
 * Gets Areas from server and creates overlay in map
 * NOT USED ANYMORE!
 */
AreaSelect.prototype.getAreas = function(){
  var me = this;
  
  $.post('area-select/get-area', function(data){
    console.info(data);
    for(var i in data){
      if(data[i].type == 'polygon'){
        new Polygon(me.map, data[i]);
      } else if (data[i].type == 'marker'){
        console.info('Marker: blabla');
        new Marker(me.map, data[i]);
      } else{
        console.error('Type of overlay is undefined!');
      }
      
    }
  }
  , 'json');
  
};

/**
* Get the rows which are currently selected
*/
AreaSelect.prototype.getSelected = function( oTableLocal )
{
  var aReturn = new Array();
  var aTrs = oTableLocal.fnGetNodes();
  
  for ( var i=0 ; i<aTrs.length ; i++ )
  {
    if ( $(aTrs[i]).hasClass('row_selected') )
    {
      aReturn.push( aTrs[i] );
    }
  }
  return aReturn;
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

new AreaSelect();
