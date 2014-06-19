<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - SNMP代理设置 </title>
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
env['sys.time'] = '946685430';
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
var syssnmp = new Array('snmp_enable=1',
'snmp_port=161',
'snmp_sysname=EOC System',
'snmp_syscontact=Shanghai Cloud Vision Networks Co., Ltd.',
'snmp_syslocation=Shanhai, China',
'snmp_v1v2_limitation=0',
'snmp_trust_ip=',
'snmp_ro_user=guest',
'snmp_ro_user_pwd=12345678',
'snmp_ro_user_type=1',
'snmp_rw_user=admin',
'snmp_rw_user_pwd=12345678',
'snmp_rw_user_type=1',
'snmp_trap_enable=1');
var communities = new Array('1;public;2',
'2;private;1');
var traps = new Array('1;192.168.0.148;public;1');

var max_community_num = 8;
var max_trap_num = 8;
 
var set_communities = new Array();
var set_traps = new Array();

/* COMMUNITIES */

function html_community_row(index, community)
{
	var html = '<tr>';

	html += '<td class=tb_content style="border:0;"> 	<input name=snmp_community_' + index + ' size=31 maxlength=31> </td>';
	html += '<td class=tb_content style="border:0;"> <select name="snmp_community_mode_' + index + '" ' + ((index < 2) ? 'disabled' : '') + '>';
  html += '<option value="2">只读</option> <option value="1">读写</option> </select> </td>';
	html += '<td class=tb_content style="text-align:center;border:0;">';
	if (index == 0){
		html += '<input class="button" type=button value=" 添加 " onclick=community_add_onclick()>'; 
	}else if (index > 1){
		html += '<input class="button" type=button value=" 删除 " onclick=community_delete_onclick("' + index + '")>'; 	
	}else {
		html += '&nbsp;';	
	}
	html += '</td>';
	
	html += '</tr>';
	return html;	
}



function html_communities(comm_array)
{
	var html = '';
	html += '<table width=100% border=0 align=center cellpadding=2 cellspacing=0>';
	for (var i=0; i < comm_array.length; i ++){
		html += html_community_row(i, comm_array[i]);	
	}
	html += '</table>';
	return html;
}


function community_value(community)
{
	var vs = community.split(';');
	if (vs.length > 1){
		return vs[1];	
	}	
	return '';
}

function community_mode(community)
{
	var vs = community.split(';');
	if (vs.length > 2){
		return vs[2];	
	}	
	return '';
}

function community_update()
{
	var obj_value, obj_mode;
	for (var i = 0; i < set_communities.length; i ++){
		obj_value = document.getElementsByName('snmp_community_' + i)[0];
		obj_mode = document.getElementsByName('snmp_community_mode_' + i)[0];
		if (obj_mode && obj_value){
			set_communities[i] = '0;' + obj_value.value + ';' + obj_mode.value;
		}
	}
}

function community_add_onclick()
{
	if (set_communities.length >= max_community_num){
			alert("团体字的数量已达到最大值，不能再添加新的团体字！");
			return ;
	}
	community_update();
	set_communities[set_communities.length] = '0;;1';
	set_community_table();
}

function community_delete_onclick(index)
{
	community_update();
	var new_communities = new Array();
	for (var i = 0; i < set_communities.length; i ++){
		if (index != i){
			new_communities[new_communities.length] = set_communities[i];
		}	
	}
	set_communities = new_communities;
	set_community_table();
}


function set_community_table()
{
	var obj = document.getElementById('tb_community');
	if (obj){
		obj.innerHTML = html_communities(set_communities);	
	}
	
	// udpate values
	for (var i = 0; i < set_communities.length; i ++){
		obj = document.getElementsByName('snmp_community_' + i)[0];
		if (obj) obj.value = community_value(set_communities[i]);
		obj = document.getElementsByName('snmp_community_mode_' + i)[0];
		if (obj) obj.value = community_mode(set_communities[i]);		
	}
	
}

