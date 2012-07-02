/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area-reticle.js
 * @note A lot o this code is taken from the Google Maps API Reference: https://developers.google.com/maps/documentation/javascript/overlays#CustomOverlays
 * 
 * Create a reticle at the center of the map.
 */

function ReticleOverlay(image, map) {
	// Now initialize all properties.
	this.image_ = image;
	this.map_ = map;

	// We define a property to hold the image's
	// div. We'll actually create this div
	// upon receipt of the add() method so we'll
	// leave it null for now.
	this.div_ = null;

	// Explicitly call setMap() on this overlay
	this.setMap(map);
}

ReticleOverlay.prototype = new google.maps.OverlayView();

ReticleOverlay.prototype.onAdd = function() {

	// Note: an overlay's receipt of onAdd() indicates that
	// the map's panes are now available for attaching
	// the overlay to the map via the DOM.

	// Create the DIV and set some basic attributes.
	var div = document.createElement('div');
	div.style.border = "none";
	div.style.borderWidth = "0px";
	div.style.position = "absolute";

	// Create an IMG element and attach it to the DIV.
	var img = document.createElement("img");
	img.src = this.image_;
	img.style.width = "24px";
	img.style.height = "24px";
	div.appendChild(img);

	// Set the overlay's div_ property to this DIV
	this.div_ = div;

	// We add an overlay to a map via one of the map's panes.
	// We'll add this overlay to the overlayImage pane.
	var panes = this.getPanes();
	panes.overlayLayer.appendChild(div);
};

ReticleOverlay.prototype.hide = function() {
	if (this.div_) {
		this.div_.style.visibility = "hidden";
	}
};

ReticleOverlay.prototype.show = function() {
	if (this.div_) {
		this.div_.style.visibility = "visible";
	}
};

ReticleOverlay.prototype.toggle = function() {
	if (this.div_) {
		if (this.div_.style.visibility == "hidden") {
			this.show();
		} else {
			this.hide();
		}
	}
};

ReticleOverlay.prototype.toggleDOM = function() {
	if (this.getMap()) {
		this.setMap(null);
	} else {
		this.setMap(this.map_);
	}
};

ReticleOverlay.prototype.draw = function() {
	// Size and position the overlay. We use a southwest and northeast
	// position of the overlay to peg it to the correct position and size.
	// We need to retrieve the projection from this overlay to do this.
	var overlayProjection = this.getProjection();

	// Retrieve the southwest and northeast coordinates of this overlay
	// in latlngs and convert them to pixels coordinates.
	// We'll use these coordinates to resize the DIV.
	var sw = overlayProjection
			.fromLatLngToDivPixel(this.map_.getBounds().getSouthWest());
	var ne = overlayProjection
			.fromLatLngToDivPixel(this.map_.getBounds().getNorthEast());

	// Resize the image's DIV to fit the indicated dimensions.
	var div = this.div_;
	div.style.left = sw.x + (ne.x - sw.x)/2 - 12 + 'px'; // MAGIC!!
	div.style.top = ne.y + (sw.y - ne.y)/2 - 12 + 'px';
	div.style.width = (ne.x - sw.x) + 'px';
	div.style.height = (sw.y - ne.y) + 'px';
};

ReticleOverlay.prototype.onRemove = function() {
	this.div_.parentNode.removeChild(this.div_);
	this.div_ = null;
};

jQuery(document).ready(function() {
	var srcImage = Drupal.settings.area.reticleimageurl;
	overlay = new ReticleOverlay(srcImage, areabasic.googlemap);
	google.maps.event.addListener(areabasic.googlemap, 'idle', function() { // change idle to center_changed or another event if needed
		overlay.draw();
	});
});
