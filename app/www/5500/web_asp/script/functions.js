

var xeData = new Array();
var applyfunc = new Array();
var valmap = {
  read_only : "read-only",
  read_write : "read-write",
  label : "label",
  plain_label : "plain-label",
  textbox : "textbox",
  textarea : "textarea",
  passwdbox : "textbox-passwd",
  radio : "radio",
  radio_left : "radio-left",
  radio_none : "radio-none",
  vradio : "vradio",
  vradio_left : "vradio-left",
  vradio_none : "vradio-none",
  combo : "combo",
  combo_multiselect : "combo-multiselect",
  checkbox : "checkbox",
  checkboxNeg : "checkbox-neg",
  checkbox_multiselect : "checkbox-multiselect",
  button : "button",
  file : "file",
  url : "url",
  image : "image",
  url_list : "url-list",
  hidden : "hidden",
  none : "none",
  onClick : 2,
  onChange : 1,
  onLoad : 0
}; 

if (!Array.prototype.indexOf)
{
Array.prototype.indexOf = function(elt /*, from*/)
{
var len = this.length >>> 0;
var from = Number(arguments[1]) || 0;
from = (from < 0)
? Math.ceil(from)
: Math.floor(from);
if (from < 0)
from += len;
for (; from < len; from++)
{
if (from in this &&
this[from] === elt)
return from;
}
return -1;
};
}
String.prototype.trim = function () {
return this.replace(/^\s*/, "").replace(/\s*$/, "");
};

function NikolaIEBrowserPadding()
{
  if($('.deftestme').length > 0)
  {
    var TR = $('.deftestme');
    for(j = 0; j < TR.length;j++)
    {
      var flag = "false" ;
      var tr = TR[j]
      for(i = 0; i < tr.childNodes.length; i++)
      {   
        if(tr.childNodes[i] != undefined && tr.childNodes[i].nodeName != "#text")
        {
          if(tr.childNodes[i].style.display == "none")
          {
            continue;
          }
          else
          {
            var class_Name = tr.childNodes[i].className;
            if(class_Name != "filtercollapseright" && class_Name != "keyimgtd3" && class_Name != ""
                && class_Name != "def5left" && class_Name != "def5right")
            {   
              class_Name = class_Name + " TopPadding";
              tr.childNodes[i].className = class_Name;
            } 
            flag = "true";
          }  
        }
      }
      if(flag == "true")
      {
        break;
      }
    }
  }
}

 
function NikolaHRLabelPadding()
{
  var td = document.getElementsByTagName('TD');
  for(var i=0; i < td.length ; i++)
  {
    if(td[i].className == "hr_lbl_line")
    {
      if(td[i].parentNode.nextSibling.tagName != "TR")
        var tr = td[i].parentNode.nextSibling.nextSibling;
      else
        var tr = td[i].parentNode.nextSibling; 
      if(tr.childNodes[0].tagName != "TD")
      {
        if(tr.childNodes[1].className == "defleft")
          tr.childNodes[1].className = "HRPaddingleft";
      }
      else
      {
        if(tr.childNodes[0].className == "defleft")
        {
          tr.childNodes[0].className = "HRPaddingleft";
          tr.childNodes[1].className = "HRPaddingright";
        }
      }
    }
    else if(td[i].className == "keyimgtd")
    {
     var parent = td[i];
     if(parent.nextSibling != null)
     {
      if(parent.nextSibling.tagName != "TD")
      {
        if(parent.nextSibling.nextSibling != null)
        {
          if(parent.nextSibling.nextSibling.className == "defleft")
            parent.nextSibling.nextSibling.className = "imgpadding";
       
          if(parent.nextSibling.nextSibling.className == "vradioStyleleft")
            parent.nextSibling.nextSibling.className = "vradioStyleleft2";

          if(parent.nextSibling.nextSibling.className == "inGroupHeaderleft")
            parent.nextSibling.nextSibling.className = "inGroupHeaderleft2";

          if(parent.nextSibling.nextSibling.className == "collapseright")
            parent.nextSibling.nextSibling.className = "collapseright2";
        }
      }
      else
      {
        if(parent.nextSibling.className == "defleft")
          parent.nextSibling.className = "imgpadding";
       
        if(parent.nextSibling.className == "vradioStyleleft")
          parent.nextSibling.className = "vradioStyleleft2";

        if(parent.nextSibling.className == "inGroupHeaderleft")
          parent.nextSibling.className = "inGroupHeaderleft2";

        if(parent.nextSibling.className == "collapseright")
          parent.nextSibling.className = "collapseright2";
      }
     }
    }
  }
} 
function isDigit( s )
{
  var exp = "^([0-9]|[1-4][0-9]|50)$";
  return RegExp(exp).test($.trim(s));
}

function xuiRunJsRoutine(func)
{
var routine = window[func];
var retVal = null;
if(routine !== null && routine !== undefined)
{
 try{
  retVal = routine();
 } catch(err) {
 alert("There was an error in Function " + func + " (" + err + ")");
 retVal = 0;
 }   
}
 return 0;
}
function xui_oem_getElemAttributeWithRetval(ent,attrib,retval){
if(ent.nodeType != 1) return retval;
var attribValue = ent.getAttribute(attrib);
var attribValueType = typeof(attribValue);
if(attribValueType == "undefined") return retval;
if(attribValueType != "string") return attribValue;
if(attribValue == null) return retval;
return attribValue;
}

