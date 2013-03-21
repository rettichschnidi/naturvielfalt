jQuery(document).ready(function() {
	$ = jQuery;

   $('.qtip_element').qtip({
      position: {
         corner: {
            target: 'topRight',
            tooltip: 'bottomLeft'
         }
      },
      style: {
         name: 'cream',
         padding: '7px 13px',
         width: {
            max: 210,
            min: 0
         },
         tip: true
      },
      show: {
    	  delay: 0
      }
   });
   
   /*
   $("#attributes_Bemerkung,#attributes_Positionsgenauigkeit").hover(function(){
	    alert("here!");
	},function(){
	    
	});
	*/
   
   $('#attributes_Bemerkung').qtip({  
	   	content: $('div#qtipBemerkung').attr("title"),
	      position: {
	         corner: {
	            target: 'topRight',
	            tooltip: 'bottomLeft'
	         }
	      },
	      style: {
	         name: 'cream',
	         padding: '7px 13px',
	         width: {
	            max: 210,
	            min: 0
	         },
	         tip: true
	      },
	      show: {
	    	  delay: 0
	      }
	   });
   
   $('#attributes_Positionsgenauigkeit').qtip({  
	   	content: $('div#qtipPosition').attr("title"),
	      position: {
	         corner: {
	            target: 'topRight',
	            tooltip: 'bottomLeft'
	         }
	      },
	      style: {
	         name: 'cream',
	         padding: '7px 13px',
	         width: {
	            max: 210,
	            min: 0
	         },
	         tip: true
	      },
	      show: {
	    	  delay: 0
	      }
	   });
   
});