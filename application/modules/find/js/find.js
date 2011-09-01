jQuery(function ($) {

    $('body.page-find .columns-overlay ul').sortable();

    $('body.page-find .columns-select').click(function () {
        $('body.page-find .columns-overlay').toggle();
        return false;
    });
    

    $('body.page-find .filter-selector').change(function () {
        $('body.page-find .filter-' + $(this).val()).slideDown();
        $('body.page-find .filter-selector option[value="'+$(this).val()+'"]').attr('disabled', 'disabled');
        $(this).val('');
     });
    $('body.page-find .clear').click(function () {
        $(this).closest('div').slideUp();
        window.location = $(this).attr('href');
        return false;
     });

    var map;
    var latlng = new google.maps.LatLng(46.8, 8.233333); // CH
    $('body.page-find .map-select').click(function () {
        $('body.page-find .map-overlay').css('left', '50%');
        google.maps.event.trigger(map, 'resize');
        map.setCenter(latlng);
    });
    $('body.page-find .map-overlay .map-close').click(function () {
        $('.map-overlay').css('left', '-9000px');
    });

    var myOptions = {
        center: latlng,
        mapTypeId: google.maps.MapTypeId.ROADMAP,
        maxZoom: 18,
        minZoom: 5,
        mapTypeControl: false,
        zoom: 8
    };
    map = new google.maps.Map(document.getElementById('map-canvas'), myOptions);

    var overlay;
    
    var control = new GeometryControl(map, function () {
        if (overlay) {
            overlay.overlay.setMap(null);
        }
    });

    control.addPolygon(function () {

        // don't hide polygon
        control.overlay.overlay.setMap(map);

        var geo = [];
        control.overlay.overlay.getPath().forEach(function (position) {
            geo.push(position.toUrlValue());
        });

        var parameters = Drupal.settings.find.parameters;
        parameters['geo'] = geo;

        var url = Drupal.settings.find.url + '?' + $.param(parameters);
        window.location.href = url;
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
