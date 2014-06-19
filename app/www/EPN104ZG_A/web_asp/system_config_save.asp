<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<script language=JavaScript>
var str_copyright =<%CGI_GET_COPYRIGHT();%>
var PROD_MODEL = <%CGI_GET_MODEL();%>
</SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<script language=JavaScript>
pageTitle();
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

function save_apply()
{
	f = document.forms[0];
	if( !confirm('确定保存所有更新?'))
		return;
	diffCfg('config_0', 'config', 'save');	
	subForm(f,'goform/command','CONFIG','index.asp');	
}

</script>
</head>

<body>
<script>pageHead(webVars,PROD_MODEL);</script>

<form name=system_configure action=goform/command method=post>
<INPUT type=hidden name=CMD value=CONFIG>
<INPUT type=hidden name=GO value=index.asp>
<INPUT type=hidden name=action>
	
<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td width=100% class=greybluebg>保存更新</td>
</tr>
<tr>
	<td width=100% class=bggrey>
		<p align=center>
			<input type=button value="保存" name=save onclick=save_apply()>
		</p>
	</td>
</tr>
</table>
</form>
<script>pageTail(str_copyright);</script>
</body>
</html>

