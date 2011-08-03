jQuery(function ($) {

    $('body.page-find .map-select').click(function () {
        $('body.page-find .map-overlay').css('left', '50%');
    });

    var latlng = new google.maps.LatLng(46.8, 8.233333); // CH
    var myOptions = {
        center: latlng,
        mapTypeId: google.maps.MapTypeId.ROADMAP,
        mapTypeControl: false
    };
    var map = new google.maps.Map(document.getElementById('map-canvas'), myOptions);

    var marker1;
    var marker2;
    var rectangle;

    
    var applyControl = document.createElement('div');
    applyControl.style.padding = '5px';
    applyControl.index = 1;

    var controlUI = document.createElement('div');
    $(controlUI).addClass('map-apply');
    controlUI.innerHTML = 'Gebiet übernehmen';
    applyControl.appendChild(controlUI);

    google.maps.event.addDomListener(controlUI, 'click', function() {

        var geo0 = marker1.getPosition().toUrlValue();
        var geo1 = marker2.getPosition().toUrlValue();

        var parameters = Drupal.settings.find.parameters;

        parameters['geo'] = [marker1.getPosition().toUrlValue(), marker2.getPosition().toUrlValue()];

        var url = Drupal.settings.find.url + '?' + $.param(parameters);
        window.location.href = url;

        $('.map-overlay').css('left', '-9000px');
    });

    map.controls[google.maps.ControlPosition.TOP_RIGHT].push(applyControl);

    // Plot two markers to represent the Rectangle's bounds.
    var coords = (Drupal.settings.find.geo[0] ? Drupal.settings.find.geo[0] : '47.67, 7.08').split(',');
    marker1 = new google.maps.Marker({
        map: map,
        position: new google.maps.LatLng(parseFloat(coords[0]), parseFloat(coords[1])),
        draggable: true,
        title: 'Ziehen, um das Gebiet zu ändern.'
    });
    var coords = (Drupal.settings.find.geo[1] ? Drupal.settings.find.geo[1] : '46.81, 9.62').split(',');
    marker2 = new google.maps.Marker({
        map: map,
        position: new google.maps.LatLng(parseFloat(coords[0]), parseFloat(coords[1])),
        draggable: true,
        title: 'Ziehen, um das Gebiet zu ändern.'
    });

    var bounds = new google.maps.LatLngBounds();
    bounds.extend(marker1.getPosition());
    bounds.extend(marker2.getPosition());
    map.fitBounds(bounds);

    // Allow user to drag each marker to resize the size of the Rectangle.
    google.maps.event.addListener(marker1, 'drag', redraw);
    google.maps.event.addListener(marker2, 'drag', redraw);

    rectangle = new google.maps.Rectangle({
        map: map,
        strokeColor: "#AA0000",
        strokeWeight: 1,
        strokeOpacity: 0.5,
        fillColor: "#AA0000",
        fillOpacity: 0.25
    });
    redraw();

    /**
     * Updates the Rectangle's bounds to resize its dimensions.
     */
    function redraw() {
        var latLngBounds = new google.maps.LatLngBounds(
            marker1.getPosition(),
            marker2.getPosition()
        );
        rectangle.setBounds(latLngBounds);
    }
});
