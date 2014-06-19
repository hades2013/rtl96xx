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
var supdev = new Array(<% CGI_GET_DEVINFO(); %>);
var tmpl_list = new Array(<% CGI_GET_TEMPLATES("brief"); %>);
var user_list = new Array(<% CGI_GET_USER_CONFIG(); %>);
var client_desc = new Array(<% CGI_CNU_INFO("mac"); %>);
var client_stats = new Array(<% CGI_CNU_MIBS("mac"); %>);

function get_port_str(p)
{
	var vs;
	for (var i = 0; i < client_stats.length; i ++){
		vs = client_stats[i].split(';');
		if ((vs.length > 0) && (vs[0] == p)){
			return client_stats[i];
		}	
	}
	return '';
}


function port_is_up(p)
{
	var ps = get_port_str(p);
	var vs = ps.split(';');
	if(vs.length > 1){
		return (vs[1] == '1') ? true : false;
	}
	return false;
}

function get_port_num(p)
{
	return client_stats.length;	
}

function get_port_speed(p)
{
	var ps = get_port_str(p);
	var vs = ps.split(';');
	if(vs.length > 2){
		return vs[2];
	}
	return "N/A";		
}


function get_port_rxpkt(p)
{
	var ps = get_port_str(p);
	var vs = ps.split(';');
	if(vs.length > 3){
		return vs[3];
	}
	return "N/A";				
}



function get_port_txpkt(p)
{
	var ps = get_port_str(p);
	var vs = ps.split(';');
	if(vs.length > 4){
		return vs[4];
	}
	return "N/A";					
}



function get_dev_model()
{
	var vs;
	if (client_desc.length > 0){
		vs = client_desc[0].split(';');
		if (vs.length > 6){
			return vs[6];
		}	
	}	
	return "UNKNOWN";
}

function get_dev_mac()
{
	var vs;
	if (client_desc.length > 0){
		vs = client_desc[0].split(';');
		if (vs.length > 2){
			return vs[2];
		}	
	}	
	return "00:00:00:00:00:00";
}

function get_dev_id()
{
	var vs;
	if (client_desc.length > 0){
		vs = client_desc[0].split(';');
		if (vs.length > 10){
			return vs[10];
		}	
	}	
	return "-1";
}

function get_dev_tmpl_id()
{
	var vs;
	if (client_desc.length > 0){
		vs = client_desc[0].split(';');
		if (vs.length > 11){
			return vs[11];
		}	
	}	
	return "-1";
}


function get_dev_online_time()
{
	var vs, th, tm, ts;
	var time = 0;
	if (client_desc.length > 0){
		vs = client_desc[0].split(';');
		if (vs.length > 12){
			time = vs[12];
		}	
	}	
	th = parseInt(time /3600);
	tm = parseInt((time - th * 3600) / 60);
	ts = parseInt(time % 60);
	return th + ' Hour ' + tm + ' Minute ' + ts + ' Second'; 
}



function get_dev_name()
{
	var vs;
	if (client_desc.length > 0){
		vs = client_desc[0].split(';');
		if (vs.length > 9){
			return vs[9];
		}	
	}	
	return "";
}

function dev_in_userlist()
{
	var vs;
	for (var i = 0; i < user_list.length; i ++){
			vs = user_list[i].split(';');
			if ((vs.length > 0) && vs[0] == get_dev_mac()){
				return true;
			}
	}
	return false;		
}

function dev_is_online()
{
	var vs;
	if (client_desc.length > 0){
		vs = client_desc[0].split(';');
		if (vs.length > 7){
			return (vs[7] == "1") ? true : false;
		}	
	}	
	return false;		
}


function dev_is_unconfig()
{
	var devid = get_dev_id();
	if (devid < 0) return true;
	if (devid > supdev.length) return true;
	return false;	
}


function get_dev_tmpl_name()
{
	var vs;
	for (var i = 0; i < tmpl_list.length; i ++){
		vs = tmpl_list[i].split(';');
		if ((vs.length > 0) && (vs[0] == get_dev_tmpl_id())){
			return (vs.length > 2) ? vs[2] : 'UNKNOWN';
		}
	}	
	return 'UBKNOWN';
}


