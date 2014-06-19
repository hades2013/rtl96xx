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
msgs['unknown'] = 'Unknown!';
msgs['ok'] = 'Success!';
msgs['fail'] = 'Failed!!! Error Code = ';
msgs['reboot'] = 'Rebooting, please wait!';
msgs['restore'] = 'System is restoring default settings, please wait!';
msgs['cfgimport'] = 'System has successfully imported new configurations, please wait!';
msgs['upgrade'] = 'Software download successfully, system is performing upgrade, please wait!';

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
		 	msg = msgs[MSG] + ERR +'.';
		 	if (ERRSTR != '') msg += '[' + ERRSTR + ']';
		 }else if (MSG == 'cfgimport'){
		 	 msg = "System has successfully imported " + ERR + " items, system is going to reboot, please wait!";	
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
				<input name=cbtn type=button value="    Continue    " onclick="gurl(GO);" class=btn_o>
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

