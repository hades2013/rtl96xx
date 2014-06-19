<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 系统状态 </title>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
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
env['sys.time'] = '946685326';
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
  
var status = new Array(<% CGI_PON_STATUS(); %>);
var link_mibs = new Array(<% CGI_GET_PON_MIB(); %>); 
/*
var status = new Array('30;1.2;3;2;0;ff02::6e;CTC2.1;1;0x111111;0x52010020');
var link_mibs = new Array('11;22;33;44;55;66;77;88;;1;2;3;4;5;6;7;8;9');
//link_mibs:
//'queue0;..1;..;7;;mpcpTxReport;mpcpRxGate;onuLlidNotBcst;mpcpRxDiscGate;mpcpTxRegRequest;crc8Err;notBcstBitNotOnuLlid;bcstBitPlusOnuLLid;bcstNotOnuLLid'
//*/
var status_str = new Array('Offline', 'Online', 'Down');
var listnum = 12;
var portnum = 1;

function page_refresh()
{
	location.reload();
}


function show_status_info()
{
	var f=document.forms[0];
    
	document.getElementById("is_online").innerHTML =  status_str[GetVal(status, 0, 4)];
	document.getElementById("temp").innerHTML = GetVal(status, 0, 0);
	document.getElementById("voltage").innerHTML = GetVal(status, 0, 1);
	document.getElementById("tx_power").innerHTML = GetVal(status, 0, 2);
	document.getElementById("rx_power").innerHTML = GetVal(status, 0, 3);
	document.getElementById("ctc_ver").innerHTML = GetVal(status, 0, 6);
	document.getElementById("oam_ver").innerHTML = GetVal(status, 0, 7);
	document.getElementById("oui").innerHTML = GetVal(status, 0, 8);
	document.getElementById("vendor").innerHTML = GetVal(status, 0, 9);
}

function show_statistic_info()
{
    var i = 0;
	var str = '';
	var val = 0;
	
	while((val=GetVal(link_mibs, 0, i))!='')
	{
		str +=  '<tr>' +
				'<td width=30% class=tb_title>队列 '+(i++)+'</td>' +
				'<td width=70% class=tb_content>'+val+'</td>' +
				'</tr>';		
	}
	i++;
	
	str +=  '<tr>' +
			'<td width=30% class=tb_title>MPCP TX Report</td>' +
			'<td width=70% class=tb_content>'+GetVal(link_mibs, 0, (i++))+'</td>' +
			'</tr>' +	
			'<tr>' +
			'<td width=30% class=tb_title>MPCP RX Gate</td>' +
			'<td width=70% class=tb_content>'+GetVal(link_mibs, 0, (i++))+'</td>' +
			'</tr>' +	
			'<tr>' +
			'<td width=30% class=tb_title>ONU Llid Not Broadcast</td>' +
			'<td width=70% class=tb_content>'+GetVal(link_mibs, 0, (i++))+'</td>' +
			'</tr>' +		
			'<tr>' +
			'<td width=30% class=tb_title>MPCP RX Discovery Gate</td>' +
			'<td width=70% class=tb_content>'+GetVal(link_mibs, 0, (i++))+'</td>' +
			'</tr>' +	
			'<tr>' +
			'<td width=30% class=tb_title>MPCP TX Reg Request</td>' +
			'<td width=70% class=tb_content>'+GetVal(link_mibs, 0, (i++))+'</td>' +
			'</tr>' +
			'<tr>' +
			'<td width=30% class=tb_title>CRC8 Errors</td>' +
			'<td width=70% class=tb_content>'+GetVal(link_mibs, 0, (i++))+'</td>' +
			'</tr>' +
			'<tr>' +
			'<td width=30% class=tb_title>Not Broadcast Bit<br>Not ONU Llid</td>' +
			'<td width=70% class=tb_content>'+GetVal(link_mibs, 0, (i++))+'</td>' +
			'</tr>' +	
			'<tr>' +
			'<td width=30% class=tb_title>Broadcast BIT<br>Plus ONU Llid</td>' +
			'<td width=70% class=tb_content>'+GetVal(link_mibs, 0, (i++))+'</td>' +
			'</tr>' +						
			'<tr>' +
			'<td width=30% class=tb_title>Broadcast Bit<br>Not ONU Llid</td>' +
			'<td width=70% class=tb_content>'+GetVal(link_mibs, 0, (i++))+'</td>' +
			'</tr>';					
	document.write(str);
}

function Init()
{
	var f = document.forms[0];
	show_status_info();
}

function reset_pon_statistic()
{
	f=document.forms[0];

	if (confirm("确定要清除所选端口的统计信息？"))
	{
		subForm(f,'goform/command','PON_COUNT_RESET', 'pon_stat.asp');			
	}
}
</script>
</head>
<body>
<script>pageHead(env, env['web.page']);</script>
<form name=system_status action=goform/command method=post onsubmit="return false;">
<table width=100% cellpadding=5 cellspacing=0>
		<tr ><td colspan=2 class="tb_head">PON口状态</td> </tr>			
	<tr>
		<td class=tb_title>ONU在线状态</td>
		<td class=tb_content id=is_online></td>
	</tr>	
		<tr ><td colspan=2 class="tb_head">光功率参数</td> </tr>		
	<tr>
		<td class=tb_title>工作温度(℃)</td>
		<td class=tb_content id=temp></td>
	</tr>
	<tr>
		<td class=tb_title>供电电压(V)</td>
		<td class=tb_content id=voltage></td>
	</tr>	
	<tr>
		<td class=tb_title>发送光功率(dBm)</td>
		<td class=tb_content id=tx_power></td>
	</tr>	
	<tr>
		<td class=tb_title>接收光功率(dBm)</td>
		<td class=tb_content id=rx_power></td>
	</tr>		
	<tr ><td colspan=2 class="tb_head">OTL相关信息</td> </tr>		
	<tr>
		<td class=tb_title>CTC版本</td>
		<td class=tb_content id=ctc_ver></td>
	</tr>	
	<tr>
		<td class=tb_title>OAM 版本</td>
		<td class=tb_content id=oam_ver></td>
	</tr>	
	<tr>
		<td class=tb_title>OUI</td>
		<td class=tb_content id=oui></td>
	</tr>	
	<tr>
		<td class=tb_title>制造商信息</td>
		<td class=tb_content id=vendor></td>
	</tr>	
	
	<tr ><td colspan=2 class="tb_head">PON口统计信息</td> </tr>	
		<tr>
		<td width=30% class=tb_title>端口选择</td>
		<td width=70% class=tb_content> 
		PON口
		</td>
	</tr>
	<script>show_statistic_info();</script>
</table>
<!--
<p>
<input class="button" type=button value=' 刷新 '  onclick="page_refresh();">
</p>-->
<p><input name="curport" type="hidden" value=""></p>
<p><input name="resetport" type="hidden" value=""></p>
<p>
<input class="button" type=button value='刷新' onclick='page_refresh()'> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<!--
<input class="button" type=button value='返 回' onclick=to_upper_page()>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
-->
<input class="button" type=button value='清除PON口的统计信息' onclick='reset_pon_statistic();'> 
</p>
</form>
<script> pageTail(); Init();</script>
</body>
</html>