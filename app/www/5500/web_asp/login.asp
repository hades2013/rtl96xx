<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 用户登录 </title>
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src="theme/cvnchina/main.js"></script>
 
<script language=JavaScript> 
 
var err_flag = <% CGI_GET_LOGIN_ERR(); %>
function apply()
{
	var f=document.forms[0];
  var t = /^\s*$/;
   
  if (1 == t.test (f.uname.value))
  {
      alert ("用户名不能为空!");
      f.uname.focus();
      return false;
  }
  if (1 == t.test (f.pws.value))
  {
      alert ("密码不能为空!");
      f.pws.focus();
      return false;
  } 
  
  f.submit();
  return true;
}
</script>
</head>
<body>
<form name=login action=goform/command method=post  onsubmit="return apply();">
<input type=hidden name=CMD value="LOGIN">
<div id="loginBorder">
<div id="loginHeader">OCN-OUTDOOR&nbsp;&nbsp;&nbsp;EOC System</div>
<div id="loginTitle">用户登录</div>
 
<div id="loginWrapper" style="background-image:url('theme/cvnchina/images/login_title.png')">
<table>
    <tr>
    <td width=40% align=right height=40 ><label for="uname">用户名:</label></td>
    <td><input class="loginInput" maxLength=12 size=20 name=uname></td>
    </tr>
    <tr>
    <td align=right height=40><label for="pws">密&nbsp;&nbsp;&nbsp;码:</label></td>
    <td><input class="loginInput" type=password maxLength=12 size=20 name=pws ></td>
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
    <td colspan=2 height=40 align=center nowrap>
    <input class="loginButton" type=submit value="  登 录  " >&nbsp;&nbsp;&nbsp;&nbsp;
    <input class="loginButton" type=button value="  退 出  " onClick="self.close();return false;">
    </td>
    </tr>
</table>
</div>
</div>
</form>
</body>
</html>

