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
<script language=JavaScript>

pageTitle();

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var loopback_value = new Array(<% CGI_LOOPBACK(); %>);

/* just for test
var webVars = new Array( 'EPN104', '0', '0');
var loopback_value = new Array('0;3;3;3;3;0'); //开启/关闭  uni1 2 3 4  update_time
*/



function get_uni_string(number)
{	
	if(0 == number)
	{	
		return 'disabled';    
	}
	else if(1 == number)
	{
		return 'blocking';
	}
	
	else if(2 == number)
	{
		return 'listening';
	}
	
	else if(3 == number)
	{
		return 'forwarding';
	}
	else
	{	
		return 'N/A';
	}
}

function get_loopback()
{
	if (loopback_value.length > 0)
	{
		var vs = loopback_value[0].split(';');
		if (vs.length > 0) return vs[0];
	}
	return false;	
}

function get_uni1_value()
{	
	if (loopback_value.length > 0)
	{
		var vs = loopback_value[0].split(';');
		if (vs.length > 4)
		{	
			if(0 == vs[0])
			{
				return get_uni_string(vs[0]);
			}
			else
			{
				return get_uni_string(vs[1]); 
			}
		}
	}
	return 'N/A';
}

function get_uni2_value()
{	
	if (loopback_value.length > 0)
	{
		var vs = loopback_value[0].split(';');
		if (vs.length > 4)
		{	
			if(0 == vs[0])
			{
				return get_uni_string(vs[0]);
			}
			else
			{
				return get_uni_string(vs[2]); 
			}
		}
	}
	return 'N/A';
}
function get_uni3_value()
{	
	if (loopback_value.length > 0)
	{
		var vs = loopback_value[0].split(';');
		if (vs.length > 4)
		{	
			if(0 == vs[0])
			{
				return get_uni_string(vs[0]);
			}
			else
			{
				return get_uni_string(vs[3]); 
			}
		}
	}
	return 'N/A';
}
function get_uni4_value()
{	
	if (loopback_value.length > 0)
	{
		var vs = loopback_value[0].split(';');
		if (vs.length > 4)
		{	
			if(0 == vs[0])
			{
				return get_uni_string(vs[0]);
			}
			else
			{
				return get_uni_string(vs[4]); 
			}
		}
	}
	return 'N/A';
}

var uni1_string = get_uni1_value();
var uni2_string = get_uni2_value();
var uni3_string = get_uni3_value();
var uni4_string = get_uni4_value();

function get_loop_update_time()
{
	if (loopback_value.length > 0)
	{
		var vs = loopback_value[0].split(';');
		if (vs.length > 0) return vs[5];
	}
	return false;	
}

function refresh()
{
	return "GURL(\"loopback_detection.asp\")";
}

function init()
{
	var web = 'loopback_detection.asp';
	var f=document.forms[0];
	f.LOOPBACK.value = get_loopback();
	f.sample.value = get_loop_update_time();
	
	var sample_val = document.getElementById("sample").value;
	refresh_current_web(refresh(), parseInt(sample_val));
}

function apply_all()
{	
	var f=document.forms[0];
	
	diffCfg("LOOPBACK", "loopback", f.LOOPBACK.value);
	diffCfg("sample", "sample", f.sample.value);
	subForm(f,'goform/command','SYS_LOOPBACK', 'loopback_detection.asp');
}

</script>
</head>

<body onload=init()>
<script>pageHead(webVars,PROD_MODEL);</script>

<form name=form1 action=goform/command method=post>


<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td colspan=2 class=greybluebg>环路检测</td>
</tr>
<tr>
	<td class=bgblue>环路检测状态</td>
	<td class=bggrey><select name=LOOPBACK >
						<option value=0>关闭</option>
						<option value=1>开启</option></td>
	<tr >
		<td width="35%" class=bgblue>自动刷新时间</td>
		<td width="65%" class=bggrey>
			<select name="sample" id="sample" >
			<option value=10>10秒</option>
			<option value=30>30秒</option>
			<option value=60>1分</option>
			<option value=0>不刷新</option>
			</select>
		</td>
	</tr>
</tr> 
<tr>
	<td colspan=2 class=greybluebg>端口环路状态</td>
</tr>

<tr>
	<td class=bgblue>端口</td>
	<td align=center class=bggrey>端口环路状态</td>
</tr>
<tr>
	<td class=bgblue>1</td>
	<td align=center class=bggrey>
		<label maxlength="15" name="uni1" size="15">
		<script language="JavaScript">
		document.write(uni1_string);
		</script>
		</label>
	</td>
</tr>
<tr>
	<td class=bgblue>2</td>
	<td align=center class=bggrey>
		<label maxlength="15" name="uni3" size="15">
		<script language="JavaScript">
		document.write(uni2_string);
		</script>
		</label>
	</td>
</tr>
<tr>
	<td class=bgblue>3</td>
	<td align=center class=bggrey>
		<label maxlength="15" name="uni3" size="15">
		<script language="JavaScript">
		document.write(uni3_string);
		</script>
		</label>
	</td>
</tr>
<tr>
	<td class=bgblue>4</td>
	<td align=center class=bggrey>
		<label maxlength="15" name="uni4" size="15">
		<script language="JavaScript">
		document.write(uni4_string);
		</script>
		</label>
	</td>
</tr>
	
</table>

<p>
<input type=button value="确认" onclick=apply_all() >
<input type=button value="取消" onclick=init() >
<input type=button value="刷新" onclick=GURL("loopback_detection.asp");>
</p>
<table>
 <tr>
	<td><b>说明:</b></td>  
	 <td>(1) disabled表示环路检测功能未开启或端口处于禁闭状态;</td>  
</tr>

 <tr>
	 <td>&nbsp;</td> 
	 <td>(2) blocking表示端口因环路而处于阻塞状态;</td>  
	
 </tr>
 <tr>
	 <td>&nbsp;</td> 
	 <td>(3) listening表示端口处于侦听环路状态;</td>  
	
 </tr>
 <tr>
	 <td>&nbsp;</td> 
	 <td>(4) forwarding表示端口环路检测状态正常.</td>  
	
 </tr>
 
 </table>


</form>


<script>pageTail(str_copyright);</script>
</body>
</html>
