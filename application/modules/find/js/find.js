jQuery(function ($) {

    $('body.page-find .map-select').click(function () {
        $('body.page-find .map-overlay').css('left', '50%');
    });
    $('body.page-find .map-overlay .map-close').click(function () {
        $('.map-overlay').css('left', '-9000px');
    });

    var latlng = new google.maps.LatLng(46.8, 8.233333); // CH
    var myOptions = {
        center: latlng,
        mapTypeId: google.maps.MapTypeId.ROADMAP,
        maxZoom: 18,
        minZoom: 5,
        mapTypeControl: false,
        zoom: 8
    };
    var map = new google.maps.Map(document.getElementById('map-canvas'), myOptions);

    var overlay;
    
    var control = new GeometryControl(map, function () {
        if (overlay) {
            overlay.overlay.setMap(null);
        }
    });

    control.addPolygon(function () {

        var geo = [];
        control.overlay.overlay.getPath().forEach(function (position) {
            geo.push(position.toUrlValue());
        });

        var parameters = Drupal.settings.find.parameters;
        parameters['geo'] = geo;

        var url = Drupal.settings.find.url + '?' + $.param(parameters);
        window.location.href = url;

        $('.map-overlay').css('left', '-9000px');
    });

    // add overlay if geo is set
    if (Drupal.settings.find.geo.length > 1) {
        
        var path = [];
        for (var i = 0, l = Drupal.settings.find.geo.length; i < l; i++) {
            var values = Drupal.settings.find.geo[i].split(',');
            path.push(new google.maps.LatLng(values[0], values[1]));
        }

        overlay = new Polygon(map);
        overlay.overlay.setPath(path);
        map.fitBounds(overlay.getBounds());
    }
});
