/**
 * @author Roger Wolfer
 * 
 * uses code for deleting and editing from here: 
 * http://snipplr.com/view/38270/google-maps-api-v3--enableediting-polylines/
 */
function GeometryOverlay(map, opt_opts){
	
};

GeometryOverlay.prototype.initGO = function(map, opt_opts){
	this.type = 'geometryOverlay';
	this.name = this.type;
	this.id = null;
	this._altitude = null; //meters above sea
  this._area = null;   
  this._center = null; //google.maps.latlng
  this.address = null;
	this.map = map;
	this.zoomInterval = {min:0, max:19}; //for which zoomlevels should this overlay be shown (default: whole zoom interval 0..19)
	this.gOverlay = null; //contains the google maps overlay object
	this.attr = null; //contains all the attributes for a area like name, Kat. Nr., habitat and more...
	//this.tmpPolyLine.setOptions({strokeColor: "FFFFFF"});
};

GeometryOverlay.prototype.getOverlay = function(){
	return this.gOverlay;
};

GeometryOverlay.prototype.getZoomInterval = function(){
	return this.zoomInterval;
};

GeometryOverlay.prototype.setZoomInterval = function(min, max){
	if(min <= max 
		&& min >= Config.zoomInterval.min && min <= Config.zoomInterval.max
		&& max >= Config.zoomInterval.min && max <= Config.zoomInterval.max){
			this.zoomInterval.min = min;
			this.zoomInterval.max = max;
	} else {
		console.error("ZoomIntervalError: Min: %i Max: %i", new Array(min, max));
	}
};

GeometryOverlay.prototype.setAttr = function(attributes){
	this.attr = attributes;
};

GeometryOverlay.prototype.getAttr = function(){
	return this.attr;
};

GeometryOverlay.prototype.getCoords = function(){
	return this.coords;
};

/**
 * 
 * @param {Object} map
 * @param {Object} true: show overlay on the map, false: don't show overlay on the map (Default: true)
 */
GeometryOverlay.prototype.show = function(show){
	if(typeof show == undefined){
		var show = true;
	}
	if(this.overlay != 'undefined'){
		if(show == true){
			this.gOverlay.setMap(this.map);
		} else {
			this.gOverlay.setMap(null);
		}
	}
};


GeometryOverlay.prototype.getAltitude = function(func) {
    var elevator = new google.maps.ElevationService();
    var positionalRequest = {
      'locations' : new Array(this.getCenter())
    };

    if (elevator) {
        elevator.getElevationForLocations(positionalRequest, function(results, status) {
            if (status == google.maps.ElevationStatus.OK) {

                // Retrieve the first result
                if (results[0]) {
                    var altitude = parseInt(results[0].elevation + 0.5);
                    //User defined function
                    if(func){
                      func(altitude);
                    }
                } else {
                  //TODO: Change alert
                    alert("No results found");
                }
            } else {
              //TODO: Change alert
                alert("Elevation service failed due to: " + status);
            }
        });
    }
};

GeometryOverlay.prototype.setAltitude = function(altitude){
  this._altitude = parseInt(altitude + 0.5);
};



GeometryOverlay.prototype.getAddress = function(func) {
    var me = this;
    var geocoder = new google.maps.Geocoder();
    var latlng = me.getCenter();
    if (geocoder) {
        geocoder.geocode({'latLng': latlng, language: 'de'}, function(results, status) {
            if (status == google.maps.GeocoderStatus.OK) {

               /* //for testing only!
                for(var result in results) {
                    var string = "result: ";
                    for(var i in results[result].address_components) {
                        string += results[result].address_components[i].long_name+", "+results[result].address_components[i].short_name+" / ";
                    }
                    string += "type: "+results[result].types;
                    console.info(string);

                }*/
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
                console.info(address);
                func(address);
            } else {
                //TODO: Change alert!
                alert("Geocoder failed due to: " + status);
            }
        });
    }
};

GeometryOverlay.prototype.setAddress = function(address){
  this._address = address;
};
/**
 * OVERRIDE
 * @param {Object} coord
 */
GeometryOverlay.prototype.addCoord = function(latLng){};

/**
 * OVERRIDE
 * @param {Object} coords
 */
GeometryOverlay.prototype.setCoords = function(latLngs){};

/**
 * bla
 */
/**
 * OVERRIDE
 */
GeometryOverlay.prototype.toJson = function(){};
