$ = jQuery;

var mainImageHolder;
var previousImageHolders = new Array();
var currentImageHolders = new Array();
var futureImageHolders = new Array();

var currentMainImageIndex;
var stepsToMove;
var imageIndex = 0;
var marker;

var navigationDisabled = true;

/**
 * Initializes the image slider.
 */
function initializeImageSlider() {
	initializeImages();
	initializeCache();
}

/**
 * Adds the object references to the different arrays and variables.
 */
function initializeImages() {
	mainImageHolder = $('#mainImage');
	previousImageHolders[0] = $('#previousImage01');
	previousImageHolders[1] = $('#previousImage02');
	previousImageHolders[2] = $('#previousImage03');
	previousImageHolders[3] = $('#previousImage04');
	currentImageHolders[0] = $('#currentImage01');
	currentImageHolders[1] = $('#currentImage02');
	currentImageHolders[2] = $('#currentImage03');
	currentImageHolders[3] = $('#currentImage04');
	futureImageHolders[0] = $('#futureImage01');
	futureImageHolders[1] = $('#futureImage02');
	futureImageHolders[2] = $('#futureImage03');
	futureImageHolders[3] = $('#futureImage04');
}

/**
 * Loads the image paths into the cache.
 */
function initializeCache() {
	$.ajax({
		url: "/vote/getdata/json",
		success: function(result) {
			// cache all fetched database data
			cacheAllData(result);
			
			// load the first image sources from the cache
			mainImageHolder.attr('src', observations[imageIndex].fullsize_image_path);
			
			// write the current number of images to the diashow link
			$('#numberOfImages').html(observations[imageIndex].observation_images.images.length);
			
			currentMainImageIndex = imageIndex;
			prepareSlideShow();
			
			initLightBox();
			
			var autoHeight = mainImageHolder.attr('height');
			$("#mainImageFieldset").animate({ height: autoHeight + 20 });
			
			imageIndex = 1;
			loadImagesFromCache();
			
			// add marker to map and set initial zoom level
			initializeGoogleMap();
			
			// load suggestions from other users
			initializeVotesFromOtherUsers();
			
			$('#imagesContainer').waitForImages(function() {
				navigationDisabled = false;
			});
		},
		error: function(result) {
			observation.setMessage(Drupal.t('Could not fetch the needed information from the server. Please try again by reloading the page.'), 'error', 5000);
		}
	});
}

/**
 * Initializes the light box.
 */
function initLightBox() {
	var galleryLightboxSettings = {
		captionSelector : ".caption",
		captionAttr : false,
		captionHTML : true,
	};
	
	$('a.lightbox').lightBox(galleryLightboxSettings);
}

/**
 * Loads the images from the cache using the imageIndex variable as index.
 */
