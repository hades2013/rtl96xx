<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<script language=JavaScript>
	pageTitle();
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
</script>
</head>
<body>
<script>pageHead(webVars);</script>
<table>
	<tr>		<td bgcolor=gray>		•  管理 IP 设置 </td>		</tr>
		<tr>		<td>		设置管理 IP 地址。 </td>		</tr>
			
			<tr>		<td bgcolor=gray>		•  管理 VLAN 设置  </td>		</tr>
		<tr>		<td>		设置管理 VLAN ID。注意：管理 VLAN ID 只可设置为1~4094。  </td>		</tr>
		
		<tr>		<td bgcolor=gray>		•  时间设置  </td>		</tr>
		<tr>		<td>		此功能允许设备通过SNTP连接到SNTP服务器，从而使本地时间和互联网服务器时间同步。 设备中的同步时钟被用来记录安全信息和客户过滤控制。  </td>		</tr>
		
		<tr>		<td bgcolor=gray>		•  SNMP 设置  </td>		</tr>
		<tr>		<td>		SNMP相关设置。 </td>		</tr>
		
		<tr>		<td bgcolor=gray>		•  账号管理  </td>		</tr>
		<tr>		<td>		通过设置一个密码可以限制对设备的管理访问. </td>		</tr>
		
		<tr>		<td bgcolor=gray>		•  配置工具  </td>		</tr>
		<tr>		<td>		此页面的主要功能是对配置文件的管理，恢复出厂设置功能允许你把设备的配置还原到出厂时刻， 使用备份设置允许你把设备的配置保存为本地电脑上的"config.bin"文件. 你也可以通过使用载入设置把曾经保存的设备的配置文件载入到设备。  </td>		</tr>
		
		<tr>		<td bgcolor=gray>		•  系统复位 </td>		</tr>
		<tr>		<td>		当设备出现停止应答和工作的情况, 你可以点击下面的系统复位按钮. 当系统指示灯开始闪烁复位结束。 

 </td>		</tr>
		
		<tr>		<td bgcolor=gray>		•  固件升级  </td>		</tr>
		<tr>		<td>		输入系统文件的路径 然后点击下面的确定按钮. 系统将会升级并重启动.  </td>		</tr>
		
		<tr>		<td bgcolor=gray>		•  系统日志  </td>		</tr>
		<tr>		<td>		显示和下载系统日志信息  </td>		</tr>
		
</table>
<script>pageTail();</script>
</body>
