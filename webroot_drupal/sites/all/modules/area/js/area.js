/**
 * @author Reto Schneider, 2012
 * @copyright Naturwerk
 * @file area.js
 */

jQuery(document).ready(function() {
	$ = jQuery;
	area = {};
	area.message = $('#message');
	minZoom = 2;
	//bounds around europe
	//no longer wanted by albert
//	europeanBounds = new google.maps.LatLngBounds(
//		     new google.maps.LatLng(36.173357,-5.58838), 
//		     new google.maps.LatLng(59.601095,24.891357)
//		   );

	/**
	 * Show a loading indicator
	 */
	area.showLoading = function() {
		if (!area.loading) {
			area.loading = $('<div><img src="' + Drupal.settings.basePath + 'sites/all/modules/commonstuff/images/loading.gif" /></div>').hide();
			$('body').append(area.loading);
		}
		area.loading.dialog({
			modal: true,
			draggable: false,
			resizable: false,
			closeOnEscape: false,
			closeText: '',
			dialogClass: 'loading'
		});
	};
		
	/**
	 * Hide the loading indicator
	 */
	area.hideLoading = function() {
		if (!area.loading) return;
		area.loading.dialog('close');
	};
	
	/**
	 * Filter the area list by access right
	 */
	area.aclFilter = function(tableId, filter) {
		var url = $('#' + tableId)[0].p.url;
		// replace string after last slash and preserve query string
		url = url.replace(/(.+\/)[^?]+(.*)/, '$1' + filter + '$2');
		$('#' + tableId).flexOptions({
			url: url
		}).flexReload();
	}
	$('.acl_filter').change(function(event) {
		var tableId = $(event.target).closest('div.flexigrid').find('div.bDiv table').first().attr('id');
		var filter = $(this).val();
		area.aclFilter(tableId, filter);
	});
	
	/**
	 * Export the selected rows
	 * 
	 * @param string tableId
	 */
	area.exportSelectedRows = function(tableId) {
		url = Drupal.settings.basePath + 'area/export?';
		var $table = $('#' + tableId);
		if ($table.length < 1)
			return false;
		var $flexidiv = $table.closest('div.flexigrid');
		
		var areas = '';
		var $num_areas = 0;
		$table.find('input.gridSelect').each(function() {
			if (this.checked) {
				var area_id = $(this).val();
				areas +=area_id + ',';
				$num_areas++;
			}
		});
		
		if (areas.length == '') {
			alert(Drupal.t('No records selected'));
			return false;
		}
		areas = '(' +areas.substring(0,areas.length-1) + ')';
		
		// pass params
		var gridPrefs = $table[0].p;
		url += '&areas=' + areas + '&shape=' + "mixed";
		
		var really = confirm('Es werden ' + $num_areas + ' Gebiete exportiert!');
		if (really){
			// load export url in a hidden iframe to get a download prompt
			var $status = $flexidiv.find('.pPageStat');
			var oldStatus = $status.text();
			$status.text(gridPrefs.procmsg);
			$('<iframe />').attr('src', url).hide().appendTo('body').load(function() {
				$status.text(oldStatus);
		});
		}
	};
	
	/**
	 * Toggle the selected rows
	 * 
	 * @param string tableId
	 */
	area.toggleSelectedRows = function(event, status) {
		$flexiDiv = $(event.target).closest('div.flexigrid');
		$flexiDiv.find('input.gridSelect:visible').each(function() {
			this.checked = status;
			areamap.selectMultipleAreas(event, parseInt($(this).val()), !status);
		});
	};
	
	/**
	 * Adds selected observations to an inventory. Overrides observations which already belong to an inventory.
	 * If none are selected, all rows will be verified.
	 * 
	 * @param string tableId
	 */
	area.addSelectedRowsToArea = function(tableId) {
		var table = $('#' + tableId);
		if (table.length < 1)
			return false;
		var area_ids = [];
		table.find('input.gridSelect').each(function() {
			if (this.checked) {
				area_ids.push($(this).val());
			}
		});
		area.showLoading();
		$.ajax({
			type: "POST",
			url: Drupal.settings.basePath + "area/addtoareadata",
			success: function(result){
				var areas = result.areas;
				var count = result.count;
				var areasAsOptions = result.areas_as_options;
				
				area.hideLoading();
				dialog = $('<div id="add-to-area-wrapper" title="' + Drupal.t('Add to Area (Subarea)') + '" />');
				
				//if no areas exists to add an observation to, display a message.
				if(!count) {
					dialog.append(Drupal.t('There are no areas available, </br> on which you are allowed to add a subarea.'));
					dialog.dialog({
						modal: true,
						resizable: false,
						closeOnEscape: false,
						closeText: '',
						close: function (event, ui) {
							$(this).remove();
						},
						width: 'auto'
					});
					return;
				}
				//area title and select
				areaTitle = $('<label>' + Drupal.t('Area') + '</label>');
				areaSelect = $('<select id="area-select" class="form-select" \>');
				var i = 0;
				$.each(areasAsOptions, function(key, value) {
					if(i++ == 0) {
						inventories = areas[key].inventories;
					}
					areaSelect.append($("<option></option>")
				     .attr("value", key).text(value));
				});
				
				//saveButton
				saveButton = $('<input class="form-submit" type="button" value="' + Drupal.t('Save') + '" />');
				
				//saveButton onclick
				saveButton.off().on('click', function() {
					var really = confirm(Drupal.t('All selected areas will become a subarea of this area. Continue?'));
					if(really) {
						var areaId = areaSelect.val();
						area.showLoading();
						//make ajax call to save the areas as subarea
						$.ajax({
							type: "POST",
							url: Drupal.settings.basePath + "area/addtoarea/" + areaId,
							success: function(result){
								area.hideLoading();
								//remove dialog, show response, and reload table
								dialog.remove();
								var translations = {};
								translations['@successfull'] = result.successfull;
								translations['@total'] = result.total;
								if(result.success) {
									area.setMessage(Drupal.t('@successfull of @total areas were added as subareas.', translations), 'status', 5000);
									table.flexReload();
								}	
								else {
									area.setMessage(Drupal.t('@successfull of @total areas were added as subareas.', translations), 'warning', 5000);
								} 
								
							},
							error: function(result) {
								area.hideLoading();
								dialog.remove();
								area.setMessage(Drupal.t('Could not add areas as subareas.'), 'error', 5000);
							},
							data: {
								areas: area_ids
							}
						});
					}
				});
				
				//open dialog with selects
				dialog.append(areaTitle, areaSelect, '</br></br>', saveButton);
				dialog.dialog({
					modal: true,
					resizable: false,
					closeOnEscape: false,
					closeText: '',
					close: function (event, ui) {
						$(this).remove();
					},
					width: 'auto'
				});
			},
			error: function(result) {
				area.setMessage(Drupal.t('Could not fetch area data.'), 'error', 5000);
				area.hideLoading();
			},
			data: {
				areas: area_ids
			}
		});
		
	};
	
	/**
	 * Delete the selected rows
	 * 
	 * @param string tableId
	 */
	area.deleteSelectedRows = function(tableId) {
		
		var $table = $('#' + tableId);
		if ($table.length < 1)
			return false;
		
		var pathname = window.location.pathname;
		var area_edit_id = pathname.match(/area\/(\d+)\/edit/);
		if (area_edit_id != null) {
			if (area_edit_id.length >= 2)
				area_edit_id = area_edit_id[1];
		}
		else
			area_edit_id = 0;
		
		var redirect = false;
		var transportData = '';
		$table.find('input.gridSelect').each(function() {
			if (this.checked) {
				var area_id = $(this).val();
				transportData +=area_id + ',';
				if (area_id == area_edit_id)
					redirect = true;
			}
		});
		
		if (transportData.length == '') {
			alert(Drupal.t('No records selected'));
			return false;
		}
		transportData = '{' + transportData.substring(0,transportData.length-1) + '}';
			
		var really = confirm(Drupal.t('Do you really want to delete the selected records?'));
		if (really){
			var data = {
				areas: transportData
			};
			
			var ajaxurl = Drupal.settings.basePath + 'area/delete';

			$.getJSON(ajaxurl, data, function(json){
				if(json.count > 0) $table.flexReload();
				area.showDeleteResponse(json);
				if (redirect) {
					$.safetynet.suppressed(true);
					window.location.href = '/areas/show';
				}
			});
		}
	};
	
	area.deleteInventory = function(inventoryID) {
		var inventoryTable = $('#' + 'inventories');
		var observationTable = $('#' + 'observations');
		
		var pathname = window.location.pathname;
		pathname = pathname.split('/');
		var really = confirm(Drupal.t('Do you really want to delete this inventory?'));
		if (really){
			
			var ajaxurl = Drupal.settings.basePath + 'inventory/' + inventoryID + '/delete';
			$.getJSON(ajaxurl, null, function(json){
				if(json.count > 0) {
					if(pathname.length > 4) {
						pathname.pop();
						pathname = pathname.join('/');
						window.location.pathname = pathname;
					} else {
						inventoryTable.flexReload();
						observationTable.flexReload();
					}
				}
				area.setMessage(json.message, json.type, 5000);
			});
		}
	};
	
	
	
	/**
	 * Show the message returned from the deletion request
	 */
	area.showDeleteResponse = function(responseText, statusText, xhr, $form)  {
		if(responseText != null && responseText.success == true){
			area.setMessage(responseText.message, responseText.type, 5000);
		} else if (responseText != null) {
			area.setMessage('&bull;&nbsp;' + responseText.message.join("<br>&bull;&nbsp;"), 'error', 5000);
		} else {
			area.setMessage('&bull;&nbsp;' + Drupal.t('Deletion failed due to unknown error.'), 'error', 5000);
		}
	};
	
	/**
	* Show a top message banner
	*
	* @param string message
	* @param string type
	* @param int time
	*/
	area.setMessage = function(message, type, time) {
		if(area.messageTimer) window.clearTimeout(area.messageTimer);
		area.message.children('.messages').html(message).attr('class', 'messages').addClass(type);
		area.message.stop().css('height', 'auto').slideDown('fast');
		if(time) area.messageTimer = window.setTimeout(function () {
			area.message.slideUp('fast');
		}, time);
	
		// scroll to message
		$('body,html').animate({
		scrollTop: area.message.offset().top
		});
	};
	
	/**
	 * Executed after table is populated
	 * 
	 * @param jQuery object flexigrid
	 */
	area.onTableSuccess = function(flexigrid) {
		var tableId = $(flexigrid.bDiv).find('table').first().attr('id');
		area.displayBatchArea(tableId);
	};
	
	/**
	 * Executed before table is populated
	 * ! must return the data !
	 * 
	 * @param JSON object data
	 */
	area.tablePreProcess = function(data, flexigridOptions) {
		var area_ids = [];
		$(data.rows).each(function(idx, area) {
			area_ids[idx] = area.id;
		});
		area.syncMapWithTable(area_ids);

		data = gallery_addon.preProcess(flexigridOptions.tableId, 'gallery_image', data, flexigridOptions);
		
		return data;
	};
	
	/**
	 * toogles (show/hide) the children rows of an area
	 * 
	 * @param tableid
	 * @param area_id the parent area of the childrenrows to display/hide.
	 */
	area.toogleChildren = function(tableid, area_id) {
		var table = $('#' + tableid);
		if(!table.length) return;
		//check wether hierarchy is open or closed
		var showChildren = parseInt($('#showChildren' + area_id).val());
		//define the image, depending on the current status (open or closed)
		var imageName = showChildren ? 'Forward.png' : 'Down.png';
		$('#toogleChildren' + area_id)
	    .attr('src', Drupal.settings.basePath + 'sites/all/themes/zen_swissmon/images/icons/enabled/' + imageName);
 		$.ajax({
			type: 'GET',
			url: Drupal.settings.basePath + 'area/' + area_id + '/getchildren',
			success: function (result) {
				jQuery.each(result.children,function(index, element) {
					table.toogleRowVisibility(element.id, !showChildren);
				});
				//save new status of hierarchy (open or closed)
				$('#showChildren' + area_id).val(!showChildren ? 1 : 0);
			}
		});
	};
	
	/**
	 * Update the map to show only overlays of the given data.
	 * 
	 * @param array(int) area_ids
	 */
	area.syncMapWithTable = function(area_ids) {
		if (areamap == undefined)
			return false;
		
		areamap.clearOverlays();
		
		if (area_ids.length < 1)
			return true;
		
		$.ajax({
			type: 'POST',
			url: areamap.options.geometriesfetchurl,
			data: {
				area_ids: area_ids
			},
			success: function (data) {
				areamap.loadGeometriesAndOverlaysFromJson(data);

				var mapcontains = true;
				var bounds = new google.maps.LatLngBounds();
				for (var i = 0; i < area_ids.length; i++) {
					var id = area_ids[i];
					
					if (!(id in areamap.overlaysArray))
						continue;
					
					var item = areamap.overlaysArray[id];
					
					if(areamap.geometriesArray[id].type == 'polygon' 
						|| areamap.geometriesArray[id].type == 'polyline')
					{
			
						var positions = item.getPath();
						var position; 
						
						for(var k = 0; k < positions.length; k++) {
							var tmp = positions.getAt(k);
							position = new google.maps.LatLng(tmp.lat(), tmp.lng());
							bounds.extend(position);
							// don't touch mapcontains once it changed to false
							if (mapcontains)
							{
								mapcontains = areamap.googlemap.getBounds().contains(position);
							}
						}
					}
					//overlay type marker
					else {
						var position = item.getPosition();
						bounds.extend(position);
						// don't touch mapcontains once it changed to false
						if (mapcontains)
						{
							mapcontains = areamap.googlemap.getBounds().contains(position);
						}
					}
				}
				if (!mapcontains)
					areamap.googlemap.fitBounds(bounds);
				
				return true;
			}
		});
	}
	
	/**
	 * Display the batch div, holding the select-toggle, delete and export buttons
	 */
	area.displayBatchArea = function(tableId) {
		$flexiDiv = $('#' + tableId).closest('div.flexigrid');
		
		if ($flexiDiv.find('.batch-div').length == 0) {
			$checkbox = $('<input type="checkbox" />');
			$btnDeleteSelected = $('<input type="button" class="btnDeleteSelected" disabled="true" value="' + Drupal.t('Delete') + '" />');
			$btnExportSelected = $('<input type="button" class="btnExportSelected" value="' + Drupal.t('Export all') + '" />');
			$btnAddToArea = $('<input type="button" class="btnAddToArea" disabled="true" value="' + Drupal.t('Add to Area (Subarea)') + '" />');
			$batchDiv = $('<div class="batch-div" />')
				.append($checkbox, $btnDeleteSelected, $btnExportSelected, $btnAddToArea)
				.insertBefore($flexiDiv.find('.sDiv'));
			
			$checkbox.click(function(event) {
				area.toggleSelectedRows(event, this.checked);
			});
			$btnDeleteSelected.click(function() {
				area.deleteSelectedRows(tableId);
			});
			$btnExportSelected.click(function() {
				area.exportSelectedRows(tableId);
			});
			$btnAddToArea.click(function() {
				area.addSelectedRowsToArea(tableId);
			});
		}
		else {
			$flexiDiv.find('.btnDeleteSelected').attr('disabled', true);
			$flexiDiv.find('.btnExportSelected').val(Drupal.t('Export all'));
			$flexiDiv.find('.btnAddToArea').attr('disabled', true);
		}
	};
	
});

