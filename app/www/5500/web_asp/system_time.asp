<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 日期及时间 </title>
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
env['sys.time'] = '946685203';
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

//var sys_time = new Array('1/1/2000', '0:6:43', '25;0;pool.ntp.org;604800;0;0');
var sys_time =new Array(<% CGI_SYS_TIME(); %>);



function get_time()
{
	return (sys_time.length > 1) ? sys_time[1] : "00:00:00";	
}

function get_date()
{
	return (sys_time.length > 0) ? sys_time[0] : "01:01:1970";
}


function get_time_zone()
{
	if(sys_time.length > 2){
		var vs = sys_time[2].split(';');
		if (vs.length > 0) return vs[0];	 
	}
	return '22';	
}

function get_time_server()
{
	if(sys_time.length > 2){
		var vs = sys_time[2].split(';');
		if (vs.length > 2) return vs[2];	 
	}
	return '';	
}

function get_ntp_enabled()
{
	if(sys_time.length > 2){
		var vs = sys_time[2].split(';');
		if (vs.length > 1) return (vs[1] == '0') ? false : true;	 
	}
	return false;	
}

function get_ntp_sync_interval()
{
	if(sys_time.length > 2){
		var vs = sys_time[2].split(';');	
		if (vs.length > 3) {
		   var interval = parseInt(vs[3], 10);
		   if (interval == 3600){
		        return '3600';
		   }else if (interval == 604800){
		        return '604800';
		   }else if (interval == 2592000){
		        return '2592000';
		   }else {
		        return '86400'; 
		   }
		}
	}
	return '86400';
}

function get_ntp_synced_status()
{
	if(sys_time.length > 2){
		var vs = sys_time[2].split(';');	
		if (vs.length > 4) return (vs[4] == '0') ? false : true;
	}	
	return false;
}

function get_ntp_already_synced()
{
	if(sys_time.length > 2){
		var vs = sys_time[2].split(';');	
		if (vs.length > 5) {
			return (vs[5] == '0') ? false : true;
		}
	}	
	return false;
}


function get_ntp_synced_time()
{
	if(sys_time.length > 2){
		var vs = sys_time[2].split(';');	
		if (vs.length > 5) {
			return time_string(vs[5]);
		}
		return '0';
	}	
}

function time_format(t)
{
	var i = Number(t);
	return (i < 10) ? '0' + i : i;	
}	

function time_string(v)
{
	var v = parseInt(v, 10);
	if (v == 0) return 'N/A';
	
	var t = new Date();
	t.setTime(v*1000);
	
	var str = t.getUTCFullYear()  + "-" + time_format(t.getUTCMonth() + 1) + "-" + time_format(t.getUTCDate()) + " " + time_format(t.getUTCHours()) + ":" +  time_format(t.getUTCMinutes());
	return str;
}


String.prototype.trim = function()
{
	return this.replace(/(^\s*)|(\s*$)/g,"");
}

function ntp_sync_apply()
{
	//var f = document.forms[0];
	//subForm(f,'goform/command','NTP_SYNC','system_time.asp');
	location.reload();
}

function apply() 
{
	f=document.forms[0];
	
	if (f.tSYNC.checked && ((f.tSERVER.value.trim() == '')
                 || (!valid_xname(f.tSERVER.value, 127)))){
		alert("请输入有效的时间服务器的URL！");
		return ;	
	}
	
	var utc = new Date(Date.UTC(f.tYEAR.value,f.tMON.value-1,f.tDAY.value,f.tHOUR.value,f.tMIN.value,f.tSEC.value));
	
	diffCfg("time_zone", "time_zone", f.tZONE.value);
	diffCfg("time_sync", "time_sync", f.tSYNC.checked ? "1" : "0");
	diffCfg("time_server", "time_server", f.tSERVER.value);
	diffCfg("time_utc", "time_utc", utc.getTime()/1000);	
	//diffCfg("time_interval","time_interval",f.tUPCYCLE.value);
	subForm(f,'goform/command','SYS_TIME','system_time.asp');

}

var WeekAlias=["Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"];
var MonthAlias=["1","2","3","4","5","6","7","8","9","10","11","12"];

