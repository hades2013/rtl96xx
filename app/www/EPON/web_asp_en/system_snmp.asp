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
var syssnmp = new Array(<% CGI_SYS_SNMP(); %>);

addCfg("SNMP_EN", "snmp_enabled", syssnmp[0]);
addCfg("SNMP_RO", "snmp_community_ro", syssnmp[5]);
addCfg("SNMP_RW", "snmp_community_rw", syssnmp[4]);
addCfg("TRAP_EN", "snmp_trap_enabled", syssnmp[1]);
addCfg("TRAP_IP", "snmp_trap_ip", syssnmp[2]);
addCfg("TRAP_CM", "snmp_trap_community", syssnmp[6]);
addCfg("TRUST_IP", "snmp_trust_host", syssnmp[3]);

pageTitle();
function snmp_endis()
{
	var f=document.forms[0];
	var dis = (f.SNMP_EN.value == 0);
	var dis2 = (f.TRAP_EN.value == 0) || dis;
	f.SNMP_RO.disabled = dis;
	f.SNMP_RW.disabled = dis;
	f.TRAP_EN.disabled = dis;
	
	for(i = 0; i < 4; i ++) { 
		f.TRAP_IP[i].disabled = dis2;
	}
	f.TRAP_CM.disabled = dis2;
}

function init()
{
	var f=document.forms[0];
	cfg2Form(f);
	snmp_endis();
}

function CheckForm(f)
{
	if(f.TRAP_EN.value == '0') return true;
	if(!verifyIP2(f.TRAP_IP, "IP Address", f.NM)) return false;
   	return true;
}
function apply()
{
	//document.title = "Hello";
	if(!CheckForm(system_man_ip))return;
	var f=document.forms[0];
	form2Cfg(f);
	subForm(f,'goform/command','SNMP',cPage);
}

</script>
</head>

<body onload=init()>
<script>pageHead(webVars);</script>
<form name=system_man_ip action=goform/command method=post>

<table width=100% border=0 align=center cellpadding=0 cellspacing=0
	class=box_tn>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1
			bgcolor="#FFFFFF">
			<tr>
				<td class=bgblue>SNMP</td>
				<td class=bggrey>
					<select name=SNMP_EN onchange=snmp_endis()>
					<option value=0>Disabled</option>
					<option value=1>Enabled</option>
					</select>
				</td>
			</tr>
			<tr>
				<td class=bgblue>Public Community</td>
				<td class=bggrey><input name=SNMP_RO size=31 maxlength=31></td>
			</tr>
			<tr>
				<td class=bgblue>Private Community</td>
				<td class=bggrey><input name=SNMP_RW size=31 maxlength=31></td>
			</tr>	
			<tr>
				<td class=bgblue>Trust Host</td>
				<td class=bggrey><input name=TRUST_IP size=31 maxlength=31>
				<br>IP Address/Subnet Mask Bits, such as: 192.168.1.0/24
				</td>
			</tr>
			<tr>
				<td class=bgblue>TRAP </td>
				<td class=bggrey>
					<select name=TRAP_EN onchange=snmp_endis()>
					<option value=0>Disabled</option>
					<option value=1>Enabled</option>
					</select>
				</td>
			</tr>
			<tr>
				<td class=bgblue>TRAP Community</td>
				<td class=bggrey><input name=TRAP_CM size=31 maxlength=31></td>
			</tr>					
			<tr>
				<td class=bgblue>TRAP Server IP </td>
				<td class=bggrey><input name=TRAP_IP size=3 maxlength=3>. <input
					name=TRAP_IP size=3 maxlength=3>. <input name=TRAP_IP size=3
					maxlength=3>. <input name=TRAP_IP size=3 maxlength=3></td>
			</tr>
		</table>
		</td>
	</tr>
</table>
<p>
<input type=button value="  OK  " onclick=apply() >
<input type=button value="Cancel" onclick=init() >
</p>
</form>

<script>pageTail();</script>
</body>
</html>
