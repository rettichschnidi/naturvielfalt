$ = jQuery;

/**
 * Executed after page was loaded.
 */
$(document).ready(function() {
	initializeImageSlider();
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
				
				//scroll to the top of the page to display the next observation
				$('html, body').animate({ scrollTop: 0 });
				
				// TODO: scientific name hidden field
				var myVote = {
						translated_name: $('#organism_name').val(),
						scientific_name: $('#organism_name').val(),
						votes: 1,
						user_ids: [imageSourceCache[currentMainImageIndex].currentUserId]
				};
				
				// don't load imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers into a variable because it will create an instance of the variable and it would not work with the real array
				if (imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers == null) {
					imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers = new Array();
				} else {
					for (var i = 0; i < imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers.length; i++) {
						for(var a = 0; a < imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers[i].user_ids.length; a++) {
							if (imageSourceCache[currentMainImageIndex].currentUserId == imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers[i].user_ids[a]) {
								if (imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers[i].votes > 1) {
									imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers[i].votes--;
									imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers[i].user_ids[a] = null;
								} else {
									imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers.splice(i, 1);
								}
								break;
							}
						}
					}
				}
				
				var done = false;
				
				for (var i = 0; i < imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers.length; i++) {
					if (imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers[i].translated_name == myVote.translated_name) {
						imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers[i].votes++;
						imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers[i].user_ids = [imageSourceCache[currentMainImageIndex].currentUserId];
						done = true;
						break;
					}
				}
			
				if (!done) {
					imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers.push(myVote);
				}
				
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
		var totalVotes = 0;
		for (var i = 0; i < suggestions.length; i++) {
			totalVotes += parseInt(suggestions[i].votes);
		}
		for (var i = 0; i < suggestions.length; i++) {
			var votes_percent = Math.round(suggestions[i].votes / totalVotes * 100, 0);
			container.append(	'<div class="entry">'
							  + '<div class="progressBar" style="width: ' + votes_percent + '%;">'
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