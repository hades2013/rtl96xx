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
	<tr>		<td bgcolor=gray>		•  上行端口设置 </td>		</tr>
		<tr>		<td>		对上行端口属性进行设置.  </td>		</tr>
			
			<tr>		<td bgcolor=gray>		•  Cable 端口设置  </td>		</tr>
		<tr>		<td>		对Cable端口进行设置。  </td>		</tr>
		
			<tr>		<td bgcolor=gray>		•  VLAN 模式设置  </td>		</tr>
		<tr>		<td>		设置VLAN模式,可选透传和802.1Q.  </td>		</tr>
			
			<tr>		<td bgcolor=gray>		•  802.1Q VLAN  </td>		</tr>
		<tr>		<td>		802.1Q VLAN参数配置，包含创建和删除功能。   </td>		</tr>
		
		
			<tr>		<td bgcolor=gray>		•  Trunk 设置 </td>		</tr>
		<tr>		<td>		创建和删除Trunk.  </td>		</tr>
			
			<tr>		<td bgcolor=gray>		•  Hybrid 设置   </td>		</tr>
		<tr>		<td>		创建和删除Hybrid。  </td>		</tr>
		
			<tr>		<td bgcolor=gray>		•  端口状态 </td>		</tr>
		<tr>		<td>		显示端口状态信息.  </td>		</tr>
			


	</table>
<script>pageTail(str_copyright);</script>
</body>
