<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 配置VLAN 模式 </title>
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
env['sys.time'] = '946684845';
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

<script language=JavaScript>

var vlan_options=new Array( <% CGI_GET_VLAN_OPTIONS(); %>);
//var vlan_options = new Array('0','1','eth0;access','eth1;trunk','eth2;trunk','eth3;trunk');

var apply_list;


/* port const functions */
function port_if_cpu(sh)
{
	var re_cpu = /^cpu[0-9]{1,}$/;
	return re_cpu.test(sh);
}

function port_if_cable(sh)
{
	var re_cab = /^cab[0-9]{1,}$/;
	return re_cab.test(sh);
}

function port_if_eth(sh)
{
	var re_eth = /^eth[0-9]{1,}$/;
	return re_eth.test(sh);
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

/* end port const functions */


function vlan_mode()
{
	if (vlan_options.length > 0){
		return vlan_options[0];	
	}	
	return '0';
}

function get_linktype(n)
{
	for (var i = 0; i < vlan_options.length; i ++){
		var vs = vlan_options[i].split(';');
		if ((vs.length > 1) && (vs[0] == n)){
				return vs[1];
		}	
	}
	return 'trunk';
}

function vmchange()
{
	if(document.forms[0].vlan_mode.value == '1') {
		document.getElementById('vm_transparent').style.display = 'block';
		/*
		if ((env['vendor.id'] == 'cvnchina')){
			document.forms[0].vlan_mode.disabled = true;
		}*/
	}
	else {
		document.getElementById('vm_transparent').style.display = 'none';
	}		
}

function port_rows(p)
{
	var select_disable = port_if_cable(p) ? ' disabled' : '';
	var select_name = 'type_' + p;
	var select_access_check = (get_linktype(p) == 'access') ? 'selected' : '';
	var select_trunk_check = (get_linktype(p) == 'trunk') ? 'selected' : '';	
		
	if ((env['vendor.id'] == 'cvnchina') && (p != 'eth0')){
		select_disable = 'disabled';
	}
	
	var str = "	<tr> " +
	"	<td class='tb_title'>" + port_fancy_name(p) + "</td> " +
	"	<td width=70% class='tb_content'> " +
	"		<select name='" + select_name + "' " + select_disable + "> " +
		"		<option value='access' " + select_access_check + " >管理 </option> " +
		"		<option value='trunk' " + select_trunk_check + ">业务端口</option> " +
		"	</select> " +
	"	</td> " +
"	</tr> \n";
	return str;
}

function port_type_rows()
{
	var str = "";
	var p;
	apply_list = "";
	//get cables
	for (var i = 2; i < vlan_options.length; i ++ ){
	 p = vlan_options[i].split(';')[0];
		if(port_if_cable(p)){
			str +=	port_rows(p);
			if (apply_list != '') apply_list += ',';
			apply_list += p;
		} 
	}
	// get ethernets
	for (var i = 2; i < vlan_options.length; i ++ ){
		p = vlan_options[i].split(';')[0];
		if(port_if_eth(p)){
			str +=	port_rows(p);
			if (apply_list != '') apply_list += ',';
			apply_list += p;			
		} 
	}	
	return str;
}

function apply()
{
	//var mode_change = false;
	//var f=document.forms[0];
	
	//if(f.vlan_mode.value != vlan_mode()) {
	//	if(!confirm('VLAN模式改变将导致以前的VLAN配置丢失，确定继续？')) return;
	//	mode_change = true;
	//}
	// if(f.vlan_mode.value == '1') {
	//		diffCfg('vlan_0', 'vlan_mode', f.vlan_mode.value);
			/*
			if (apply_list != ''){
				var ps = apply_list.split(',');
				for (var i = 0; i < ps.length; i ++){
					var v = document.getElementsByName('type_' + ps[i]);
					if (v.length){
							diffCfg('vlan_'+ (i + 1), 'port_type', ps[i] + ';' + v[0].value);		
					}
				}	
			}*/
	//		subForm(f,'goform/command','VLAN_MODE','vlan_mode.asp');			
	//	}else if (mode_change){
	//		diffCfg('vlan_0', 'vlan_mode', f.vlan_mode.value);
	//		subForm(f,'goform/command','VLAN_MODE','vlan_mode.asp');			
	//	}
	var f=document.forms[0];
	if(f.vlan_mode.value != vlan_mode()) 
	{
		if(!confirm('VLAN模式改变将导致以前的VLAN配置丢失，确定继续？')) 
			return;
		else
		{
			diffCfg('vlan_0', 'vlan_mode', f.vlan_mode.value);
			subForm(f,'goform/command','VLAN_MODE','vlan_mode.asp');
		}	
	}
}


function init()
{
	var f=document.forms[0];
	f.vlan_mode.value = vlan_mode();
	vmchange();
}
</script>
</head>

<body onload=init()>
<script>pageHead(env, env['web.page']);</script>

<form name=vlan_8021q action=goform/command method=post onsubmit="return false;">
<INPUT type=hidden name=CMD>
<INPUT type=hidden name=GO>
<INPUT type=hidden name=action>


<table width=100% cellpadding=5 cellspacing=0>
		<tr ><td colspan=2 class="tb_head">配置VLAN 模式</td> </tr>		
	<tr>
		<td width=30% class=tb_title>VLAN 模式</td>
		<td width=70% class=tb_content>
			<select name='vlan_mode' onchange="vmchange()";>
				<option value="0">802.1q VLAN</option>
				<option value="1">VLAN透传模式</option>
			</select>
		</td>
	</tr>
</table>
<div id="vm_transparent">
<table width=100% cellpadding=5 cellspacing=0>
	<script> document.write(port_type_rows()) </script>	
</table>
</div>
<p>
<input class="button" type=button value=" 确  认 " onclick="apply();">&nbsp;&nbsp;&nbsp;&nbsp;
<input class="button" type=button value=" 取  消 " onclick="init();">
</p>

</form>
<script>pageTail();</script>
</body>
</html>