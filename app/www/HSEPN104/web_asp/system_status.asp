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
/* just foe test
var webVars = new Array( 'EPN104', '0', '0');
var sysinfo = new Array('1147','4094','EPN104V100R001','VER.B','00:00:00:00:00:00','00:00:00:00:00:00', 'EE:EE:62:EE:EE:FF', 'Wed Nov 14 01:45:55 2012');
var sysip = new Array('ALLWIN-LOSO;EE:EE:62:EE:EE:FF;static;4094','ok;192.168.0.10;255.255.255.0;0.0.0.0;192.168.0.1;0.0.0.0;0.0.0.0','stop;0.0.0.0;0.0.0.0;0.0.0.0;0.0.0.0;0.0.0.0;0.0.0.0;0;');

*/


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

function get_mvlan()
{
	if (sysip.length > 0){
		var vs = sysip[0].split(';');
		if (vs.length > 3) return vs[3];
	}
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
//addCfg('SYSMAC','lan_mac_addr', get_mac());
addCfg('SYSMAC','lan_mac_addr', sysinfo[6]);
addCfg('SYSIP','lan_ip_addr', get_ip());
addCfg('SYSNM','vlan_interface_1_netmask', get_nm());
addCfg('SYSVID','vlan_man_vid', get_mvlan());
//addCfg('SYSVID','vlan_man_vid', sysinfo[1]);
//addCfg('CABMAC0','port_mac_addr_cab0',sysinfo[4]);
//addCfg('CABMAC1','port_mac_addr_cab1',sysinfo[5]);
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
	+ d + ((d > 1) ? " 天 " : " 天 ") 
	+ h + ((h > 1) ? " 时 " : " 时 ") 
	+ m + ((m > 1) ? " 分 " : " 分 ") 
	+ s + ((s > 1) ? " 秒 " : " 秒") ;	
}

</script>
</head>
<body >
<script>pageHead(webVars);</script>
<form name=system_status action=goform/command method=post>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1
	class=box_tn>
	<tr>
		<td class=bggrey2 align=center><font size=4>版本信息</font></td>
	</tr>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1
			bgcolor="#FFFFFF">
			<tr>
				<td class=bgblue>软件版本</td>
				<td class=bggrey><b><script>document.write(getCfg('SWVER'));</script></b>
				</td>
			</tr>
			<tr>
				<td class=bgblue>硬件版本</td>
				<td class=bggrey><b><script>document.write(getCfg('HWVER'));</script></b></td>
			</tr>	
		</table>
		</td>
	</tr>
	
	<tr>
		<td class=bggrey2 align=center><font size=4>管理参数</font></td>
	</tr>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1
			bgcolor="#FFFFFF">
			<tr>
				<td class=bgblue>MAC 地址</td>
				<td class=bggrey><b><script>document.write(getCfg('SYSMAC'));</script></b>
				</td>
			</tr>
			<tr>
				<td class=bgblue>管理VLAN</td>
				<td class=bggrey><b><script>document.write(getCfg('SYSVID'));</script></b></td>
			</tr>
			<tr>
				<td class=bgblue>IP 地址</td>
				<td class=bggrey><b><script>document.write(getCfg('SYSIP'));</script></b></td>
			</tr>	
			<tr>
				<td class=bgblue>子网掩码</td>
				<td class=bggrey><b><script>document.write(getCfg('SYSNM'));</script></b>
				</td>
			</tr>
			
                       
		</table>
		</td>
	</tr>
	<!--
	<tr>
		<td class=bggrey2 align=center><font size=4>Cable 端口参数</font></td>
	</tr>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1
			bgcolor="#FFFFFF">
			<tr>
				<td class=bgblue>Cable1 MAC 地址</td>
				<td class=bggrey><b><script>document.write(getCfg('CABMAC0'));</script></b>
				</td>
			</tr>
			<tr>
				<td class=bgblue>Cable2 MAC 地址</td>
				<td class=bggrey><b><script>document.write(getCfg('CABMAC1'));</script></b>
				</td>
			</tr>
			<tr>
				<td class=bgblue>在线终端数量</td>
				<td class=bggrey><b><script>document.write(online);</script></b></td>
			</tr>			
		</table>
		</td>
	</tr>	
	-->
	<tr>
		<td class=bggrey2 align=center><font size=4><script>document.write(show_runtime());</script></font></td>
	</tr>
	
</table>
<p>
<input type=button value=' 刷新 '  onclick=refresh("system_status.asp")>
</p>
</form>

<script>pageTail();</script>
</body>
</html>
