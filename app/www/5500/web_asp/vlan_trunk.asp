<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - Trunk VLAN设置 </title>
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
env['sys.time'] = '946684941';
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
//var vlan_list = new Array('0;eth1,eth0,eth2,eth3;1');
var vlan_list = new Array(<% CGI_GET_VLAN_CONFIG(); %>);
var vlanif_list =  new Array(<% CGI_GET_VLANIF_CONFIG();%>);
//var vlanif_list = new Array('cpu0;access;1;;',
//'eth0;access;1;;',
//'eth1;trunk;1;;',
//'cab0;trunk;1;;',
//'cab1;trunk;1;;',
//'cab2;trunk;1;;',
//'cab3;trunk;1;;');
//var vlan_options = new Array('0','1','cpu0;access','eth0;access','eth1;trunk','cab0;trunk','cab1;trunk','cab2;trunk','cab3;trunk');
var vlan_options=new Array( <% CGI_GET_VLAN_OPTIONS(); %>);
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
	/*
	var re_cpu = /^cpu[0-9]{1,}$/;
	var re_eth = /^GE[0-9]{1,}$/;
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
		  i = parseInt(sh.substring(i), 10) ;
	}
	base += ((i != -1) ? i : "");*/
	return sh;
}

/* end port const functions */

function port_select_options()
{
	var i, sh, re = "";
	for (i = 0; i < vlanif_list.length; i ++){
		sh = vlanif_list[i].split(';')[0];
		if (port_if_cpu(sh))continue;
		re += "<option value='" + sh + "'> " + port_fancy_name(sh) + "</option>\n";	  	
	}
	return re;
}


var vlan_ids = new Array();
var vlan_trunk_list = new Array();

function vlan_mode()
{
	if (vlan_options.length > 0){
		return vlan_options[0];	
	}	
	return '0';
}

function vlan_mvid()
{
	if (vlan_options.length > 1){
		return vlan_options[1];	
	}	
	return '1';		
}


function get_vlanif_type(p)
{
	for (var i = 0; i < vlanif_list.length; i ++){
		var vs = vlanif_list[i].split(';');
		if (vs[0] == p) return vs[1];
	}
	return '';	
}

function get_vlanif_pvid(p)
{
	for (var i = 0; i < vlanif_list.length; i ++){
		var vs = vlanif_list[i].split(';');
		if (vs[0] == p) return vs[2];
	}
	return '';	
}

function get_vlanif_vlans(p)
{
	for (var i = 0; i < vlanif_list.length; i ++){
		var vs = vlanif_list[i].split(';');
		if (vs[0] == p) return vs[3];
	}
	return '';	
}

function init_trunk_list()
{
	var i, ts = '';
	for (i = 0; i < vlanif_list.length; i ++){
		sh = vlanif_list[i].split(';')[0];
		if (port_if_cpu(sh))continue;
		if(get_vlanif_type(sh) == 'trunk') {
			ts = sh + ';' + get_vlanif_pvid(sh) + ';' + get_vlanif_vlans(sh);
			vlan_trunk_list[vlan_trunk_list.length ++] = ts;
		}	
	}	
}

function vlan_group(s)
{
	var vs = s.split(';');
	var vstr = '';
	for(var i = 0; i < vs.length; i ++) {
		vstr += port_fancy_name(vs[i]);
		if(i != vs.length - 1) vstr += ', ';
	}
	return vstr;
}

function vlanid_string(s)
{
	var thr = 54;
	if (s.length > thr){
		var ss = s.split(',');
		var i, base = 0, r = '';
		for (i = 0; i < ss.length; i ++){
			if ((s.indexOf(ss[i]) + ss[i].length - base) > thr){
				base = s.indexOf(ss[i]) + ss[i].length;
				if (r != '')r += ',</br>';
			}else {
				if (r != '')r += ',';				
			}	
			r += ss[i];							
		}	
		return r;
	}else {
		return s;
	}
}

function vlan_plist(s)
{
	if(s == '') return '所有 (1-4094)';
	return vlanid_string(s);
}



function checkbox_click(item)
{
	var cnt = 0;
	for(var i = 0; i < vlan_trunk_list.length; i ++) {
		if(pageview_ischecked(i)){
			cnt ++;	
		}
	}
	document.getElementById('btn_del').disabled = !(cnt > 0);
}

function show_trunk(){
	var f = document.forms[0];
	var p = f.tport.value;
	var vlans = (get_vlanif_type(p) == 'trunk') ? get_vlanif_vlans(p) : '';
	f.pvid.value = get_vlanif_pvid(p);
	f.allowed.value = (vlans == '') ? '0' : '1';
	f.vid_allowed.disabled = (f.allowed.value == '0');
	f.vid_allowed.value = (!f.vid_allowed.disabled) ? vlans : ''; 
}

function show_trunk_vid() {
	var f = document.forms[0];
	f.vid_allowed.disabled = (f.allowed.value == '0');
	if (!f.vid_allowed.disabled) f.vid_allowed.focus();
}

