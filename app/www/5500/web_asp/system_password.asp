<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 管理员设置 </title>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
<script language=javascript src=script/valid.js></script>
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
env['sys.time'] = '946685448';
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

var sysadmin = new Array(<% CGI_SYS_ADMIN(); %>);
//var sysadmin = new Array('600','1;admin;admin','0;auxadmin;admin');


function get_idle_time()
{
	return sysadmin[0]/60;
}

function default_user()
{
	var us;
	for (var i=0; i < sysadmin.length; i++)
	{
		us = sysadmin[i].split(';'); 
		if (us.length > 2){
			return us[1];
		}
	}
	return "";
}

function user_get_pwd(u)
{
	var us;
	for (var i=0; i < sysadmin.length; i++)
	{
		us = sysadmin[i].split(';'); 
		if ((us.length > 2) && (us[1] == u) ){
			return us[2];
		}
	}
	return "";
}


function user_get_en(u)
{
	var us;
	for (var i=0; i < sysadmin.length; i++)
	{
		us = sysadmin[i].split(';'); 
		if ((us.length > 2) && (us[1] == u) ){
			return us[0];
		}
	}
	return "0";
}

function username_exist(u, n)
{
	var us;
	if (u == n) return false;
	for (var i=0; i < sysadmin.length; i++)
	{
		us = sysadmin[i].split(';'); 
		if ((us.length > 2) && (us[1] != u) && (us[1] == n) ){
			return true;
		}
	}	
	return false;
}


function user_options()
{
	var us, ret = "";
	for (var i=0; i < sysadmin.length; i++)
	{
		us = sysadmin[i].split(';'); 
		if (us.length > 2){
			ret += "<option value='" + us[1] + "'> "+ us[1] +"</option>\n";
		}
	}
	return ret;
}


function user_en_change()
{
	f=document.forms[0];
	var dis = (f.user_en.value == '0') ? true : false;
	f.user_name.disabled = dis;
	f.user_cpwd.disabled = dis;
	f.user_npwd.disabled = dis;	
	f.user_npwd1.disabled = dis;	
}


function user_select()
{
	f=document.forms[0];
	var n = f.user.value;
	//f.user_name.value = n;
	
	//f.user_en.value = user_get_en(n); 
		
	//if (n == default_user()){
	//	f.user_en.value = '1';
	//	f.user_en.disabled = true;
	//}else {
	//	f.user_en.disabled = false;
	//}
	f.user_cpwd.value = '';
	f.user_npwd.value = '';	
	f.user_npwd1.value = '';		
	
	//user_en_change();
}

function init()
{
	f=document.forms[0];
	f.user.value = default_user();
	user_select();
	f.idle_time.value = get_idle_time();
}

function valid_idletime(v)
{
  var re = /^[1-9][0-9]*$/;
	if (!re.test(v)){
			return false;	
	}
  v = parseInt(v, 10);  	

	if (v < 1 || v > 60 ){
			return false;
	}	
	return true;
}


