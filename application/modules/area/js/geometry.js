
function GeometryControl(map, changed) {

    this.map = map;
    this.changed = changed;
    this.element = false; // active element

    // add edit buttons
    this.controls = jQuery('<div style="margin: 5px;"></div>');
    this.map.controls[google.maps.ControlPosition.TOP_LEFT].push(this.controls.get(0));
    
}

GeometryControl.prototype.reset = function (first) {

    // reset all icons
    jQuery('img', this.controls).each(function (i, img) {
        var $img = jQuery(img);
        $img.attr('src', Drupal.settings.basePath + 'modules/area/images/map_controls/' + $img.data('icon'));
    });
    
    // set focus to first element
    if (first) {
        var $img = jQuery('img:first-child', this.controls);
        $img.attr('src', Drupal.settings.basePath + 'modules/area/images/map_controls/' + $img.data('selected'));
    }

    // inform others
    this.changed();
};

GeometryControl.prototype.click = function (event) {

    this.reset();
    
    if (this.overlay) {
        this.overlay.clear();
    }

    // mark current icon as selected
    this.element = jQuery(event.target);
    this.element.attr('src', Drupal.settings.basePath + 'modules/area/images/map_controls/' + this.element.data('selected'));
    
    this.element.trigger('start');
};

GeometryControl.prototype.finish = function () {

    this.map.fitBounds(this.overlay.getBounds());
    if (this.map.getZoom() > 17) {
        this.map.setZoom(17);
    }

    this.reset(true);

    this.element.trigger('stop');
};

GeometryControl.prototype.add = function (title, icon, selected, start, stop) {
    
    var element = jQuery('<img title="' + title + '" src="' + Drupal.settings.basePath + 'modules/area/images/map_controls/' + icon + '" />');

    element.data('icon', icon);
    element.data('selected', selected);

    element.bind('click', jQuery.proxy(this.click, this));
    element.bind('start', jQuery.proxy(start, this));
    element.bind('stop', stop);

    this.controls.append(element);

    return element;
};

GeometryControl.prototype.addHand = function (callback) {
    this.hand = this.add('', 'hand.png', 'hand-selected.png', function () {}, callback);
};

GeometryControl.prototype.addMarker = function (callback) {
    this.marker = this.add('Gebiet zeichnen', 'markerCtl.png', 'markerCtl-selected.png', function () {
        this.overlay = new Marker(this.map);
        this.overlay.start(this);
    }, callback);
};

GeometryControl.prototype.addPath = function (callback) {
    this.path = this.add('Gebiet zeichnen', 'path.png', 'path-selected.png', function () {
        this.overlay = new Polyline(this.map);
        this.overlay.start(this);
    }, callback);
};

GeometryControl.prototype.addPolygon = function (callback) {
    this.polygon = this.add('Gebiet zeichnen', 'polygon.png', 'polygon-selected.png', function () {
        this.overlay = new Polygon(this.map);
        this.overlay.start(this);
    }, callback);
};

/**
 * GeometryOverlay
 */
function GeometryOverlays(map){
    this.map = map;
    this.bounds = new google.maps.LatLngBounds();
    this.overlays = []; //contains all the overlays currently shown on the map; key = overlay.id
}

GeometryOverlays.prototype.addOverlaysJson = function(json){

    var overlay;

    for (var i in json) {

        if (json[i].type == 'polygon') {

            overlay = new Polygon(this.map, json[i]);
            this.overlays[json[i].id] = overlay;

        } else if (json[i].type == 'polyline') {

            overlay = new Polyline(this.map, json[i]);
            this.overlays[json[i].id] = overlay;

        } else if (json[i].type == 'marker') {

            overlay = new Marker(this.map, json[i]);
            this.overlays[json[i].id] = overlay;

        } else {
            console.error('Unknown type of overlay!');
        }

        var latLngs = [];
        for (var k in json[i].area_points) {
            latLngs[k] = new google.maps.LatLng(json[i].area_points[k].lat, json[i].area_points[k].lng);
        }
        if (json[i].type == 'polygon' || json[i].type == 'polyline') {
            overlay.overlay.setPath(latLngs);
        } else if (json[i].type == 'marker') {
            overlay.overlay.setCenter(latLngs[0]);
        }

        var points = overlay.getLatLngs().getArray();
        for (var n = 0; n < points.length; n++) {
            this.bounds.extend(points[n]);
        }
    }
    this.map.fitBounds(this.bounds);
};

