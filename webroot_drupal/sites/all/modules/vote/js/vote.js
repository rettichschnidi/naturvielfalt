$ = jQuery;

/**
Executed after page was loaded.
*/
$(document).ready(function () {
	initializeImages();
	initializeCache();
	initializeSubmitVerificationButton();
});

/**
 * Initializes the submit verification button.
 */
function initializeSubmitVerificationButton() {
	$('#submitVerification').click(function() {
		$.ajax({
			type: "POST",
			url: "vote/save",
			success: function(result){
				$('html, body').animate({ scrollTop: 0 });
				
				/**
				 * Use this, if the item, which has been voted for, should be removed from the array
				 */
				//imageSourceCache.splice(currentMainImageIndex, 1);
				// Set imageIndex-1 because the next image would be 1 step too far away (flogys description would be: i'm a getter and setter faggy)
				//imageIndex -= 1;
				
				nextImage();
			},
			error: function(result) {
				alert('fail');
			},
			data: {
				observation_id: getCurrentObservationId(),
				organism_name: $('#organism_name').val(),
				date: $('#date').val(),
				time: $('#time').val(),
				comment: $('#comment').val()
			}
		});
	});
}

/**
 * Loads the votes from other users for the current observation.
 */
function initializeVotesFromOtherUsers() {
	var container = $('#selectBoxContainer');
	var noVerificationsMessage = $('#noVerificationsMessage');
	container.html('');
	var suggestions = imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers;
	if (suggestions != null) {
		noVerificationsMessage.hide();
		container.show();
		for (var i = 0; i < suggestions.length; i ++) {
			container.append(	'<div class="entry">'
							  + '<div class="progressBar" style="width: ' + suggestions[i].votes_percent + '%;">'
							  + '<span class="translatedDescription">' + suggestions[i].translated_name + '</span>'
							  + '<span class="latinDescription"><i>' + suggestions[i].scientific_name + '</i></span>'
							  + '<span class="votes">' + suggestions[i].votes + ' ' + imageSourceCache[currentMainImageIndex].labels.verifications + '</span>'
							  + '</div>'
							  + '<div class="suggestButton">' + imageSourceCache[currentMainImageIndex].labels.agree + '</div>'
							  + '</div>');
		}
	} else {
		container.hide();

		noVerificationsMessage.html(imageSourceCache[currentMainImageIndex].labels.noVerifications);
		noVerificationsMessage.show();
	}
	initializeSelectBox();
}