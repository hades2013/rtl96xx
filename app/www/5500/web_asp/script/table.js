


var sortTableIds = new Array();
var nonSortTableIds = new Array();
var globalTable = new Array();
var Common_0_Results_Found = '0 Results Found';
Array.prototype.has = function(value) 
{
  for (var i = 0, loopCnt = this.length; i < loopCnt; i++) 
  {
    if (this[i] === value) 
    {
      return true;
    }
  }
  return false;
};

function NikolaDataTableDisableTextSelection(obj, cursor)
{
  if($.browser.mozilla){//Firefox
    $(obj).css('MozUserSelect','none');
  }else if($.browser.msie){//IE
    $(obj).bind('selectstart',function(){return false;});
  }else{//Opera, etc.
    $(obj).mousedown(function(){return false;});
  }

  $(obj).css('cursor',cursor);
}
function EnableRadioButtons()
{

  if(this.document.body.id == "AddressTablesDynamicArray")
  {
    return;
  }
  
  var tr = document.getElementsByTagName('tr');
  if(tr != null)
  {
    for(var i=0; i < tr.length;i++)
    {
      if(tr[i].className.indexOf("selectedRow") != -1)
      {
        if(tr[i].childNodes[0].childNodes[0] != undefined)
        {
          tr[i].childNodes[0].childNodes[0].checked = true;
          return;
        }
      }
    }
  }
}

function PaginationUpdate(tid, oTable)
{
  var paginatorValues = Array(5,10,25,50,100);
  var oSettings = oTable.fnSettings();
  var allItems = oSettings.fnRecordsDisplay ();
  var startIndex = oSettings.fnDisplayStartGet();
  var itemsPerPage = oSettings.fnDisplayLengthGet();
  var currentPage = parseInt(startIndex/itemsPerPage)+1;

  PaginationFieldsUpdate(tid, itemsPerPage, currentPage, allItems, paginatorValues);
  var tr = document.getElementsByTagName('tr');
  var selected = "false";
  if(tr != null)
  {
    for(var i=0; i < tr.length;i++)
    {
      if(tr[i].className.indexOf("selectedRow") != -1)
      {
        selected = "true";
        break;
      }
    }
  }
  var buttons = $(".button180");
  for(j = 0; j < buttons.length; j++)
  {
    if (navigator.appVersion.indexOf("MSIE") != -1)
    {
      var name = buttons[j].children[0].innerText; 
      var buttonclass = buttons[j].children[0].className; 
      /*why name contain a blank key.?*/
      if(name == Customer_Common_Detail || name == Customer_Common_Edit ||
         name == Customer_Common_CopySettings || name == Customer_Common_Del)
      { 
      	     
        if(selected == "false")
        {
          if(buttonclass != "ButtonUrlDisabledMouseOut")
          {
            var btnname = buttons[j].children[0].name;
            var butid = btnname.substr(2,btnname.length);
            button = GetElementsByXid(butid,'INPUT')[0];
            xui_oem_disable(button);
          }
        }
        else if(selected == "true")
        {
          if(buttonclass != "ButtonUrlEnabledMouseOut")
          {
            var btnname = buttons[j].children[0].name;
            var butid = btnname.substr(2,btnname.length);
            button = GetElementsByXid(butid,'INPUT')[0];
            xui_oem_enable(button);
          }
        }
      }
    }
    else
    {
      var name = buttons[j].childNodes[1].textContent; 
      var buttonclass = buttons[j].childNodes[1].className;  
      if(name == Customer_Common_Detail || name == Customer_Common_Edit ||
         name == Customer_Common_CopySettings || name == Common_Common_Del)
      { 
        if(selected == "false")
        {
          if(buttonclass != "ButtonUrlDisabledMouseOut")
          {
            var btnname = buttons[j].childNodes[1].name;
            var butid = btnname.substr(2,btnname.length);
            button = GetElementsByXid(butid,'INPUT')[0];
            xui_oem_disable(button);
          }
        }
        else if(selected == "true")
        {
          if(buttonclass != "ButtonUrlEnabledMouseOut")
          {
            var btnname = buttons[j].childNodes[1].name;
            var butid = btnname.substr(2,btnname.length);
            button = GetElementsByXid(butid,'INPUT')[0];
            xui_oem_enable(button);
          }
        }
      }
    }
  }
  EnableRadioButtons();
}



