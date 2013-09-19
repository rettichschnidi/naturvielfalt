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
				
				// TODO: scientific name hidden field
				var suggestions = imageSourceCache[currentMainImageIndex].suggestionsFromOtherUsers;
				var myVote = {
						translated_name: $('#organism_name').val(),
						scientific_name: $('#organism_name').val(),
						votes: 1,
						user_ids: [imageSourceCache[currentMainImageIndex].currentUserId]
				};
				
				console.log(suggestions);
				
				if (suggestions == null) {
					suggestions = new Array();
				} else {
					for (var i = 0; i < suggestions.length; i++) {
						console.log(suggestions[i].user_ids);
						for(var a = 0; a < suggestions[i].user_ids.length; a++) {
							
							if (imageSourceCache[currentMainImageIndex].currentUserId == suggestions[i].user_ids[a]) {
								
								if (suggestions[i].votes > 1) {
									suggestions[i].votes--;
									suggestions[i].user_ids[a] = null;
								} else {
									suggestions.splice(i, 1);
								}
								break;
								
							}
							
						}
						
					}
				}
				
				var done = false;
				
				for (var i = 0; i < suggestions.length; i++) {
					if (suggestions[i].translated_name == myVote.translated_name) {
						suggestions[i].votes++;
						suggestions[i].user_ids = [imageSourceCache[currentMainImageIndex].currentUserId];
						done = true;
						break;
					}
				}
			
				if (!done) {
					suggestions.push(myVote);
				}
				console.log(suggestions);
				initializeVotesFromOtherUsers();
				
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