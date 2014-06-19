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
function loadConfirm()
{
	f = document.forms[0];
	
	if( !confirm('Are you sure to load configurations from the file?') )
		return;	
	if (f.files.value=="")
	{
		alert("Please select a file!");
		return ;
	}
	f.encoding="multipart/form-data";
	f.action="goform/upldForm"
	
	f.submit();
}

function configSave()
{
	f = document.forms[0];
	f.action = "EPN104.config";
	f.submit();
}

function restoreConfirm()
{
	f = document.forms[0];
	
	if( !confirm('Factory restore will cause system lose all configurations, confirm?') )
		return;
	diffCfg('config_0', 'config', 'restore');	
	subForm(f,'goform/command','CONFIG', cPage);	
}


</script>
</head>

<body>
<script>pageHead(webVars);</script>

<form name=system_configure action=goform/command method=post>
<INPUT type=hidden name=CMD value=CONFIG>
<INPUT type=hidden name=GO value=index.asp>
<input type=hidden name=TYPE value=CONFIG>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td width=100% class=greybluebg>Factory Restore</td>
</tr>
<tr>
	<td width=100% class=bggrey>
		<p align=center>
			<input type=button value="Factory Restore" name=restore onclick=restoreConfirm()>
		</p>
	</td>
</tr>
</table>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td width=100% class=greybluebg>Export a Backup</td>
</tr>
<tr>
	<td width=100% class=bggrey>
		<p align=center>
		<input type=button value="Export" name=save onClick=configSave()>
		</p>
	</td>
</tr>
</table>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td width=100% class=greybluebg>Load a Configurations file</td>
</tr>
<tr>
	<td width=100% class=bggrey>
		<p align=center>
			<input type=file size=25 maxlength=31 name=files>
			<input type=button value=" Import " name=load onclick=loadConfirm()>
		</p>
	</td>
</tr>
</table>
</form>

<script>pageTail();</script>
</body>
</html>