/**
 * @Class Contains all the logic to handle a map.
 */
function Area(options) {
	$ = jQuery;
	
	/**
	 * Member variable initialisation
	 * Short explanation:
	 *  - A geometry is the JSON representation of the following data:
	 *  
	 *    Column  |   Type   |                             Modifiers                             
	 *  ----------+----------+-------------------------------------------------------------------
	 *   id       | integer  | not null default nextval('drupal_area_geometry_id_seq'::regclass)
	 * 	 altitude | integer  | default 0
	 *   township | text     | 
	 * 	 zip      | text     | 
	 * 	 canton   | text     | 
	 * 	 country  | text     | 
	 * 
	 *  - an overlay is a Google maps element (Marker, Polyline, Polygon)
	 */
	// save the options for later usage
	this.options = jQuery.extend({
		geometries_autoload: true
	}, options);
	// map holds the google maps object
	this.googlemap = null;
	// holds the id of the currently selected overlays/geometrys, for single select
	this.selectedId = null;
	// holds the ids of the currently selected overlays/geometrys, for multiple select
	this.selectedIdsArray = null;
	// points to last created element
	this.newOverlay = null;
	// contains all the overlays currently shown on the map
	this.overlaysArray = [];
	this.geometriesArray = [];
	
	// pin colors
	this.pinColor = {
		'blue': '6F98FF',
		'green': '34BA46',
		'red': 'FE7569'
	};

	// there is just one at a time
	this.infoWindow = null;
	this.drawingManager = null;
	this.automaticSaveLocationListener = null;
	this.ch1903MapChangeListener = null;
	this.mapTypeSwitchListener = null;
	
	// Initialize a map
	this.googlemap = new google.maps.Map(document
			.getElementById(this.options.canvasid), this.options.googlemapsoptions);
	// if something bad happened during creation, just return false
	if (!this.googlemap) {
		console.error("Could not initialize maps");
		return false;
	}
	// enable the extensions (if requested)
	this.mapTypeSwitch(this.options.mapswitch);
	this.loadLastLocation(this.options.loadlastlocation);
	this.automaticallySaveLocation(this.options.savelastlocation);
	this.createSearchbar(this.options.search);
	this.createSearchbarCH1903(this.options.ch1903);
	this.createDrawingManager(this.options.drawingmanager);
	this.createReticle(this.options.reticle);

	// load data into map
	if(this.options.geometries_autoload && this.options.geometriesfetchurl.length > 0) {
		var this_ = this;
		jQuery.getJSON(this.options.geometriesfetchurl,
				function(data) {
					this_.loadGeometriesAndOverlaysFromJson(data);
					if(this_.options.geometryedit) {
						this_.geometryEdit(this_.options.geometryeditid);
					}
					if(this_.options.showandcenter || this_.options.geometryeditid)
						this_.showAndCenter(this_.options.geometryeditid);
				}
			);
	}
	
	this.createDrawingManagerGetcoordinate(this.options.getcoordinate);

};

