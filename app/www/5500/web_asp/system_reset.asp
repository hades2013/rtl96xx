<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 系统重启 </title>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
<script language=javascript src=theme/cvnchina/main.js></script>

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
env['sys.time'] = '946685240';
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
<!-- end header -->

<script language=JavaScript>


function doConfirm()
{
	f = document.forms[0];
	
	if (confirm('确定要重新启动系统？'))
	{
		diffCfg('config_0', 'config', 'reboot');	
		subForm(f,'goform/command','CONFIG','index.asp');	
	}
}
</script>
</head>

<body >
<script>pageHead(env, env['web.page']);</script>
<form name=system_reset action=goform/command method=post onsubmit="return false;">
<INPUT type=hidden name=CMD value=SYS_CONF>
<INPUT type=hidden name=GO value=system_reset.asp>
<INPUT type=hidden name=CCMD value=100>

	<table width=100% cellpadding=5 cellspacing=0>
	<tr>
		<td colspan=2 class="tb_head" >系统重启</td>
	</tr>
	<tr>
	<td class=tb_content style="text-align:center;">
		<p style="text-align:left;">点击此按钮将重新启动系统	</p>
			<input class="button" type=button value=" 重启 " onclick=doConfirm() class=ButtonSmall>
	</td>
</tr>
</table>
</form>
<script>pageTail();</script>
</body>
</html>