function loadImagesFromCache() {
	var numberToCount;
	if(observations.length < currentImageHolders.length) {
		numberToCount = observations.length - 1;
	} else {
		numberToCount = currentImageHolders.length;
	}
	
	for (var i = 0; i < numberToCount; i ++) {
		// load previous thumbnail and fullsize images into cache
		var cleanIndex = checkIndex(imageIndex - numberToCount + i);
		previousImageHolders[i].attr('src', observations[cleanIndex].thumbnail_image_path);
		$('#mainImageContainer').append("<img src=\"" + observations[cleanIndex].fullsize_image_path + "\" style=\"display:none;\" alt=\"Image\" />");

		// load next thumbnail and fullsize images into cache
		cleanIndex = checkIndex(imageIndex + i);
		currentImageHolders[i].attr('src', observations[cleanIndex].thumbnail_image_path);
		$('#mainImageContainer').append("<img src=\"" + observations[cleanIndex].fullsize_image_path + "\" style=\"display:none;\" alt=\"Image\" />");

		// load future thumbnail and fullsize images into cache
		cleanIndex = checkIndex(imageIndex + numberToCount + i);
		futureImageHolders[i].attr('src', observations[cleanIndex].thumbnail_image_path);
		$('#mainImageContainer').append("<img src=\"" + observations[cleanIndex].fullsize_image_path + "\" style=\"display:none;\" alt=\"Image\" />");
		
		previousImageHolders[i].css({ width: 0, opacity: 0 });
		
		if (i < futureImageHolders.length) {
			futureImageHolders[i].css({ width: 0, opacity: 0 });
		}
		
		currentImageHolders[i].css({ width: 190, opacity: 1 });
	}
	
	// hide next/previous button if there are not enough images to switch
	if (observations.length <= currentImageHolders.length) {
		$('.imageSliderNavigation').hide();
		for (var i = observations.length - 1; i < currentImageHolders.length; i ++) {
			currentImageHolders[i].hide();
		}
	} else {
		$('.imageSliderNavigation').show();
		for (var i = observations.length - 1; i < currentImageHolders.length; i ++) {
			currentImageHolders[i].show();
		}
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
	var myLatlng = new google.maps.LatLng(observations[currentMainImageIndex].google_map_marker_lng, observations[currentMainImageIndex].google_map_marker_lat);
	marker.setPosition(myLatlng);
	observationmap.googlemap.setCenter(myLatlng);
	observationmap.googlemap.setZoom(16);
	
	initializeDetailTable();
}

/**
 * Load detail information and draw the table.
 */
function initializeDetailTable() {
	
	// initialize detail table
	var info = observations[currentMainImageIndex].detail_information['#rows'];
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
	
	// initialize comment table
	info = observations[currentMainImageIndex].comment_information['#rows'];
	element = $('#commentTable');
	
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
 * Prepares the images for the lightbox pop-up slideshow.
 */
function prepareSlideShow() {
	// change the ID of the current image onclick handler
	$('#mainImageLink').attr("href", observations[currentMainImageIndex].fullsize_image_path);
	$('.caption').html('<p class="captionText"><span class="author">&copy; ' + observations[currentMainImageIndex].observation_images.images[0].author + ', ' + observations[currentMainImageIndex].observation_images.images[0].location + '</span> <em>' + observations[currentMainImageIndex].detail_information["#rows"][0][1] + '</em></p>');
	
	// load slideshow images for current main image
	$('#slideshowImages').html('');
	for (var j = 1; j < observations[currentMainImageIndex].observation_images.images.length; j ++) {
		var currentPath = '/gallery/observation/' + observations[currentMainImageIndex].observation_id + '/image/' + observations[currentMainImageIndex].observation_images.images[j].id;
		$('#slideshowImages').append("<a class=\"lightbox\" href=\"" + currentPath + "\" style=\"display: none;\"><img src=\"" + currentPath + "\" alt=\"Image\" /><div class=\"caption\"><p class=\"captionText\"><span class=\"author\">&copy; " + observations[currentMainImageIndex].observation_images.images[0].author + ", " + observations[currentMainImageIndex].observation_images.images[0].location + "</span> <em>" + observations[currentMainImageIndex].detail_information["#rows"][0][1] + "</em></p></div></a>");
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
	if (navigationDisabled) {
		return;
	}

	stepsToMove = steps;
	
	navigationDisabled = true;
	
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
				currentImageHolders[stepsToMove - 1].animate({
						left: "-=" + (currentImageHolders[stepsToMove - 1].position().left - mainImageHolder.position().left),
						top: "-=" + (currentImageHolders[stepsToMove - 1].position().top - mainImageHolder.position().top)
					}, 500, function() {
						/*
							Animation complete of selected image movement
							Switch the main image source with the selected image source
						*/
						mainImageHolder.attr("src", observations[currentMainImageIndex].fullsize_image_path);
						
						// write the current number of images to the diashow link
						$('#numberOfImages').html(observations[currentMainImageIndex].observation_images.images.length);
						
						prepareSlideShow();
						
						initLightBox();
						
						// Calculate the new height of the image
						mainImageHolder.css({ width: 440 });
						var autoHeight = mainImageHolder.attr('height');
						// Show the main image and animate it to the normal size
						mainImageHolder.css({ width: 190, height: "auto" });
						$("#mainImageContainer").css({ opacity: 1 });
						
						mainImageHolder.animate({ width: 440 });
						$("#mainImageFieldset").animate({ height: autoHeight + 20 });
						// Reset position of the selected image
						currentImageHolders[stepsToMove - 1].removeAttr("style");
						// check if the amount of steps is possible to do
						animateCurrentImages(replaceMainImage);
						
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
		animateCurrentImages(replaceMainImage);
	}
}

/**
 * Animates the next images.
 * 
 * @param replaceMainImage if the main image should be replaced or not
 */
function animateCurrentImages(replaceMainImage) {
	
	// set the new image index to reload the images
	imageIndex = checkIndex(stepsToMove + imageIndex);
	
	//check if the next images are positive or negative. if positive the future images will be loaded, if negative the previousImageHolders will be loaded
	var positive = stepsToMove >= 0;
	for (var i = 0; i < Math.abs(stepsToMove); i ++) {
		if (positive) {
			if (i < futureImageHolders.length) {
				futureImageHolders[i].animate({ width: 190, opacity: 1 }, 500, function() {
					navigationDisabled = false;
				});
			}
			if (i < currentImageHolders.length) {
				if(i >= Math.abs(stepsToMove) - 1){
					currentImageHolders[i].animate({ width: 0, opacity: 0 }, 500, function() {
						loadImagesFromCache();
						navigationDisabled = false;
					});
				} else {
					currentImageHolders[i].animate({ width: 0, opacity: 0 }, 500, function() {
						navigationDisabled = false;
					});
				}
			}
		} else {
			if (i < previousImageHolders.length) {
				var cleanIndex = currentImageHolders.length - i - 1;
				if (cleanIndex < 0) {
					cleanIndex = 0;
				}
				previousImageHolders[cleanIndex].animate({ width: 190, opacity: 1 }, 500, function() {
					navigationDisabled = false;
				});
			}
			if (i < currentImageHolders.length) {
				if(i >= Math.abs(stepsToMove) - 1){
					currentImageHolders[currentImageHolders.length - i - 1].animate({ width: 0, opacity: 0 }, 500, function() {
						loadImagesFromCache();
						navigationDisabled = false;
					});
				} else {
					currentImageHolders[currentImageHolders.length - i - 1].animate({ width: 0, opacity: 0 }, 500);
					navigationDisabled = false;
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
		return observations.length + index;
	} else if (index >= observations.length) {
		return index % observations.length;
	} else {
		return index;
	}
}