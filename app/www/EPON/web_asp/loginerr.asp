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
</script>
</head>

<body>
<script>pageHead(webVars);</script>

<table height=250 width=100% border=0 cellpadding=0 cellspacing=0>
<tr>
	<td>
		<div align=center>
		<table width=250 border=0 cellpadding=5 cellspacing=1>
		<tr>
			<td align=center>
				<p class=bluetextbold><font size=4 face="Arial, Helvetica, sans-serif">
				登录错误!!
				</font></p>
				<input type=button value="Back" onclick=GURL("login.asp") class=btn_o>
			</td>
		</tr>
		</table>
		</div>
	</td>
</tr>
</table>

<script>pageTail();</script>
</body>
</html>
