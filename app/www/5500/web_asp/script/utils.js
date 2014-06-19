/* 
	Copyright 2013-2020, 
	Shenzhen Hexicom Technologies Co., Ltd. 
	All rights reserved.
	Author: Einsn Liu (liuchuansen@hexicomtech.com)
	Date: 2013-03-25 
*/

var cookie_dontShowDoCMDPage = "dontShowDoCMDPage";
var url_system_status = "system_status.asp"

function current_page()
{
    var page = window.location.toString().replace(/.*\//,'');   
    return page.replace(/\?.*/,'');
}

function GURL(x)
{
    location=x;
}
function string_trim(str)
{
   return str.replace(/(^\s*)|(\s*$)/g,"");
}


var timeTable=[
[0,"-12:00","Eniwetok, Kwajalein"],
[1,"-11:00","Midway Island, Samoa"],
[2,"-10:00","Hawaii"],
[3,"-9:00","Alaska"],
[4,"-8:00","Pacific Time (US &amp; Canada)"],
[5,"-7:00","Mountain Time (US &amp; Canada)"],
[6,"-6:00","Central Time (US &amp; Canada), Mexico City"],
[7,"-5:00","Eastern Time (US &amp; Canada), Bogota, Lima"],
[8,"-4:00","Atlantic Time (Canada), Caracas, La Paz"],
[9,"-3:30","Newfoundland"],
[10,"-3:00","Brazil, Buenos Aires, Georgetown"],
[11,"-2:00","Mid-Atlantic"],
[12,"-1:00","Azores, Cape Verde Islands"],
[13,"+0:00","Western Europe Time, London, Lisbon, Casablanca"],
[14,"+1:00","Brussels, Copenhagen, Madrid, Paris"],
[15,"+2:00","Kaliningrad, South Africa"],
[16,"+3:00","Baghdad, Riyadh, Moscow, St. Petersburg"],
[17,"+3:30","Tehran"],
[18,"+4:00","Abu Dhabi, Muscat, Baku, Tbilisi"],
[19,"+4:30","Kabul"],
[20,"+5:00","Ekaterinburg, Islamabad, Karachi, Tashkent"],
[21,"+5:30","Bombay, Calcutta, Madras, New Delhi"],
[22,"+5:45","Kathmandu"],
[23,"+6:00","Almaty, Dhaka, Colombo"],
[24,"+7:00","Bangkok, Hanoi, Jakarta"],
[25,"+8:00","Beijing, Perth, Singapore, Hong Kong"],
[26,"+9:00","Tokyo, Seoul, Osaka, Sapporo, Yakutsk"],
[27,"+9:30","Adelaide, Darwin"],
[28,"+10:00","Eastern Australia, Guam, Vladivostok"],
[29,"+11:00","Magadan, Solomon Islands, New Caledonia"],
[30,"+12:00","Auckland, Wellington, Fiji, Kamchatka"]];


function genTimeOpt()
{
	var s='';
	for (i=0; i<timeTable.length; i++)
	{
		var t=timeTable[i];
		s+='<option value='+t[0]+'> (GMT'+t[1]+') '+t[2]+'</option>\n';
	}
	document.write(s);
}

function getTimeZoneOffsetByIndex(v)
{
	var t;
	for (var i=0; i<timeTable.length; i++)
	{
		var t = timeTable[i];
		if (t[0] == v){
			var vs = t[1].split(':');
			var v = (vs.length > 0) ? parseInt(vs[0], 10) : 0;
			var m = (vs.length > 1) ? parseInt(vs[1], 10) : 0;
			if (t[1].charAt(0) == '-'){
					return -1 * ((v * 60 * 60) + m * 60);
			}else {
					return ((v * 60 * 60) + m * 60);	
			}
		}
	}
	return null;
}

function show_month_option()
{
	var str=new String("");
	var i;

	for(i=1;i<=12;i++)
	{
		str="";
		str+="<option value="+i+">"+month[i-1]+"</option>";
		document.write(str);
	}
}
function show_day_option()
{
	var str=new String("");
	var i;

	for(i=1;i<=31;i++)
	{
		str="";
		str+="<option value="+i+">"+ (i<10 ? "0" : "") +i+"</option>";
		document.write(str);
	}
}


/* to be removed */

function combinIP(d1,d2,d3,d4)
{
    var ip=d1.value+"."+d2.value+"."+d3.value+"."+d4.value;
    if (ip=="...")
        ip="";
    return ip;
}

function combinMAC(m1,m2,m3,m4,m5,m6)
{
    var mac=m1.value+":"+m2.value+":"+m3.value+":"+m4.value+":"+m5.value+":"+m6.value;
    if (mac==":::::")
        mac="";
    return mac;
}



function decomMAC2(ma,macs,nodef)
{
    var re = /^[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}$/;
    if (re.test(macs)||macs=='')
    {
		if (ma.length!=6)
		{
			ma.value=macs;
			return true;
		}
	if (macs!='')
        	var d=macs.split(":");
	else
		var d=['','','','','',''];
        for (i = 0; i < 6; i++)
		{
            ma[i].value=d[i];
			if (!nodef) ma[i].defaultValue=d[i];
		}
        return true;
    }
    return false;
}

function decomIP2(ipa,ips,nodef)
{
    var re = /^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$/;
    if (re.test(ips))
    {
        var d =  ips.split(".");
        for (i = 0; i < 4; i++)
		{
            ipa[i].value=d[i];
			if (!nodef) ipa[i].defaultValue=d[i];
		}
        return true;
    }
    return false;
}

function combinIP2(d)
{
	if (d.length!=4) return d.value;
    var ip=d[0].value+"."+d[1].value+"."+d[2].value+"."+d[3].value;
    if (ip=="...")
        ip="";
    return ip;
}
function combinMAC2(m)
{
    var mac=m[0].value+":"+m[1].value+":"+m[2].value+":"+m[3].value+":"+m[4].value+":"+m[5].value;
	mac=mac.toUpperCase();
    if (mac==":::::")
        mac="";
    return mac;
}


function Cfg(i,n,v)
{
	this.i=i;
    this.n=n;
    this.v=this.o=v;
}

var CA = new Array() ;

function addCfg(n,i,v)
{
	CA.length++;
    CA[CA.length-1]= new Cfg(i,n,v);
}

function newCfg(n,i,v)
{
	CA.length++;
  CA[CA.length-1]= new Cfg(i,n,v);
  CA[CA.length-1].o='';  
}

function idxOfCfg(kk)
{
    if (kk=='undefined') { alert("undefined"); return -1; }
    for (var i=0; i< CA.length ;i++)
    {

        if ( CA[i].n != 'undefined' && CA[i].n==kk )
            return i;
    }
    return -1;
}

function getCfg(n)
{
	var idx=idxOfCfg(n)
	if ( idx >=0)
		return CA[idx].v ;
	else
		return "";
}

function setCfg(n,v)
{
	var idx=idxOfCfg(n)
	if ( idx >=0)
	{
		CA[idx].v = v ;
	}
}

function diffCfg(n,i,v)
{
	var idx=idxOfCfg(n)
	if ( idx >=0)
	{
		CA[idx].i = i ;
		CA[idx].v = v ;
	}else newCfg(n, i, v);
}


function cfg2Form(f)
{
    for (var i=0;i<CA.length;i++)
    {
        var e=eval('f.'+CA[i].n);
        if ( e )
		{
			if (e.name=='undefined') continue;
			if ( e.length && e[0].type=='text' )
			{
				if (e.length==4) decomIP2(e,CA[i].v);
				else if (e.length==6) decomMAC2(e,CA[i].v);
			}
			else if ( e.length && e[0].type=='radio')
			{
				for (var j=0;j<e.length;j++)
					e[j].checked=e[j].defaultChecked=(e[j].value==CA[i].v);
			}
			else if (e.type=='checkbox')
				e.checked=e.defaultChecked=Number(CA[i].v);
			else if (e.type=='select-one')
			{
                                e.value=CA[i].v;
				for (var j=0;j<e.options.length;j++)
					 e.options[j].selected=e.options[j].defaultSelected=(e.options[j].value==CA[i].v);
			}
			else
				e.value=getCfg(e.name);
			if (e.defaultValue!='undefined')
				e.defaultValue=e.value;
		}
    }
}

var frmExtraElm='';
function form2Cfg(f)
{

    for (var i=0;i<CA.length;i++)
    {
        var e=eval('f.'+CA[i].n);
		if ( e )
		{
			if (e.disabled) continue;
			if ( e.length && e[0].type=='text' )
			{
				if (e.length==4) CA[i].v=combinIP2(e);
				else if (e.length==6) CA[i].v=combinMAC2(e);
			}
			else if ( e.length && e[0].type=='radio')
			{
				for (var j=0;j<e.length;j++)
					if (e[j].checked) { CA[i].v=e[j].value; break; }
			}
			else
			if (e.type=='checkbox')
				setCfg(e.name, Number(e.checked) );
			else
				setCfg(e.name, e.value);
		}
    }
}

var OUTF;
function frmHead(na,to,cmd,go)
{
	OUTF="<FORM name="+na+" action="+to+" method=POST>\n"+
	"<INPUT type=hidden name=CMD value="+cmd+">\n"+
	"<INPUT type=hidden name=GO value="+go+">\n";
}

function frmEnd()
{
	OUTF+="</FORM>\n";
}

function frmAdd(n,v)
{
	set1="<input type=hidden name="+n+" value=\"";
	v=v.replace(/\"/g,"&quot;");
	var r=new RegExp(set1+".*\n","g");
	if (OUTF.search(r) >= 0)
		OUTF=OUTF.replace(r,(set1+v+"\">\n"));
	else
		OUTF += (set1+v+"\">\n");
}

function genForm(n,a,d,g,u)
{
	frmHead(n,a,d,g);
	var sub=0;
    for (var i=0;i<CA.length;i++)
	{
			if (u && (CA[i].v == CA[i].o)){
				continue;
			}
			frmAdd("SET"+sub,String(CA[i].i)+"="+CA[i].v);
			sub++;
	}
	if (frmExtraElm.length)
		OUTF+=frmExtraElm;
	frmExtraElm=''; //reset
	frmEnd();
	return OUTF;
}

function _subForm(f1,a,d,g,u)
{
	var msg=genForm('OUT',a,d,g,u);

	 //Debug option
  //if (!confirm(msg)) return;
 
	var newElem = document.createElement("div");
	newElem.innerHTML = msg ;
	f1.parentNode.appendChild(newElem);
	f=document.OUT;
	f.submit();
}

function subForm(f1,a,d,g)
{
	_subForm(f1,a,d,g,0);
}

function subFormChanges(f1,a,d,g)
{
	_subForm(f1,a,d,g,1);
}

function geturlparam()
{
var s=window.location.href;
var start=s.indexOf("?v=");
var end=s.length;
var str=s.substring(start,end);
str=str.substring(3);
return str;

}

function QueryString(qry)
{
	var parm = geturlparam();
	if ( parm.indexOf("?") > -1 ) {
//		var strQueryString = parm.substr(parm.indexOf("?") + 1).toLowerCase();
		var strQueryString = parm.substr(parm.indexOf("?") + 1);
		var s = strQueryString.split('&');
		for(var i = 0; i < s.length; i ++) {
			var s1 = s[i].split('=');
			if(s1[0] == qry && qry != "GO") return s1[1];
			if(s1[0] == "GO") return parm.substr(parm.indexOf(qry) + 3);
		}
	}
	return '';
}

function gotoUrl(url)
{
	location = url;
}


function getBackground(element)  
{//author: Longbill (www.longbill.cn)  
//dnew.cn修补  
  var rgbToHex=function(rgbarray,array){  
      if (rgbarray.length < 3) return false;  
      if (rgbarray.length == 4 && rgbarray[3] == 0 && !array) return 'transparent';  
      var hex = [];  
      for (var i = 0; i < 3; i++){  
        var bit = (rgbarray[i] - 0).toString(16);  
        hex.push((bit.length == 1) ? '0' + bit : bit);  
      }  
      return array ? hex : '#' + hex.join('');  
    }  
//---------------  
  if (typeof element == "string") element = document.getElementById(element);  
  if (!element) return;  
  cssProperty = "backgroundColor";  
  mozillaEquivalentCSS = "background-color";  
  if (element.currentStyle)  
    var actualColor = element.currentStyle[cssProperty];  
  else  
  {  
    var cs = document.defaultView.getComputedStyle(element, null);  
    var actualColor = cs.getPropertyValue(mozillaEquivalentCSS).match(/d{1,3}/g);  
//-----  
    actualColor = (actualColor) ? rgbToHex(actualColor) : "transparent";  
  }  
  if (actualColor == "transparent" && element.parentNode)  
    return arguments.callee(element.parentNode);  
  if (actualColor == null)  
    return "#ffffff";  
  else  
    return actualColor;  
}  


function GetVal(strArray, primary, index)
{	
	if (strArray.length > 0)
	{
		var vs = strArray[primary].split(';');
		if (vs.length > 0) 
		{	
			return vs[index];
		}
	}
	return false;
}


function setCookie(name,value)//两个参数，一个是cookie的名子，一个是值
{
    var Days = 300; //此 cookie 将被保存 30 天
    var exp  = new Date();    //new Date("December 31, 9998");
    exp.setTime(exp.getTime() + Days*24*60*60*1000);
    document.cookie = name + "="+ escape (value) + ";expires=" + exp.toGMTString();
}
function getCookie(name)//取cookies函数       
{
    var arr = document.cookie.match(new RegExp("(^| )"+name+"=([^;]*)(;|$)"));
     if(arr != null) return (arr[2]); return null;
}
function delCookie(name)//删除cookie
{
    var exp = new Date();
    exp.setTime(exp.getTime() - 1);
    var cval=getCookie(name);
    if(cval!=null) document.cookie= name + "="+cval+";expires="+exp.toGMTString();
}


function get_cookie(name_to_get) {
var cookie_pair
var cookie_name
var cookie_value
var cookie_array = document.cookie.split("; ")
for (var counter = 0; counter < cookie_array.length; counter++) {
cookie_pair = cookie_array[counter].split("=")
cookie_name = cookie_pair[0]
cookie_value = cookie_pair[1]
if (cookie_name == name_to_get) {
return unescape(cookie_value)
}
}
return null
}
function delete_cookie(cookie_name) {
//set_cookie(cookie_name,"",-1)
set_cookie(cookie_name,"",null,"/");
}

function delete_temporary_cookie(cookie_name) {
//set_cookie(cookie_name,"",-1)
set_cookie(cookie_name,"","-1","/");
}

function set_cookie(cookie_name, cookie_value, cookie_expire, cookie_path, cookie_domain, cookie_secure) {
var expire_string;
var cookie_string = cookie_name + "=" + cookie_value;
if (cookie_expire)
{
if( typeof(cookie_expire)!="string")
{
expire_string=cookie_expire.toGMTString();
}
else{
var expire_date = new Date();
var ms_from_now = cookie_expire * 24 * 60 * 60 * 1000;
expire_date.setTime(expire_date.getTime() + ms_from_now)
expire_string = expire_date.toGMTString();
}
cookie_string += "; expires=" + expire_string;
}
if (cookie_path) {
cookie_string += "; path=" + cookie_path;
}
if (cookie_domain) {
cookie_string += "; domain=" + cookie_domain;
}
if (cookie_secure) {
cookie_string += "; true"
}
top.document.cookie = cookie_string;
}