/**
 * Removes all Overlays from the Map
 */
GeometryOverlays.prototype.clear = function() {
    for (id in this.overlays){
        this.overlays[id].show(false);
    }
    this.overlays = [];
};

GeometryOverlays.prototype.selectOverlay = function(id) {
    console.info("select: " + id);
    this.overlays[id].setStyle('selected');
};

GeometryOverlays.prototype.highlightOverlay = function(id) {
    console.info("highlight: " + id);
    this.overlays[id].setStyle('highlighted');
};


/**
 * Abstract Geometry, implementation should provide this.overlay, this.type, addLatLng(), ?removeMarkers() and getCenter()
 */
function Geometry() {
     
}

Geometry.prototype.start = function (control) {

    this.control = control;
    this.map.setOptions({disableDoubleClickZoom: true, draggableCursor: 'crosshair'});

    this.listener = google.maps.event.addListener(this.map, 'click', jQuery.proxy(this.addLatLng, this));
    
    // remove all overlay listeners (just make overlays not clickable)
    for(var i in areaselect.mapOverlays.overlays) {
        var overlay = areaselect.mapOverlays.overlays[i];
        overlay.overlay.setOptions({clickable: false});
    }

    //deselect area if one was previously selected
    if (areaselect.selected_area){
        areaselect.mapOverlays.overlays[areaselect.selected_area].setStyle('selected-disable');
    }

    areaselect.areaInfo.close();

    this.hover = jQuery('<div style="index: 100; width: 200px; position: absolute; display: none; border: 1px solid #666; background-color: #fff; padding: 3px 6px 2px;"><span class="desc"></span><span class="distance" style="color: #666;"></span></div>');
    jQuery('body').append(this.hover);
    
    var that = this;

    jQuery(document).mousemove(function (e) {
        that.hover.css('left', e.pageX + 8);
        that.hover.css('top', e.pageY + 8);
        jQuery('.distance', that.hover).html('<br/>' + Math.ceil(that.distance) + 'm');
    });

    this.measure = new google.maps.Polyline({
        strokeColor:    "#AA0000",
        strokeWeight:   3,
        strokeOpacity:  0.25
    });
    this.measure.setMap(this.map);
    this.measure.setOptions({clickable: false});
    google.maps.event.addListener(this.map, 'mousemove', function (event) {
        if (that.last) {
            that.measure.setPath([that.last, event.latLng]);
            that.distance = google.maps.geometry.spherical.computeDistanceBetween(that.last, event.latLng);
        }
    });
};

Geometry.prototype.stop = function () {

    google.maps.event.removeListener(this.listener);

    if (this.removeMarkers) {
        this.removeMarkers();
    }

    this.hover.hide();
    this.measure.setMap(null);

    this.map.setOptions({disableDoubleClickZoom: false, draggableCursor: null});

    //after editing stops we have to enable our fancy listeners again
    for (var i in areaselect.mapOverlays.overlays) {
        var overlay = areaselect.mapOverlays.overlays[i];
        overlay.overlay.setOptions({clickable: true});
    }
};

/**
 * Return length of path as area value
 */
Geometry.prototype.getArea = function () {
    return Math.ceil(google.maps.geometry.spherical.computeLength(this.overlay.getPath()));
};

/**
 * Return altitude using Google elevation service
 */
Geometry.prototype.getAltitude = function (callback) {

    var request = {locations: [this.getCenter()]};

    var elevator = new google.maps.ElevationService();
    elevator.getElevationForLocations(request, function(results, status) {
        if (status == google.maps.ElevationStatus.OK) {
        
            // Retrieve the first result
            if (results[0]) {
                var altitude = parseInt(results[0].elevation + 0.5);
                callback(altitude);
            }
        }
    });
};

Geometry.prototype.getAddress = function (callback) {

    var geocoder = new google.maps.Geocoder();
    var latlng = this.getCenter();

    geocoder.geocode({'latLng': latlng, language: 'de'}, function(results, status) {
        if (status == google.maps.GeocoderStatus.OK) {

            var address = {};
            jQuery.each(results, function(index, result) {

                if(result.types == 'postal_code') {
                    var length = result.address_components.length;
                    address.zip = result.address_components[0].long_name;
                    address.locality = result.address_components[1].long_name;
                    address.canton = result.address_components[length-2].short_name;
                    address.country = result.address_components[length-1].long_name;
                }
                if(result.types == 'locality,political') {
                    address.township = result.address_components[0].long_name;
                }
            });

            console.debug(address);

            callback(address);

        } else {
            console.error("Geocoder failed due to: " + status);
        }
    });
};

