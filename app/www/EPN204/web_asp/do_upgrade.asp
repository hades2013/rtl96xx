
<html>
<head> 
<meta http-equiv=Content-Type content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<script language=JavaScript>
pageTitle();
var msgs = new Array();
msgs['e_boot'] = '系统正在擦除引导区...';
msgs['w_boot'] = '系统正在更新引导区...';
msgs['e_rootfs'] = '系统正在擦除根文件系统...';
msgs['w_rootfs'] = '系统正在更新根文件系统...';
msgs['e_kernel'] = '系统正在擦除内核...';
msgs['w_kernel'] = '系统正在更新内核...';
msgs['fail'] = '系统升级失败,错误代码:';
msgs['ok'] = '系统升级成功,正在重启中,请稍等!';
msgs['unknown'] = '未知状态!!!';

//var webVars = new Array("CLT502", '1', '1');
//var upgrade_state = new Array("w_rootfs", '60', '-2');

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var upgrade_state = new Array( <% CGI_GET_UPGRADE_STATE(); %>);


var delay = 25;
var count = 0;

function to_root(){
	location = 'index.asp';
}

function get_process()
{
	if (upgrade_state.length > 0){
		return upgrade_state[0];	
	}
	return 'unknown';
}

function get_percentage()
{
	if (upgrade_state.length > 1){
		return upgrade_state[1];	
	}
	return '0';	
}

function get_errorcode()
{
	if (upgrade_state.length > 2){
		return upgrade_state[2];	
	}
	return '-1';	
}



function refresh_self()
{
	location = 'do_upgrade.asp';	
}

function get_message()
{
	var process = get_process();
	if ((process == 'ok') || (process == 'unknown')){
		return msgs[process];		
	}else if (process == 'fail'){
		return msgs[process] + get_errorcode();	
	}else {
		return msgs[process] + get_percentage() + '%';	
	}
}


function start_delay()
{
	var rest = 0;
	
	if(count < delay) {
		count ++;
		setTimeout("start_delay()", 1000);
		rest = delay - count;
		document.getElementById('dmsg').innerHTML = '<p class=bluetextbold><font size=4 face="Arial, Helvetica, sans-serif">Waiting: ' 
			+ rest + ' s.</font></p>';
	}
	if(count >= delay) {
		document.forms[0].cbtn.disabled = false;
		document.getElementById('dmsg').innerHTML = '<p class=bluetextbold><font size=4 face="Arial, Helvetica, sans-serif">&nbsp;</font></p>';
	}	
}

function start_polling()
{
	var p = get_process();
	if ((p != 'fail') && (p != 'ok') && (p != 'unknown')){
			setTimeout("refresh_self()", 3000);
	}	else {
		if (( p == 'fail') || ( p == 'unknown')){
			count = delay;
		}else {
			count = 0;	
		}
		start_delay();
	}
}


function init()
{
	document.forms[0].cbtn.disabled = true;
	start_polling();			
}
</SCRIPT>
</HEAD>

<body onload="init();">
<script>pageHead(webVars);</script>
<form>
<table height=250 width=100% border=0 cellpadding=0 cellspacing=0>
<tr>
	<td>
		<div align=center>
		<table width=400 border=0 cellpadding=5 cellspacing=1>
		<tr>
			<td align=center>
				<p class=bluetextbold>
				<font size=4 face="Arial, Helvetica, sans-serif"><script>	document.writeln(get_message());</script>
				</font>
				</p>
			</td>
		</tr>
		<tr>
			<td align=center>
				<div id="dmsg"></div>
			</td>
		</tr>
		<tr>
			<td align=center>
				<p>
				<input name=cbtn type=button value="    继续    " onclick="to_root();" class=btn_o>
				</p>
			</td>
		</tr>
		</table>
		</div>
	</td>
</tr>
</table>
</form>
<script>pageTail();</script>
</body>
</html>