function button_config_onclick()
{
	var devid = (get_dev_id() < 0) ? '' : ('&dev_id=' + get_dev_id());
	if (dev_in_userlist()){
		location='user_edit.asp?action=edit&user_mac=' + get_dev_mac() + devid;
	}else {
		location='user_edit.asp?action=new&user_mac=' + get_dev_mac() + devid;
	}
}


function get_dev_status()
{
	var table_head = '<table width=100% border = 0 align=left cellpadding=0 cellspacing=0> <tr>';
	var table_end = '</tr></table>';
	var inner_str;	
	if (get_dev_id() < 0){
		inner_str = '<td width=80%> Unsupported Devices</td>';
	}else {
		var str_onoff = (dev_is_online() ? 'Online' : 'Offline');
		var str_config;
		var str_button;
		if (dev_is_unconfig()){
			str_config = 'Unconfigurated!';	
		}else {
			str_config = "Configrated with template '" + get_dev_tmpl_name() + "'";
		}
		str_button = dev_in_userlist() ? 'Edit Configration' : 'Setup Configuration';
		inner_str = '<td width=80%>' + str_onoff + ', ' + str_config + 
								'</td><td width=20%><input type=button value="' + str_button + '" onclick="button_config_onclick();"></input></td>';
	}
	return table_head + inner_str + table_end;
}


function port_string(pn)
{
	if(pn == 0) return 'Cable';
	return 'Ethernet ' + (pn - 1);
}

function port_row_string(pn)
{
	var str_portname;
	var str_updown;
	if ((pn == 0) && dev_is_online()){
		str_portname = '<a href="service_pcab_stat.asp?mac=' + get_dev_mac().toUpperCase() + '">' + port_string(pn) + '</a>';
	}else {
		str_portname = port_string(pn);	
	}
	str_updown = port_is_up(pn) ? '<font color=green>UP</font></td>' : '<font color=red>DOWN</font></td>';
	return '<tr><td class=bgblue>' + str_portname + '</td>'+
	'<td class=bgblue>' + str_updown + '</td>'+
	'<td class=bgblue>' + get_port_speed(pn) + '</td>'+
	'<td class=bgblue>' + get_port_rxpkt(pn) + '</td>'+
	'<td class=bgblue>' + get_port_txpkt(pn) + '</td></tr>';
}

function get_dev_port_stats()
{
	var str = "";
	for(var i = 0 ; i < get_port_num(); i ++){
			str += port_row_string(i);
	}	
	return str;
}

function reload()
{
	location='service_sta_prop.asp?mac=' + get_dev_mac();	
}

function init()
{
	document.getElementById('page_header').innerHTML = 'Device - ' + get_dev_mac();
}
</script>
</head>
<body onload=init()>
<script>pageHead(webVars);</script>
<form name=sys action=goform/command method=post>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1
	class=box_tn>
	<tr>
		<td colspan=2 class=titleCell>Device Info</td>
	</tr>
	<tr>
		<td width=25% class=bgblue>Device Model</td>
		<td width=75% class=bggrey><script>document.write(get_dev_model()); </script></td>
	</tr>
	<tr>
		<td width=25% class=bgblue>Device Name</td>
		<td width=75% class=bggrey><script> document.write(get_dev_name()); </script></td>
	</tr>
	<tr>
		<td width=25% class=bgblue>Device Status</td>
		<td width=75% class=bggrey><script> document.write(get_dev_status()); </script></td>
	</tr>
	<tr>
		<td width=25% class=bgblue>Online time</td>
		<td width=75% class=bggrey><script> document.write(get_dev_online_time()); </script></td>
	</tr>
</table>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
<td width=15% class=titleCell>Ports</td>
<td width=10% class=titleCell>Link</td>
<td width=15% class=titleCell>Speed</td>
<td width=30% class=titleCell>Rx Pkts</td>
<td width=30% class=titleCell>Tx Pkts</td>
</tr>
<script> document.write(get_dev_port_stats()); </script>
</table>
<p>
<input type=button value='Refresh' onclick="reload();">&nbsp;&nbsp;&nbsp;&nbsp;
<input type=button value='Return' onclick="gotoUrl('service_map.asp');">
</p>
</form>

<script>pageTail();</script>
</body>
</html>
