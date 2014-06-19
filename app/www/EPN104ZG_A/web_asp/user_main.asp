<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<script language=JavaScript>
var str_copyright =<%CGI_GET_COPYRIGHT();%>
var PROD_MODEL = <%CGI_GET_MODEL();%>
</SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<script language=JavaScript>
	pageTitle();
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
</script>
</head>
<body>
<script>pageHead(webVars,PROD_MODEL);</script>


<table>

			
			<tr>		<td bgcolor=gray>		•  用户列表  </td>		</tr>
		<tr>		<td>		增加删除用户，并允许进行白名单和黑名单操作.  </td>		</tr>
		
					<tr>		<td bgcolor=gray>		•  匿名访问  </td>		</tr>
		<tr>		<td>		查询所有匿名接入的用户名单.  </td>		</tr>
		
			<tr>		<td bgcolor=gray>		•  白名单  </td>		</tr>
		<tr>		<td>		查询允许介入的用户清单.  </td>		</tr>
		
		
				<tr>		<td bgcolor=gray>		•  黑名单  </td>		</tr>
		<tr>		<td>		查询不允许接入的用户名单.  </td>		</tr>
		
		
	
		
		
	
		
				
		
	</table>
	
	
	
<script>pageTail(str_copyright);</script>
</body>
