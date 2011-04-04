/**
 * This function creates a autocomplete field, in which you must select a value from the dropdown.
 * @param textField The text field, where the user can enter the text.
 * @param idField A hidden input field, where the id will be saved.
 * @param source The source, where we can get the items for the autocomplete field. The source must return a JSON in the form [{id, label, ....}] 
 * @param onSelect This function will be executed, when an entry has been selected.
 * @return
 */

var util = {};

util.geocode = function(input_id, search_button_id, map) {
    var me = this;
    var geocoder = new google.maps.Geocoder();
    console.info('geocode init'+' #'+search_button_id);

    jQuery('#'+input_id).geo_autocomplete({
        geocoder_region: 'Schweiz',
        geocoder_types:  'natural_feature,street_address,route,intersection,locality,political,sublocality,neighborhood,country',
        maptype: 'roadmap',
        mapwidth: 200,
        select: function(_event, _ui) {
            if (_ui.item.viewport)
                map.fitBounds(_ui.item.viewport);
        }
    });

    //TODO funktion auch ausführen, wenn die enter taste betätigt wurde...
    jQuery('#'+search_button_id).click( function() {
        console.info('geocode');
        var term = jQuery('#'+input_id).val().trim();
        if(term.length > 3) {
            var geo_opts = {
                address: term,
                language: 'de',
                region: 'CH'
            };
            console.info(geo_opts);
            if (geocoder) {
                geocoder.geocode(geo_opts, function(results, status) {
                    if (status == google.maps.GeocoderStatus.OK) {
                        console.info(results);
                        map.fitBounds(results[0].geometry.viewport);

                    } else {
                        //TODO: Change alert!
                        alert("Geocoder failed due to: " + status);
                    }
                });
            }
        }

    });
};

jQuery.expr[':'].textEquals = function(a, i, m) {
    return jQuery(a).text().match("^" + m[3] + "$");
}

// Create an entry for every console function that is used.
function checkForConsole() {
    if(typeof console === "undefined") {
        console = {
            info: function() {
            },
            error: function() {
            }
        };
    }
};

checkForConsole();