function apply()
{
/*
	var f=document.forms[0];
	
	f.user_cpwd.value = string_trim(f.user_cpwd.value);
	f.user_npwd.value = string_trim(f.user_npwd.value);
	f.user_npwd1.value = string_trim(f.user_npwd1.value);		
	
	if (f.user_en.value == '0'){
		if (user_get_en(f.user.value) != '0'){
			diffCfg('user0', 'user',  f.user.value + ';0;' + f.user.value + ';'+ user_get_pwd(f.user.value));
		}
	}else {
		if (!valid_username(f.user_name.value)){
			alert('无效的用户名,请重新输入.');
			return;		
		}
		if (username_exist(f.user.value, f.user_name.value)){
			alert('用户名已经被使用,请输入其他用户名.');
			return;
		}
		
		if ((f.user_cpwd.value != '')
	  || (f.user_npwd.value != '')
	  || (f.user_npwd1.value != '')){
	  
		if (!password_match(user_get_pwd(f.user.value), f.user_cpwd.value)){
			alert('当前密码不正确!');
			return;		
		}
		
		if (!password_match(f.user_npwd.value, f.user_npwd1.value)){
			alert('新密码不匹配!');
			return;		
		}
		
		if (!valid_password(f.user_npwd.value)){
			alert('无效的新密码!');
			return;
		}
		diffCfg('user0', 'user',  f.user.value + ';1;' +f.user_name.value + ';'+ f.user_npwd.value);	
		}else {
			diffCfg('user0', 'user',  f.user.value + ';1;' + f.user_name.value + ';'+ user_get_pwd(f.user.value));
		}	
	}
	
	if (!valid_idletime(f.idle_time.value)){
		alert('无效的登录超时时间!');
		return;
	}
	diffCfg('idle_time', 'idle_time', f.idle_time.value * 60);	
	subForm(f,'goform/command','SYS_ADMIN', current_page());	
*/
	
	f=document.forms[0];
	var user_change = 0;
	if ((f.user_cpwd.value != '') || (f.user_npwd.value != '') || (f.user_npwd1.value != '')){
		user_change = 1;
	}
	
	if (user_change == 1){
		if (!password_match(user_get_pwd(f.user.value), f.user_cpwd.value)){
			alert('当前密码不正确!');
			return;		
		}
		
		if (f.user_npwd.value=='' && f.user_npwd1.value==''){
			alert('新密码不能为空!');
			return;		
		}		
		if (!password_match(f.user_npwd.value, f.user_npwd1.value)){
			alert('新密码不匹配!');
			return;		
		}
		
		if (!valid_password(f.user_npwd.value)){
			alert('无效的新密码!');
			return;
		}
		diffCfg('user0', 'user',  f.user.value + ';' + f.user_npwd.value);	
	}
	
	if ((f.idle_time.value < 1) || (f.idle_time.value > 60)){
		alert('无效的登录超时时间!');
		return;
	}
    
	var tmpstr = f.idle_time.value.toString();
	for(var i=0;i<tmpstr.length;i++)
	{
		if((tmpstr.charAt(i) < '0') || (tmpstr.charAt(i) > '9'))
		{
			alert("无效的登录超时时间!");
			return;
		}
	}
    
	diffCfg('idle_time', 'idle_time', f.idle_time.value * 60);	
	subForm(f,'goform/command','SYS_ADMIN', current_page());	
}


</script>
</head>

<body onload=init()>
<script>pageHead(env, env['web.page']);</script>

<form name=system_password action=goform/command method=post onsubmit="return false;">
<input type=hidden name=SYSPSC value=0>
<input type=hidden name=SYSPSC_AUX value=0>

<table width=100% cellpadding=5 cellspacing=0>
		<tr ><td colspan=2 class="tb_head">管理员</td> </tr>			
	<tr>
	<td width=30% class=tb_title>选择用户</td>
	<td width=70% class=tb_content>
		<select name=user onchange="user_select();">
			<script> document.write(user_options()); </script>
		</select>
	</td>
</tr>
<!--
<tr>
	<td width=30% class=bgblue>用户控制</td>
	<td width=70% class=bggrey>
		<select name=user_en onchange="user_en_change();">
			<option value="0">禁用</option>
			<option value="1">启用</option>
		</select>
	</td>
</tr>

<tr>
	<td width=30% class=bgblue>用户名</td>
	<td width=70% class=bggrey>
		<input name=user_name size=10 maxlength=12>
	</td>
</tr>
-->
<tr>
	<td width=30% class=tb_title>当前密码</td>
	<td width=70% class=tb_content>
		<input type=password name=user_cpwd size=12 maxlength=12 >
	</td>
</tr>
<tr>
	<td class=tb_title>新密码</td>
	<td class=tb_content>
		<input type=password name=user_npwd size=12 maxlength=12 >
		(3-12个字符)
	</td>
</tr>
<tr>
	<td class=tb_title>确认新密码</td>
	<td class=tb_content>
		<input type=password name=user_npwd1 size=12 maxlength=12 >
		(3-12个字符)
	</td>
</tr>
</table>

<table width=100% border=0 align=center cellpadding=5 cellspacing=0>
<tr>
	<td colspan=2 class=tb_head>登录超时</td>
</tr>
<tr>
	<td class=tb_title width=30%>超时</td>
	<td class=tb_content width=70%>
		<input name=idle_time size=4 maxlength=4>分钟 (有效范围:1-60) 
	</td>
</tr>
</table>
<p>
<input class="button" type=button value="确  认" onclick=apply() >
&nbsp;&nbsp;&nbsp;
<input class="button" type=button value="取  消" onclick=init() >
</p>
</form>

<script>pageTail();</script>
</body>
</html>