function PaginationBuild(tid, oTable)
{
  var paginatorValues = Array(5,10,25,50,100);
  var oSettings = oTable.fnSettings();
  var allItems = oSettings.fnRecordsDisplay ();
  var startIndex = oSettings.fnDisplayStartGet();
  var itemsPerPage = oSettings.fnDisplayLengthGet();
  var currentPage = parseInt(startIndex/itemsPerPage)+1;

  PaginationHeaderBuild(tid, itemsPerPage, currentPage, allItems, paginatorValues);
  PaginationPerPageSelector(tid, function(val) {
      oSettings.fnDisplayLengthSet(val);
      oTable.fnDraw();
      PaginationUpdate(tid, oTable);
      });
  PaginationFooterBuild(tid, itemsPerPage, currentPage, allItems, paginatorValues);
  PaginationPageSelector(tid, function(val) {
      currentPage = val - 1;
      startIndex = currentPage * oSettings.fnDisplayLengthGet();
      oSettings.fnDisplayStartSet(startIndex);
      oTable.fnStartChange(startIndex);
      PaginationUpdate(tid, oTable);
      });

  PaginationButtonHandlers(tid, function(val) {PaginationChange(tid, oTable, val);});
}

function PaginationCheckNeed(oTable, tid)
{
  var oSettings = oTable.fnSettings();
  if(oSettings.fnRecordsTotal() > oSettings.fnDisplayLengthGet())
  {
    PaginationBuild(tid, oTable);
    $("#xuiPaginationTableOuter" + tid).show();
  }
  else
  {
    $("#xuiPaginationTableOuter" + tid).hide();
  }
}

function NikolaDataTableRemoveSorting(oSettings, sort)
{
  for(var i = 0; i < oSettings.aoColumns.length; i++)
  {
    var col = oSettings.aoColumns[i];
    var th = col.nTh;

    // disable sorting if asked for and also text selection
    if(sort == false)
    {
      NikolaDataTableDisableTextSelection(th, 'auto');
      col.bSortable = false;
      continue;
    }

    // disable sorting for selectors and also text selection
    if ($(th).is('.checkBoxSelect_TH') || $(th).is('.radioSelect_TH') || 
        $(th).is('.defTH_lastCol') || $(th).is('.def_lastCol')) {
      NikolaDataTableDisableTextSelection(th, 'auto');
      col.bSortable = false;
      continue;
    }

    // enable sorting and also text selection
    col.bSortable = sort;
    NikolaDataTableDisableTextSelection(th, 'pointer');
    if ($(th).is('.intcol')) {
      col.sType = "intcol";
    } else if ($(th).is('.uspcol')) {
      col.sType = "uspcol";
    } else {
      col.sType = "html";
    }
  }
} 

function NikolaUsp2Index(s)
{
  s = s.toLowerCase();
  var ret = 0;
  if(s.match("^ch"))
  {
    ret = parseInt(s.replace("ch", "")) + 1000;
  }
  else if(s.match("^e"))
  {
    ret = parseInt(s.replace("e", "")) + 2000;
  }
  else if(s.match("^g"))
  {
    ret = parseInt(s.replace("g", "")) + 3000;
  }
  return ret.toString();
}

jQuery.fn.dataTableExt.oSort['intcol-asc'] = function(a,b) {
  var x = a.replace( /<.*?>/g, "" ).toLowerCase().trim();
  var y = b.replace( /<.*?>/g, "" ).toLowerCase().trim();
  x = parseInt(x);
  y = parseInt(y);
  return ((x < y) ? -1 : ((x > y) ? 1 : 0));
}

jQuery.fn.dataTableExt.oSort['intcol-desc'] = function(a,b) {
  var x = a.replace( /<.*?>/g, "" ).toLowerCase().trim();
  var y = b.replace( /<.*?>/g, "" ).toLowerCase().trim();
  x = parseInt(x);
  y = parseInt(y);
  return ((x < y) ? 1 : ((x > y) ? -1 : 0));
}