function time_format(t)
{
	var i = Number(t);
	return (i < 10) ? '0' + i : i;	
}	

function createSelectTagChild(start,end,selected,special_case)
{
	var str=new String("");

	for(var i=start; i <= end; i++)
	{
		switch (special_case)
		{
			case "week":
					str+="<option value="+i+ ((i==selected) ? " selected>" : ">") + WeekAlias[i] +"</option>";
				 break;
			case "month":
					str+="<option value="+i+ ((i==selected) ? " selected>" : ">") + MonthAlias[i - 1] +"</option>";
				 break;
			default:
					str+="<option value="+i+ ((i==selected) ? " selected>" : ">") + time_format(i) +"</option>";
				 break;
		}
	}
	document.writeln(str);
}

function on_input_time_change()
{
	var f=document.forms[0];
	var days = new Array(31,28,31,30,31,30,31,31,30,31,30,31);
	if((f.tYEAR.value%4) == 0) days[1] = 29;
	var v = f.tDAY.value;
	
  for(i = 0; i < f.tDAY.length; i++){
		f.tDAY.options[i] = null;
  }
  for(i = 0; i < days[eval(f.tMON.value) - 1]; i++){
		f.tDAY.options[i] = new Option(i + 1, i + 1);
		if (v == i + 1) f.tDAY.value = v;
  }
}

function on_ntp_enable_change()
{
	var f=document.forms[0];
	var c = f.tSYNC.checked;

	f.tMON.disabled=c;
	f.tDAY.disabled=c;
	f.tYEAR.disabled=c;
	f.tHOUR.disabled=c;
	f.tMIN.disabled=c;
	f.tSEC.disabled=c;
	f.tSERVER.disabled=!c;
	f.tUPCYCLE.disabled=!c;

	f.startSync.disabled=(!get_ntp_enabled() || !c) ? true : false;
}

function localtime2string()
{
	var f=document.forms[0];
	var date = get_date();
	var time = get_time();
	var str = "";
	var ty, tm, td, th, tmin, ts;
	var vs = date.split("/");
	ty = vs[2];
	tm = parseInt(eval(vs[0]), 10);
	td = vs[1];
	vs = time.split(":");	
	th = vs[0];
	tmin = vs[1];
	ts = vs[2];
	
	var d = new Date();
	var lt = Date.parse(date + ' ' + time)/1000;
	var utc;
	var def_tz = getTimeZoneOffsetByIndex(get_time_zone());
	var set_tz = getTimeZoneOffsetByIndex(f.tZONE.value);
	var utc = lt - def_tz;
	var new_lt = new Date();
	new_lt.setTime((utc + set_tz)*1000);
	
	str += MonthAlias[new_lt.getMonth()] + "-" + new_lt.getDate() + "-" + new_lt.getFullYear();
	str += " " + time_format(new_lt.getHours()) + ":" + time_format(new_lt.getMinutes()) + ":" +time_format(new_lt.getSeconds());
	return str;
}

var pc_date = new Date();


function on_time_zone_change()
{
	document.getElementById("localTime").innerHTML = localtime2string();
}

function get_ntpclient()
{
	var nu = (sys_time.length > 3) ? sys_time[3] : "0";
	i = Number(nu);
	if(i==1)
	return "同步成功！" ;
	else 
	return "未同步！" ;	
}
	
/*
function ntp_status_update()
{
    var str='';
    if (get_ntp_enabled()){
        if (get_ntp_already_synced()){
            str += '在 ' + get_ntp_synced_time() + ' ';
            if (get_ntp_synced_status()) {
                str += '同步成功！';    
            }else {
                str += '同步失败！ ';    
            }
        }else {
            str += "Not Sync ";    
        }        
        document.getElementById("syncStatus").innerHTML = str;	 
}	
}
*/
function init()
{
	var f=document.forms[0];
	f.tZONE.value = get_time_zone();
	f.tSYNC.checked = get_ntp_enabled() ? true : false;
	f.tSERVER.value = get_time_server();
	//f.tUPCYCLE.value = get_ntp_sync_interval();
	f.tUPCYCLE.value = '86400';
    // update ntp status
    //ntp_status_update();
	on_time_zone_change();
	on_input_time_change();
	on_ntp_enable_change();
	
	document.getElementById("sy_stat").innerHTML = get_ntpclient() ;
}


