<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<script language=JavaScript>

pageTitle();
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var dev_status_value = new Array(<% CGI_SYS_SNMP_CATVINFO(); %>);

/* just for test
var webVars = new Array( 'EPN104', '0', '0');
var dev_status_value = new Array('5;600;1.0;0.5;2.5;80;12;0');
//*/

var REFRESH_TIME_INDEX = 0;
var TIMEOUT_INDEX = 1;
var SOFT_VERSION_INDEX = 2;
var INPUT_LASER_POWER_INDEX = 3;
var OUTPUT_LEVEL_VAL_INDEX = 4;
var TEMPER_INDEX_INDEX = 5;
var POWER_12V_INDEX = 6;
var ATTENUATION_INDEX = 7;

function get_value(INDEX)
{
	if (dev_status_value.length > 0)
	{
		var vs = dev_status_value[0].split(';');
		if (vs.length > 0) return vs[INDEX];
	}
	return false;
}
var refresh_time_val = get_value(REFRESH_TIME_INDEX)
var admin_timeout_val = get_value(TIMEOUT_INDEX);
var soft_version_val = get_value(SOFT_VERSION_INDEX);
var input_laser_power_val = get_value(INPUT_LASER_POWER_INDEX);
var output_level_val = get_value(OUTPUT_LEVEL_VAL_INDEX);
var temper_val =get_value(TEMPER_INDEX_INDEX);
var power_12v_val = get_value(POWER_12V_INDEX);
var output_level_atten_val = get_value(ATTENUATION_INDEX);

function get_dev_status() 
{
	if (dev_status_value.length > 0)
	{
		var vs = dev_status_value[0].split(';');
		if (vs.length > 0) return vs[0];
	}
	return 'N/A';	
}

function check_all_input()
{
	var f = document.forms[0];
	
	var string = f.output_level_atten.value;
	var reg = /^(\d{1,3})$/;
	if(-1 == string.search(reg))
	{
		alert("输出电平衰减设置非法，请检查输入数字的长度(1-3位)和范围！");
		return false;		
	}
	if((string > 100) || (string < 0))
	{
		alert("请输入提示范围内的整数！");
		return false;		
	}
	return true;
}

function refresh()
{
	return "GURL(\"dev_status.asp\")";
}

function time_compare_check()
{
	var sample_val = document.getElementById("sample").value; 
	if(parseInt(sample_val) > admin_timeout_val)
	{
		alert("刷新时间大于系统超时时间(" + admin_timeout_val/60 + "分)，请重新选择刷新时间");	
		return false;
	}
	return true;
}

function init()
{	
	var f=document.forms[0];
	f.sample.value = refresh_time_val;
	f.output_level_atten.value = output_level_atten_val;
	
	var sample_val = document.getElementById("sample").value;
	refresh_current_web(refresh(), parseInt(sample_val));
}

function apply_all()
{	
	var f=document.forms[0];

	if(false == time_compare_check())
	{
		return false;
	}
/*
	if (false == check_all_input())
	{	
		return false;
	}
*/
	diffCfg("sample", "dev_status_update", f.sample.value);
	diffCfg("output_level_atten", "dev_status_output_level_atten", f.output_level_atten.value);
	subForm(f,'goform/command','SNMP_CATVINFO', 'dev_status.asp');
}


</script>
</head>

<body onload=init()>
<script>pageHead(webVars);</script>

<form name=dev_status action=goform/command method=post>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>

<tr>
	<td colspan=2 class=greybluebg>状态信息</td>
</tr>

 <tr >
	<td width="40%" class=bgblue>刷新时间</td>
	<td width="60%" class=bggrey>
		<select name="sample" id="sample" >
		<option value=5>5秒</option>
		<option value=15>15秒</option>
		<option value=60>1分</option>
		<option value=600>10分</option>			    	  
		<option value=1800>30分</option>
		<option value=0>不刷新</option>
		</select>
	</td>
</tr>

<tr>
	<td class=bgblue>管理软件版本</td>
	<td class=bggrey>
		<label maxlength="15" name="soft_version" size="15">
		<script language="JavaScript">
		document.write(soft_version_val);
		</script>
		</label>
	</td>
</tr> 
<tr>
	<td class=bgblue>输入光功率 (单位:dBm)</td>
	<td class=bggrey>
		<label maxlength="15" name="input_laser_power" size="15">
		<script language="JavaScript">
		document.write(input_laser_power_val);
		</script>
		</label>
	</td>
</tr> 
<tr>
	<td class=bgblue>输出电平 (单位:dBuV)</td>
	<td class=bggrey>
		<label maxlength="15" name="output_level" size="15">
		<script language="JavaScript">
		document.write(output_level_val);
		</script>
		</label>
	</td>
</tr> 
<!--
<tr>
	<td class=bgblue>温度 (单位: ℃)</td>
	<td class=bggrey>
		<label maxlength="15" name="temper" size="15">
		<script language="JavaScript">
		document.write(temper_val);
		</script>
		</label>
	</td>
</tr>
--> 
<tr>
    <td class=bgblue>电源电压 (单位:V)</td>
	<td class=bggrey>
		<label  maxlength="15" name="power_12v" size="15">
		<script language="JavaScript">
		document.write(power_12v_val);
		</script>
		</label>
	</td>
</tr> 

<tr>
	<td width="40%" class=bgblue>输出电平衰减 (单位:dB)</td>
	<td width="60%" class=bggrey>
		<select name="output_level_atten" style="width:65px">
		<option value=0>0</option>
		<option value=2>2</option>
		<option value=4>4</option>
		<option value=6>6</option>	
		</select>
	</td>
</tr> 

<!--
					  <TR class=textCell>
						<TD>输入光功率(单位:dBm)</TD>
						<TD><LABEL maxLength=15 name=vol size=15 >@A14</TD></TR>
					  <TR class=textCell>
						<TD>输入光功率(单位:mW)</TD>
						<TD><LABEL maxLength=15 name=vol size=15 >@A21</TD></TR>
					  <TR class=textCell>
						<TD>输出电平 (单位: dBuV)</TD>
						<TD><LABEL maxLength=15 name=vol size=15 >@A15</TD></TR>
					  <TR class=textCell>
						<TD>电源电压 (单位: V)</TD>
						<TD><LABEL maxLength=15 name=vol size=15 >@A00</TD></TR>
					  <TR class=textCell>
						<TD ><script language=javascript>document.write(worksrc);</script></TD>
						<TD><LABEL maxLength=15 name=power size=15 >@A01</TD></TR>
					  <TR class=textCell >
						<TD>温度 (单位: ℃)</TD>
						<TD><LABEL maxLength=15 name=temp size=15 >@A02</TD></TR>
					  <TR class=textCell>
						<TD >串口发包数</TD> 
						<TD><LABEL maxLength=15 type=hidden name=uart_send size=15>@A09</TD></TR>
					  <TR class=textCell>
						<TD>串口收包数</TD>
						<TD><LABEL maxLength=15 name=uart_recv size=15 >@A10</TD></TR>
					  <TR class=textCell >
						<TD>网口发包数</TD>
						<TD><LABEL maxLength=15 name=eth_send size=15 >@A11</TD></TR>
					  <TR class=textCell>
						<TD >网口收包数</TD>
						<TD><LABEL maxLength=15 name=eth_recv size=15 >@A12</TD></TR>
					
-->
</table>
<p>
<input type=button value="确认" onclick=apply_all() >
<input type=button value="取消" onclick=init() >
</p>
</form>


<script>pageTail();</script>
</body>
</html>
