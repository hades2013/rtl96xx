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
var err_flag = <% CGI_GET_LOGIN_ERR(); %>
function apply()
{
	var f=document.forms[0];
	f.submit();
	return false;
}

</script>
</head>

<body>
<script>pageHead(webVars,PROD_MODEL);</script>

<form name=login action=goform/command method=post>
<input type=hidden name=CMD value="LOGIN">
<table height=250 width=100% border=0 cellpadding=0 cellspacing=0>
<tr>
	<td>
		<div align=center>
		<table border=0 cellpadding=5 cellspacing=1 class=box_subtn>
		<tr>
			<td align=center bgcolor=#CCCCCC>
				<b>系统登录</b>
			</td>
		</tr>
		<tr>
			<td bgcolor=#F0F0F0>
				<table border=0 cellspacing=0 cellpadding=5>
				<tr>
					<td norwap><b>用户名:</b></td>
					<td><input maxLength=12 size=20 name=uname style="WIDTH: 145px"></td>
				</tr>
				<tr>
					<td norwap><b>密  码:</b></td>
					<td><input type=password maxLength=12 size=20 name=pws  style="WIDTH: 145px"></td>
				</tr>
				<tr>
					<td></td>
					<td style="color:red">
						<script>
							if(1 == err_flag)
							{
								document.write("用户名或密码错误!");
							}
							else if(2 == err_flag)
							{
								document.write("登录失败!");
							}
							else
							{
								document.write("");
							}
						</script>
					</td>
				</tr>
				<tr>
					<td colspan=2 align=center nowrap>
						<input type=submit value="  登录  " >
						<input type=button value="  取消  " onClick="self.close();return false;">
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

<script>pageTail(str_copyright);</script>
</body>
</html>