function valid_snmp_communities()
{
	var obj;
	for (var i = 0; i < set_communities.length; i ++){
		obj = document.getElementsByName('snmp_community_' + i)[0];
		if (obj){
			if (!valid_snmp_community(obj.value)){
				alert('SNMP团体字中包含无效的字符，请重新输入！');
				obj.focus();	
				return false;
			}	
		}
	}
	return true;
}

/* END COMMUNITIES */

/* TRAPS */

function html_trap_row(index, trap)
{
	var html = '<tr>';
	html += '<td class=tb_content style="border:0;"><input name=snmp_trap_ip_' + index + ' size=24 maxlength=31> </td>';
	html += '<td class=tb_content style="border:0;"><input name=snmp_trap_community_' + index + ' size=20 maxlength=31> </td>';
	html += '<td class=tb_content style="border:0;">';
 	html += '<select name=snmp_trap_admin_' + index + ' >';
  html += '<option value="0">禁用</option>';
  html += '<option value="1">启用</option>';
  html += '</select>';
	html += '</td>';
	html += '<td class=tb_content style="text-align:center;border:0;">';
	if (index > 0){
		html += '<input class="button" type=button value=" 删除 " onclick=trap_delete_onclick("' + index + '")>'; 	
	}else {
		html += '&nbsp;';	
	}
	html += '</td>';
	
	html += '</tr>';
	return html;	
}



function html_traps(trap_array)
{
	var html = '';
	html += '<table width=100% border=0 align=center cellpadding=2 cellspacing=0>';
	html += '<tr>';
	html += '<td class=tb_content style="width:30%;text-align:center;border:0;"> 日志服务器IP地址 </td>';
	html += '<td class=tb_content style="width:30%;text-align:center;border:0;"> 团体字 </td>';
	html += '<td class=tb_content style="width:10%;text-align:center;border:0;"> 管理状态 </td>';
	html += '<td class=tb_content style="text-align:center;border:0;"> <input class="button" type=button value=" 添加 " onclick=trap_add_onclick() > </td>';
	html += '</tr>';
		
	for (var i=0; i < trap_array.length; i ++){
		html += html_trap_row(i, trap_array[i]);	
	}
	html += '</table>';
	return html;
}


function trap_ip(trap)
{
	var vs = trap.split(';');
	if (vs.length > 1){
		return vs[1];	
	}	
	return '';
}

function trap_community(trap)
{
	var vs = trap.split(';');
	if (vs.length > 2){
		return vs[2];	
	}	
	return '';
}

function trap_admin(trap)
{
	var vs = trap.split(';');
	if (vs.length > 3){
		return vs[3];	
	}	
	return '';
}

function trap_update()
{
	var obj_ip, obj_comm, obj_admin;
	for (var i = 0; i < set_traps.length; i ++){
		obj_ip = document.getElementsByName('snmp_trap_ip_' + i)[0];
		obj_comm = document.getElementsByName('snmp_trap_community_' + i)[0];
		obj_admin = document.getElementsByName('snmp_trap_admin_' + i)[0];
		
		if (obj_ip && obj_comm && obj_admin){
			set_traps[i] = '0;' + obj_ip.value + ';' + obj_comm.value + ';' + obj_admin.value;
		}
	}
}

function trap_add_onclick()
{
	if (set_traps.length >= max_trap_num){
			alert("TRAP的数量已达到最大值，不能再添加新的TRAP！");
			return ;
	}
	trap_update();
	set_traps[set_traps.length] = '0;;public;1';
	set_trap_table();
}

function trap_delete_onclick(index)
{
	trap_update();
	var new_traps = new Array();
	for (var i = 0; i < set_traps.length; i ++){
		if (index != i){
			new_traps[new_traps.length] = set_traps[i];
		}	
	}
	set_traps = new_traps;
	set_trap_table();
}


function set_trap_table()
{
	var obj = document.getElementById('tb_trap');
	if (obj){
		obj.innerHTML = html_traps(set_traps);	
	}
	
	// udpate values
	for (var i = 0; i < set_traps.length; i ++){
		obj = document.getElementsByName('snmp_trap_ip_' + i)[0];
		if (obj) obj.value = trap_ip(set_traps[i]);
		obj = document.getElementsByName('snmp_trap_community_' + i)[0];
		if (obj) obj.value = trap_community(set_traps[i]);
		obj = document.getElementsByName('snmp_trap_admin_' + i)[0];
		if (obj) obj.value = trap_admin(set_traps[i]);
	}
	
}

