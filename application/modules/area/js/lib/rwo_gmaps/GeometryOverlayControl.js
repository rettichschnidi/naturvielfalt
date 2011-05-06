/**
 * @author Roger Wolfer
 * 
 * uses code from gmaps-utility-library-dev.geometrycontrols
 * url: http://code.google.com/p/gmaps-utility-library-dev/
 */

function GeometryOverlayControl(map, opt_opts){
	if(map){
		this.initGOC(map, opt_opts);
	}
}

GeometryOverlayControl.prototype.initGOC = function(map, opts){
	var me = this;
	me.name = "GeometryOverlayControl";
	me.map = map;
	me.mapCache = new MapCache();
	me.overlay = null;		//Overlay that is currently drawn or edited
	me.controls = {};		 //contains all controls added to the map.
	me.buttons = {};
	me.markers = [];
	me.eventListeners = {};
	me.div = null;
	me.stopFunc = function(){};
	
	me.options = {
		controlPostitionFloat:google.maps.ControlPosition.TOP_RIGHT, 
	    controlPosition:[0,0],
	    buttonWidth:'33',
	    buttonHeight:'33',
	    buttonBorder:'0',
	    buttonCursor:'pointer'
//	    infoWindowHtmlURL:"data/geometry_html_template.html",
//	    stylesheets:["styles/google.maps.base.css","styles/google.maps.ms_styles.css"],
//	    autoSave:true, //TODO have option to turn on autoSave for individual controls?
//	    cssId:"emmc-geom", //for generic components shared between multiple controls 
//	    debug:true   
	};

//	for (var at in me) {
//  		this[at] = me[at];
//  	}
	console.info(this.name + " init finished!");
	return me;
};

GeometryOverlayControl.prototype.setStopFunction = function(stopFunc){
  this.stopFunc = stopFunc;
};

GeometryOverlayControl.prototype.addControl = function(control){
	this.controls[control.name] = control;
	this.div.appendChild(control.div);
};

GeometryOverlayControl.prototype.removeControl = function(controlDiv){
	this.div.removeChild(controlDiv);
};

GeometryOverlayControl.prototype.createButton = function(requiredOpts){
  var me = this, opts = requiredOpts, Options = me.options;
  
  var button = {};
  button.opts = opts.buttonOpts;  
  var button_img = document.createElement('img');
  button_img.style.cursor = button.opts.buttonCursor || Options.buttonCursor;
  button_img.width = button.opts.buttonWidth || Options.buttonWidth;
  button_img.height = button.opts.buttonHeight || Options.buttonHeight;
  button_img.border = button.opts.buttonBorder || Options.buttonBorder;
  button_img.src = button.opts.img_up_url;
  //button_img.title = button.opts.tooltip;
    
  button.img = button_img;
 
  //Button toggle. First click turns it on (and other buttons off), triggers bound events. Second click turns it off
  google.maps.event.addDomListener(button.img, "click", function() { 
    if(button.img.getAttribute("src") === button.opts.img_up_url){
      me.toggleButtons(opts.controlName);
      opts.startDigitizing();
    } else {
      me.toggleButtons();
      opts.stopDigitizing();
    }    
  });  

	me.buttons[opts.controlName] = button;
//  me.stopDigitizingFuncs_[opts.controlName] = opts.stopDigitizing;
	console.info("Created: " + opts.controlName);
  return button;
};

GeometryOverlayControl.prototype.toggleButtons = function(button_name){
	console.info("toggle button: " + button_name);
  //Calls with no name will turn everything off. Calls with a name will turn all off except the named button
  for (var button in this.buttons) {
  	this.buttons[button].img.src = this.buttons[button].opts.img_up_url;
  }  
  if(button_name){
    this.buttons[button_name].img.src = this.buttons[button_name].opts.img_down_url;  
  }
  
  //turn off other digitizing listeners. Note: to avoid recursion, external calls to this function should always be made
  //without parameters!!!
//  if (button_name) {
//    for (var func in me.stopDigitizingFuncs_) {
//      if (func != button_name) {
//        me.stopDigitizingFuncs_[func](false);
//      }
//    }
//  }
};

GeometryOverlayControl.prototype.startDigitizing = function(){
	var me = this;
	var eventListener = google.maps.event.addListener(me.map, "click", me.addLatLngEvent(), true);
	this.eventListeners.addLatLng = eventListener;
};

GeometryOverlayControl.prototype.stopDigitizing = function(){
	if(this.eventListeners.addLatLng){
			google.maps.event.removeListener(this.eventListeners.addLatLng);
	}
	this.eventListeners.addLatLng = null;
	if(this.overlay){
		this.overlay.stopEditing();
	}
	this.stopFunc();
	//this.overlay = null;
};

GeometryOverlayControl.prototype.addLatLngEvent = function(){
  var me = this;
  return function(event){
    if(me.overlay == null){ // no point set
      me.overlay = new Marker(me.map);
      me.overlay.removeListener = function(){
        me.overlay = null;
      };
    } else if (me.overlay.type == 'marker'){ // second point added -> make polygon
      var latLng = me.overlay.getLatLng();
      me.overlay.marker.setMap(null);
      me.overlay = new Polygon(me.map);
      me.overlay.removeListener = function() {
        if(me.overlay.getLatLngs().getLength() == 1){
          var latLng = me.overlay.getLatLngs().getAt(0);
          me.overlay.polygon.setMap(null);
          me.overlay.removeAllMarkers();
          me.overlay = new Marker(me.map);
          me.overlay.removeListener = function(){
            me.overlay = null;
          };
          me.overlay.addLatLng(latLng);
          console.info('polygon removed');
        }
      };
      me.overlay.addLatLng(latLng);
    }
    me.overlay.addLatLng(event.latLng);
    console.info("query due to addlateve");
    refresh_map_info();
  };
};

GeometryOverlayControl.prototype.enableEditing = function(isEnabled){
	this.editing = isEnabled;
};

GeometryOverlayControl.prototype.save = function(overlay){
	this.mapCache.save(overlay);
};