function tokenize(inputString, separator,ignoreEmptyTokens)
{
var input = inputString;
var trim = " ";
var i, max;
var arr = input.split(separator);
if(trim)
{
for(i=0, max = arr.length; i<max; i++)
{
while(arr[i].slice(0, trim.length) == trim)
{
arr[i] = arr[i].slice(trim.length);
}
while(arr[i].slice(arr[i].length-trim.length) == trim)
{
arr[i] = arr[i].slice(0, arr[i].length-trim.length);
}
}
}
var tmpArray = new Array();
if(ignoreEmptyTokens)
{
for(i=0, max = arr.length; i<max; i++)
{
if(arr[i] != "")
{
tmpArray.push(arr[i]);
}
else
{
tmpArray.push(0);
}
}
}
else
{
tmpArray = arr;
}
return tmpArray;
}

function xuiOldOrNewGet(obj) {
  return obj ? obj : new Object();
}

function xuiSpanDataGet(xid) {
 xeData.spanData = xuiOldOrNewGet(xeData.spanData);
 xeData.spanData[xid] = xuiOldOrNewGet(xeData.spanData[xid]);
 return xeData.spanData[xid];
}

function xuiVisibleAnchorNodeGet(obj) {
// if(obj != null && obj.type != null && obj.type == "hidden" ) {
  var spanEnt = obj/*.nextSibling?obj.nextSibling:null*/;
  if(spanEnt != null)
  {
     var url = $(spanEnt).find('a')?$(spanEnt).find('a')[0]:null;
     if( url != null)
     {
       return url;
     }
  }
//  }
  return null;
}

function xuiClassCheck(ele,cls) {
  return ele.className.match(new RegExp('(\\s|^)'+cls+'(\\s|$)'));
}
function xuiClassAdd(ele,cls) {
  if (!this.xuiClassCheck(ele,cls)) ele.className += " "+cls;
}
function xuiClassRemove(ele,cls) {
  if (xuiClassCheck(ele,cls)) {
  var reg = new RegExp('(\\s|^)'+cls+'(\\s|$)');
  ele.className=ele.className.replace(reg,' ');
 }
}

function xuiGetParent(ent, tag)
{
  if (document.all)
  {
    while (ent !== null)
    {
      if (ent.tagName.toLowerCase() == tag.toLowerCase())
      {
        return ent;
      }
      else
      {
        ent = ent.parentElement;
      }
    }
  }
  else if (document.getElementById)
  {
    if (ent.nodeType == 1 && ent.tagName.toLowerCase() == tag.toLowerCase())
    {
      return ent;
    }
    else
    {
      return xuiGetParent(ent.parentNode,tag);
    }
  }

  return undefined;
}
function xuiAddEvent( obj, type, fn )
{
  if (obj.addEventListener) {
  obj.addEventListener( type, fn, false );
  }
  else if (obj.attachEvent)
  {
  obj["e"+type+fn] = fn;
  obj[type+fn] = function() {
  obj["e"+type+fn]( window.event );
  }
  obj.attachEvent( "on"+type, obj[type+fn] );
  }
}

function xuiSpanClassChange(obj, xid, cl, value) {
   var spanData = xuiSpanDataGet(xid);
   if(cl != null && value != null) {
   spanData[cl] = value;
   }
   obj.className = spanData.baseClass + spanData.showClass + spanData.MouseClass;
}

function xuiCheckDisabled(obj)
{
   if(obj.getAttribute("disabled") == "disabled") {
   return true;
   } else if(obj.getAttribute("disabled") == true) {
   return true;
   }
   return false;
}
function set_button_default_action(xid,spanEnt)
{
   var spanData = xuiSpanDataGet(xid);
   spanData.baseClass = 'ButtonUrl';
   spanData.showClass = 'Enabled';
   spanData.MouseClass = 'MouseOut';
   xuiAddEvent(spanEnt, "mouseover", function() {
   xuiSpanClassChange(this, xid, "MouseClass", "MouseOver");
   });
   xuiAddEvent(spanEnt, "mouseout", function() {
   xuiSpanClassChange(this, xid, "MouseClass", "MouseOut");
   });
   xuiAddEvent(spanEnt, "mousedown", function() {
   xuiSpanClassChange(this, xid, "MouseClass", "MouseDown");
   });
   xuiAddEvent(spanEnt, "mouseup", function() {
   xuiSpanClassChange(this, xid, "MouseClass", "MouseUp");
   });

   var url_elements = spanEnt.getElementsByTagName('a');
   var visent = url_elements[0]; 
   if(xuiCheckDisabled(visent) == true) {
     xuiSpanClassChange(spanEnt, xid, "showClass", "Disabled");
   }else {
       xuiSpanClassChange(spanEnt, xid, null, null);
    }    	
 }
 
 // This function sets the styling of the row to hover
function repeatTRMouseOver(senderObject){
  var childrenLength = senderObject.childNodes.length;
  for (var childCounter = 0; childCounter < childrenLength; childCounter++){
	var currTD = senderObject.childNodes[childCounter];
	if (currTD.nodeName.toUpperCase() == "TD"){
        currTD.className = currTD.className + " hoverRow";
	}	
  }
}

