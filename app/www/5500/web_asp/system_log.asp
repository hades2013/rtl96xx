<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 系统日志 </title>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
<script language=javascript src=theme/cvnchina/main.js></script>

<script type="text/javascript" src="script/jquery.min.js"></script>
<script type="text/javascript" src="script/functions.js?version=1.0.1.454"></script>
<script type="text/javascript" src="script/jquery.cookie.js"></script>
<script type="text/javascript" src="script/jquery.dataTables.js"></script>
<script type="text/javascript" src="script/table.js?version=1.0.1.454"></script>
<script type="text/javascript" src="script/Pagination.js?version=1.0.1.454"></script>

<script language=javascript>
var env=new Array();var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

env['vendor.name'] = 'Shanghai Cloud Vision Networks Co., Ltd.';
env['vendor.link'] = 'http://www.cvnchina.com';
env['vendor.copyright'] = '2013-2020';
env['vendor.id'] = 'cvnchina';
env['vendor.logo'] = 'default';

env['sys.theme'] = 'cvnchina';
env['sys.lang'] = 'cn';
env['sys.model'] = 'OCN-OUTDOOR';
env['sys.name'] = 'EOC System';
env['sys.version'] = 'R1.0.5';
env['sys.type'] = 'eoc';
env['sys.mib'] = 'nscrtv-hc';
env['sys.ip'] = '192.168.0.100';
env['sys.user'] = 'admin';
env['sys.time'] = '946685285';
env['sys.linkup'] = '0';
env['sys.linkoff'] = '0';
env['sys.busy'] = '0';
env['sys.ipc'] = '0';
env['sys.vlan'] = '0';
env['sys.unsaved'] = webVars[1];
env['sys.feature'] = 'vlanpool;mme_drop_ports;slave_access_mgmt;bcmp;';


env['web.page'] = current_page();env['sys.parent_name'] = '';

pageRedirectCheck(env);
var strArray = new Array(<%CGI_GET_SYSLOG();%>);
var strArraySet = new Array(<%CGI_SYS_LOG_SET();%>);
var PROD_MODEL = <%CGI_GET_MODEL();%>
//var strArraySet = new Array('1;192.168.1.10;6;6;514');
/*var strArray = new Array('Jan 01 22:18:23;STP;debug; Port 1 STP port state is set to Forwarding;#\
							Jan 02 22:18:23;STP;debug;Port 2 STP port state is set to Forwarding;#\
						  Jan 03 22:18:23;STP;debug; Port 3 STP port state is set to Forwarding;#\
						  Jan 04 22:18:23;STP;debug;Port 4 STP port state is set to Forwarding;\
						  Jan 05 22:18:23;STP;debug; Port 1 STP port state is set to Forwarding;\
						  Jan 06 22:18:23;STP;debug;Port 1 STP port state is set to Forwarding;\
						  Jan 07 22:18:23;STP;debug; Port 1 STP port state is set to Forwarding;\
						  Jan 08 22:18:23;STP;debug;Port 1 STP port state is set to Forwarding;\
						  Jan 09 22:18:23;STP;debug; Port 1 STP port state is set to Forwarding;\
						  Jan 10 22:18:23;STP;debug;Port 1 STP port state is set to Forwarding;\
						  Jan 11 22:18:23;STP;debug; Port 1 STP port state is set to Forwarding;\
						  Jan 12 22:18:23;STP;debug;Port 1 STP port state is set to Forwarding;','1;1;1','');*/
</script>
<!-- end header -->

<script language=JavaScript>

var logcfg = new Array('syslog_en=1',
'syslog_save_degree=6',
'syslog_ip=0.0.0.0',
'syslog_degree= 6',
'syslog_port=514'
);

function reload()
{
	location = current_page();
}

function preDownload() 
{
	var f = document.forms[0];
	f.action = PROD_MODEL+"-syslog.bin";
	f.logindex.value=0;
	f.submit();
	//window.location.href ="5500";

}

function preClear() 
{
	var f = document.forms[0];
	diffCfg('config_0', 'config', 'syslog');	
	subForm(f,'goform/command','CONFIG', current_page());	
}


function remote_port_select_on_change()
{
	var f=document.forms[0];
	var c = (f.remote_port_select.value == 0) ? true :false;
	c = (f.syslog_en.value == 0) ? true : c;
	f.remote_port_input.disabled = c;
}
	


