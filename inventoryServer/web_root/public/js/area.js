/**
 * @author Roger Wolfer
 * This class is for creating an Area and handles the form input
 */
var map;

function Area() {
    var me = this;
    me.ajax = new MapAjaxProxy();
    google.maps.event.addDomListener(window,"load", function() {
        //create map
        var bruggCenter = new google.maps.LatLng(47.487084, 8.207273);
        // google maps options
        var myOptions = {
            zoom: 15,
            center: bruggCenter,
            mapTypeId: google.maps.MapTypeId.ROADMAP
        };
        var map = new google.maps.Map(document.getElementById('map_canvas'), myOptions);

        $("#habitat_id").attr('id', "habitat_id_0");
        me.habitatsCnt = 0;
        me.curHabitatId = $("#habitat_id_0");

        
        me.overlayControl = new GeometryOverlayControl(map);
        me.overlayControl.startDigitizing();

        // bring up the form
        $('#continue').click(function() {
          if(me.overlayControl.overlay != null){
            $('#surface_area').val(me.overlayControl.overlay.getArea());
            me.overlayControl.overlay.getAltitude( function(altitude) {
                $('#altitude').val(altitude);
            });
            me.overlayControl.overlay.getAddress( function(address) {
                $('#canton').val(address.canton);
                $('#township').val(address.township);
                $('#locality').val(address.locality);
                $('#zip').val(address.zip);
            });
            $('#area_create_form').toggle('slow');
            //this.val('Zurück...');
            return false;
          } else {
              //TODO: Change!
              alert("Bitte zuerst ein Gebiet eintragen.");
          }

        });
        
        util.geocode('map_search', 'map_search_button', map);

        //save and go on to create an inventory
        $('#save_to_inventory').click( function() {
            var area = me.formToJson();
            $.post("area-create/save", area, function(response) {
                //TODO: Change!
                console.info(response);
                if(response.id != 'undefined'){
                    window.location.href = 'inventory?area_id='+response.id;
                }
                alert('speichern erfolgreich\n' + response.id);
            });
        });

        //save and create new area
        $('#save_new_area').click( function() {
            var area = me.formToJson();
            $.post("area-create/save", area, function(response) {
                //TODO: Change!
                if(response.id != 'undefined'){
                    window.location.href = 'area-create';
                }
                alert('speichern erfolgreich\n' + response.id);
            });
        });

        // Habitat Autocomplete
        $("#habitat").autocomplete({
            source: "area-create/get-habitats",
            focus: function( event, ui ) {
                console.info(ui);
                $( "#habitat" ).val( ui.item.label+" "+ui.item.name );
                return false;
            },
            select: function(event, ui) {
                $('#habitat').val(ui.item.label+" "+ui.item.name);
                $('#habitat_id_'+me.habitatsCnt).val(ui.item.id);
                me.setHabitat(ui.item);
                return false;
            },
            minLength: 1,
            change: function (event, ui) {
                console.info('change');
                //if the value of the textbox does not match a suggestion, clear its value
                if ($(".ui-autocomplete li:textEquals('"+ $(this).val().replace(/([{}\(\)\^$&.\*\?\/\+\|\[\\\\]|\]|\-)/g, '\\$1') + "')").size() == 0) {
                    $(this).val('');
                    $('#habitat_id_'+me.habitatsCnt).val('');
                } else {
                    me.setHabitat(ui.item);
                }
            }
        }).live('keydown', function (e) {
            var keyCode = e.keyCode || e.which;
            //if TAB or RETURN is pressed and the text in the textbox does not match a suggestion, set the value of the textbox to the text of the first suggestion
            if((keyCode == 9 || keyCode == 13)) {
                if($(".ui-autocomplete li:textEquals('"+ $('#habitat').val().replace(/([{}\(\)\^$&.\*\?\/\+\|\[\\\\]|\]|\-)/g, '\\$1') + "')").size() == 0) {
                    $(this).val('');
                    $('#habitat_id_'+me.habitatsCnt).val('');

                } else {
                    var item = $("#habitat").data('autocomplete').selectedItem;
                    me.setHabitat(item);
                    $('#habitat').focus();
                }
            }
        }).data( "autocomplete" )._renderItem = function( ul, item ) {
            //highlighting of matches
            var re_label = new RegExp("^"+this.term, 'i');
            var term = this.term.replace(/[aou]/, function (m){
              // to highlight with term 'wasser' -> 'wasser' and 'gewässer'
              var hash = {
                'a' : '(ä|a)',
                'o' : '(ö|o)',
                'u' : '(ü|u)'
              };
              return hash[m];
            });
            var re_name = new RegExp(term, 'ig');
            var label = item.label.replace(re_label ,"<span class='ui-term-match'>$&</span>");
            var name = item.name.replace(re_name,"<span class='ui-term-match'>$&</span>");
            
            return $( "<li></li>" )
            .data( "item.autocomplete", item )
            .append( "<a><div class='ui-menu-item-label'>" + label + "</div><div class='ui-menu-item-name'>" + name + "</div></a>" )
            .appendTo( ul );
        };

    });

};

