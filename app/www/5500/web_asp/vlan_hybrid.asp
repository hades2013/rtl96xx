<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - Hybrid VLAN设置 </title>
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
env['sys.time'] = '946684955';
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
var vlan_list = new Array(<% CGI_GET_VLAN_CONFIG(); %>);
var vlanif_list =  new Array(<% CGI_GET_VLANIF_CONFIG();%>);
//var vlan_list = new Array('0;cab0,cab1,cab2,cab3,eth0,eth1;1');
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
var vlan_hybrid_list = new Array();

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

function get_vlanif_vlans_tag(p)
{
	for (var i = 0; i < vlanif_list.length; i ++){
		var vs = vlanif_list[i].split(';');
		if (vs[0] == p) return vs[3];
	}
	return '';	
}

function get_vlanif_vlans_untag(p)
{
	for (var i = 0; i < vlanif_list.length; i ++){
		var vs = vlanif_list[i].split(';');
		if (vs[0] == p) return vs[4];
	}
	return '';	
}


function isAnyVlanConflict(tag, untag)
{
	if (tag == '' || untag == ''){
		return false;	
	}	
	var vtag = new Array();
	var vs=parse_vlan(tag);
	for (var j=0; j < vs.length; j++){
		vtag[parseInt(vs[j], 10)]=parseInt(vs[j], 10);
	}	
	return isAnyVlanExist(untag, vtag);		
}