function remote_enable_on_change()
{
	var f=document.forms[0];
	var c = (f.syslog_en.value == 0) ? true :false;
	f.syslog_ip.disabled = c;
	f.remote_port_select.disabled = 1;
	f.syslog_degree.disabled = c;
	f.syslog_save_degree.disabled = c;
	remote_port_select_on_change();
}
/*
function get_init_value(item)
{
	for (var i=0; i < logcfg.length; i ++){
			s = logcfg[i].split('=');alert(s);
			if ((s.length == 2)&& (s[0] == item)){
				 return s[1];
			}
	}	
	return "N/A";
}
*/
function valid_syslog_ip(s)       
{
	var re_ip = /^\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3}$/;

	if (!re_ip.test(s)){
		return false;
	}
	var ips = s.split('.');
	for (var i = 0; i < 4; i ++){
		if (parseInt(ips[i], 10) > 255) return false;	
	}
	var a0 = parseInt(ips[0], 10);
	if ((a0 == 127) || (a0 > 223)){
		return false;	
	}
	return true;
}

function valid_remote_port(s)
{
  var re = /^[1-9][0-9]*$/;
  if (s == '514'){
  	return true;	
  }
	if (!re.test(s)){
			return false;	
	}
  s = parseInt(s, 10);  	
	
	if ((s == 514) || ((s >= 1000) && (s < 65535))){
		return true;		
	}
	return false;
}


function check_all_input()
{
	var f=document.forms[0];
	var updates = 0;
	//var en = get_init_value('syslog_en');
	var en = f.syslog_en.value;
	if (document.getElementsByName('syslog_en')[0].value == '0'){
		if (en == '0'){
			alert('关闭日志功能!');
			return false;	
		}else {
			return true;	
		}	
	}
	for (var i=0; i < logcfg.length; i ++){
			s = logcfg[i].split('=');
			if ((s.length == 2)&& document.getElementsByName(s[0])[0]){
				if (document.getElementsByName(s[0])[0].value != s[1]){
					updates ++;	
				}
			}else {
				alert('内部错误，未知事项：' + logcfg[i]);				
			}
	}
	if (updates == 0) {
           alert('配置没有改变!');
           return false;
	} 
 
	if (!valid_syslog_ip(f.syslog_ip.value)){ 
		alert('无效的日志服务器IP地址，请重新输入');
		return false;
	}        
	if (!valid_remote_port(f.syslog_port.value)){
		 alert('无效的日志服务器端口，请重新输入');
		 return false;
	}      

	return true;		
}



function apply()
{
	var f=document.forms[0];
	
	if (f.remote_port_select.value == '0'){
		f.syslog_port.value = '514';
	}else {
		f.syslog_port.value = f.remote_port_input.value;
	}
	
	if('0' == f.syslog_en.value)
	{
		f.syslog_en.value = 0;
		f.syslog_ip.value = "0.0.0.0";
		diffCfg("syslog_en", "syslog_en", f.syslog_en.value);	//alert(f.syslog_en.value)
		diffCfg("syslog_save_degree", "syslog_save_degree", f.syslog_save_degree.value);//alert(f.syslog_save_degree.value)
		diffCfg("syslog_ip", "syslog_ip", f.syslog_ip.value);//alert(f.syslog_ip.value)
		diffCfg("syslog_degree", "syslog_degree", f.syslog_degree.value);//alert(f.syslog_degree.value)
		diffCfg("syslog_port", "syslog_port", f.syslog_port.value);//alert(f.syslog_port.value)
		setCookie(cookie_dontShowDoCMDPage, '0');
		subForm(f,'goform/command','SYSLOG',current_page());
		return;	
	}
	else
	{
		if(f.syslog_ip.value == '')
		{
			f.syslog_ip.value = '0.0.0.0';
		}
		if (!check_all_input())
		{
			return false;
		}
		else
		{
			for (var i=0; i < logcfg.length; i ++){
			s = logcfg[i].split('=');
			if ((s.length == 2)&& document.getElementsByName(s[0])[0]){
				diffCfg(s[0], s[0], document.getElementsByName(s[0])[0].value);//alert(s[0]+"+"+document.getElementsByName(s[0])[0].value);
				
			}else {
				alert('Internal Error, Unknown Item:' + logcfg[i]);	
				return	 false;			
			}
	}
			
		}
	}
/*	
	if (!check_all_input()){
		f.syslog_en.value = 0;
		f.syslog_ip.value = "0.0.0.0";
		diffCfg("syslog_en", "syslog_en", f.syslog_en.value);	//alert(f.syslog_en.value)
		diffCfg("syslog_ip", "syslog_ip", f.syslog_ip.value);//alert(f.syslog_ip.value)
		diffCfg("syslog_degree", "syslog_degree", f.syslog_degree.value);//alert(f.syslog_degree.value)
		diffCfg("syslog_save_degree", "syslog_save_degree", f.syslog_save_degree.value);//alert(f.syslog_save_degree.value)
		diffCfg("syslog_port", "syslog_port", f.syslog_port.value);//alert(f.syslog_port.value)
		subForm(f,'goform/command','SYSLOG','system_log.asp');
		return;
	}
*/	
	
	//diffCfg("syslog_port", "syslog_port", f.syslog_port.value);
	//diffCfg("cmd", "CLEAR_LOG_FILE", LOG_RAM);
	setCookie(cookie_dontShowDoCMDPage, '0');
	subForm(f,'goform/command','SYSLOG','system_log.asp');	
}

