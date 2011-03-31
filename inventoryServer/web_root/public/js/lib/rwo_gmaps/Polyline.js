/**
 * @author Roger Wolfer
 */

	Polyline.prototype = new GeometryOverlay();
	Polyline.prototype.constructor = Polyline
	
function Polyline(map, opt_opts){
	this.initGO(map, opt_opts);
	this.prototype = new GeometryOverlay; //Inheritance
	this.type = 'polyline';
	this.name = this.type;
	
}

Polyline.prototype.initPline = function(map, opt_opts){
	
};