jQuery.fn.dataTableExt.ofnSearch['html'] = function(data) {
  EnableRadioButtons();
  return data.replace( /<.*?>/g, "" ).toLowerCase().trim();
}

jQuery.fn.dataTableExt.ofnSearch['intcol'] = function(data) {
  EnableRadioButtons();
  return data.replace( /<.*?>/g, "" ).toLowerCase().trim();
}


jQuery.fn.dataTableExt.oSort['uspcol-asc'] = function(a,b) {
  var x = NikolaUsp2Index(a.replace( /<.*?>/g, "" ).toLowerCase().trim());
  var y = NikolaUsp2Index(b.replace( /<.*?>/g, "" ).toLowerCase().trim());
  return ((x < y) ? -1 : ((x > y) ? 1 : 0));
}

jQuery.fn.dataTableExt.oSort['uspcol-desc'] = function(a,b) {
  var x = NikolaUsp2Index(a.replace( /<.*?>/g, "" ).toLowerCase().trim());
  var y = NikolaUsp2Index(b.replace( /<.*?>/g, "" ).toLowerCase().trim());
  return ((x < y) ? 1 : ((x > y) ? -1 : 0));
}
function NikolaDataTableInit(tid, sort)
{
  $(document).ready(function() {
      var oTable  = $("#xuiRepeatTable" + tid).dataTable( {
        "bAutoWidth": false,
        "oLanguage": { "sZeroRecords": Common_0_Results_Found },
        "iDisplayLength" : 10,
        "aaSorting": [],
        "sDom": '<"top">rt<"bottom"<"clear">'
        });
      var oSettings = oTable.fnSettings();
      NikolaDataTableRemoveSorting(oSettings, sort);
      PaginationCheckNeed(oTable, tid);

      globalTable[tid] = oTable;
      });
}

