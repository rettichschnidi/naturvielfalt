$ = jQuery;


function initializeSelectBox() {

	// define suggest button animation on mouse hover
	$(".entry").hover(function() {
		$(this).find(".suggestButton").stop(true, false).animate({ opacity: 1, right: 0 }, 200);
	}, function() {
		$(this).find(".suggestButton").stop(true, false).animate({ opacity: 0, right: -120 }, 200);
	});
	
	//	add event to accept buttons
	$(".suggestButton").click(function() {
		$('#organism_name').val($(this).parent().find(".translatedDescription").html());
		$('html, body').animate({ scrollTop: $("#ownVerification").offset().top });
	});
	
	// define comment box animation on mouse click
	$(".entry").toggle(function() {
		$(this).stop(true).animate({ height: $(this).find(".entryCommentsContainer").outerHeight() + $(this).height() }, 500);
	}, function(){
		$(this).stop(true).animate({ height: $(this).height() - $(this).find(".entryCommentsContainer").outerHeight() }, 500);
	});
	
	// set the background color of the entries
	$(".entry").each(function(index) {
		var colorValue = index % 2 == 0 ? 85 : 90;
		$(this).css({ "background-color": "hsv(0, 0, " + colorValue + ")" });
	});
	
	// set a random color to the bars
	$(".progressBar").each(function() {
		$(this).css({ "background-color": "hsv(" + getNextRandomHue() + ", 30, 100)" });	
	});
}

var previousHues = [];
var index = 0;

function getNextRandomHue() {
	var nextHue;
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
			previousHues[index] = nextHue;
			index ++;
			return nextHue;
		}
	}
}

/**
 * Returns a random integer between min and max
 * Using Math.round() will give you a non-uniform distribution!
 */
function getRandomInt(min, max) {
    return Math.floor(Math.random() * (max - min + 1)) + min;
}