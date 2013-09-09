$ = jQuery;

var mainImage;
var currentMainImageIndex;
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
	$.ajax({
		url: "vote/getdata/json",
		success: function(result){
			for (var i = 0; i < result.length; i++) {
				imageSourceCache[i] = {
					imageId: result[i].imageId,
					imageThumbPath: result[i].imageThumbPath,
					imagePath: result[i].imagePath,
					images: result[i].images,
					imagesCount: result[i].images.images.length
				};
			}
			// load the first image sources from the cache
			mainImage.attr('src', imageSourceCache[imageIndex].imagePath);
			
			// write the current number of images to the diashow link
			$('#numberOfImages').html(imageSourceCache[imageIndex].imagesCount);
			
			currentMainImageIndex = imageIndex;
			prepareSlideShow();
			
			initLightBox();
			
			var autoHeight = mainImage.attr('height');
			$("#mainImageFieldset").animate({ height: autoHeight + 20 });
			
			imageIndex = 1;
			loadImagesFromCache();
		},
		error: function(result){
			alert("error");
		}
	});
}

function initLightBox() {
	var galleryLightboxSettings = {
		captionSelector : ".caption",
		captionAttr : false,
		captionHTML : true,
	};
	$('a.lightbox').lightBox(); // Select all links with lightbox class
}

/**
	Loads the images from the cache using the imageIndex variable as index.
*/
function loadImagesFromCache() {
	for (var i = 0; i < nextImages.length; i ++) {
		
		// load previous thumbnail and fullsize images into cache
		var cleanIndex = checkIndex(imageIndex - nextImages.length + i);
		previousImages[i].attr('src', imageSourceCache[cleanIndex].imageThumbPath);
		$('#mainImageContainer').append("<a href=\"" + imageSourceCache[cleanIndex].imagePath + "\" style=\"display: none;\"><img src=\"" + imageSourceCache[cleanIndex].imagePath + "\" alt=\"Image\" /></a>");

		// load next thumbnail and fullsize images into cache
		cleanIndex = checkIndex(imageIndex + i);
		nextImages[i].attr('src', imageSourceCache[cleanIndex].imageThumbPath);
		$('#mainImageContainer').append("<a href=\"" + imageSourceCache[cleanIndex].imagePath + "\" style=\"display: none;\"><img src=\"" + imageSourceCache[cleanIndex].imagePath + "\" alt=\"Image\" /></a>");
		
		// load future thumbnail and fullsize images into cache
		cleanIndex = checkIndex(imageIndex + nextImages.length + i);
		futureImages[i].attr('src', imageSourceCache[cleanIndex].imageThumbPath);
		$('#mainImageContainer').append("<a href=\"" + imageSourceCache[cleanIndex].imagePath + "\" style=\"display: none;\"><img src=\"" + imageSourceCache[cleanIndex].imagePath + "\" alt=\"Image\" /></a>");
		
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
	return $(".nextImages:animated, #mainImage:animated, #mainImageFieldset:animated, #mainImageContainer:animated").length == 0;
}

/**
 * Prepares the images for the lightbox pop-up slideshow.
 */
function prepareSlideShow() {
	// change the ID of the current image onclick handler
	$('#mainImageLink').attr("href", imageSourceCache[currentMainImageIndex].imagePath);
	
	// load slideshow images for current main image
	$('#slideshowImages').html('');
	for (var j = 1; j < imageSourceCache[currentMainImageIndex].imagesCount; j ++) {
		var currentPath = '/gallery/observation/' + imageSourceCache[currentMainImageIndex].imageId + '/thumb/' + imageSourceCache[currentMainImageIndex].images.images[j].id + '/fullsize';
		$('#slideshowImages').append("<a class=\"lightbox\" href=\"" + currentPath + "\" style=\"display: none;\"><img src=\"" + currentPath + "\" alt=\"Image\" /></a>");
	}
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
	currentMainImageIndex = checkIndex(imageIndex + stepsToMove - 1);
	
	if (replaceMainImage) {
		// Fade main image out
		$("#mainImageContainer").animate({
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
						mainImage.attr("src", imageSourceCache[currentMainImageIndex].imagePath);
						
						// write the current number of images to the diashow link
						$('#numberOfImages').html(imageSourceCache[currentMainImageIndex].imagesCount);
						
						prepareSlideShow();
						
						initLightBox();
						
						// Calculate the new height of the image
						mainImage.css({ width: 400 });
						var autoHeight = mainImage.attr('height');
						// Show the main image and animate it to the normal size
						mainImage.css({ width: 190, height: "auto" });
						$("#mainImageContainer").css({ opacity: 1 });
						
						mainImage.animate({ width: 400 });
						$("#mainImageFieldset").animate({ height: autoHeight + 20 });
						// Reset position of the selected image
						nextImages[stepsToMove - 1].removeAttr("style");
						// check if the amount of steps is possible to do
						animateNextImages(replaceMainImage);
					}
				);
			}
		);
	} else {
		// check if the amount of steps is possible to do
		animateNextImages(replaceMainImage);
	}
}

/**
	Animates the next images.
*/
function animateNextImages(replaceMainImage) {
	var tempStepsToMove = stepsToMove;
	
	// if we step below the beginning of the image cache, reduce the steps to move so we exactly hit the beginning
	/*if (imageIndex + stepsToMove < 0) {
		stepsToMove = -imageIndex;
	}*/

	// if we step over the end of the image cache, reduce the steps to move so we exactly hit the ending
	/*if (imageIndex + stepsToMove + nextImages.length >= imageSourceCache.length) {
		stepsToMove = imageSourceCache.length - nextImages.length - imageIndex;
	}*/
	
	// set the new image index to reload the images
	imageIndex = checkIndex(stepsToMove + imageIndex);
	
	/*if(imageIndex < 0) {
		imageIndex -= imageSourceCache.length;
		console.log(imageIndex);
	}*/
	
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
				var cleanIndex = nextImages.length - i - 1;
				if (cleanIndex < 0) {
					cleanIndex = 0;
				}
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
	Checks if the index is below zero or above the array length and returns a valid index (regarding the image loop)
*/
function checkIndex(index) {
	if (index < 0) {
		return imageSourceCache.length + index;
	} else if (index >= imageSourceCache.length) {
		return index % imageSourceCache.length;
	} else {
		return index;
	}
}