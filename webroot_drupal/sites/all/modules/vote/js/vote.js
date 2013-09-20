$ = jQuery;

var generalInformation;
var observations = new Array();
var verifications = new Array();

/**
 * Executed after page was loaded.
 */
$(document).ready(function() {
	initializeImageSlider();
	initializeSubmitVerificationButton();
});

function cacheAllData(result) {
	
	// cache general information
	generalInformation = {
		current_user_id: result.general.current_user_id,
		translated_labels: result.general.translated_labels
	};
	
	// cache observation information
	for (var i = 0; i < result.observations.length; i++) {
		observations[i] = {
			observation_id: result.observations[i].observation_id,
			thumbnail_image_path: result.observations[i].thumbnail_image_path,
			fullsize_image_path: result.observations[i].fullsize_image_path,
			observation_images: result.observations[i].observation_images,
			detail_information: result.observations[i].detail_information,
			google_map_marker_lat: result.observations[i].google_map_marker_lat,
			google_map_marker_lng: result.observations[i].google_map_marker_lng,
		};
	}
	
	// cache verification information
	for (var i = 0; i < result.verifications.length; i ++) {
		verifications[i] = {
			vote_id: result.verifications[i].vote_id,
			observation_id: result.verifications[i].observation_id,
			organism_id: result.verifications[i].organism_id,
			scientific_name: result.verifications[i].scientific_name,
			translated_name: result.verifications[i].translated_name,
			vote_timestamp: result.verifications[i].vote_timestamp,
			comment: result.verifications[i].comment,
			user_id: result.verifications[i].user_id,
			user_name: result.verifications[i].user_name
		};
	}
}

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
						user_verifications: [{ user_id: generalInformation.current_user_id, vote_comment: $('#comment').val() }]
				};
				
				// don't load observations[currentMainImageIndex].current_verifications into a variable because it will create an instance of the variable and it would not work with the real array
				if (observations[currentMainImageIndex].current_verifications == null) {
					observations[currentMainImageIndex].current_verifications = new Array();
				} else {
					for (var i = 0; i < observations[currentMainImageIndex].current_verifications.length; i++) {
						for(var a = 0; a < observations[currentMainImageIndex].current_verifications[i].user_verifications.length; a++) {
							
							// check if the current user voted for this suggestion
							if (generalInformation.current_user_id == observations[currentMainImageIndex].current_verifications[i].user_verifications[a].user_id) {
								
								// check if the suggestion, which the user voted for has votes from other users. if yes, then leave the verifications there and just remove 1 verification
								if (observations[currentMainImageIndex].current_verifications[i].votes > 1) {
									observations[currentMainImageIndex].current_verifications[i].votes--;
									observations[currentMainImageIndex].current_verifications[i].user_verifications[a].user_id = null;
									observations[currentMainImageIndex].current_verifications[i].user_verifications[a].vote_comment = null;
								} else {
									observations[currentMainImageIndex].current_verifications.splice(i, 1);
								}
								break;
							}
						}
					}
				}
				
				// check if the verification, which the user selected already exists. if yes, then just count 1 vote up, else push the vote to the verification array
				var verificationExists = false;
				for (var i = 0; i < observations[currentMainImageIndex].current_verifications.length; i++) {
					if (observations[currentMainImageIndex].current_verifications[i].translated_name == myVote.translated_name) {
						observations[currentMainImageIndex].current_verifications[i].votes++;
						observations[currentMainImageIndex].current_verifications[i].user_verifications = [{ user_id: generalInformation.current_user_id, vote_comment: $('#comment').val() }];
						verificationExists = true;
						break;
					}
				}
			
				if (!verificationExists) {
					observations[currentMainImageIndex].current_verifications.push(myVote);
				}
				
				nextImage();
			},
			error: function(result) {
				alert('fail');
			},
			data: {
				observation_id: observations[currentMainImageIndex].observation_id,
				organism_name: $('#organism_name').val(),
				date: $('#date').val(),
				time: $('#time').val(),
				vote_comment: $('#comment').val()
			}
		});
	});
}

function getVerificationsForObservation(observation_id) {
	var resultArray = new Array();
	for (var i = 0; i < verifications.length; i ++) {
		if (verifications[i].observation_id == observation_id) {
			resultArray.push(verifications[i]);
		}
	}
	return resultArray;
}

function getVerificationsPerOrganism(filteredVerificationsArray) {
	var resultArray = [];
	
	for(var i = 0; i < filteredVerificationsArray.length; i++) {
		if(resultArray.length > 0) {
			for(var a = 0; a < resultArray.length; a++) {
				if(resultArray[a].organism_id == filteredVerificationsArray[i].organism_id) {
					resultArray[a].verifications.push(filteredVerificationsArray[i]);
				} else {
					resultArray.push({
						organism_id: filteredVerificationsArray[i].organism_id,
						verifications: [filteredVerificationsArray[i]]
					});
				}
			}
		} else {
			resultArray.push({
				organism_id: filteredVerificationsArray[i].organism_id,
				verifications: [filteredVerificationsArray[i]]
			});
		}
	}
	
	return resultArray;
}

/**
 * Loads the votes from other users for the current observation.
 */
function initializeVotesFromOtherUsers() {
	var container = $('#selectBoxContainer');
	var noVerificationsMessage = $('#noVerificationsMessage');
	
	container.html('');
	
	// get the verifications according to the current observation
	var currentVerifications = getVerificationsForObservation(observations[currentMainImageIndex].observation_id);
	
	if (currentVerifications.length > 0) {
		
		// display verification container
		noVerificationsMessage.hide();
		container.show();
		
		var totalVotes = currentVerifications.length;
		
		var verificationsPerOrganism = getVerificationsPerOrganism(currentVerifications);
		
		for (var i = 0; i < verificationsPerOrganism.length; i++) {
			var votesForCurrentOrganism = verificationsPerOrganism[i].verifications.length;
			var votesPercent = Math.round(votesForCurrentOrganism / totalVotes * 100, 0);
			
			var htmlString = "";
			htmlString +=		'<div class="entry">'
							  + '<div class="progressBar" style="width: ' + votesPercent + '%;">'
							  + '<span class="translatedDescription">' + verificationsPerOrganism[i].verifications[0].translated_name + '</span>'
							  + '<span class="latinDescription"><i>' + verificationsPerOrganism[i].verifications[0].scientific_name + '</i></span>'
							  + '<span class="votes">' + votesForCurrentOrganism + ' ' + generalInformation.translated_labels.verifications + '</span>'
							  + '</div>'
							  + '<div class="suggestButton">' + generalInformation.translated_labels.agree + '</div>'
							  + '<div class="entryCommentsContainer">';
			
			for (var a = 0; a < verificationsPerOrganism[i].verifications.length; a++) {
			htmlString +=		'<div class="entryComments">'
							  + '<p class="header"><b>' + verificationsPerOrganism[i].verifications[a].user_name + '</b> am ' + verificationsPerOrganism[i].verifications[a].vote_timestamp + ':</p>'
							  + '<p>' + verificationsPerOrganism[i].verifications[a].comment + '</p>'
							  + '</div>';
			}
			htmlString +=		'</div>'
							  + '</div>';
			
			container.html(htmlString);
		}
	} else {
		
		// hide container and display information message
		container.hide();
		noVerificationsMessage.html(generalInformation.translated_labels.noVerifications);
		noVerificationsMessage.show();
	}
	
	initializeSelectBox();
}