// This function cancells the row's hover styling
function repeatTRMouseOut(senderObject){
  var childrenLength = senderObject.childNodes.length;
  for (var childCounter = 0; childCounter < childrenLength; childCounter++){
	var currTD = senderObject.childNodes[childCounter];
	if (currTD.nodeName.toUpperCase() == "TD"){
      var currTDClass = currTD.className;
	  var hoverRowIndex = currTDClass.indexOf("hoverRow");
	  if (hoverRowIndex != -1){
          currTD.className = currTDClass.substr(0, hoverRowIndex);
	  }
	}
  }
}

function ValueBasedButtonAction(args,selector,enableCount)
{
  var count = 0;
  var elements = args.split(",");
  for(var i = 0 ; i < elements.length; i++)
  {
    ids = elements[i].split("|");
    var element_Id = GetElementsByXid(ids[0]);
    var value = ids[1];
    var button = GetElementByXid(ids[2]);
    
    for(var j = 1; j < element_Id.length; j++)
    {
    	var span_e = element_Id[j].getElementsByTagName("input");
      var inp = span_e[0];
      if(inp.value == value && inp.checked == true)
      {
        count++;
      }
    }
  }       
    
  if(count > 0)
  {
    xui_oem_disable(button);
  }
  else if(enableCount>0)
  {
    xui_oem_enable(button);
  }
}

function CheckBoxSelectAllValueBasedButtonAction(args,selector, selector_num)
{
  var elements = args.split(",");
  if(elements.length > 1)
  {
    for(var i = 0 ; i < elements.length; i++)
    {
      ids = elements[i].split("|");

      var element_Id = GetElementByXid(ids[0]);
      var value = ids[1];
      var button = GetElementByXid(ids[2]);
      var span_e = selector.getElementsByTagName("input");
      var inp = span_e[0];
      if(inp.checked == true)
      {
        for(j = 0; j < element_Id.length; j++)
        {
          if(element_Id[j].value == value)
          {
            xui_oem_disable(button);
          }
        }
      }
    }
  }
  else
  {
    ids = elements[0].split("|");

    var element_Id = GetElementsByXid(ids[0]);
    var value = ids[1];
    var button = GetElementByXid(ids[2]);
    var span_e = selector.getElementsByTagName("input");
    var inp = span_e[0];
    if(inp.checked == true)
    {
      for(j = 1; j < element_Id.length; j++)
      {
      	var span_enty = element_Id[j].getElementsByTagName("input");
        var inp_tmp = span_enty[0];
        if(inp_tmp.value == value)
        {
          xui_oem_disable(button);
        }
      }
    }
  }
}
function xuiVisibleElementDisable(obj, disable, xid)
{

if(obj.nodeType != 1) return;
if(obj.disabled == disable) return;
var suffix = disable ? "Disabled" : "Enabled";
if(obj.tagName != "TD")
{
//obj.disabled= disable; 
if(disable) 
  obj.setAttribute("disabled", "disabled");
else
	obj.removeAttribute("disabled");
}
if (obj.tagName == "TEXTAREA") {
obj.className = 'xuiTextArea' + suffix;
} else if(obj.tagName == "SELECT") {
obj.className = 'xuiSelect' + suffix;
} else if (obj.tagName != "INPUT") {
} else if (obj.type == "text") {
obj.className = 'xuiInputText' + suffix;
} else if (obj.type == "password") {
obj.className = 'xuiInputPassword' + suffix;
} else if (obj.type == "radio") {
obj.className = 'xuiInputRadio' + suffix;
} else if (obj.type == "checkbox") {
obj.className = 'xuiInputCheckbox' + suffix;
} else if (obj.type == "image") {
  //for buttons with style image, set disabled image
  //if(disable == true) {
  //xuiButtonDisble(obj, xid);
  //} else {
  //xuiButtonEnable(obj, xid);
  //}
}
}
//
function GetElementByXid(xid)
{
  	 	var span_elements = document.getElementsByTagName('SPAN'); 
  	 	   	 		
  	 	for(var i=0;i<span_elements.length;i++)
  	 	{
  	 		var spanEnt=span_elements[i];
  	 		var t_xid=spanEnt.getAttribute("xid"); 
  	 		if(t_xid==null)
  	 		   continue; 
  	 		if(t_xid ==xid)
  	 		{
  	 			return  spanEnt;
  	 		} 
  	 	}	
}

function GetElementsByXid(xid)
{
	    var tmp_array = new Array();
  	 	var span_elements = document.getElementsByTagName('SPAN'); 
  	 	   	 		
  	 	for(var i=0;i<span_elements.length;i++)
  	 	{
  	 		var spanEnt=span_elements[i];
  	 		var t_xid=spanEnt.getAttribute("xid"); 
  	 		if(t_xid==null)
  	 		   continue; 
  	 		if(t_xid ==xid)
  	 		{
  	 			tmp_array.push(spanEnt);
  	 		} 
  	 	}
  	 	return tmp_array;	
}

