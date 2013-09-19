$ = jQuery;

/*
 * Global variables which all functions use 
 */
var mainImage;
var currentMainImageIndex;
var previousImages = new Array();
var nextImages = new Array();
var futureImages = new Array();
var imageSourceCache = new Array();
var stepsToMove;
var imageIndex = 0;
var marker;

/**
 * Adds the object references to the different arrays and variables.
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
 * Loads the image paths into the cache.
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
					imagesCount: result[i].images.images.length,
					wgs84_center_lat: result[i].wgs84_center_lat,
					wgs84_center_lng: result[i].wgs84_center_lng,
					table: result[i].table,
					suggestionsFromOtherUsers: result[i].suggestionsFromOtherUsers,
					currentUserId: result[i].currentUserId,
					labels: result[i].labels
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
			
			// add marker to map and set initial zoom level
			initializeGoogleMap();
			
			// load suggestions from other users
			initializeVotesFromOtherUsers();
		},
		error: function(result){
			alert("error");
		}
	});
}

/**
 * Initializes the light box.
 */
function initLightBox() {
	
	// TODO: add description etc. to the images in the light box
	var galleryLightboxSettings = {
		captionSelector : ".caption",
		captionAttr : false,
		captionHTML : true,
	};
	
	$('a.lightbox').lightBox(); // Select all links with lightbox class
}

/**
 * Returns the current observation id.
 * 
 * @returns the current observation id
 */
function getCurrentObservationId() {
	return imageSourceCache[currentMainImageIndex].imageId;
}

/**
 * Loads the images from the cache using the imageIndex variable as index.
 */
function loadImagesFromCache() {
	for (var i = 0; i < nextImages.length; i ++) {
		
		// load previous thumbnail and fullsize images into cache
		var cleanIndex = checkIndex(imageIndex - nextImages.length + i);
		previousImages[i].attr('src', imageSourceCache[cleanIndex].imageThumbPath);
		$('#mainImageContainer').append("<img src=\"" + imageSourceCache[cleanIndex].imagePath + "\" style=\"display:none;\" alt=\"Image\" />");
		
		// load next thumbnail and fullsize images into cache
		cleanIndex = checkIndex(imageIndex + i);
		nextImages[i].attr('src', imageSourceCache[cleanIndex].imageThumbPath);
		$('#mainImageContainer').append("<img src=\"" + imageSourceCache[cleanIndex].imagePath + "\" style=\"display:none;\" alt=\"Image\" />");
		
		// load future thumbnail and fullsize images into cache
		cleanIndex = checkIndex(imageIndex + nextImages.length + i);
		futureImages[i].attr('src', imageSourceCache[cleanIndex].imageThumbPath);
		$('#mainImageContainer').append("<img src=\"" + imageSourceCache[cleanIndex].imagePath + "\" style=\"display:none;\" alt=\"Image\" />");
		
		previousImages[i].css({ width: 0, opacity: 0 });
		
		if (i < futureImages.length) {
			futureImages[i].css({ width: 0, opacity: 0 });
		}
		
		nextImages[i].css({ width: 190, opacity: 1 });
	}
}

/**
 * Initializes google map and adds a marker of the observation location.
 */