function valid_snmp_traps()
{
	var obj;
	for (var i = 0; i < set_traps.length; i ++){
		obj = document.getElementsByName('snmp_trap_ip_' + i)[0];
		if (obj){
			if (!valid_snmp_trapip(obj.value)){
				alert('TRAP服务器IP地址中包含无效的字符，请重新输入！');
				obj.focus();	
				return false;
			}	
		}
		obj = document.getElementsByName('snmp_trap_community_' + i)[0];
		if (obj){
			if (!valid_snmp_community(obj.value)){
				alert('TRAP团体字中包含无效的字符，请重新输入！');
				obj.focus();	
				return false;
			}	
		}
	}
	return true;
}

/* END TRAPS */
 
 

/*
function snmp_trap_enable_on_change()
{
	var f=document.forms[0];
	var c = (f.snmp_trap_enable.value == 0) ? true :false;
	c = (f.snmp_enable.value == 0) ? true : c;
	
	f.snmp_trap_ip.disabled = c;
	f.snmp_trap_port.disabled = c;
	f.snmp_trap_community.disabled = c;
	f.snmp_trap_version.disabled = c;
}
	

function snmp_enable_on_change()
{
	var f=document.forms[0];
	var c = (f.snmp_enable.value == 0) ? true :false;
	f.snmp_port.disabled = c;
	f.snmp_sysname.disabled = c;
	f.snmp_syscontact.disabled = c;
	f.snmp_syslocation.disabled = c;
	f.snmp_ro_community.disabled = c;
	f.snmp_rw_community.disabled = c;
	f.snmp_v1v2_limitation.disabled = c;
	f.snmp_trust_ip.disabled = c;
	f.snmp_ro_user.disabled = c;
	f.snmp_ro_user_pwd.disabled = c;
	f.snmp_ro_user_type.disabled = c;
	f.snmp_rw_user.disabled = c;
	f.snmp_rw_user_pwd.disabled = c;
	f.snmp_rw_user_type.disabled = c;
	f.snmp_trap_enable.disabled = c;
	
	snmp_trap_enable_on_change();
}
*/

function init()
{
	set_communities = communities;
	set_traps = traps;
	
	for (var i=0; i < syssnmp.length; i ++){
			var eq = syssnmp[i].indexOf('=');
			if ((eq > 0)
			&& document.getElementsByName(syssnmp[i].substring(0, eq))[0]){
				document.getElementsByName(syssnmp[i].substring(0, eq))[0].value = syssnmp[i].substring(eq + 1);
			}else {
				alert('Internal Error, Unknown Item:' + syssnmp[i]);
			}
	}
	
	set_community_table();
	set_trap_table();
	
	//snmp_enable_on_change();
}

