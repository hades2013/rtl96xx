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
<!-- end header -->

<script language=JavaScript>
    
//var sysinfo = new Array('465','1','R1.0.5','REV.A','00:00:62:EE:EE:EE','Thu Jun 20 18:36:58 2013','Linux2.6.21.5-gd3cc8481-dirty#282 PREEMPT Sat Jun 8 15:21:29 CST 2013','0.94','1', '0','0','62');
//var sysinfo = new Array('465','1','R1.0.5','REV.A','00:00:62:EE:EE:EE','Thu Jun 20 18:36:58 2013','Linux2.6.21.5-gd3cc8481-dirty#282 PREEMPT Sat Jun 8 15:21:29 CST 2013','0.94','2', '0','0','62');

//var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var sysinfo = new Array(<% CGI_SYS_INFO(); %>);

addCfg('SWVER','fw_version', sysinfo[2]);
addCfg('HWVER','device_hw_rev', sysinfo[3]);
addCfg('HWADDR','lan_mac_addr', sysinfo[4]);


function apply()
{
	var f=document.forms[0];
	var file_name=f.uploadfile.value;
	
	if (file_name=="")
    {
		alert("请选择升级文件!");
		return ;
	}
	var len=file_name.toString().length;
	var name_ext=file_name.toString().substring(len-4, len);
	
	if(name_ext != ".bin")
	{
		alert("文件类型错误！");
		return;
	}
    document.getElementById("up_file").value=document.getElementById("uploadfile").value;
	
	if (confirm('系统正在处理升级文件,它需要一些时间,请不要关闭设备电源,并耐心等待!'))
	{			
		displaylay(1);
		f.submit();	
	}
}
</script>
</head>
<body >
<script>pageHead(env, env['web.page']);</script>
<form name=system_upgrade action=goform/upldForm method=post onsubmit="return false;" enctype="multipart/form-data">
<input type=hidden name=TYPE value=UFILE>
<input type=hidden name="up_file" id="up_file">

	<table width=100% cellpadding=5 cellspacing=0>
	<tr>
		<td colspan=2 class="tb_head" >系统升级</td>
	</tr>
<tr>
	<td  class=tb_title>
		硬件版本
	</td>
	<td  class=tb_content>
		<script>document.write(getCfg('HWVER'));</script>
	</td>
</tr>
<tr>
	<td  class=tb_title>
		MAC 地址
	</td>
	<td class=tb_content>
		<script>document.write(getCfg('HWADDR'));</script>
	</td>
</tr>
<tr>
	<td class=tb_title>
		软件版本
	</td>
	<td class=tb_content>
		<script>document.write(sysinfo[2]);</script>
	</td>
</tr>
<tr>
	<td class=tb_title>发布日期</td>
	<td class=tb_content>
		<script>document.write(sysinfo[5]);</script>
	</td>
</tr>
<tr>
	<td class=tb_title>系统升级</td>
	<td class=tb_content><input type=file size=30 name=uploadfile id="uploadfile"><input type=button value="  确定  " onclick=apply();><input style="display:none" mce_style="display:none"><!--只有一个input类型为text的,使其回车不提交表单--></td>
</tr>
</table>
</form>
<script>pageTail();</script>
<script>
//DisScheduleDiv();
DisCover();
displaylay(0);
</script>
</body>
</html>