/**
 * Returns LatLng at vertex position
 * @var int vertex position
 * @return google.maps.LatLng at position
 */
Geometry.prototype.getLatLng = function(latLngAt) {
    return this.overlay.getPath().getAt(latLngAt);
};

/**
 * Returns polygon path
 * @return google.maps.MVCArray<google.maps.LatLng>
 */
Geometry.prototype.getLatLngs = function(){
    return this.overlay.getPath();
};

Geometry.prototype.getBounds = function() {

    var path = this.overlay.getPath();

    var bounds = new google.maps.LatLngBounds();
    path.forEach(function (point) {
        bounds.extend(point);
    });

    return bounds;
};

Geometry.prototype.getCenter = function() {

    var path = this.overlay.getPath();
    var bounds = this.getBounds();

    return bounds.getCenter();
};

/**
 * Create a json from polygon
 * @return json
 */
Geometry.prototype.toJson = function(){
    var json = {
        type: this.type,
        attr: {},
        coords: {}
    }
    var latLngs = this.overlay.getPath();
    for (var i = 0; i < latLngs.getLength(); i++){
        json.coords[i] = {
            'lat' : latLngs.getAt(i).lat(),
            'lng': latLngs.getAt(i).lng()
        };
    }
    return json;
};

Geometry.prototype.clear = function () {
    this.stop();
    this.overlay.setMap(null);
};

/**
 * Set Style of geometry
 * @var style : selected, selected-disable, highlighted, highlighted-disable
 */
Geometry.prototype.setStyle = function(style) {

    switch(style) {
        case 'selected':
            this.style = this.type + '-selected';
            break;
        case 'selected-disable':
            if(this.style == this.type + '-selected') {
              this.style = this.type;
            }
            break;
        case 'highlighted':
            if(this.style != 'polygon-selected') {
              this.style = 'polygon-highlighted';
            }
            break;
        case 'highlighted-disable':
            if(this.style == 'polygon-highlighted') {
                this.style = 'polygon';
            }
            break;
        default:
            this.style = this.type;
    }
    this.overlay.setOptions(overlayStyle[this.style]);
};


/**
 * Polyline extends Geometry
 */
Polyline.prototype = new Geometry(); 
Polyline.prototype.constructor = Polyline;
function Polyline(map, opts) {

    this.type = 'polyline';

    this.id = opts && opts.id ? opts.id: null;

    this.map = map;

    this.overlay = new google.maps.Polyline(overlayStyle.polyline);
    this.overlay.setMap(this.map);

    this.markers = [];
}

Polyline.prototype.addLatLng = function (event) {

    this.hover.show();
    if (this.markers.length > 0) {
        jQuery('.desc', this.hover).text('Klicke auf den letzten Punkt, um die Linie abzuschliessen.');
    } else {
        jQuery('.desc', this.hover).text('Klicke, um die Linie zu zeichnen.');
    }
    this.last = event.latLng;

    this.overlay.getPath().push(event.latLng);

    var imageNormal = new google.maps.MarkerImage(
        Drupal.settings.basePath + "modules/area/js/lib/rwo_gmaps/images/square.png",
        new google.maps.Size(11, 11),
        new google.maps.Point(0, 0),
        new google.maps.Point(6, 6)
    );
    var imageHover = new google.maps.MarkerImage(
        Drupal.settings.basePath + "modules/area/js/lib/rwo_gmaps/images/square_over.png",
        new google.maps.Size(11, 11),
        new google.maps.Point(0, 0),
        new google.maps.Point(6, 6)
    );
    
    var marker = new google.maps.Marker({
        position: event.latLng,
        map: this.map,
        icon: imageNormal,
        draggable: true
    });

    // hover icon
    google.maps.event.addListener(marker, "mouseover", function () {
        marker.setIcon(imageHover);
    });
    google.maps.event.addListener(marker, "mouseout", function () {
        marker.setIcon(imageNormal);
    });

    var that = this;

    // enable dragging the markers
    google.maps.event.addListener(marker, "drag", function () {
        for (var m = 0, l = that.markers.length; m < l; m++) {
            if (that.markers[m] == marker) {
                that.overlay.getPath().setAt(m, marker.getPosition());
                break;
            }
        }
    });

    // remove marker on click
    google.maps.event.addListener(marker, "click", function () {
        for (var m = 0, l = that.markers.length; m < l; m++) {
            if (that.markers[m] == marker) {
                
                if (l - 1 == m && m > 0) {

                    // clicked on last marker
                    that.stop();
                    that.control.finish();

                } else {

                    // clicked on any marker
                    marker.setMap(null);
                    that.markers.splice(m, 1);
                    that.overlay.getPath().removeAt(m);
                }
                break;
            }
        }
    });

    this.markers.push(marker);
};

