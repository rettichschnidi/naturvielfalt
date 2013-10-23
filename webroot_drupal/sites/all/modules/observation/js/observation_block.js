var container;
var wrapper;
var page = 1;
var pageSize = 25;
var total = 9007199254740992; //max int
var isFetchingResults = false;

var stepsToMove;

var observations = [];
var previousEntryHolders = new Array();
var currentEntryHolders = new Array();
var futureEntryHolders = new Array();
var imageIndex;

var navigationDisabled = true;

var timerId = null;

jQuery(document).ready(function() {
	$ = jQuery;
	container = $('.new_observations_container');
	initializeImages();
	initializeCache();
	initializeHover();
	initializeTimer();
});

/**
 * Adds the object references to the different arrays and variables.
 */
function initializeImages() {
	previousEntryHolders[0] =  {
			content: $('#previousEntry01'),
			observationIndex : 0
	};
	previousEntryHolders[1] =  {
			content: $('#previousEntry02'),
			observationIndex : 0
	};
	previousEntryHolders[2] =  {
			content: $('#previousEntry03'),
			observationIndex : 0
	};
	previousEntryHolders[3] =  {
			content: $('#previousEntry04'),
			observationIndex : 0
	};
	currentEntryHolders[0] = {
			content: $('#currentEntry01'),
			observationIndex : 0
	};
	currentEntryHolders[1] = {
			content: $('#currentEntry02'),
			observationIndex : 0
	};
	currentEntryHolders[2] = {
			content: $('#currentEntry03'),
			observationIndex : 0
	};
	currentEntryHolders[3] = {
			content: $('#currentEntry04'),
			observationIndex : 0
	};
	futureEntryHolders[0] = {
			content: $('#futureEntry01'),
			observationIndex : 0
	};
	futureEntryHolders[1] = {
			content: $('#futureEntry02'),
			observationIndex : 0
	};
	futureEntryHolders[2] = {
			content: $('#futureEntry03'),
			observationIndex : 0
	};
	futureEntryHolders[3] = {
			content: $('#futureEntry04'),
			observationIndex : 0
	};
}

function initializeHover() {
	container.hover(function() {
		if(!navigationDisabled)
			stopTimer();
	}, function() {
		if(!navigationDisabled)
			initializeTimer();
	});
}

function initializeTimer() {
	if(timerId) stopTimer(timerId);
	timerId = setInterval(function() {moveImages(4); }, 8000);
}

function stopTimer() {
	if(timerId)
		clearInterval(timerId);
}

function cacheAllData(result) {
	
	// cache general information
	total = result.total;
	var tmpDiv = null;
	// cache observation information
	var j = 0;
	for(var i = (result.page-1) * pageSize; j < result.content.length; i++) {
		//we need to save the height of the new content.. add it to dom, get display height and remove it again.
		tmpDiv = $('<div style="height:auto">' + result.content[j] + '</div>');
		container.append(tmpDiv.hide());
		observations[i] = {
				content: result.content[j++],
				height: tmpDiv.height()
		};
		tmpDiv.remove();
	}
	tmpDiv = null;
}

function initializeCache() {
	showLoading(true);
	isFetchingResults = true;
	$.ajax({
		type: "POST",
		url: Drupal.settings.basePath + "observation/block/newobservations",
		success: function(result){
			showLoading(false,function() {
				cacheAllData(result);
				imageIndex = 0;
				loadImagesFromCache();
				container.waitForImages(function() {
					navigationDisabled = false;
					page++;
					isFetchingResults = false;
				});
			});
		},
		error: function(result) {
			isFetchingResults = false;
			showLoading(false);
		},
		data: {
			page: page
		}
	});
	
}
function fetchPage(steps) {
	if(isFetchingResults || (page-1) * pageSize >= parseInt(total)) return false; //no more observations to fetch
	showLoading(true);
	isFetchingResults = true;
	$.ajax({
		type: "POST",
		url: Drupal.settings.basePath + "observation/block/newobservations",
		success: function(result){
			showLoading(false,function() {
				cacheAllData(result);
				loadImagesFromCache();
				container.waitForImages(function() {
					navigationDisabled = false;
					page++;
					moveImages(steps);
					isFetchingResults = false;
				});
			});
		},
		error: function(result) {
			isFetchingResults = false;
			showLoading(false);
		},
		data: {
			page: page
		}
	});
	return true;
}

/**
 * Move the image index using the given number of steps (e.g. -4 moves four images back, 3 moves three images forward).
 * If the boolan replaceMainImage is set to true, the main image will be replaced with the destination image.
 * 
 * @param steps number of steps to move
 * @param replaceMainImage if the main image should be replaced or not
 */
function moveImages(steps) {
	if (navigationDisabled) {
		return;
	}
	
	//if imageIndex + step + 4 future images  is greater then pagesize, and more images are available on server, fetch next page first to load the images
	tmpsteps = imageIndex + steps + 4;
	if(tmpsteps >= (page-1)*pageSize && (page-1)*pageSize < parseInt(total)) {
		navigationDisabled = true;
		fetchPage(steps);
		return;
	}
	stepsToMove = steps;
	
	navigationDisabled = true;
	
	animateCurrentImages();
}

/**
 * Loads the images from the cache using the imageIndex variable as index.
 * changeOpacityOfCurrentImages is used to if the opacity of images should be changed by showLoading()
 */
