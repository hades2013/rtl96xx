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
	var ts = '';
	if(get_vlanif_type('cab0') == 'trunk') {
		ts = 'cab0;' + get_vlanif_pvid('cab0') + ';' + get_vlanif_vlans('cab0');
		vlan_trunk_list[vlan_trunk_list.length ++] = ts;
	}
	if(get_vlanif_type('cab1') == 'trunk') {
		ts = 'cab1;' + get_vlanif_pvid('cab1') + ';' + get_vlanif_vlans('cab1');
		vlan_trunk_list[vlan_trunk_list.length ++] = ts;
	}
	if(get_vlanif_type('eth0') == 'trunk') {
		ts = 'eth0;' + get_vlanif_pvid('eth0') + ';' + get_vlanif_vlans('eth0');
		vlan_trunk_list[vlan_trunk_list.length ++] = ts;
	}
	if(get_vlanif_type('eth1') == 'trunk') {
		ts = 'eth1;' + get_vlanif_pvid('eth1') + ';' + get_vlanif_vlans('eth1');
		vlan_trunk_list[vlan_trunk_list.length ++] = ts;
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
	if(s == '') return 'ALL (1-4094)';
	return s;
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
	//document.getElementById('btn_edit').disabled = !(cnt == 1);
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
	for (var j = 0; j < vlan_ids.length; j++)
		if (vlan_ids[j]== f.pvid.value) break;
	
	if (j == vlan_ids.length){
		alert("VLAN with VID " + f.pvid.value + " does not exist");
		f.pvid.focus();
		return false;	
	}
	
	if (f.allowed.value == '1'){
		vlans = f.vid_allowed.value;
	
		var t = /^\s*$/;
		if (1 == t.test (vlans))
		{
			alert ("VLAN ID is empty！");
			f.vid_allowed.focus();
			return false;
		}
		if(!isValidVlanString(vlans))
		{
			alert ("Invalid VLAN ID string！");
			f.vid_allowed.focus();
			return false;
		}
		if(isAnyVlanNotExist(vlans, vlan_ids))
		{
			alert("Some of VLANs do not exist in VLAN table.Please input again!");
			f.vid_allowed.focus();
			return false;	
		}		
	}else {
		vlans = '';	
	}
	
	if(get_vlanif_type(port) == 'hybrid') 
		if(!confirm('Port ' + port_name(port) + ' is set as a Hybrid VLAN port, do you want to change it?')) return false; 
	
	for(var i = 0; i < vlan_trunk_list.length; i ++){ 
		if( (vlan_trunk_list[i].split(';')[0] == port) && (vlan_trunk_list[i].split(';')[1] != f.pvid.value) ) {
		if(!confirm('Port ' + port_name(port) + 
			' has existed in Trunk VLAN List, Are you sure to set its PVID to ' + 
			f.pvid.value + ' and submit other changes?')) return false;
		} 
	}	
	
	diffCfg('vlanif_0', 'vlanif', port+';'+'trunk;'+f.pvid.value +';'+ vlans + ';');
	return true;
}

function new_trunk()
{
	var f = document.forms[0];
  if(!new_trunk_port(f.tport.value)) return;
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
			vlan_ids[parseInt(vs[j])]=parseInt(vs[j]);
		}
	}
}


function init()
{
	update_vlan_ids();
	init_trunk_list();
	pageview_add('Trunk Port', 0, '15%', false, vlan_group);
	pageview_add('PVID', 1, '10%', false, null);
	pageview_add('Allowed VLANs', 2, '55%', false, vlan_plist);
	pageview_init(vlan_trunk_list, 10, document.getElementById('trunk_list'), true, checkbox_click, null, 'Trunk VLAN List');
	vmchange();	
	document.forms[0].tport.value = (vlan_trunk_list.length > 0) ? vlan_trunk_list[0].split(';')[0] : 'cab0';
	show_trunk();
}

</script>
</head>
<body onload=init()>
<script>pageHead(webVars);</script>
<form name=vlan_1q action=goform/command method=post>
<input type=hidden name=CMD value=VLAN_TRUNK>
<input type=hidden name=GO value=vlan_trunk.asp>
<div id="desc">System is in VLAN transparent mode ,
Please<a href="vlan_mode.asp">change VLAN mode setting</a> to configurate Trunk VLAN.
</div>	
<div id="vlan_trunk">
<table width=100% border=0 align=center cellpadding=5 cellspacing=1	class=box_tn>
	<tr>
		<td colspan=2 class=titleCell>Add Trunk</td>
	</tr>
	<tr>
		<td width=20%   class=bgblue>Port</td>
		<td width=80% class=bggrey>
			<select name="tport" onchange="show_trunk();">
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
		<td width=20%   class=bgblue>Allowed VLANs</td>
		<td width=80% class=bggrey>
			<select name="allowed" onchange="show_trunk_vid();">
				<option value="0">All VLANs</option>
				<option value="1">Specified VLANs</option>
			</select>
			&nbsp;&nbsp;VLAN ID:&nbsp;
			<input name="vid_allowed" size=35 maxlength=64>
		</td>
	</tr>
	<tr><td colspan=2 align=center bgcolor="#9EA2A5"><input name="btn_new" type=button value="Create Trunk"  onclick="new_trunk();"></td></tr>
</table>
<br>

<div id="trunk_list"></div>
<table>
	<tr>
	<td width=30 valign=top>
		<INPUT id ="btn_del" type=button value="Delete Selected Trunks" onclick="delete_trunk();" disabled=true>
	</td>
	</tr>
</table>
</div>
</form>
<script>pageTail();</script>
</body>
</html>
