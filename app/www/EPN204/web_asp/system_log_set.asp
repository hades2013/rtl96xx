<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=navigatortab.js></SCRIPT>
<SCRIPT language=JavaScript src=valid.js></SCRIPT>

<script language=JavaScript>
pageTitle();

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var syslog_set = new Array(<% CGI_SYS_LOG_SET(); %>);

/*just for test
var webVars = new Array( 'EPN104', '0', '0');
var syslog_set = new Array('1;192.168.0.66;2;7');			
*/
function get_value(INDEX)
{
	if (syslog_set.length > 0)
	{
		var vs = syslog_set[0].split(';');
		if (vs.length > 0) return vs[INDEX];
	}
	return false;
}

function on_sync_change()
{
	var f=document.forms[0];
	if(true==f.LOG_EN.checked)
	{
		f.LOG_IP.disabled = false;
		f.LOG_COM.disabled = true;
		f.LOG_DEGREE.disabled = false;
		f.LOG_SAVE.disabled = false;
	}
	else
	{
		f.LOG_IP.disabled = true;
		f.LOG_COM.disabled = true;
		f.LOG_DEGREE.disabled = true;
		f.LOG_SAVE.disabled = true;			
	}
}

function check_all_input()
{
	var f = document.forms[0];
	var string = "";
	var reg;
	
	string = f.LOG_IP.value;
	if(string=="")//允许IP为空
	{
		return true;
	}

	reg = /^(([1-9]|[1-9]\d|1\d\d|2[0-1]\d|22[0-3])\.){1}(([01]?\d?\d|2[0-4]\d|25[0-5])\.){2}([1-9]|[1-9]\d|1\d\d|2[0-4]\d|25[0-4])$/;
	var vs = string.split('.');		
	if((-1 == string.search(reg)) || (127 == vs[0]))
	{
		alert("目标主机IP地址非法！");
		return false;		
	}
	return true;
}

function init()
{
	var f=document.forms[0];
	var check=get_value(0);
	if(check==1)
	{
		f.LOG_EN.checked = true ;
	}
	else
	{
		f.LOG_EN.checked =false;
	}	
	f.LOG_IP.value = get_value(1);
	if(f.LOG_IP.value =="0.0.0.0")
	{
		f.LOG_IP.value="";
	}
	f.LOG_DEGREE.value = get_value(2);
	f.LOG_SAVE.value = get_value(3);
	f.LOG_COM.value = 514;
	f.LOG_COM.disabled = true;
	on_sync_change();
}
function apply()
{
	var f=document.forms[0];
	if (!check_all_input())return;
	
	diffCfg("syslog_en", "syslog_en", f.LOG_EN.checked ? "1" : "0");
	diffCfg("syslog_ip", "syslog_ip", f.LOG_IP.value =="" ? "0.0.0.0" : f.LOG_IP.value);
	diffCfg("syslog_degree", "syslog_degree", f.LOG_DEGREE.value);
	diffCfg("syslog_save_degree", "syslog_save_degree", f.LOG_SAVE.value);
	
	subForm(f,'goform/command','SYSLOG','system_log_set.asp');
}

</script>
</head>

<body onload=init()>
<script>pageHead(webVars);</script>
<SCRIPT language=JavaScript>buildNavigatorTab(system_syslog, 1)</SCRIPT>
<form name=system_log_set action=goform/command method=post>

<TABLE cellSpacing=1 cellPadding=2 border=0 width=100%>
    <TR>
    <TD colspan=3 class=titleCell>&nbsp;日志使能设置</TD>
    </TR>
		<tr>
			<td>
			<table>
				<tr>
				<td  class=textCell >使能日志</td>
				<td><input type=checkbox name=LOG_EN onClick=on_sync_change();></td>	
				</tr>	
				</table>
			</td>
		</tr>			
</TABLE>

<TABLE cellSpacing=1 cellPadding=2 border=0 width=100%> 
	<TR>
    <TD colspan=3 class=titleCell>&nbsp;日志主机设置</TD>
    </TR> 
    <TR><TD height=8></TD></TR>
    <TR class=textCell >
		<TD width="200">日志主机IP地址(默认设为空)</TD>
		<TD width=40%><input type="text" size="32" id="ip" name="LOG_IP" maxlength="15" ></TD> 
		<td class=textCellRed align = left id=checkip></td>
    </TR>
    <TR class=textCell >
		<TD width="200">发送日志端口</TD>
		<TD><input type="text" size="32" id="port_no" name="LOG_COM" maxlength="5"></TD> 
		<td class=textCellRed align = left id=checkport></td>
    </TR>
    <TR class=textCell >
		<td width="200">发送日志等级</td>
		<td><select id="slct1" name="LOG_DEGREE" style="WIDTH: 228">
		<OPTION value=0 selected> 紧急(Emergency)
		<OPTION value=1> 报警(Alert)
		<OPTION value=2> 严重(Critical)
		<OPTION value=3> 错误(Error)
		<OPTION value=4> 警告(Warning)
		<OPTION value=5> 通知(Notice)
		<OPTION value=6> 报告(Informational)
		<OPTION value=7> 调试(Debug)
		</select></td>
    </TR>
    <TR class=textCell >
		<td width="200">日志保存等级</td>
		<td><select id="slct1" name="LOG_SAVE" style="WIDTH: 228">
		<OPTION value=0 selected> 紧急(Emergency)
		<OPTION value=1> 报警(Alert)
		<OPTION value=2> 严重(Critical)
		<OPTION value=3> 错误(Error)
		<OPTION value=4> 警告(Warning)
		<OPTION value=5> 通知(Notice)
		<OPTION value=6> 报告(Informational)
		<OPTION value=7> 调试(Debug)
		</select></td>
    </TR>
 </TABLE>	
<p>
<input type=button value="确认" onclick=apply() >
<input type=button value="取消" onclick=init() >
</p>
</form>

 <script>pageTail();

</script>
</body>
</html>
