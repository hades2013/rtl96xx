<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=navigatortab.js></SCRIPT>
<script language=JavaScript>
pageTitle();
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
//var webVars = new Array( 'EPN104', '0', '0');
function preDownload() 
{
	var f = document.forms[0];
	
	f.action = "syslog.bin";
	f.submit();
}

function preClear() 
{
	var f = document.forms[0];
	diffCfg('config_0', 'config', 'syslog');	
	subForm(f,'goform/command','CONFIG', cPage);	
}
</script>
</head>

<body>
<script>pageHead(webVars);</script>
<SCRIPT language=JavaScript>buildNavigatorTab(system_syslog, 2)</SCRIPT>
<form name=system_log action=goform/command method=post>
<INPUT type=hidden name=CMD value=SYS_LOG>
<INPUT type=hidden name=GO value=system_log.asp>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td width=100% class=greybluebg>日志信息</td>
</tr>
<tr>
	<td class=bggrey>

<textarea rows=20 cols=105 style='font-size:12;FONT-FAMILY:Arial,Helvetica, sans-serif' readonly>
&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp日期/时间&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp模块/级别&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp消息
<%CGI_GET_SYSLOG();%>
</textarea>

	</td>
</tr>
</table>
</form>
<input type=button value=" 导出 " onclick=onclick=preDownload()>
<input type=button value=" 清除 " onclick=preClear()>
<input type=button value=" 刷新 " onclick=refresh(cPage)></td>

<script>pageTail();</script>
</body>
</html>
