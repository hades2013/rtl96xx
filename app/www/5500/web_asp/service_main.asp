<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 局端管理 </title>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
<script language=javascript src=script/valid.js></script>
<script language=javascript src=script/pageview.js></script>
<script language=javascript src=script/cnu_mgmt.js></script>
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
env['sys.time'] = '946684912';
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
 
<style type="text/css">
.css_link{
   font-size: 12px;
   color: green;
}</style>

<script language=JavaScript>

/*id;mac;type;link;local;snid;numofslaves*/
var clt_list = new Array('1;00:00:00:00:00:00;;0;0;0;0;',
'2;00:00:00:00:00:00;;0;0;0;0;',
'3;00:00:00:00:00:00;;0;0;0;0;',
'4;00:00:00:00:00:00;;0;0;0;0;');

/*As some reason, we should put CGI_CLT_ATTRIBUTE after CGI_CLT_LIST */
var clt_attr = new Array('60','1', '1', '0', '1', '0', '0');


function get_refresh_interval()
{
	var vs;
	if (clt_attr.length > 0){
			return clt_attr[0];	
	}
	return '60';	
}

function get_anonymous_control()
{
	var vs;
	if (clt_attr.length > 1){
			return clt_attr[1];	
	}
	return '1';	
}

function get_loop_detect_enable()
{
	var vs;
	if (clt_attr.length > 2){
			return clt_attr[2];	
	}
	return '0';	
}

function get_template_option()
{
	var vs;
	if (clt_attr.length > 3){
			return clt_attr[3];	
	}
	return '0';	
}

function get_extmib_option()
{
	var vs;
	if (clt_attr.length > 4){
			return clt_attr[4];	
	}
	return '0';	
}


function get_vlanpool_option()
{
	var vs;
	if (clt_attr.length > 5){
			return clt_attr[5];	
	}
	return '0';	
}

function get_access_option()
{
	var vs;
	if (clt_attr.length > 6){
		return clt_attr[6];	
	}	
	return '0';
}

function clt_redirect(ind)
{
	location='service_clt.asp?clt_mac=' + clt_list[ind].split(';')[1];
}

function page_refresh()
{
	location.reload();
}

function clt_status(s)
{
	if (s == '1'){
		return '初始化';		
	}else if (s == '2'){
		return "<span class='css_link'>在线</span>";
	}else if (s == '3'){
		return '离线';	
	}else {
		return '未知';	
	}
}

function clt_location(s)
{
	if (s == '1'){
		return "本地";
	}else {
		return '远程';
	}
}

function on_auto_disabled()
{
	var f=document.forms[0];
	f.interval.disabled=f.auto_disable.checked;
}

function init()
{
	var f=document.forms[0];
	
	if (!env_has(env, "ext_mib")){
		document.getElementById('tr_extmib_option').style.display = "none";
	}
	if (!env_has(env, "template")){
		document.getElementById('tr_template_option').style.display = "none";
	}
	if (!env_has(env, "vlanpool")){
		document.getElementById('tr_vlanpool_option').style.display = "none";
	}
	if (!env_has(env, "slave_access_mgmt")){
		document.getElementById('tr_access_option').style.display = "none";
	}
	
	var val = get_refresh_interval();
	
	if (val == 0){
		f.auto_disable.checked = true;
		f.interval.value = 60;		
	}else {
		f.interval.value = val;	
	}	
	f.anonymous_ctrl.value = get_anonymous_control();	
	f.loop_detect.value = get_loop_detect_enable();
	f.template_option.value = get_template_option();
	f.extmib_option.value = get_extmib_option();
	f.vlanpool_option.value = get_vlanpool_option();
	f.access_option.value = get_access_option();
	
	on_auto_disabled();
	
	pageview_reset();		
	pageview_add('CLT', 0, '5%', false);
	pageview_add('MAC 地址', 1, '15%', true, null, "clt_redirect");	
	pageview_add('型号', 2, '10%', true, null);	
	pageview_add('状态', 3, '10%', true, clt_status);
	pageview_add('位置', 4, '10%', true, clt_location);
	pageview_add('SNID', 5, '10%', false, null);
	pageview_add('终端数量', 6, '10%', false, null);
	pageview_init(clt_list, 10, document.getElementById('clt_table'), false, null, null, '局端列表');
}


/* Functions for setting */

function check_all_input()
{
	var f=document.forms[0];
	if((f.auto_disable.checked == false) 
	&& ((f.interval.value < 30) || (f.interval.value > 180))){
		alert("错误的刷新间隔,请重新输入.");
		return false;	
	}
	var interval = (f.auto_disable.checked) ? '0' : f.interval.value;
	if ((f.loop_detect.value == get_loop_detect_enable()) 
	&& (interval == get_refresh_interval())
	&& (f.anonymous_ctrl.value == get_anonymous_control())
	&& (f.template_option.value == get_template_option())
	&& (f.extmib_option.value == get_extmib_option())		
	&& (f.vlanpool_option.value == get_vlanpool_option())
	&& (f.access_option.value == get_access_option())
	){
			alert("配置未改变,请返回!");
			return false;
	}
	return true;
}