</script>
</head>
<body onload=init()>
<script>pageHead(env, env['web.page']);</script>
<form name=system_time action=goform/command method=post onsubmit="return false;">
	<table width=100% cellpadding=5 cellspacing=0>
			<tr ><td colspan=2 class="tb_head" >日期及时间</td> </tr>			
			<tr>
				<td class=tb_title>本地时间</td>
				<td class=tb_content><span><font color=green size=2><b id=localTime> </b></font></span>&nbsp;&nbsp;&nbsp;
				    <b id=syncStatus></b>
				</td>
			</tr>
<tr>
	<td  class=tb_title>时间同步状态</td>
	<td  class=tb_content><span><font color=green size=2><b id=sy_stat> </b></font></span></td>
</tr>
<tr>
	<td class=tb_title>时区</td>
	<td class=tb_content>
		 <select name=tZONE style="color: #000000" size=1 onChange="on_time_zone_change();">
			<script>genTimeOpt();</script>
		</select>
	</td>
</tr>
<tr>
	<td class=tb_title>自动同步(可选)</td>
	<td class=tb_content>
		<table class="table_noborder" width=100% border=0 cellpadding=3 cellspacing=1>
		<tr>
			<td width=20% nowrap>同步</td>
			<td width=80%><input type=checkbox name=tSYNC onClick=on_ntp_enable_change(); > &nbsp;&nbsp;&nbsp;
			<input type=button name=startSync value="立即更新" onClick=ntp_sync_apply(); >
			</td>
		</tr>
		<tr>
			<td nowrap>时间服务器</td>
			<td><input name=tSERVER size=25 maxlength=30><font color=blue><br />
				( 推荐： 210.72.145.44 )</font>
			    <input style="display:none" mce_style="display:none"><!--只有一个input类型为text的,使其回车不提交表单-->	
			</td>
		</tr>
		<tr>
			<td width=20%> 更新周期 </td>
			<td width=80%>
			 	<select name=tUPCYCLE >
				<!--<option value='3600'>每小时</option>-->
				<option value='86400'>每天</option>
				<!--<option value='604800'>每星期</option>
				<option value='2592000'>每个月</option>-->
				</select>
			</td>
		</tr>
        </table>
	</td>
</tr>
<tr>
	<td class=tb_title>时间设置</td>
	<td class=tb_content>
		<table class="table_noborder" width=100% border=0 cellpadding=3 cellspacing=1>
		<tr>
			<td >
				<select name=tYEAR size=1 onChange=on_input_time_change();>
	  				<script>createSelectTagChild(2011,2037,pc_date.getFullYear(),"")</script>
	  			</select>年
				<select name=tMON size=1 onChange=on_input_time_change();>
	  				<script>createSelectTagChild(1,12,pc_date.getMonth() + 1,"month")</script>
	  			</select>月
				<select name=tDAY size=1>
	  				<script>createSelectTagChild(1,31,pc_date.getDate(),"")</script>
	  			</select>日
			</td>
		</tr>
		<tr>
			<td>
				<select size=1 name=tHOUR>
	  				<script>createSelectTagChild(0,23,pc_date.getHours(),"")</script>
	  			</select>时
				<select size=1 name=tMIN>
	  				<script>createSelectTagChild(0,59,pc_date.getMinutes(),"")</script>
	  			</select>分
				<select size=1 name=tSEC>
	  				<script>createSelectTagChild(0,59,pc_date.getSeconds(),"")</script>
	 		 	</select>秒
			</td>
		</tr>
		</table>
	</td>
</tr>
</table>
<p>
<input class="button" type=button value="确  认" onclick=apply()>&nbsp;&nbsp;&nbsp;	
<input class="button" type=button value="取  消" onclick=init()>
</p>
</form>
<script>pageTail();</script>
</body>
</html>