function init_hybrid_list()
{
	var i, ts = '';
	for (i = 0; i < vlanif_list.length; i ++){
		sh = vlanif_list[i].split(';')[0];
		if (port_if_cpu(sh))continue;
		if(get_vlanif_type(sh) == 'hybrid') {
		ts = sh + ';' + get_vlanif_pvid(sh) + ';' + get_vlanif_vlans_tag(sh)+ ';' + get_vlanif_vlans_untag(sh);
			vlan_hybrid_list[vlan_hybrid_list.length ++] = ts;
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
	var thr = 38;
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

function checkbox_click(item)
{
	var cnt = 0;
	for(var i = 0; i < vlan_hybrid_list.length; i ++) {
		if(pageview_ischecked(i)){
			cnt ++;	
		}
	}
	document.getElementById('btn_del').disabled = !(cnt > 0);
}

function show_hybrid(){
	var f = document.forms[0];
	var p = f.tport.value;
	f.pvid.value = get_vlanif_pvid(p);
	f.vid_tag.value = (get_vlanif_type(p) == 'hybrid') ? get_vlanif_vlans_tag(p) : '';
	f.vid_untag.value = (get_vlanif_type(p) == 'hybrid') ? get_vlanif_vlans_untag(p) : '';
}


function new_hybrid_port(port)
{
	var f = document.forms[0];

	if(vlan_valid(f.pvid.value) == false)
		return false;
	
	for (var j = 0; j < vlan_ids.length; j++)
		if (vlan_ids[j]== f.pvid.value) break;
	
	if (j == vlan_ids.length){
		alert("VLAN " + f.pvid.value + " 不存在,请重新输入!");
		f.pvid.focus();
		return false;	
	}

	var vlans_tag = f.vid_tag.value;
	var vlans_untag = f.vid_untag.value;
	var t = /^\s*$/;
	
	if (t.test(vlans_tag) && t.test(vlans_untag)){
			alert('请至少输入一个VLAN!');
			f.vid_tag.focus();
			return false;
	}
	
	if(!t.test(vlans_tag) && !isValidVlanString(vlans_tag))
	{
		alert ("VLAN ID字串包含非法字符" + vlans_tag + "!");
		f.vid_tag.focus();
		return false;
	}
	if(!t.test(vlans_untag) && !isValidVlanString(vlans_untag))
	{
		alert ("VLAN ID字串包含非法字符 " + vlans_untag + "!");
		f.vid_untag.focus();
		return false;
	}
		
	if(!t.test(vlans_tag) && isAnyVlanNotExist(vlans_tag, vlan_ids))
	{
		alert("Tag 列表中含有不存的VLAN ID,请重新输入!");
		f.vid_tag.focus();
		return false;	
	}		

	if(!t.test(vlans_untag) && isAnyVlanNotExist(vlans_untag, vlan_ids))
	{
		alert("Untag 列表中含有不存的VLAN ID,请重新输入!");
		f.vid_untag.focus();
		return false;	
	}	
	
	if (isAnyVlanConflict(vlans_tag, vlans_untag)){
		alert('一个或多个VLAN同时存在Tag列表及Untag列表中!');
		f.vid_tag.focus();		
		return false;	
	}
	
	if(get_vlanif_type(port) == 'trunk') 
		if(!confirm('端口 ' + port_fancy_name(port) + ' 已经被设置为 Trunk 端口, 确定要继续?')) return false; 
	
	for(var i = 0; i < vlan_hybrid_list.length; i ++){ 
		if( (vlan_hybrid_list[i].split(';')[0] == port) && (vlan_hybrid_list[i].split(';')[1] != f.pvid.value) ) {
		if(!confirm('端口 ' + port_fancy_name(port) + 
			' 已经存在于 Hybrid 列表中, 确定将它的PVID改成 ' + 
			f.pvid.value + ',并更新其VLAN设置?')) return false;
		} 
	}	
 diffCfg('vlanif_0', 'vlanif', port+';'+'hybrid;'+f.pvid.value +';'+ vlans_tag +';'+ vlans_untag);
	return true;
}

function new_hybrid()
{
    var f = document.forms[0];
    if(!new_hybrid_port(f.tport.value)) return;
	if(!confirm("如果当前通信端口的pvid与管理vlan不同,与网页的连接将会被断开，是否继续？")) 
		return false;
    subForm(f,'goform/command', 'VLANIF', 'vlan_hybrid.asp');
}

function delete_hybrid()
{
	var port;
	for(var i = 0; i < vlan_hybrid_list.length; i ++) 
		if(pageview_ischecked(i)) {
			port = vlan_hybrid_list[i].split(';')[0];
 			diffCfg('vlanif_' + i, 'vlanif', port+';'+'access;1;;');
		}
	subForm(document.forms[0],'goform/command', 'VLANIF', 'vlan_hybrid.asp');
}

function vmchange()
{
	if(vlan_mode() == '0') {
		document.getElementById('vlan_hybrid').style.display = 'block';
		document.getElementById('desc').style.display = 'none';
	}
	else {
		document.getElementById('vlan_hybrid').style.display = 'none';
		document.getElementById('desc').style.display = 'block';
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
	init_hybrid_list();
	pageview_add('Hybrid 端口', 0, '15%', false, vlan_group);
	pageview_add('PVID', 1, '8%', false, null);
	pageview_add('Tag VLAN列表', 2, '30%', false, vlanid_string);
	pageview_add('Untag VLAN列表', 3, '30%', false, vlanid_string);
	pageview_init(vlan_hybrid_list, 10, document.getElementById('hybrid_list'), true, checkbox_click, null, 'Hybrid VLAN列表');
	vmchange();
	document.forms[0].tport.value = (vlan_hybrid_list.length > 0) ? vlan_hybrid_list[0].split(';')[0] : 'eth0';
	show_hybrid();
}

</script>
</head>
<body onload=init()>
<script>pageHead(env, env['web.page']);</script>
<form name=vlan_trunk action=goform/command method=post onsubmit="return false;">
<INPUT type=hidden name=CMD value=VLAN_TRUNK>
<INPUT type=hidden name=GO value=vlan_trunk.asp>
<div id="desc">系统VLAN在透传模式 ,
请点击<a href="vlan_mode.asp">更改VLAN模式</a>配置VLAN Hybrid 端口</div>
<div id="vlan_hybrid">
<table width=100% cellpadding=5 cellspacing=0>
		<tr ><td colspan=2 class="tb_head">修改或创建Hybrid端口</td> </tr>			
	<tr>
		<td width=20%   class=tb_title>端口</td>
		<td width=80% class=tb_content>
			<select name="tport" onchange="show_hybrid();">
			<script> document.write(port_select_options()) </script>	
			</select>
		</td>
	</tr>
	<tr>
		<td width=20% class=tb_title>PVID</td>
		<td width=80% class=tb_content><input name="pvid" size=5 maxlength=4>
		</td>
	</tr>
	<tr>
		<td width=20%   class=tb_title>Tagged VLAN</td>
		<td width=80% class=tb_content>
			<input name="vid_tag" size=64>
		</td>
	</tr>
	<tr>
		<td width=20%   class=tb_title>Untagged VLAN</td>
		<td width=80% class=tb_content>
		<input name="vid_untag" size=64>
		</td>
	</tr>	
	<tr>
		<td colspan=2 class=tb_message><input name="btn_new" type=button value="提 交"  onclick="new_hybrid();"></td>
	</tr>
</table>
<br>

<div id="hybrid_list"></div>
<table>
	<tr>
	<td width=30 valign=top>
		<INPUT id = "btn_del" class="button" type=button value="删除所选Hybrid" onclick="delete_hybrid();" disabled=true>
	</td>
	</tr>
</table>
</div>
</form>
<script>pageTail();</script>
</body>
</html>