/**
 * As requested by Albert:
 * 	Automatically switch to ROADMAP/HYBRID map when below/above zoomlevel 12
 * Build bounds around europe, the map should only show europe
 */
Area.prototype.mapTypeSwitch = function(enable) {
	var googlemap = this.googlemap;
	if (arguments.length == 0 || enable) {
		if(this.mapTypeSwitchListener != null) {
			this.mapTypeSwitch(false);
		}
		this.mapTypeSwitchListener = google.maps.event.addListener(this.googlemap, 'zoom_changed', function() {
			if (googlemap.getZoom() < minZoom) 
				googlemap.setZoom(minZoom);
			if(googlemap.getZoom() >= 12) {
				googlemap.setMapTypeId(google.maps.MapTypeId.HYBRID);
			}
			if(googlemap.getZoom() < 12) {
				googlemap.setMapTypeId(google.maps.MapTypeId.ROADMAP);
			}
		});
		// focus on map on europ
		//no longer wanted by albert
//		   google.maps.event.addListener(this.googlemap, 'center_changed', function() {
//		     if (europeanBounds.contains(googlemap.getCenter())) return;
//
//		     // We're out of switzerlands bounds - Move the map back within the bounds
//
//		     var c = googlemap.getCenter(),
//		         x = c.lng(),
//		         y = c.lat(),
//		         maxX = europeanBounds.getNorthEast().lng(),
//		         maxY = europeanBounds.getNorthEast().lat(),
//		         minX = europeanBounds.getSouthWest().lng(),
//		         minY = europeanBounds.getSouthWest().lat();
//
//		     if (x < minX) x = minX;
//		     if (x > maxX) x = maxX;
//		     if (y < minY) y = minY;
//		     if (y > maxY) y = maxY;
//
//		     googlemap.setCenter(new google.maps.LatLng(y, x));
//		   });
	} else {
		google.maps.event.removeListener(this.mapTypeSwitchListener);
		this.mapTypeSwitchListener = null;
	}
};