Polyline.prototype.removeMarkers = function() {
    for (var m = 0, l = this.markers.length; m < l; m++) {
        this.markers[m].setMap(null);
    }
};

/**
 * Use center of two middle points as polyline center.
 */
Polyline.prototype.getCenter = function() {

    var path = this.overlay.getPath();
    var middle = Math.floor(path.getLength() / 2);

    var bounds = new google.maps.LatLngBounds();
    bounds.extend(path.getAt(middle));
    bounds.extend(path.getAt(middle - 1));

    return bounds.getCenter();
};


/**
 * Polygon extends Geometry
 */
Polygon.prototype = new Geometry(); 
Polygon.prototype.constructor = Polygon;
function Polygon(map, opts) {

    this.type = 'polygon';

    this.id = opts && opts.id ? opts.id: null;
    this.map = map;

    this.removeListener = function(){};
    this.overlay = new google.maps.Polygon(overlayStyle.polygon);

    this.markers = []; // markers at the vertexes to edit/delete the vertex of a polygon
    this.vmarkers = []; // markers on line between vertexes
    this.tmpPolyLine = new google.maps.Polyline(); // if marker is draged a polyline is shown
    this.tmpPolyLine.setMap(this.map);

    this.polyline = new google.maps.Polyline({
        strokeColor:    "#AA0000",
        strokeWeight:   1,
        strokeOpacity:  0.75
    });

    this.polyline.setMap(this.map);
    this.overlay.setMap(this.map);
};

/**
 * Add vertex to polygon
 * @var latLng google.maps.LatLng new vertex
 * @var marker_on if true, vertex is editable
 */
Polygon.prototype.addLatLng = function(event) {

    this.hover.show();
    if (this.markers.length > 0) {
        jQuery('.desc', this.hover).text('Klicke auf den Anfangspunkt, um das Gebiet abzuschliessen.');
    } else {
        jQuery('.desc', this.hover).text('Klicke, um das Gebiet zu zeichnen.');
    }
    this.last = event.latLng;

    var latLng = event.latLng;
    
    this.polyline.setMap(this.map);
    this.polyline.getPath().push(latLng);
    this.overlay.getPath().push(latLng);

    this.overlay.setMap(null); // hide polygon while editing
    this.createControlMarker(latLng);
    if (this.markers.length != 1) {
        this.createVMarker(latLng);
    }
};

/**
 * Create control markers for path to edit vertexes of polygon
 * @var latLngs array<google.maps.LatLng>
 */
Polygon.prototype.createControlMarkers = function(latLngs){

    this.markers.length = 0;

    for (latLng in latLngs) {
        this.createControlMarker(latLng);
    }
};

/**
 * Create control marker for a vertex at specified coordinate
 * @var latLng google.maps.LatLng point at which a control marker is created
 */
Polygon.prototype.createControlMarker = function(latLng) {

    var hasmoved = false;

    var imageNormal = new google.maps.MarkerImage(
        Drupal.settings.basePath + "modules/area/js/lib/rwo_gmaps/images/square.png",
        new google.maps.Size(11, 11),
        new google.maps.Point(0, 0),
        new google.maps.Point(6, 6)
    );
    var imageHover = new google.maps.MarkerImage(
        Drupal.settings.basePath + "modules/area/js/lib/rwo_gmaps/images/square_over.png",
        new google.maps.Size(11, 11),
        new google.maps.Point(0, 0),
        new google.maps.Point(6, 6)
    );
    var marker = new google.maps.Marker({
        position: latLng,
        map: this.map,
        icon: imageNormal,
        draggable: true
    });
    google.maps.event.addListener(marker, "mouseover", function() {
        marker.setIcon(imageHover);
    });
    google.maps.event.addListener(marker, "mouseout", function() {
        marker.setIcon(imageNormal);
    });
    
    var that = this;

    google.maps.event.addListener(marker, "drag", function() {
        for (var m = 0; m < that.markers.length; m++) {
            if (that.markers[m] == marker) {
                that.overlay.getPath().setAt(m, marker.getPosition());
                that.moveVMarker(m);
                break;
            }
        }
        m = null;
        hasmoved = true;
    });

    google.maps.event.addListener(marker, "click", function() {

        if (that.markers[0] == marker) {
            
            // polygon closed
            that.polyline.setMap(null); // hide line
            that.overlay.setMap(that.map); // show polygon

            // finish
            that.stop();
            that.control.finish();

        } else {
            that.addLatLng(marker.getPosition());
            console.info("planning to query due to click");
        }
    });

    this.markers.push(marker);

    return marker;
};

