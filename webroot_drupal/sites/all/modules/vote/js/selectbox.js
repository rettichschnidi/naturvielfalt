$ = jQuery;

/**
 * Initialize the select box.
 */
function initializeSelectBox() {

	// define suggest button animation on mouse hover
	$(".entry").hover(function() {
		$(this).find(".suggestButton").stop(true, false).animate({ opacity: 1, right: 0 }, 200);
		$(this).stop(true, true).animate({ height: $(this).find(".entryCommentsContainer").outerHeight() + $(this).height() }, 500);
	}, function() {
		$(this).find(".suggestButton").stop(true, false).animate({ opacity: 0, right: -120 }, 200);
		$(this).stop(true, true).animate({ height: $(this).height() - $(this).find(".entryCommentsContainer").outerHeight() }, 500);
	});
	
	//	add event to accept buttons
	$(".suggestButton").click(function() {
		$('#organism_name').val($(this).parent().find(".translatedDescription").html());
		$('html, body').animate({ scrollTop: $("#ownVerification").offset().top });
	});
	
//	// define comment box animation on mouse click
//	$(".entry").toggle(function() {
//		$(this).stop(true, true).animate({ height: $(this).find(".entryCommentsContainer").outerHeight() + $(this).height() }, 500);
//	}, function(){
//		$(this).stop(true, true).animate({ height: $(this).height() - $(this).find(".entryCommentsContainer").outerHeight() }, 500);
//	});
	
	// set the background color of the entries
	$(".entry").each(function(indexTest) {
		var colorValue = indexTest % 2 == 0 ? 85 : 90;
		$(this).css({ "background-color": "hsv(0, 0, " + colorValue + ")" });
	});
	
	// set a random color to the bars
	$(".progressBar").each(function() {
		$(this).css({ "background-color": "hsv(" + getNextRandomHue() + ", 30, 100)" });	
	});
}

var previousHues = [];
var indexTest = 0;

/**
 * Returns a random hue that differs from the previous one.
 * @returns a random hue
 */
function getNextRandomHue() {
	var tolerance = 20;
	var distance = 5;
	
	// if the next hue is too close to the previous one, generate a new one
	while (true) {
		var nextHue = getRandomInt(0, 255);
		var takeThisColor = true;
		for (var i = 0; i < distance; i ++) {
			if (previousHues.length - i < 0) {
				break;
			}
			if (Math.abs(previousHues[previousHues.length - i] - nextHue) < tolerance) {
				takeThisColor = false;
			}
		}
		if (takeThisColor) {
			previousHues[indexTest] = nextHue;
			indexTest ++;
			return nextHue;
		}
	}
}

/**
 * Returns a random integer between min and max.
 */
function getRandomInt(min, max) {
    return Math.floor(Math.random() * (max - min + 1)) + min;
}