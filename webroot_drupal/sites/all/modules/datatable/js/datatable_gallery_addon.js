/**
 * the "gallery_addon" integrates our drupal gallery into the flexigrid.
 * have a look at the datatable documentation to see how the gallery can be enabled
 */
jQuery(document).ready(function() {
	$ = jQuery;
	gallery_addon = {};
	// Amount of rows in image div
	var COL_COUNT = 4;

	/**
	 * is called by every refresh before the data is displayed
	 */
	gallery_addon.preProcess = function(gridid, json_item_name, data, flexigridOptions) {
		var $galleryTable = gallery_addon.__clearGalleryTable(gridid);
		
		if (data.total <= 0) {
			// rebuild flexigrid's 'no elements' div
			if ($galleryTable.siblings('div.nomsg').length <= 0) {
				$('<div class="nomsg">' + flexigridOptions.nomsg + '</div>')
					.insertAfter($galleryTable);
			}
			// set height of gallery div because table is empty
			$galleryDiv = gallery_addon.__getGalleryDiv(gridid).height(flexigridOptions.height);
			// adjust flexidiv blocker element's position due to missing column headers
			if (gallery_addon.__isGalleryActive(gridid)) {
				$gBlock = $galleryDiv.siblings('.gBlock');
				if ($gBlock.length > 0) {
					$gBlock.css({
						top: $galleryDiv.offset().top - $galleryDiv.offsetParent().offset().top
					});
				}
			}
			// let flexigrid finish
			return data;
		}
		
		var $tbody = $('<tbody />');				
		
		for (var i = 0; i <= data.rows.length / COL_COUNT; i++) {
			var $tr = $('<tr />');
			for (var j = 0; j < COL_COUNT; j++) {
				var index = i * COL_COUNT + j;
				var $td = $('<td class="datatable_gallery_row" />');

				if (data.rows[index] != undefined){
					if(data.rows[index].cell[json_item_name] != undefined){
						$td.html(data.rows[index].cell[json_item_name]);
					}
				}
				$tr.append($td);
			}
			$tbody.append($tr);
		}
	
		$tbody.appendTo(gallery_addon.__getGalleryTable(gridid));
		
		// Reregister lightbox
		gallery_lightbox.registerLightBox();
		
		return data;
	}
	
	/**
	 * displays the gallery or the datatable. 
	 * 
	 *  @param boolean enabled
	 *  	true -> show gallery
	 *  	false -> switch back to the datatable
	 */
	gallery_addon.toggleGallery = function(gridid, enabled) {
		// find the div of this flexigrid
		var $flexiDiv = $('#' + gridid).closest('div.flexigrid');
		
		$flexiDiv.children('.bDiv').toggle();
		$flexiDiv.children('.hDiv').toggle();
		if(enabled) {
			$flexiDiv.children('.cDrag').hide();
		} else {
			$flexiDiv.children('.cDrag').show();
		}
		gallery_addon.__getGalleryDiv(gridid).toggle();
		
		$gBlock = $flexiDiv.children('.gBlock');
		
		if(enabled) {
			$('#' + gridid + '_gallery_link').attr('disabled', 'disabled');
			$('#' + gridid + '_table_link').removeAttr('disabled');
			$('#' + gridid + '_gallery_image_source').closest('div').show();
			if ($gBlock.length > 0) {
				$gBlock.css({
					top: $('#' + gridid + '_gallery_images').offset().top - $flexiDiv.offset().top
				});
			}
			window.location.hash = 'gallery';
		}
		else {
			$('#' + gridid + '_table_link').attr('disabled', 'disabled');
			$('#' + gridid + '_gallery_link').removeAttr('disabled');
			$('#' + gridid + '_gallery_image_source').closest('div').hide();
			if ($gBlock.length > 0) {
				$gBlock.css({
					top: $('#' + gridid).offset().top - $flexiDiv.offset().top
				});
			}
			window.location.hash = '';
		}
	}
	
	/**
	 * is called when the user has chosen another imagesource.
	 * imagesouce = "Nur Portraitbilder", "Nur Belegbilder" ....
	 * 
	 * this method adds the chosen imagesource to the flexigrid parameters
	 * the php backend can then evaluate the "imagesource" parameter during the ajax calls
	 * 
	 * @param the <select> element
	 */
	gallery_addon.sourceChanged = function(gridid, element) {
		// add the chosen imagesource to the ajax request
		$("#" + gridid).flexOptions({params:[{name:'imagesource', value: element.value}]});
		$("#" + gridid).flexigrid().flexReload();
	}
	$('.form-item-image-source select').change(function() {
		var gridid = this.id.substring(0, this.id.length - "_gallery_image_source".length);
		gallery_addon.sourceChanged(gridid, this);
	});
	
	/**
	 * Removes all content of the gallery table, and returns the table jQuery object
	 */
	gallery_addon.__clearGalleryTable = function(gridid) {
		return gallery_addon.__getGalleryTable(gridid).empty();		
	};
	
	/**
	 * Returns (and creates if neccessary) the table for the gallery images as jQuery
	 * object
	 */
	gallery_addon.__getGalleryTable = function(gridid) {
		var $galleryDiv = gallery_addon.__getGalleryDiv(gridid);
		var $galleryTable = $galleryDiv.find('table.imgGallery');
		if ($galleryTable.length <= 0) {
			$galleryTable = $('<table class="imgGallery" />')
				.appendTo($galleryDiv);
		}
		return $galleryTable;
	};
	
	/**
	 * Returns (and creates if neccessary) the div for the gallery table as jQuery object
	 */
	gallery_addon.__getGalleryDiv = function(gridid) {
		var $galleryDiv = $('#' + gridid + '_gallery_images');
		if ($galleryDiv.length == 0) {
			$galleryDiv = $('<div id="' + gridid + '_gallery_images" class="datatable_gallery" />')
				.insertAfter($('#' + gridid).parent());
		}
		return $galleryDiv;
	};
	
	/**
	 * @returns true if the gallery is currently displayed
	 */
	gallery_addon.__isGalleryActive = function(gridid) {
		return $('#' + gridid + '_gallery_images:visible').length > 0;
	};
	
});