/**
 * Load the last saved location.
 * 
 * @param enable
 * 	Load last boundaries only if enable is true
 * @see automaticallySaveLocation(boolean)
 */
Area.prototype.loadLastLocation = function(enable) {
	var googlemap = this.googlemap;
	if(enable) {
		
		if (window.localStorage) {
			var bounds = window.localStorage.getItem('naturvielfalt_ne_lat');
			if (bounds != null) {
				var ls = window.localStorage;
				var ne_lat = ls.getItem('naturvielfalt_ne_lat');
				var ne_lng = ls.getItem('naturvielfalt_ne_lng');
				var sw_lat = ls.getItem('naturvielfalt_sw_lat');
				var sw_lng = ls.getItem('naturvielfalt_sw_lng');

				var ne = new google.maps.LatLng(ne_lat, ne_lng);
				var sw = new google.maps.LatLng(sw_lat,	sw_lng);
				
				/**
				 * This is an ugly hack, but is needed due to the fact that Google
				 * adds some space to fitBounds and because of this the map would
				 * zoom out at each refresh otherwise.
				 * 
				 * This will probably not work in any case, but it is by far better
				 * than nothing.
				 */
				var factor =  0.25;
				var new_ne = google.maps.geometry.spherical.interpolate(ne, sw, factor);
				var new_sw = google.maps.geometry.spherical.interpolate(ne, sw, 1 - factor);

				bounds = new google.maps.LatLngBounds(new_sw, new_ne);
				googlemap.fitBounds(bounds);
			}
		} else {
			console.error("Local storage not available. Last seen boundaries can not be loaded.");
		}
	}
};

/**
 * At each map change, save the current position in the users localStorage
 * if possible. The used localStorage keys are:
 * 	- naturvielfalt_ne_lat
 *  - naturvielfalt_ne_lng
 *  - naturvielfalt_sw_lat
 *  - naturvielfalt_sw_lng
 * 
 * @param enable
 *	If true, enable automatically saving. Otherwise disable it.
 */
Area.prototype.automaticallySaveLocation = function(enable) {
	if (arguments.length == 0 || enable) {
		// store location whenever bounds change
		if (window.localStorage) {
			var googlemap = this.googlemap;
			this.automaticSaveLocationListener = google.maps.event
					.addListener(googlemap, 'bounds_changed', function() {
						var b = googlemap.getBounds();
						var ne = b.getNorthEast();
						var sw = b.getSouthWest();
						window.localStorage.setItem('naturvielfalt_ne_lat',
								ne.lat());
						window.localStorage.setItem('naturvielfalt_ne_lng',
								ne.lng());
						window.localStorage.setItem('naturvielfalt_sw_lat',
								sw.lat());
						window.localStorage.setItem('naturvielfalt_sw_lng',
								sw.lng());
					});
		}
	} else {
		// remove bounds change listener
		if (this.automaticSaveLocationListener != 'undefined') {
			google.maps.event
					.removeListener(this.automaticSaveLocationListener);
		}
	}
};

/**
 * Select the geometry with the given id, set style to "selected", set map to
 * show the given element
 * 
 * @param id integer
 * 	geometry id
 */
Area.prototype.selectGeometry = function(geometryid) {
	if (geometryid in this.geometriesArray) {
		if (this.selectedId != null)
			this.overlaysArray[this.selectedId].deselect();
		this.selectedId = geometryid;

		var item = this.overlaysArray[geometryid];
		item.select();
		this.deselectOtherGeometries(item);
		
	} else {
		console.error("Geometry not available: " + geometryid);
		this.selectedId = null;
	}
};


Area.prototype.deselectOtherGeometries = function(current) {
	for (var i = 0; i < this.overlaysArray.length; i++) {
		var overlay = this.overlaysArray[i];
		if(overlay && overlay != current)
		{
			overlay.deselect();					
		}
	}
};

Area.prototype.zoomIn = function(event, id) {
	var e = event || window.event;
	if (e != undefined)	e.stopPropagation();
	
	this.selectGeometry(id);
	var bounds = this.overlaysArray[id].getBounds();
	this.googlemap.fitBounds(bounds);
	
	if(this.googlemap.getZoom() > 19)
	{
		this.googlemap.setZoom(19);		
	}
};

Area.prototype.selectMultipleAreas = function(event, id, deselect) {
	var e = event || window.event;
	if (e != undefined)	e.stopPropagation();
	
	if (deselect == undefined)
		deselect = false;
	
	$flexiDiv = $(event.target).closest('div.flexigrid');
	
	// select both checkboxes in table/gallery
	$flexiDiv.find('input.gridSelect[value="'+id+'"]').each(function() {
		$(this)[0].checked = !deselect;
	});
	
	// update batch area buttons
	if ($flexiDiv.find('input.gridSelect:checked').length == 0) {
		$flexiDiv.find('.btnDeleteSelected').attr('disabled', true);
		$flexiDiv.find('.btnExportSelected').val(Drupal.t('Export all'));
		$flexiDiv.find('.btnAddToArea').attr('disabled', true);
	}
	else {
		$flexiDiv.find('.btnDeleteSelected').removeAttr('disabled');
		$flexiDiv.find('.btnExportSelected').val(Drupal.t('Export selected'));
		$flexiDiv.find('.btnAddToArea').removeAttr('disabled');
	}
	
	// check if there exists an overlay with the given id
	if (id in this.overlaysArray) {
		if (this.selectedIds == null) {
			this.selectedIds = [];
		}
		if (deselect == true) {
			// deselect
			var idx = $.inArray(id, $.unique(this.selectedIds));
			if (idx > -1) {
				this.selectedIds.splice(idx, 1);
				//this.overlaysArray[id].deselect(this.pinColor.red);
				item = this.overlaysArray[id];
				item.inMultiSelection = false;
				item.deselect();
			}
		}
		else {
			// select
			this.selectedIds.push(id);
		}
		
		// loop through selected overlays, select them on the map, calculate the
		// bounds around them, and check if refitting map is needed
		var mapcontains = true;
		var bounds = new google.maps.LatLngBounds();
		for (var i = 0; i < this.selectedIds.length; i++) {
			var item = this.overlaysArray[this.selectedIds[i]];

			item.inMultiSelection = true;
			item.select();
			//iterate over all points if overlay is of type polygon or polyline
			if(this.geometriesArray[this.selectedIds[i]].type == 'polygon' 
				|| this.geometriesArray[this.selectedIds[i]].type == 'polyline')
			{
	
				var positions = item.getPath();
				var position; 
				
				for(var k = 0; k < positions.length; k++) {
					var tmp = positions.getAt(k);
					position = new google.maps.LatLng(tmp.lat(), tmp.lng());
					bounds.extend(position);
					// don't touch mapcontains once it changed to false
					if (mapcontains)
					{
						mapcontains = this.googlemap.getBounds().contains(position);
					}
				}
			}
			//overlay type marker
			else {
				var position = item.getPosition();
				bounds.extend(position);
				// don't touch mapcontains once it changed to false
				if (mapcontains)
				{
					mapcontains = this.googlemap.getBounds().contains(position);
				}
			}
		}
		if (!mapcontains)
		{
			this.googlemap.fitBounds(bounds);
		}
	}	
};

