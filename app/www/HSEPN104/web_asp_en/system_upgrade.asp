<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<script language=JavaScript>

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var sysinfo = new Array(<% CGI_SYS_INFO(); %>);

addCfg('SWVER','fw_version', sysinfo[2]);
addCfg('HWVER','device_hw_rev', sysinfo[3]);
addCfg('HWADDR','lan_mac_addr', sysinfo[6]);

pageTitle();

function apply()
{
	var f=document.forms[0];
	if (f.uploadfile.value=="")
    {
		alert("Please select a firmware file!");
		return ;
	}
    
	if (confirm('System will be in process of firmware upgrade, it will take some time, please do not shutdown the device. Continue?'))
	{			
		f.submit();	
	}
}

function init()
{

}
</script>
</head>
<body onload=init();>
<script>pageHead(webVars);</script>

<form name=system_upgrade action=goform/upldForm method=post enctype="multipart/form-data">
<input type=hidden name=TYPE value=UFILE>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td colspan=2 width=100% class=greybluebg>Upgrade from a local firmware</td>
</tr>	
<tr>
	<td width=%20 class=bgblue>
		<b>Hardware Version</b>
	</td>
	<td width=%80 class=bggrey>
		<b><script>document.write(getCfg('HWVER'));</script></b>
	</td>
</tr>
<tr>
	<td width=%20 class=bgblue>
		<b>MAC Address</b>
	</td>
	<td width=%80 class=bggrey>
		<b><script>document.write(getCfg('HWADDR'));</script></b>
	</td>
</tr>
<tr>
	<td width=%20 class=bgblue>
		<b>Firmware Version</b>
	</td>
	<td width=%80 class=bggrey>
		<b><script>document.write(sysinfo[2]);</script></b>
	</td>
</tr>
<tr>
	<td class=bgblue><b>Released Date</b></td>
	<td class=bggrey>
		<b><script>document.write(sysinfo[7]);</script></b>
	</td>
</tr>
<tr>
	<td class=bgblue><b>Firmware Upgrade</b></td>
	<td class=bggrey><input type=file size=30 name=uploadfile><input type=button value="  OK  " onclick=apply();></td>
</tr>
</table>
</form>

<script>pageTail();</script>
</body>
</html>
