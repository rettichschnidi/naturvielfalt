/**
 * @author Simon Käser
 */

var inventory = {
  
  id: false,
  container: false,
  add: false,
  message: false,
  form: false,
  template: false,
  loading: false,
  count: 1,
  group_count: 1,
  templates: false
  
};

(function($) {
  
  inventory.init = function() {
    inventory.initCustomFields();
    inventory.initAdditionalFields();
    inventory.initTemplates();
    inventory.container = $('#inventories');
    if(!inventory.container.size())
      return;
    inventory.form = inventory.container.parents('form');
    inventory.id = inventory.form.find('#invId').val();
    inventory.form.find('#edit-actions').append(inventory.form.find('.form-item-add'));
    inventory.form.find('input[name^="form_"]').remove();
    inventory.form.submit(inventory.saveEntries);
    inventory.add = inventory.form.find('#add-inventory-group');
    inventory.add.change(inventory.addInventory)
    inventory.message = inventory.form.find('#message');
    if(!inventory.message.size()) {
      inventory.message = $('<div id="message" class="messages"></div>').hide();
      inventory.form.find('#edit-actions').append(inventory.message);
    }
    inventory.message.ajaxError(inventory.ajaxError);
    inventory.container.find('.invTable').each(function() {
      inventory.initializeTable($(this));
    });
    inventory.template = inventory.form.find('#template');
    if(inventory.template.size())
      inventory.template.click(inventory.templatesDialog);
  }
  
  inventory.setMessage = function(message, type, time) {
    if(inventory.messageTimer)
      window.clearTimeout(inventory.messageTimer);
    inventory.message.html(message).attr('class', 'messages').addClass(type).stop().slideDown('fast');
    if(time)
      inventory.messageTimer = window.setTimeout(function() {
        inventory.message.slideUp('fast');
      }, time);
  }
  
  inventory.ajaxError = function() {
    inventory.hideLoading();
    inventory.setMessage(Drupal.t('An AJAX HTTP request terminated abnormally.'), 'error', 15000);
  }
  
  inventory.showLoading = function() {
    if(!inventory.loading) {
      inventory.loading = $('<div><img src="'+Drupal.settings.basePath+'modules/inventory/images/loading.gif" /></div>').hide();
      inventory.form.append(inventory.loading);
    }
    inventory.loading.dialog({
      modal: true,
      draggable: false,
      resizable: false,
      closeOnEscape: false,
      closeText: '',
      dialogClass: 'inventory-edit-loading',
      width: 42,
      height:42,
      minWidth: 42,
      minHeight: 42
    });
  }
  
  inventory.hideLoading = function() {
    if(!inventory.loading)
      return;
    inventory.loading.dialog('close');
  }
  
  inventory.saveEntries = function(event) {
    inventory.showLoading();
    $.post(
      inventory.form.attr('action').replace('/edit', '/save_entries')+'?ajax=1',
      inventory.form.serializeArray(),
      function(data) {
        inventory.hideLoading();
        if(!data)
          return;
        for(var i=0; i<data.inventories.length; i++) {
          inventory.container.find('#invTitle'+data.inventories[i]['id_old']).attr('id', 'invTitle'+data.inventories[i]['id_new']);
          inventory.container.find('#invTable'+data.inventories[i]['id_old']).attr('id', 'invTable'+data.inventories[i]['id_new']);
          inventory.container.find('input.inventory[name="inventories['+data.inventories[i]['id_old']+']"]').attr('name', 'inventories['+data.inventories[i]['id_new']+']');
          inventory.container.find('input[name^="entries['+data.inventories[i]['id_old']+']"]').each(function() {
            $(this).attr('name', $(this).attr('name').replace('entries['+data.inventories[i]['id_old']+']', 'entries['+data.inventories[i]['id_new']+']'));
          });
        }
        for(var i=0; i<data.entries.length; i++) {
          var entry = inventory.container.find('input.entry_id[value="'+data.entries[i]['id_old']+'"]');
          entry.val(data.entries[i]['id_new']);
          var additional = entry.parents('tr').find('.additional');
          if(!additional.size())
            continue;
          var href = additional.attr('href').split('/');
          href[href.length-1] = entry.val();
          entry.parents('tr').find('.additional').attr('href', href.join('/'));
        }
        inventory.group_count = data.group_count;
        if(data.message)
          inventory.setMessage(data.message, data.result == 1 ? 'status' : 'error', 15000);
      },
      'json'
    );
    return false;
  }
  
  inventory.resetOddEven = function() {
    inventory.container.find('.invTable tbody tr:odd').removeClass('odd').addClass('even');
    inventory.container.find('.invTable tbody tr:even').removeClass('even').addClass('odd');
  }
  
  inventory.deleteEntry = function() {
    var row = $(this).parents('tr');
    var id = row.find('input.entry_id').val();
    if(id.substr(0, 4) == 'new_') { // The entry has not yet been stored, therefor we just delete the row
      inventory.removeEntry(row);
      return;
    }
    inventory.showLoading();
    $.getJSON(
      inventory.form.attr('action').replace('/edit', '/delete_entry') + '/' + id,
      {ajax : 1},
      $.proxy(function(data) {
        if(!data)
          return;
        if(data.result == 1)
          inventory.removeEntry(row);
        if(data.message)
          inventory.setMessage(data.message, data.result == 1 ? 'status' : 'error', 15000);
        inventory.hideLoading();
      }, row)
    );
  }
  
  inventory.removeEntry = function(row) {
    var table = row.parents('table');
    row.remove();
    inventory.updateGroupCount(table);
    inventory.resetOddEven();
    inventory.updateRowPositions();
    inventory.initializeSortable(table);
  }
  
  inventory.updateGroupCount = function(table) {
    var count = table.find('tbody tr:not(.disabled)').size();
    var prev = table.prev();
    while(!prev.hasClass('invTitle'))
      prev = prev.prev();
    prev.find('small').html('('+count+')');
  }
  
  inventory.enableDisable = function(row) {
    if(row.find == undefined)
      row = $(row);
    if(!row.find('input.organism_id').val()) {
      row.addClass('disabled');
      row.find('td input:not(input[type="hidden"], .organism)').attr('disabled', 'disabled');
    } else {
      row.removeClass('disabled');
      row.find('td input:not(input[type="hidden"], .organism)').attr('disabled', '');
    }
    inventory.updateGroupCount(row.parents('table'));
  }
  
  inventory.updateRowPositions = function() {
    inventory.container.find('tbody tr').each(function() {
      inventory.updateRowPosition($(this));
    });
  }
  
  inventory.updateRowPosition = function(row) {
    var position = row.parents('tbody').find('tr').index(row);
    row.find('*[name^="entries["]').each(function() {
      var name = $(this).attr('name').split('[');
      name[2] = position+']';
      $(this).attr('name', name.join('['));
    });
  }
  
  inventory.duplicateRow = function(row) {
    var clone = $('<tr>'+row.html()+'</tr>');
    clone.addClass(row.hasClass('odd') ? 'even' : 'odd');
    clone.find('a.delete').remove();
    clone.find('em').html('');
    clone.find('td.image').html('');
    clone.find('input:not(.identifier, .date)').val('');
    clone.find('select option').attr('selected', '');
    clone.find('input.date').attr('id', '').removeClass('hasDatepicker');
    row.parents('tbody').append(clone);
    inventory.initializeRow(clone);
    inventory.initializeSortable(clone.parents('.invTable'));
    inventory.updateRowPosition(clone);
  }
  
  inventory.getImage = function(row) {
    var id = row.find('input.organism_id').val();
    row.find('td.image').html('');
    $.getJSON(
      Drupal.settings.basePath+'inventory/get_entry_image/'+id,
      $.proxy(function(data) {
        if(!data)
          return;
        this.find('td.image').html(data.data);
      }, row)
    );
  }
  
  inventory.addInventory = function() {
    inventory.showLoading();
    $.getJSON(
      inventory.form.attr('action').replace('/edit', '/add_inventory_group') + '/' + $(this).val() + '/' + inventory.group_count++,
      function(data) {
        inventory.hideLoading();
        if(!data)
          return;
        if(data.data) {
          data = $(data.data);
          inventory.container.append(data);
          inventory.initializeTable(data);
          data.find('.organism').focus();
          inventory.form.find('p.empty').fadeOut();
        }
        inventory.add.find('option[value=""]').attr('selected', true);
    });
  }
  
  inventory.templatesDialog = function(e) {
    e.preventDefault();
    inventory.showLoading();
    var data = {ajax: 1};
    $.post(
      $(this).attr('href'),
      data,
      inventory.templatesCreateDialog,
      'json'
    );
  }
  
  inventory.templatesCreateDialog = function(data) {
    if(data && data.form) {
      var dialog = $('<div title="'+inventory.template.html()+'" />');
      dialog.append($(data.form));
      dialog.dialog({
        modal: true,
        resizable: false,
        closeOnEscape: false,
        closeText: '',
        close: function(event, ui) {
          $(this).remove();
        },
        width: 700
      });
      inventory.initTemplates();
      dialog.find('#edit-actions a').click(function(e) {
        e.preventDefault();
        $(this).parents('.ui-dialog-content').dialog('close');
      });
      dialog.find('form').submit(function(e) {
        var data = $(this).serializeArray();
        inventory.form.find('.inventory').each(function() {
          data.push({name: $(this).attr('name'), value: $(this).val()});
        });
        inventory.form.find('.organism_id').each(function() {
          data.push({name: 'organisms[]', value: $(this).val()});
        });
        $.post(
          $(this).attr('action')+'?ajax=1',
          data,
          function(data) {
            inventory.hideLoading();
            if(!data.result && data.form) {
              inventory.templatesCreateDialog(data);
              return;
            }
            if(!data)
              return;
            inventory.group_count = data.group_count;
            for(var id in data.new_inventories) {
              inv = $(data.new_inventories[id]);
              inventory.container.append(inv);
              inventory.initializeTable(inv);
              inventory.form.find('p.empty').fadeOut();
            }
            for(var inventory_id in data.new_entries) {
              var entries = data.new_entries[inventory_id];
              var table = inventory.container.find('#invTable'+inventory_id);
              var lastrow = table.find('tr:last');
              for(var i=0; i<entries.length; i++) {
                if(!entries[i])
                  continue;
                var entry = $(entries[i]);
                entry.insertBefore(lastrow);
                inventory.initializeRow(entry);
              }
              inventory.initializeSortable(table);
            }
            inventory.updateRowPositions();
            inventory.resetOddEven();
            if(data.message)
              inventory.setMessage(data.message, data.result == 1 ? 'status' : 'error', 15000);
          },
          'json'
        );
        inventory.showLoading();
        $(this).parents('.ui-dialog-content').dialog('close');
        return false;
      });
    }
    inventory.hideLoading();
  }
  
  inventory.customFieldsDialog = function(e) {
    e.preventDefault();
    inventory.showLoading();
    $.getJSON($(this).attr('href'), {ajax: 1}, function(data) {
      if(data && data.form) {
        var dialog = $('<div title="' + inventory.form.find('.custom').attr('title') + '" />');
        dialog.append($(data.form));
        dialog.dialog({
          modal: true,
          resizable: false,
          closeOnEscape: false,
          closeText: '',
          close: function(event, ui) {
            $(this).remove();
          },
          width: 500
        });
        inventory.initCustomFields();
        dialog.find('#edit-actions a').click(function(e) {
          e.preventDefault();
          $(this).parents('.ui-dialog-content').dialog('close');
        });
        dialog.find('form').submit(function(e) {
          $.post(
            $(this).attr('action'),
            $(this).serializeArray(),
            function(data) {
              inventory.hideLoading();
              if(!data)
                return;
              if(data.message)
                inventory.setMessage(data.message, data.result == 1 ? 'status' : 'error', 15000);
            },
            'json'
          );
          inventory.showLoading();
          $(this).parents('.ui-dialog-content').dialog('close');
          return false;
        });
      }
      inventory.hideLoading();
    });
  }
  
  inventory.additionalFieldsDialog = function(e) {
    e.preventDefault();
    inventory.showLoading();
    var data = {ajax: 1};
    $(this).parents('tr').find('td:last input[type="hidden"]').each(function() {
      var name = $(this).attr('name');
      data[name.substring(name.indexOf('col_'), name.length-1)] = $(this).val();
    });
    $.getJSON($(this).attr('href'),
      data,
      function(data) {
        if(data && data.form) {
          var dialog = $('<div title="'+inventory.form.find('.additional').attr('title')+'" />');
          dialog.append($(data.form));
          dialog.dialog({
            modal: true,
            resizable: false,
            closeOnEscape: false,
            closeText: '',
            close: function(event, ui) {
              $(this).remove();
            },
            width: 500
          });
          inventory.initAdditionalFields();
          dialog.find('#edit-actions a').click(function(e) {
            e.preventDefault();
            $(this).parents('.ui-dialog-content').dialog('close');
          });
          dialog.find('form').submit(function(e) {
            var values = $(this).serializeArray();
            var entry_id = $(this).attr('action').split('/').pop().replace(/\?.*$/, '');
            var row = inventory.container.find('input.entry_id[value="'+entry_id+'"]').parents('tr');
            var base_name = row.find('input.entry_id').attr('name');
            var inventory_id = row.parents('table').find('.inventory').attr('name');
            inventory_id = inventory_id.substring(12, inventory_id.length-1);
            for(var i=0; i<values.length; i++) {
              var name = values[i]['name'];
              if(name.substr(0, 4) != 'col_')
                continue;
              var value = values[i]['value'];
              var iname = base_name.replace('[id]', '['+name+']');
              var input = row.find('input[name="'+iname+'"]');
              if(!input.size()) { // create the hidden field if it is not yet present
                input = $('<input type="hidden" name="'+iname+'" value="" />');
                row.find('td:last').append(input);
              }
              input.val(value);
            }
            inventory.setMessage(Drupal.t('The additional fields data will be stored only after saving the whole form by pressing the <em>Save</em> button in the lower right.'), 'warning', 15000)
            $(this).parents('.ui-dialog-content').dialog('close');
            return false;
          }
        );
      }
      inventory.hideLoading();
    });
  }
  
  inventory.initializeTable = function(table) {
    table.find('tbody tr').each(function() {
      inventory.initializeRow($(this));
    });
    table.find('tr th:last-child').html('');
    
    table.find('thead tr').prepend('<th>&nbsp;</th>');
    inventory.initializeSortable(table);
    
    var prev = table.prev();
    while(!prev.hasClass('invTitle'))
      prev = prev.prev();
    prev.find('.custom').click(inventory.customFieldsDialog);
  }
  
  inventory.initializeSortable = function(table) {
    table.sortable('destroy');
    table.sortable({
      items: 'tbody tr:not(.disabled)',
      handle: '.handler',
      axis: 'y',
      tolerance: 'pointer',
      update: function(event, ui) {
        inventory.resetOddEven();
        inventory.updateRowPositions();
      }
    });
  }
  
  inventory.initializeFields = function(container) {
    // Date fields
    container.find('input.date').datepicker({
      dateFormat : 'dd.mm.yy',
      duration : 0
    }).width(80);
    
    // Numeric fields
    container.find('input.int').numeric().width(70);
  }
  
  inventory.initializeRow = function(row) {
    if(!row.find('.handler').size())
      row.prepend('<td><a class="handler"><img src="'+Drupal.settings.basePath+'modules/inventory/images/sort.gif" /></a></td>');
    
    row.find('td.image a').lightBox();
    
    row.find('input.entry_id[value=""]').each(function() {
      $(this).val('new_'+inventory.count++);
      var additional = $(this).parents('tr').find('.additional');
      if(!additional.size())
        return;
      var href = additional.attr('href').split('/');
      href[href.length-1] = $(this).val();
      additional.attr('href', href.join('/'));
    });
    
    // Replace delete checkbox
    row.find('input.delete').attr('checked', '').hide();
    row.find('td:last-child').append(
      '<a class="delete" href="javascript://" title="'+row.parents('table').find('tr th:last').html()+'">'+
        '<img src="'+Drupal.settings.basePath+'modules/inventory/images/delete.gif" />'+
      '</a>'
    );
    row.find('a.delete').click(inventory.deleteEntry);
    
    // Add dialog for additional fields form
    row.find('a.additional').click(inventory.additionalFieldsDialog);
    
    // Disable input fields if no organism is selected
    inventory.enableDisable(row);
    
    // Initialize fields
    inventory.initializeFields(row);
    
    // Organism field
    row.find('input.organism').autocomplete({
      minLength : 2,
      autoFocus : true,
      source : function(request, response) {
        //new search, so we change the indicator to searching
        this.element.removeClass('notfound');
        this.element.addClass('searching');
        actualElement = this.element;
        $.ajax({
          url : Drupal.settings.basePath + 'inventory/organism_autocomplete',
          dataType : "json",
          data : {
            inv_id : this.element.parents('table').find('input.inventory').val(),
            term : this.element.val()
          },
          // success : response,
          success : function(data){
            if(data.length==0){
              //change visual indicator to notfound and hide menu again
              actualElement.removeClass("searching");
              actualElement.addClass("notfound");
              $('.ui-autocomplete').hide();
            } else {
              // Remove search symbol
              actualElement.removeClass("searching");
              response(data);
            }
          }
        });
      },
      focus: function(event, ui) {
        return false;
      },
      select: function(event, ui) {
        $(this).val(ui.item.label || ui.item.label_latin);
        $(this).parents('td').find('input.organism_id').val(ui.item.id);
        $(this).parents('tr').find('td > em').html(ui.item.label_latin);
        inventory.enableDisable($(this).parents('tr'));
        if(event.keyCode == 9) { // TAB
          $(this).parents('tr').find('input.date').parents('td').next().find('input').focus();
          event.preventDefault();
        } else if(event.keyCode == 13) // ENTER
          $(this).parents('tr').find('input.date').focus();
        if($(this).parents('tr:last-child').size())
          inventory.duplicateRow($(this).parents('tr'));
        inventory.getImage($(this).parents('tr'));
        return false;
      }
    })
    .keyup(function() {
      //monitor field and remove class 'notfound' if its length is less than 2
      if($(this).val().length < 2) {
        $(this).removeClass('notfound');
        $(this).parents('td').find('input.organism_id').val('');
        $(this).parents('tr').find('td > em').html('');
        inventory.enableDisable($(this).parents('tr'));
      }
    })
    .blur(function() {
      inventory.enableDisable($(this).parents('tr'));
    })
    .each(function() {
      $(this).data('autocomplete')._renderItem = function(ul, item) {
        var term = this.term.replace(/[aou]/, function(m) {
          // to find with term 'wasser' -> 'wasser' and 'gewasesser'
          var hash = {
            'a' : '(ä|a)',
            'o' : '(ö|o)',
            'u' : '(ä|u)'
          };
          return hash[m];
        });
        var term = $.trim(term).split(' ')
        // highlighting of matches
        var label = item.label;
        var label_latin = item.label_latin;
        var old_label = item.old_label;
        var old_label_latin = item.old_label_latin;
        while(term.length) {
          var re = new RegExp(term.pop(), 'ig');
          if(old_label || old_label_latin) {
            old_label = old_label.replace(re, '<span class="ui-state-highlight">$&</span>');
            old_label_latin = old_label_latin.replace(re, '<span class="ui-state-highlight">$&</span>');
          } else {
            label = label.replace(re, '<span class="ui-state-highlight">$&</span>');
            label_latin = label_latin.replace(re, '<span class="ui-state-highlight">$&</span>');
          }
        }
        var old = '';
        if(old_label || old_label_latin)
          old = '<small>'+ old_label + '<em>' + old_label_latin + '</em></small>';
        return $('<li></li>')
          .data('item.autocomplete', item)
          .append('<a>' + label + '<em>' + label_latin + '</em>' + old + '</a>')
          .appendTo(ul);
      };
    })
  }
  
  // Templates form
  inventory.initTemplates = function() {
    inventory.templates = $('#inventory-templates-form');
    if(!inventory.templates.size())
      return;
    inventory.templates.find('input[name="source"]').change(function() {
      inventory.templates.find('input.id, input.term').val('');
    });
    inventory.templates.find('input.term').autocomplete({
      minLength : 0,
      autoFocus : true,
      source : function(request, response) {
        //new search, so we change the indicator to searching
        this.element.removeClass('notfound');
        this.element.addClass('searching');
        var source = inventory.templates.find('input[name="source"]:checked').val();
        $.getJSON(
          inventory.templates.attr('action').replace('templates', source+'_autocomplete'),
          {term : this.element.val()},
          function(data){
            if(data.length==0){
              //change visual indicator to notfound and hide menu again
              inventory.templates.find('input.term').removeClass("searching");
              inventory.templates.find('input.term').addClass("notfound");
              $('.ui-autocomplete').hide();
            } else {
              // Remove search symbol
              inventory.templates.find('input.term').removeClass("searching");
              response(data);
            }
          }
        );
      },
      focus: function(event, ui) {
        return false;
      },
      select: function(event, ui) {
        $(this).val(ui.item.name);
        inventory.templates.find('input.id').val(ui.item.id);
        return false;
      }
    })
    .focus(function() {
      $(this).autocomplete('search', $(this).val());
    })
    .keyup(function() {
      //monitor field and remove class 'notfound' if its length is less than 2
      if($(this).val().length < 2) {
        $(this).removeClass('notfound');
        inventory.templates.find('input.id').val('');
      }
    })
    .data('autocomplete')._renderItem = function(ul, item) {
      var term = this.term.replace(/[aou]/, function(m) {
        // to find with term 'wasser' -> 'wasser' and 'gewasesser'
        var hash = {
          'a' : '(ä|a)',
          'o' : '(ö|o)',
          'u' : '(ä|u)'
        };
        return hash[m];
      });
      // highlighting of matches
      var name = item.name;
      if($.trim(term)) {
        var re = new RegExp($.trim(term), 'ig');
        name = name.replace(re, '<span class="ui-state-highlight">$&</span>');
      }
      return $('<li></li>')
        .data('item.autocomplete', item)
        .append('<a>' + (item.owner ? '<em class="owner">'+item.owner+'</em> ' : '') + name + '</a>')
        .appendTo(ul);
    };
  }
  
  // Additional fields form
  inventory.initAdditionalFields = function() {
    var additionalfields = $('#inventory-edit-entry-form');
    if(!additionalfields.size())
      return;
    inventory.initializeFields(additionalfields);
  }
  
  // Custom fields form
  inventory.initCustomFields = function() {
    var customfields = $('#customfields');
    if(!customfields.size())
      return;
    customfields.find('tr:last td:first input').blur(inventory.addCustomField);
  }
  
  inventory.addCustomField = function() {
    if(!$(this).val())
      return;
    var row = $(this).parents('tr');
    var clone = $('<tr>'+row.html()+'</tr>');
    clone.addClass(row.hasClass('odd') ? 'even' : 'odd');
    row.parents('tbody').append(clone);
    clone.find('td:first input').blur(inventory.addCustomField);
    clone.find('input, select').each(function() {
      var name = $(this).attr('name');
      var i = parseInt(name.replace(/[^\d]+(\d+)[^\d]+/, '$1'));
      $(this).attr('name', name.replace('['+i+']', '['+(i+1)+']'));
    });
    $(this).unbind('blur');
  }
  
  $(document).ready(inventory.init);
})(jQuery);
