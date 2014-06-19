<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 系统配置 </title>
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
env['sys.time'] = '946685223';
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
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var cfgName = <% CGI_GET_BAKUP_CFG_NAME(); %>;
function loadConfirm()
{
	var f = document.forms[0];
	var file_name=f.files.value;
	
	if( !confirm('确定导入新的配置文件?') )
		return;	
	if (file_name=="")
	{
		alert("请选择一个配置文件!");
		return ;
	}
	var len=file_name.toString().length;
	var name_ext=file_name.toString().substring(len-7, len);
	if(name_ext != ".config")
	{
	    alert("文件类型错误！");
		return;
	}
	f.encoding="multipart/form-data";
	f.action="goform/upldForm"
	
	f.submit();
}

function configSave()
{
	f = document.forms[0];
	//f.action = "config.bin";
	f.action = cfgName;
	f.submit();
}

function restoreConfirm()
{
	f = document.forms[0];
	
	if( !confirm('恢复出厂设置将失去现有的所有配置,请确认?') )
		return;
	diffCfg('config_0', 'config', 'restore');	
	subForm(f,'goform/command','CONFIG', 'login.asp');	
}


</script>
</head>

<body>
<script>pageHead(env, env['web.page']);</script>
<form name=system_configure action=goform/command method=post>
<INPUT type=hidden name=CMD value=CONFIG>
<INPUT type=hidden name=GO value=index.asp>
<input type=hidden name=TYPE value=CONFIG>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td width=100% class=tb_head>恢复出厂设置</td>
</tr>
<tr>
	<td width=100% class=bggrey>
		<p align=center>
			<input type=button value="恢复出厂设置" name=restore onclick=restoreConfirm()>
		</p>
	</td>
</tr>
</table>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td width=100% class=tb_head>导出配置文件</td>
</tr>
<tr>
	<td width=100% class=bggrey>
		<p align=center>
		<input type=button value="导出" name=save onClick=configSave()>
		</p>
	</td>
</tr>
</table>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td width=100% class=tb_head>导入配置文件</td>
</tr>
<tr>
	<td width=100% class=bggrey>
		<p align=center>
			<input type=file size=25 maxlength=31 name=files>
			<input type=button value="导入" name=load onclick=loadConfirm()>
			<input style="display:none" mce_style="display:none"><!--只有一个input类型为text的,使其回车不提交表单-->
		</p>
	</td>
</tr>
</table>
</form>
<script>pageTail();</script>
</body>
</html>