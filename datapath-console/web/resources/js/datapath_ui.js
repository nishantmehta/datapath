var loader = function ($){
    if (!$) {
        throw new Error ('jQuery');
    }

    var that,
    defaults = {
	contentID: 'datapath-content',
	getDPConfigServlet: 'GetDPConfig',
	saveGLAContentServlet: 'SaveGLAContent',
	getGLAContentServlet: 'GetGLAContent',
	addGLAServlet: 'AddGLA',
	deleteGLAServlet: 'DeleteGLA',
	savePiggyContentServlet: 'SavePiggyContent',
	getPiggyContentServlet: 'GetPiggyContent',
	addPiggyServlet: 'AddPiggy',
	deletePiggyServlet: 'DeletePiggy',
    runPiggyServlet: 'RunPiggy',
	getChartDataServlet: 'GetChartData',
	getQueryResultServlet: 'GetQueryResult',
	timer: null,
	statsInterval: 1000, /*1 sec interval between every stats request*/
        tools: {
            "nav": [
		{label: 'GLA Editor', description: 'Show GLA Editor', id: 'show-gla-editor', icon: 'glas.png', enabled: true, selected: true},
		{label: 'Piggy Editor', description: 'Show Piggy Editor', id: 'show-piggy-editor', icon: 'piggy.png', enabled: true, selected: false},
		{label: 'Charts', description: 'Show Charts', id: 'show-charts-box', icon: 'chart.png', enabled: true, selected: false},
		{label: 'Query Result', description: 'Query Result', id: 'query-result', icon: 'done.png', enabled: true, selected: false},
		{label: 'DataPath Config', description: 'DataPath Configuration Center', id: 'show-dp-config', icon: 'dpconfig.png', enabled: true, selected: false},
		{label: 'About Us', description: 'About the Developers', id: 'show-about-box', icon: 'about.png', enabled: true, selected: false}
            ],
            "glas": [
                {label: 'Create', description: 'Create a new GLA', id: 'create-gla', icon: 'add.png', enabled: true},
                {label: 'Delete', description: 'Delete GLA', id: 'delete-gla', icon: 'remove.png', enabled: true}
            ],
            "piggy": [
                {label: 'Create', description: 'Create a new Piggy', id: 'create-piggy', icon: 'add.png', enabled: true},
                {label: 'Delete', description: 'Delete Piggy', id: 'delete-piggy', icon: 'remove.png', enabled: true}
            ],
            "chart": [
                {label: 'Refresh', description: 'Refresh Charts', id: 'refresh-charts', icon: 'refresh.png', enabled: true}
            ],
            "conf": [
            ]
        },
	snippet: {
	    "load": "<waypointName> = LOAD <relationName>",
	    "join": "<waypointName> = JOIN <dependentWPName1 BY attribute1 [, dependentWPName2 BY attribute2, ....]>",
	    "print": "PRINT <waypointName> USING <attributeList>",
	    "gla": "<waypointName> = GLA (<glaType>) <> USING <inputAttributeList> AS <outputAttributeList>"
	},
	chartOptions: {
	    'gauge': {
		width: 120, height: 120,
		redFrom: 90, redTo: 100,
		yellowFrom:75, yellowTo: 90,
		minorTicks: 10
	    },
	    'line': {
		width: $('body').width()-300,
		height: 200,
		lineWidth: 2,
		hAxis: {
		    title: 'Time (seconds)',
		    showTextEvery: 10,
		    gridlines: {
			count: 61
		    }
		},
		vAxis: {
		    title: 'Usage (%age)',
		    gridlines: {
			count: 6
		    }
		}
	    },
	    'nwline': {
		width: 1000,
		height: 200,
		lineWidth: 1,
		hAxis: {
		    title: 'Time (seconds)',
		    showTextEvery: 10,
		    gridlines: {
			count: 61
		    }
		},
		vAxis: {
		    title: 'KiB per second',
		    gridlines: {
			count: 6
		    }
		}
	    }
	}
    };

    var init = function() {
	this.dpProject = null;
	this.dpProject = {};
        that = this;

	$('#user-info').addClass('user').text('praveen@cise.ufl.edu');
        $('#login-info').addClass('logout').text('LogOut');
	showDPToolbar();
        $.ajax({
            url: defaults.getDPConfigServlet,
            dataType: 'json',
            success: function (response) {
		that.dpProject['errCount'] = 0;
		that.dpProject['warnCount'] = 0;
		that.dpProject['renderCharts'] = false;
		that.dpProject['types'] = [];
		$.merge(that.dpProject.types, response.datapath_json.types);
		that.dpProject['glas'] = $.extend(true, {}, response.datapath_json.glas);
		that.dpProject['piggys'] = $.extend(true, {}, response.datapath_json.piggys);
		that.dpProject['rezs'] = $.extend(true, {}, response.datapath_json.rezs);
		that.dpProject['relations'] = $.extend(true, {}, response.datapath_json.relations);

		that.dpProject['configs'] = getConfigList(); //dp configuration

		that.dpProject['charts'] = {
		    'chart_list': getChartList(),
		    'chart_data': {
			'graphs':  {},
			'gauges': {}
		    },
		    'chart_vis': {
			'graphs':  {},
			'gauges': {}
		    }
		};

		$.each(that.dpProject.charts.chart_list.graphs, function(id, gObj){
		    var data = new google.visualization.DataTable();
		    data.addColumn('timeofday', 'TimeStamp');
		    data.addColumn('number', gObj.name);
		    that.dpProject.charts.chart_data.graphs[gObj.type] = data;
		});

		$.each(that.dpProject.charts.chart_list.gauges, function(id, gObj){
		    var data = new google.visualization.DataTable();
		    data.addColumn('timeofday', 'TimeStamp');
		    data.addColumn('number', gObj.name);
		    that.dpProject.charts.chart_data.gauges[gObj.type] = data;
		});

		//console.log(response);
		$('#' + defaults.contentID).empty();
		showGLAEditor();
		runStatisticsThread();
            },
            error: function (jqXHR, textStatus, errorThrown) {
		console.log(jqXHR);
		notify($.parseJSON(jqXHR.responseText).datapath_error.error_desc, 5000, true);
            }
        });
    };

    var executeAction = function (actionID, args) {
        switch (actionID) {
        case 'refresh-dp-console':
	    location.reload(true);
            break;
        case 'show-gla-editor':
	    that.dpProject.renderCharts = false;
            $('#' + defaults.contentID).empty();
            loadingBox('Loading GLA Editor', 'abs-center');

            setTimeout(function(){
                $('#loading').remove();
		showGLAEditor();
            }, 300);
            break;
        case 'show-piggy-editor':
	    that.dpProject.renderCharts = false;
            $('#' + defaults.contentID).empty();
            loadingBox('Loading Piggy Editor', 'abs-center');

            setTimeout(function(){
                $('#loading').remove();
		showPiggyEditor();
            }, 300);
            break;
        case 'show-charts-box':
	    that.dpProject.renderCharts = true;
            $('#' + defaults.contentID).empty();
            loadingBox('Loading DataPath Graphs', 'abs-center');

            setTimeout(function(){
                $('#loading').remove();
		showChartDiv();
            }, 300);
            break;
        case 'query-result':
	    that.dpProject.renderCharts = false;
            $('#' + defaults.contentID).empty();
            loadingBox('Loading Query Results', 'abs-center');

            setTimeout(function(){
                $('#loading').remove();
		showResultDiv();
            }, 300);
            break;
        case 'show-dp-config':
	    that.dpProject.renderCharts = false;
            $('#' + defaults.contentID).empty();
            loadingBox('Loading DataPath Configuration Center', 'abs-center');

            setTimeout(function(){
                $('#loading').remove();
		showConfigDiv();
            }, 300);
            break;
        case 'show-about-box':
	    that.dpProject.renderCharts = false;
            $('#' + defaults.contentID).empty();
            loadingBox('Loading Developers\'s Information', 'abs-center');

            setTimeout(function(){
                $('#loading').remove();
		notify('Developers\' Information..... Coming Soon...!', 3000, false);
            }, 300);
            break;
        default:
            console.log (actionID, args);
        }
    };

    var showDPToolbar = function() {
        var $toolbar = $('<div/>').attr ('id', 'dp-toolbar'),
        $ul = $('<ul/>').appendTo ($toolbar),
        $a, $li, tool;

        for (var i in defaults.tools.nav) {
            if (defaults.tools.nav.hasOwnProperty(i)) {
		tool = defaults.tools.nav [i];
		$li = $('<li/>');
		$a = $('<a/>').attr ('href', '#' + tool.id)
                    .attr ('id', tool.id)
                    .text (tool.label)
                    .attr ('title', tool.description)
                    .appendTo ($li)
                    .css('background-image', 'url(resources/images/' + tool.icon + ')');
		if (!tool.enabled) {
                    $a.addClass ('disabled');
		}
		if (tool.selected) {
                    $a.addClass ('selected');
		}
		$a.bind('click', function(){
                    if(!$(this).hasClass('disabled')){
			$('a', $toolbar).removeClass('selected');
			$(this).addClass('selected');
			executeAction.apply (that, [$(this).attr('href').substring (1)]);
                    }
                    return false;
		});
		$li.appendTo ($ul);
            }
	}
        $toolbar.appendTo ($('body'));
    };

    var processSaveGLA = function(glaID, glaContent, successCallback, errorCallback){
	$.ajax ({
	    url: defaults.saveGLAContentServlet,
	    type: 'POST',
	    dataType: 'json',
	    data: {gla_id: glaID,
		   gla_content: glaContent
		  },
            success: function (jsonResult) {
        	//console.log(jsonResult);
                successCallback(jsonResult);
            },
            error: function (jqXHR, textStatus, errorThrown) {
		console.log(jqXHR);
		logMessage('error', $.parseJSON(jqXHR.responseText).datapath_error.error_desc);
	    }
	});
    };

    var processSavePiggy = function(pigID, pigContent, successCallback, errorCallback){
	$.ajax ({
	    url: defaults.savePiggyContentServlet,
	    type: 'POST',
	    dataType: 'json',
	    data: {pig_id: pigID,
		   pig_content: pigContent
		  },
            success: function (jsonResult) {
        	//console.log(jsonResult);
                successCallback(jsonResult);
            },
            error: function (jqXHR, textStatus, errorThrown) {
		console.log(jqXHR);
		logMessage('error', $.parseJSON(jqXHR.responseText).datapath_error.error_desc);
	    }
	});
    };

    var showGLAEditor = function () {
        var $glaEditor = $('<div/>').attr ('id', 'gla-editor').addClass('full-panel'),
        $topbar = $('<div/>').addClass ('top-bar'),
	$glaList = $('<div/>').attr ('id', 'gla-list').addClass('left-panel'),
        $resultbox = $('<div/>').attr('id', 'gla-result-box').addClass('result-box'),
        $table = $('<table/>'),
	$bottombar1 = $('<div/>').addClass ('bottom-bar'),
        $a, $li, too,

	$glaBox = $('<div/>').attr ('id', 'gla-box').addClass('center-panel center-panel-gla'),
	$glaTabs = $('<div/>').attr ('id', 'gla-tabs').addClass('result-box result-box-reg-top'),
	$ulglatabs = $('<ul/>').appendTo ($glaTabs),
        $bottombar2 = $('<div/>').addClass ('bottom-bar').append(get_loggerbox('gla'));


        $('<h2>').text ('GLA Editor').appendTo($glaEditor);
        $topbar.appendTo($glaEditor),
        $('<p/>').text ('GLA List').appendTo ($glaList);

	var $tbody = $('<tbody/>').appendTo ($table);
        $.each(that.dpProject.glas, function(id, glaObj) {
	    var $tr = $('<tr/>').data('info', {'gla_id': id, 'gla_name': glaObj.gla_name});

	    $('<td/>').text(glaObj.gla_name).appendTo($tr);
	    $($tr).click(function(event) {
		selectRow(event, this, $('#gla-result-box'));
		var glaID = $(this).data('info').gla_id,
		glaName = $(this).data('info').gla_name;
		if($('#gla-'+glaID).length > 0){
		    $('#gla-tabs').tabs( 'select' , '#gla-'+glaID);
		}
		else{
		    $.ajax ({
			url: defaults.getGLAContentServlet,
			type: 'POST',
			data: {gla_id: glaID},
			success: function (response) {
			    insert_newgla_tab(glaID, response);
			    $('#gla-tabs').tabs( "add", "#gla-" + glaID, glaName);
			    $('#gla-tabs').tabs( 'select' , '#gla-'+glaID);
			},
			error: function (jqXHR, textStatus, errorThrown) {
			    console.log(jqXHR);
			    logMessage('error', $.parseJSON(jqXHR.responseText).datapath_error.error_desc);
			}
		    });
		}
	    });

	    $tr.appendTo($tbody);
	});

        $table.appendTo($resultbox);
        $resultbox.appendTo($glaList);
        var  $form = $('<form/>').submit(function(){
            return false;
        }).appendTo ($bottombar1);

        $('<input/>').addClass ('search-box')
            .attr ('placeholder', 'Search GLAs')
            .attr ('type', 'text')
            .attr ('name', 'filterText')
            .attr ('id', 'filterText')
            .attr ('size','30')
            .keyup (function() {
            }).appendTo ($form);
        $('<span/>').attr('id', 'glaCounter')
            .addClass ('counter')
            .text ('#GLAs: ' + $("tr", $resultbox).length )
	    .appendTo($bottombar1);

        $('<button/>').attr('id', 'newGLA')
            .text('New GLA')
            .attr ('title', 'Create New GLA')
            .click(function(){
		$(get_newgla_box()).dialog({
		    modal: true,
		    draggable: true,
		    width: '650px',
		    resizable: false,
		    title: 'Create a new GLA Definition',
		    buttons: {
			'Create GLA': function() {
			    var glaname, glatype, glaInput, glaOutput, glaProps, dbox = this;
			    glaname = $.trim($('#glaName', this).val());
			    glatype = $('#selGLAType', this).val();
			    gladesc = $('#glaDesc', this).val();
			    glaProps = $('input:checked', this).map(function() { return $(this).val(); }).get();
			    glaInput = $('tr', $('#glaInput')).map(function() {
				return {
				    'name': $('input', this).val(),
				    'type': $('select', this).val()
				};
			    }).get();
			    glaOutput = $('tr', $('#glaOutput')).map(function() {
				return {
				    'name': $('input', this).val(),
				    'type': $('select', this).val()
				};
			    }).get();

			    if(glaname !== ""){
				$.ajax ({
				    url: defaults.addGLAServlet,
				    type: 'POST',
				    dataType: 'json',
				    data: {gla_name: glaname,
					   gla_desc: gladesc,
					   gla_type: glatype},
				    success: function (responseJson) {
					$('input:checkbox', $('#gla-result-box')).prop('checked', false);
					$('tr', $('#gla-result-box')).removeClass('selected');

        				//console.log(responseJson);
					$.extend(that.dpProject.glas, responseJson.gla);
					var $tr = $('<tr/>').data('info', {'gla_id': responseJson.gla_id, 'gla_name': responseJson.gla[responseJson.gla_id].gla_name}).addClass('selected');
					$('<td/>').text(responseJson.gla[responseJson.gla_id].gla_name).appendTo($tr);
					$($tr).click(function(event) {
					    selectRow(event, this, $('#gla-result-box'));
					    var glaID = $(this).data('info').gla_id,
					    glaName = $(this).data('info').gla_name;
					    if($('#gla-'+glaID).length > 0){
						$('#gla-tabs').tabs( 'select' , '#gla-'+glaID);
					    }
					    else{
						$.ajax ({
						    url: defaults.getGLAContentServlet,
						    type: 'POST',
						    data: {gla_id: glaID},
						    success: function (response) {
							insert_newgla_tab(glaID, response);
							$('#gla-tabs').tabs( "add", "#gla-" + glaID, glaName);
							$('#gla-tabs').tabs( 'select' , '#gla-'+glaID);
						    },
						    error: function (jqXHR, textStatus, errorThrown) {
							console.log(jqXHR);
							logMessage('error', $.parseJSON(jqXHR.responseText).datapath_error.error_desc);
						    }
						});
					    }
					});
        				$tr.appendTo($('table' , $('#gla-result-box')));
					$('#glaCounter').text ('#GLAs: ' + $("tr", $('#gla-result-box')).length );

					insert_newgla_tab(responseJson.gla_id, loadGLATemplate(glaname, glatype, glaInput, glaOutput, glaProps));
					$('#gla-tabs').tabs( "add", "#gla-" + responseJson.gla_id, glaname);
					$('#gla-tabs').tabs( 'select' , '#gla-'+responseJson.gla_id);
					setUnsavedGLASpan(true, 'current');
					logMessage('info', 'A new GLA ' + responseJson.gla[responseJson.gla_id].gla_name + ' has been created.');
					$( dbox ).dialog( "destroy" );
					$('#dialog-box').remove();

				    },
				    error: function (jqXHR, textStatus, errorThrown) {
					console.log(jqXHR);
					logMessage('error', $.parseJSON(jqXHR.responseText).datapath_error.error_desc);
				    }
				});
			    }
			    else{
				notify('GLA Name can not be left blank.', 3000, true);
			    }
			},
			'Cancel': function() {
                            $( this ).dialog( "destroy" );
			    $('#dialog-box').remove();
			}
		    }
		});
            })
            .appendTo($topbar);

        $('<button/>').attr('id', 'deleteGLA')
            .text('Delete GLA')
            .attr ('title', 'Delete GLA')
            .click(function(){
		var index = $('#gla-tabs').tabs('option', 'selected'),
		$a = $('a:eq('+index+')', $('ul.ui-tabs-nav', $('#gla-tabs')));
		if($($a).length > 0){
		    var glaRef = $($a).attr('href');
		    if(glaRef !== -1){
			$(get_confirm_box('Do you want to delete GLA \'' + that.dpProject.glas[glaRef.substring(5)].gla_name + '\' ?')).dialog({
			    modal: true,
			    draggable: false,
			    width: '400px',
			    resizable: false,
			    title: 'Delete GLA',
			    buttons: {
				'Yes': function() {
				    var dbox = this;
				    $.ajax ({
					url: defaults.deleteGLAServlet,
					type: 'POST',
					dataType: 'json',
					data: {gla_id: glaRef.substring(5)},
					success: function (jsonResult) {
					    $('#gla-tabs').tabs("remove", glaRef);
					    logMessage('info', 'GLA ' + that.dpProject.glas[glaRef.substring(5)].gla_name + ' has been deleted.');
					    delete that.dpProject.glas[glaRef.substring(5)];

					    $.each($('tr', $('#gla-result-box')), function() {
						var glaID = $(this).data('info').gla_id,
						tr = this;
						if(glaID === glaRef.substring(5)) {
                                                    $(tr).fadeOut(300, function() {
                                                        $(this).remove();
                                                    });
						    return false;
                                                }
					    });
					    $( dbox ).dialog( "destroy" );
					    $('#dialog-box').remove();
					},
					error: function (jqXHR, textStatus, errorThrown) {
					    console.log(jqXHR);
					    logMessage('error', $.parseJSON(jqXHR.responseText).datapath_error.error_desc);
					    $( dbox ).dialog( "destroy" );
					    $('#dialog-box').remove();
					}
				    });
				},
				'No': function() {
				    $( this ).dialog( "destroy" );
				    $('#dialog-box').remove();
				}
			    }
			});
		    }
		}
            })
            .appendTo($topbar);

	$('<span/>').css({'display': 'inline-block', 'width': '50px'}).appendTo($topbar);
        $('<button/>').attr('id', 'saveGLA')
            .text('Save')
            .attr ('title', 'Save this GLA')
            .click(function(){
		var glaRef = getUnsavedGLAID('current');
		//console.log(glaRef);
		if(glaRef !== -1){
		    var editor = $(glaRef).data('editorRef');
		    processSaveGLA(glaRef.substring(5),
				   editor.getValue(),
				   function(responseJson){
				       setUnsavedGLASpan(false, 'current');
				       logMessage('info', 'GLA ' + that.dpProject.glas[glaRef.substring(5)].gla_name + ' has been saved.');
				   },
				   function(jqXHR, textStatus, errorThrown){
				   }
				  );
		}
            })
            .appendTo($topbar);

        $('<button/>').attr('id', 'Reset')
            .text('Reset')
            .attr ('title', 'Reset content of GLA')
            .click(function(){
		var index = $('#gla-tabs').tabs('option', 'selected'),
		$a = $('a:eq('+index+')', $('ul.ui-tabs-nav', $('#gla-tabs')));
		if($($a).length > 0){
		    var glaRef = $($a).attr('href');
		    if(glaRef !== -1){
			$(get_confirm_box('Do you want to reset the content of \'' + that.dpProject.glas[glaRef.substring(5)].gla_name + '\' ?')).dialog({
			    modal: true,
			    draggable: false,
			    width: '400px',
			    resizable: false,
			    title: 'Reset Editor',
			    buttons: {
				'Yes': function() {
				    var dbox = this,
				    editor = $(glaRef).data('editorRef');
				    editor.setValue('');
				    setUnsavedGLASpan(true, 'current');
				    logMessage('info', 'GLA ' + that.dpProject.glas[glaRef.substring(5)].gla_name + ' has been reset.');
				    $( dbox ).dialog( "destroy" );
				    $('#dialog-box').remove();
				},
				'No': function() {
				    $( this ).dialog( "destroy" );
				    $('#dialog-box').remove();
				}
			    }
			});
		    }
		}
            })
            .appendTo($topbar);

        $('<button/>').attr('id', 'cleanUp')
            .text('CleanUp')
            .attr ('title', 'Clean GLA Code')
            .click(function(){
		index = $('#gla-tabs').tabs('option', 'selected'),
		$a = $('a:eq('+index+')', $('ul.ui-tabs-nav', $('#gla-tabs')));
		if($($a).length > 0){
		    glaRef = $($a).attr('href');
		    if(glaRef !== -1){
			var editor = $(glaRef).data('editorRef');
			//console.log(editor.getValue());
			CodeMirror.commands["selectAll"](editor);
			editor.autoFormatRange(editor.getCursor(true), editor.getCursor(false));
			setUnsavedGLASpan(true, 'current');
			logMessage('info', 'GLA ' + that.dpProject.glas[glaRef.substring(5)].gla_name + ' has been cleaned up.');
		    }
		}
            })
            .appendTo($topbar);

	$('<span/>').css({'display': 'inline-block', 'width': '50px'}).appendTo($topbar);
        $('<button/>').attr('id', 'saveAllGLA')
            .text('Save All')
            .attr ('title', 'Save All GLAs')
            .click(function(){
		var glaRef = getUnsavedGLAID('all'), glaNames = '';
		//console.log(glaRef);
		$.each(glaRef, function(i){
		    var editor = $(glaRef[i]).data('editorRef');
		    glaNames = glaNames + ', ' + that.dpProject.glas[glaRef[i].substring(5)].gla_name;
		    processSaveGLA(glaRef[i].substring(5),
				   editor.getValue(),
				   function(responseJson){
				   },
				   function(jqXHR, textStatus, errorThrown){
				   }
				  );
		});
		setUnsavedGLASpan(false, 'all');
		logMessage('info', 'GLAs ' + glaNames + ' have been saved.');
            })
            .appendTo($topbar);

        $('<button/>').attr('id', 'closeAllGLA')
            .text('Close All')
            .attr ('title', 'Close All GLAs')
            .click(function(){
		var glaRefs = getUnsavedGLAID('all');
		if(glaRefs.length> 0){
		    $(get_confirm_box('Save changes to GLAs before closing?')).dialog({
			modal: true,
			draggable: false,
			width: '400px',
			resizable: false,
			title: 'Save Confirmation',
			buttons: {
			    'Save': function() {
				var dbox = this, glaNames = '';
				//console.log(glaRefs);
				$.each(glaRefs, function(i){
				    var editor = $(glaRefs[i]).data('editorRef');
				    glaNames = glaNames + ', ' + that.dpProject.glas[glaRefs[i].substring(5)].gla_name;
				    processSaveGLA(glaRefs[i].substring(5),
						   editor.getValue(),
						   function(responseJson){
						       $('#gla-tabs').tabs("remove", glaRefs[i]);
						   },
						   function(jqXHR, textStatus, errorThrown){
						   }
						  );
				});
				var tab_count = $('#gla-tabs').tabs('length'), i;
				for (i=0; i<tab_count; i++){
				    $('#gla-tabs').tabs( "remove" , 0 )
				}
				logMessage('info', 'GLAs ' + glaNames + ' have been saved.');

				$( dbox ).dialog( "destroy" );
				$('#dialog-box').remove();
			    },
			    'Cancel': function() {
				$( this ).dialog( "destroy" );
				$('#dialog-box').remove();
			    },
			    'Discard': function() {
				var tab_count = $('#gla-tabs').tabs('length'), i;
				for (i=0; i<tab_count; i++){
				    $('#gla-tabs').tabs( "remove" , 0 )
				}
				$( this ).dialog( "destroy" );
				$('#dialog-box').remove();
			    }
			}
		    });
		}
		else {
		    var tab_count = $('#gla-tabs').tabs('length'), i;
		    for (i=0; i<tab_count; i++){
			$('#gla-tabs').tabs( "remove" , 0 )
		    }
		}
            })
            .appendTo($topbar);

	// tabs init with a custom tab template and an "add" callback filling in the content
	$($glaTabs).tabs({
	    tabTemplate: "<li><a href='#{href}'>#{label}<span class='unsaved'> *</span></a><span class='ui-icon ui-icon-closethick'>Remove Tab</span></li>"
	});

	// close icon: removing the tab on click
	$( 'span.ui-icon-closethick', $('#gla-tabs') ).live( 'click', function() {
	    var liindex = $('li', $('#gla-tabs')).index( $( this ).parent() );
	    if(liindex !== -1){ //ui problem
		var $a = $('a:eq('+liindex+')', $('ul.ui-tabs-nav', $('#gla-tabs')));
		if($('span.unsaved-active', $($a)).length > 0){
		    //unsaved, confirm
		    $(get_confirm_box('Save changes to GLA before closing?')).dialog({
			modal: true,
			draggable: false,
			width: '400px',
			resizable: false,
			title: 'Save Confirmation',
			buttons: {
			    'Save': function() {
				var dbox = this;
				var glaRef = $($a).attr('href');
				//console.log(glaRef);
				if(glaRef !== -1){
				    var editor = $(glaRef).data('editorRef');
				    processSaveGLA(glaRef.substring(5),
						   editor.getValue(),
						   function(responseJson){
						       $('#gla-tabs').tabs('remove', liindex);
						       logMessage('info', 'GLA ' + that.dpProject.glas[glaRef.substring(5)].gla_name + ' has been saved.');
						   },
						   function(jqXHR, textStatus, errorThrown){
						   }
						  );
				}
				$( dbox ).dialog( "destroy" );
				$('#dialog-box').remove();
			    },
			    'Cancel': function() {
				$( this ).dialog( "destroy" );
				$('#dialog-box').remove();
			    },
			    'Discard': function() {
				$('#gla-tabs').tabs('remove', liindex);
				$( this ).dialog( "destroy" );
				$('#dialog-box').remove();
			    }
			}
		    });
		}
		else {
		    $('#gla-tabs').tabs('remove', liindex);
		}
	    }
	});

        $bottombar1.appendTo ($glaList);
        $glaList.appendTo ($glaEditor);
	$glaTabs.appendTo ($glaBox);
        $bottombar2.appendTo ($glaBox);
        $glaBox.appendTo ($glaEditor);
        $glaEditor.appendTo('#' + defaults.contentID);
    };

    var insert_newgla_tab = function (glaID, glaContent){
	var $glaText = $('<div/>').attr ('id', 'gla-'+glaID).appendTo($('#gla-tabs')),
	$glaTextArea = $('<textarea/>').attr ('id', 'glaTextArea-'+glaID).appendTo($($glaText)),
	editor, foldFunc, hlLine;

	foldFunc = CodeMirror.newFoldFunction(CodeMirror.braceRangeFinder);
	editor = CodeMirror.fromTextArea(document.getElementById('glaTextArea-'+glaID), {
            lineNumbers: true,
	    lineWrapping: true,
            matchBrackets: true,
	    onGutterClick: foldFunc,
	    extraKeys: {"Ctrl-Q": function(cm){foldFunc(cm, cm.getCursor().line);}},
            mode: "text/x-c++src",
	    onCursorActivity: function() {
		editor.setLineClass(hlLine, null, null);
		hlLine = editor.setLineClass(editor.getCursor().line, null, "activeline");
	    },
	    onKeyEvent: function(){
		setUnsavedGLASpan(true, 'current');
	    }
	});
	hlLine = editor.setLineClass(0, "activeline");
	editor.setValue(glaContent);
	CodeMirror.commands['selectAll'](editor);
	editor.autoFormatRange(editor.getCursor(true), editor.getCursor(false));
	$($glaText).data('editorRef', editor);
    };

    var get_newgla_box = function (){
	$('#dialog-box').remove();
        var $box = $('<div/>').attr('id', 'dialog-box'),
        $form = $('<form/>').submit(function(){
            return false;
        }).appendTo ($box),
	$hoverinfo = $('<div>').addClass('hover-info'),
        $table = $('<table/>').addClass('dialog-box-table'),
        $tr1 = $('<tr/>').appendTo($table).addClass('dialog-box-table-tr'), $input1,
        $tr6 = $('<tr/>').appendTo($table).addClass('dialog-box-table-tr'), $input6,
        $tr2 = $('<tr/>').appendTo($table).addClass('dialog-box-table-tr'),$select2,
        $tr5 = $('<tr/>').appendTo($table).addClass('dialog-box-table-tr'), $chbox1, $chbox1label, $chbox2, $chbox2label,
        $tr3 = $('<tr/>').appendTo($table).addClass('dialog-box-table-tr'),
        $tr4 = $('<tr/>').appendTo($table).addClass('dialog-box-table-tr'),

	$inputAttBox = $('<div>').addClass('io-box io-box-reg'),
	$outputAttBox = $('<div>').addClass('io-box io-box-reg'),
	$inputAttBtns= $('<ul>'),
	$outputAttBtns= $('<ul>'),
	$inputTable = $('<table/>').attr('id', 'glaInput').addClass('io-box-table io-box-table-reg').appendTo($inputAttBox),
	$outputTable = $('<table/>').attr('id', 'glaOutput').addClass('io-box-table io-box-table-reg').appendTo($outputAttBox),
	$addBtnI = $('<button/>').attr('id', 'addAttI').text('Add')
	    .click(function(){
		var $tr = $('<tr/>').addClass('io-box-tr').click(function(){
		    $('tr', $inputAttBox).removeClass('selected');
		    $(this).addClass('selected');
		}),
		$input, $select;
		$input = $('<input/>').attr ('placeholder', 'Enter Attribute Name')
		    .attr ('type', 'text');
		$('<td/>').append ($input).appendTo ($tr);

		$select=$('<select/>');

		$.each(that.dpProject.types, function(i) {
		    $('<option/>').attr('value', that.dpProject.types[i]).text(that.dpProject.types[i]).appendTo ($select);
		});

		$('<td/>').append ($select).appendTo ($tr);
		$tr.appendTo($($inputTable));
	    }),

	$deleteBtnI = $('<button/>').attr('id', 'deleteAttI').text('Delete')
	    .click(function(){
		$.each($('tr.selected', $inputAttBox), function(){
		    $(this).fadeOut(function(){
			$(this).remove();
		    });
		});
	    }),
	$moveUpBtnI = $('<button/>').attr('id', 'moveUpAttI').text('Move Up')
	    .click(function(){
		var selected = $('tr.selected', $inputAttBox);
		selected.insertBefore(selected.prev());
	    }),
	$moveDownBtnI = $('<button/>').attr('id', 'moveDownAttI').text('Move Down')
	    .click(function(){
		var selected = $('tr.selected', $inputAttBox);
		selected.insertAfter(selected.next());
	    }),
	$addBtnO = $('<button/>').attr('id', 'addAttO').text('Add')
	    .click(function(){
		var $tr = $('<tr/>').addClass('io-box-tr')
		    .click(function(){
			$('tr', $outputAttBox).removeClass('selected');
			$(this).addClass('selected');
		    }),
		$input, $select;
		$input = $('<input/>').attr ('placeholder', 'Enter Attribute Name')
		    .attr ('type', 'text');
		$('<td/>').append ($input).appendTo ($tr);

		$select=$('<select/>');
		for (var i in that.dpProject.types) {
		    if (that.dpProject.types.hasOwnProperty(i)) {
			$('<option/>').attr('value', that.dpProject.types[i]).text(that.dpProject.types[i]).appendTo ($select);
		    }
		}
		$('<td/>').append ($select).appendTo ($tr);
		$tr.appendTo($($outputTable));
	    }),

	$deleteBtnO = $('<button/>').attr('id', 'deleteAttO').text('Delete')
	    .click(function(){
		$.each($('tr.selected', $outputAttBox), function(){
		    $(this).fadeOut(function(){
			$(this).remove();
		    });
		});
	    }),
	$moveUpBtnO = $('<button/>').attr('id', 'moveUpAttO').text('Move Up')
	    .click(function(){
		var selected = $('tr.selected', $outputAttBox);
		selected.insertBefore(selected.prev());
	    }),
	$moveDownBtnO = $('<button/>').attr('id', 'moveDownAttO').text('Move Down')
	    .click(function(){
		var selected = $('tr.selected', $outputAttBox);
		selected.insertAfter(selected.next());
	    });

	$('<li/>').append($addBtnI).appendTo($inputAttBtns);
	$('<li/>').append($deleteBtnI).appendTo($inputAttBtns);
	$('<li/>').append($moveUpBtnI).appendTo($inputAttBtns);
	$('<li/>').append($moveDownBtnI).appendTo($inputAttBtns);
//	$inputAttBtns.appendTo($inputAttBox);

	$('<li/>').append($addBtnO).appendTo($outputAttBtns);
	$('<li/>').append($deleteBtnO).appendTo($outputAttBtns);
	$('<li/>').append($moveUpBtnO).appendTo($outputAttBtns);
	$('<li/>').append($moveDownBtnO).appendTo($outputAttBtns);
//	$outputAttBtns.appendTo($outputAttBox);

        $('<td/>').addClass('dialog-box-table-td').text('GLA Name').appendTo ($tr1);
        $input1 = $('<input/>').attr ('placeholder', 'Enter GLA Name')
            .attr ('type', 'text')
            .attr ('name', 'glaName')
            .attr ('id', 'glaName')
	    .hover(function(){$($hoverinfo).text('A valid GLA Name e.g. AverageGLA, TopKGLA');}, function(){$($hoverinfo).text('')});
        $('<td/>').addClass('dialog-box-table-td').append ($input1).appendTo ($tr1);

        $('<td/>').addClass('dialog-box-table-td').text('GLA Desc').appendTo ($tr6);
        $input6 = $('<input/>').attr ('placeholder', 'Enter GLA Description')
            .attr ('type', 'text')
            .attr ('name', 'glaDesc')
            .attr ('id', 'glaDesc')
	    .hover(function(){$($hoverinfo).text('An optional GLA description.');}, function(){$($hoverinfo).text('')});
        $('<td/>').addClass('dialog-box-table-td').append ($input6).appendTo ($tr6);

        $('<td/>').addClass('dialog-box-table-td').text('GLA Type').appendTo($tr2);
        $select2 = $('<select/>').attr ('name', 'selGLAType')
            .attr ('id', 'selGLAType')
	    .bind("change keyup", function(event){
		if($(this).val() === 'state'){
		    $('button', $outputAttBtns).attr('disabled', 'disabled');
		}
		else {
		    $('button', $outputAttBtns).removeAttr('disabled');
		}
	    })
	    .hover(function(){$($hoverinfo).text('Select a GLA Type to use from the list.');}, function(){$($hoverinfo).text('')});
        $('<option/>').attr('value', 'none').text('Select Type').appendTo ($select2);
        $('<option/>').attr('value', 'state').text('STATE').appendTo ($select2);
        $('<option/>').attr('value', 'single').text('SINGLE').appendTo ($select2);
        $('<option/>').attr('value', 'multi').text('MULTI').appendTo ($select2);
        $('<option/>').attr('value', 'fragment').text('FRAGMENT').appendTo ($select2);
        $('<td/>').addClass('dialog-box-table-td').append ($select2).appendTo ($tr2);

	$('<td/>').addClass('dialog-box-table-td').text('GLA Properties').appendTo($tr5);

	$chbox1label = $('<label for="checkbox1" />')
	    .text ('Chunk Boundary');

	$chbox1 = $('<input/>').attr ('type', 'checkbox')
	    .attr('id', 'checkbox1')
	    .attr('value', 'CHUNKBOUNDARY')
	    .prop ('checked', false)
	    .click ( function(){
	    });

	$chbox2label = $('<label for="checkbox2" />')
	    .text ('Iteratable');
	$chbox2 = $('<input/>').attr ('type', 'checkbox')
	    .attr('id', 'checkbox2')
	    .attr('value', 'ITERATABLE')
	    .prop ('checked', false)
	    .click ( function(){
	    });

	$('<td/>').append ($chbox1).append ($chbox1label).append ($chbox2).append ($chbox2label).appendTo ($tr5);

	$('<td/>').addClass('dialog-box-table-td').text('GLA Input Attributes').appendTo($tr3);
	$('<td/>').addClass('dialog-box-table-td').append($inputAttBox).append($inputAttBtns).appendTo($tr3);

	$('<td/>').addClass('dialog-box-table-td').text('GLA Output Attributes').appendTo($tr4);
	$('<td/>').addClass('dialog-box-table-td').append($outputAttBox).append($outputAttBtns).appendTo($tr4);

	$table.appendTo ($form);
	$hoverinfo.appendTo ($box);
        $box.input = function() {
            return $form.serialize();
        };
        return $box;
    };

    var showPiggyEditor = function () {
        var $piggyEditor = $('<div/>').attr ('id', 'piggy-editor').addClass('full-panel'),
        $topbar = $('<div/>').addClass ('top-bar'),
	$piggyList = $('<div/>').attr ('id', 'piggy-list').addClass('left-panel'),
        $resultbox = $('<div/>').attr('id', 'piggy-result-box').addClass('result-box'),
        $table = $('<table/>'),
	$bottombar1 = $('<div/>').addClass ('bottom-bar'),
        $a, $li, too,

	$piggyBox = $('<div/>').attr ('id', 'piggy-box').addClass('center-panel center-panel-piggy'),
	$piggyTabs = $('<div/>').attr ('id', 'piggy-tabs').addClass('result-box result-box-reg-top'),
	$ulpiggytabs = $('<ul/>').appendTo ($piggyTabs),
        $bottombar2 = $('<div/>').addClass ('bottom-bar').append(get_loggerbox('piggy')),

	$glaDragList = $('<div/>').attr ('id', 'gla-drag-list').addClass('right-panel'),
	$div = $('<div/>').attr ('id', 'gla-accordion').appendTo($glaDragList),
	$h31 = $('<h2>').append($('<a/>').attr('href', '#').text('Code Snippets')), $div1 = $('<div/>'), $table1 = $('<table/>'),
	$h32 = $('<h2>').append($('<a/>').attr('href', '#').text('Relations')), $div2 = $('<div/>'), $table2 = $('<table/>'),
	$h33 = $('<h2>').append($('<a/>').attr('href', '#').text('GLAs')), $div3 = $('<div/>'), $table3 = $('<table/>'),
	$h34 = $('<h2>').append($('<a/>').attr('href', '#').text('GLA Templates')), $div4 = $('<div/>'), $table4 = $('<table/>');

	$h31.appendTo($div);
	$div1.appendTo($div);
	$table1.appendTo($div1);
	$h32.appendTo($div);
	$div2.appendTo($div);
	$table2.appendTo($div2);
	$h33.appendTo($div);
	$div3.appendTo($div);
	$table3.appendTo($div3);
	$h34.appendTo($div);
	$div4.appendTo($div);
	$table4.appendTo($div4);

        $('<h2>').text ('Piggy Editor').appendTo($piggyEditor);
	$topbar.appendTo($piggyEditor),
        $('<p/>').text ('Piggy List').appendTo ($piggyList);

	var $tbody = $('<tbody/>').appendTo ($table);
        $.each(that.dpProject.piggys, function(id, pigObj) {
	    var $tr = $('<tr/>').data('info', {'pig_id': id, 'pig_name': pigObj.pig_name});
	    $('<td/>').text(pigObj.pig_name).appendTo($tr);
	    $($tr).click(function(event) {
		selectRow(event, this, $('#piggy-result-box'));
		var pigID = $(this).data('info').pig_id,
		pigName = $(this).data('info').pig_name;
		if($('#pig-'+pigID).length > 0){
		    $('#piggy-tabs').tabs( 'select' , '#pig-'+pigID);
		}
		else{
		    $.ajax ({
			url: defaults.getPiggyContentServlet,
			type: 'POST',
			data: {pig_id: pigID},
			success: function (response) {
			    insert_newpiggy_tab(pigID, response);
			    $('#piggy-tabs').tabs( "add", "#pig-" + pigID, pigName);
			    $('#piggy-tabs').tabs( 'select' , '#pig-'+pigID);
			},
			error: function (jqXHR, textStatus, errorThrown) {
			    console.log(jqXHR);
			    logMessage('error', $.parseJSON(jqXHR.responseText).datapath_error.error_desc);
			}
		    });
		}
	    });
	    $tr.appendTo($tbody);

	});

        $table.appendTo($resultbox);
        $resultbox.appendTo($piggyList);
        var  $form = $('<form/>').submit(function(){
            return false;
        }).appendTo ($bottombar1);

        $('<input/>').addClass ('search-box')
            .attr ('placeholder', 'Search Piggys')
            .attr ('type', 'text')
            .attr ('name', 'filterText')
            .attr ('id', 'filterText')
            .attr ('size','30')
            .keyup (function() {
            }).appendTo ($form);
        $('<span/>').attr('id', 'piggyCounter')
            .addClass ('counter')
            .text ('#Pgy: ' + $("tr", $resultbox).length )
	    .appendTo($bottombar1);

        $('<button/>').attr('id', 'newPiggy')
            .text('New Piggy')
            .attr ('title', 'Create New Piggy')
            .click(function(){
		$(get_newpiggy_box()).dialog({
		    modal: true,
		    draggable: true,
		    width: '450px',
		    resizable: false,
		    title: 'Create a new Piggy Definition',
		    buttons: {
			'Create Piggy': function() {
			    var pigname, pigdesc, dbox = this;
			    pigname = $.trim($('#pigName', this).val());
			    pigdesc = $('#pigDesc', this).val();
			    if(pigname !== ""){
				$.ajax ({
				    url: defaults.addPiggyServlet,
				    type: 'POST',
				    dataType: 'json',
				    data: {pig_name: pigname,
					   pig_desc: pigdesc},
				    success: function (responseJson) {
					$('input:checkbox', $('#piggy-result-box')).prop('checked', false);
					$('tr', $('#piggy-result-box')).removeClass('selected');

        				//console.log(responseJson);
					$.extend(that.dpProject.glas, responseJson.piggy);
					var $tr = $('<tr/>').data('info', {'pig_id': responseJson.pig_id, 'pig_name': responseJson.piggy[responseJson.pig_id].pig_name}).addClass('selected');
					$('<td/>').text(responseJson.piggy[responseJson.pig_id].pig_name).appendTo($tr);
					$($tr).click(function(event) {
					    selectRow(event, this, $('#piggy-result-box'));
					    var pigID = $(this).data('info').pig_id,
					    pigName = $(this).data('info').pig_name;
					    if($('#pig-'+pigID).length > 0){
						$('#piggy-tabs').tabs( 'select' , '#pig-'+pigID);
					    }
					    else{
						$.ajax ({
						    url: defaults.getPiggyContentServlet,
						    type: 'POST',
						    data: {pig_id: pigID},
						    success: function (response) {
							insert_newpiggy_tab(pigID, response);
							$('#piggy-tabs').tabs( "add", "#pig-" + pigID, pigName);
							$('#piggy-tabs').tabs( 'select' , '#pig-'+pigID);
						    },
						    error: function (jqXHR, textStatus, errorThrown) {
							console.log(jqXHR);
							logMessage('error', $.parseJSON(jqXHR.responseText).datapath_error.error_desc);
						    }
						});
					    }
					});

        				$tr.appendTo($('table' , $('#piggy-result-box')));
					$('#piggyCounter').text ('#Pgy: ' + $("tr", $('#piggy-result-box')).length );

					insert_newpiggy_tab(responseJson.pig_id, ' ');
					$('#piggy-tabs').tabs( "add", "#pig-" + responseJson.pig_id, pigname);
					$('#piggy-tabs').tabs( 'select' , '#pig-'+responseJson.pig_id);
					setUnsavedPiggySpan(true, 'current');
					logMessage('info', 'A new Piggy ' + responseJson.piggy[responseJson.pig_id].pig_name + ' has been added.');
					$( dbox ).dialog( "destroy" );
					$('#dialog-box').remove();

				    },
				    error: function (jqXHR, textStatus, errorThrown) {
					console.log(jqXHR);
					logMessage('error', $.parseJSON(jqXHR.responseText).datapath_error.error_desc);
				    }
				});


			    }
			    else{
				notify('Piggy Name can not be left blank.', 3000, true);
			    }
			},
			'Cancel': function() {
                            $( this ).dialog( "destroy" );
			    $('#dialog-box').remove();
			}
		    }
		});

            })
            .appendTo($topbar);

        $('<button/>').attr('id', 'deletePiggy')
            .text('Delete Piggy')
            .attr ('title', 'Delete selected Piggy')
            .click(function(){
		var index = $('#piggy-tabs').tabs('option', 'selected'),
		$a = $('a:eq('+index+')', $('ul.ui-tabs-nav', $('#piggy-tabs')));
		if($($a).length > 0){
		    piggyRef = $($a).attr('href');
		    if(piggyRef !== -1){
			$(get_confirm_box('Do you want to delete \'' + that.dpProject.piggys[piggyRef.substring(5)].pig_name + '\' ?')).dialog({
			    modal: true,
			    draggable: false,
			    width: '400px',
			    resizable: false,
			    title: 'Delete Piggy',
			    buttons: {
				'Yes': function() {
				    var dbox = this;
				    $.ajax ({
					url: defaults.deletePiggyServlet,
					type: 'POST',
					dataType: 'json',
					data: {pig_id: piggyRef.substring(5)},
					success: function (jsonResult) {
					    $('#piggy-tabs').tabs("remove", piggyRef);
					    logMessage('info', 'Piggy ' + that.dpProject.piggys[piggyRef.substring(5)].pig_name + ' has been deleted.');
					    delete that.dpProject.piggys[piggyRef.substring(5)];

					    $.each($('tr', $('#piggy-result-box')), function() {
						var pigID = $(this).data('info').pig_id,
						tr = this;
						if(pigID === piggyRef.substring(5)) {
                                                    $(tr).fadeOut(300, function() {
                                                        $(this).remove();
                                                    });
						    return false;
                                                }
					    });
					    $( dbox ).dialog( "destroy" );
					    $('#dialog-box').remove();

					},
					error: function (jqXHR, textStatus, errorThrown) {
					    console.log(jqXHR);
					    logMessage('error', $.parseJSON(jqXHR.responseText).datapath_error.error_desc);
					    $( dbox ).dialog( "destroy" );
					    $('#dialog-box').remove();
					}
				    });
				},
				'No': function() {
				    $( this ).dialog( "destroy" );
				    $('#dialog-box').remove();
				}
			    }
			});
		    }
		}
            })
            .appendTo($topbar);

	$('<span/>').css({'display': 'inline-block', 'width': '50px'}).appendTo($topbar);

        $('<button/>').attr('id', 'savePiggy')
            .text('Save')
            .attr ('title', 'Save this Piggy')
            .click(function(){
		var piggyRef = getUnsavedPiggyID('current');
		//console.log(piggyRef);
		if(piggyRef !== -1){
		    var editor = $(piggyRef).data('editorRef');
		    processSavePiggy(piggyRef.substring(5),
				   editor.getValue(),
				   function(responseJson){
				       setUnsavedPiggySpan(false, 'current');
				       logMessage('info', 'Piggy ' + that.dpProject.piggys[piggyRef.substring(5)].pig_name + ' has been saved.');
				   },
				   function(jqXHR, textStatus, errorThrown){
				   }
				  );
		}
            })
            .appendTo($topbar);
        $('<button/>').attr('id', 'Reset')
            .text('Reset')
            .attr ('title', 'Reset content of Piggy')
            .click(function(){
		var index = $('#piggy-tabs').tabs('option', 'selected'),
		$a = $('a:eq('+index+')', $('ul.ui-tabs-nav', $('#piggy-tabs')));
		if($($a).length > 0){
		    piggyRef = $($a).attr('href');
		    if(piggyRef !== -1){
			$(get_confirm_box('Do you want to reset the content of \'' + that.dpProject.piggys[piggyRef.substring(5)].pig_name + '\' ?')).dialog({
			    modal: true,
			    draggable: false,
			    width: '400px',
			    resizable: false,
			    title: 'Reset Editor',
			    buttons: {
				'Yes': function() {
				    var dbox = this,
				    editor = $(piggyRef).data('editorRef');
				    editor.setValue('');
				    setUnsavedGLASpan(true, 'current');
				    logMessage('info', 'Piggy ' + that.dpProject.piggys[piggyRef.substring(5)].pig_name + ' has been reset.');
				    $( dbox ).dialog( "destroy" );
				    $('#dialog-box').remove();
				},
				'No': function() {
				    $( this ).dialog( "destroy" );
				    $('#dialog-box').remove();
				}
			    }
			});
		    }
		}
            })
            .appendTo($topbar);

        $('<button/>').attr('id', 'cleanUp')
            .text('CleanUp')
            .attr ('title', 'Clean Piggy Code')
            .click(function(){
		index = $('#piggy-tabs').tabs('option', 'selected'),
		$a = $('a:eq('+index+')', $('ul.ui-tabs-nav', $('#piggy-tabs')));
		if($($a).length > 0){
		    piggyRef = $($a).attr('href');
		    if(piggyRef !== -1){
			var editor = $(piggyRef).data('editorRef');
			//console.log(editor.getValue());
			CodeMirror.commands["selectAll"](editor);
			editor.autoFormatRange(editor.getCursor(true), editor.getCursor(false));
			setUnsavedGLASpan(true, 'current');
			logMessage('info', 'Piggy ' + that.dpProject.piggys[piggyRef.substring(5)].pig_name + ' has been cleaned up.');
		    }
		}
            })
            .appendTo($topbar);
        $('<span/>').css({'display': 'inline-block', 'width': '50px'}).appendTo($topbar);
        $('<button/>').attr('id', 'executePiggy')
        .text('Execute Piggy')
        .attr ('title', 'Execute this Piggy')
        .click(function(){
		        var index = $('#piggy-tabs').tabs('option', 'selected'),
			        $a = $('a:eq('+index+')', $('ul.ui-tabs-nav', $('#piggy-tabs')));
		        if($($a).length > 0){
			        piggyRef = $($a).attr('href');
			        if(piggyRef !== -1){
				        var dbox = this;
				        logMessage('info', 'Piggy ' + that.dpProject.piggys[piggyRef.substring(5)].pig_name + ' is running');
				        $.ajax ({
						        url: defaults.runPiggyServlet,
							        type: 'POST',
							        dataType: 'json',
							        data: {pig_id: piggyRef.substring(5)},
							        success: function (jsonResult) {
							        logMessage('info', 'Piggy ' + that.dpProject.piggys[piggyRef.substring(5)].pig_name + ' is running.');
						        }
					        })
					        }
		        }
	        })
        .appendTo($topbar);

	$('<span/>').css({'display': 'inline-block', 'width': '50px'}).appendTo($topbar);
        $('<button/>').attr('id', 'saveAllPiggy')
            .text('Save All')
            .attr ('title', 'Save All Piggys')
            .click(function(){
		var piggyRef = getUnsavedPiggyID('all'), pigNames = '';
		//console.log(piggyRef);
		$.each(piggyRef, function(i){
		    var editor = $(piggyRef[i]).data('editorRef');
		    pigNames = pigNames + ', ' + that.dpProject.piggys[piggyRef[i].substring(5)].pig_name;
		    processSavePiggy(piggyRef[i].substring(5),
				   editor.getValue(),
				   function(responseJson){
				   },
				   function(jqXHR, textStatus, errorThrown){
				   }
				  );
		});
		setUnsavedGLASpan(false, 'all');
		logMessage('info', 'Piggys ' + pigNames + ' have been saved.');
            })
            .appendTo($topbar);
        $('<button/>').attr('id', 'closeAllPiggy')
            .text('Close All')
            .attr ('title', 'Close All Piggys')
            .click(function(){
		var piggyRefs = getUnsavedPiggyID('all');
		if(piggyRefs.length> 0){
		    $(get_confirm_box('Save changes to Piggys before closing?')).dialog({
			modal: true,
			draggable: false,
			width: '400px',
			resizable: false,
			title: 'Save Confirmation',
			buttons: {
			    'Save': function() {
				var dbox = this, pigNames = '';
				//console.log(piggyRefs);
				$.each(piggyRefs, function(i){
				    var editor = $(piggyRefs[i]).data('editorRef');
				    pigNames = pigNames + ', ' + that.dpProject.piggys[piggyRef[i].substring(5)].pig_name;
				    processSavePiggy(piggyRefs[i].substring(5),
						   editor.getValue(),
						   function(responseJson){
						       $('#piggy-tabs').tabs("remove", piggyRefs[i]);
						   },
						   function(jqXHR, textStatus, errorThrown){
						   }
						  );
				});
				var tab_count = $('#piggy-tabs').tabs('length'), i;
				for (i=0; i<tab_count; i++){
				    $('#piggy-tabs').tabs( "remove" , 0 )
				}
				logMessage('info', 'Piggys ' + pigNames + ' have been saved.');
				$( dbox ).dialog( "destroy" );
				$('#dialog-box').remove();
			    },
			    'Cancel': function() {
				$( this ).dialog( "destroy" );
				$('#dialog-box').remove();
			    },
			    'Discard': function() {
				var tab_count = $('#piggy-tabs').tabs('length'), i;
				for (i=0; i<tab_count; i++){
				    $('#piggy-tabs').tabs( "remove" , 0 )
				}
				$( this ).dialog( "destroy" );
				$('#dialog-box').remove();
			    }
			}
		    });
		}
		else {
		    var tab_count = $('#piggy-tabs').tabs('length'), i;
		    for (i=0; i<tab_count; i++){
			$('#piggy-tabs').tabs( "remove" , 0 )
		    }
		}
            })
            .appendTo($topbar);

	// tabs init with a custom tab template and an "add" callback filling in the content
	$($piggyTabs).tabs({
	    tabTemplate: "<li><a href='#{href}'>#{label}<span class='unsaved'> *</span></a><span class='ui-icon ui-icon-closethick'>Remove Tab</span></li>"
	});

	// close icon: removing the tab on click
	$( 'span.ui-icon-closethick', $('#piggy-tabs') ).live( 'click', function() {
	    var liindex = $('li', $('#piggy-tabs')).index( $( this ).parent() );
	    if(liindex !== -1){ //ui problem
		var $a = $('a:eq('+liindex+')', $('ul.ui-tabs-nav', $('#piggy-tabs')));
		if($('span.unsaved-active', $($a)).length > 0){
		    //unsaved, confirm
		    $(get_confirm_box('Save changes to Piggy before closing?')).dialog({
			modal: true,
			draggable: false,
			width: '400px',
			resizable: false,
			title: 'Save Confirmation',
			buttons: {
			    'Save': function() {
				var dbox = this;
				var piggyRef = $($a).attr('href');
				//console.log(piggyRef);
				if(piggyRef !== -1){
				    var editor = $(piggyRef).data('editorRef');
				    processSavePiggy(piggyRef.substring(5),
						   editor.getValue(),
						   function(responseJson){
						       $('#piggy-tabs').tabs('remove', liindex);
						       logMessage('info', 'Piggy ' + that.dpProject.piggys[piggyRef.substring(5)].pig_name + ' has been saved.');
						   },
						   function(jqXHR, textStatus, errorThrown){
						   }
						  );
				}
				$( dbox ).dialog( "destroy" );
				$('#dialog-box').remove();
			    },
			    'Cancel': function() {
				$( this ).dialog( "destroy" );
				$('#dialog-box').remove();
			    },
			    'Discard': function() {
				$('#piggy-tabs').tabs('remove', liindex);
				$( this ).dialog( "destroy" );
				$('#dialog-box').remove();
			    }
			}
		    });
		}
		else {
		    $('#piggy-tabs').tabs('remove', liindex);
		}
	    }
	});

	$.each(defaults.snippet, function(i){
	    var tr = $('<tr/>').appendTo($table1).data('info', {'id': i, 'type': 'snippet'}),
	    popS = $('<a/>').attr('href', '#').attr('value', i).text(i.toUpperCase()).click(function(){
		showPopupBox(this, 'snippet', $(this).val());
		return false;
	    });
	    $('<td/>').append(popS).appendTo(tr);
	});


	$.each(that.dpProject.relations, function(i, relObj){
	    var tr = $('<tr/>').appendTo($table2).data('info', {'id': i, 'type': 'relation'}),
	    popR = $('<a/>').attr('href', '#').attr('value', i).text(i).click(function(){
		showPopupBox(this, 'relation', $(this).val());
		return false;
	    });
	    $('<td/>').append(popR).appendTo(tr);
	});

	$.each(that.dpProject.glas, function(i, glaObj){
	    var tr = $('<tr/>').appendTo($table3).data('info', {'id': i, 'type': 'gla'}),
	    popG = $('<a/>').attr('href', '#').attr('value', i).text(glaObj.gla_name).click(function(){
		showPopupBox(this, 'gla', $(this).val());
		return false;
	    });
	    $('<td/>').append(popG).appendTo(tr);
	});

	for(var i=0; i< 6; i++){
	    var tr = $('<tr/>').appendTo($table4);
	    $('<td/>').text('Hello'+i).appendTo(tr);
	}

        $('tr', $div ).draggable({
            appendTo: "body",
            helper: "clone",
            cursor: "move"
        })
	    .swipe({swipeLeft: function() {
		var index = $('#piggy-tabs').tabs('option', 'selected'),
		$a = $('a:eq('+index+')', $('ul.ui-tabs-nav', $('#piggy-tabs')));
		if($($a).length > 0){
		    piggyRef = $($a).attr('href');
		    if(piggyRef !== -1){
			var editor = $(piggyRef).data('editorRef'),
			currLine = editor.getCursor().line,
			id = $(this).data('info').id,
			type = $(this).data('info').type;
			setUnsavedPiggySpan(true, 'current');
			
			if(type==='snippet'){
			    editor.setLine(currLine, defaults.snippet[id]+'\n');
			    editor.setCursor(currLine+1, 0);
			}
			else if(type==='relation'){
			    editor.setLine(currLine, id +' = LOAD ' + id +'\n');
			    editor.setCursor(currLine+1, 0);
			}
			else if(type==='gla'){
			    $.ajax ({
				url: defaults.getGLAContentServlet,
				type: 'POST',
				data: {gla_id: id},
				success: function (response) {
				    editor.setLine(currLine, response+'\n');
				    editor.setCursor(currLine+1, 0);
				},
				error: function (jqXHR, textStatus, errorThrown) {
				    console.log(jqXHR);
				    logMessage('error', $.parseJSON(jqXHR.responseText).datapath_error.error_desc);
				}
			    });
			}
		    }
		}
            },
		    threshold:200,
		    triggerOnTouchEnd:false
		   });
	
        $($piggyTabs ).droppable({
            drop: function( event, ui ) {
		var index = $('#piggy-tabs').tabs('option', 'selected'),
		$a = $('a:eq('+index+')', $('ul.ui-tabs-nav', $('#piggy-tabs')));
		if($($a).length > 0){
		    piggyRef = $($a).attr('href');
		    if(piggyRef !== -1){
			var editor = $(piggyRef).data('editorRef'),
			currLine = editor.getCursor().line,
			id = $(ui.draggable).data('info').id,
			type = $(ui.draggable).data('info').type;
			setUnsavedPiggySpan(true, 'current');

			if(type==='snippet'){
			    editor.setLine(currLine, defaults.snippet[id]+'\n');
			    editor.setCursor(currLine+1, 0);
			}
			else if(type==='relation'){
			    editor.setLine(currLine, id +' = LOAD ' + id +'\n');
			    editor.setCursor(currLine+1, 0);
			}
			else if(type==='gla'){
			    $.ajax ({
				url: defaults.getGLAContentServlet,
				type: 'POST',
				data: {gla_id: id},
				success: function (response) {
				    editor.setLine(currLine, response+'\n');
				    editor.setCursor(currLine+1, 0);
				},
				error: function (jqXHR, textStatus, errorThrown) {
				    console.log(jqXHR);
				    logMessage('error', $.parseJSON(jqXHR.responseText).datapath_error.error_desc);
				}
			    });
			}
		    }
		}
            }
        });

        $bottombar1.appendTo ($piggyList);
        $piggyList.appendTo ($piggyEditor);
	$piggyTabs.appendTo ($piggyBox);
        $bottombar2.appendTo ($piggyBox);
        $piggyBox.appendTo ($piggyEditor);
	$glaDragList.appendTo ($piggyEditor);
        $piggyEditor.appendTo('#' + defaults.contentID);

        $('#gla-accordion').accordion({
            collapsible: true,
            fillSpace: true
        });
    };

    var insert_newpiggy_tab = function (pigID, pigContent){
	var $pigText = $('<div/>').attr ('id', 'pig-'+pigID).appendTo($('#piggy-tabs')),
	$pigTextArea = $('<textarea/>').attr ('id', 'pigTextArea-'+pigID).appendTo($($pigText)),
	editor, foldFunc, hlLine;

	foldFunc = CodeMirror.newFoldFunction(CodeMirror.braceRangeFinder);
	editor = CodeMirror.fromTextArea(document.getElementById('pigTextArea-'+pigID), {
            lineNumbers: true,
	    lineWrapping: true,
            matchBrackets: true,
	    onGutterClick: foldFunc,
	    extraKeys: {"Ctrl-Q": function(cm){foldFunc(cm, cm.getCursor().line);}},
            mode: "text/x-c++src",
	    onCursorActivity: function() {
		editor.setLineClass(hlLine, null, null);
		hlLine = editor.setLineClass(editor.getCursor().line, null, "activeline");
	    },
	    onKeyEvent: function(){
		setUnsavedPiggySpan(true, 'current');
	    }
	});
	hlLine = editor.setLineClass(0, "activeline");
	editor.setValue(pigContent);
	CodeMirror.commands['selectAll'](editor);
	editor.autoFormatRange(editor.getCursor(true), editor.getCursor(false));
	$($pigText).data('editorRef', editor);
    };

    var get_newpiggy_box = function (){
	$('#dialog-box').remove();
        var $box = $('<div/>').attr('id', 'dialog-box'),
        $form = $('<form/>').submit(function(){
            return false;
        }).appendTo ($box),
	$hoverinfo = $('<div>').addClass('hover-info'),
        $table = $('<table/>').addClass('dialog-box-table'),
        $tr1 = $('<tr/>').appendTo($table).addClass('dialog-box-table-tr'), $input1,
        $tr2 = $('<tr/>').appendTo($table).addClass('dialog-box-table-tr'),$input2;

        $('<td/>').addClass('dialog-box-table-td').text('Piggy Name').appendTo ($tr1);
        $input1 = $('<input/>').attr ('placeholder', 'Enter Piggy Name')
            .attr ('type', 'text')
            .attr ('name', 'pigName')
            .attr ('id', 'pigName')
	    .hover(function(){$($hoverinfo).text('A valid Piggy Name e.g. AveragePiggy, TopKPiggy');}, function(){$($hoverinfo).text('')});
        $('<td/>').addClass('dialog-box-table-td').append ($input1).appendTo ($tr1);

        $('<td/>').addClass('dialog-box-table-td').text('Piggy Desc').appendTo ($tr2);
        $input2 = $('<input/>').attr ('placeholder', 'Enter Piggy Description')
            .attr ('type', 'text')
            .attr ('name', 'pigDesc')
            .attr ('id', 'pigDesc')
	    .hover(function(){$($hoverinfo).text('An optional Piggy description.');}, function(){$($hoverinfo).text('')});
        $('<td/>').addClass('dialog-box-table-td').append ($input2).appendTo ($tr2);

	$table.appendTo ($form);
	$hoverinfo.appendTo ($box);
        $box.input = function() {
            return $form.serialize();
        };
        return $box;
    };

    var showChartDiv = function () {
        var $chartPanel = $('<div/>').attr ('id', 'chart-panel').addClass('full-panel'),
        $topbar = $('<div/>').addClass ('top-bar'),
	$chartList = $('<div/>').attr ('id', 'chart-list').addClass('left-panel'),
//        $resultbox = $('<div/>').attr('id', 'chart-result-box').addClass('result-box'),
        $a, $li, too,

	$div = $('<div/>').attr ('id', 'chart-accordion').appendTo($chartList),
	$h31 = $('<h2>').append($('<a/>').attr('href', '#').text('Graphs')), $div1 = $('<div/>'), $table1 = $('<table/>'),
	$h32 = $('<h2>').append($('<a/>').attr('href', '#').text('Gauges')), $div2 = $('<div/>'), $table2 = $('<table/>'),
	$chartBox = $('<div/>').addClass('center-panel center-panel-chart'),
	$graphs = $('<div/>').attr ('id', 'graph-box').addClass('result-box result-box-reg-top').css('overflow', 'auto').appendTo($chartBox),
        $gaugebar = $('<div/>').attr ('id', 'gauge-box').addClass ('gauge-bar').appendTo($chartBox);

	$h31.appendTo($div);
	$div1.appendTo($div);
	$table1.appendTo($div1);
	$h32.appendTo($div);
	$div2.appendTo($div);
	$table2.appendTo($div2);

        $('<h2>').text ('DataPath Charts').appendTo($chartPanel);
        $topbar.appendTo($chartPanel);
//        $('<p/>').text ('Chart Options').appendTo ($chartList);

	var $tbody1 = $('<tbody/>').appendTo ($table1);
        $.each(that.dpProject.charts.chart_list.graphs, function(id) {
	    var $tr = $('<tr/>').data('info', {'chart_type': that.dpProject.charts.chart_list.graphs[id].type, 'chart_name': that.dpProject.charts.chart_list.graphs[id].name}),
	    $inputCB = $('<input/>').attr ('type', 'checkbox')
		.prop ('checked', false)
		.click ( function(){
		    var mytr = $(this).parent().parent(),
		    cType = $(mytr).data('info').chart_type,
		    cName = $(mytr).data('info').chart_name;
		    
		    $(mytr).toggleClass('selected');
		    if($(mytr).hasClass('selected')){
			handleGraphChart(cType, cName, 'add');
		    }
		    else {
			handleGraphChart(cType, cName, 'remove');
		    }
		});
	    $('<td/>').append ($inputCB).addClass ('check').appendTo ($tr);
	    $('<td/>').text(that.dpProject.charts.chart_list.graphs[id].name).appendTo($tr);
	    $($tr).click(function(event) {
	    });
	    $tr.appendTo($tbody1);

	});

	var $tbody2 = $('<tbody/>').appendTo ($table2);
        $.each(that.dpProject.charts.chart_list.gauges, function(id) {
	    var $tr = $('<tr/>').data('info', {'chart_type': that.dpProject.charts.chart_list.gauges[id].type, 'chart_name': that.dpProject.charts.chart_list.gauges[id].name}),
	    $inputCB = $('<input/>').attr ('type', 'checkbox')
		.prop ('checked', false)
		.click ( function(){
		    var mytr = $(this).parent().parent(),
		    cType = $(mytr).data('info').chart_type,
		    cName = $(mytr).data('info').chart_name;
		    
		    $(mytr).toggleClass('selected');
		    if($(mytr).hasClass('selected')){
			handleGaugeChart(cType, cName, 'add');
		    }
		    else {
			handleGaugeChart(cType, cName, 'remove');
		    }

		});
	    $('<td/>').append ($inputCB).addClass ('check').appendTo ($tr);

	    $('<td/>').text(that.dpProject.charts.chart_list.gauges[id].name).appendTo($tr);
	    $($tr).click(function(event) {
	    });

	    $tr.appendTo($tbody2);

	});

        $('<button/>').attr('id', 'pauseChart')
            .text('Pause Charts')
            .attr ('title', 'Pause/Play all charts')
            .click(function(){
/*		if($(this).text() === 'Pause Charts'){
		    that.dpProject.renderCharts = false;
		    $(this).text('Play Charts');
		}
		else {
		    that.dpProject.renderCharts = true;
		    $(this).text('Pause Charts');
		}
*/
		var str = 'Window Width: ' + $('body').width() + ' Chart Box width: ' + $($chartBox).width() + ' Graphs width: ' + $($graphs).width();
		alert(str);
            })
            .appendTo($topbar);

//        $table.appendTo($resultbox);
//        $resultbox.appendTo($chartList);
        $chartList.appendTo ($chartPanel);
        $chartBox.appendTo ($chartPanel);
        $chartPanel.appendTo('#' + defaults.contentID);

        $('#chart-accordion').accordion({
            collapsible: true,
            fillSpace: true
        });
    };

    var getChartList = function(){
	var result  = {},
	graphs = [],
	gauges = [];
	graphs.push({type: 'cpu', name: 'CPU Usage'});
	graphs.push({type: 'memory', name: 'Memory Usage'});
	graphs.push({type: 'disk', name: 'Disk Usage'});
	graphs.push({type: 'nw', name: 'Network Usage'});
	gauges.push({type: 'io', name: 'IO Rate'});
	gauges.push({type: 'chunk', name: 'Chunk Rate'});

	result['graphs'] = graphs;
	result['gauges'] = gauges;
	return result;
    };

    var getConfigList = function(){
	var Obj = [];
	Obj.push({type: 'constants', name: 'DataPath Constants'});
	Obj.push({type: 'ui', name: 'UI Config'});
	return Obj;
    };

    var getResultList = function(){
	var Obj = [];
	Obj.push({type: 'table', name: 'Table [Default]'});
	Obj.push({type: 'column', name: 'Column Chart'});
	Obj.push({type: 'line', name: 'Line Chart'});
	Obj.push({type: 'pie', name: 'Pie Chart'});
	Obj.push({type: 'scatter', name: 'Scatter Chart'});
	return Obj;
    };

    var drawOutputTable = function(queryName, content) {
	var tableData = new google.visualization.DataTable(),
	tableVis, i, tab_count;
	for(i=0; i< content.col_count; i++){
	    tableData.addColumn(content.columns[i].col_type, content.columns[i].col_name, content.columns[i].col_name);
	}

	tableData.addRows(content.rows);
	$('#rez-panel').data('result', {'data': tableData, 'query_name': queryName});
	
	$('#rez-chart-table').empty();
	$('span', $('#rez-table')).text(queryName);
	tab_count = $('#rez-tabs').tabs('length');
	for (i=1; i< tab_count; i++){
	    $('#rez-tabs').tabs( "remove" , 1 );
	}
	
	tableVis = new google.visualization.Table(document.getElementById('rez-chart-table'));
        tableVis.draw(tableData, {showRowNumber: true});	
    };

    var drawOutputColumnChart = function(title, xa, ya, cols) {
	var colViewData = new google.visualization.DataView($('#rez-panel').data('result').data),
	queryName = $('#rez-panel').data('result').query_name,
	colVis;
	$('#rez-chart-column').empty();
	$('span', $('#rez-column')).text(queryName);
	
	colViewData.setColumns(cols);
	colVis = new google.visualization.ColumnChart(document.getElementById('rez-chart-column'));
        colVis.draw(colViewData, {
            title: title,
	    height: 400,
	    hAxis: {
		title: xa
	    },
	    vAxis: {
		title: ya
	    }
	});	
    };

    var drawOutputLineChart = function(title, xa, ya, cols) {
	var lineViewData = new google.visualization.DataView($('#rez-panel').data('result').data),
	queryName = $('#rez-panel').data('result').query_name,
	lineVis;
	$('#rez-chart-line').empty();
	$('span', $('#rez-line')).text(queryName);
	
	lineViewData.setColumns(cols);
	lineVis = new google.visualization.LineChart(document.getElementById('rez-chart-line'));
        lineVis.draw(lineViewData, {
            title: title,
	    lineWidth: 1,
	    height: 400,
	    hAxis: {
		title: xa
	    },
	    vAxis: {
		title: ya
	    }
	});	
    };

    var drawOutputPieChart = function(title, cols) {
	var pieViewData = new google.visualization.DataView($('#rez-panel').data('result').data),
	queryName = $('#rez-panel').data('result').query_name,
	pieVis;
	$('#rez-chart-pie').empty();
	$('span', $('#rez-pie')).text(queryName);
	
	pieViewData.setColumns(cols);
	pieVis = new google.visualization.PieChart(document.getElementById('rez-chart-pie'));
        pieVis.draw(pieViewData, {
            title: title,
	    height: 400
	});	
    };

    var drawOutputScatterChart = function(title, xa, ya, cols) {
	var sctViewData = new google.visualization.DataView($('#rez-panel').data('result').data),
	queryName = $('#rez-panel').data('result').query_name,
	sctVis;
	$('#rez-chart-scatter').empty();
	$('span', $('#rez-scatter')).text(queryName);
	
	sctViewData.setColumns(cols);
	sctVis = new google.visualization.ScatterChart(document.getElementById('rez-chart-scatter'));
        sctVis.draw(sctViewData, {
            title: title,
	    height: 400,
	    hAxis: {
		title: xa
	    },
	    vAxis: {
		title: ya
	    }
	});	
    };

    var getColumnInfo = function(type) {
	var rezData = $('#rez-panel').data('result').data,
	count = rezData.getNumberOfColumns(), 
	result = {}, 
	xCols = {},
	yCols = {},
	i;
	
	if(type==='line'){
	    for(i=0;i<count; i++){
		xCols[i] = rezData.getColumnLabel(i);
		if(rezData.getColumnType(i) === 'number'){
		    yCols[i] = rezData.getColumnLabel(i);
		}
	    }
	}
	else if(type==='column' || type === 'pie'){
	    for(i=0;i<count; i++){
		if(rezData.getColumnType(i) === 'string' || rezData.getColumnType(i).indexOf('date') > -1 || rezData.getColumnType(i).indexOf('time') > -1){
		    xCols[i] = rezData.getColumnLabel(i);
		}
		else {
		    yCols[i] = rezData.getColumnLabel(i);
		}
	    }
	}
	else if(type==='scatter'){
	    for(i=0;i<count; i++){
		if(rezData.getColumnType(i) === 'number'){
		    xCols[i] = rezData.getColumnLabel(i);
		    yCols[i] = rezData.getColumnLabel(i);
		}
	    }
	}
	result['x_cols'] = xCols;
	result['y_cols'] = yCols;
	//console.log(result);
	return result;
    };


    var get_newchart_box = function (type){
	$('#dialog-box').remove();
        var $box = $('<div/>').attr('id', 'dialog-box'),
	cols = getColumnInfo(type),
        $form = $('<form/>').submit(function(){
            return false;
        }).appendTo ($box),
	$hoverinfo = $('<div>').addClass('hover-info'),
        $table = $('<table/>').addClass('dialog-box-table'),
        $tr1 = $('<tr/>').appendTo($table).addClass('dialog-box-table-tr'), $input1,
        $tr3 = $('<tr/>').appendTo($table).addClass('dialog-box-table-tr'), $input31, $input32,
        $tr2 = $('<tr/>').appendTo($table).addClass('dialog-box-table-tr'),$select2,

	$axesBox = $('<div>').addClass('io-box io-box-extra'),
	$axesBtns= $('<ul>'),
	$axesTable = $('<table/>').attr('id', 'chartAxes').addClass('io-box-table io-box-table-extra').appendTo($axesBox),
	$addBtn = $('<button/>').attr('id', 'addBtn').text('Add Y Axis')
	    .click(function(){
		var $tr = $('<tr/>').addClass('io-box-tr').click(function(){
		    $('tr', $axesBox).removeClass('selected');
		    $(this).addClass('selected');
		}),
		$select;
		$('<td/>').text ('Line ' + $('tr', $axesBox).length).appendTo ($tr);

		$select=$('<select/>');

		$.each(cols.y_cols, function(colID) {
		    $('<option/>').attr('value', colID).text(cols.y_cols[colID]).appendTo ($select);
		});

		$('<td/>').append ($select).appendTo ($tr);
		$tr.appendTo($($axesTable));
	    }),

	$deleteBtn = $('<button/>').attr('id', 'deleteBtn').text('Delete Y Axis')
	    .click(function(){
		$.each($('tr.selected', $axesBox), function(){
		    if($(this).index() > 0){ //can not remove X Axis
			$(this).fadeOut(function(){
			    $(this).remove();
			});
		    }
		});
	    }),
	$moveUpBtn = $('<button/>').attr('id', 'moveUpBtn').text('Move Up')
	    .click(function(){
		var selected = $('tr.selected', $axesBox);
		selected.insertBefore(selected.prev());
	    }),
	$moveDownBtn = $('<button/>').attr('id', 'moveDownBtn').text('Move Down')
	    .click(function(){
		var selected = $('tr.selected', $axesBox);
		selected.insertAfter(selected.next());
	    });

	var $deftr = $('<tr/>').addClass('io-box-tr').click(function(){
	    $('tr', $axesBox).removeClass('selected');
	    $(this).addClass('selected');
	}),
	$defselect;
	$('<td/>').text ('X Axis').appendTo ($deftr);
	
	$defselect=$('<select/>');
	
	$.each(cols.x_cols, function(colID) {
	    $('<option/>').attr('value', colID).text(cols.x_cols[colID]).appendTo ($defselect);
	});
	
	$('<td/>').append ($defselect).appendTo ($deftr);
	$deftr.appendTo($($axesTable));
	
	$('<li/>').append($addBtn).appendTo($axesBtns);
	$('<li/>').append($deleteBtn).appendTo($axesBtns);
//	$('<li/>').append($moveUpBtn).appendTo($axesBtns);
//	$('<li/>').append($moveDownBtn).appendTo($axesBtns);

        $('<td/>').addClass('dialog-box-table-td').text('Chart Display Name').appendTo ($tr1);
        $input1 = $('<input/>').attr ('placeholder', 'Enter Chart Name')
            .attr ('type', 'text')
            .attr ('name', 'chartName')
            .attr ('id', 'chartName')
	    .hover(function(){$($hoverinfo).text('An optional chart name to display on top of the chart.');}, function(){$($hoverinfo).text('')});
        $('<td/>').addClass('dialog-box-table-td').append ($input1).appendTo ($tr1);

        $('<td/>').addClass('dialog-box-table-td').text('Axes Titles').appendTo ($tr3);
        $input31 = $('<input/>').attr ('placeholder', 'X Axis Title')
            .attr ('type', 'text')
            .attr ('name', 'xAxisTitle')
            .attr ('id', 'xAxisTitle')
	    .hover(function(){$($hoverinfo).text('An optional title for X axis of this chart.');}, function(){$($hoverinfo).text('')});
        $input32 = $('<input/>').attr ('placeholder', 'Y Axis Title')
            .attr ('type', 'text')
            .attr ('name', 'yAxisTitle')
            .attr ('id', 'yAxisTitle')
	    .hover(function(){$($hoverinfo).text('An optional title for Y axis of this chart.');}, function(){$($hoverinfo).text('')});

        $('<td/>').addClass('dialog-box-table-td').append ($input31).append ($input32).appendTo ($tr3);

	$('<td/>').addClass('dialog-box-table-td').text('Chart Axes').appendTo($tr2);
	$('<td/>').addClass('dialog-box-table-td').append($axesBox).append($axesBtns).appendTo($tr2);

	$table.appendTo ($form);
	$hoverinfo.appendTo ($box);
        $box.input = function() {
            return $form.serialize();
        };
        return $box;
    };

    var get_newpiechart_box = function (type){
	$('#dialog-box').remove();
        var $box = $('<div/>').attr('id', 'dialog-box'),
	cols = getColumnInfo(type),
        $form = $('<form/>').submit(function(){
            return false;
        }).appendTo ($box),
	$hoverinfo = $('<div>').addClass('hover-info'),
        $table = $('<table/>').addClass('dialog-box-table'),
        $tr1 = $('<tr/>').appendTo($table).addClass('dialog-box-table-tr'), $input1,
        $tr2 = $('<tr/>').appendTo($table).addClass('dialog-box-table-tr'),$select2,

	$axesBox = $('<div>').addClass('io-box io-box-extra'),
	$axesTable = $('<table/>').attr('id', 'chartAxes').addClass('io-box-table io-box-table-extra').appendTo($axesBox);

	var $deftr = $('<tr/>').addClass('io-box-tr').click(function(){
	    $('tr', $axesBox).removeClass('selected');
	    $(this).addClass('selected');
	}),
	$defselect,
	$deftr2 = $('<tr/>').addClass('io-box-tr').click(function(){
	    $('tr', $axesBox).removeClass('selected');
	    $(this).addClass('selected');
	}),
	$defselect2;

	$('<td/>').text ('Slice labels').appendTo ($deftr);
	
	$defselect=$('<select/>');
	
	$.each(cols.x_cols, function(colID) {
	    $('<option/>').attr('value', colID).text(cols.x_cols[colID]).appendTo ($defselect);
	});
	
	$('<td/>').append ($defselect).appendTo ($deftr);
	$deftr.appendTo($($axesTable));

	$('<td/>').text ('Slice values').appendTo ($deftr2);
	
	$defselect2=$('<select/>');
	
	$.each(cols.y_cols, function(colID) {
	    $('<option/>').attr('value', colID).text(cols.y_cols[colID]).appendTo ($defselect2);
	});
	
	$('<td/>').append ($defselect2).appendTo ($deftr2);
	$deftr2.appendTo($($axesTable));
	
        $('<td/>').addClass('dialog-box-table-td').text('Chart Display Name').appendTo ($tr1);
        $input1 = $('<input/>').attr ('placeholder', 'Enter Chart Name')
            .attr ('type', 'text')
            .attr ('name', 'chartName')
            .attr ('id', 'chartName')
	    .hover(function(){$($hoverinfo).text('An optional chart name to display on top of the chart.');}, function(){$($hoverinfo).text('')});
        $('<td/>').addClass('dialog-box-table-td').append ($input1).appendTo ($tr1);

	$('<td/>').addClass('dialog-box-table-td').text('Chart Axes').appendTo($tr2);
	$('<td/>').addClass('dialog-box-table-td').append($axesBox).appendTo($tr2);

	$table.appendTo ($form);
	$hoverinfo.appendTo ($box);
        $box.input = function() {
            return $form.serialize();
        };
        return $box;
    };

    var redrawCharts = function() {
	$.each(that.dpProject.charts.chart_vis.graphs, function(id, gObj){
	    gObj.draw(that.dpProject.charts.chart_data.graphs[id], defaults.chartOptions.line);
	});
	
	$.each(that.dpProject.charts.chart_vis.gauges, function(id, gObj){
	    gObj.draw(that.dpProject.charts.chart_data.gauges[id], defaults.chartOptions.gauge);
	});
    };

    var runStatisticsThread = function() {
	    $.ajax ({
		url: defaults.getChartDataServlet,
		dataType: 'json',
		success: function (response) {
		    //console.log(response);
		    $.each(that.dpProject.charts.chart_data.graphs, function(id, gObj){
			gObj.addRow(response.graphs[id]);
			if(gObj.getNumberOfRows() > defaults.chartOptions.line.hAxis.gridlines.count)
			    gObj.removeRow(0);
		    });
		    
		    $.each(that.dpProject.charts.chart_data.gauges, function(id, gObj){
			if(gObj.getNumberOfRows() > 0)
			{
			    gObj.removeRow(0);
			}
			gObj.addRow(response.gauges[id]);
		    });
		    
		    if(that.dpProject.renderCharts === true){
			redrawCharts();
		    }
		    
		    defaults.timer = setTimeout(function(){
			return runStatisticsThread();
		    }, defaults.statsInterval);
		},
		error: function (jqXHR, textStatus, errorThrown) {
		    console.log(jqXHR);
		    if(that.dpProject.renderCharts === true){
			redrawCharts();
		    }
		    defaults.timer = setTimeout(function(){
			return runStatisticsThread();
		    }, defaults.statsInterval);
		}
	    });
    };

    var handleGraphChart = function(ctype, cname, cmd){
	if(cmd === 'add'){
	    var $box = $('<div/>').attr ('id', ctype+'-graph-chart').css({'height': '200px', 'width': '100%'});
	    
            $('<p>').attr ('id', ctype+'-graph-title').text (cname).appendTo($('#graph-box'));
	    $box.appendTo($('#graph-box'));
	    
	    var dataVis = new google.visualization.LineChart(document.getElementById(ctype+'-graph-chart'));
	    dataVis.draw(that.dpProject.charts.chart_data.graphs[ctype], defaults.chartOptions.line);
	    that.dpProject.charts.chart_vis.graphs[ctype] = dataVis;
	}
	else if(cmd === 'remove'){
	    $('#'+ctype+'-graph-chart').remove();
	    $('#'+ctype+'-graph-title').remove();
	    delete that.dpProject.charts.chart_vis.graphs[ctype];
	}
    };

    var handleGaugeChart = function(ctype, cname, cmd){
	if(cmd === 'add'){
	    var $box = $('<div/>').attr ('id', ctype+'-gauge-chart').css({'height': '120px', 'width': '120px', 'float': 'left', 'display': 'inline'});
	    $box.appendTo($('#gauge-box'));
	    
	    var dataVis = new google.visualization.Gauge(document.getElementById(ctype+'-gauge-chart'));
	    dataVis.draw(that.dpProject.charts.chart_data.gauges[ctype], defaults.chartOptions.gauge);
	    that.dpProject.charts.chart_vis.gauges[ctype] = dataVis;
	}
	else if(cmd === 'remove'){
	    $('#'+ctype+'-gauge-chart').remove();
	    delete that.dpProject.charts.chart_vis.gauges[ctype];
	}
    };

    var showDPConstants = function(){
	var $thtable = $('<table/>').addClass('conf-table').appendTo('#conf-box'),
        $resultbox = $('<div/>').attr('id', 'constants-result-box').addClass('result-box result-box-extra-top').css('overflow', 'auto').appendTo($('#conf-box')),
        $table = $('<table/>').addClass('conf-table').appendTo($resultbox),
        $thtr = $('<tr/>').appendTo($thtable),
	$bottombar = $('<div/>').addClass ('bottom-bar').appendTo($('#conf-box')),
	$inputDef = $('<input/>').attr ('type', 'checkbox')
	    .attr ('id', 'setDefault')
            .attr ('title', 'Set All Properties to Default Values')
            .prop ('checked', true)
            .click ( function(){
		$('#setUserDef').prop('checked', false);
		$(':radio', $('td.dp-td2', $table)).trigger('click');
            }),
	$inputUser = $('<input/>').attr ('type', 'checkbox')
	    .attr ('id', 'setUserDef')
            .attr ('title', 'Set All Properties to User Defined Values')
            .prop ('checked', false)
            .click ( function(){
		$('#setDefault').prop('checked', false);
		$(':radio', $('td.dp-td3', $table)).trigger('click');
            });

        $('<th/>').addClass('dp-th1').text('Property').appendTo($thtr);
        $('<th/>').addClass('dp-th2').append($inputDef).append($('<span/>').text('Default Values')).appendTo($thtr);
        $('<th/>').addClass('dp-th3').append($inputUser).append($('<span/>').text('User Defined Values')).appendTo($thtr);
        $('<th/>').addClass('dp-th4').text('Description').appendTo($thtr);

	for(var i=0; i< 10; i++){
	    var tr = $('<tr/>').appendTo($table),
	    $radio1label = $('<label for="defProp_'+i+'" />')
		.text ('Property Name' +i),

            $radio1 = $('<input/>').attr ('type', 'radio')
		.attr ('title', 'Set this property to it\'s default value')
		.attr ('name', 'prop_'+i)
		.attr ('id', 'defProp_'+i)
		.attr ('value', 'default')
		.attr ('checked', true)
		.click(function(){
		    $('#setUserDef').prop('checked', false);
		    $(':text', $(this).parent().parent()).attr('disabled', 'disabled').removeAttr('placeholder');
		}),

            $inputU = $('<input/>')
		.attr ('type', 'text')
		.attr ('name', 'udTBProp_'+i)
		.attr ('id', 'udTBProp_'+i)
		.attr ('size', '20')
		.attr('disabled', 'disabled')
		.click (function() {
		}),

            $radio2 = $('<input/>').attr ('type', 'radio')
		.attr ('title', 'Set this property to your value')
		.attr ('name', 'prop_'+i)
		.attr ('id', 'udProp_'+i)
		.attr ('value', 'default')
		.attr ('checked', false)
		.click(function(){
		    $('#setDefault').prop('checked', false);
		    $(':text', $(this).parent().parent()).removeAttr('disabled').attr ('placeholder', 'Enter Property Value')
		});

            $('<td/>').addClass('dp-td1').text('Property' + i).appendTo(tr);
            $('<td/>').addClass('dp-td2').append($radio1).append($radio1label).appendTo(tr);
            $('<td/>').addClass('dp-td3').append($radio2).append($inputU).appendTo(tr);
            $('<td/>').addClass('dp-td4').text('Description').appendTo(tr);
	}
    };

    var showResultDiv = function () {
        var $rezPanel = $('<div/>').attr ('id', 'rez-panel').addClass('full-panel'),
        $topbar = $('<div/>').addClass ('top-bar'),
	$rezList = $('<div/>').attr ('id', 'rez-list').addClass('left-panel'),
        $resultbox = $('<div/>').attr('id', 'rez-result-box').addClass('result-box'),
        $table = $('<table/>'),
        $a, $li, too, $select=$('<select/>'),

	$rezBox = $('<div/>').attr ('id', 'rez-box').addClass('center-panel center-panel-rez'),
	$rezTabs = $('<div/>').attr ('id', 'rez-tabs').addClass('result-box result-box-reg-top result-box-z-bottom'),
	$ulreztabs = $('<ul/>').appendTo ($rezTabs);

        $('<h2>').text ('Query Results').appendTo($rezPanel);
        $topbar.appendTo ($rezPanel);
        $('<p/>').text ('Output Display Formats').appendTo ($rezList);

	var $tbody = $('<tbody/>').appendTo ($table),
	rlist = getResultList();
        $.each(rlist, function(id) {
	    var $tr = $('<tr/>').data('info', {'rez_type': rlist[id].type});
	    $('<td/>').text(rlist[id].name).appendTo($tr);

	    $($tr).click(function(event) {
		selectRow(event, this, $('#rez-result-box'));
		var rezT = $(this).data('info').rez_type;
		if($('#rez-'+rezT).length > 0){
		    $('#rez-tabs').tabs( 'select' , '#rez-'+rezT);
		}
		else{
		    if(rezT==='table'){
			insert_newrez_tab(rezT, '',  'Table Result');
			$('#rez-tabs').tabs( 'add', '#rez-'+rezT, 'Output Table');
			$('#rez-tabs').tabs( 'select' , '#rez-'+rezT);
		    }
		    else if(rezT==='column'){
			$(get_newchart_box(rezT)).dialog({
			    modal: true,
			    draggable: true,
			    width: '650px',
			    resizable: false,
			    title: 'Column Chart Information',
			    buttons: {
				'Display': function() {
				    var chartname, xaxis, yaxis, chartCols, dbox = this;
				    chartname = $.trim($('#chartName', this).val());
				    xaxis = $.trim($('#xAxisTitle', this).val());
				    yaxis = $.trim($('#yAxisTitle', this).val());
				    chartCols = $('tr', $('#chartAxes')).map(function() {
					return parseInt($('select', this).val(), 10);
				    }).get();
				    insert_newrez_tab(rezT, '', 'Column Chart');
				    $('#rez-tabs').tabs( 'add', '#rez-'+rezT, 'Column Chart');
				    $('#rez-tabs').tabs( 'select' , '#rez-'+rezT);
				    drawOutputColumnChart(chartname, xaxis, yaxis, chartCols);
				    $( dbox ).dialog( "destroy" );
				    $('#dialog-box').remove();
				},
				'Cancel': function() {
				    $( this ).dialog( "destroy" );
				    $('#dialog-box').remove();
				}
			    }
			});
		    }
		    else if(rezT==='line'){
			$(get_newchart_box(rezT)).dialog({
			    modal: true,
			    draggable: true,
			    width: '650px',
			    resizable: false,
			    title: 'Line Chart Information',
			    buttons: {
				'Display': function() {
				    var chartname, xaxis, yaxis, chartCols, dbox = this;
				    chartname = $.trim($('#chartName', this).val());
				    xaxis = $.trim($('#xAxisTitle', this).val());
				    yaxis = $.trim($('#yAxisTitle', this).val());
				    chartCols = $('tr', $('#chartAxes')).map(function() {
					return parseInt($('select', this).val(), 10);
				    }).get();
				    insert_newrez_tab(rezT, '', 'Line Chart');
				    $('#rez-tabs').tabs( 'add', '#rez-'+rezT, 'Line Chart');
				    $('#rez-tabs').tabs( 'select' , '#rez-'+rezT);
				    drawOutputLineChart(chartname, xaxis, yaxis, chartCols);
				    $( dbox ).dialog( "destroy" );
				    $('#dialog-box').remove();
				},
				'Cancel': function() {
				    $( this ).dialog( "destroy" );
				    $('#dialog-box').remove();
				}
			    }
			});
		    }
		    else if(rezT==='pie'){
			$(get_newpiechart_box(rezT)).dialog({
			    modal: true,
			    draggable: true,
			    width: '650px',
			    resizable: false,
			    title: 'Pie Chart Information',
			    buttons: {
				'Display': function() {
				    var chartname, chartCols, dbox = this;
				    chartname = $.trim($('#chartName', this).val());
				    chartCols = $('tr', $('#chartAxes')).map(function() {
					return parseInt($('select', this).val(), 10);
				    }).get();
				    insert_newrez_tab(rezT, '', 'Pie Chart');
				    $('#rez-tabs').tabs( 'add', '#rez-'+rezT, 'Pie Chart');
				    $('#rez-tabs').tabs( 'select' , '#rez-'+rezT);
				    drawOutputPieChart(chartname, chartCols);
				    $( dbox ).dialog( "destroy" );
				    $('#dialog-box').remove();
				},
				'Cancel': function() {
				    $( this ).dialog( "destroy" );
				    $('#dialog-box').remove();
				}
			    }
			});
		    }
		    else if(rezT==='scatter'){
			$(get_newchart_box(rezT)).dialog({
			    modal: true,
			    draggable: true,
			    width: '650px',
			    resizable: false,
			    title: 'Scatter Chart Information',
			    buttons: {
				'Display': function() {
				    var chartname, xaxis, yaxis, chartCols, dbox = this;
				    chartname = $.trim($('#chartName', this).val());
				    xaxis = $.trim($('#xAxisTitle', this).val());
				    yaxis = $.trim($('#yAxisTitle', this).val());
				    chartCols = $('tr', $('#chartAxes')).map(function() {
					return parseInt($('select', this).val(), 10);
				    }).get();
				    insert_newrez_tab(rezT, '', 'Scatter Chart');
				    $('#rez-tabs').tabs( 'add', '#rez-'+rezT, 'Scatter Chart');
				    $('#rez-tabs').tabs( 'select' , '#rez-'+rezT);
				    drawOutputScatterChart(chartname, xaxis, yaxis, chartCols);
				    $( dbox ).dialog( "destroy" );
				    $('#dialog-box').remove();
				},
				'Cancel': function() {
				    $( this ).dialog( "destroy" );
				    $('#dialog-box').remove();
				}
			    }
			});
		    }
		}
	    });

	    $tr.appendTo($tbody);
	});

        $('<button/>').attr('id', 'printOutput')
            .text('Print Output')
            .attr ('title', 'Print this output')
            .click(function(){
		var index = $('#rez-tabs').tabs('option', 'selected'),
		$a = $('a:eq('+index+')', $('ul.ui-tabs-nav', $('#rez-tabs'))),
		rezRef;
		if($($a).length > 0){
		    rezRef = $($a).attr('href');
		    if(rezRef !== -1){
			$(rezRef).printElement({ leaveOpen: true, printMode: 'popup' });			
		    }
		}
            })
            .appendTo($topbar);

        $('<button/>').attr('id', 'exportOutput')
            .text('Export as CSV')
            .attr ('title', 'Export this output in a CSV file')
            .click(function(){

            })
            .appendTo($topbar);


	$('<option/>').attr('value', 'none').text('Select from previous results').appendTo ($select);
	$.each(that.dpProject.rezs, function(rezID) {
	    $('<option/>').attr('value', rezID).text(that.dpProject.rezs[rezID].rez_name + ' | ' + that.dpProject.rezs[rezID].rez_timestamp).appendTo ($select);
	});

        $('<button/>').attr('id', 'loadResults')
	    .css('float', 'right')
            .text('Load')
            .attr ('title', 'Load Previous Results')
            .click(function(){
		var rezID = $($select).val();
		if(rezID !== 'none'){
		    $(get_confirm_box('Current output will be replaced with this result.<br/><br/>Do you want to continue?')).dialog({
		    modal: true,
		    draggable: false,
		    width: '400px',
		    resizable: false,
		    title: 'Load Confirmation',
		    buttons: {
			'Yes': function() {
			    var dbox = this;
			    $.ajax({
				url: defaults.getQueryResultServlet,
				data: {rez_id: rezID},
				dataType: 'json',
				success: function (response) {
				    //console.log(response);
				    drawOutputTable(that.dpProject.rezs[rezID].rez_name + ' | ' + that.dpProject.rezs[rezID].rez_timestamp, response);
				    $( dbox ).dialog( "destroy" );
				    $('#dialog-box').remove();
				},
				error: function (jqXHR, textStatus, errorThrown) {
				    console.log(jqXHR);
				    notify($.parseJSON(jqXHR.responseText).datapath_error.error_desc, 5000, true);
				}
			    });
			},
			'No': function() {
			    $( this ).dialog( "destroy" );
			    $('#dialog-box').remove();
			}
		    }
		});

		}
            })
            .appendTo($topbar);

	$select.appendTo($topbar);

	// tabs init with a custom tab template and an "add" callback filling in the content
	$($rezTabs).tabs({
	    tabTemplate: "<li><a href='#{href}'>#{label}<span class='unsaved'> *</span></a><span class='ui-icon ui-icon-closethick'>Remove Tab</span></li>"
	});

	// close icon: removing the tab on click
	$('span.ui-icon-closethick', $('#rez-tabs') ).live( 'click', function() {
	    var liindex = $('li', $('#rez-tabs')).index( $( this ).parent() );
	    if(liindex > 0){ //ui problem + cannot close Table output
		var $a = $('a:eq('+liindex+')', $('ul.ui-tabs-nav', $('#rez-tabs')));
		//unsaved, confirm
		$(get_confirm_box('Save this output before closing?')).dialog({
		    modal: true,
		    draggable: false,
		    width: '400px',
		    resizable: false,
		    title: 'Save Confirmation',
		    buttons: {
			'Save': function() {
			    var dbox = this;
			    var rezRef = $($a).attr('href');
//			    console.log(rezRef);
			    if(rezRef !== -1){
			    }
			    $( dbox ).dialog( "destroy" );
			    $('#dialog-box').remove();
			},
			'Cancel': function() {
			    $( this ).dialog( "destroy" );
			    $('#dialog-box').remove();
			},
			'Discard': function() {
			    $('#rez-tabs').tabs('remove', liindex);
			    $( this ).dialog( "destroy" );
			    $('#dialog-box').remove();
			}
		    }
		});
	    }
	});

        $table.appendTo($resultbox);
        $resultbox.appendTo($rezList);
        $rezList.appendTo ($rezPanel);
	$rezTabs.appendTo ($rezBox);
        $rezBox.appendTo ($rezPanel);
        $rezPanel.appendTo('#' + defaults.contentID);
	insert_newrez_tab('table', '', 'Table Result');
	$('#rez-tabs').tabs( "add", "#rez-table", 'Output Table');
	$('#rez-tabs').tabs( 'select' , '#rez-table');
    };

    var insert_newrez_tab = function (rezT, queryName, rezContent){
	var $rezText = $('<div/>').attr ('id', 'rez-'+rezT).appendTo($('#rez-tabs'));

	$('<span/>').addClass('query-name').text(queryName).appendTo($rezText);
	$('<div/>').attr ('id', 'rez-chart-'+rezT).css('margin-top', '10px').appendTo($rezText);
	
//	$($rezText).text(rezContent);
    };

    var showConfigDiv = function () {
        var $confPanel = $('<div/>').attr ('id', 'conf-panel').addClass('full-panel'),
        $topbar = $('<div/>').addClass ('top-bar'),
	$confList = $('<div/>').attr ('id', 'conf-list').addClass('left-panel'),
        $resultbox = $('<div/>').attr('id', 'conf-result-box').addClass('result-box'),
        $table = $('<table/>'),
        $a, $li, too,

	$confBox = $('<div/>').attr ('id', 'conf-box').addClass('center-panel center-panel-conf');

        $('<h2>').text ('DataPath Configuration Center').appendTo($confPanel);
        $topbar.appendTo ($confPanel);
        $('<p/>').text ('Config Options').appendTo ($confList);

	var $tbody = $('<tbody/>').appendTo ($table);
        $.each(that.dpProject.configs, function(id) {
	    var $tr = $('<tr/>').data('info', {'conf_type': that.dpProject.configs[id].type});
	    $('<td/>').text(that.dpProject.configs[id].name).appendTo($tr);
	    $($tr).click(function(event) {
		selectRow(event, this, $('#conf-result-box'));
		$('#conf-box').empty();
		var cType = $(this).data('info').conf_type;
		if(cType==='constants'){
		    showDPConstants();
		}
	    });
	    $tr.appendTo($tbody);
	});

        $('<button/>').attr('id', 'saveProps')
            .text('Save Properties')
            .attr ('title', 'Save changes')
            .click(function(){
            })
            .appendTo($topbar);

        $table.appendTo($resultbox);
        $resultbox.appendTo($confList);
        $confList.appendTo ($confPanel);
        $confBox.appendTo ($confPanel);
        $confPanel.appendTo('#' + defaults.contentID);
    };

    var loadGLATemplate = function(glaName, glaType, input, output, prop) {
	var template = defHeader(glaName);
	template+= metaGLAs(glaName, input, output, prop, glaType);
	template+=commonGLA(glaName, input);

	if(glaType==='state'){

	}
	else if(glaType==='single'){
	    template+= singleGLA(output);
	}
	else if(glaType==='multi'){
	    template+= multiGLA(output);
	}
	else if(glaType==='fragment'){
	    template+= fragmentGLA(glaName, output);
	}

	template+='\n}; \n\n#endif //_'+glaName+'_H_';
	return template;
    };

    var selectRow = function(event, tr, container) {
//	if (!event.ctrlKey) {
	    $('tr', container).removeClass('selected');
//	}

	$(tr).addClass('selected');
    };

    var selectRowMulti = function(event, tr, container) {
	if (event.target.type !== 'checkbox') {
	    $(':checkbox', tr).trigger('click');
	}
    };

    var notify = function(msg, timeout, err) {
	//remove any previous notification, if any
	$('#notify-box').remove();

        var $box = $('<div/>').attr('id', 'notify-box').addClass('notification');

	if(err === true){
	    $($box).css({'color': 'red', 'border-color': 'red', 'text-align': 'center' });
	}
	else {
	    $($box).css({'color': '#1B5266', 'border-color': '#9CBDFF', 'text-align': 'center'});
	}

        $box.text(msg);
	$box.appendTo($('body')).slideDown('slow');

	if(timeout > 0){
	    $box.click(function() {
		$(this).remove();
	    });

	    $('#notify-box').mouseenter(function(){
		$(this).stop(true);
	    }).mouseleave(function(){
		$('#notify-box').delay(timeout/2).slideUp(100, function(){
		    $(this).remove();
		});
	    });

	    $('#notify-box').delay(timeout).slideUp(100, function(){
		$(this).remove();
	    });
	}
    };

    //what = 'init' or 'update' or 'exit'
    var progress = function(what, perc, msg) {

	//initialize
	if(what === 'init'){
	    //remove any previous notification, if any
	    $('.notification').remove();

            var $box = $('<div/>').attr('id', 'progress-box').addClass('notification'),
	    $pbar = $('<div/>').css({'margin': '-16px 80px 0px 250px'}),
	    $pMsg = $('<p/>'),
	    $sp = $('<span/>').css({'margin': '-8px 8px 0 0', 'float': 'right'}),
	    $overlay = $('<div/>').addClass('ui-widget-overlay').css('z-index', '1003');

	    $($box).css({'color': '#1B5266', 'border-color': '#9CBDFF', 'text-align': 'left ' });
            $($pMsg).text(msg);
            $($pMsg).appendTo($box);
	    $pbar.appendTo($box);
	    $sp.appendTo($box);
	    $($pbar).progressbar({
		value: perc
	    });
	    $($sp).text(perc+'%');

	    $($overlay).appendTo('body');
	    $box.appendTo($('body')).slideDown('slow');
	}
	else if (what === 'update'){ //otherwise just update the value in progressbar
	    $('div', '#progress-box').progressbar( "value" , perc);
	    $('span', '#progress-box').text((perc.toFixed(1))+'%');
	}
	else if (what === 'exit'){ //exit
	    $('div', '#progress-box').progressbar( "value" , perc);
	    $('span', '#progress-box').text(perc+'%');
	    $('p', '#progress-box').text(msg);
	    $('#progress-box').delay(2000).slideUp(100, function(){
		$(this).remove();
		$('.ui-widget-overlay').remove();
	    });
	}
    };

    var get_confirm_box = function (msg){
	$('#dialog-box').remove();
        var $box = $('<div/>').attr('id', 'dialog-box');
        $box.html(msg);
        return $box;
    };

    var loadingBox = function (msg, position){
        var $loadBox = $('<div/>').attr('id', 'loading').addClass('loading-box');
        if(position === 'abs-center'){
            $($loadBox).css ({'top': '30%', 'left': '35%'});
        }
        else if (position === 'side-center'){
            $($loadBox).css ({'top': '30%', 'left': '50%'});
        }
        $loadBox.text(msg);
        $loadBox.appendTo('#' + defaults.contentID);
    };

    var get_loggerbox = function (container){
	var $logbox = $('<div/>').addClass('log-box'),
	$logbar = $('<div/>').addClass('logger-bar'),
	$logger = $('<div/>').addClass('logger');

	that.dpProject.errCount=0;
	that.dpProject.warnCount=0;

	$('<span/>').addClass('logtab').text('Logger')
            .attr ('title', 'Toggle Logger')
	    .click(function(){
		$(this).toggleClass('logtabclose');
		$('.bottom-bar', $('#'+container+'-box')).toggleClass('bottom-bar-closed');
		$('#'+container+'-tabs').toggleClass('result-box-closed');
	    }).appendTo($logbar);

	$('<span/>').addClass('clear-sign').text('Clear')
            .attr ('title', 'Clear Log')
	    .click(function(){
		$($logger).empty();
		$('.err-sign').css('display', 'none');
		$('.warn-sign').css('display', 'none');
		that.dpProject.errCount=0;
		that.dpProject.warnCount=0;
	    }).appendTo($logbar);

	$('<span/>').addClass('err-sign').text('3')
	    .attr ('title', 'Errors')
	    .appendTo($logbar);

	$('<span/>').addClass('warn-sign').text('3')
	    .attr ('title', 'Warnings')
	    .appendTo($logbar);

	$logger.appendTo($logbox);
	$logbar.appendTo($logbox);
	return $logbox;
    };

    var logMessage = function(what, msg){
	if(what==='error'){
	    $('<p/>').addClass('error').text(msg).appendTo($('.logger'));
	    that.dpProject.errCount++;
	    if(that.dpProject.errCount === 1){
		$('.err-sign').css('display', 'inline-block');
	    }
	    $('.err-sign').text(that.dpProject.errCount);
	}
	else if(what==='info'){
	    $('<p/>').addClass('info').text(msg).appendTo($('.logger'));
	}
	else if(what==='warn'){
	    $('<p/>').addClass('warn').text(msg).appendTo($('.logger'));
	    that.dpProject.warnCount++;
	    if(that.dpProject.warnCount === 1){
		$('.warn-sign').css('display', 'inline-block');
	    }
	    $('.warn-sign').text(that.dpProject.warnCount);
	}
	$('.logger').scrollTop($('p', $('.logger')).length*20);
    };

    var get_popupbox = function(type, id){
	$('.popup').remove();
        var $pBox = $('<div/>').addClass('popup').appendTo($('body'));
        if(type==='snippet'){
	    $($pBox).css('width', '600px');
            $('<h3/>').append($('<u/>').text(id.toUpperCase())).appendTo($pBox);
	    $('<p/>').text(defaults.snippet[id]).appendTo($pBox);
        }
	else if(type==='relation'){
	    var $table = $('<table/>');
	    $($pBox).css('width', '260px');
            $('<h3/>').append($('<u/>').text(id)).appendTo($pBox);
	    $table.appendTo($pBox);

	    $.each(that.dpProject.relations[id], function(i, attObj){
		var $tr = $('<tr/>').appendTo($table);
		$('<th/>').text(attObj.att_name).appendTo($tr);
		$('<td/>').text(attObj.att_type).appendTo($tr);
	    });
        }
	else if(type==='gla'){
	    $($pBox).css('width', '600px');

	    $.ajax ({
		url: defaults.getGLAContentServlet,
		type: 'POST',
		data: {gla_id: id},
		success: function (response) {
		    $('<h3/>').append($('<u/>').text(that.dpProject.glas[id].gla_name + ' [Not formatted yet]')).appendTo($pBox);
		    $('<p/>').text(response).appendTo($pBox);
		},
		error: function (jqXHR, textStatus, errorThrown) {
		    console.log(jqXHR);
		    logMessage('error', $.parseJSON(jqXHR.responseText).datapath_error.error_desc);
		}
	    });
       }

	return $pBox;
    };

    var showPopupBox = function(trigger, type, id) {
	var distance = 10,
	time = 100,
	hideDelay = 100,
	overlay,
	popup = get_popupbox(type, id);

	if ($('.overlay').length === 0) {
	    //add overlay for the first time
	    overlay = $('<div/>').addClass('overlay')
		.click(function() {
		    $(this).hide();
		    setTimeout(function() {
			hideDelayTimer = null;
			$('.popup').animate({
			    top: '-=' + distance + 'px',
			    opacity: 0
			}, time, 'swing', function() {
			    $('.popup').css('display', 'none');
			});
			$('.popup').remove();
		    }, hideDelay);
		});
	    overlay.appendTo($('body'));
	} else {
	    overlay = $('.overlay');
	}

	$(popup).css('opacity', 0);

	// reset position of popup
	overlay.show();
	popup.css({
	    top: $(trigger).offset().top - popup.height() - 10,
	    left: $(trigger).offset().left - popup.width() - 25,
	    display: 'block'
	}).animate({
	    top: '-=' + distance + 'px',
	    opacity: 1
	}, time, 'swing');
    };

    var setUnsavedGLASpan = function(set, all){
	if(all === 'current'){
	    var index = $('#gla-tabs').tabs('option', 'selected');
	    if(set){
		$('span.unsaved:eq('+index+')', $('ul.ui-tabs-nav', $('#gla-tabs'))).addClass('unsaved-active');
	    }
	    else {
		$('span.unsaved:eq('+index+')', $('ul.ui-tabs-nav', $('#gla-tabs'))).removeClass('unsaved-active');
	    }
	}
	else if(all === 'all'){
	    if(set){
		$('span.unsaved', $('ul.ui-tabs-nav', $('#gla-tabs'))).addClass('unsaved-active');
	    }
	    else {
		$('span.unsaved', $('ul.ui-tabs-nav', $('#gla-tabs'))).removeClass('unsaved-active');
	    }
	}
    };

    var getUnsavedGLAID = function(all){
	if(all==='all'){
	    var $a = $('a:has(span.unsaved-active)', $('ul.ui-tabs-nav', $('#gla-tabs'))).map(function() { return $(this).attr('href'); }).get();
	    return $a;
	}
	else if(all==='current'){
	    var index = $('#gla-tabs').tabs('option', 'selected'),
	    $a = $('a:eq('+index+')', $('ul.ui-tabs-nav', $('#gla-tabs')));
	    if($('span.unsaved-active', $($a)).length > 0){
		return $($a).attr('href');
	    }
	    else {
		return -1;
	    }
	}
    };

    var setUnsavedPiggySpan = function(set, all){
	if(all === 'current'){
	    var index = $('#piggy-tabs').tabs('option', 'selected');
	    if(set){
		$('span.unsaved:eq('+index+')', $('ul.ui-tabs-nav', $('#piggy-tabs'))).addClass('unsaved-active');
	    }
	    else {
		$('span.unsaved:eq('+index+')', $('ul.ui-tabs-nav', $('#piggy-tabs'))).removeClass('unsaved-active');
	    }
	}
	else if(all === 'all'){
	    if(set){
		$('span.unsaved', $('ul.ui-tabs-nav', $('#piggy-tabs'))).addClass('unsaved-active');
	    }
	    else {
		$('span.unsaved', $('ul.ui-tabs-nav', $('#piggy-tabs'))).removeClass('unsaved-active');
	    }
	}
    };

    var getUnsavedPiggyID = function(all){
	if(all==='all'){
	    var $a = $('a:has(span.unsaved-active)', $('ul.ui-tabs-nav', $('#piggy-tabs'))).map(function() { return $(this).attr('href'); }).get();
	    return $a;
	}
	else if(all==='current'){
	    var index = $('#piggy-tabs').tabs('option', 'selected'),
	    $a = $('a:eq('+index+')', $('ul.ui-tabs-nav', $('#piggy-tabs')));
	    if($('span.unsaved-active', $($a)).length > 0){
		return $($a).attr('href');
	    }
	    else {
		return -1;
	    }
	}
    };

    var defHeader = function(glaName){
	var str = '#ifndef _'+glaName+'_H_ \n #define _'+glaName+'_H_ \n \n';
	return str;
    };

    var metaGLAs = function(glaName, input, output, prop, glaType){
	var str = '/** GLA description. Please keep in sync with rest of implementation \n';

	//INPUTS
	str+= 'm4_define('+glaName+'_INPUTS, </';
	$.each(input, function(i){
	    if(i!== 0){
		str+=', ';
	    }
	    str+= '('+input[i].name+', '+input[i].type+')';
	});
	str+= '/>) \n';

	//OUTPUTS
	str+= 'm4_define('+glaName+'_OUTPUTS, </';
	$.each(output, function(i){
	    if(i!== 0){
		str+=', ';
	    }
	    str+= '('+output[i].name+', '+output[i].type+')';
	});
	str+= '/>) \n';

	//Property
	$.each(prop, function(i){
	    str+='m4_define(GLA_'+prop[i]+'_'+glaName+') \n';
	});

	//TYPE
	str+='m4_define(GLA_REZTYPE_'+glaName+', '+glaType+') \n';
	str+= '*/ \n';

	return str;
    };

    var commonGLA = function(glaName, input){
	//remember, class bracket is not closed here
	var str = 'class '+glaName+' { \n \n public: \n \n';

	//constructor
	str+= glaName+'(){ \n } \n \n';

	//AddItem
	str+='void AddItem(';
	$.each(input, function(i){
	    if(i!== 0){
		str+=', ';
	    }
	    str+= input[i].type+' '+input[i].name;
	});
	str+='){ \n } \n \n';

	//AddState
	str+= 'void AddState('+glaName+'& other){ \n } \n \n';
	return str;
    };

    var singleGLA = function(output){
	var str = 'void GetResult(';
	$.each(output, function(i){
	    if(i!== 0){
		str+=', ';
	    }
	    str+= output[i].type+'& '+output[i].name;
	});
	str+='){ \n } \n \n';

	return str;
    };

    var multiGLA = function(output){
	var str = 'void Finalize(){ \n } \n \n';
	str += 'void GetResult(';
	$.each(output, function(i){
	    if(i!== 0){
		str+=', ';
	    }
	    str+= output[i].type+'& '+output[i].name;
	});
	str+='){ \n } \n \n';

	return str;
    };

    var fragmentGLA = function(glaName, output){
	var str = 'struct '+glaName+'_Iterator { \n'+
	    '/** fill in with iterator code */ \n \n'+
	    'bool isDone(){ return true; }  }; \n \n'+
	    'int GetNumFragments(void){int numFrags=1; \n'+
	    '/* determine how many fragments to split the output into */ \n'+
	    'return numFrags;} \n \n'+
	    glaName+'_Iterator* Finalize(int _fragment){ \n'+
	    '/* create iterator for fragment _fragment */ \n'+
	    glaName+'_Iterator* it = new '+glaName+'_Iterator( ... );'+
	    'return it;} \n \n'+
	    'bool GetNextResult('+glaName+'_Iterator* it';
	$.each(output, function(i){
	    str+= ', '+output[i].type+'& '+output[i].name;
	});
	str+= '){ \n'+
	    '/* check if we are done. Use different code if detection performed differently*/ \n'+
	    'if (it->isDone()) return false; \n'+
	    '/* produce 1 tuple.*/ \n'+
	    '/* advance iterator */ \n}';

	return str;
    };

    (function ($){
	init();
    })($);
};

$(window).load (function() {
    loader (jQuery);
});

