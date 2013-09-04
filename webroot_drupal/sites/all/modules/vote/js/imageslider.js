$ = jQuery;

var mainImage;
var previousImages = new Array();
var nextImages = new Array();
var futureImages = new Array();
var imageSourceCache = new Array();
var stepsToMove;
var imageIndex = 0;

/**
	Executed after page was loaded.
*/
$(document).ready(function () {
	initializeImages();
	initializeCache();
	
	// load the first image sources from the cache
	mainImage.attr('src', imageSourceCache[0].imagePath);
	imageIndex = 1;
	loadImagesFromCache();
});

/**
	Adds the object references to the different arrays and variables.
*/
function initializeImages() {
	mainImage = $('#mainImage');
	previousImages[0] = $('#previousImage01');
	previousImages[1] = $('#previousImage02');
	previousImages[2] = $('#previousImage03');
	previousImages[3] = $('#previousImage04');
	nextImages[0] = $('#image01');
	nextImages[1] = $('#image02');
	nextImages[2] = $('#image03');
	nextImages[3] = $('#image04');
	futureImages[0] = $('#futureImage01');
	futureImages[1] = $('#futureImage02');
	futureImages[2] = $('#futureImage03');
	futureImages[3] = $('#futureImage04');
}

/**
	Loads the image paths into the cache.
*/
function initializeCache() {
	for (var i = 0; i < 14; i++) {
		imageSourceCache[i] = { imagePath: '/sites/all/modules/vote/img/test/image' + (i + 1) + '.jpg' };
	}
}

/**
	Loads the images from the cache using the imageIndex variable as index.
*/
function loadImagesFromCache() {
	for (var i = 0; i < nextImages.length; i ++) {
		var cleanIndex = checkIndex(imageSourceCache, imageIndex - nextImages.length + i); // imageIndex - nextImages.length - i - 1
		previousImages[i].attr('src', imageSourceCache[cleanIndex].imagePath);

		nextImages[i].attr('src', imageSourceCache[imageIndex + i].imagePath);
		
		cleanIndex = checkIndex(imageSourceCache, imageIndex + nextImages.length + i);
		futureImages[i].attr('src', imageSourceCache[cleanIndex].imagePath);
		
		previousImages[i].css({ width: 0, opacity: 0 });
		
		if (i < futureImages.length) {
			futureImages[i].css({ width: 0, opacity: 0 });
		}
		
		nextImages[i].css({ width: 190, opacity: 1 });
	}
}

/**
	Switches to the next image.
*/
function nextImage() {
	moveImages(1, true);
}

/**
	Checks if currently there are animations in progress and returns true if so.
*/
function finishedAllAnimations() {
	return $(".nextImages:animated, #mainImage:animated").length == 0;
}

/**
	Move the image index using the given number of steps (e.g. -4 moves four images back, 3 moves three images forward).
	If the boolan replaceMainImage is set to true, the main image will be replaced with the destination image.
*/
function moveImages(steps, replaceMainImage) {
	if (!finishedAllAnimations()) {
		return;
	}	

	stepsToMove = steps;
	
	animateMainImage(replaceMainImage);
}

/**
	Animates the main image replacement.
*/
function animateMainImage(replaceMainImage) {
	if (replaceMainImage) {
		// Fade main image out
		mainImage.animate({
				opacity: 0
			}, 500, function() {
				/**
					Animation complete of main image fade out
					Move the selected image to the main image position
				*/
				nextImages[stepsToMove - 1].animate({
						left: "-=" + (nextImages[stepsToMove - 1].position().left - mainImage.position().left),
						top: "-=" + (nextImages[stepsToMove - 1].position().top - mainImage.position().top)
					}, 500, function() {
						/**
							Animation complete of selected image movement
							Switch the main image source with the selected image source
						*/
						mainImage.attr("src", nextImages[stepsToMove - 1].attr("src"));
						// Calculate the new height of the image
						mainImage.css({ width: 400 });
						var autoHeight = mainImage.attr('height');
						// Show the main image and animate it to the normal size
						mainImage.css({ width: 190, height: "auto" });
						mainImage.css({ opacity: 1 });
						
						mainImage.animate({ width: 400 });
						$("#mainImageFieldset").animate({ height: autoHeight + 20 });
						// Reset position of the selected image
						nextImages[stepsToMove - 1].removeAttr("style");
						// check if the amount of steps is possible to do
						animateNextImages(replaceMainImage)
					}
				);
			}
		);
	} else {
		// check if the amount of steps is possible to do
		animateNextImages(replaceMainImage)
	}
}

/**
	Animates the next images.
*/
function animateNextImages(replaceMainImage) {

	// if we step below the beginning of the image cache, reduce the steps to move so we exactly hit the beginning
	if (imageIndex + stepsToMove < 0) {
		stepsToMove = -imageIndex;
	}
	
	// if we step over the end of the image cache, reduce the steps to move so we exactly hit the ending
	if (imageIndex + stepsToMove + nextImages.length >= imageSourceCache.length) {
		stepsToMove = imageSourceCache.length - nextImages.length - imageIndex;
	}
	
	// set the new image index to reload the images
	imageIndex += stepsToMove;

	var positive = stepsToMove >= 0;
	for (var i = 0; i < Math.abs(stepsToMove); i ++) {
		if (positive) {
			if (i < futureImages.length) {
				futureImages[i].animate({ width: 190, opacity: 1 }, 500);
			}
			if (i < nextImages.length) {
				nextImages[i].animate({ width: 0, opacity: 0 }, 500, function() {
					loadImagesFromCache();
				});
			}
		} else {
			if (i < previousImages.length) {
				var cleanIndex = checkIndex(previousImages, nextImages.length - i - 1);
				previousImages[cleanIndex].animate({ width: 190, opacity: 1 }, 500);
			}
			if (i < nextImages.length) {
				nextImages[nextImages.length - i - 1].animate({ width: 0, opacity: 0 }, 500, function() {
					loadImagesFromCache();
				});
			}
		}
	}
}

/**
	Checks if the index is below zero or above the array length and returns a valid index
*/
function checkIndex(array, index) {
	return index < 0 ? 0 : index >= array.length ? array.length - 1 : index;
}