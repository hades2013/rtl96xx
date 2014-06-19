<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="content-type" content="text/html;charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=javascript src=pageview.js></script>
<script language=JavaScript>
pageTitle();


var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var client_list = new Array (<% CGI_CLIENT_QUERY("ALL", "1"); %>);
var user_list = new Array(<% CGI_GET_USER_CONFIG(); %>);
var max_users = <% CGI_GET_VARIABLE("EOC_MAX_USER_NUMS"); %>;
var user_count = user_list.length;


function dev_string(alias)
{
	return alias;
}

var anon_count = 0;

function acc_string(accs) 
{
	if(accs == 'acc') return 'Authenticated';
	if(accs == 'blk') return 'Forbidden';
	if(accs == 'any') return 'Anonymous';
	return 'N/A';
}

function link_string(lnk)
{
	switch(parseInt(lnk)) {
	case 1:
		return 'Up';
	case 2:
		return 'Down';
	case 4:
		return 'Refused';
	default:
		return 'Unknown';
	}
}

function rate_string(rate)
{
	if(parseInt(rate) == 0) return "N/A";
	return rate;
}

function checkbox_click(item)
{
	if(client_list[item].split(';')[8] == 'any')
		if(pageview_ischecked(item)) 
			anon_count ++;
		else
			anon_count --;
	if(user_count + anon_count > max_users) {
		alert('The number of user device has reached top, "' + client_list[item].split(';')[2] + '" could not be added to whitelist or blacklist!');
		pageview_clean_check(item, false);
		anon_count --;
		return;
	}
	
	for(var i = 0; i < client_list.length; i ++) { 
		if(pageview_ischecked(i)) break;
	}
	document.getElementById('btn_wht').disabled = !(i < client_list.length); 
	document.getElementById('btn_blk').disabled = !(i < client_list.length);
}

function sta_info(item)
{
	if(client_list[item].split(';')[10] != '-1')
		location='service_sta_prop.asp?mac=' + client_list[item].split(';')[2].toUpperCase();
}

function set_row_color(index)
{
	if(client_list[index].split(';')[7] == '4') return '#FF7070';
	if(client_list[index].split(';')[8] == 'acc') return '#F0F0F0';
	if(client_list[index].split(';')[8] == 'blk') return '#FFB0B0';
	if(client_list[index].split(';')[8] == 'any') return '#C0C0F0';
}

function apply(cmd)
{
	var nstr, mac;
	var cnt = 0;
	var f = document.forms[0];
	for(var i = 0; i < client_list.length; i ++)
		if(pageview_ischecked(i)) {
			if((cmd == 'ADD_TO_WHITE') && (client_list[i].split(';')[8] == 'acc')) continue;
			if((cmd == 'ADD_TO_BLACK') && (client_list[i].split(';')[8] == 'blk')) continue;
			for(var x = 0; x < user_count; x ++) 
				if(user_list[x].split(';')[0] == client_list[i].split(';')[2]) {
					nstr = '';
					for(var j = 0; j < 3; j ++) nstr += user_list[x].split(';')[j] + ';';
					if(cmd == 'ADD_TO_WHITE') 
						nstr += 'acc';
					else
						nstr += 'blk';
					for(j = 4; j < user_list[x].split(';').length; j ++)
						nstr += ';' + user_list[x].split(';')[j];
					diffCfg('user_' + cnt, 'user_' + cnt, nstr);
					cnt ++;
					break;
				}
			if(x >= user_count) {
					mac = client_list[i].split(';')[2];
					nstr = mac + ';' +
						client_list[i].split(';')[10] + ';' +
						client_list[i].split(';')[10] + ';';
					if(cmd == 'ADD_TO_WHITE') 
						nstr += 'acc;';
					else
						nstr += 'blk;';
					nstr += 'user' + mac.split(':')[3] + mac.split(':')[4] + mac.split(':')[5] + ';';
					diffCfg('user_' + cnt, 'user_' + cnt, nstr);
					cnt ++;
			}	
		}
	  if (cnt > 0) subForm(f,'goform/command', "USER_NEW", cPage);
}

function init()
{
	pageview_add('CLT', 0, '5%', false);
	pageview_add('MAC', 2, '20%', true, null, "sta_info");
	pageview_add('Model', 6, '15%', true, dev_string);
	pageview_add('Status', 7, '10%', true, link_string);
	pageview_add('Tx Row(Mbps)', 4, '12%', false, rate_string);
	pageview_add('Rx Row(Mbps)', 5, '12%', false, rate_string);
	pageview_add('Name', 9, '17%', false);
	pageview_init(client_list, 10, document.getElementById('list_table'), true, checkbox_click, set_row_color, 'Devices List');
}
</script>
</head>

<body onload=init()>
<script>pageHead(webVars);</script>

<form name=system_status action=goform/command method=post>

<div id="list_table"></div>

<INPUT type=hidden name=CMD value=WAN_CON>
<INPUT type=hidden name=GO value=system_status.asp>
<p>
<table width=80% border=0 align=center cellpadding=1 cellspacing=2>
	<tr>
		<td bgcolor=#F0F0F0 width=30>&nbsp;&nbsp;</td>
		<td>Whitelist</td>
		<td bgcolor=#FFB0B0 width=30>&nbsp;&nbsp;</td>
		<td>Blacklist</td>
		<td bgcolor=#C0C0F0 width=30>&nbsp;&nbsp;</td>
		<td>Anonymous</td>
		<td bgcolor=#FF7070 width=30>&nbsp;&nbsp;</td>
		<td>Forbidden</td>
	</tr>
</table>
</p>
<table width=100% border=0 align=center cellpadding=0 cellspacing=1>
	<tr><td>
	<input id="btn_wht" type=button value="Add to Whitelist" onclick="apply('ADD_TO_WHITE');" disabled=true>
	<input id="btn_blk" type=button value="Add to Blacklist" onclick="apply('ADD_TO_BLACK');" disabled=true>&nbsp;&nbsp;&nbsp;&nbsp;
	<input type=button value="   Refresh   " onclick=refresh("service_map.asp")>
	</td></tr>
</table>
</form>

<script>pageTail();</script>
</body>
</html>
