<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 端口状态及统计信息 </title>
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
env['sys.time'] = '946684897';
env['sys.linkup'] = '0';
env['sys.linkoff'] = '0';
env['sys.busy'] = '0';
env['sys.ipc'] = '0';
env['sys.vlan'] = '0';
env['sys.unsaved'] = webVars[1];
env['sys.feature'] = 'vlanpool;mme_drop_ports;slave_access_mgmt;bcmp;';


env['web.page'] = current_page();env['sys.parent_name'] = '端口设置';

pageRedirectCheck(env);
</script>
<!-- end header -->

<script language=JavaScript>

var link_status = new Array( 'cpu0;UP;1000;FULL',
'eth0;UP;1000;FULL',
'eth1;DOWN;10;HALF',
'cab0;DOWN;10;HALF',
'cab1;DOWN;10;HALF',
'cab2;DOWN;10;HALF',
'cab3;DOWN;10;HALF');
//stats_rx_pkts stats_tx_pkts stats_rx_bytes stats_tx_bytes stats_rx_bcast stats_tx_bcast
//stats_rx_mcast stats_tx_mcast stats_rx_fc stats_tx_fc stats_rx_errs stats_tx_errs
//portnum
//var link_mibs = new Array('100','12','2','2','2','2','2','2','2','2','2','2',
//'100','12','2','2','2','2','2','2','2','2','2','2',
//'100','12','2','2','2','2','2','2','2','2','2','2','3');
//'100','12','2','2','2','2','2','2','2','2','2','2',3);
var link_mibs = new Array(<% CGI_GET_IF_MIB(); %>);

var listnum = 12;
//var portnum = 3;
//var portname =new Array('GE1','GE2','CAB1','CAB2');
var portnum = new Array(<% CGI_GET_PORT_LOGICNUM(); %>);
var portname = new Array(<% CGI_GET_PORT_NAME(); %>);


function portName(pno)
{
	return portname[(pno - 1)];
}
/* port const functions */
function port_if_cpu(sh)
{
	var re_cpu = /^cpu[0-9]{1,}$/;
//	var sh = port_config[ind].split(';')[0];
	return re_cpu.test(sh);
}

function port_if_cable(sh)
{
	var re_cab = /^cab[0-9]{1,}$/;
//	var sh = port_config[ind].split(';')[0];
	return re_cab.test(sh);
}

function port_fancy_name(sh)
{
	var re_cpu = /^cpu[0-9]{1,}$/;
	var re_eth = /^eth[0-9]{1,}$/;
	var re_cab = /^cab[0-9]{1,}$/;		
	var base = "";

	if (re_cpu.test(sh)){
		return "CPU";
	}else if (re_eth.test(sh)){
		base = "Ethernet ";
	}else if (re_cab.test(sh)){
		base = "Cable ";
	    if (env['sys.type'] == "plc"){
	        base = "PLC ";
	    }	
	}else {
		base = "Unknown ";	
	}
	var i = sh.search(/[0-9]/);
	if (i != -1){
		  i = parseInt(sh.substring(i), 10) + 1;
	}
	base += ((i != -1) ? i : "");
	return base;
}

/* end port const functions */


