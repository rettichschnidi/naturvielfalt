/*
 * "Real" combobox widget :
 * When the user can either select from the list like a classic select 
 * or start typing and the list entries are automatically filtered.
 * 
 * Based on the jQuery UI Autocomplete widget and the example showed in the widget
 * documentation's examples page : http://jqueryui.com/demos/autocomplete/#combobox
 */

(function($) {
		$.widget("ui.combobox", {
			
			options : {
				delay : 0,
				minLength: 0,
				disabled: false,
				maxHeight: 0,
				scroll: true,
				zebra : true,
				maxItems : 0,
				highlight : false,
				button : true
			},
			_create: function() {
				var self = this;
				//save the style attr of the select to be applied to the replacement later
				var style = this.element.attr("style");
				//hide the select
				var select = this.element.hide();
				//list of possible items - derived from the base select't options
				this.items = new Array();
				//create the text input with autocomplete
				self.input = $("<input>")
					.insertAfter(select)
					.autocomplete({
						source : function(request, response){
							var matcher = new RegExp(request.term, "i");
							var result = new Array();
							var maxItems = false;
							var itemsFound = 0;
							if(self.options.maxItems != 0){
								maxItems = self.options.maxItems;
							}
							for(var i=0; i<self.items.length; i++){
								var text = self.items[i].value;
								if (text && (!request.term || matcher.test(text))){
									if(self.options.highlight){
										result.push({
											id: self.items[i].id,
											label: self.items[i].label.replace(new RegExp("(?![^&;]+;)(?!<[^<>]*)(" + $.ui.autocomplete.escapeRegex(request.term) + ")(?![^<>]*>)(?![^&;]+;)", "gi"), "<strong>$1</strong>"),
											value: self.items[i].value
										});
									}else{
										result.push(self.items[i]);
									}
									itemsFound ++;
									if(maxItems && itemsFound >= maxItems){
										break;
									}
								}
							}
							response(result);
						},
						delay: self.options.delay,
						//function executed when the valued is changed, after the menu is closed
						change: function(event, ui) {
							if (!ui.item) {
								// remove invalid value, as it didn't match anything
								$(this).val("");
								select.val("");
								select.change();
								return true;
							}
							select.val(ui.item.id);
							self._trigger("selected", event, {
								item: select.find("[value='" + ui.item.id + "']")
							});
						},
						//function executed when an item is selected
						select: function(event, ui){
							select.val(ui.item.id);
							//select.change();
						},
						//function executed when the menu is opened
						open : function( event, ui ) {
							//add alternate styles to the items ('zebra stripes')
							if(self.options.zebra){
								$( this ).autocomplete( "widget" )
									.find( "ui-menu-item-alternate" )
								    .removeClass( "ui-menu-item-alternate" )
								    .end()
								    .find( "li.ui-menu-item:odd" )
								    .addClass( "ui-menu-item-alternate" );
							}
							if(self.options.maxHeight != 0){
								$( this ).autocomplete( "widget" ).css("max-height", self.options.maxHeight);
							}
							if(self.options.scroll){
								$( this ).autocomplete( "widget" ).css("overflow", "auto");
							}
							$( this ).autocomplete( "widget" ).css("z-index", "10000");
						},
						//function executed when the menu is closed
						close : function(event, ui){
							//alert("close");
							$(select).change();
						},
						minLength: 0
					})
					.addClass("ui-widget ui-widget-content ui-corner-left");
				//button that allows to unfold the menu
				var button = $("<a href='#'>&nbsp;</a>")
					.attr("tabIndex", -1)
					.insertAfter(self.input)
					.button({
						icons: {
							primary: "ui-icon-triangle-1-s"
						},
						text: false
					}).removeClass("ui-corner-all")
					.addClass("ui-corner-right ui-button-icon")
					.click(function() {
						// close if already visible
						if (self.input.autocomplete("widget").is(":visible")) {
							self.input.autocomplete("close");
							return false;
						}
						//for the button there must be no delay
						var delay = self.input.autocomplete("option", "delay");
						self.input.autocomplete("option", "delay", 0);
						// pass empty string as value to search for, displaying all results
						self.input.autocomplete("search", "");
						self.input.focus();
						//restore delay
						self.input.autocomplete("option", "delay", delay);
						return false;
					});
				//apply the styles what were previously copied from the underlying select to the text field
				self.input.attr("style", style);
				//adjust the dimensions of the button
				var height = self.input.outerHeight()-2;				
				button.css("height", height).css("vertical-align", "bottom");
				button.find("span.ui-button-text").remove();
				button.find("span").css("height", height).css("position", "relative");
				//adapt the width of the input in order to have a total width 
				//(input + button) equal to the width of the original select
				var width = self.input.outerWidth()-2;
				if(self.options.button){
					width = width - 29; //29 is width of the button
				}
				self.input.css("width", width);
				//
				self.refreshComboItems();
			},
			//function that generates the items from the select's option elements
			//this function is called once in the create function
			//it can be called subsequently if options are added/removed from the underlying select.
			refreshComboItems : function(){
				var self = this;
				self.items=new Array();
				$(self.element).find('option').each(function(index, element){
					var elementJ=$(element);
					var text = elementJ.text();
					self.items.push({
						id: element.value,
						label: text,
						value: text
					});					
				});
				var selected = $(self.element).find('option:selected');
				if(selected.length >0){
					self.input.val(selected.text());
				}
			},
			focus : function(){
				this.input.focus();
			},
			select : function(){
				this.input.select();
			},
			disableComboBox : function() {
				this.input.attr('disabled','disabled');
			},
			enableComboBox : function() {
				this.input.removeAttr('disabled');
			}
		});
	})(jQuery);
