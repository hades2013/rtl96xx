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
var client_list = new Array(<% CGI_GET_USER_CONFIG(); %>);
var max_users = <% CGI_GET_VARIABLE("EOC_MAX_USER_NUMS"); %>;
var user_count = 0;

function update_user_count()
{
//	for(var i = 0; i < max_users; i ++)
//		if(client_list[i].length > 0) user_count ++;
	user_count = client_list.length;
}

function dev_string(index)
{
	return supdev[index].split(';')[0];
}


function acc_string(accs) 
{
	if(accs == 'acc') return 'Authorised';
	if(accs == 'blk') return 'Forbidden';
	if(accs == 'any') return 'Anonymous';
	return 'N/A';
}

function set_row_color(index)
{
	if(client_list[index].split(';')[3] == 'acc') return '#F0F0F0';
	if(client_list[index].split(';')[3] == 'blk') return '#FFB0B0';
	if(client_list[index].split(';')[3] == 'any') return '#C0C0F0';
}


function tmpl_string(tmplid)
{
	return tmpl_list[tmplid].split(';')[2];
}

function user_edit(index)
{
	location='user_edit.asp?action=edit&user_mac=' + client_list[index].split(';')[0];
}

function checkbox_click(item)
{
	for(var i = 0; i < client_list.length; i ++) 
		if(pageview_ischecked(i)) break;
	document.getElementById('btn_wht').disabled = !(i < client_list.length); 
	document.getElementById('btn_blk').disabled = !(i < client_list.length);
	document.getElementById('btn_del').disabled = !(i < client_list.length);
}

function apply(cmd)
{
	var mac, ncmd, nstr;
	var f = document.forms[0];
	var count = 0;
	for(var i = 0; i < client_list.length; i ++)
		if(pageview_ischecked(i)) {
			if(cmd == 'USER_DEL') {
				diffCfg('user_' + i, 'user_del', client_list[i].split(';')[0]);
				ncmd = 'USER_DEL';
				count ++; 
			}
			else {
				if((cmd == 'ADD_TO_WHITE') && (client_list[i].split(';')[3] == 'acc')) continue;
				if((cmd == 'ADD_TO_BLACK') && (client_list[i].split(';')[3] == 'blk')) continue;
				nstr = '';
				for(var j = 0; j < 3; j ++) nstr += client_list[i].split(';')[j] + ';';
				if(cmd == 'ADD_TO_WHITE') 
					nstr += 'acc';
				else
					nstr += 'blk';
				for(j = 4; j < client_list[i].split(';').length; j ++)
					nstr += ';' + client_list[i].split(';')[j];
				diffCfg('user_' + i, 'user_' + i, nstr);
				ncmd = 'USER_PROP';
				count ++;
			}
		}
	if(count > 0) subForm(f,'goform/command', ncmd, cPage);
}

function new_client()
{
	location='user_edit.asp?action=new';
}

function init()
{
	update_user_count();
	pageview_add('MAC', 0, '15%', true, null, "user_edit");
	pageview_add('Device Model', 2, '15%', true, dev_string);
	pageview_add('Template', 1, '20%', true, tmpl_string);
	pageview_add('Name', 4, '25%', true);
	pageview_add('Access Control', 3, '15%', true, acc_string);
	pageview_init(client_list, 10, document.getElementById('list_table'), true, checkbox_click, set_row_color, 'Users Table');
	document.getElementById('btn_new').disabled = user_count >= max_users;
}

</script>
</head>
<body onload="init();">
<script>pageHead(webVars);</script>

<form name=system_status action=goform/command method=post>
<INPUT type=hidden name=CMD value=WAN_CON>
<INPUT type=hidden name=GO value=user_list.asp>
<INPUT type=hidden name=action>

<div id="list_table"></div>

<p>
<table width=60% border=0 align=center cellpadding=1 cellspacing=2>
	<tr>
		<td bgcolor=#F0F0F0 width=30>&nbsp;&nbsp;</td>
		<td>Whitelist</td>
		<td bgcolor=#FFB0B0 width=30>&nbsp;&nbsp;</td>
		<td>Blacklist</td>
	</tr>
</table>
</p>
<table>
	<tr>
	<td width=30 valign=top>
		<INPUT id = "btn_del" type=button value="Delete Selected Users" onclick="apply('USER_DEL');" disabled=true></td>
	<td width=30 valign=top>
		<INPUT id = "btn_wht" type=button value="Add to Whitelist" onclick="apply('ADD_TO_WHITE');" disabled=true></td>
	<td width=30 valign=top>
		<INPUT id = "btn_blk" type=button value="Add to Blacklist"  onclick="apply('ADD_TO_BLACK');" disabled=true></td>
	<td width=30 valign=top>
		<INPUT id="btn_new" type=button value="New User" onclick="new_client();"></td>
	</tr>
</table>
</form>

<script>pageTail();</script>
</body>
