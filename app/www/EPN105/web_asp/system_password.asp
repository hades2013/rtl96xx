<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<script language=JavaScript>
var str_copyright =<%CGI_GET_COPYRIGHT();%>
var PROD_MODEL = <%CGI_GET_MODEL();%>
</SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=valid.js></SCRIPT>
<script language=JavaScript>

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var sysadmin = new Array(<% CGI_SYS_ADMIN(); %>);

pageTitle();


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


//function user_en_change()
//{
//	f=document.forms[0];
//	var dis = (f.user_en.value == '0') ? true : false;
	//f.user_name.disabled = dis;
//	f.user_cpwd.disabled = dis;
//	f.user_npwd.disabled = dis;	
//	f.user_npwd1.disabled = dis;	
//}


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

function apply()
{
	f=document.forms[0];
	var user_change = 0;
	//if (f.user_en.value != user_get_en(f.user.value)){
	//	user_change = 1;
	//}else 
	if ((f.user_cpwd.value != '') || (f.user_npwd.value != '') || (f.user_npwd1.value != '')){
		user_change = 1;
	}
	
	if (user_change == 1){
		//if (!valid_username(f.user_name.value)){
		///	alert('无效的用户名,请重新输入.');
		//	return;		
		//}
		//if (username_exist(f.user.value, f.user_name.value)){
		//	alert('用户名已经被使用,请输入其他用户名.');
		//	return;
		//}
		
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
	//}else
	//if (user_change == 1) {
		//diffCfg('user0', 'user', f.user.value + ';0;;');
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
	subForm(f,'goform/command','SYS_ADMIN', cPage);	
}


</script>
</head>

<body onload=init()>
<script>pageHead(webVars,PROD_MODEL);</script>

<form name=system_password action=goform/command method=post>
<input type=hidden name=SYSPSC value=0>
<input type=hidden name=SYSPSC_AUX value=0>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td colspan=2 class=greybluebg>管理员</td>
</tr>
<tr>
	<td width=30% class=bgblue>选择用户</td>
	<td width=70% class=bggrey>
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
	<td width=30% class=bgblue>当前密码</td>
	<td width=70% class=bggrey>
		<input type=password name=user_cpwd size=12 maxlength=12 >
	</td>
</tr>
<tr>
	<td class=bgblue>新密码</td>
	<td class=bggrey>
		<input type=password name=user_npwd size=12 maxlength=12 >
		(3-12 characters)
	</td>
</tr>
<tr>
	<td class=bgblue>确认新密码</td>
	<td class=bggrey>
		<input type=password name=user_npwd1 size=12 maxlength=12 >
		(3-12 characters)
	</td>
</tr>
</table>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td colspan=2 class=greybluebg>登录超时</td>
</tr>
<tr>
	<td class=bgblue width=30%>超时</td>
	<td class=bggrey width=70%>
		<input name=idle_time size=4 maxlength=4>Minute(1-60)
	</td>
</tr>
</table>
<p>
<input type=button value="确认" onclick=apply() >
<input type=button value="取消" onclick=init() >
</p>
</form>

<script>pageTail(str_copyright);</script>
</body>
</html>
