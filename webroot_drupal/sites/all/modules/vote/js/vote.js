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
	generalInformation = result.general;
	
	// cache observation information
	if(result.observations != null) {
		for (var i = 0; i < result.observations.length; i++) {
			observations[i] = result.observations[i];
		}
	}
	
	// cache verification information
	if(result.verifications != null) {
		for (var i = 0; i < result.verifications.length; i ++) {
			verifications[i] = result.verifications[i];
		}
	}
}

/**
 * Initializes the submit verification button.
 */
function initializeSubmitVerificationButton() {
	$('#submitVerification').click(function() {
		$.ajax({
			type: "POST",
			url: "/vote/save",
			success: function(result){
				// scroll to the top of the page to display the next observation
				$('html, body').animate({ scrollTop: 0 });
				
				var currentVerifications = getVerificationsForObservation(observations[currentMainImageIndex].observation_id);
				
				if (result.name_lang == null) {
					result.name_lang = Drupal.t("No translation available");
				}
				var myVote = {
					comment: $('#comment').val(),
					observation_id: observations[currentMainImageIndex].observation_id.toString(),
					organism_id: result.id,
					scientific_name: result.name_lat,
					translated_name: result.name_lang,
					user_id: generalInformation.current_user_id.toString(),
					user_name: generalInformation.current_user_name,
					vote_timestamp: Math.round((new Date()).getTime() / 1000).toString() 
				};
				
				if(currentVerifications == null) {
					currentVerifications = [myVote];
				} else {
					for(var i = 0; i < currentVerifications.length; i++) {
						if(currentVerifications[i].user_id == generalInformation.current_user_id) {
							currentVerifications.splice(i, 1);
						}
					}
					currentVerifications.push(myVote);
				}
				
				setVerificationsForObservation(observations[currentMainImageIndex].observation_id, currentVerifications);
				
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

function setVerificationsForObservation(observation_id, newVerificationsArray) {
	for (var i = 0; i < verifications.length; i ++) {
		if (verifications[i].observation_id == observation_id) {
			verifications.splice(i, 1);
		}
	}
	
	for (var i = 0; i < newVerificationsArray.length; i ++) {
		verifications.push(newVerificationsArray[i]);
	}
}

function getVerificationsPerOrganism(filteredVerificationsArray) {
	var alreadyPushedOrganismIds = [];
	var resultArray = [];
	
	for (var i = 0; i < filteredVerificationsArray.length; i++) {
		var currentOrganismId = filteredVerificationsArray[i].organism_id;
		if (alreadyPushedOrganismIds.indexOf(currentOrganismId) == -1) {
			resultArray.push({
				organism_id: currentOrganismId,
				verifications: [filteredVerificationsArray[i]]
			});
			alreadyPushedOrganismIds.push(currentOrganismId);
		} else {
			for (var b = 0; b < resultArray.length; b++) {
				if (currentOrganismId == resultArray[b].organism_id) {
					resultArray[b].verifications.push(filteredVerificationsArray[i]);
					break;
				}
			}
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
		var htmlString = "";
		
		for (var i = 0; i < verificationsPerOrganism.length; i++) {
			var votesForCurrentOrganism = verificationsPerOrganism[i].verifications.length;
			var votesPercent = Math.round(votesForCurrentOrganism / totalVotes * 100, 0);
			
			htmlString +=		'<div class="entry">'
							  + '<div class="progressBar" style="width: ' + votesPercent + '%;">'
							  + '<span class="translatedDescription">' + verificationsPerOrganism[i].verifications[0].translated_name + '</span>'
							  + '<span class="latinDescription"><i>' + verificationsPerOrganism[i].verifications[0].scientific_name + '</i></span>'
							  + '<span class="votes">' + votesForCurrentOrganism + ' ' + Drupal.t('verifications') + '</span>'
							  + '</div>'
							  + '<div class="suggestButton">' + Drupal.t('Agree') + '</div>'
							  + '<div class="entryCommentsContainer">';
			
			for (var a = 0; a < verificationsPerOrganism[i].verifications.length; a++) {
				htmlString +=		'<div class="entryComments">'
								  + '<p class="header"><b>' + verificationsPerOrganism[i].verifications[a].user_name + '</b> am ' + new Date(verificationsPerOrganism[i].verifications[a].vote_timestamp * 1000).format("d.m.Y H:i") + ':</p>'
								  + '<p>' + verificationsPerOrganism[i].verifications[a].comment + '</p>'
								  + '</div>';
			}
			
			htmlString +=		'</div>'
							  + '</div>';
		}
		container.html(htmlString);
	} else {
		
		// hide container and display information message
		container.hide();
		noVerificationsMessage.html(Drupal.t('No verifications found for this observation.'));
		noVerificationsMessage.show();
	}
	
	initializeSelectBox();
}

//Simulates PHP's date function
Date.prototype.format=function(e){var t="";var n=Date.replaceChars;for(var r=0;r<e.length;r++){var i=e.charAt(r);if(r-1>=0&&e.charAt(r-1)=="\\"){t+=i}else if(n[i]){t+=n[i].call(this)}else if(i!="\\"){t+=i}}return t};Date.replaceChars={shortMonths:["Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"],longMonths:["January","February","March","April","May","June","July","August","September","October","November","December"],shortDays:["Sun","Mon","Tue","Wed","Thu","Fri","Sat"],longDays:["Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"],d:function(){return(this.getDate()<10?"0":"")+this.getDate()},D:function(){return Date.replaceChars.shortDays[this.getDay()]},j:function(){return this.getDate()},l:function(){return Date.replaceChars.longDays[this.getDay()]},N:function(){return this.getDay()+1},S:function(){return this.getDate()%10==1&&this.getDate()!=11?"st":this.getDate()%10==2&&this.getDate()!=12?"nd":this.getDate()%10==3&&this.getDate()!=13?"rd":"th"},w:function(){return this.getDay()},z:function(){var e=new Date(this.getFullYear(),0,1);return Math.ceil((this-e)/864e5)},W:function(){var e=new Date(this.getFullYear(),0,1);return Math.ceil(((this-e)/864e5+e.getDay()+1)/7)},F:function(){return Date.replaceChars.longMonths[this.getMonth()]},m:function(){return(this.getMonth()<9?"0":"")+(this.getMonth()+1)},M:function(){return Date.replaceChars.shortMonths[this.getMonth()]},n:function(){return this.getMonth()+1},t:function(){var e=new Date;return(new Date(e.getFullYear(),e.getMonth(),0)).getDate()},L:function(){var e=this.getFullYear();return e%400==0||e%100!=0&&e%4==0},o:function(){var e=new Date(this.valueOf());e.setDate(e.getDate()-(this.getDay()+6)%7+3);return e.getFullYear()},Y:function(){return this.getFullYear()},y:function(){return(""+this.getFullYear()).substr(2)},a:function(){return this.getHours()<12?"am":"pm"},A:function(){return this.getHours()<12?"AM":"PM"},B:function(){return Math.floor(((this.getUTCHours()+1)%24+this.getUTCMinutes()/60+this.getUTCSeconds()/3600)*1e3/24)},g:function(){return this.getHours()%12||12},G:function(){return this.getHours()},h:function(){return((this.getHours()%12||12)<10?"0":"")+(this.getHours()%12||12)},H:function(){return(this.getHours()<10?"0":"")+this.getHours()},i:function(){return(this.getMinutes()<10?"0":"")+this.getMinutes()},s:function(){return(this.getSeconds()<10?"0":"")+this.getSeconds()},u:function(){var e=this.getMilliseconds();return(e<10?"00":e<100?"0":"")+e},e:function(){return"Not Yet Supported"},I:function(){var e=null;for(var t=0;t<12;++t){var n=new Date(this.getFullYear(),t,1);var r=n.getTimezoneOffset();if(e===null)e=r;else if(r<e){e=r;break}else if(r>e)break}return this.getTimezoneOffset()==e|0},O:function(){return(-this.getTimezoneOffset()<0?"-":"+")+(Math.abs(this.getTimezoneOffset()/60)<10?"0":"")+Math.abs(this.getTimezoneOffset()/60)+"00"},P:function(){return(-this.getTimezoneOffset()<0?"-":"+")+(Math.abs(this.getTimezoneOffset()/60)<10?"0":"")+Math.abs(this.getTimezoneOffset()/60)+":00"},T:function(){var e=this.getMonth();this.setMonth(0);var t=this.toTimeString().replace(/^.+ \(?([^\)]+)\)?$/,"$1");this.setMonth(e);return t},Z:function(){return-this.getTimezoneOffset()*60},c:function(){return this.format("Y-m-d\\TH:i:sP")},r:function(){return this.toString()},U:function(){return this.getTime()/1e3}}