/**
 * Show a info window for a given, existing geometry
 * 
 * @param id integer
 * 	geometry id
 */
Area.prototype.showInfoWindow = function(id) {
	if(id == null) {
		console.log("Invalid id");
		return;
	}
	var url = null;
	switch(this.geometriesArray[id].entity) {
		case 'area' : 
			url = this.options.infowindowcontentfetchurl_area;
			break;
		case 'observation' :
			url = this.options.infowindowcontentfetchurl_observation;
			break;
		default :
			break;
	}
	if(url) {
		url = url.replace(/{ID}/, id);
		var this_ = this;
	
		if (this.infoWindow != null) {
			google.maps.event.trigger(this.infoWindow, 'closeclick');
		}
		var infowindow = this.infoWindow = new google.maps.InfoWindow(
				this.options.infowindowoptions
			);
		
		google.maps.event.addListener(infowindow, 'closeclick', function() {
			jQuery('#row' + id).removeClass('trSelected');
			infowindow.close();
			this_.infoWindow = null;
		});
	
		jQuery.get(url, function(data) {
			infowindow.close();
			infowindow.setContent(data);
			infowindow.open(this_.googlemap, this_.overlaysArray[id]);
			jQuery(data);
		});
		
		infowindow.open(this.googlemap, this.overlaysArray[id]);
	} else {
		alert("No infowindowcontentfetchurl given!");
	}
};

/**
 * Show a info window for a new geometry
 * 
 * @param id integer
 *   
 */
Area.prototype.showInfoWindowToCreateNewGeometry = function(overlayElement, html) {
	var infowindow = this.infoWindow = new google.maps.InfoWindow({
				content: html
		});

	// Delete overlayElement if window closed
	google.maps.event.addListener(infowindow, 'closeclick', function() {
		overlayElement.setMap(null);
	});
	this.googlemap.fitBounds(overlayElement.getBounds());
	infowindow.open(this.googlemap, overlayElement);
};

/**
 * Create a single overlayElement from a JSON
 */
Area.prototype.createOverlayElementFromJson = function(currentjsonoverlay) {
	var newoverlay;
	if (currentjsonoverlay.type == 'polygon') {
		var isChild = (currentjsonoverlay.child === undefined) ? false : currentjsonoverlay.child;
		newoverlay = new google.maps.Polygon( { inMultiSelection: false, child : isChild} );
		newoverlay.setOptions({
			zIndex : 0,
		});		
	} else if (currentjsonoverlay.type == 'polyline') {
		newoverlay = new google.maps.Polyline( { inMultiSelection: false} );
		newoverlay.setOptions({
			zIndex : 1,
		});
	} else if (currentjsonoverlay.type == 'marker') {
		newoverlay = new google.maps.Marker( { inMultiSelection: false} );
		newoverlay.setZIndex(2);
	} else {
		console.error('Unknown type of overlay!');
		return;
	}
	return newoverlay;
};

Area.prototype.addWindowsListenerToGeometry = function(id) {
	var this_ = this;
	var currentoverlay = this.overlaysArray[id];
	
	if(!currentoverlay.listeners.click) {
		currentoverlay.listeners.click = [];
	}
	currentoverlay.listeners.click.push(
		google.maps.event.addListener(currentoverlay, 'click', function() {
			var row = $('#row' + id);
			//if row is not yet visible, display now
			if(!row.is(":visible")) row.show();
			
			row.addClass('trSelected');
			this_.showInfoWindow(id);
			currentoverlay.select();
			this_.deselectOtherGeometries(currentoverlay);
		})
	);
};

/**
 * Add an overlay to the google map
 * 
 * @param currentjsonoverlay
 *            overlayElement to add
 */
Area.prototype.addGeometryFromJsonToGoogleMapArray = function(currentjsonoverlay) {
	var newoverlay = this.createOverlayElementFromJson(currentjsonoverlay);
	this.overlaysArray[currentjsonoverlay.id] = newoverlay;

	var latLngs = [];
	for ( var k in currentjsonoverlay.area_points) {
		var currentpoint = currentjsonoverlay.area_points[k];
		var newlatlng = new google.maps.LatLng(currentpoint.lat,
				currentpoint.lng);
		/**
		 * PostGIS requires that the first and last point are the same value
		 * for polygons - Google closes the map automatically.
		 * Solution: Drop points which are equal to the first one
		 */
		if(k == 0 || !latLngs[0].equals(newlatlng)) {
			latLngs[k] = newlatlng;
		}
	}

	newoverlay.setPath(latLngs);
	newoverlay.setMap(this.googlemap);
	newoverlay.listeners = {};
	newoverlay.setup();

	this.addWindowsListenerToGeometry(currentjsonoverlay.id);
};

/**
 * Add overlayData to this map.
 * 
 * @param currentjsonoverlay
 *            overlayData to add
 */
Area.prototype.addGeometryFromJsonToGeometriesArray = function(currentjsonoverlay) {
	this.geometriesArray[currentjsonoverlay.id] = currentjsonoverlay;
};

/**
 * Load all elements from a given json object to this object
 * 
 * @param json
 *            a json object
 */
Area.prototype.loadGeometriesAndOverlaysFromJson = function(json) {
	var this_ = this;
	json.forEach(function(e) {
		this_.addGeometryFromJsonToGeometriesArray(e);
		this_.addGeometryFromJsonToGoogleMapArray(e);
	});
};

/**
 * Remove all overlays from this map.
 * @note this function only clears the displayed map but keeps overlays stored in the
 * Area object to show them again.
 */
