<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<script language=JavaScript>
	pageTitle();


var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var cab_attr = new Array(<% CGI_CLT_CABLE_PARAM();%>);

/*var cab_attr = new Array('2', // cable num
 '1;60;1', // cab0 snid, refresh period, anonymous control 
 '2;60;1' //cab 1
 );
*/

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

function check_all_input()
{
	return true;
}


function init()
{
	var f=document.forms[0];
	f.anonymous_ctrl.value = get_anonymous_control();
}


function apply()
{
	var f=document.forms[0];
	if (!check_all_input()){
		return;	
	}
	diffCfg("anonymous", "anonymous", f.anonymous_ctrl.value);
	diffCfg("refresh", "refresh", get_refresh_interval());
	for (var c = 0; c < get_cable_num(); c ++ ){
		diffCfg("snid_cab"+c, "snid_cab"+c, get_cable_snid(c));
	}	
	subForm(f,'goform/command','CAB_PROP',cPage);
}

</script>
</head>

<body onload="init();">
<script>pageHead(webVars);</script>
<form name=user_anon action=goform/command method=post>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1
	class=box_tn>
	<tr>
		<td class=bgblue>Access Control</td>
		<td class=bggrey>
			<select name=anonymous_ctrl>
			<option value=0>Forbidden</option>
			<option value=1>Allowed</option>
			</select>
		</td>
	</tr>
</table>
<p>
<input type=button value="  OK  " onclick=apply()>&nbsp;&nbsp;&nbsp;&nbsp;
<input type=button value="Cancel" onclick=init()>
</p>
</form>
<script>pageTail();</script>
</body>
</html>
