/**
 * @author Roger Wolfer
 * This class saves the overlays used in the map, also the ones not currently in use
 * and automatically loads overlays not in the cache.
 */
function MapCache(){
	this.ajaxProxy = new MapAjaxProxy();
	this.overlayCache = {};
	this.formCache = {};
}

MapCache.prototype.getOverlays = function(){
	//this will be very difficult to implement.
};

MapCache.prototype.getForm = function(overlayType){
	if(!this.formCache[overlayType]){
		this.formCache[overlayType] = this.ajaxProxy.getForm(overlayType);
	}
	return this.formCache[overlayType];
};

MapCache.prototype.save = function(overlay){
	var overlayJson = overlay.toJson();
	this.ajaxProxy.saveOverlay(overlayJson);
};