function xuiShed(which, element_id , str )
{
  if(str == "") {
  return 0;
  }
  var xids = tokenize(str,"|" ,true);
  // for all the ids from xml shed
  for ( var i = 0, max = xids.length; i < max; i++ )
  {
  var cur_obj =GetElementByXid(xids[i]); 

  if(cur_obj == undefined)
     continue;
  var parent_obj= cur_obj.parentNode;
  switch(which)
  {
  case 0: //show   
  if(parent_obj.tagName =="TD")
  {
    parent_obj.style.display = "";
    cur_obj.style.display = "";
  }
  else
  	cur_obj.style.display = "";
  break;
  case 1: //hide
  if(parent_obj.tagName =="TD") 
  {
    parent_obj.style.display = "none";
    cur_obj.style.display = "none";
  }  
  else
  	cur_obj.style.display = "none";
  break;
  case 2: //disable
  for(var j=0;j<cur_obj.childNodes.length;j++)
  {
   if(cur_obj.childNodes[j].nodeType!=1)
     continue;
   else
   	{
     xuiVisibleElementDisable(cur_obj.childNodes[j], true, xids[i]);
     break;
    }
  }
  break;
  case 3: //enable
  for(var j=0;j<cur_obj.childNodes.length;j++)
  {
   if(cur_obj.childNodes[j].nodeType!=1)
     continue;
   else
   	{
       xuiVisibleElementDisable(cur_obj.childNodes[j], false, xids[i]);
       break;
    }
  }

  break;
  default:
  break;
  }
  }
  return 0;
}

function xuiDelegateJSActions(spanEnt,xa, xid ,off, value)
{
   xuiShed(0,xid, xa[++off]); //show
   xuiShed(1,xid, xa[++off]); //hide
   xuiShed(2,xid, xa[++off]); //disable
   xuiShed(3,xid, xa[++off]); //enable  
   
   return 0;
}

function xuiProcessJSActions(spanEnt,xid,value){
	
	var xa = xeData["xa_" + xid];
  if(xa == null) {
   return;
   }
   for(var j=0, max = xa.length; j <max; j+=5)
   {
     var result = null;
     var regex = new RegExp(xa[j]);
     if(regex == null) {
     alert("Invalid regular expression " + xa[j]);
     return;
     }
     result = regex.exec(value) ;
     //value matched with the regular expression pattern
     if(result != null && RegExp.leftContext.length ==0 && RegExp.rightContext.length ==0)
     {
       xuiDelegateJSActions(spanEnt,xa, xid , j, value);
       break;
     }
   }	
}

function xuiSpanEnable(obj, xid, enable) {
  if(obj && xid) {
     xuiSpanClassChange(obj, xid, "showClass", enable ? "Enabled" : "Disabled");
  }
}
function xuiHtmlNodeDisable(obj, disable) {
 return xuiHtmlNodeEnable(obj, (disable == true) ? false : true);
}
function xuiHtmlNodeEnable(obj, enable) {
  //xuiHtmlNodeEnable2(obj, enable);
  var visEnt = xuiVisibleAnchorNodeGet(obj);
  if(visEnt != null) {
    xuiHtmlNodeEnable2(visEnt, enable);
    xuiSpanEnable(visEnt.parentNode, obj.getAttribute("xid"), enable);
  }
}
function xui_oem_enable (node)
{
//  if(node.nodeType != 1) return;
    xuiHtmlNodeEnable(node, true);
}
function xui_oem_disable (node)
{
 //if(node.nodeType != 1) return;
 xuiHtmlNodeDisable(node, true);
}
function xuiHtmlNodeEnable2(obj, enable) {
  if(obj.nodeType != 1) return;
  if(enable == true) {
  obj.disabled = false;
  if(typeof(obj.removeAttribute) == "function") {
  obj.removeAttribute("disabled");
  }
  } else {
  obj.disabled = true;
  if(typeof(obj.setAttribute) == "function") {
  obj.setAttribute("disabled", "disabled");
  }
  }
}

function NikolaSelectRow(row, td, selected)
{
  if(row == undefined) row = xuiGetParent (td, 'tr');
  if(row == undefined) return row;

  if(selected) {
    xuiClassAdd(row, "selectedRow");
  } else {
    xuiClassRemove(row, "selectedRow");
  }
  return row;
}
/********************************************************************************************/
function xuiProcessOnChangeCombo (ent, xid)
{
    var val;
   /*Restore to default value*/
        xuiProcessJSActions(ent,xid,"All");
	  
          for(var i=0;i<ent.length;i++)//select
          	{
          	   if( ent[i].selected )
          	   	{
          	   	   val=ent[i].value;
			   xuiProcessJSActions(ent,xid,val);
			    break;
          	   	}
          	}
    
	 var jsfunc = xeData["js_" + xid + "_" + val];
   if(jsfunc!=null)
       xuiRunJsRoutine(jsfunc); 	
}

function xuiProcessOnClickRadio(ent,xid)
{ 
	var val=ent.value;
	xuiProcessJSActions(ent,xid,val);
	
	 var jsfunc = xeData["js_" + xid + "_" + val];
   if(jsfunc!=null)
       xuiRunJsRoutine(jsfunc); 	
}

function xuiProcessOnClickCheckBox(ent, xid,val1, val2)
{
 var val = ent.checked? val1 : val2;
 xuiProcessJSActions(ent,xid,val); 
 
 var jsfunc = xeData["js_" + xid + "_" + val];
   if(jsfunc!=null)
       xuiRunJsRoutine(jsfunc);
}
function xuiButtonNoneSubmit(ent,submitFlag,xid)
{
var attrib = xui_oem_getElemAttributeWithRetval(ent, 'disabled',true);
if(attrib == true || attrib == "disabled") return false;

var jsfunc = xeData["js_" + xid + "_" + "button"];
   if(jsfunc!=null)
       xuiRunJsRoutine(jsfunc); 
       
return true;
}
/********************************************valid check Start***********************/
function xuiDisplayInlineErrorMsgPersistent(msg)
{
  var message="<table><TR><TD><IMG src=\"image/Status_criticalerror_icon.png\" width=32 height=32></TD><TD id=\"displaymsg\"><DIV >"+msg+"</DIV></TD></TR></table>";	
  $("#inlineErrorMessagesTd").html(message);
  $("#inlineErrorMessagesTd").show();
}

