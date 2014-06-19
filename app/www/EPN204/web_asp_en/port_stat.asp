<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="content-type" content="text/html;charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<script language=JavaScript>


var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var link_status = new Array(<% CGI_GET_LINK_STATUS(); %>);
var link_mibs = new Array(<% CGI_GET_PORT_MIBS();%>);
/*
var link_mibs = new Array('cpu0;123457;256;0;3457465;123457;256;0;3457465',
'eth0;123457;256;0;3457465;457;86;0;57465',
'eth1;2041;26;0;3447465;1237;23;0;3465',
'cab0;1248;2;0;399965;127;27;0;37465',
'cab1;9777;25;0;3354111;7;256;0;10244');
*/

var display_ports = new Array('eth0','eth1','cab0','cab1');

pageTitle();

function reload()
{
	location.reload();
}

function port_stastr(n)
{
	 for (var i = 0; i < link_status.length; i ++){
	 	 if (link_status[i].split(';')[0] == n){
	 	 		return link_status[i];
	 	 	}
	 	}
	 return "";
}

function port_mibstr(n)
{
	 for (var i = 0; i < link_mibs.length; i ++){
	 	 if (link_mibs[i].split(';')[0] == n){
	 	 		return link_mibs[i];
	 	 	}
	 	}
	 return "";
}



function port_link(n)
{
	var sts = port_stastr(n);
	if(sts.length  && sts.split(';')[1] == 'UP') return 'Up'; else return 'Down';	
}

function port_spd(n)
{
	var sts = port_stastr(n);
		
	if(sts.length  && sts.split(';')[1] == 'UP')
		return sts.split(';')[2] + ' Mbps';
	else
		return '';
}

function port_dup(n)
{
	var sts = port_stastr(n);
		
	if(sts.length  && sts.split(';')[1] == 'UP') {
		if(sts.split(';')[3] == 'FULL') 
			return 'Full Duplex';
		else
			return 'Half Duplex';
	} 
	else
		return '';
}

function port_link_str(n)
{
	if (port_link(n) == 'Up'){
		return port_link(n) + ' / ' + port_spd(n) + ' / ' + port_dup(n);
	}	
	return port_link(n);
}


function port_stats(n, i)
{
	var sts = port_mibstr(n).split(';');;
	if (sts.length > i){
		return sts[i];	
	}	
	return '0';
}

Number.prototype.toFixed=function(len)
{
	var add = 0;
	var s, temp;
	var s1=this + "";
	var start = s1.indexOf(".");
	if (s1.substr(start+len+1,1)>=5)add=1;
	temp = Math.pow(10,len);
	s = Math.floor(this*temp) + add;
	return s/temp;	
}

function fancy_bytes(s)
{
	if (s < (1024*1024)){
		return s + ' (' + ((s*1.0)/(1024*1024)).toFixed(3) +'MB)';			
	}else if (s < (1024*1024*1024)){
		return s + ' (' + ((s*1.0)/(1024*1024)).toFixed(2) +'MB)';	
	}else {
		return s + ' (' + ((s*1.0)/(1024*1024*1024)).toFixed(2) +'GB)';	
	}
}

function port_stats_code(n)
{
	return " <tr> <td class=greybluebg>" + port_name(n) + "</td>"+
	 "<td class=bggrey>" + port_link_str(n) + "</td>"+
	 "<td class=bggrey></td>" +
	 "<td class=bggrey></td>" +
	"</tr>"+
	"<tr>"+
	"	<td width=15% class=bgblue>Rx Pkts</td>"+
	"	<td width=30% class=bggrey>" + port_stats(n, 1) + "</td>"+
	"	<td width=15% class=bgblue>Tx Pkts</td> "+
	"	<td width=30% class=bggrey>" + port_stats(n, 5) + "</td>" +	
	"</tr>"+
	"<tr>"+
	"	<td class=bgblue>Rx B/Mcast</td>"+
	"	<td class=bggrey>" + port_stats(n, 2) + "</td>" +
	"	<td class=bgblue>Tx B/Mcast</td>"+
	"	<td class=bggrey>" + port_stats(n, 6) + "</td>" +
	"</tr>" +	
	"<tr>" +
	"	<td class=bgblue>Rx Errors</td>"+
	"	<td class=bggrey>" + port_stats(n, 3) + "</td>" +	
	"	<td class=bgblue>Tx Drops</td>"+
	"	<td class=bggrey>" + port_stats(n, 7) + "</td>" +
	"</tr>	"+
	"<tr>"+
	"	<td class=bgblue>Rx Bytes</td>"+
	"	<td class=bggrey>" + fancy_bytes(port_stats(n, 4)) + "</td>" +
	"	<td class=bgblue>Tx Bytes</td>"+
	"	<td class=bggrey>" + fancy_bytes(port_stats(n, 8)) + "</td>" +
	"</tr>";
}

function ports_html_code()
{
	var str = "";
	for (var i = 0; i < display_ports.length; i ++){
			str += port_stats_code(display_ports[i]);
	}
	return str;	
}

function reset_all()
{
	f=document.forms[0];
	diffCfg("reset0", "reset", "all");
	subForm(f,'goform/command','IF_MIBS',cPage);	
}


function init()
{
	
}
</script>
</head>

<body onload=init()>
<script>pageHead(webVars);</script>
<p>
<input type=button value=Refresh onclick=reload()> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input type=button value='Reset All' onclick=reset_all()>
</p>
<form name=system_status action=goform/command method=post>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<script>document.write(ports_html_code())</script>
</table>
</form>

<script>pageTail();</script>
</body>
</html>
