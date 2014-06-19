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
var sysinfo = new Array(<% CGI_SYS_INFO(); %>);
var sysip = new Array(<% CGI_SYS_IP(); %>);

function get_mac()
{
	if (sysip.length > 0){
		var vs = sysip[0].split(';');
		if (vs.length > 1) return vs[1];
	}
	return 'static';
}

function get_ip_proto()
{
	if (sysip.length > 0){
		var vs = sysip[0].split(';');
		if (vs.length > 2) return vs[2];
	}
	return 'static';
}

function get_ip()
{
	var type = get_ip_proto();
	var item = (type == 'static') ? 1 : 2;
	if (sysip.length > item){
		var vs = sysip[item].split(';');
		if (vs.length > 1) return vs[1];		
	}
	return '0.0.0.0';
}

function get_nm()
{
		var type = get_ip_proto();
	var item = (type == 'static') ? 1 : 2;
	if (sysip.length > item){
		var vs = sysip[item].split(';');
		if (vs.length > 2) return vs[2];		
	}
	return '0.0.0.0';
}

function get_gw(type)
{
		var type = get_ip_proto();
	var item = (type == 'static') ? 1 : 2;
	if (sysip.length > item){
		var vs = sysip[item].split(';');
		if (vs.length > 3) return vs[3];		
	}
	return '0.0.0.0';
}

addCfg('SWVER','fw_version', sysinfo[2]);
addCfg('HWVER','device_hw_rev', sysinfo[3]);
addCfg('SYSMAC','lan_mac_addr', get_mac());
addCfg('SYSIP','lan_ip_addr', get_ip());
addCfg('SYSNM','lan_ip_mask', get_nm());
addCfg('SYSVID','vlan_man_vid', sysinfo[1]);
addCfg('CABMAC0','port_mac_addr_cab0',sysinfo[4]);
addCfg('CABMAC1','port_mac_addr_cab1',sysinfo[5]);
var runtime= sysinfo[0];

var client_list = new Array (<% CGI_CLIENT_QUERY("ALL", "0"); %>);
var online = client_list.length;

function show_runtime()
{
	var d,h,m, s;
	var r = runtime;
	d = Math.floor(r / (60*60*24));
	r = (r % (60*60*24));
	h = Math.floor(r / (60*60));
	r = (r % (60*60));
	m = Math.floor(r / 60);
	s = Math.floor(r % 60);
	return "Runtime: " 
	+ d + ((d > 1) ? " days " : " day ") 
	+ h + ((h > 1) ? " hours " : " hour ") 
	+ m + ((m > 1) ? " minutes " : " minute ") 
	+ s + ((s > 1) ? " seconds " : " second") ;	
}

</script>
</head>
<body >
<script>pageHead(webVars);</script>
<form name=system_status action=goform/command method=post>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1
	class=box_tn>
	<tr>
		<td class=bggrey2 align=center><font size=4>Version Info</font></td>
	</tr>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1
			bgcolor="#FFFFFF">
			<tr>
				<td class=bgblue>Software Version</td>
				<td class=bggrey><b><script>document.write(getCfg('SWVER'));</script></b>
				</td>
			</tr>
			<tr>
				<td class=bgblue>Hardware Version</td>
				<td class=bggrey><b><script>document.write(getCfg('HWVER'));</script></b></td>
			</tr>	
		</table>
		</td>
	</tr>
	
	<tr>
		<td class=bggrey2 align=center><font size=4>Management Parameters</font></td>
	</tr>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1
			bgcolor="#FFFFFF">
			<tr>
				<td class=bgblue>MAC Address</td>
				<td class=bggrey><b><script>document.write(getCfg('SYSMAC'));</script></b>
				</td>
			</tr>
			<tr>
				<td class=bgblue>IP Address</td>
				<td class=bggrey><b><script>document.write(getCfg('SYSIP'));</script></b></td>
			</tr>	
			<tr>
				<td class=bgblue>Subnet Mask</td>
				<td class=bggrey><b><script>document.write(getCfg('SYSNM'));</script></b>
				</td>
			</tr>
			<tr>
				<td class=bgblue>Management VLAN</td>
				<td class=bggrey><b><script>document.write(getCfg('SYSVID'));</script></b></td>
			</tr>			
		</table>
		</td>
	</tr>
	
	<tr>
		<td class=bggrey2 align=center><font size=4>Cable Ports Parameters</font></td>
	</tr>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1
			bgcolor="#FFFFFF">
			<tr>
				<td class=bgblue>Cable1 MAC Address</td>
				<td class=bggrey><b><script>document.write(getCfg('CABMAC0'));</script></b>
				</td>
			</tr>
			<tr>
				<td class=bgblue>Cable2 MAC Address</td>
				<td class=bggrey><b><script>document.write(getCfg('CABMAC1'));</script></b>
				</td>
			</tr>
			<tr>
				<td class=bgblue>Online Slaves</td>
				<td class=bggrey><b><script>document.write(online);</script></b></td>
			</tr>			
		</table>
		</td>
	</tr>	
	
	<tr>
		<td class=bggrey2 align=center><font size=4><script>document.write(show_runtime());</script></font></td>
	</tr>
	
</table>
<p>
<input type=button value=' Refresh '  onclick=refresh("system_status.asp")>
</p>
</form>

<script>pageTail();</script>
</body>
</html>
