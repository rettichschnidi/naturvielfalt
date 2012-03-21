  var default_localisation = new google.maps.LatLng(47.480988, 8.209748);
  var marker;
  var map;
  var temp;

  function initialize_map() {
    var mapOptions = {
      zoom: 13,
      mapTypeId: google.maps.MapTypeId.ROADMAP,
      center: default_localisation
    };

    map = new google.maps.Map(document.getElementById("map"),
            mapOptions);
    
    google.maps.event.addListener(map, 'click', addMarkerClick, true);
    
    
    // Try W3C Geolocation (Preferred)
    if(navigator.geolocation) {
      browserSupportFlag = true;
      navigator.geolocation.getCurrentPosition(function(position) {
        initialLocation = new google.maps.LatLng(position.coords.latitude,position.coords.longitude);
        map.setCenter(initialLocation);
      }, function() {
        handleNoGeolocation(browserSupportFlag);
      });
    // Try Google Gears Geolocation
    } else if (google.gears) {
      browserSupportFlag = true;
      var geo = google.gears.factory.create('beta.geolocation');
      geo.getCurrentPosition(function(position) {
        initialLocation = new google.maps.LatLng(position.latitude,position.longitude);
        map.setCenter(initialLocation);
      }, function() {
        handleNoGeoLocation(browserSupportFlag);
      });
    // Browser doesn't support Geolocation
    } else {
      browserSupportFlag = false;
      handleNoGeolocation(browserSupportFlag);
    }
    
    function handleNoGeolocation(errorFlag) {
      if (errorFlag == true) {
        initialLocation = default_localisation;
      } else {
        initialLocation = default_localisation;
      }
      map.setCenter(initialLocation);
    }

    
  }

  function toggleBounce() {

    if (marker.getAnimation() != null) {
      marker.setAnimation(null);
    } else {
      marker.setAnimation(google.maps.Animation.BOUNCE);
    }
  }
 
  function addMarkerClick(e){
	  addMarker(e.latLng);
  }
  
  function addMarker(e){
	  if(marker){
		  marker.setPosition(e);
	  }else{
	    marker = new google.maps.Marker({
	        map:map,
	        draggable:true,
	        animation: google.maps.Animation.DROP,
	        position: e
	      });
	  }
	  google.maps.event.addListener(marker, 'click', toggleBounce);
	  position = marker.getPosition();
	  jQuery('#coordinate_x').val(position.Ua);
	  jQuery('#coordinate_y').val(position.Va);
  }
  
  function setMarker(){
	  e = {};
	  e.Ua = jQuery('#coordinate_x').val();
	  e.Va = jQuery('#coordinate_y').val();
	  addMarker(e);
  }

  