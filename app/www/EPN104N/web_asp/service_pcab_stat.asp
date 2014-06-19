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
var link_stats = new Array(<% CGI_CNU_LINK_STATS("mac");%>);
var stat = (link_stats.length > 0) ? link_stats[0] : '';
if(stat.split(';').length < 11) stat = 'N/A;N/A;N/A;N/A;N/A;N/A;N/A;N/A;N/A;N/A;N/A';

function valid_tone()
{
	var vs = stat.split(';');
	if (vs.length > 11){
		if (vs[11] & 0x02) return true;
	}
	return false;
}

function valid_link()
{
	var vs = stat.split(';');
	if (vs.length > 11){
		if (vs[11] & 0x01) return true;
	}
	return false;
}



var sta_mac = QueryString('mac');

function goBack()
{
	gotoUrl('service_sta_prop.asp?mac=' + sta_mac);
}

function reload()
{
	location='service_pcab_stat.asp?mac=' + sta_mac;	
}

function init()
{
	document.getElementById('page_header').innerHTML = 'Cable Statistics - ' + sta_mac;	
}
</script>
</head>
<body onload=init()>
<script>pageHead(webVars);</script>
<form name=service_pcab_stat action=goform/command method=post>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1
	class=box_tn>
	<tr>
		<td colspan=2 class=titleCell>链路质量</td>
	</tr>
	<tr>
		<td width=35% class=bgblue3>平均 Pre-FEC 位错误率</td>
		<td width=55% class=bggrey><script>document.write(valid_link() ? stat.split(';')[0] : "N/A");</script></td>
	</tr>
	<tr>
		<td class=bgblue3>平均比特率</td>
		<td class=bggrey><script>document.write(valid_tone() ? stat.split(';')[1] : "N/A");</script></td>
	</tr>
	<tr>
		<td class=bgblue3>平均信噪比</td>
		<td class=bggrey><script>document.write(valid_tone() ? stat.split(';')[2] : "N/A");</script></td>
	</tr>
	<tr>
		<td class=bgblue3>平均发送CRC错误率</td>
		<td class=bggrey><script>document.write(valid_link() ? stat.split(';')[3] : "N/A");</script></td>
	</tr>
	<tr>
		<td class=bgblue3>平均接收CRC错误率</td>
		<td class=bggrey><script>document.write(valid_link()? stat.split(';')[4] : "N/A");</script></td>
	</tr>
	<tr>
		<td class=bgblue3>平均衰减</td>
		<td class=bggrey><script>
			if (valid_tone()){
				var v = parseInt(stat.split(';')[5], 0);
				if (v >= 60){
					document.write("≥60.00dB");
				}else {
					document.write(stat.split(';')[5]);
				}
			}else {
					document.write('N/A');
			}
			</script></td>
	</tr>
	<tr>
		<td class=bgblue3>平均输出功率</td>
		<td class=bggrey><script>document.write(valid_tone() ? stat.split(';')[6] : "N/A");</script></td>
	</tr>
	<tr>
		<td colspan=2 class=titleCell>端口统计信息</td>
	</tr>
	<tr>
		<td class=bgblue3>发送报文总数</td>
		<td class=bggrey><script>document.write(stat.split(';')[7]);</script></td>
	</tr>
	<tr>
		<td class=bgblue3>发送失败总数</td>
		<td class=bggrey><script>document.write(stat.split(';')[8]);</script></td>
	</tr>
	<tr>
		<td class=bgblue3>接收报文总数</td>
		<td class=bggrey><script>document.write(stat.split(';')[9]);</script></td>
	</tr>
	<tr>
		<td class=bgblue3>接收失败总数</td>
		<td class=bggrey><script>document.write(stat.split(';')[10]);</script></td>
	</tr>
</table>
<p>
<input type=button value='刷新' onclick="reload();">&nbsp;&nbsp;&nbsp;&nbsp;
<input type=button value='返回' onclick="goBack();">
</p>
</form>

<script>pageTail();</script>
</body>
</html>

