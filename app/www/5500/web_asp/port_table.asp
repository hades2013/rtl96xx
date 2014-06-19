<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 端口管理 </title>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
<script language=javascript src=script/valid.js></script>
<script language=javascript src=script/pageview.js></script>
<script language=javascript src=theme/cvnchina/main.js></script>

<script language=javascript>
var env=new Array();var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

env['vendor.name'] = 'Shanghai Cloud Vision Networks Co., Ltd.';
env['vendor.link'] = 'http://www.cvnchina.com';
env['vendor.copyright'] = '2013-2020';
env['vendor.id'] = 'cvnchina';
env['vendor.logo'] = 'default';

env['sys.theme'] = 'cvnchina';
env['sys.lang'] = 'cn';
env['sys.model'] = 'OCN-OUTDOOR';
env['sys.name'] = 'EOC System';
env['sys.version'] = 'R1.0.5';
env['sys.type'] = 'eoc';
env['sys.mib'] = 'nscrtv-hc';
env['sys.ip'] = '192.168.0.100';
env['sys.user'] = 'admin';
env['sys.time'] = '946684891';
env['sys.linkup'] = '0';
env['sys.linkoff'] = '0';
env['sys.busy'] = '0';
env['sys.ipc'] = '0';
env['sys.vlan'] = '0';
env['sys.unsaved'] = webVars[1];
env['sys.feature'] = 'vlanpool;mme_drop_ports;slave_access_mgmt;bcmp;';


env['web.page'] = current_page();env['sys.parent_name'] = '';

pageRedirectCheck(env);
</script>
<!-- end header -->

<style type="text/css">
.css_link{
   font-size: 12px;
   color: green;
}</style>

<script language=JavaScript>

var port_list = new Array( 'cpu0;UP;1000;FULL',
'eth0;DOWN;100;HALF',
'eth1;DOWN;10;HALF',
'cab0;DOWN;10;HALF',
'cab1;DOWN;10;HALF',
'cab2;DOWN;10;HALF',
'cab3;DOWN;10;HALF');


function port_if_cpu(ind)
{
	var re_cpu = /^cpu[0-9]{1,}$/;
	var sh = port_list[ind].split(';')[0];
	return re_cpu.test(sh);
}

function port_if_cable(ind)
{
	var re_cab = /^cab[0-9]{1,}$/;
	var sh = port_list[ind].split(';')[0];
	return re_cab.test(sh);
}



function port_fancy_name(sh)
{
	var re_cpu = /^cpu[0-9]{1,}$/;
	var re_eth = /^eth[0-9]{1,}$/;
	var re_cab = /^cab[0-9]{1,}$/;		
	var base = "";

	if (re_cpu.test(sh)){
		return "CPU";
	}else if (re_eth.test(sh)){
		base = "Ethernet ";
	}else if (re_cab.test(sh)){
		base = "Cable ";	
	    if (env['sys.type'] == "plc"){
	        base = "PLC ";
	    }
	}else {
		base = "Unknown ";	
	}
	var i = sh.search(/[0-9]/);
	if (i != -1){
		  i = parseInt(sh.substring(i), 10) + 1;
	}
	base += ((i != -1) ? i : "");
	return base;
}


function port_speed_name(s)
{
	if (s == '1000'){
		return "1000 Mbps";	
	}	else if (s == '100'){
		return "100 Mbps";
	}else if (s == "10"){
		return "10 Mbps";	
	}else {
		return "Unknown";	
	}
}

function port_duplex_name(s)
{
	if (s == 'FULL' || s == 'full'){
		return "全双工";	
	}	else if (s == 'HALF' || s == 'half'){
		return "半双工";
	}else {
		return "Unknown";	
	}
}

function port_link_name(s)
{
	if (s == 'UP' || s == 'up'){
		return "<span class='css_link'>UP</span>";	
	}	else if (s == 'DOWN' || s == 'down'){
		return "DOWN";
	}else {
		return "Unknown";	
	}
}

function port_setup_entry(s)
{
	return "设置";
}

function port_stats_entry(s)
{
	return "统计信息";
}

function port_setup_redirect(ind)
{
	if (!port_if_cpu(ind)){
		<!-- location='port_setup.asp?port=' + port_list[ind].split(';')[0];-->
		location="port_set.asp";
	}else {
		alert("CPU端口不能被设置！");	
	}
}

function port_stats_redirect(ind)
{
	<!-- location='port_stat.asp?port=' + port_list[ind].split(';')[0];-->
	location="port_statistic.asp";
}

function page_refresh()
{
	location.reload();
}

function init()
{
	var f=document.forms[0];
	pageview_add('端口', 0, '15%', true, port_fancy_name);
	pageview_add('Status ', 1, '10%', true, port_link_name);	
	pageview_add('速率', 2, '15%', true, port_speed_name);	
	pageview_add('双工模式', 3, '10%', true, port_duplex_name);
	pageview_add('设置', 0, '10%', false, port_setup_entry, "port_setup_redirect");
	pageview_add('统计信息', 0, '10%', false, port_stats_entry, "port_stats_redirect");
	pageview_init(port_list, 10, document.getElementById('port_table'), false, null, null, '端口列表 ');
}


</script>
</head>
<body onload="init();">
<script>pageHead(env, env['web.page']);</script>
<form name=form_port_table action=goform/command method=post onsubmit="return false;">

<div id="port_table">
<p>
<table>
	<tr>
	<td width=30 valign=top>
		<INPUT name=but_refresh class="button" type=button value="  刷新 " onclick="page_refresh();"></td>
	</tr>
</table>
</p></div>
</form>
<script>pageTail();</script>
</body>
</html>