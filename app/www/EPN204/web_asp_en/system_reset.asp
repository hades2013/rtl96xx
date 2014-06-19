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
function doConfirm()
{
	f = document.forms[0];
	
	if (confirm('Are you sure to reboot system？'))
	{
		diffCfg('config_0', 'config', 'reboot');	
		subForm(f,'goform/command','CONFIG','index.asp');	
	}
}
</script>
</head>

<body>
<script>pageHead(webVars);</script>

<form name=system_reset action=goform/command method=post>
<INPUT type=hidden name=CMD value=SYS_CONF>
<INPUT type=hidden name=GO value=system_reset.asp>
<INPUT type=hidden name=CCMD value=100>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td class=bggrey>
		<p>Click this button will reboot the system.</p>
		<p align=center>
			<input type=button value=" Reboot " onclick=doConfirm() class=ButtonSmall>
		</p>
	</td>
</tr>
</table>
</form>

<script>pageTail();</script>
</body>
</html>
