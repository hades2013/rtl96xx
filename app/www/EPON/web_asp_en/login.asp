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

function apply()
{
	var f=document.forms[0];
	f.submit();
	return false;
}

</script>
</head>

<body>
<script>pageHead(webVars);</script>

<form name=login action=goform/command method=post>
<input type=hidden name=CMD value="LOGIN">
<table height=250 width=100% border=0 cellpadding=0 cellspacing=0>
<tr>
	<td>
		<div align=center>
		<table border=0 cellpadding=5 cellspacing=1 class=box_subtn>
		<tr>
			<td align=center bgcolor=#CCCCCC>
				<b>System Login</b>
			</td>
		</tr>
		<tr>
			<td bgcolor=#F0F0F0>
				<table border=0 cellspacing=0 cellpadding=5>
				<tr>
					<td norwap><b>Username:</b></td>
					<td><input maxLength=12 size=20 name=uname style="WIDTH: 145px"></td>
				</tr>
				<tr>
					<td norwap><b>Passwrod:</b></td>
					<td><input type=password maxLength=12 size=20 name=pws  style="WIDTH: 145px"></td>
				</tr>
				<tr>
					<td colspan=2 align=center nowrap>
						<input type=submit value="  Login  " >
						<input type=button value="  Canel  " onClick="self.close();return false;">
					</td>
				</tr>
				</table>
			</td>
		</tr>
		</table>
		</div>
	</td>
</tr>
</table>
</form>

<script>pageTail();</script>
</body>
</html>
