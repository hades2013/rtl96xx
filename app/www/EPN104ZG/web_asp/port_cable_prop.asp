<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<script language=JavaScript>
var str_copyright =<%CGI_GET_COPYRIGHT();%>
var PROD_MODEL = <%CGI_GET_MODEL();%>
</SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<script language=JavaScript>

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var cab_attr = new Array(<% CGI_CLT_CABLE_PARAM();%>);
/*
var cab_attr = new Array('2', // cable num
 '1;60;1', // cab0 snid, refresh period, anonymous control 
 '2;60;1' //cab 1
 );
 */
pageTitle();

function get_cable_num()
{
	if (cab_attr.length > 0){
		return cab_attr[0];
	}
	return '0';	
}

function get_cable_snid(cab)
{
	var vs;
	if (cab_attr.length > (1 + cab)){
			vs = cab_attr[1 + cab].split(';');
			if (vs.length > 0){
				return vs[0];	
			}
	}
	return '1';
}

function get_refresh_interval()
{
	var cab = 0; // use 0
	var vs;
	if (cab_attr.length > (1 + cab)){
			vs = cab_attr[1 + cab].split(';');
			if (vs.length > 1){
				return vs[1];	
			}
	}
	return '60';	
}

function get_anonymous_control()
{
	var cab = 0; // use 0
	var vs;
	if (cab_attr.length > (1 + cab)){
			vs = cab_attr[1 + cab].split(';');
			if (vs.length > 2){
				return vs[2];	
			}
	}
	return '1';	
}


function select_option(i, s)
{
	return "<option value=" + i + " " + s ? "selected" : "" +  "> " + i +"</option>";
}
	
function snid_selects(cab, val)
{
var	str =  "<tr> " + 
				"<td class=bgblue>Cable " + (cab + 1) + " SNID</td>" + 
				"<td class=bggrey> " + 
					"<select name=snid_cab" + (cab) + " id=snid_cab" + cab + "> " + 
					"<option value=1 " + ((val == 1) ? "selected" : "") +  "> " + "1</option>" +
					"<option value=2 " + ((val == 2) ? "selected" : "") +  "> " + "2</option>" +
					"<option value=3 " + ((val == 3) ? "selected" : "") +  "> " + "3</option>" +
					"<option value=4 " + ((val == 4) ? "selected" : "") +  "> " + "4</option>" +
					"<option value=5 " + ((val == 5) ? "selected" : "") +  "> " + "5</option>" +
					"<option value=6 " + ((val == 6) ? "selected" : "") +  "> " + "6</option>" +
					"<option value=7 " + ((val == 7) ? "selected" : "") +  "> " + "7</option>" +
					"<option value=8 " + ((val == 8) ? "selected" : "") +  "> " + "8</option>" +
					"</select>" + 
				"</td>" + 
			"</tr>";	
	return str;
}

function cable_snid_selects() 
{
	var str = "";
	for (var c = 0; c < get_cable_num(); c ++ ){
			str += snid_selects(c, get_cable_snid(c));
	}
	return str;
}

function in_range(v, b, t)
{
	if ((v <= t) && (v >= b))	{
		return true;	
	}
	return false;
}

function check_all_input()
{
	var f=document.forms[0];
	if (!in_range(f.interval.value, 30, 180)){
		alert("错误的刷新间隔,请重新输入.");
		return false;	
	}	
	return true;
}


function init()
{
	var f=document.forms[0];
	f.interval.value = get_refresh_interval();
}


function apply()
{
	var f=document.forms[0];
	if (!check_all_input()){
		return;	
	}
	diffCfg("anonymous", "anonymous", get_anonymous_control());// no change
	diffCfg("refresh", "refresh", f.interval.value);
	for (var c = 0; c < get_cable_num(); c ++ ){
		diffCfg("snid_cab"+c, "snid_cab"+c, document.getElementById('snid_cab' + c).value);
	}	
	subForm(f,'goform/command','CAB_PROP',cPage);
}

</script>
</head>

<body onload=init()>
<script>pageHead(webVars,PROD_MODEL);</script>
<form name=system_man_ip action=goform/command method=post>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1
	class=box_tn>
	<tr>
		<td class=bggrey2 align=center><font size=4>Cable 端口</font></td>
	</tr>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1
			bgcolor="#FFFFFF">
			<tr>
				<td class=bgblue>网络刷新间隔</td>
				<td class=bggrey><input name="interval" size=7 maxlength=7></input> S</td>
			</tr>
			<script> document.write(cable_snid_selects()) </script>					
		</table>
		</td>
	</tr>
</table>
<p>
<input type=button value=确认 onclick=apply()>
<input type=button value=取消 onclick=init() >
</p>
</form>

<script>pageTail(str_copyright);</script>
</body>
</html>