function init()
{
	var f=document.forms[0];
	var s;
	for (var i=0; i < logcfg.length; i ++){
			s = logcfg[i].split('=');
			if ((s.length == 2)&& document.getElementsByName(s[0])[0]){
				document.getElementsByName(s[0])[0].value = s[1];
			}else {
				alert('Internal Error, Unknown Item:' + logcfg[i]);
			}
	}

	if (f.syslog_port.value == '514'){
		f.remote_port_input.value = '514';
		f.remote_port_select.value = '0';	
	}else {
		f.remote_port_input.value = f.syslog_port.value;	
		f.remote_port_select.value = '1';		
	}
	if(GetVal(strArraySet, 0, 0) == '')
	{
		f.syslog_ip.value ='';
		f.syslog_port.value = 514;
	}
	else
	{	f.syslog_ip.value =GetVal(strArraySet, 0, 1);
		if(f.syslog_ip.value =="0.0.0.0")
		{
			f.syslog_ip.value = '';
			f.syslog_port.value = 514;
		}
		f.syslog_en.value = GetVal(strArraySet, 0, 0);
		f.syslog_degree.value = GetVal(strArraySet, 0, 2);
		f.syslog_save_degree.value = GetVal(strArraySet, 0, 3);
	}
	
	//f.syslog_en.value = 1;
	//f.syslog_degree.value = 6;
	remote_enable_on_change();
	

}

function GetVal(strArray, primary, index)
{	
	if (strArray.length > 0)
	{
		var vs = strArray[primary].split(';');
		if (vs.length > 0) 
		{	
			return vs[index];
		}
	}
	return false;
}
function writeMssge()
{

	var str = '';
	var i = 0;
	var INTERNAL = 5;
	var NO;
	var Timestamp;
	var Category;
	var Severity;
	var Message;
	var modules;
	var info = strArray[0].split(";");
	var len = info.length-1; 
	for(i=0; i< len; i=i+INTERNAL)
	{
		//NO = GetVal(strArray, 0, i);
		Timestamp = GetVal(strArray, 0, i);
		Category = GetVal(strArray, 0, i+1);
		Severity = GetVal(strArray, 0, i+2);
		modules= GetVal(strArray, 0, i+3);
		Message = GetVal(strArray, 0, i+4);//alert(Timestamp)
		
		if(-1 != Timestamp.indexOf("#"))
		{
			Timestamp = Timestamp.replace('#','');//alert(Timestamp)
		}

		str +=	'<TR class="alt0" onmouseover="repeatTRMouseOver(this);" onmouseout="repeatTRMouseOut(this);" >' +
				//'<TD class=intcol id=1_1_1> <span>'+NO+'</span></TD>' +
				'<TD class=def id=1_1_2> <span> '+Timestamp+'</span></TD>' +
				'<TD class=enumcol id=1_1_3> <span> '+Category+'</span></TD>' +
				'<TD class=def id=1_1_4> <span> '+Severity+' </span></TD>' +
				'<TD class=def id=1_1_4> <span> '+modules+' </span></TD>' +
				'<TD class=def id=1_1_5 name = result> <span> '+Message+'</span></TD>' +
				'<TD class="def_lastCol">&nbsp;</TD>' +
				'</TR>';		
	}
	document.write(str);
}