function port_select_options()
{
	var i, re = "";
    var port;
	for (i = 0; i < portnum; i ++){
        port = i + 1;
		re += "<option value='" + port + "'>" + portName(port) + "</option>\n";	  	
	}
	return re;
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
			return '全双工';
		else
			return '半双工';
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
    var arrayidx = 0;

    arrayidx =n - 1;
    return link_mibs[listnum*arrayidx + i];
    
    //var sts = port_mibstr(n).split(';');;
	//if (sts.length > i){
	//	return sts[i];	
	//}	
	//return '0';
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

function port_select_on_change()
{
	var f=document.forms[0];
	var p = f.port_select.value;
    
	document.getElementById("stats_rx_pkts").innerHTML =port_stats(p, 0);
	document.getElementById("stats_tx_pkts").innerHTML =port_stats(p, 1);
	document.getElementById("stats_rx_bytes").innerHTML =port_stats(p, 2);
	document.getElementById("stats_tx_bytes").innerHTML =port_stats(p, 3);		
	document.getElementById("stats_rx_bcast").innerHTML =port_stats(p, 4);
	document.getElementById("stats_tx_bcast").innerHTML =port_stats(p, 5);	
  document.getElementById("stats_rx_mcast").innerHTML =port_stats(p, 6);		
  document.getElementById("stats_tx_mcast").innerHTML =port_stats(p, 7);			
  document.getElementById("stats_rx_fc").innerHTML =port_stats(p, 8);		
  document.getElementById("stats_tx_fc").innerHTML =port_stats(p, 9);		
  document.getElementById("stats_rx_errs").innerHTML =port_stats(p, 10);
	document.getElementById("stats_tx_errs").innerHTML =port_stats(p, 11);	
}



function reset_all()
{
	f=document.forms[0];
	var p = f.port_select.value;
	if (confirm("确定要清除所有端口的统计信息？")){
		setCookie(cookie_dontShowDoCMDPage, '1');
		diffCfg("curport", "curport", p);
		diffCfg("resetport", "resetport", "all");
		subForm(f,'goform/command','IF_MIBS', 'port_statistic.asp');			
	}
}

function reset_select()
{
	f=document.forms[0];
	var p = f.port_select.value;
	if (confirm("确定要清除所选端口的统计信息？")){
		setCookie(cookie_dontShowDoCMDPage, '1');
		diffCfg("curport", "curport", p);
		diffCfg("resetport", "resetport", p);
		subForm(f,'goform/command','IF_MIBS', 'port_statistic.asp');			
	}
}

function to_upper_page()
{
	location = "port_stat.asp";
}

function reload()
{
	//var f=document.forms[0];
	//var p = f.port_select.value;
	//location = "port_stat.asp?port=" + p;
		var f=document.forms[0];
	  var p = f.port_select.value;
	  setCookie(cookie_dontShowDoCMDPage, '1');
		diffCfg("curport", "curport", p);
		diffCfg("resetport", "resetport", 0);
		subForm(f,'goform/command','IF_MIBS', 'port_statistic.asp');
}

function init()
{
    /*
	var f=document.forms[0];
	if (QueryString('port')){
		f.port_select.value = QueryString('port');
	}else {
		f.port_select.value = 'eth0';	
	}*/
	var f=document.forms[0];
    f.port_select.value = link_mibs[listnum*portnum];
    if(f.port_select.value == '0')
    {
    	f.port_select.value = 1;
    }
	  port_select_on_change();	
}
</script>
</head>

<body onload=init()>
<script>pageHead(env, "port_table.asp");</script>    
<form name=form_port_config action=goform/command method=post onsubmit="return false;">
<INPUT type=hidden name=CMD>
<INPUT type=hidden name=GO>
<INPUT type=hidden name=action>
<table width=100% border=0 align=center cellpadding=5 cellspacing=0 >
	<tr>
		<td colspan=2 class=tb_head>端口统计信息</td>
	</tr>	
	<tr>
		<td width=30% class=tb_title>端口选择</td>
		<td width=70% class=tb_content> 
			<select name=port_select onchange=port_select_on_change()>
			<script>document.write(port_select_options());</script> 
		</select>
		<input class="button" type=button value=' 端口复位 ' onclick=reset_select()> 		
		</td>
	</tr>	
	<tr>
		<td width=30% class=tb_title>接收报文总数</td>
		<td width=70% class=tb_content id=stats_rx_pkts> 
		</td>
	</tr>
	<tr>
		<td width=30% class=tb_title>发送报文总数</td>
		<td width=70% class=tb_content id=stats_tx_pkts> 
		</td>
	</tr>
	<tr>
		<td width=30% class=tb_title>接收字节总数</td>
		<td width=70% class=tb_content id=stats_rx_bytes> 
		</td>
	</tr>		
	<tr>
		<td width=30% class=tb_title>发送字节总数</td>
		<td width=70% class=tb_content id=stats_tx_bytes>  
		</td>
	</tr>			
	<tr>
		<td width=30% class=tb_title>接收广播报文</td>
		<td width=70% class=tb_content id=stats_rx_bcast>
		</td>
	</tr>
		<tr>
		<td width=30% class=tb_title>接收多播报文</td>
		<td width=70% class=tb_content id=stats_rx_mcast>
		</td>
	</tr>		
	<tr>
		<td width=30% class=tb_title>接收流控帧</td>
		<td width=70% class=tb_content id=stats_rx_fc>
		</td>
	</tr>
	<tr>
		<td width=30% class=tb_title>接收错误报文</td>
		<td width=70% class=tb_content id=stats_rx_errs>
		</td>
	</tr>
		<tr>
		<td width=30% class=tb_title>发送广播报文</td>
		<td width=70% class=tb_content id=stats_tx_bcast>
		</td>
	</tr>
		<tr>
		<td width=30% class=tb_title>发送多播报文</td>
		<td width=70% class=tb_content id=stats_tx_mcast>
		</td>
	</tr>		
	<tr>
		<td width=30% class=tb_title>发送流控帧</td>
		<td width=70% class=tb_content id=stats_tx_fc>
		</td>
	</tr>
	<tr>
		<td width=30% class=tb_title>发送错误报文</td>
		<td width=70% class=tb_content id=stats_tx_errs>
		</td>
	</tr>			
</table>
<p><input name="curport" type="hidden" value=""></p>
<p><input name="resetport" type="hidden" value=""></p>
<p>
<input class="button" type=button value='刷新' onclick=reload()> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input class="button" type=button value='返 回' onclick=to_upper_page()>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input class="button" type=button value='清除所有端口的统计信息' onclick=reset_all()> 
</p>
</form>
<script>pageTail();</script>
</body>
</html>
