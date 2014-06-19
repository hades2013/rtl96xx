<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=pageview.js></SCRIPT>
<script language=JavaScript>
	pageTitle();
	
var vlan_list = new Array(<% CGI_GET_VLAN_CONFIG(); %>);
var vlanif_list = new Array(<% CGI_GET_VLANIF_CONFIG(); %>);

var vlan_options = new Array(<% CGI_GET_VLAN_MODE_CONFIG(); %>);
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

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
		vtag[parseInt(vs[j])]=parseInt(vs[j]);
	}	
	return isAnyVlanExist(untag, vtag);		
}


function init_hybrid_list()
{
	var ts = '';
	if(get_vlanif_type('cab0') == 'hybrid') {
		ts = 'cab0;' + get_vlanif_pvid('cab0') + ';' + get_vlanif_vlans_tag('cab0') + ';' +  get_vlanif_vlans_untag('cab0');
		vlan_hybrid_list[vlan_hybrid_list.length ++] = ts;
	}
	if(get_vlanif_type('cab1') == 'hybrid') {
		ts = 'cab1;' + get_vlanif_pvid('cab1') + ';' + get_vlanif_vlans_tag('cab1')+ ';' + get_vlanif_vlans_untag('cab1');
		vlan_hybrid_list[vlan_hybrid_list.length ++] = ts;
	}
	if(get_vlanif_type('eth0') == 'hybrid') {
		ts = 'eth0;' + get_vlanif_pvid('eth0') + ';' + get_vlanif_vlans_tag('eth0')+ ';' + get_vlanif_vlans_untag('eth0');
		vlan_hybrid_list[vlan_hybrid_list.length ++] = ts;
	}
	if(get_vlanif_type('eth1') == 'hybrid') {
		ts = 'eth1;' + get_vlanif_pvid('eth1') + ';' + get_vlanif_vlans_tag('eth1')+ ';' + get_vlanif_vlans_untag('eth1');
		vlan_hybrid_list[vlan_hybrid_list.length ++] = ts;
	}			
}

function vlan_group(s)
{
	var vs = s.split(';');
	var vstr = '';
	for(var i = 0; i < vs.length; i ++) {
		vstr += port_name(vs[i]);
		if(i != vs.length - 1) vstr += ', ';
	}
	return vstr;
}

function vlan_plist(s)
{
  return s;
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
		alert ("无效的VLAN ID字串" + vlans_tag + "!");
		f.vid_tag.focus();
		return false;
	}
	if(!t.test(vlans_untag) && !isValidVlanString(vlans_untag))
	{
		alert ("无效的VLAN ID字串 " + vlans_untag + "!");
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
		if(!confirm('端口 ' + port_name(port) + ' 被设置为 Trunk 端口, 确定继续?')) return false; 
	
	for(var i = 0; i < vlan_hybrid_list.length; i ++){ 
		if( (vlan_hybrid_list[i].split(';')[0] == port) && (vlan_hybrid_list[i].split(';')[1] != f.pvid.value) ) {
		if(!confirm('端口 ' + port_name(port) + 
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
	subForm(document.forms[0],'goform/command', 'VLANIF', 'vlan_trunk.asp');
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
			vlan_ids[parseInt(vs[j])]=parseInt(vs[j]);
		}
	}
}


function init()
{
	update_vlan_ids();
	init_hybrid_list();
	pageview_add('Hybrid 端口', 0, '15%', false, vlan_group);
	pageview_add('PVID', 1, '8%', false, null);
	pageview_add('Tag VLAN列表', 2, '30%', false, vlan_plist);
	pageview_add('Untag VLAN列表', 3, '30%', false, vlan_plist);
	pageview_init(vlan_hybrid_list, 10, document.getElementById('hybrid_list'), true, checkbox_click, null, 'Hybrid VLAN 列表');
	vmchange();
	document.forms[0].tport.value = (vlan_hybrid_list.length > 0) ? vlan_hybrid_list[0].split(';')[0] : 'cab0';
	show_hybrid();
}

</script>
</head>
<body onload=init()>
<script>pageHead(webVars);</script>
<form name=vlan_trunk action=goform/command method=post>
<INPUT type=hidden name=CMD value=VLAN_TRUNK>
<INPUT type=hidden name=GO value=vlan_trunk.asp>
<div id="desc">系统VLAN在透传模式 ,
请点击<a href="vlan_mode.asp">更改VLAN模式</a>配置VLAN Hybrid 端口</div>
<div id="vlan_hybrid">
<table width=100% border=0 align=center cellpadding=5 cellspacing=1	class=box_tn>
	<tr>
		<td colspan=2 class=titleCell>添加 Hybrid</td>
	</tr>
	<tr>
		<td width=20%   class=bgblue>端口</td>
		<td width=80% class=bggrey>
			<select name="tport" onchange="show_hybrid();">
				<option value="cab0">Cable 1</option>
				<option value="cab1">Cable 2</option>
				<option value="eth0">Ethernet 1</option>
				<option value="eth1">Ethernet 2</option>
			</select>
		</td>
	</tr>
	<tr>
		<td width=20% class=bgblue>PVID</td>
		<td width=80% class=bggrey><input name="pvid" size=4 maxlength=8>
		</td>
	</tr>
	<tr>
		<td width=20%   class=bgblue>VLAN</td>
		<td width=80% class=bggrey>
			Tag:&nbsp;<input name="vid_tag" size=25 maxlength=64>&nbsp;&nbsp;&nbsp;
			Untag:&nbsp;<input name="vid_untag" size=25 maxlength=64>
		</td>
	</tr>
	<tr>
		<td colspan=2 align=center bgcolor="#9EA2A5"><input name="btn_new" type=button value="创建Hybrid"  onclick="new_hybrid();"></td>
	</tr>
</table>
<br>

<div id="hybrid_list"></div>
<table>
	<tr>
	<td width=30 valign=top>
		<INPUT id = "btn_del" type=button value="删除所选Hybrid" onclick="delete_hybrid();" disabled=true>
	</td>
	</tr>
</table>
</div>
</form>
<script>pageTail();</script>
</body>
</html>