function NikolaDataTableArrayInit(tid)
{
  $(document).ready(function() {
      if(document.body.id == "IGMPSnooping") {
	    var oTable  = $("#xuiRepeatTable" + tid).dataTable( {
        "bAutoWidth": false,
        "oLanguage": { "sZeroRecords": Common_0_Results_Found },
        "iDisplayLength" : 10,
        "aaData": arraydata_3_1,
        "sDom": '<"top">rt<"bottom"<"clear">',
        "aoColumns": [
        { "sTitle": "&nbsp;", "sClass": "def header" ,"bVisible":    true },
        { "sTitle": Security_VLAN_ID, "sClass": "intcol header", "bVisible":    true },
        { "sTitle": IGMP_Snooping_Table_Operational_Status, "sClass": "enumcol header", "bVisible":    true },
        { "sTitle": IGMP_Snooping_Table_Auto_Learn, "sClass": "enumcol header", "bVisible":    true },
        { "sTitle": IGMP_Snooping_Table_Forbidden_Ports, "sClass": "uspcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Forbidden_MRouter_Ports, "sClass": "uspcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Robustness, "sClass": "intcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Interval, "sClass": "intcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Response, "sClass": "intcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Last_Counter, "sClass": "intcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Last_Interval, "sClass": "intcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Immediate, "sClass": "intcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_IGMP_Status, "sClass": "enumcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Version, "sClass": "enumcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Address, "sClass": "enumcol header", "bVisible":    true },
        { "sTitle": "&nbsp;", "sClass": "defTH_lastCol header", "bVisible": true }
        ]
        });
	  }
	  else if(document.body.id == "MLDSnooping"){
		var oTable  = $("#xuiRepeatTable" + tid).dataTable( {
        "bAutoWidth": false,
        "oLanguage": { "sZeroRecords": Common_0_Results_Found },
        "iDisplayLength" : 10,
        "aaData": arraydata_3_1,
        "sDom": '<"top">rt<"bottom"<"clear">',
        "aoColumns": [
        { "sTitle": "&nbsp;", "sClass": "def header" ,"bVisible":    true },
        { "sTitle": Security_VLAN_ID, "sClass": "intcol header" },
        { "sTitle": IGMP_Snooping_Table_Operational_Status, "sClass": "enumcol header" },
        { "sTitle": IGMP_Snooping_Table_Auto_Learn, "sClass": "enumcol header", "bVisible":    true },
        { "sTitle": IGMP_Snooping_Table_Forbidden_Ports, "sClass": "uspcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Forbidden_MRouter_Ports, "sClass": "uspcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Robustness, "sClass": "intcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Interval, "sClass": "intcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Response, "sClass": "intcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Last_Counter, "sClass": "intcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Last_Interval, "sClass": "intcol header", "bVisible":    true },
		{ "sTitle": IGMP_Snooping_Table_Immediate, "sClass": "intcol header", "bVisible":    true },
        { "sTitle": "&nbsp;", "sClass": "defTH_lastCol header", "bVisible": true }
        ]
        });
	  }
	  else{
      var oTable  = $("#xuiRepeatTable" + tid).dataTable( {
        "bAutoWidth": false,
        "oLanguage": { "sZeroRecords": Common_0_Results_Found },
        "iDisplayLength" : 10,
        "aaData": arraydata_3_1,
        "sDom": '<"top">rt<"bottom"<"clear">',
        "aoColumns": [
        { "sTitle": CONTENT_Interface, "bVisible":    false },
        { "sTitle": "VLAN ID", "sClass": "intcol header" },
        { "sTitle": Port_MAC_Address, "sClass": "def header" },
        { "sTitle": Page_Interface, "sClass": "uspcol header", "bVisible":    true },
        { "sTitle": Common_Type, "sClass": "uspcol header", "bVisible":    true },
        { "sTitle": "&nbsp;", "sClass": "defTH_lastCol header", "bVisible": true }
        ]
        });
	  }
      PaginationCheckNeed(oTable, tid);
      var oSettings = oTable.fnSettings();
      NikolaDataTableRemoveSorting(oSettings, true);
      PaginationCheckNeed(oTable, tid);
      globalTable[tid] = oTable;
  });
  tr = document.getElementById('3_1');
  $(tr).addClass('norepeatheading');
}

function PaginationChange(tid, oTable, which)
{
  oTable.fnPageChange(which, true); 
  PaginationUpdate(tid, oTable);
}

function NikolaDataTableSearch(str, col, tid)
{
  if(globalTable[tid] != null)
  {
    globalTable[tid].fnFilter( "^" + str + "$", col, false );
    $(".defTH_lastCol").attr('style', '');
    PaginationCheckNeed(globalTable[tid], tid);
  }
}
function NikolaDataTableUndoSearch(str, col,tid)
{
  if(globalTable[tid] != null)
  {
    globalTable[tid].fnFilter( "", col, true);
  }
}
function NikolaRepeatTableSortingInit(tid)
{
  sortTableIds.push(tid);
}
function NikolaRepeatTablePaginationInit(tid)
{
  if(sortTableIds.has(tid) == false)
  {
    nonSortTableIds.push(tid);
  }
}
function Taobao_DoTableSortOrPage()
{ 
  for(var i = 0; i < sortTableIds.length; i++) {
    if(document.body.id != "AddressTablesDynamicArray") {
	  if(document.body.id == "IGMPSnooping" || document.body.id == "MLDSnooping"){
	    NikolaDataTableArrayInit(sortTableIds[i]);
	  }
	  else{
	    NikolaDataTableInit(sortTableIds[i], true);
	  }
    } else {
      NikolaDataTableArrayInit(sortTableIds[i]);
    }
  }
  for(var i = 0; i < nonSortTableIds.length; i++) {
    if(document.body.id != "AddressTablesDynamicArray") {
	  if(document.body.id == "IGMPSnooping" || document.body.id == "MLDSnooping"){
	    NikolaDataTableArrayInit(nonSortTableIds[i]);
	  }
	  else{
      NikolaDataTableInit(nonSortTableIds[i], false);
	  }
    } else {
      NikolaDataTableArrayInit(nonSortTableIds[i]);
    }
  }
}