<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=pageview.js></SCRIPT>
<script language=JavaScript>
	pageTitle();

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var supdev = new Array(<% CGI_GET_DEVINFO(); %>);
var tmpl_list = new Array(<% CGI_GET_TEMPLATES("brief"); %>);

function dev_string(index)
{
	return supdev[index].split(';')[0];
}
function tmpl_edit(index)
{
	var cl = tmpl_list[index].split(';');
	location='service_template_prop.asp?action=edit&tmpl_id=' + cl[0];
}

function new_tmpl()
{
	location='service_template_prop.asp?action=new';
}

function apply()
{
	var f = document.forms[0];
	for(var i = 0; i < tmpl_list.length; i ++)
		if(pageview_ischecked(i)) {
			if(i < supdev.length) {
				alert('Defalut template could not be deleted!');
				for(var j = 0; j < tmpl_list.length; j ++) {
					pageview_clearselected(j);
					checkbox_click(j);
				}
				return 0;
			}
			diffCfg('tmpl_' + i, 'tmpl_del', i);
		}
	subForm(f,'goform/command', "TMPL_DEL", cPage);
}

function apply_tmpl_supp()
{
	var f = document.forms[0];
	setCfg('TMPL_DIS', f.template_disable.value);
	subForm(f, 'goform/command', "TMPL_EN", cPage);
}

function checkbox_click(item)
{
	for(var i = 0; i < tmpl_list.length; i ++) 
		if(pageview_ischecked(i)) break;
	document.getElementById('btn_del').disabled = !(i < tmpl_list.length);
}

function init()
{
	document.forms[0].template_disable.value = 0;
	pageview_add('Device Model', 1, '20%', true, dev_string);
	pageview_add('Template Name', 2, '30%', true, null, "tmpl_edit");
	pageview_add('Template Description', 3, '45%', false);
	pageview_init(tmpl_list, 10, document.getElementById('list_table'), true, checkbox_click, null, 'Templates List');
}
</script>
</head>
<body onload="init();">
<script>pageHead(webVars);</script>

<form name=system_status action=goform/command method=post>
<INPUT type=hidden name=CMD value=WAN_CON>
<INPUT type=hidden name=GO value=user_list.asp>
<INPUT type=hidden name=action>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1
	class=box_tn>
	<tr>
		<td class=bggrey2 align=center>Templates Application</td>
		<td class=bggrey align=left>
			<select name="template_disable">
				<option value="0">Enabled</option>
				<option value="1">Disabled</option>
			</select>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
			<input type=button value='  Submit  ' onclick=apply_tmpl_supp() disabled >
	</tr>
</table>
<div id="list_table"></div>

<table>
	<tr>
	<td width=30 valign=top>
		<INPUT id = "btn_del" type=button value="Delete Selected Templates"onclick="apply('DELETE');" disabled=true></td>
	<td width=30 valign=top>
		<INPUT type=button value="New Template" onclick="new_tmpl();"></td>
	</tr>
</table>
</form>
<script>pageTail();</script>
</body>
</html>