/**
 * Create Json Array from form values
 * @return Json
 */
Area.prototype.formToJson = function() {
    var me = this;
    var area_coords;
    var centroid;
 if(me.overlayControl.overlay != null) {
        area_coords = me.overlayControl.overlay.toJson();
        var centGLatLng = me.overlayControl.overlay.getCenter();
        centroid = {
          'lat':  centGLatLng.lat(),
          'lng':  centGLatLng.lng()
         };
    } else {
        //TODO: Change!
        alert('Kein Gebiet eingetragen!');
    }

    var habitat_ids = {};
    $.each($('input[name^="habitat_id"]'), function(i, val) {
        if(val.value !== '') {
            habitat_ids[i] = val.value;
        }
    });

    var area = {
        'field_name'    : $('#field_name').val(),
        'parcel_nr'     : $('#parcel_nr').val(),
        'altitude'      : $('#altitude').val(),
        'locality'      : $('#locality').val(),
        'zip'           : $('#zip').val(),
        'township'      : $('#township').val(),
        'canton'        : $('#canton').val(),
        'surface_area'  : $('#surface_area').val(),
        'centroid'      : centroid,
        'habitats'      : habitat_ids,
        'comment'       : $('#comment').val(),
        'area'          : area_coords
    };
    console.info(area);
    return area;
};

/**
 * Sets a habitat for area
 */
Area.prototype.setHabitat = function(item) {
    var me = this;
    var habitat = $('#habitat');
    var habitat_table = me.getHabitatTable(habitat);

    habitat_table.append("<tr id='habitat-tr_"+this.habitatsCnt+"'><td>"+item.label+"</td><td>"+item.name+"</td><td><img id='habitat-del_"+this.habitatsCnt+"' src='images/can_delete.png' /></td></tr>");

    $('#habitat-del_'+this.habitatsCnt).bind('click', {'entry': this.habitatsCnt}, (function(e) {
        $('#habitat-tr_'+e.data.entry).remove();
        $('#habitat_id_'+e.data.entry).remove();
    }));
    
    habitat.val('');
    var habitat_id = $('#habitat_id_'+me.habitatsCnt);
    me.habitatsCnt++;
    var new_habitat_id = habitat_id.clone();
    new_habitat_id.val('');
    new_habitat_id.attr('id', "habitat_id_"+me.habitatsCnt);
    habitat_id.after(new_habitat_id);
    me.curHabitatId = new_habitat_id;

};

/**
 * @return html table of habitats for the area
 */
Area.prototype.getHabitatTable = function(before) {
    if($('#habitat_table > tbody:last').length == 0) {
        before.before("<table id='habitat_table'><tbody></tbody></table>");
    }
    return $('#habitat_table > tbody:last');
};


new Area();