function DisplayServerMessageInline()
{ 
	var err_id=$("#err_flag").val();
	var msg1= $("#err_msg").val();
	var post_type= $("#post_type").val();
	
	if(typeof(msg1)=="undefined")
		return;
	//msg1 = msg1.replace(/[\s\.\!\?\-\;\,\/\'\:\+\(\)]+/g, '');
	var msg = msg1.replace(/[\s\.\!\?\-\;\,\/\'\:\+\(\)]+/g, '');
	var variable =window[msg1];
        if(err_id =="undefined")
	    return;
	if(err_id=="1") /*Successful*/
	{
		if(typeof(variable)=="undefined")
	    msg= ERR_OK; /*TODO: Message*/
	 var message="<table><TR><TD><IMG src=\"image/Status_success_icon.png\" width=32 height=32></TD><TD><DIV>"+msg+"</DIV></TD></TR></table>";	
   $("#inlineErrorMessagesTd").html(message);
   $("#inlineErrorMessagesTd").removeClass("inlineErrorMessagesTd");
   $("#inlineErrorMessagesTd").addClass("inlineSuccessMessageTd");
   $("#inlineErrorMessagesTd").show();
	}
	else if(err_id=="2") /*Error*/
		{
		 		if(typeof(variable)=="undefined")
					msg = ERR_ERROR;
				else
					msg= variable;
	    	 var message="<table><TR><TD><IMG src=\"image/Status_criticalerror_icon.png\" width=32 height=32></TD><TD><DIV>"+msg+"</DIV></TD></TR></table>";	
        $("#inlineErrorMessagesTd").html(message);
        $("#inlineErrorMessagesTd").show(); 	
		}
/*Do nothing if error_id=0*/

/*if apply in child window and successful   refresh parent window*/
if(err_id=="1"&&post_type=="1"&&window.opener!=undefined) //
{
   var sessionid=new Date().getTime();
   var new_url="/cgi-bin/dispatcher.cgi?";
   var cmd;
   var old_url=window.opener.location.href;

   var cmd_start=old_url.indexOf("cmd=");
   if(cmd_start==-1)
     window.opener.location.reload(); //refresh parent window
    else
    { 
           var cmd_url=old_url.substring(cmd_start,old_url.length);
           var cmd_end=cmd_url.indexOf("&");
	   if(cmd_end!=-1)
	   {
		  if(cmd_url.indexOf("interface") != -1){
			if(cmd_url.indexOf("errCode") == -1){
				if(cmd_url.indexOf("#") != -1)
					cmd_end=cmd_url.indexOf("#");
				else if(cmd_url.indexOf("sessionid") != -1)
					cmd_end=cmd_url.indexOf("sessionid")-1;
			}
			else{
				cmd_end=cmd_url.indexOf("errCode")-1;
			}
		  }
		  
		  cmd= cmd_url.substring(0,cmd_end);
	   }	   
	   else if((cmd_end=cmd_url.indexOf("#"))!=-1)
	   {
	       cmd= cmd_url.substring(0,cmd_end);
	   }
	   else
	    cmd= cmd_url.substring(0,cmd_url.length);   
	    
	    window.opener.location.href=new_url+cmd+"&sessionid="+sessionid;
    }	
}
}
 
function jScriptValidateUiAndElementLevel(xid, value)
{
//default function
return null;
}

function jScriptValidateUiAndElementAction()
{
//default function
return null;
}
function xuiSubstituteValue(xv,eleName,value)
{
var tok;
if(xv.indexOf("$$objName$$") != -1)
{
var tmpElename = eleName.replace("<BR>", "").replace("<br>", "");
tok = xv.replace("$$objName$$",tmpElename);
xv = tok;
}
if(xv.indexOf("$$objValue$$") != -1)
{
tok = xv.replace("$$objValue$$",value);
}
else
{
tok = xv;
}
return tok;
}
function xuiDynamicError(xid,value,retval)
{

var eleName = xeData["xeleName_" + xid];
var buf = xuiSubstituteValue(retval, eleName, value);
xuiDisplayInlineErrorMsgPersistent(""+buf);
return false;
}
function ValidatePerElement(xid)
{
var xd = xeData["xd_" + xid];
if(xd == null)
{
  alert("Missing 'xd_'...!");
  return false; 
}
var span_entry=GetElementByXid(xid);
var inp =span_entry.getElementsByTagName("input"); 
var value=inp[0].value;

var attrib = xui_oem_getElemAttributeWithRetval(inp[0], 'disabled',true);
   if(attrib == true || attrib == "disabled") return true;

var retval = jScriptValidate(xd,value);  

if(retval!=null) {
	var variable =window[retval];
  if(typeof(variable) == "undefined")
     retval = ERR_INPUT_FORMAT; //NLS_COMMON_ERR_MSG
 return xuiDynamicError(xid,value,retval); 
}
// we need to perform UI and element level validations
 retval = jScriptValidateUiAndElementLevel(xid,value);
 if(retval!=null) {
	var variable =window[retval];
  if(typeof(variable) == "undefined")
    variable = ERR_INPUT_FORMAT; //NLS_COMMON_ERR_MSG
 return xuiDynamicError(xid, value, variable);
 }
// perform min-max validations
retval = jScriptValidateMinMax(xd, value, xid);

if(retval!=null) {
	var variable =window[retval];
  if(typeof(variable) == "undefined")
  retval = ERR_INPUT_RANGE; //NLS_COMMON_RANGE_ERR_MSG 
  return xuiDynamicError(xid, value, retval); 
}

// perform string length validations
retval = jScriptValidateStringLength(xd, value, xid);

if(retval!=null) {
	var variable =window[retval];
  if(typeof(variable) == "undefined")
  retval = Exceedrecommendedmaximumstringlength; //NLS_COMMON_RANGE_ERR_MSG 
  return xuiDynamicError(xid, value, retval); 
}

return true;
}
function xuiValidateElements(xid)
{
   if(xid == null)
   {
     return false;
   }
   var valList = xeData["xbScope_" + xid];
   if(valList == null)
   {
     return true;/*No any need to check*/
   }
   for(var i=0; i<valList.length; i++)
   {
     if(ValidatePerElement(valList[i])==false)
       return false;  
   }
   return true;	
} 

function xuiValidateElementsAction()
{
 var retval;
// we need to perform UI and element level validations
 retval = jScriptValidateUiAndElementAction();
 if(retval!=null) {
	var variable =window[retval];
  if(typeof(variable) == "undefined")
     variable = HeaderSel[inex].Error_Prom;//ERR_INPUT_FORMAT; //NLS_COMMON_ERR_MSG
     xuiDisplayInlineErrorMsgPersistent(variable);
     return false;
  }
   return true;
}
/********************************************valid check END***********************/

function onclickSubmit(ent,submitFlag, xid)
{

   var attrib = xui_oem_getElemAttributeWithRetval(ent, 'disabled',true);
   if(attrib == true || attrib == "disabled") return false;

  //check input vlue valid and show the error message
  
   if(xuiValidateElements(xid)==false)
      return;    
   
   if(xuiValidateElementsAction()==false)
      return;    
   
  //Run special js function,if 
     var jsfunc = xeData["js_" + xid + "_" + "button"];
   if(jsfunc!=null)
       xuiRunJsRoutine(jsfunc); 

if(window.opener!=undefined)
	window.opener.top.document.PopWindowClose=1;
   
	var tmpapplyfunc=applyfunc["0"];
   window[tmpapplyfunc]();
  // document.forms[0].submit();	//we don't need this when using asp 
}

function SubmitCheck(ent, submitFlag, xid)
{
   var attrib = xui_oem_getElemAttributeWithRetval(ent, 'disabled',true);
   if(attrib == true || attrib == "disabled") return false;

  /*check input vlue valid and show the error message*/
  
   if(xuiValidateElements(xid)==false)
      return false;    
   
   if(xuiValidateElementsAction()==false)
      return false;    
   
  /*Run special js function,if */ 
     var jsfunc = xeData["js_" + xid + "_" + "button"];
   if(jsfunc!=null)
       xuiRunJsRoutine(jsfunc); 

	var tmpapplyfunc=applyfunc["0"];
    window[tmpapplyfunc]();

if(window.opener!=undefined)
	window.opener.top.document.PopWindowClose=1;

	return true;
}

function SubmitCheckmultiform(ent,submitFlag, xid, num)
{

   var attrib = xui_oem_getElemAttributeWithRetval(ent, 'disabled',true);
   if(attrib == true || attrib == "disabled") return false;
  	
  /*check input vlue valid and show the error message*/
  
   if(xuiValidateElements(xid)==false)
      return;    
  /*Run special js function,if */ 
     var jsfunc = xeData["js_" + xid + "_" + "button"];
   if(jsfunc!=null)
       xuiRunJsRoutine(jsfunc);
   
   document.forms[num].submit();	 
}

//Add by Wolf
function onclickLink(ent,cmd,xid)
{
	var attrib = xui_oem_getElemAttributeWithRetval(ent, 'disabled',true);
	if(attrib == true || attrib == "disabled") return false;
	
	if(xuiValidateElements(xid)==false)
		return;
	/* Run special js function,if */ 
     var jsfunc = xeData["js_" + xid + "_" + "button"];
	if(jsfunc!=null)
       xuiRunJsRoutine(jsfunc);
	
	var mypage="/cgi-bin/dispatcher.cgi?cmd="+cmd;
	window.location.href = mypage;
}

function navigationRedirection(arg)
{
  if(typeof(top.frames["treefrm"]) != "undefined")
  {
    window.parent.top.frames["treefrm"].open_page(arg);
  }
  return true;
}
function TaobaoCheckBoxSelectAll(args, xid, ent, action_args)
{
  var Ids = args.split("|");
  var buttons = new Array();
  var i, max, row;
  var selected = Ids[0];
  var enableCount = 0;
  var visibleCount = 0;

  /*This to escape enabling of buttons when table search result is zero*/
  if($('.dataTables_empty').length != 0)
  { 
    return;
  }
  for(i = 0, max = Ids.length; i < max; i++) {
    buttons[i] = GetElementByXid(Ids[i]);
  }

  var selectors = GetElementsByXid(xid);/*all span entry*/
  if(selectors.length <= 1) return;
  

  var span_e = selectors[0].getElementsByTagName("input");
  var gselector = span_e[0];

  for(i = 1, max = selectors.length; i < max; i++ ) 
  {
    var ent1 = selectors[i].getElementsByTagName("input");
    var ent2 =ent1[0];
    if(ent2.type == "checkbox") 
	  {
	    row = xuiGetParent (ent2, 'tr');
	    if(row.style.display != "none")
	   {
	    //var inp = row.firstElementChild.firstElementChild;
      var span = $($(row).children(":first")[0]).children(":first")[0];
      var tmp_inp =span.getElementsByTagName("input");
      var inp=tmp_inp[0];

      if(inp.disabled == false)
	    {
          ent2.checked = gselector.checked;
          row = xuiGetParent (ent2, 'tr');
          NikolaSelectRow(row, ent2, ent2.checked);
         // NikolaEnableRowHidden(row, ent2.checked);
          if(row.style.display == "none") 
          {
		  	    continue;
          }
          visibleCount++;
          if(ent2.checked) 
		       {
            enableCount = enableCount + 1;
          }
        }
      }
	 }
  }

  // enable/disable the buttons
  for(i = 0, max = buttons.length; i < max; i++) {
    if(enableCount == 0) {
      xui_oem_disable(buttons[i]);
    }  else if(enableCount == 1) {
      xui_oem_enable(buttons[i]);
    } else {
      xui_oem_disable(buttons[i]);
    }
  }
  if(action_args != "")
  {
     //CheckBoxSelectAllValueBasedButtonAction(action_args,selectors[0], 0);
     ValueBasedButtonAction(action_args, selectors,enableCount);
  }
}
function TaobaoCheckBoxSelect(args, xid, ent, action_args)
{
  var Ids = args.split("|");
  var buttons = new Array();
  var i, max, row;
  var selected = Ids[0];
  var enableCount = 0;
  var visibleCount = 0;

  if(action_args != "")
  {
    var action_ids = action_args.split("|");
    var element_Id = GetElementsByXid(action_ids[0]);
    var value = action_ids[1];
  }

  for(i = 0, max = Ids.length; i < max; i++) {
    buttons[i] = GetElementByXid(Ids[i]);
  }

  var selectors = GetElementsByXid(xid);
  if(selectors.length <= 1) return;
  
  var span_e = selectors[0].getElementsByTagName("input");
  var gselector = span_e[0];
  var HiddenCheckEnableCount = 0;

  for(i = 1, max = selectors.length; i < max; i++ ) {  /* i=1 Skip Select All checkbox*/
    var ent1 = selectors[i].getElementsByTagName("input");
    var ent2 =ent1[0];
    if(ent2.type != "checkbox") continue;
    row = xuiGetParent (ent2, 'tr');
    if(row == undefined || row.style == "undefined") continue;
    if(row.style.display == "none") continue;
    visibleCount++;
    if((ent2.checked) || (ent2.style.display == "none")) {
      enableCount = enableCount + 1;
    }

    if(ent2.style.display == "none") {
      HiddenCheckEnableCount++;
    }
  }

  // check the current entry
  row = NikolaSelectRow(undefined, ent, ent.checked);
//  NikolaEnableRowHidden(row, ent.checked);

  //check if we need to check/uncheck global checkbox
  if(enableCount >= visibleCount) {
    gselector.checked = true;  
  } else {
    gselector.checked = false;  
  }

  // enable/disable the buttons
  for(i = 0, max = buttons.length; i < max; i++) {
    if(enableCount == 0) {
      xui_oem_disable(buttons[i]);
    }  else if(enableCount == 1) {
      xui_oem_enable(buttons[i]);
    } else if((enableCount-HiddenCheckEnableCount) == 1) {
      xui_oem_enable(buttons[i]); 
    } else {
      xui_oem_disable(buttons[i]);
    }
  }
  if(action_args != "") 
  {
    ValueBasedButtonAction(action_args, selectors,enableCount);
  }
}

function TaobaoRadioSelect(args, xid, ent)
{
  var Ids = args.split("|");
  var buttons = new Array();
  var i, max, row;
  var selected = Ids[0];

  for(i = 0, max = Ids.length; i < max; i++) {
    buttons[i] = GetElementByXid(Ids[i]); 
  }

  var selectors = GetElementsByXid(xid);/*return multi span obj*/

  for(i = 0, max = selectors.length; i < max; i++) {

    var ent1 = selectors[i].getElementsByTagName("input");
    var ent2 =ent1[0];
    if(ent2.type == "radio") {

      ent2.checked = false;
      NikolaSelectRow(undefined, ent2, ent2.checked);
    }
  }

  // check the current entry as we have unchecked all
  ent.checked = true;
  row = NikolaSelectRow(undefined, ent, ent.checked);
 // NikolaEnableRowHidden(row, ent.checked);

  // enable the buttons
  for(i = 0, max = buttons.length; i < max; i++) {
    xui_oem_enable(buttons[i]);
  }
}

function CheckSelectRadioValue(xid,val)
{
var save_action=GetElementByXid(xid);
var inp =save_action.getElementsByTagName("input");
for(var i=0;i<inp.length;i++)
{
if(inp[i].checked ==true)
{
if(inp[i].value == val)
{
return 0;
}
else
return 1;
}
}
}

function GetVlaueFromSelectID(xid)
{
     var i, max;
     var selectors = GetElementsByXid(xid);/*return multi span obj*/
     for(i = 0, max = selectors.length; i < max; i++) {
     var ent1 = selectors[i].getElementsByTagName("input");
     var ent2 =ent1[0];
     if(i==0&&ent2.type=="checkbox")
         continue;
     if(ent2.checked ==true) {
          return ent2.value;
    }
  }
}
function GetVlaueFromCombo(xid)
{
  var span_entry=GetElementByXid(xid);  
	var selector =span_entry.getElementsByTagName("SELECT"); 
	var  combo=selector[0]; 
  for ( var i = 0; i < combo.length; i++ )
  {
    if( combo[i].selected )
    {
      return combo[i].value;
    }
  }	
}
/**********************************************************************************************/
function load_default_action()
{
  	 	var span_elements = document.getElementsByTagName('SPAN'); 
  	 	   	 		
  	 	for(var i=0;i<span_elements.length;i++)
  	 	{
  	 		var spanEnt=span_elements[i];
  	 		var xid=spanEnt.getAttribute("xid"); 
  	 		if(xid==null)
  	 		   continue;  

        for(var j=0;j<spanEnt.childNodes.length;j++)
        {
         if(spanEnt.childNodes[j].nodeType!=1)
           continue;
         else
         	{
           var visent = spanEnt.childNodes[j];
           break;
          }
        }  	 	  
  	 		var xc = xeData["xc_" + xid];
  	 		switch(xc)
  	 		{
  	 			case valmap.button:
  	 			  set_button_default_action(xid,spanEnt); /*Button maybe default disabled*/
          break;
          
          case valmap.label:
          case valmap.plain_label:
         // xuiProcessJSActions(ent,xid, ent.name,ent.value,exec);
          break;
          case valmap.textbox:
         // xuiProcessJSActions(ent,xid, ent.name,visent.value, exec);
          break;
          case valmap.textarea:
         // xuiProcessJSActions(ent,xid, ent.name,visent.value, exec);
          break;
          case valmap.passwdbox:
        //  xuiProcessJSActions(ent,xid, ent.name,ent.value, exec);
          break;
          case valmap.vradio:
          case valmap.vradio_none:
          case valmap.vradio_left:
          case valmap.radio:
          case valmap.radio_none:
          case valmap.radio_left:
          var node = visent;
          while(node != null)
          {
          if(node.checked == true)
          {  
          xuiProcessJSActions(spanEnt,xid, node.value);
          break;
          }
          node = node.nextSibling;
          }
          break;
          case valmap.combo:

         xuiProcessJSActions(spanEnt,xid,"All");
          for(var i=0;i<visent.length;i++)//select
          	{
          	   if( visent[i].selected )
			xuiProcessJSActions(spanEnt,xid,visent[i].value);
          	}
          //var tmp_val = ent.value;
          //ev = getAllWebEnums(xid);
          //if(ev != null && visent.selectedIndex >= 0 &&
          //visent.selectedIndex < ev.length) {
          //tmp_val = ev[visent.selectedIndex];
          //}
          //xuiProcessJSActions(ent,xid,ent.name,tmp_val,exec);
          break;
          case valmap.combo_multiselect:
          //xuiProcessMultiSelectActions(ent,xid, ent.name, visent.options, exec);
          break;
          case valmap.checkbox:
          if(visent.checked == true)
          {
          xuiProcessJSActions(spanEnt,xid, "Enable");
          }
          else
          {
          xuiProcessJSActions(spanEnt,xid, "Disable");
          }
          break;
          case valmap.checkboxNeg:
          //ev = getAllWebEnums(xid);
          //if(ev == null)
          //{
          //break;
          //}
          //if(ent.value == ev[0])
          //{
          //xuiProcessJSActions(ent,xid, ent.name, ev[0], exec);
          //}
          //else
          //{
          //xuiProcessJSActions(ent,xid, ent.name, ev[1], exec);
          //}
          break;
          case valmap.url:
          break;
          case valmap.url_list:
          //node = visent.childNodes[0].childNodes[0]; //TR
          //node = node.childNodes[0] ; //TD
          //while(node != null){
          //if(node.childNodes[0].getAttribute("clicked") != null){
          //xuiProcessJSActions(node.childNodes[0],xid, ent.name,node.childNodes[0].getAttribute("value") ,exec);
          //break;
          //}
          //node = node.nextSibling;
          //}
          break;
          case valmap.file:
          break;
          case valmap.hidden:
          //xuiProcessJSActions(ent,xid, ent.name,ent.value, exec);
          break;
          case valmap.none:
          break;
          default: /*Unkown control type*/            
          break;
          }  	 			
  	 	}
             
        
  NikolaHRLabelPadding();
  NikolaIEBrowserPadding(); 
  
  var userJS = (typeof(xeData.userJS) != "undefined") ? xeData.userJS : null;
   if(userJS != null) {
     xuiRunJsRoutine(userJS);
   }

  if(window.opener!=undefined)
  {
	window.opener.top.document.PopWindowClose=0;
	window.opener.top.document.getElementById("windowopen").style.display="block";
  }

  DisplayServerMessageInline();  
}

$(document).ready(function(){
  	  	  	 	
  	load_default_action();
  	/*Page Table Control*/
  	Taobao_DoTableSortOrPage();
  }); 

