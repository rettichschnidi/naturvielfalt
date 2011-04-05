/**
 * @author Roger Wolfer
 * This class handels all the ajax requests to the server.
 */
function MapAjaxProxy(){
	var requestId = this._getIdGenerator();
};

MapAjaxProxy.prototype.getOverlaysJson = function(coords, idsNotToLoad){
	
};

MapAjaxProxy.prototype.saveArea = function(overlayJson, callback){
	$.post("area/save", overlayJson, callback);
};

MapAjaxProxy.prototype._getIdGenerator = function(){
	var id = 0;
	return function(){
		return 'req:' + (id++);
	};
};

MapAjaxProxy.prototype.getHabitats = function(hab_start, callback){
  $.post("area/get-habitats", hab_start, callback);
};