function apply()
{
	var f=document.forms[0];
	if (!check_all_input()){
		return;	
	}
	diffCfg("anonymous", "anonymous", f.anonymous_ctrl.value);
	if (f.auto_disable.checked){
		diffCfg("refresh", "refresh", 0);		
	}else {
		diffCfg("refresh", "refresh", f.interval.value);		
	}
	diffCfg("loop_detect", "loop_detect", f.loop_detect.value);
		
	if (env_has(env, "ext_mib")){
		diffCfg("extmib", "extmib", f.extmib_option.value);	
	}
	if (env_has(env, "template")){
		diffCfg("template", "template", f.template_option.value);
	}
	if (env_has(env, "vlanpool")){
		diffCfg("vlanpool", "vlanpool", f.vlanpool_option.value);
	}
	if (env_has(env, "slave_access_mgmt")){
		diffCfg("access_enable","access_enable",f.access_option.value);		
	}
	subForm(f,'goform/command','CAB_PROP',current_page());
}

function clt_refresh()
{
	var f = document.forms[0];
	subForm(f,'goform/command', "REFRESH", "service_main.asp");	
}

</script>
</head>
<body onload="init();">
<script>pageHead(env, env['web.page']);</script>

<form name=form_port_table action=goform/command method=post onsubmit="return false;">
<INPUT type=hidden name=CMD>
<INPUT type=hidden name=GO>
<INPUT type=hidden name=action>
<table width=100% cellpadding=5 cellspacing=0>
		<tr ><td colspan=2 class="tb_head">管理参数</td> </tr>		
	<tr>
		<td class=tb_title>网络刷新间隔</td>
		<td class=tb_content>
			<input name="interval" size=3 maxlength=3></input>秒 (有效范围:30-180)&nbsp; &nbsp; 
			手动模式 <input type=checkbox name=auto_disable onClick=on_auto_disabled();> 
			<span style="color:red;"> 请注意, "手动模式"是测试选项,谨慎启用！</span>
		</td>
	</tr>	
	<tr>
		<td class=tb_title>匿名访问控制</td>
		<td class=tb_content>
			<select name=anonymous_ctrl>
			<option value='0'>拒绝</option>
			<option value='1'>允许</option>
			</select>
		</td>
	</tr>
	<tr>
		<td class=tb_title>环路检测</td>
		<td class=tb_content>
			<select name=loop_detect>
			<option value='0'>不使用环路检测</option>
			<option value='1'>开启,检测到环路时关闭整个终端</option>
			<option value='2'>开启,检测到环路时关闭自环端口</option>			
			</select> 			
		</td>
	</tr>	
	<tr id="tr_template_option">
		<td class=tb_title>模板选项</td>
		<td class=tb_content>
			<select name=template_option>
			<option value='0'>所有终端使用用户定义模板</option>
			<option value='1'>所有终端使用默认模板</option>
			<option value='2'>所有终端不使用模板</option>			
			</select> &nbsp;&nbsp;(重启后才能生效)
		</td>
	</tr>	
	<tr id="tr_extmib_option">
		<td class=tb_title>扩展MIB选项</td>
		<td class=tb_content>
			<select name=extmib_option>
			<option value='0'>禁用</option>	
			<option value='1'>启用</option>
			</select> &nbsp;&nbsp;(重启后才能生效)
		</td>
	</tr>
	<tr id="tr_vlanpool_option">
		<td class=tb_title>VLAN池全局控制</td>
		<td class=tb_content>
			<select name=vlanpool_option>
			<option value='0'>禁用</option>	
			<option value='1'>启用</option>
			</select> 
		</td>
	</tr>	
		<tr id="tr_access_option">
		<td class=tb_title>允许从终端访问局端</td>
		<td class=tb_content>
			<select name=access_option>
			<option value='0'>禁用</option>	
			<option value='1'>启用</option>
			</select> 
		</td>
	</tr>	
</table>
<p>
<input class="button"  type=button value=" 确  认 " onclick=apply() >&nbsp;&nbsp;&nbsp;
<input class="button"  type=button value=" 取  消" onclick=init() >
</p>
<p>
<div id="clt_table"></div>
</p>
<p>
<table>
	<tr>
	<td width=30 valign=top>
		<INPUT name=but_refresh class="button" type=button value="  刷新  " onclick="clt_refresh();"></td>
	</tr>
</table>
</p>
</form>
<script>pageTail();</script>
</body>
</html>