function valid_snmp_info(n)
{
	var len = utf8_strlen(n);
	if (len > 63){
	  return false;	
	}
	var re = /[\"\'\\]/;
	if (re.test(n)){
		return false;
	}
	re = /^\s*$/;
	if (re.test(n)){
			return false;	
	}
	return true;
}

function valid_snmp_community(s)  
{
	// only 
  var re_any = /^([\x21-\x5d]|[\x60-\x7e]){1,31}$/;
	var re_exclude = /[\"\'\\;]/;	
	var re_sp = /^\s*$/;
	
	if (!re_any.test(s)){
		return false;
	}
	if (re_exclude.test(s)){
		return false;	
	}
	if (re_sp.test(s)){
		return false;	
	}
	return true;
}

function valid_snmpv3_user(s)     
{
	var re_any = /^\w{1,31}$/;
	var re_exclude = /[\"\'\\]/;	
	
	if (!re_any.test(s)){
		return false;
	}
	if (re_exclude.test(s)){
		return false;	
	}
	return true;
}

function valid_snmpv3_user_pwd(s)      
{
	var re_any = /^.{8,31}$/;
	var re_exclude = /[\"\'\\]/;	
	var re_sp = /^\s*$/;
	
	if (!re_any.test(s)){
		return false;
	}
	if (re_exclude.test(s)){
		return false;	
	}
	if (re_sp.test(s)){
		return false;	
	}
	return true;
}

function valid_snmp_trustip(s)       
{
	var re_ip = /^\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3}\/\d{1,2}$/;
	var re_ip1 = /^\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3}$/;
	var re_sp = /^\s*$/;
	if (re_sp.test(s)){
		return true;
	}
	
	if (!re_ip.test(s) && !re_ip1.test(s)){
		return false;
	}

	var ipms = s.split('/');
	var ips, ipm;
	if (ipms.length > 1){
		ips = ipms[0].split('.');	
		ipm = parseInt(ipms[1], 10);
	}else {
		ips = s.split('.');	
		ipm = 32;	
	}
	
	if ((ipm > 32) || (ipm < 1)){
		return false;	
	}
	for (var i = 0; i < 4; i ++){
		if (parseInt(ips[i], 10) > 255) return false;	
	}
	var a0 = parseInt(ips[0], 10);
	if ((a0 == 0) || (a0 == 127) || (a0 > 223)){
		return false;	
	}
	
	var ipv = (parseInt(ips[0],10)<<24)|(parseInt(ips[1],10)<<16)|(parseInt(ips[2], 10)<<8)|parseInt(ips[3], 10);
 	var nms = 0;
 	var mbit = 0x80000000;
 	while(ipm--){
 		nms |= mbit;
 		mbit >>= 1;
 	}
	if (ipv & ~nms){
		return false;
	}	
	
	return true;
}

function valid_snmp_trapip(s)       
{
	var re_ip = /^\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3}$/;

	if (!re_ip.test(s)){
		return false;
	}
	var ips = s.split('.');
	for (var i = 0; i < 4; i ++){
		if (parseInt(ips[i], 10) > 255) return false;	
	}
	var a0 = parseInt(ips[0], 10);
	if ((a0 == 0) || (a0 == 127) || (a0 > 223)){
		return false;	
	}
	return true;
}

function valid_snmp_port(s, def)
{
  var re = /^[1-9][0-9]*$/;  
	if (!re.test(s)){
			return false;	
	}
  s = parseInt(s, 10);  	
	
	if ((s == def) 
		|| ((s >= 1000) && (s < 65535))){
		return true;		
	}
	return false;
}

function get_init_value(item)
{
	for (var i=0; i < syssnmp.length; i ++){
			s = syssnmp[i].split('=');
			if ((s.length == 2)&& (s[0] == item)){
				 return s[1];
			}
	}	
	return "N/A";
}

function check_all_input()
{
	var f=document.forms[0];
	      
	if (!valid_snmp_port(f.snmp_port.value, 161)){ 
		alert('SNMP端口输入有误，请重新输入');
		return false;
	}           
	if (!valid_snmp_info(f.snmp_sysname.value)){
		 alert('无效的系统名称，请重新输入');
		 return false;
	}        
	if (!valid_snmp_info(f.snmp_syscontact.value)){ 
		alert('无效的系统联系方式，请重新输入');
		return false;
	}     
	if (!valid_snmp_info(f.snmp_syslocation.value)){ 
		alert('无效的系统位置信息，请重新输入');
		return false;
	}    
	
	if (!valid_snmp_communities()){
		return false;
	}
	/*   
	if (!valid_snmp_community(f.snmp_ro_community.value)){
		alert('无效的SNMP只读团体字，请重新输入');
		return false;
	}   
	if (!valid_snmp_community(f.snmp_rw_community.value)){
		alert('无效的SNMP读写团体字，请重新输入');
		return false;
	}   
	*/  
	if (!valid_snmp_trustip(f.snmp_trust_ip.value)){
		alert('无效的SNMP V1/V2 信任主机地址，请重新输入');
		return false;
		}       
	if (!valid_snmpv3_user(f.snmp_ro_user.value)){
		alert('无效的SNMP V3 只读用户名，请重新输入');
		return false;
	}        
	if (!valid_snmpv3_user_pwd(f.snmp_ro_user_pwd.value)){ 
		alert('无效的SNMP V3 只读用户密码，请重新输入');
		return false;
	}     
	if (!valid_snmpv3_user(f.snmp_rw_user.value)){
		 alert('无效的SNMP V3 读写用户名，请重新输入');
		 return false;
	}        
	if (!valid_snmpv3_user_pwd(f.snmp_rw_user_pwd.value)){ 
		alert('无效的SNMP V3 读写用户密码，请重新输入');
		return false;
	}       
	  
	if (!valid_snmp_traps()){
		return false;
	}	
	/*     
	if (!valid_snmp_trapip(f.snmp_trap_ip.value)){ 
		alert('无效的SNMP TRAP服务器IP地址，请重新输入');
		return false;
	}        
	if (!valid_snmp_port(f.snmp_trap_port.value, 162)){
		 alert('无效的SNMP TRAP端口，请重新输入');
		 return false;
	}      
	if (!valid_snmp_community(f.snmp_trap_community.value)){ 
		alert('无效的SNMP TRAP团体字，请重新输入');
		return false;
	} 	
	*/
	if (f.snmp_rw_user.value == f.snmp_ro_user.value){
		alert('SNMP V3 只读用户和读写用户相同，请重新输入');
		return false;		
	}
		
	return true;		
}


function apply()
{
	var f=document.forms[0];
	
	f.snmp_sysname.value = string_trim(f.snmp_sysname.value);      
	f.snmp_syscontact.value = string_trim(f.snmp_syscontact.value);    
	f.snmp_syslocation.value = string_trim(f.snmp_syslocation.value);    
	//f.snmp_ro_community.value = string_trim(f.snmp_ro_community.value);
	//f.snmp_rw_community.value = string_trim(f.snmp_rw_community.value);
	f.snmp_trust_ip.value = string_trim(f.snmp_trust_ip.value);      
	f.snmp_ro_user.value = string_trim(f.snmp_ro_user.value);      
	f.snmp_ro_user_pwd.value = string_trim(f.snmp_ro_user_pwd.value);     
	f.snmp_rw_user.value = string_trim(f.snmp_rw_user.value);       
	f.snmp_rw_user_pwd.value = string_trim(f.snmp_rw_user_pwd.value);    
	//f.snmp_trap_ip.value = string_trim(f.snmp_trap_ip.value);        
	//f.snmp_trap_community.value = string_trim(f.snmp_trap_community.value); 
	
	if (!check_all_input()){
		return;
	}	
	for (var i=0; i < syssnmp.length; i ++){
			var eq = syssnmp[i].indexOf('=');
			if (eq > 0){
				var item = syssnmp[i].substring(0, eq);
				diffCfg(item, item, document.getElementsByName(item)[0].value);
			}else {
				alert('Internal Error, Unknown Item:' + syssnmp[i]);				
			}
	}
	
	community_update();
	for (var i=0; i < set_communities.length; i ++){
		diffCfg('snmp_community_' + i, 'snmp_community_n', set_communities[i]);
}

	trap_update();
	for (var i=0; i < set_traps.length; i ++){
		diffCfg('snmp_trap_' + i, 'snmp_trap_n', set_traps[i]);
}
	
	subForm(f,'goform/command','NSCRTV_SNMP','n_system_snmp.asp');	
}


</script>
</head>

<body onload=init()>
<script>pageHead(env, env['web.page']);</script>
<form name=system_snmp action=goform/command method=post onsubmit="return false;">

<table width=100% cellpadding=5 cellspacing=0>
<tr ><td colspan=2 class="tb_head">基本设置</td> </tr>		
<tr>
		<td class=tb_title>SNMP 启用</td>
	<td class=tb_content>
		<select name=snmp_enable <!--onchange=snmp_enable_on_change()-->>
					<option value=0>禁用</option>
					<option value=1>启用</option>
					</select>
	</td>
</tr>
<tr>
	<td class=tb_title >SNMP 端口</td>
	<td class=tb_content ><input name=snmp_port size=5 maxlength=5> 
		默认为161, 其他可选值为(1000-65534), 请谨慎修改
	</td>
</tr>
<tr>
	<td class=tb_title>系统名称</td>
	<td class=tb_content >
	<input style="width:80%;" name=snmp_sysname maxlength=63>
	</td>
</tr>
<tr>
	<td class=tb_title >系统联系方式</td>
	<td class=tb_content >
	<input style="width:80%;" name=snmp_syscontact maxlength=63>
	</td>
</tr>
<tr>
	<td class=tb_title >系统位置</td>
	<td class=tb_content >
	<input style="width:80%;" name=snmp_syslocation maxlength=63>
	</td>
</tr>
</table>

<table width=100% border=0 align=center cellpadding=5 cellspacing=0>
<tr>
	<td colspan=2 class=tb_head>访问控制</td>
</tr>
<tr>
	<td class=tb_title style="width:20%;" >信任的主机</td>
	<td class=tb_content><input name=snmp_trust_ip size=24 maxlength=31> 
		&nbsp;&nbsp;网络地址[/子网掩码位数], 如： 192.168.0.0/24
	</td>
</tr>

<tr>
	<td class=tb_title >读写团体字</td>
	<td id=tb_community class=tb_content style="padding:0px;">
	</td>
</tr>
<tr>
	<td colspan=2 class=tb_head> 禁止SNMP v1/v2访问</td>
</tr>
<tr>
	<td class=tb_title >禁止SNMP v1/v2访问</td>
	<td class=tb_content >
		<select name=snmp_v1v2_limitation >
					<option value=0>禁用</option>
					<option value=1>启用</option>
					</select>	
	</td>
</tr>

<tr>
	<td class=tb_title >只读用户</td>
	<td class=tb_content >
		<table  cellpadding=0 cellspacing=0>
		<tr>
			<td>  用户名:&nbsp;</td> <td> <input name=snmp_ro_user size=16 maxlength=31>&nbsp;&nbsp;</td>
			<td> 密&nbsp;&nbsp;&nbsp;码:&nbsp;</td> <td> <input name=snmp_ro_user_pwd size=16 maxlength=31>&nbsp;&nbsp;</td>
			<td> 方  式: &nbsp;</td> 
		<td> <select name=snmp_ro_user_type>
					<option value=0>No Auth</option>
					<option value=1>Auth</option>
					</select>
		</td>
		</tr>				
		</table>	
	</td>
</tr>
<tr>
	<td class=tb_title >读写用户</td>
	<td class=tb_content >
		<table  cellpadding=0 cellspacing=0>
		<tr>
		<td>用户名:&nbsp;</td> <td> <input name=snmp_rw_user size=16 maxlength=31>&nbsp;&nbsp;</td>
		<td> 密&nbsp;&nbsp;&nbsp;码:&nbsp;</td> <td> <input name=snmp_rw_user_pwd size=16 maxlength=31>&nbsp;&nbsp;</td>
		<td> 方  式:&nbsp;</td> 
		<td> <select name=snmp_rw_user_type>
					<option value=0>No Auth</option>
					<option value=1>Auth</option>
					</select>
		</td>
		</tr>				
		</table>			
	</td>
</tr>
</table>


<table width=100% border=0 align=center cellpadding=5 cellspacing=0 >
<tr>
	<td colspan=2 class=tb_head>TRAP 设置</td>
</tr>
<tr>
	<td class=tb_title >TRAP 启用</td>
	<td class=tb_content >
		<select name=snmp_trap_enable <!--onchange=snmp_trap_enable_on_change()-->>
					<option value=0>禁用</option>
					<option value=1>启用</option>
					</select>	
	</td>
</tr>
<tr>
	<td class=tb_title >TRAP 设置</td>
	<td id=tb_trap class=tb_content style="padding:0px;">
	</td>
</tr>
</table>
<p>
<input class="button" type=button value="确  认" onclick=apply() >
&nbsp;&nbsp;&nbsp;
<input class="button" type=button value="取  消" onclick=init() >
</p>
</form>
<script>pageTail();</script>
</body>
</html>