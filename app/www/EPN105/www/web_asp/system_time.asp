<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<script language=JavaScript>
var str_copyright =<%CGI_GET_COPYRIGHT();%>
var PROD_MODEL = <%CGI_GET_MODEL();%>
</SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>

<script language=JavaScript>

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

pageTitle();

var sys_time = new Array(<% CGI_SYS_TIME(); %>);

function get_time()
{
	return (sys_time.length > 1) ? sys_time[1] : "00:00:00";	
}

function get_date()
{
	return (sys_time.length > 0) ? sys_time[0] : "01:01:1970";
}
//add by zhouguanhua 
function get_ntpclient()
{
	var nu = (sys_time.length > 3) ? sys_time[3] : "0";
	i = Number(nu);
	if(i==1)
	return "        时间与网络同步" ;
	else 
	return "        时间未与网络同步" ;	
}
//end add by zhouguanhua 

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

function get_time_sync()
{
	if(sys_time.length > 2){
		var vs = sys_time[2].split(';');
		if (vs.length > 1) return (vs[1] == '0') ? false : true;	 
	}
	return false;	
}

String.prototype.trim = function()
{
	return this.replace(/(^\s*)|(\s*$)/g,"");
}




function apply() 
{
	f=document.forms[0];
	
	string = f.tSERVER.value;
	reg = /^(([1-9]|[1-9]\d|1\d\d|2[0-1]\d|22[0-3])\.){1}(([01]?\d?\d|2[0-4]\d|25[0-5])\.){2}([1-9]|[1-9]\d|1\d\d|2[0-4]\d|25[0-4])$/;
	var vs = string.split('.');		
	if(f.tSYNC.checked && ((-1 == string.search(reg)) || (127 == vs[0])))
	{
		alert("时间服务器IP地址非法！");
		return ;	
	}
	
	var utc = new Date(Date.UTC(f.tYEAR.value,f.tMON.value-1,f.tDAY.value,f.tHOUR.value,f.tMIN.value,f.tSEC.value));
	
	diffCfg("time_zone", "time_zone", f.tZONE.value);
	diffCfg("time_sync", "time_sync", f.tSYNC.checked ? "1" : "0");
	diffCfg("time_server", "time_server", f.tSERVER.value);
	diffCfg("time_utc", "time_utc", utc.getTime()/1000);
	subForm(f,'goform/command','SYS_TIME','system_time.asp');
}

var WeekAlias=["Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"];
var MonthAlias=["January","February","March","April","May","June","July","August","September","October","November","December"];

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

function on_sync_change()
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
}

function localtime2string()
{
	var date = get_date();
	var time = get_time();
	var str = "";
	
	var vs = date.split("/");
	if (vs.length == 3){
		str += MonthAlias[parseInt(eval(vs[0])) - 1] + " " + vs[1] + " " + vs[2];
	}else {
		return "N/A";	
	}
	
	vs = time.split(":");
	if (vs.length == 3){
		str += " " + time_format(vs[0]) + ":" + time_format(vs[1]) + ":" +time_format(vs[2]);
	}else {
		return "N/A";	
	}
	return str;
}

var pc_date = new Date();

function init()
{
	f=document.forms[0];
	document.getElementById("localTime").innerHTML = localtime2string() ;
	document.getElementById("sy_stat").innerHTML = get_ntpclient() ;
	f.tZONE.value = get_time_zone();
	f.tSYNC.checked = get_time_sync() ? true : false;
	f.tSERVER.value = get_time_server();
	on_input_time_change();
	on_sync_change();
	f.tMON.value=pc_date.getMonth() + 1;
	f.tDAY.value=pc_date.getDate();
	f.tYEAR.value=pc_date.getFullYear();
	f.tHOUR.value=pc_date.getHours();
	f.tMIN.value=pc_date.getMinutes();
	f.tSEC.value=pc_date.getSeconds();
}


</script>
</head>

<body onload=init()>
<script>pageHead(webVars,PROD_MODEL);</script>

<form name=system_time action=goform/command method=post>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td width=30% class=bgblue>本地时间</td>
	<td width=70% class=bggrey><span><font color=green size=3><b id=localTime> </b></font></span></td>
</tr>
<tr>
	<td width=30% class=bgblue>同步网络时间状态</td>
	<td width=70% class=bggrey><span><font color=green size=2><b id=sy_stat> </b></font></span></td>
</tr>
<tr>
	<td class=bgblue>时区</td>
	<td class=bggrey>
		 <select name=tZONE style="color: #000000" size=1>
			<script>genTimeOpt();</script>
		</select>
	</td>
</tr>
<tr>
	<td class=bgblue>自动同步(可选)</td>
	<td class=bggrey>
		<table width=100% border=0 cellpadding=3 cellspacing=1>
		<tr>
			<td width=20% nowrap>同步</td>
			<td width=80%><input type=checkbox name=tSYNC onClick=on_sync_change();></td>
		</tr>
		<tr>
			<td nowrap>时间服务器</td>
			<td>
			<input name=tSERVER size=25 maxlength=30>（例:202.112.29.82）
			<input style="display:none" mce_style="display:none"><!--只有一个input类型为text的,使其回车不提交表单-->
			</td>
		</tr>
		<tr>
		<td></td>
		<td>若时间未同步,请检查ONU到时间服务器是否网络可达.</td>
		</tr>
        </table>
	</td>
</tr>
<tr>
	<td class=bgblue>时间设置</td>
	<td class=bggrey>
		<table width=100% border=0 cellpadding=3 cellspacing=1>
		<tr>
			<td>
				<select name=tYEAR size=1 onChange=on_input_time_change();>
	  				<script>createSelectTagChild(1971,2037,pc_date.getFullYear(),"")</script>
	  			</select>Year
				<select name=tMON size=1 onChange=on_input_time_change();>
	  				<script>createSelectTagChild(1,12,pc_date.getMonth() + 1,"month")</script>
	  			</select>Month
				<select name=tDAY size=1>
	  				<script>createSelectTagChild(1,31,pc_date.getDate(),"")</script>
	  			</select>Day
			</td>
		</tr>
		<tr>
			<td>
				<select size=1 name=tHOUR>
	  				<script>createSelectTagChild(0,23,pc_date.getHours(),"")</script>
	  			</select>Hour
				<select size=1 name=tMIN>
	  				<script>createSelectTagChild(0,59,pc_date.getMinutes(),"")</script>
	  			</select>Minute
				<select size=1 name=tSEC>
	  				<script>createSelectTagChild(0,59,pc_date.getSeconds(),"")</script>
	 		 	</select>Second
			</td>
		</tr>
		</table>
	</td>
</tr>
</table>
<p>
<input type=button value="确定" onclick=apply()>
<input type=button value="取消" onclick=init()>
</p>
</form>

<script>pageTail(str_copyright);</script>
</body>
</html>
