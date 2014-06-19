<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 系统升级 </title>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
<script language=javascript src=script/valid.js></script>
<script language=javascript src=theme/cvnchina/main.js></script>
<script language=javascript src=script/schedule.js></script>
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
env['sys.time'] = '946685265';
env['sys.linkup'] = '0';
env['sys.linkoff'] = '0';
env['sys.busy'] = '0';
env['sys.ipc'] = '0';
env['sys.vlan'] = '0';
env['sys.unsaved'] = webVars[1];
env['sys.feature'] = 'vlanpool;mme_drop_ports;slave_access_mgmt;bcmp;';


env['web.page'] = current_page();env['sys.parent_name'] = '';

pageRedirectCheck(env);
</script>
<script language=JavaScript>

var msgs = new Array();
msgs['unknown'] = '未知错误!';
msgs['ok'] = '配置成功!';
//msgs['fail'] = '配置失败!!! 错误代码: ';
msgs['fail'] = '配置失败!';
msgs['reboot'] = '系统正在重启中...请稍等!';
msgs['restore'] = '系统正在恢复出厂设置,请稍等!';
msgs['cfgimport'] = '系统已成功导入配置文件,导入配置文件后会自动重启,请稍等!';
msgs['upgrade'] = '系统软件下载成功,系统正在执行更新,请稍等!';



var GO	= QueryString('GO');
var MSG = QueryString('MSG');
var DELAY = QueryString('DELAY');
var ERR = QueryString('ERR');
var ERRSTR = QueryString('ERRSTR');
var COUNT = 0;

function gurl(u) {
	if (u=='') u = 'index.asp'; 
	location=u; 
}

var cookie_val =getCookie(cookie_dontShowDoCMDPage);
if(null == cookie_val)
{		
	gurl(GO);
}
else if('1' == cookie_val)
{
	setCookie(cookie_dontShowDoCMDPage, '0');
	gurl(GO);
}

function get_message()
{
	var msg;
	if (MSG){
		 msg = msgs[MSG];
		 if (MSG == 'fail'){
		 	//msg = msgs[MSG] + ERR +'.';
		 	msg = msgs[MSG];
		 //	if (ERRSTR != '') msg += '[' + ERRSTR + ']';
		 }else if (MSG == 'cfgimport'){
		 	 msg = "系统已成功导入配置, 系统正在重启,请稍等!";	
		 }
	}
	return msg ? msg : msgs['unknown'];
}


function start_delay()
{
	var rest = 0;
	
	if(COUNT < DELAY) {
		COUNT ++;
		setTimeout("start_delay()", 1000);
		rest = DELAY - COUNT;
		document.getElementById('dmsg').innerHTML = '<p class=bluetextbold><font size=4 face="Arial, Helvetica, sans-serif">Waiting: ' 
			+ rest + ' s.</font></p>';
	}
		if(COUNT >= DELAY) {
			document.forms[0].cbtn.disabled = false;
			document.getElementById('dmsg').innerHTML = '<p class=bluetextbold><font size=4 face="Arial, Helvetica, sans-serif">&nbsp;</font></p>';
		if (MSG == 'upgrade')
		{
			document.getElementById('MyScheduleDiv').style.display='none';
			gurl('login.asp');	
		}
	}
}

function init()
{
	if (DELAY > 0){
		document.forms[0].cbtn.disabled = true;
		start_delay();			
	}
}
</SCRIPT>
</HEAD>

<body onload="init();">
<script>pageHead(env, env['web.page']);</script>
<form>
<table height=250 width=100% border=0 cellpadding=0 cellspacing=0>
<tr>
	<td>
		<div align=center>
		<table width=400 border=0 cellpadding=5 cellspacing=1>
		<tr>
			<td align=center>
				<p class=bluetextbold>
				<font size=4 face="Arial, Helvetica, sans-serif"><script>	document.writeln(get_message());</script>
				</font>
				</p>
			</td>
		</tr>
		<tr>
			<td align=center>
				<div id="dmsg"></div>
			</td>
		</tr>
		<tr>
			<td align=center>
				<p>
				<input name=cbtn type=button value="    继续    " onclick="gurl(GO);" class=btn_o>
				</p>
			</td>
		</tr>
		</table>
		</div>
	</td>
</tr>
</table>
</form>
<script>pageTail();</script>
<script>

DisCover();
	if (MSG == 'upgrade')
	{	
		document.getElementById('MyScheduleDiv').style.display='block';	
	 }
</script>
</body>
</html>