</script>
</head>
<body onload=init()>
<script>pageHead(env, env['web.page']);</script>
<form name=system_log action=goform/command method=post onsubmit="return false;">
<INPUT type=hidden name=CMD value=SYS_LOG>
<INPUT type=hidden name=GO value=system_log.asp>
<INPUT type=hidden name=syslog_port >
	<table width=100% cellpadding=5 cellspacing=0>
	<tr>
		<td colspan=2 class="tb_head" >系统日志 </td>
	</tr>
<tr>
<td class=tb_content>
<div id=topdiv >
<table class="topTable" cellspacing="0" cellpadding="0"><tbody><tr><td>
<table width="575" height="37" cellpadding="0" cellspacing="0" class=xuiPageNoButtonsTable><tbody>
<tr id="1">
<td width="560" colspan="100"><table class="xuiGroupRepeatTable" id="1" cellspacing="0" cellpadding="0"><tbody>
<tr id="1">
<td colspan="100"><table width="571" border="0" cellpadding="0" cellspacing="0" class="xuiGroupNoButtonsTable"><tbody>
<tr id=1_1><td width="416" colspan="100">
<table width="542" cellpadding="0" cellspacing="0" class="xuiRepeatTable" id="xuiRepeatTable1" ><thead>
<TR>
<TH width="112" class=def_TH id=1_1_2> 日期/时间</TH>
<TH width="75" class=enumcol id=1_1_3> 类型</TH>
<TH width="86" class=def_TH id=1_1_4> 级别</TH>
<TH width="86" class=def_TH id=1_1_4> 模块</TH>
<TH width="252" class=def_TH id=1_1_5>消息</TH>
<TH width="15" class='defTH_lastCol'>&nbsp; </TH>
</TR>
</thead>
<tbody id='xuiRepeatTableBody1'>
<script>writeMssge();</script>
</tbody></table></td></tr>
<script type="text/javascript">NikolaRepeatTableSortingInit("1");</script>
<script type="text/javascript">NikolaRepeatTablePaginationInit("1");</script>
</tbody></table></td>
</tr>
</tbody></table></td>
</tr>

<tr id=1_1><td colspan="100">
<table class=xuiRepeatHeadingTable cellspacing="0" cellpadding="0" ><tr>
<td class="xuiRepeatPaginationControls" align="right"><div class="xuiPaginationHeader1">&nbsp;</div></td>
<td>&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp</td>
<td>&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp</td>
<td>&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp</td>
<td>&nbsp&nbsp&nbsp&nbsp</td>
<td>&nbsp&nbsp&nbsp&nbsp</td>
<td align="right" class=xuiPaginationFooterTd><div class='xuiPaginationFooter1'></div></td>
</tr></table>
</td></tr>
</tbody></table>
</td></tr></tbody></table>
</div>


<!--
<table class=xuiPageNoButtonsTable cellspacing="0" cellpadding="0"><tbody>
<tr id="1">
<td colspan="100"><table class="xuiGroupRepeatTable" id="1" cellspacing="0" cellpadding="0"><tbody>
<tr id="1">
<td colspan="100"><table class="xuiGroupNoButtonsTable" cellspacing="0" cellpadding="0" border="0"><tbody>
<tr id=1_1><td colspan="100">
<table class="xuiRepeatTable" id="xuiRepeatTable1" cellspacing="0" cellpadding="0" ><thead>
<TR>
<TH class=def_TH id=1_1_2> 日期/时间
</TH>
<TH class=enumcol id=1_1_3> 模块
</TH>
<TH class=def_TH id=1_1_4> 级别
</TH>
<TH class=def_TH id=1_1_5> 消息
</TH>
<TH class='defTH_lastCol'>&nbsp;</TH>
</TR>
</thead>
<tbody id='xuiRepeatTableBody1'>
<script>writeMssge();</script>
</tbody></table></td></tr>
<script type="text/javascript">NikolaRepeatTableSortingInit("1");</script>
<script type="text/javascript">NikolaRepeatTablePaginationInit("1");</script>
</tbody></table></td>
</tr>

</tbody></table></td>
</tr>
</tbody></table>

