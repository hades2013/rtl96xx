<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="content-type" content="text/html;charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=pageview.js></SCRIPT>
<script language=JavaScript>

pageTitle();


var vlan_options = new Array(<% CGI_GET_VLAN_MODE_CONFIG(); %>);
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

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
	}
	else {
		document.getElementById('vm_transparent').style.display = 'none';
	}		
}

function apply()
{
	var f=document.forms[0];
	if(f.vlan_mode.value != vlan_mode()) {
		if(!confirm('VLAN mode change will cause the lose of VLAN previous settings. Do you want to continue?')) return;
		if(f.vlan_mode.value == '1') {
			diffCfg('vlan_0', 'vlan_mode', f.vlan_mode.value);
			diffCfg('vlan_1', 'port_type', 'eth0;' + f.type_eth0.value);
			diffCfg('vlan_2', 'port_type', 'eth1;' + f.type_eth1.value);
			diffCfg('vlan_3', 'port_type', 'cab0;' + f.type_cab0.value);
			diffCfg('vlan_4', 'port_type', 'cab1;' + f.type_cab1.value);			
		}
		else {
			diffCfg('vlan_0', 'vlan_mode', f.vlan_mode.value);
		}
	}
	subForm(f,'goform/command','VLAN_MODE','vlan_mode.asp');
}


function init()
{
	var f=document.forms[0];
	f.vlan_mode.value = vlan_mode();
	f.type_cab0.value = get_linktype('cab0');
	f.type_cab1.value = get_linktype('cab1');
	f.type_eth0.value = get_linktype('eth0');
	f.type_eth1.value = get_linktype('eth1');	
	vmchange();
}
</script>
</head>

<body onload=init()>
<script>pageHead(webVars);</script>

<form name=vlan_8021q action=goform/command method=post>
<INPUT type=hidden name=CMD value=WAN_CON>
<INPUT type=hidden name=GO value=vlan_8021q.asp>
<INPUT type=hidden name=action>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1	class=box_tn>
	<tr>
		<td width=30% class=titleCell>VLAN Mode</td>
		<td width=70% class=bggrey>
			<select name='vlan_mode' onchange="vmchange()";>
				<option value="0">802.1q VLAN</option>
				<option value="1">Transparent VLAN</option>
			</select>
		</td>
	</tr>
</table>
<div id="vm_transparent">
<table width=100% border=0 align=center cellpadding=5 cellspacing=1	class=box_tn>
	<tr>
		<td colspan=2 class=titleCell>VLAN ports setting in transparent mode</td>
	</tr>
	<tr>
		<td class=bgblue>Cable 1</td>
		<td width=70% class=bggrey>
			<select name='type_cab0' disabled="true">
				<option value="access">Management</option>
				<option value="trunk">Data Traffic</option>
			</select>
		</td>
	</tr>	
	<tr>
		<td class=bgblue>Cable 2</td>
		<td width=70% class=bggrey>
			<select name='type_cab1' disabled="true">
				<option value="access">Management</option>
				<option value="trunk">Data Traffic</option>
			</select>
		</td>
	</tr>
	<tr>
		<td class=bgblue>Ethernet 1</td>
		<td width=70% class=bggrey>
			<select name='type_eth0'>
				<option value="access">Management</option>
				<option value="trunk">Data Traffic</option>
			</select>
		</td>
	</tr>
	<tr>
		<td class=bgblue>Ethernet 2</td>
		<td width=70% class=bggrey>
			<select name='type_eth1'>
				<option value="access">Management</option>
				<option value="trunk">Data Traffic</option>
			</select>
		</td>
	</tr>
</table>
</div>
<p>
<input type=button value="  OK  " onclick="apply();">&nbsp;&nbsp;&nbsp;&nbsp;
<input type=button value="Cancel" onclick="init();">
</p>

</form>
<script>pageTail();</script>
</body>
</html>
