<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<script language=JavaScript>

var msgs = new Array();
msgs['unknown'] = '未知错误!';
msgs['ok'] = '配置成功!';
//msgs['fail'] = '配置失败!!! 错误代码: ';
msgs['fail'] = '配置失败!';
msgs['reboot'] = '系统正在重启中...请稍等!';
msgs['restore'] = '系统正在恢复出厂设置,请稍等!';
msgs['cfgimport'] = '系统已成功导入配置文件,导入配置文件后会自动重启,请稍等!';
msgs['upgrade'] = '系统软件下载成功,系统正在执行更新,请稍等!';

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

var GO	= QueryString('GO');
var MSG = QueryString('MSG');
var DELAY = QueryString('DELAY');
var ERR = QueryString('ERR');
var ERRSTR = QueryString('ERRSTR');
var COUNT = 0;

function gurl(u) {
	if (u=='') u = 'index.asp'; 
	location=u; 
}

function get_message()
{
	var msg;
	if (MSG){
		 msg = msgs[MSG];
		 if (MSG == 'fail'){
		 	//msg = msgs[MSG] + ERR +'.';
		 	msg = msgs[MSG];
		 //	if (ERRSTR != '') msg += '[' + ERRSTR + ']';
		 }else if (MSG == 'cfgimport'){
		 	 msg = "系统已成功导入 " + ERR + " 项配置, 系统正在重启,请稍等!";	
		 }
	}
	return msg ? msg : msgs['unknown'];
}


function start_delay()
{
	var rest = 0;
	
	if(COUNT < DELAY) {
		COUNT ++;
		setTimeout("start_delay()", 1000);
		rest = DELAY - COUNT;
		document.getElementById('dmsg').innerHTML = '<p class=bluetextbold><font size=4 face="Arial, Helvetica, sans-serif">Waiting: ' 
			+ rest + ' s.</font></p>';
	}
		if(COUNT >= DELAY) {
			document.forms[0].cbtn.disabled = false;
			document.getElementById('dmsg').innerHTML = '<p class=bluetextbold><font size=4 face="Arial, Helvetica, sans-serif">&nbsp;</font></p>';
		if (MSG == 'upgrade'){
			gurl('do_upgrade.asp');	
		}
	}
}

function init()
{
	if (DELAY > 0){
		document.forms[0].cbtn.disabled = true;
		start_delay();			
	}
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
				<input name=cbtn type=button value="    继续    " onclick="gurl(GO);" class=btn_o>
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