Area.prototype.clearOverlays = function() {
	for (var i in this.overlaysArray)
		if(i != this.options.geometryeditid)
			this.overlaysArray[i].setMap();
};

/**
 * Create and show the drawing manager
 */
Area.prototype.createDrawingManager = function(enable) {
	if(enable) {
		var this_ = this;
		this.drawingManager = new google.maps.drawing.DrawingManager({
			// do not select a tool yet
			drawingMode : false,
			// show the tools
			drawingControl : true,
			drawingControlOptions : {
				// show the toolbox on top left
				position : google.maps.ControlPosition.TOP_LEFT,
				// enable marker, polyline and polygon as drawing primitves
				drawingModes :  [ google.maps.drawing.OverlayType.MARKER,
									google.maps.drawing.OverlayType.POLYLINE,
									google.maps.drawing.OverlayType.POLYGON ]
			},
			// set options for the 3 elements
			makerOptions : {
				draggable : true
			},
			polylineOptions : {
				strokeWeight : 3,
				strokeColor : '#ff0000',
				strokeOpacity : 0.5,
				clickable : true,
				editable : true,
			},
			polygonOptions : {
				fillColor : '#ff0000',
				fillOpacity : 0.5,
				strokeWeight : 0,
				clickable : true,
				editable : true,
			},
			map : this.googlemap
		});
	
		google.maps.event.addListener(this.drawingManager, 'overlaycomplete', function(overlay) {
			overlay.overlay.setEditable(false);
			jQuery.get(this_.options.infowindowcreateformfetchurl, function(data) {
				if(this_.infoWindow) {
					this_.infoWindow.close();
					this_.infoWindow = null;
				}
				if(this_.newOverlay) {
					this_.newOverlay.setMap(null);
				}
				this_.showInfoWindowToCreateNewGeometry(overlay.overlay, data);
				//make it look like all our other overlays
				overlay.overlay.type = overlay.type;

				this_.newOverlay = overlay.overlay;
				updateHiddenfields(overlay.overlay);
			});
			this.setDrawingMode(null);
		});
	}
};

/**
 * Create a searchbar using autocomplete-feature by google places api.
 */
Area.prototype.createSearchbar = function(enable) {
	if(enable) {
		var googlemap = this.googlemap;
		// geocoder
		var geocoder = new google.maps.Geocoder();
		// rectangle around switzerland
		var geocoderBounds = new google.maps.LatLngBounds(new google.maps.LatLng(45.768673, 5.783386), new google.maps.LatLng(47.983671, 10.622864));
		// create a new div element to hold everything needed for the searchbar
		var searchdiv = document.createElement('div');
		// create new input field
		var searchinput = document.createElement('input');
		// create new text field
		// add searchinput to searchdiv
		searchdiv.appendChild(searchinput);
		// set id's for both elements
		searchdiv.setAttribute('id', 'search_container');
		searchinput.setAttribute('id', 'search_input');
	
		// push the search element on the google map in the top left corner
		googlemap.controls[google.maps.ControlPosition.TOP_LEFT].push(searchdiv);
	
		var autocomplete = new google.maps.places.Autocomplete(searchinput, {
			// available types: 'geocode' for places, 'establishment' for
			// companies
			types : [ 'geocode' ]
		});
	
		// listen do pressed suggestions and center map on them
		autocomplete.bindTo('bounds', googlemap);
	
		google.maps.event.addListener(autocomplete,	'place_changed', function() {
			var place = autocomplete.getPlace();
			// if not an autocomplete value was selected, geocode the search string
			if (typeof place.geometry === 'undefined') {
				geocoder.geocode({
					'address': place.name,
					'bounds': geocoderBounds
				}, function(results, status) {
					if (status == google.maps.GeocoderStatus.OK) {
						place = results[0];
						for (var i = 0; i < results.length; i++) {
							if(results[i].formatted_address.search(', Schweiz') != -1) {
								place = results[i];
								break;
							}
						}
						if (place.geometry.viewport) {
							googlemap.fitBounds(place.geometry.viewport);
						} else {
							googlemap.setCenter(place.geometry.location);
							googlemap.setZoom(17);
						}
					}
				});
			}
			// if an autocomplete value was selected, geometry information is already available
			else if (place.geometry.viewport) {
				googlemap.fitBounds(place.geometry.viewport);
			} else {
				googlemap.setCenter(place.geometry.location);
				googlemap.setZoom(17);
			}
		});
	
		// remove focus from searchinput when map moved
		google.maps.event.addListener(googlemap, 'bounds_changed', function() {
			searchinput.blur();
		});
	
		// select all text when user focus the searchinput field
		searchinput.onfocus = function() {
			searchinput.select();
		};
	}
};

/**
 * Create a coordinates searchbar on top of the google maps
 */