<textarea rows=20  style='width:100%; font-size:14px;FONT-FAMILY:Arial,Helvetica, sans-serif' readonly>
&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp日期/时间&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp模块&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp级别&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp消息
Jan  1 00:00:09 Master syslog.info syslogd started: BusyBox v1.19.4
Jan  1 00:00:09 Master user.info syslog: [INFO]: BCMP start..done
Jan  1 00:00:09 Master user.info syslog: [INFO]: BCMP enters state: Registering
Jan  1 00:00:09 Master user.info syslog: [INFO]: Total 8 SNMP Alarms loaded.
Jan  1 00:00:20 Master user.debug syslog: [DEBUG]: Trap(6-11:MEM USAGE) snmptrap -v2c -c public 192.168.0.148
Jan  1 00:01:10 Master user.info syslog: [INFO]: Master is ready to receive HL2MP requests!
Jan  1 00:01:20 Master user.debug syslog: [DEBUG]: Trap(6-11:MEM USAGE) snmptrap -v2c -c public 192.168.0.148
Jan  1 00:02:10 Master user.err syslog: [ERR]: Detected all Cables lost, start reboot timer (600 sec)!
Jan  1 00:02:20 Master user.debug syslog: [DEBUG]: Trap(6-11:MEM USAGE) snmptrap -v2c -c public 192.168.0.148
Jan  1 00:03:20 Master user.debug syslog: [DEBUG]: Trap(6-11:MEM USAGE) snmptrap -v2c -c public 192.168.0.148
Jan  1 00:04:20 Master user.debug syslog: [DEBUG]: Trap(6-11:MEM USAGE) snmptrap -v2c -c public 192.168.0.148
</textarea>
-->
</td>
</tr>
</table>
<table  width=100% cellpadding=5 cellspacing=0><tr class=tb_content>
<td >
<input class="button" type=button value=" 下 载 " charset="utf-8" onclick="preDownload();">&nbsp;&nbsp;&nbsp;	
<input class="button" type=button value=" 清 空 " onclick="preClear();">&nbsp;&nbsp;&nbsp;	
<input class="button" type=button value="刷 新" onclick="reload();">&nbsp;&nbsp;&nbsp;	
</td>
<input name=logindex id=logindex type=hidden>
</tr></table>
	<table width=100% cellpadding=5 cellspacing=0>
			<tr>
		<td colspan=2 class="tb_head" >远程日志服务器参数设置</td>
	</tr>
		
<tr>
	<td class=tb_title >系统日志服务器启用</td>
	<td class=tb_content >
		<select name=syslog_en onchange=remote_enable_on_change()>
					<option value=0>禁用</option>
					<option value=1 selected >启用</option>
					</select>
	</td>
</tr>
<tr>
	<td class=tb_title >日志服务器IP地址</td>
	<td class=tb_content><input name=syslog_ip size=31 maxlength=31>
	</td>
</tr>
 <TR  >
		<td class=tb_title>发送日志等级</td>
		<td class=tb_content><select id="slct1" name="syslog_degree" style="WIDTH: 228">
		<OPTION value=0 > 紧急(Emergency)</option>
		<OPTION value=1> 报警(Alert)</option>
		<OPTION value=2> 严重(Critical)</option>
		<OPTION value=3> 错误(Error)</option>
		<OPTION value=4> 警告(Warning)</option>
		<OPTION value=5> 通知(Notice)</option>
		<OPTION value=6 selected > 报告(Informational)</option>
		<OPTION value=7> 调试(Debug)</option>
		</select></td>
    </TR>
<tr>
	<td class=tb_title >日志服务器端口</td>
	<td class=tb_content >
			<select name="remote_port_select" onchange=remote_port_select_on_change()>
				<option value="0">默认</option>
				<option value="1">手动输入</option>
			</select>
			&nbsp;&nbsp;&nbsp;	
		  <input name=remote_port_input size=5 maxlength=5> 有效范围(1000-65534)
	</td>
</tr>
  <TR class=textCell >
		<td class=tb_title>日志保存等级</td>
		<td class=tb_content><select id="slct1" name="syslog_save_degree" style="WIDTH: 228">
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
</table>
<table width=100% cellpadding=5 cellspacing=0><tr class=tb_content>
<td width="10%"  style="text-align:center;">
<input class="button" type=button value="确 认" onclick=apply() >
<td width="78%"></td>
<td width="12%"></td>
</tr></table>
</form>
<script>pageTail();</script>
</body>
</html>