function initializeGoogleMap() {
	if (marker == null) {
		marker = new google.maps.Marker();
		marker.setMap(observationmap.googlemap);
	}
	var myLatlng = new google.maps.LatLng(imageSourceCache[currentMainImageIndex].wgs84_center_lng, imageSourceCache[currentMainImageIndex].wgs84_center_lat);
	marker.setPosition(myLatlng);
	observationmap.googlemap.setCenter(myLatlng);
	observationmap.googlemap.setZoom(16);
	
	// load detail information
	var info = imageSourceCache[currentMainImageIndex].table['#rows'];
	var element = $('#detailTable');
	
	element.html('');
	element.append('<table><tbody>');
	for (var i = 0; i < info.length; i ++) {
		if (i % 2 == 0) {
			element.append('<tr class="odd"><th>' + info[i][0].data + '</th><td>' + info[i][1] + '</td></tr>');
		} else {
			element.append('<tr class="even"><th>' + info[i][0].data + '</th><td>' + info[i][1] + '</td></tr>');
		}
	}
	element.append('</tbody></table>');
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
 * Move the image index using the given number of steps (e.g. -4 moves four images back, 3 moves three images forward).
 * If the boolan replaceMainImage is set to true, the main image will be replaced with the destination image.
 * 
 * @param steps number of steps to move
 * @param replaceMainImage if the main image should be replaced or not
 */
function moveImages(steps, replaceMainImage) {
	if (!finishedAllAnimations()) {
		return;
	}

	stepsToMove = steps;
	
	animateMainImage(replaceMainImage);
}

/**
 * Animates the main image replacement.
 * 
 * @param replaceMainImage if the main image should be replaced or not
 */
function animateMainImage(replaceMainImage) {
	if (replaceMainImage) {
		currentMainImageIndex = checkIndex(imageIndex + stepsToMove - 1);
		
		// Fade main image out
		$("#mainImageContainer").animate({
				opacity: 0
			}, 500, function() {
				/*
					Animation complete of main image fade out
					Move the selected image to the main image position
				*/
				nextImages[stepsToMove - 1].animate({
						left: "-=" + (nextImages[stepsToMove - 1].position().left - mainImage.position().left),
						top: "-=" + (nextImages[stepsToMove - 1].position().top - mainImage.position().top)
					}, 500, function() {
						/*
							Animation complete of selected image movement
							Switch the main image source with the selected image source
						*/
						mainImage.attr("src", imageSourceCache[currentMainImageIndex].imagePath);
						
						// write the current number of images to the diashow link
						$('#numberOfImages').html(imageSourceCache[currentMainImageIndex].imagesCount);
						
						prepareSlideShow();
						
						initLightBox();
						
						// Calculate the new height of the image
						mainImage.css({ width: 440 });
						var autoHeight = mainImage.attr('height');
						// Show the main image and animate it to the normal size
						mainImage.css({ width: 190, height: "auto" });
						$("#mainImageContainer").css({ opacity: 1 });
						
						mainImage.animate({ width: 440 });
						$("#mainImageFieldset").animate({ height: autoHeight + 20 });
						// Reset position of the selected image
						nextImages[stepsToMove - 1].removeAttr("style");
						// check if the amount of steps is possible to do
						animateNextImages(replaceMainImage);
						
						// add marker to map and set initial zoom level
						initializeGoogleMap();
						
						// load suggestions from other users
						initializeVotesFromOtherUsers();
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
 * Animates the next images.
 * 
 * @param replaceMainImage if the main image should be replaced or not
 */
function animateNextImages(replaceMainImage) {
	
	// set the new image index to reload the images
	imageIndex = checkIndex(stepsToMove + imageIndex);
	
	//check if the next images are positive or negative. if positive the future images will be loaded, if negative the previousimages will be loaded
	var positive = stepsToMove >= 0;
	for (var i = 0; i < Math.abs(stepsToMove); i ++) {
		if (positive) {
			if (i < futureImages.length) {
				futureImages[i].animate({ width: 190, opacity: 1 }, 500);
			}
			if (i < nextImages.length) {
				if(i >= Math.abs(stepsToMove) - 1){
					nextImages[i].animate({ width: 0, opacity: 0 }, 500, function() {
						loadImagesFromCache();
					});
				} else {
					nextImages[i].animate({ width: 0, opacity: 0 }, 500);
				}
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
				if(i >= Math.abs(stepsToMove) - 1){
					nextImages[nextImages.length - i - 1].animate({ width: 0, opacity: 0 }, 500, function() {
						loadImagesFromCache();
					});
				} else {
					nextImages[nextImages.length - i - 1].animate({ width: 0, opacity: 0 }, 500);
				}
			}
		}
	}
}

/**
 * Checks if the index is below zero or above the array length and returns a valid index (regarding the image loop)
 * 
 * @param index the index to check if it's valid
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