function new_trunk_port(port)
{
	var f = document.forms[0];
	var vlans;
	
	if(vlan_valid(f.pvid.value) == false)
		return false;
	
	for (var j = 0; j < vlan_ids.length; j++)
		if (vlan_ids[j]== f.pvid.value) break;
	
	if (j == vlan_ids.length){
		alert("VLAN " + f.pvid.value + " 不存在,请重新输入!");
		f.pvid.focus();
		return false;	
	}
	
	if (f.allowed.value == '1'){
		vlans = f.vid_allowed.value;
	
		var t = /^\s*$/;
		if (1 == t.test (vlans))
		{
			alert ("不能为空！");
			f.vid_allowed.focus();
			return false;
		}
		if(!isValidVlanString(vlans))
		{
			alert ("VLAN ID字串包含非法字符！");
			f.vid_allowed.focus();
			return false;
		}
		if(isAnyVlanNotExist(vlans, vlan_ids))
		{
			alert("一个或多个VLAN不存在,请重新输入!");
			f.vid_allowed.focus();
			return false;	
		}		
	}else {
		vlans = '';	
	}
	
	if(get_vlanif_type(port) == 'hybrid') 
		if(!confirm('端口 ' + port_fancy_name(port) + ' 已经被设置为 Hybrid 端口, 确定要继续?')) return false; 
	
	for(var i = 0; i < vlan_trunk_list.length; i ++){ 
		if( (vlan_trunk_list[i].split(';')[0] == port) && (vlan_trunk_list[i].split(';')[1] != f.pvid.value) ) {
		if(!confirm('端口 ' + port_fancy_name(port) + 
			' 已经存在于 Trunk 列表中, 确定将它的PVID改成  ' + 
			f.pvid.value + ', 并更新其VLAN设置?')) return false;
		} 
	}	
	
	diffCfg('vlanif_0', 'vlanif', port+';'+'trunk;'+f.pvid.value +';'+ vlans + ';');
	return true;
}

function new_trunk()
{
	var f = document.forms[0];
  if(!new_trunk_port(f.tport.value)) return;
  if(!confirm("如果当前通信端口的pvid与管理vlan不同,与网页的连接将会被断开，是否继续？")) 
		return false;
  subForm(f,'goform/command', 'VLANIF', 'vlan_trunk.asp');
}

function delete_trunk()
{
	var port;
	for(var i = 0; i < vlan_trunk_list.length; i ++) 
		if(pageview_ischecked(i)) {
			port = vlan_trunk_list[i].split(';')[0];
 			diffCfg('vlanif_' + i, 'vlanif', port+';'+'access;1;;');
		}
	subForm(document.forms[0],'goform/command', 'VLANIF', 'vlan_trunk.asp');
}

function vmchange()
{
	if(vlan_mode() == '0') {
		document.getElementById('vlan_trunk').style.display = 'block';
		document.getElementById('desc').style.display = 'none';
	}else {
		document.getElementById('desc').style.display = 'block';
		document.getElementById('vlan_trunk').style.display = 'none';
	}		
}

function update_vlan_ids()
{
	vlan_ids = new Array();	
	for(var i = 0; i < vlan_list.length; i ++) {
		var vs=parse_vlan(vlan_list[i].split(';')[2]);//.split(',');	
		for (var j=0; j < vs.length; j++){
			vlan_ids[parseInt(vs[j], 10)]=parseInt(vs[j], 10);
		}
	}
}


function init()
{
	update_vlan_ids();
	init_trunk_list();
	pageview_add('Trunk端口', 0, '15%', false, vlan_group);
	pageview_add('PVID', 1, '10%', false, null);
	pageview_add('允许通过的VLAN', 2, '55%', false, vlan_plist);
	pageview_init(vlan_trunk_list, 10, document.getElementById('trunk_list'), true, checkbox_click, null, 'Trunk VLAN列表');
	vmchange();	
	document.forms[0].tport.value = (vlan_trunk_list.length > 0) ? vlan_trunk_list[0].split(';')[0] : 'eth0';
	show_trunk();
}

</script>
</head>
<body onload=init()>
<script>pageHead(env, env['web.page']);</script>
<form name=vlan_1q action=goform/command method=post onsubmit="return false;">
<input type=hidden name=CMD value=VLAN_TRUNK>
<input type=hidden name=GO value=vlan_trunk.asp>
<div id="desc">系统VLAN在透传模式 ,
请点击<a href="vlan_mode.asp">更改VLAN模式</a>配置VLAN Trunk 端口</div>
<div id="vlan_trunk">
<table width=100% cellpadding=5 cellspacing=0>
	<tr ><td colspan=2 class="tb_head">修改或创建Trunk端口</td> </tr>		
	<tr>
		<td class=tb_title >端口</td>
		<td class=tb_content>
			<select name="tport" onchange="show_trunk();">
			<script> document.write(port_select_options()) </script>	
			</select>
		</td>
	</tr>
	<tr>
		<td class=tb_title>PVID</td>
		<td class=tb_content><input name="pvid" size=5 maxlength=4>
		</td>
	</tr>
	<tr>
		<td class=tb_title>允许通过的VLAN</td>
		<td class=tb_content>
			<select name="allowed" onchange="show_trunk_vid();">
				<option value="0">所有VLAN</option>
				<option value="1">指定VLAN</option>
			</select>
			&nbsp;&nbsp;
			<input name="vid_allowed" size=50> &nbsp;
			<input name="btn_new" class="button" type=button value="修改或创建Trunk端口"  onclick="new_trunk();">
		</td>
	</tr>
</table>
<br>

<div id="trunk_list"></div>
<table>
	<tr>
	<td width=30 valign=top>
		<INPUT id ="btn_del" class="button" type=button value="删除所选Trunk" onclick="delete_trunk();" disabled=true>
	</td>
	</tr>
</table>
</div>
</form>
<script>pageTail();</script>
</body>
</html>