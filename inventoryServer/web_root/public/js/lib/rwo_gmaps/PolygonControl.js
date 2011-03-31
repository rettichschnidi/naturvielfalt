/**
 * @author Roger Wolfer
 */
PolygonControl.prototype = new GeometryOverlayControl();
PolygonControl.prototype.constructor = PolygonControl;

function PolygonControl(map, opts){
	if(map){
		this.initPgonC(map, opts);
	}
}
/**
 * Creates a button, that enables polygon drawing.
 */
PolygonControl.prototype.initPgonC = function(map, opts){
	var me = this;
	me.initGOC(map, opts);
	me.name = "PolygonControl";
	me.button = null;
	
	me.options.buttonOpts = {
	      img_up_url:'http://www.google.com/intl/en_us/mapfiles/ms/t/Bpu.png',
	      img_down_url:'http://www.google.com/intl/en_us/mapfiles/ms/t/Bpd.png',
	      name:'polygon',
	      tooltip:'Draw a shape'
	};
	me.div = document.createElement('div');
	me.div.id = me.name;
	me.button = me.createButton({
      controlName:	me.name,
      buttonOpts:	me.options.buttonOpts,
      startDigitizing:function(){
        me.startDigitizing();
      },
      stopDigitizing:function(t){
        me.stopDigitizing(t);
      }
  });
  console.info(me.name + "init finished!");
	me.div.appendChild(me.button.img);
};


//PolygonControl.prototype.enableEditing = function(){
//	this.editing = isEnabled;
//};


PolygonControl.prototype.addLatLngEvent = function(){
	var me = this;
	return function(event){
		me.addLatLng(event.latLng);
	};
};

PolygonControl.prototype.addLatLng = function(latLng){
	if(this.overlay == null){
		this.overlay = new Polygon(this.map);
	}
	this.overlay.addLatLng(latLng);
};

PolygonControl.prototype.deleteCoord = function(coord){
	this.overlay.deleteCoord(coord);	
};