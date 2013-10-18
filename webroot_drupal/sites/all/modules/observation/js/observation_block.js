var container;
var wrapper;
var page = 1;
var pagesize = 4;
var total = 9007199254740992; //max int

jQuery(document).ready(function() {
	$ = jQuery;
	container = $('.new_observations_container');
	wrapper = $('.new_observations_wrapper');
	if(fetchPage(page)) {
		page++;
		initialize();
	};
	
});

function fetchPage(page) {
	if((page+1) * pagesize >= total) return false; //no more observations to fetch
	showLoading(true);
	$.ajax({
		type: "POST",
		url: "observation/block/newobservations",
		success: function(result){
			showLoading(false,function() {
				total = result.total;
				wrapper.append(result.content);
				initLightBox();
			});
		},
		error: function(result) {
			showLoading(false);
		},
		data: {
			page: page
		}
	});
	return true;
}

/**
 * Initialize the select box.
 */
function initialize() {
	var offset = 0; // current scrolling "amount"
	var scrollStep = 15;
	var elementSize = 235;
	var scrollUp = false;
	var scrolling = false;
	
	function scroll() {
	   wrapper.animate({
	    	"top": "-=" + offset,
	    }, 100, 'linear',function() {
	    	scrolling = false;
	    	var wrapperPostion = wrapper.position();
	    	if(scrollUp && wrapperPostion.top + offset >= 0) {
				offset = 0;
			}
		    if (container.height() + offset > ((page-1)*pagesize * elementSize)) {
		    	offset = 0;
		    }
	        if (offset != 0) {
	            scroll();
	        }
	    });
	}
	
	wrapper.mouseover(function(event) {
		if(scrolling) return;
		var containerOffset = container.offset(); 
    	var wrapperPostion = wrapper.position();
	    var relY = event.pageY - containerOffset.top;
		var containerMiddleY = container.height()/2.5;
		if(relY < containerMiddleY)  {
			if(!scrollUp) {
				scrollUp = true;
				offset = -scrollStep;
			}
			else offset -= (scrollStep);
		}
		else  {
			if(scrollUp) {
				scrollUp = false;
				offset = scrollStep;
			} else offset += (scrollStep);
		}
		if(scrollUp && wrapperPostion.top + offset >= 0) {
			offset = 0;
			return;
		}
	    if (container.height() + offset > ((page-1)*pagesize * elementSize)) {
	    	if(fetchPage(page)) {
	    		page++;
	    	} else {
	    		offset = 0;
	    		return;
	    	}
	    }
	    scroll();
	});
	
	wrapper.mouseout(function(event) {
		offset = 0;
		wrapper.stop(true, false);
	});
}


	
//	// define suggest button animation on mouse hover
//	$(".entry").hover(function() {
//		$(this).find(".new").stop(true, false).animate({ opacity: 1, right: 0 }, 200);
//		$(this).stop(true, true).animate({ height: $(this).find(".entryCommentsContainer").outerHeight() + $(this).height() }, 500);
//	}, function() {
//		$(this).find(".suggestButton").stop(true, false).animate({ opacity: 0, right: -120 }, 200);
//		$(this).stop(true, true).animate({ height: $(this).height() - $(this).find(".entryCommentsContainer").outerHeight() }, 500);
//	});
//	
//	//	add event to accept buttons
//	$(".suggestButton").click(function() {
//		$('#artgroup_id').val($(this).parent().find("#hiddenArtgroupId").val());
//		var organism_name = $(this).parent().find(".translatedDescription").html();
//		if(organism_name == Drupal.t("No translation available")) //if no translation is available, use latin name
//			organism_name = $(this).parent().find(".latinDescription").text();
//		$('#organism_name').val(organism_name);
//		$('html, body').animate({ scrollTop: $("#ownVerification").offset().top });
//	});
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

function showLoading(display, completeCallback) {; 
	if(display) {
	//	$('#showLoadingImage').show();
		if(completeCallback)
			container.fadeTo("slow", 0.5, completeCallback); 
		else 
			container.fadeTo("slow", 0.5); 
	} else {
	//	 $('#showLoadingImage').hide();
		 if(completeCallback)
			 container.fadeTo("slow", 1, completeCallback); 
		 else 
			 container.fadeTo("slow", 1); 
	}
}