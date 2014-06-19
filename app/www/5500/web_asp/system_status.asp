<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 系统状态 </title>
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
env['sys.time'] = '946685326';
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
  
var sysinfo = new Array(<% CGI_SYS_INFO(); %>);
var sysip = new Array(<% CGI_SYS_IP(); %>);  
/*var sysinfo = new Array('526','1','R1.0.5','REV.A','00:00:62:EE:EE:EE','Thu Jun 20 18:36:58 2013','Linux2.6.21.5-gd3cc8481-dirty#282 PREEMPT Sat Jun 8 15:21:29 CST 2013','0.94','1', '0','0','62');
var sysip = new Array('Master;00:00:62:EE:EE:EE;static','ok;192.168.0.100;255.255.255.0;0.0.0.0;0.0.0.0;0.0.0.0;0.0.0.0','stop;0.0.0.0;0.0.0.0;0.0.0.0;0.0.0.0;0.0.0.0;0.0.0.0;0;');
*/

function get_mac()
{
	if (sysip.length > 0){
		var vs = sysip[0].split(';');
		if (vs.length > 1) return vs[1];
	}
	return 'static';
}

function get_ip_proto()
{
	if (sysip.length > 0){
		var vs = sysip[0].split(';');
		if (vs.length > 2) return vs[2];
	}
	return 'static';
}

function get_ip()
{
	var type = get_ip_proto();
	var item = (type == 'static') ? 1 : 2;
	if (sysip.length > item){
		var vs = sysip[item].split(';');
		if (vs.length > 1) return vs[1];		
	}
	return '0.0.0.0';
}

function get_nm()
{
		var type = get_ip_proto();
	var item = (type == 'static') ? 1 : 2;
	if (sysip.length > item){
		var vs = sysip[item].split(';');
		if (vs.length > 2) return vs[2];		
	}
	return '0.0.0.0';
}

function get_gw(type)
{
		var type = get_ip_proto();
	var item = (type == 'static') ? 1 : 2;
	if (sysip.length > item){
		var vs = sysip[item].split(';');
		if (vs.length > 3) return vs[3];		
	}
	return '0.0.0.0';
}


function vlan_mvid()
{
	if (sysinfo.length > 1){
		return sysinfo[1];	
	}	
	return '1';		
}

function sw_version()
{
	if (sysinfo.length > 2){
		return sysinfo[2];	
	}	
	return '1';		
}

function hw_version()
{
	if (sysinfo.length > 3){
		return sysinfo[3];	
	}	
	return '1';		
}

function sys_config_usaged()
{
	if (sysinfo.length > 7){
		return sysinfo[7] +"%";	
	}	
	return '0';		
}

function sys_temperture()
{
	if (sysinfo.length > 9){
		return sysinfo[9] +" &deg;C";	
	}	
	return '0';		
}

function sys_cpu_load()
{
	if (sysinfo.length > 6){
		return sysinfo[6] + '%';	
	}	
	return '0';		
}

function sys_mem_usaged()
{
	if (sysinfo.length > 7){
		return sysinfo[7] + '%';	
	}	
	return '0';		
}


function show_runtime()
{
	var d,h,m, s;
	var r = sysinfo[0];
	d = Math.floor(r / (60*60*24));
	r = (r % (60*60*24));
	h = Math.floor(r / (60*60));
	r = (r % (60*60));
	m = Math.floor(r / 60);
	s = Math.floor(r % 60);
	return "已运行：" 
	+ d + ((d > 1) ? " 天 " : " 天 ") 
	+ h + ((h > 1) ? " 时 " : " 时 ") 
	+ m + ((m > 1) ? " 分 " : " 分 ") 
	+ s + ((s > 1) ? " 秒 " : " 秒") ;	
}

function page_refresh()
{
	location.reload();
}

</script>
</head>
<body>
<script>pageHead(env, env['web.page']);</script>
<form name=system_status action=goform/command method=post onsubmit="return false;">
<table width=100% cellpadding=5 cellspacing=0>
		<tr ><td colspan=2 class="tb_head">版本信息</td> </tr>			
	<tr>
		<td class=tb_title>软件版本</td>
		<td class=tb_content><script>document.write(sw_version());</script></td>
	</tr>	
	<tr>
		<td class=tb_title>硬件版本</td>
		<td class=tb_content><script>document.write(hw_version());</script></td>
	</tr>	
		<tr ><td colspan=2 class="tb_head">管理参数</td> </tr>		
	<tr>
		<td class=tb_title>MAC 地址</td>
		<td class=tb_content><script>document.write(get_mac());</script></td>
	</tr>
	<tr>
		<td class=tb_title>IP地址</td>
		<td class=tb_content><script>document.write(get_ip());</script></td>
	</tr>	
	<tr>
		<td class=tb_title>子网掩码</td>
		<td class=tb_content><script>document.write(get_nm());</script></td>
	</tr>
	<tr>
		<td class=tb_title>管理VLAN</td>
		<td class=tb_content><script>document.write(vlan_mvid());</script></td>
	</tr>	
		<tr ><td colspan=2 class="tb_head">系统状态</td> </tr>		
	<!--tr>
		<td class=tb_title>设备温度</td>
		<td class=tb_content><script>document.write(sys_temperture());</script></td>
	</tr-->
	<tr>
		<td class=tb_title>CPU负载</td>
		<td class=tb_content><script>document.write(sys_cpu_load());</script></td>
	</tr>	
	<tr>
		<td class=tb_title>已使用内存</td>
		<td class=tb_content><script>document.write(sys_mem_usaged());</script></td>
	</tr>
		<tr ><td colspan=2 class="tb_head">网络信息</td> </tr>	
	<tr>
		<td class=tb_title>CLT信息</td>
		<td class=tb_content><a href="service_main.asp">点击查看CLT列表</a></td>
	</tr>
	<tr>
		<td class=tb_title>网络拓扑</td>
		<td class=tb_content><a href="service_map.asp">点击查看网络拓扑</a></td>
	</tr>	
	<tr>
		<td colspan=2 class=tb_message><font size=4><script>document.write(show_runtime());</script></font></td>
	</tr> 
</table>
<p>
<input class="button" type=button value=' 刷新 '  onclick="page_refresh();">
</p>
</form>
<script> pageTail(); </script>
</body>
</html>