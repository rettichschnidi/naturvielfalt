$ = jQuery;

$(document).ready(function() {
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
});