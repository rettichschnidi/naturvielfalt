/**
 * This is a Json-Stylesheet for the Geometry Overlays like Marker and Polygon
 */
 


var overlayStyle = {
    'polygon' : {
        strokeColor:    "#000000",
        strokeWeight:   2,
        strokeOpacity:  0.25,
        fillColor:      "#FF0000",
        fillOpacity:    0.25
    },

    'polygon-selected': {
        strokeColor:    "#000000",
        strokeWeight:   2,
        strokeOpacity:  0.25,
        fillColor:      "#0000FF",
        fillOpacity:    0.75
    },
    
    'polygon-highlighted': {
        strokeColor:    "#000000",
        strokeWeight:   2,
        strokeOpacity:  0.25,
        fillColor:      "00FF00",
        fillOpacity:    0.6
    },
    'marker' : {
        flat:           false,
        icon:           new google.maps.MarkerImage('modules/area/images/map_controls/marker.png'),
        raiseOnTrag:    false,
        //shadow:         '',
    },

    'marker-selected': {
        flat:           false,
        icon:           new google.maps.MarkerImage('modules/area/images/map_controls/marker_blue.png'),
        raiseOnTrag:    false,
        //shadow:         '',
    },
    
    'marker-highlighted': {
        flat:           false,
        icon:           new google.maps.MarkerImage('modules/area/images/map_controls/marker_green.png'),
        raiseOnTrag:    false,
        //shadow:         '',
    }
};
