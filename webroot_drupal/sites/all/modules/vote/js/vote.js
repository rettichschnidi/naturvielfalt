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
				
				// scroll to the top of the page to display the next observation
				$('html, body').animate({ scrollTop: 0 });
				
				// TODO: scientific name hidden field
				var myVote = {
						translated_name: $('#organism_name').val(),
						scientific_name: $('#organism_name').val(),
						votes: 1,
						user_verifications: [{ user_id: imageSourceCache[currentMainImageIndex].current_user, vote_comment: $('#comment').val() }]
				};
				
				// don't load imageSourceCache[currentMainImageIndex].current_verifications into a variable because it will create an instance of the variable and it would not work with the real array
				if (imageSourceCache[currentMainImageIndex].current_verifications == null) {
					imageSourceCache[currentMainImageIndex].current_verifications = new Array();
				} else {
					for (var i = 0; i < imageSourceCache[currentMainImageIndex].current_verifications.length; i++) {
						for(var a = 0; a < imageSourceCache[currentMainImageIndex].current_verifications[i].user_verifications.length; a++) {
							
							// check if the current user voted for this suggestion
							if (imageSourceCache[currentMainImageIndex].current_user == imageSourceCache[currentMainImageIndex].current_verifications[i].user_verifications[a].user_id) {
								
								// check if the suggestion, which the user voted for has votes from other users. if yes, then leave the suggestions there and just remove 1 verification
								if (imageSourceCache[currentMainImageIndex].current_verifications[i].votes > 1) {
									imageSourceCache[currentMainImageIndex].current_verifications[i].votes--;
									imageSourceCache[currentMainImageIndex].current_verifications[i].user_verifications[a].user_id = null;
									imageSourceCache[currentMainImageIndex].current_verifications[i].user_verifications[a].vote_comment = null;
								} else {
									imageSourceCache[currentMainImageIndex].current_verifications.splice(i, 1);
								}
								break;
							}
						}
					}
				}
				
				// check if the verification, which the user selected already exists. if yes, then just count 1 vote up, else push the vote to the verification array
				var verificationExists = false;
				for (var i = 0; i < imageSourceCache[currentMainImageIndex].current_verifications.length; i++) {
					if (imageSourceCache[currentMainImageIndex].current_verifications[i].translated_name == myVote.translated_name) {
						imageSourceCache[currentMainImageIndex].current_verifications[i].votes++;
						imageSourceCache[currentMainImageIndex].current_verifications[i].user_verifications = [{ user_id: imageSourceCache[currentMainImageIndex].current_user, vote_comment: $('#comment').val() }];
						verificationExists = true;
						break;
					}
				}
			
				if (!verificationExists) {
					imageSourceCache[currentMainImageIndex].current_verifications.push(myVote);
				}
				
				nextImage();
			},
			error: function(result) {
				alert('fail');
			},
			data: {
				observation_id: imageSourceCache[currentMainImageIndex].observation_id,
				organism_name: $('#organism_name').val(),
				date: $('#date').val(),
				time: $('#time').val(),
				vote_comment: $('#comment').val()
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
	var suggestions = imageSourceCache[currentMainImageIndex].current_verifications;
	if (suggestions != null) {
		noVerificationsMessage.hide();
		container.show();
		var totalVotes = 0;
		for (var i = 0; i < suggestions.length; i++) {
			totalVotes += parseInt(suggestions[i].votes);
		}
		for (var i = 0; i < suggestions.length; i++) {
			var votes_percent = Math.round(suggestions[i].votes / totalVotes * 100, 0);
			
			var htmlString = "";
			htmlString +=		'<div class="entry">'
							  + '<div class="progressBar" style="width: ' + votes_percent + '%;">'
							  + '<span class="translatedDescription">' + suggestions[i].translated_name + '</span>'
							  + '<span class="latinDescription"><i>' + suggestions[i].scientific_name + '</i></span>'
							  + '<span class="votes">' + suggestions[i].votes + ' ' + imageSourceCache[currentMainImageIndex].translated_labels.verifications + '</span>'
							  + '</div>'
							  + '<div class="suggestButton">' + imageSourceCache[currentMainImageIndex].translated_labels.agree + '</div>'
							  + '<div class="entryCommentsContainer">';
			
			for(var a = 0; a < imageSourceCache[currentMainImageIndex].current_verifications[i].user_verifications.length; a++) {
			htmlString +=		'<div class="entryComments">'
							  + '<p class="header"><b>Florian Gyger</b> am 21.08.2013:</p>'
							  + '<p>' + imageSourceCache[currentMainImageIndex].current_verifications[i].user_verifications[a].vote_comment + '</p>'
							  + '</div>';
			}
			htmlString +=		'</div>'
							  + '</div>';
			
			container.html(htmlString);
			
		}
	} else {
		container.hide();
		noVerificationsMessage.html(imageSourceCache[currentMainImageIndex].translated_labels.noVerifications);
		noVerificationsMessage.show();
	}
	initializeSelectBox();
}