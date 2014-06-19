<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 端口设置 </title>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
<script language=javascript src=script/valid.js></script>
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
env['sys.time'] = '946685251';
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

var port_config = new Array( 'cpu0;7;1;0;1000;full;0;7;0;0;0;1',
'eth0;6;1;1;1000;full;1;off;0;0;0;1',
'eth1;1;1;1;1000;full;1;off;0;0;0;1',
'cab0;2;1;1;1000;full;1;off;0;0;0;1',
'cab1;3;1;1;1000;full;1;off;0;0;0;1',
'cab2;4;1;1;1000;full;1;off;0;0;0;1',
'cab3;5;1;1;1000;full;1;off;0;0;0;1');

function to_upper_page()
{
	location = "port_table.asp";
}

/* port const functions */
function port_if_cpu(sh)
{
	var re_cpu = /^cpu[0-9]{1,}$/;
//	var sh = port_config[ind].split(';')[0];
	return re_cpu.test(sh);
}

function port_if_cable(sh)
{
	var re_cab = /^cab[0-9]{1,}$/;
//	var sh = port_config[ind].split(';')[0];
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

/* end port const functions */


function port_select_options()
{
	var i, sh, re = "";
	for (i = 0; i < port_config.length; i ++){
		sh = port_config[i].split(';')[0];
		if (port_if_cpu(sh)) continue;
		re += "<option value='" + sh + "'> " + port_fancy_name(sh) + "</option>\n";	  	
	}
	return re;
}


function port_cfgstr(n)
{
	 for (var i = 0; i < port_config.length; i ++){
	 	 if (port_config[i].split(';')[0] == n){
	 	 		return port_config[i];
	 	 	}
	 	}
	 return "";
}


function port_spddup(n)
{
	var v = port_cfgstr(n);
	var au, sp, dup;
	au = v.split(';')[3];
	sp = v.split(';')[4];
	dup = v.split(';')[5];
	if (au == '1'){
		return 'auto';	
	}else {
		if (sp == '1000'){
			return '1000full';	
		}
		return sp + dup;
	}
}

function port_enabled(n)
{
	var v = port_cfgstr(n);
	return v.split(';')[2]; 	
}


function port_pri(n)
{
	var v = port_cfgstr(n);
	return v.split(';')[7]; /*off, 0 - 7*/	
}

function port_flow(n)
{
	var v = port_cfgstr(n);
	return v.split(';')[6]; 
}

function port_egress_limit(n)
{
	var v = port_cfgstr(n);
	return v.split(';')[9]; 
}

function port_ingress_limit(n)
{
	var v = port_cfgstr(n);
	return v.split(';')[8]; 
}

function rate_in_mbps(rate)
{
	return !(rate % 1024)? true: false;	
}

function rate_on_change()
{
	var f=document.forms[0];
	f.cfg_ingress_rate_value.disabled = (f.cfg_ingress_rate.value == '0') ? true : false;
	f.cfg_egress_rate_value.disabled = (f.cfg_egress_rate.value == '0') ? true : false;	
}


function port_items_disable(p, v)
{
		var f=document.forms[0];
		f.cfg_speed_duplex.disabled = port_if_cable(p) ? true : v;
		f.cfg_priority.disabled = /*port_if_cable(p) ? true : */v;
		f.cfg_flow_control.disabled = /*port_if_cable(p) ? true : */v;
		f.cfg_ingress_rate.disabled = v;
		f.cfg_ingress_rate_value.disabled = v;
		f.ingress_rate_unit[0].disabled = v;	
		f.ingress_rate_unit[1].disabled = v;	
		f.cfg_egress_rate.disabled = v;
		f.cfg_egress_rate_value.disabled = v;
		f.egress_rate_unit[0].disabled = v;	
		f.egress_rate_unit[1].disabled = v;
		rate_on_change();			
}

function port_enable_on_change()
{
	var f=document.forms[0];
	var p = f.port_select.value;
	
	if (f.cfg_enable.value == '0'){
		port_items_disable(p, true);		
	}else {
		port_items_disable(p, false);
	}	
}



function port_select_on_change()
{
	var f=document.forms[0];
	var rate; 
	var p = f.port_select.value;
	
	f.cfg_enable.value = port_enabled(p);
	f.cfg_speed_duplex.value = port_spddup(p);
	f.cfg_priority.value = port_pri(p);
	f.cfg_flow_control.value = port_flow(p);

	rate = parseInt(port_ingress_limit(p), 10);
	f.cfg_ingress_rate.value = (rate == 0) ? '0' : '1';
	f.cfg_ingress_rate_value.disabled = (rate == 0) ? true : false;
	if (rate != 0){
		if (rate_in_mbps(rate)){
			rate = rate / 1024;
			f.ingress_rate_unit[0].checked = true;	
		}	else {
			f.ingress_rate_unit[1].checked = true;	
		}
		f.cfg_ingress_rate_value.value = rate;
	}else {
			f.cfg_ingress_rate_value.value = "";		
			f.ingress_rate_unit[0].checked = true;			
	}
		
	rate = parseInt(port_egress_limit(p), 10);
	f.cfg_egress_rate.value = (rate == 0) ? '0' : '1';
	f.cfg_egress_rate_value.disabled = (rate == 0) ? true : false;
	if (rate != 0){
		if (rate_in_mbps(rate)){
			rate = rate / 1024;
			f.egress_rate_unit[0].checked = true;	
		}	else {
			f.egress_rate_unit[1].checked = true;	
		}
		f.cfg_egress_rate_value.value = rate;
	}	else {
			f.cfg_egress_rate_value.value = "";
			f.egress_rate_unit[0].checked = true;			
	}	
	
	port_enable_on_change();
}

function init()
{
	var f=document.forms[0];
	if (env['vendor.id'] == 'cvnchina'){
		document.getElementById("tr_priority").style.display = "none";
	}
	if (QueryString('port')){
		f.port_select.value = QueryString('port');
	}else {
		f.port_select.value = 'eth0';	
	}
	port_select_on_change();
}

/* functions for submit */

function parse_duplex(s)
{
	if (s == '1000full'){
		return 'full';	
	}else 	if (s == '100full'){
		return 'full';	
	}else 	if (s == '100half'){
		return 'half';	
	}else 	if (s == '10full'){
		return 'full';	
	}else 	if (s == '10half'){
		return 'half';	
	}
	return 'full';
}		
					
function parse_speed(s)
{
	if (s == '1000full'){
		return '1000';	
	}else 	if (s == '100full'){
		return '100';	
	}else 	if (s == '100half'){
		return '100';	
	}else 	if (s == '10full'){
		return '10';	
	}else 	if (s == '10half'){
		return '10';	
	}
	return '100';
}		

function valid_port_rate(rate)
{
	return ((rate >= 8) && (rate <=1048568) && ((rate % 8) == 0)) ? true : false;
}

function number_check(val)
{
  var re = /^[1-9][0-9]*$/; 
  return re.test(val);	
}


function port_rate_check()
{
	var f=document.forms[0];
	var rate;
	
	if (f.cfg_ingress_rate.value == '1'){
		if (number_check(f.cfg_ingress_rate_value.value) == false) return false;
	 	if (f.ingress_rate_unit[0].checked == true){
	 		rate = f.cfg_ingress_rate_value.value * 1024;
		}else {
			rate = f.cfg_ingress_rate_value.value;	
		}	
	 	if (valid_port_rate(rate) == false){
	 		return false;	
	 	}			
	}
	
	if (f.cfg_egress_rate.value == '1'){
		if (number_check(f.cfg_egress_rate_value.value) == false) return false;
		
	 	if (f.egress_rate_unit[0].checked == true){
	 		rate = f.cfg_egress_rate_value.value * 1024;
		}else {
			rate = f.cfg_egress_rate_value.value;	
		}	
	 	if (valid_port_rate(rate) == false){
	 		return false;	
	 	}			
	}	

	return true;
}


function port_rate_get(n, ingress)
{
	var f=document.forms[0];
	
	if (ingress){
		if (f.cfg_ingress_rate.value == '0'){
			return 0;	
		}else if (f.ingress_rate_unit[0].checked == true){
			return f.cfg_ingress_rate_value.value * 1024;
		}else {
			return f.cfg_ingress_rate_value.value;	
		}
	}else {
		if (f.cfg_egress_rate.value == '0'){
			return 0;	
		}else if (f.egress_rate_unit[0].checked == true){
			return f.cfg_egress_rate_value.value * 1024;
		}else {
			return f.cfg_egress_rate_value.value;	
		}				
	}
}


function get_port_config(n)
{
	var f=document.forms[0];
	var s, au, sp, dup, flow, pri, ingr, egr;
	
	au = (f.cfg_speed_duplex.value == 'auto') ? 1 : 0;
	if (au){
		sp =  port_cfgstr(n).split(';')[4];
		dup = port_cfgstr(n).split(';')[5];
	}else {
		sp = 	parse_speed(f.cfg_speed_duplex.value);
		dup = parse_duplex(f.cfg_speed_duplex.value);				
	}
	flow = f.cfg_flow_control.value;
	pri = f.cfg_priority.value;
	ingr = port_rate_get(n, true); 
	egr = port_rate_get(n, false);						
			
	s = n + ';' + port_cfgstr(n).split(';')[1] + ';'
	+ f.cfg_enable.value + ';'
	+ au + ';'+ sp + ';'+ dup + ';'
	+ flow + ';' + pri + ';'
	+ ingr + ';' + egr + ';'	
	+ port_cfgstr(n).split(';')[10] + ';'
	+ port_cfgstr(n).split(';')[11];
			
	return s;	
}


function apply()
{
	var f=document.forms[0];	
	var p = f.port_select.value;
	if (port_rate_check() == false){
			alert("无效的速率限制值,请重新输入!");
			return;
	}
	var cfg = get_port_config(p);	
	if (cfg == port_cfgstr(p)){
		alert("所选端口的配置没有改变!");
		return;	
	}
//	alert("Get port config:" + cfg);
	diffCfg('eth_0', 'eth_cfg', cfg);		
	subForm(f,'goform/command','SYS_ETH', "port_setup.asp?port=" + p);
}




</script>
</head>
<body onload="init();">
<script>pageHead(env, "port_table.asp");</script>
<form name=form_port_config action=goform/command method=post  onsubmit="return false;">
<INPUT type=hidden name=CMD>
<INPUT type=hidden name=GO>
<INPUT type=hidden name=action>
<table width=100% border=0 align=center cellpadding=5 cellspacing=0 >
	<tr>
		<td colspan=2 class=tb_head>端口设置</td>
	</tr>	
	<tr>
		<td width=20% class=tb_title>端口选择</td>
		<td width=80% class=tb_content> 
			<select name=port_select onchange=port_select_on_change()>
			<script>document.write(port_select_options());</script> 
		</select>
		</td>
	</tr>	
	<tr>
		<td width=20% class=tb_title>端口控制</td>
		<td width=80% class=tb_content> 
			<select name=cfg_enable onchange=port_enable_on_change()>
			<option value='1'> 启用 </option>
			<option value='0'> 禁用 </option>			
		</select>
		</td>
	</tr>		
	<tr>
		<td width=20% class=tb_title>速率/双工模式</td>
		<td width=80% class=tb_content>
			<select name=cfg_speed_duplex>
			<option value="auto">自动</option>
			<option value="1000full">1000Mbps 全双工</option>
			<option value="100full">100Mbps 全双工</option>
			<option value="100half">100Mbps 半双工</option>
			<option value="10full">10Mbps 全双工</option>
			<option value="10half">10Mbps 半双工</option>
			</select>
		</td>
	</tr>
	<tr id="tr_priority">
		<td class=tb_title>优先级</td>
		<td class=tb_content>
			<select name=cfg_priority>
			<option value="off">默认</option>
			<option value=0>0</option>
			<option value=1>1</option>
			<option value=2>2</option>
			<option value=3>3</option>
			<option value=4>4</option>
			<option value=5>5</option>
			<option value=6>6</option>
			<option value=7>7</option>					
			</select>
		</td>
	</tr>
	<tr>
		<td class=tb_title>流控</td>
		<td class=tb_content>
			<select name=cfg_flow_control>
			<option value='0'>禁用</option>
			<option value='1'>启用</option>
			</select>
		</td>
	</tr>
	<tr>
		<td class=tb_title>入口带宽限制</td>
		<td class=tb_content>
			<select name=cfg_ingress_rate onchange="rate_on_change();">
			<option value="0">禁用</option>
			<option value="1">启用</option>
			</select>
			<input name=cfg_ingress_rate_value size=8 maxlength=8>
			<input type=radio name=ingress_rate_unit value="Mbps"> Mbps
			<input type=radio name=ingress_rate_unit value="Kbps"> Kbps
		</td>
	</tr>
	<tr>
		<td class=tb_title>出口带宽限制</td>
		<td class=tb_content>
			<select name=cfg_egress_rate onchange="rate_on_change();">
			<option value="0">禁用</option>
			<option value="1">启用</option>
			</select>
			<input name=cfg_egress_rate_value size=8 maxlength=8>
			<input type=radio name=egress_rate_unit value="Mbps"> Mbps
			<input type=radio name=egress_rate_unit value="Kbps"> Kbps
		</td>
	</tr>
	<tr>
		<td colspan=2><font size=2 color=blue>注意:带宽限制使用Mbps输入时,范围为1-1023; <br> 
			使用Kbps输入时,范围为8-1048568,且须为8的倍数.</font></td>
	</tr>	
</table>
<p>
<input class="button" type=button value='  确  认  ' onclick=apply() > &nbsp;&nbsp;&nbsp;
<input class="button" type=button value='  取  消  ' onclick=to_upper_page() >
</p>
</form>
<script>pageTail();</script>
</body>
</html>
 യയ        䱙 ，Ţߖ  搟￿￿￿Q       縈ߙയŢ      യയ                    ȀീƬЈაߘ    ��� （,㶴耀       PĀ 鈀    ��� （Ǯ㶴耀         艀Ā   ��� （靖㶴耀         艀Ġ   ��� （㶴耀       [Ā 舀Ā   ��� ，      烸ߙ焐ߙ  9ఀ    ��� ，      焐ߙ껸ത  ఀ    ��� ，        꺠ത깈ത  ఀ    숴 ，￿      琐