Area.prototype.createSearchbarCH1903 = function(enable) {
	if(enable) {
		var googlemap = this.googlemap;
		// create a new div element to hold everything needed
		// for the searchbarch1903
		var searchdivch1903 = document.createElement('div');
		// create new input field
		var searchinputch1903 = document.createElement('input');
		// regex to extract values
		var regexch1903 = /^[yY:\s]*([+-]?[0-9']+\.?[0-9]*)[\s,\/\\\|]+[xX:\s]*([+-]?[0-9']+\.?[0-9]*)$/;
	
		// add searchdivch1903 to searchdiv
		searchdivch1903.appendChild(searchinputch1903);
		// set id's for both elements
		searchdivch1903.setAttribute(
				'id',
				'searchch1903_container');
		searchinputch1903.setAttribute(
				'id',
				'searchch1903_input');
	
		// push the search element on the google map in the top
		// left corner
		googlemap.controls[google.maps.ControlPosition.TOP_RIGHT]
				.push(searchdivch1903);
	
		var getValueWithThousandsSeparator = function (n, separator) {
			var separateRegex = new RegExp('(-?[0-9]+)([0-9]{3})'),
			result = n +'';
		 
			if (separator === undefined) {
				separator = ',';
			}
			while(separateRegex.test(result)) {
				result = result.replace(separateRegex, '$1' + separator + '$2');
			}
			return result;
		};
		
		var getFormattedCh1903Value = function(x, y) {
			return	"Y: " + getValueWithThousandsSeparator(y.toFixed(0), '\'') +
				", X: " + getValueWithThousandsSeparator(x.toFixed(0), '\'');
		};
		
		/**
		 * When map moved: a) remove focus from
		 * searchinputch1930 b) set value of searchinputch1903
		 * to new coordinates
		 * 
		 * @param enable
		 *            boolean If true, update searchinputch1903
		 *            when map moved If false, remove listener
		 *            (if existing)
		 */
		var boundsChangeListender = function(enable) {
			if (arguments.length == 0 || enable) {
				this.ch1903MapChangeListener = google.maps.event
						.addListener(
								googlemap,
								'bounds_changed',
								function() {
									searchinputch1903.blur();
									var center = googlemap
											.getCenter();
									var lng = center.lng();
									var lat = center.lat();
									var x = WGStoCHx(lat, lng);
									var y = WGStoCHy(lat, lng);
									// round to two digits
									searchinputch1903.value = getFormattedCh1903Value(x,y);
								});
			} else {
				if (this.ch1903MapChangeListener != undefined) {
					google.maps.event
							.removeListener(this.ch1903MapChangeListener);
				}
			}
		};
	
		boundsChangeListender(true);
	
		searchinputch1903.onkeypress = function(event) {
			if (event.keyCode != 13) {
				return; // was not enter
			}
			
			// do not submit the form when pressing enter
	        if (event.preventDefault) {
	        	event.preventDefault();
	        }
			var text = searchinputch1903.value;
			// remove all "'"
			text = text.replace(/\'/g, '');
			var match = regexch1903.exec(text);
			if (match != null) {
				var y = parseFloat(match[1]);
				var x = parseFloat(match[2]);
				wgsLat = CHtoWGSlat(y, x);
				wgsLng = CHtoWGSlng(y, x);
				console.log("CH1903: " + y + " / " + x);
				console.log("WGS84: " + wgsLat + " / "
								+ wgsLng);
				var newCenter = new google.maps.LatLng(wgsLat,
						wgsLng);
				boundsChangeListender(false);
				googlemap.setCenter(newCenter);
				searchinputch1903.value = getFormattedCh1903Value(x,y);
				/**
				 * @todo Redo this in a sane way.
				 */
				setTimeout(function() {
					boundsChangeListender(true);
				}, 500);
			} else {
				console.log("Value '" + text + "' can not be parsed.");
			}
		};
	}
};

/**
 * Only show and center map.
 * 
 * @param geometryId Integer
 * 	Id of the overlay to edit.
 */
Area.prototype.showAndCenter = function(geometryId) {
	// calculate the bounds around them, and check if refitting map is needed
	var mapcontains = true;
	var bounds = new google.maps.LatLngBounds();
	var item = this.overlaysArray[geometryId];
	//iterate over all points if overlay is of type polygon or polyline
	if(this.geometriesArray[geometryId].type == 'polygon' 
		|| this.geometriesArray[geometryId].type == 'polyline')
	{

		var positions = item.getPath();
		var position; 
		
		for(var k = 0; k < positions.length; k++) {
			var tmp = positions.getAt(k);
			position = new google.maps.LatLng(tmp.lat(), tmp.lng());
			bounds.extend(position);
			// don't touch mapcontains once it changed to false
			if (mapcontains)
			{
				mapcontains = this.googlemap.getBounds().contains(position);
			}
		}
	}
	//overlay type marker
	else {
		var position = item.getPosition();
		bounds.extend(position);
		// don't touch mapcontains once it changed to false
		if (mapcontains)
		{
			mapcontains = this.googlemap.getBounds().contains(position);
		}
	}
	
	this.googlemap.setZoom(this.options.googlemapsoptions.zoom);
	this.googlemap.fitBounds(bounds);
};
	

/**
 * Edit an existing geometry.
 * 
 * @param geometryId Integer
 * 	Id of the overlay to edit.
 */
Area.prototype.geometryEdit = function(geometryId) {
	var this_ = this;
	var currentOverlay = this.overlaysArray[geometryId];
	var currentGeometry = this.geometriesArray[geometryId];
	currentOverlay.setEditable(true);
	
	/**
	 * Remove any click listeners on the editable overlay
	 */
	if(currentOverlay.listeners.click) {
		currentOverlay.listeners.click.forEach(function(e) {
			google.maps.event.removeListener(e);
		});
		currentOverlay.listeners.click = [];
	}
	
	updateHiddenfields(currentOverlay, this_.options.coordinatestorageid);
	
	google.maps.event.addListener(this_.googlemap, 'rightclick',
			function(mouseevent) {
				console.log(mouseevent.latLng);
				currentOverlay.deleteClosestVertex(mouseevent.latLng);
		});

	google.maps.event.addListener(
			currentOverlay,
			'geometry_changed',
			function() {
				var posturl = this_.options.geometryupdateurl.replace(/{ID}/, geometryId);

				var position = currentOverlay.getPosition();

				getAddress(position, function(address) {
					console.log(address.canton);
					currentGeometry.canton = address.canton;
					currentGeometry.township = address.township;
					currentGeometry.zip = address.zip;
					currentGeometry.country = address.country;
				});

				getAltitude(position, function(altitude) {
					console.log(altitude);
					currentGeometry.altitude = altitude;
				});
				
				updateHiddenfields(currentOverlay, this_.options.coordinatestorageid);

				var geometry_coordinates = new Array();
				currentOverlay.getAllCoordinates().forEach(
						function(position) {
							geometry_coordinates.push([ position.lat(),
									position.lng() ]);
						});
				geometry_coordinates = JSON.stringify(geometry_coordinates);
				currentGeometry.area_points = geometry_coordinates;

				// Ugly hack... Better to be chained with getAddress & getAltitude
				setTimeout(function() {
					jQuery.post(posturl, currentGeometry, function() {
							console.log("Geometry saved!");
						});
				}, 1500);
			});
	this.googlemap.setCenter(currentOverlay.getPosition());
};

/**
 * Create a reticle at the center of the map. For now you can not remove a created reticle.
 * 
 * @param enable
 * 	Whether to enable the reticle or not.
 * @note A lot o this code is taken from the Google Maps API Reference: https://developers.google.com/maps/documentation/javascript/overlays#CustomOverlays
 *  There is even more code which may be usefull in the future. Examples:
 *  	- onRemove()
 *  	- hide()
 *  	- show()
 *  	- toggle()
 *  	- toggleDOM()
 */
Area.prototype.createReticle = function(enable) {
	function ReticleOverlay(image, map) {
		// Now initialize all properties.
		this.image_ = image;
		this.map_ = map;

		// We define a property to hold the image's
		// div. We'll actually create this div
		// upon receipt of the add() method so we'll
		// leave it null for now.
		this.div_ = null;

		// Explicitly call setMap() on this overlay
		this.setMap(map);
	}

	ReticleOverlay.prototype = new google.maps.OverlayView();

	/**
	 * Gets triggered when the overlay is added to a map
	 */
	ReticleOverlay.prototype.onAdd = function() {
		// Note: an overlay's receipt of onAdd() indicates that
		// the map's panes are now available for attaching
		// the overlay to the map via the DOM.

		// Create the DIV and set some basic attributes.
		var div = document.createElement('div');
		div.style.border = "none";
		div.style.borderWidth = "0px";
		div.style.position = "absolute";

		// Create an IMG element and attach it to the DIV.
		var img = document.createElement("img");
		img.src = this.image_;
		img.style.width = "24px";
		img.style.height = "24px";
		div.appendChild(img);

		// Set the overlay's div_ property to this DIV
		this.div_ = div;

		// We add an overlay to a map via one of the map's panes.
		// We'll add this overlay to the overlayImage pane.
		var panes = this.getPanes();
		panes.overlayLayer.appendChild(div);
	};

	ReticleOverlay.prototype.draw = function() {
		// Size and position the overlay. We use a southwest and northeast
		// position of the overlay to peg it to the correct position and size.
		// We need to retrieve the projection from this overlay to do this.
		var overlayProjection = this.getProjection();

		// Retrieve the southwest and northeast coordinates of this overlay
		// in latlngs and convert them to pixels coordinates.
		// We'll use these coordinates to resize the DIV.
		if(overlayProjection != null) {
			var sw = overlayProjection
					.fromLatLngToDivPixel(this.map_.getBounds().getSouthWest());
			var ne = overlayProjection
					.fromLatLngToDivPixel(this.map_.getBounds().getNorthEast());
		
			// Resize the image's DIV to fit the indicated dimensions.
			var div = this.div_;
			div.style.left = sw.x + (ne.x - sw.x)/2 - 12 + 'px'; // MAGIC!!
			div.style.top = ne.y + (sw.y - ne.y)/2 - 12 + 'px';
			div.style.width = (ne.x - sw.x) + 'px';
			div.style.height = (sw.y - ne.y) + 'px';
		}
	};

	if(enable) {
		var srcImage = this.options.reticleimageurl;
		var reticleoverlay = this.reticleoverlay = new ReticleOverlay(srcImage, this.googlemap);
		google.maps.event.addListener(this.googlemap, 'center_changed', function() { // change even to e.g. idle if needed
			reticleoverlay.draw();
		});
	}
};

Area.prototype.createDrawingManagerGetcoordinate = function(enable) {
	if(enable) {
		var this_ = this;
		this.drawingManager = new google.maps.drawing.DrawingManager({
			drawingMode : google.maps.drawing.OverlayType.MARKER,
			// show the tools
			drawingControl : true,
			drawingControlOptions : {
				// show the toolbox on the right, middle
				position : google.maps.ControlPosition.TOP_LEFT,
				// enable marker, polyline and polygon as drawing primitves
				drawingModes : [ google.maps.drawing.OverlayType.MARKER ]
			},
			// set options for the 3 elements
			makerOptions : {
				draggable : true
			},
			map : this.googlemap
		});
		
		var update = function() {
			updateHiddenfields(this_.newOverlay.overlay, this_.options.coordinatestorageid);
		};
		
		google.maps.event.addListener(
				this.drawingManager,
				'overlaycomplete',
				function(overlay) {
					if (this_.newOverlay) {
						this_.newOverlay.overlay.setMap(null);
					}
					this_.newOverlay = overlay;
					this_.newOverlay.overlay
							.setupGeometryChangedEvent();
					this.setDrawingMode(null);
					overlay.overlay.setEditable(true);
					update();
	
					google.maps.event.addListener(
							this_.newOverlay.overlay,
							'geometry_changed',
							update);
				});
		google.maps.event.addListener(
				this,
				'overlaycomplete',
				function() {
					this_.newOverlay.overlay.setupGeometryChangedEvent();
					this_.newOverlay.overlay.setMap(this.googlemap);
					this_.drawingManager.setDrawingMode(null);
					this_.newOverlay.overlay.setEditable(true);
					update();
	
					google.maps.event.addListener(
							this_.newOverlay.overlay,
							'geometry_changed',
							update);
				});
	}
};

/**
 * Get the address for the submitted coordinate.
 * 
 * @param latlng
 *            coordinate
 * @param callback
 *            callback function
 */
getAddress = function(latlng, callback) {	
	var geocoder = new google.maps.Geocoder();

	geocoder.geocode({'latLng' : latlng }, function(results, status) {
		var address = {};
		if (status == google.maps.GeocoderStatus.OK) {
			jQuery.each(results, function(index, result) {
				if (false) {
					console.debug("Address from google:");
					console.debug(result);
				}
				if (result.types == 'postal_code') {
					var length = result.address_components.length;
					address.zip = result.address_components[0].long_name;
					address.locality = result.address_components[1].long_name;
					address.canton = result.address_components[length - 2].short_name;
					address.country = result.address_components[length - 1].short_name;
				}
				if (result.types == 'locality,political') {
					address.township = result.address_components[0].long_name;
				}
			});
			callback(address);
		} else {
			console.error("Geocoder failed due to: "
					+ status);
		}
	});
};

/**
 * Get the altitude for the submitted coordinate.
 * 
 * @param latlng
 *            coordinate
 * @param callback
 *            callback function
 */
getAltitude = function(latlng, callback) {
	var elevator = new google.maps.ElevationService();
	var request = {
		locations : [ latlng ]
	};

	elevator.getElevationForLocations(request, function(results, status) {
		if (status == google.maps.ElevationStatus.OK) {
			// Retrieve the first result
			if (results[0]) {
				var altitude = parseInt(results[0].elevation + 0.5);
				callback(altitude);
			}
		} else {
			console.debug("Could not get altitute. Errorstatus: "
							+ status);
		}
	});
};

/**
 * Update the hidden field so they can be read by drupal later on.
 * 
 * @param overlay
 */
updateHiddenfields = function(overlay, coordinatestorageid) {
	if(coordinatestorageid) {
		jQuery('#' + coordinatestorageid).val(
				JSON.stringify(overlay
						.getJsonCoordinates()));
	}
	
	getAddress(
			overlay.getPosition(),
			function(address) {
				jQuery('#hiddenfield-canton').val(address.canton);
				jQuery('#hiddenfield-township').val(address.township);
				jQuery('#hiddenfield-locality').val(address.locality);
				jQuery('#hiddenfield-zip').val(address.zip);
				jQuery('#hiddenfield-country').val(address.country);
				
				jQuery('#hiddenfield-latitude').val(
						overlay.getPosition().lat());
				jQuery('#hiddenfield-longitude').val(
						overlay.getPosition().lng());
			});
	getAltitude(overlay.getPosition(), function(
			altitude) {
		jQuery('#hiddenfield-altitude').val(altitude);
		jQuery('#hiddenfield-geometry-type').val(overlay.type);
		jQuery('#hiddenfield-geometry-coordinates').val(JSON.stringify(overlay.getJsonCoordinates()));
	});
};

