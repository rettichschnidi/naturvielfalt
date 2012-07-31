/**
 * This is a Json stylesheet for the geometry overlays like markers and polygons
 */

var overlayStyle = {
    'polygon' : {
        strokeColor:    "#AA0000",
        strokeWeight:   1,
        strokeOpacity:  0.75,
        fillColor:      "#AA0000",
        fillOpacity:    0.25
    },

    'polygon-selected': {
        strokeColor:    "#0000FF",
        strokeWeight:   1,
        strokeOpacity:  0.75,
        fillColor:      "#0000FF",
        fillOpacity:    0.25
    },
    
    'polygon-highlighted': {
        strokeColor:    "#000000",
        strokeWeight:   1,
        strokeOpacity:  0.25,
        fillColor:      "#AA0000",
        fillOpacity:    0.4
    },

    'polyline': {
        strokeColor:    "#AA0000",
        strokeWeight:   2,
        strokeOpacity:  0.75
    },

    'polyline-selected': {
        strokeColor:    "#0000FF",
        strokeWeight:   2,
        strokeOpacity:  0.75
    },

    'marker' : {
        strokeColor:    "#AA0000",
        strokeOpacity:  0.75,
        strokeWeight:   1,
        fillColor:      "#AA0000",
        fillOpacity:    0.25,
        radius:         20
    },

    'marker-selected': {
        strokeColor:    "#0000FF",
        strokeOpacity:  0.75,
        strokeWeight:   1,
        fillColor:      "#0000FF",
        fillOpacity:    0.25,
        radius:         20
    },
    
    'marker-highlighted': {
        flat:           false,
        icon:           new google.maps.MarkerImage(Drupal.settings.basePath+'sites/all/modules/area/images/map_controls/marker_green.png'),
        raiseOnTrag:    false,
        //shadow:         '',
    }
};
