/**
 * @author Roger Wolfer
 */

MarkerControl.prototype = new GeometryOverlayControl();
MarkerControl.prototype.constructor = MarkerControl;

function MarkerControl(map, opts){
	if(map){
		this.initMC(map, opts);
	}

}

MarkerControl.prototype.initMC = function(map, opts){
	var me = this;
	me.initGOC(map, opts);
	me.name = "MarkerControl";
	me.button = null;
	
	me.options.buttonOpts = {
	      img_up_url:'http://www.google.com/intl/en_us/mapfiles/ms/t/Bmu.png',
	      img_down_url:'http://www.google.com/intl/en_us/mapfiles/ms/t/Bmd.png',
	      name:'marker',
	      tooltip:'Set a marker'
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
  me.div.appendChild(me.button.img);
};

MarkerControl.prototype.addLatLngEvent = function(){
	var me = this;
	return function(event){
		me.addLatLng(event.latLng);
	};
};

MarkerControl.prototype.addLatLng = function(latLng){
	if(this.overlay == null){
		this.overlay = new Marker(this.map);
	}
	this.overlay.addLatLng(latLng);
};

MarkerControl.prototype.deleteCoord = function(coord){
	this.overlay.deleteCoord(coord);	
};