function loadImagesFromCache(changeOpacityOfCurrentImages) {
	var numberToCount;
	if(observations.length < currentEntryHolders.length) {
		numberToCount = observations.length - 1;
	} else {
		numberToCount = currentEntryHolders.length;
	}
	
	for (var i = 0; i < numberToCount; i ++) {
		// load previous thumbnail and fullsize images into cache
		var cleanIndex = checkIndex(imageIndex - numberToCount + i);
		previousEntryHolders[i].content.html(observations[cleanIndex].content);
		previousEntryHolders[i].observationIndex = cleanIndex;
		
		// load next thumbnail and fullsize images into cache
		cleanIndex = checkIndex(imageIndex + i);
		//display image (restarting at the beginning, if no more images can be fetched from the server
		currentEntryHolders[i].content.html(observations[cleanIndex].content);
		currentEntryHolders[i].observationIndex = cleanIndex;
		
		// load future thumbnail and fullsize images into cache
		cleanIndex = checkIndex(imageIndex + numberToCount + i);
		futureEntryHolders[i].content.html(observations[cleanIndex].content);
		futureEntryHolders[i].observationIndex = cleanIndex;
		
		previousEntryHolders[i].content.css({ height: 0, opacity: 0 });
		previousEntryHolders[i].content.hide();
		
		if (i < futureEntryHolders.length) {
			futureEntryHolders[i].content.css({ height: 0, opacity: 0 });
			futureEntryHolders[i].content.hide();
		}
		
		if(changeOpacityOfCurrentImages)
			currentEntryHolders[i].content.css({ height: observations[currentEntryHolders[i].observationIndex].height, opacity: 0.5 });
		else currentEntryHolders[i].content.css({ height: observations[currentEntryHolders[i].observationIndex].height, opacity: 1 });
		
		currentEntryHolders[i].content.show();
	}
	
	// hide next/previous button if there are not enough images to switch
	if (observations.length <= currentEntryHolders.length) {
		$('.imageSliderNavigation').hide();
		for (var i = observations.length - 1; i < currentEntryHolders.length; i ++) {
			currentEntryHolders[i].content.hide();
		}
	} else {
		$('.imageSliderNavigation').show();
		for (var i = observations.length - 1; i < currentEntryHolders.length; i ++) {
			currentEntryHolders[i].content.show();
		}
	}
	initLightBox();
}

/**
 * Animates the next images.
 * 
 * @param replaceMainImage if the main image should be replaced or not
 */
function animateCurrentImages() {
	
	// set the new image index to reload the images
	imageIndex = checkIndex(stepsToMove + imageIndex);
	
	//check if the next images are positive or negative. if positive the future images will be loaded, if negative the previousEntryHolders will be loaded
	var positive = stepsToMove >= 0;
	for (var i = 0; i < Math.abs(stepsToMove); i ++) {
		if (positive) {
			if (i < futureEntryHolders.length) {
				futureEntryHolders[i].content.show();
				futureEntryHolders[i].content.animate({ height: observations[futureEntryHolders[i].observationIndex].height, opacity: 1 }, 1500, function() {
					navigationDisabled = false;
				});
			}
			if (i < currentEntryHolders.length) {
				if(i >= Math.abs(stepsToMove) - 1){
					currentEntryHolders[i].content.animate({ height: 0, opacity: 0 }, 1500, function() {
						loadImagesFromCache();
						navigationDisabled = false;
					});
				} else {
					currentEntryHolders[i].content.animate({ height: 0, opacity: 0 }, 1500, function() {
						navigationDisabled = false;
					});
				}
			}
		} else {
			if (i < previousEntryHolders.length) {
				var cleanIndex = currentEntryHolders.length - i - 1;
				if (cleanIndex < 0) {
					cleanIndex = 0;
				}
				previousEntryHolders[cleanIndex].content.show();
				previousEntryHolders[cleanIndex].content.animate({ height: observations[previousEntryHolders[cleanIndex].observationIndex].height, opacity: 1 }, 1500, function() {
					navigationDisabled = false;
				});
			}
			if (i < currentEntryHolders.length) {
				if(i >= Math.abs(stepsToMove) - 1){
					currentEntryHolders[currentEntryHolders.length - i - 1].content.animate({ height: 0, opacity: 0 }, 1500, function() {
						loadImagesFromCache();
						navigationDisabled = false;
					});
				} else {
					currentEntryHolders[currentEntryHolders.length - i - 1].content.animate({ height: 0, opacity: 0 }, 1500, function() {
						navigationDisabled = false;
					});
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

/**
 * Initializes the light box.
 */
function initLightBox() {
	var galleryLightboxSettings = {
		captionSelector : ".block_newest_observation_img_caption",
		captionAttr : false,
		captionHTML : true,
	};
	
	$('a.lightbox').lightBox(galleryLightboxSettings);
}

function showLoading(display, completeCallback) {
	if(display) {
		$('#showLoadingImage').css({ top: - container.height()/2});
		$('#showLoadingImage').show();
		if(completeCallback)
			container.fadeTo("slow", 0.5, completeCallback); 
		else 
			container.fadeTo("slow", 0.5); 
	} else {
		 $('#showLoadingImage').hide();
		 if(completeCallback)
			 container.fadeTo("slow", 1, completeCallback); 
		 else 
			 container.fadeTo("slow", 1); 
	}
}