/**
 * Show/hide control markers
 * @var show true: show control markers, false: hide control markers
 */
Polygon.prototype.showControlMarkers = function(show){
    for(i in this.markers){
        this.showControlMarker(this.markers[i], show);
    }
};

/**
 * Show/hide control marker
 * @var marker google.maps.Marker marker to show
 */
Polygon.prototype.showControlMarker = function(marker, show){

    if (typeof show == undefined) {
        var show = true;
    }

    if (show) {
        marker.setMap(this.map);
    } else {
        marker.setMap(null);
    }
};

/**
 * Create Marker between last vertex and current vertex
 * @var latLng google.maps.LatLng current vertex
 */
Polygon.prototype.createVMarker = function(latLng) {
    var me = this;
    var prevpoint = me.markers[me.markers.length-2].getPosition();
    var imageNormal = new google.maps.MarkerImage(
        Drupal.settings.basePath + "modules/area/js/lib/rwo_gmaps/images/square_transparent.png",
        new google.maps.Size(11, 11),
        new google.maps.Point(0, 0),
        new google.maps.Point(6, 6)
    );
    var imageHover = new google.maps.MarkerImage(
        Drupal.settings.basePath + "modules/area/js/lib/rwo_gmaps/images/square_transparent_over.png",
        new google.maps.Size(11, 11),
        new google.maps.Point(0, 0),
        new google.maps.Point(6, 6)
    );
    var marker = new google.maps.Marker({
        position: new google.maps.LatLng(
            latLng.lat() - (0.5 * (latLng.lat() - prevpoint.lat())),
            latLng.lng() - (0.5 * (latLng.lng() - prevpoint.lng()))
        ),
        map: me.map,
        icon: imageNormal,
        draggable: true
    });
    google.maps.event.addListener(marker, "mouseover", function() {
        marker.setIcon(imageHover);
    });
    google.maps.event.addListener(marker, "mouseout", function() {
        marker.setIcon(imageNormal);
    });
    google.maps.event.addListener(marker, "dragstart", function() {
        for (var m = 0; m < me.vmarkers.length; m++) {
            if (me.vmarkers[m] == marker) {
                var tmpPath = me.tmpPolyLine.getPath();
                tmpPath.push(me.markers[m].getPosition());
                tmpPath.push(me.vmarkers[m].getPosition());
                tmpPath.push(me.markers[m+1].getPosition());
                break;
            }
        }
        m = null;
    });
    google.maps.event.addListener(marker, "drag", function() {
        for (var m = 0; m < me.vmarkers.length; m++) {
            if (me.vmarkers[m] == marker) {
                me.tmpPolyLine.getPath().setAt(1, marker.getPosition());
                break;
            }
        }
        m = null;
    });
    google.maps.event.addListener(marker, "dragend", function() {
        for (var m = 0; m < me.vmarkers.length; m++) {
            if (me.vmarkers[m] == marker) {
                var newpos = marker.getPosition();
                var startMarkerPos = me.markers[m].getPosition();
                var firstVPos = new google.maps.LatLng(
                    newpos.lat() - (0.5 * (newpos.lat() - startMarkerPos.lat())),
                    newpos.lng() - (0.5 * (newpos.lng() - startMarkerPos.lng()))
                );
                var endMarkerPos = me.markers[m+1].getPosition();
                var secondVPos = new google.maps.LatLng(
                    newpos.lat() - (0.5 * (newpos.lat() - endMarkerPos.lat())),
                    newpos.lng() - (0.5 * (newpos.lng() - endMarkerPos.lng()))
                );
                var newVMarker = me.createVMarker(secondVPos);
                newVMarker.setPosition(secondVPos);//apply the correct position to the vmarker
                var newMarker = me.createControlMarker(newpos);
                me.markers.splice(m+1, 0, newMarker);
                me.overlay.getPath().insertAt(m+1, newpos);
                me.polyline.getPath().insertAt(m+1, newpos);
                marker.setPosition(firstVPos);
                me.vmarkers.splice(m+1, 0, newVMarker);
                me.tmpPolyLine.getPath().removeAt(2);
                me.tmpPolyLine.getPath().removeAt(1);
                me.tmpPolyLine.getPath().removeAt(0);
                newpos = null;
                startMarkerPos = null;
                firstVPos = null;
                endMarkerPos = null;
                secondVPos = null;
                newVMarker = null;
                newMarker = null;
                break;
            }
        }
        console.info("planning to query due to dragend of vmarker");
        refresh_map_info();
    });
    me.vmarkers.push(marker);
    return marker;
};

/**
 * moves vmarker at position index and creates a new polygon vertex
 * @var index nth vmarker
 */
Polygon.prototype.moveVMarker = function(index) {
    var me = this;
    var newpos = me.markers[index].getPosition();
    if (index != 0) {
        var prevpos = me.markers[index-1].getPosition();
        me.vmarkers[index-1].setPosition(new google.maps.LatLng(
            newpos.lat() - (0.5 * (newpos.lat() - prevpos.lat())),
            newpos.lng() - (0.5 * (newpos.lng() - prevpos.lng()))
        ));
        prevpos = null;
    }
    if (index != me.markers.length - 1) {
        var nextpos = me.markers[index+1].getPosition();
        me.vmarkers[index].setPosition(new google.maps.LatLng(
            newpos.lat() - (0.5 * (newpos.lat() - nextpos.lat())), 
            newpos.lng() - (0.5 * (newpos.lng() - nextpos.lng()))
        ));
        nextpos = null;
    }
    newpos = null;
    index = null;
};

/**
 * Remove vmarker at index.
 * @var index of vmarker
 */
Polygon.prototype.removeVMarkers = function(index) {
    var me = this;
    if (me.markers.length > 0) {//clicked marker has already been deleted
        if (index != me.markers.length) {
            me.vmarkers[index].setMap(null);
            me.vmarkers.splice(index, 1);
        } else {
            me.vmarkers[index-1].setMap(null);
            me.vmarkers.splice(index-1, 1);
        }
    }
    if (index != 0 && index != me.markers.length) {
        var prevpos = me.markers[index-1].getPosition();
        var newpos = me.markers[index].getPosition();
        me.vmarkers[index-1].setPosition(new google.maps.LatLng(
            newpos.lat() - (0.5 * (newpos.lat() - prevpos.lat())),
            newpos.lng() - (0.5 * (newpos.lng() - prevpos.lng()))
        ));
        prevpos = null;
        newpos = null;
    }
    index = null;
};

Polygon.prototype.clear = function () {
    this.stop();
    this.polyline.setMap(null);
    this.overlay.setMap(null);
};

/**
 * Removes all edit markers. Polygon isn't editable afterwards.
 */
Polygon.prototype.removeMarkers = function(){
    var me = this;
    for(var i = 0; i < me.markers.length; i++){
        me.markers[i].setMap(null);
    }
    for(var i = 0; i < me.vmarkers.length; i++){
        me.vmarkers[i].setMap(null);
    }
    me.markers.length = 0;
    me.vmarkers.length = 0;
};


/**
 * Marker extends Geometry
 */
Marker.prototype = new Geometry(); 
Marker.prototype.constructor = Marker;
function Marker(map, opts) {

    this.type = 'marker';

    this.id = opts && opts.id ? opts.id: null;

    this.map = map;

    this.overlay = new google.maps.Circle(overlayStyle.marker);
    this.overlay.setMap(this.map);
}

Marker.prototype.addLatLng = function (event) {

    this.overlay.setCenter(event.latLng);

    // finish
    this.stop();
    this.control.finish();
};

/**
 * Use center of two middle points as polyline center.
 */
Marker.prototype.getCenter = function() {
    return this.overlay.getCenter();
};

Marker.prototype.getBounds = function() {
    return this.overlay.getBounds();
};

Marker.prototype.getArea = function() {
    return 1;
};

Marker.prototype.getLatLngs = function() {
    return new google.maps.MVCArray([this.overlay.getCenter()]);
};

