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

/*
eth0; phy ; enabled ; autoneg ; speed
; duplex;  flowctrl;   pri;   ing;   eg;   infilter; pvid
*/
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

var port_config = new Array(<% CGI_GET_ETH_CONFIG(); %>);

function port_cfgstr(n)
{
	 for (var i = 0; i < port_config.length; i ++){
	 	 if (port_config[i].split(';')[0] == n){
	 	 		return port_config[i];
	 	 	}
	 	}
	 return "";
}


function port_spddup(n)
{
	var v = port_cfgstr(n);
	var au, sp, dup;
	au = v.split(';')[3];
	sp = v.split(';')[4];
	dup = v.split(';')[5];
	if (au == '1'){
		return 'auto';	
	}else {
		if (sp == '1000'){
			return '1000full';	
		}
		return sp + dup;
	}
}

function port_pri(n)
{
	var v = port_cfgstr(n);
	return v.split(';')[7]; /*off, 0 - 7*/	
}

function port_flow(n)
{
	var v = port_cfgstr(n);
	return v.split(';')[6]; 
}

function port_egress_limit(n)
{
	var v = port_cfgstr(n);
	return v.split(';')[9]; 
}

function port_ingress_limit(n)
{
	var v = port_cfgstr(n);
	return v.split(';')[8]; 
}

function init()
{
	var f=document.forms[0];
	f.SPDDUP_ETH0.value = port_spddup('eth0');
	f.PRI_ETH0.value = port_pri('eth0');
	f.FLOW_ETH0.value = port_flow('eth0');
	f.IN_RL_ETH0.value = port_ingress_limit('eth0');
	f.EN_RL_ETH0.value = port_egress_limit('eth0');
	
	f.SPDDUP_ETH1.value = port_spddup('eth1');
	f.PRI_ETH1.value = port_pri('eth1');
	f.FLOW_ETH1.value = port_flow('eth1');
	f.IN_RL_ETH1.value = port_ingress_limit('eth1');
	f.EN_RL_ETH1.value = port_egress_limit('eth1');	
}

function parse_duplex(s)
{
	if (s == '1000full'){
		return 'full';	
	}else 	if (s == '100full'){
		return 'full';	
	}else 	if (s == '100half'){
		return 'half';	
	}else 	if (s == '10full'){
		return 'full';	
	}else 	if (s == '10half'){
		return 'half';	
	}
	return 'full';
}		
					
function parse_speed(s)
{
	if (s == '1000full'){
		return '1000';	
	}else 	if (s == '100full'){
		return '100';	
	}else 	if (s == '100half'){
		return '100';	
	}else 	if (s == '10full'){
		return '10';	
	}else 	if (s == '10half'){
		return '10';	
	}
	return '100';
}		

function get_port_config(n)
{
	var f=document.forms[0];
	var s, au, sp, dup, flow, pri, ingr, egr;
	
	if (n == "eth1"){
			au = (f.SPDDUP_ETH1.value == 'auto') ? 1 : 0;
			if (au){
				sp =  port_cfgstr(n).split(';')[4];
				dup = port_cfgstr(n).split(';')[5];
			}else {
				sp = 	parse_speed(f.SPDDUP_ETH1.value);
				dup = parse_duplex(f.SPDDUP_ETH1.value);				
			}
			flow = f.FLOW_ETH1.value;
			pri = f.PRI_ETH1.value;
			ingr = f.IN_RL_ETH1.value; 
			egr = f.EN_RL_ETH1.value;						
	}else {
			au = (f.SPDDUP_ETH0.value == 'auto') ? 1 : 0;
			if (au){
				sp =  port_cfgstr(n).split(';')[4];
				dup = port_cfgstr(n).split(';')[5];
			}else {
				sp = 	parse_speed(f.SPDDUP_ETH0.value);
				dup = parse_duplex(f.SPDDUP_ETH0.value);				
			}
			flow = f.FLOW_ETH0.value;
			pri = f.PRI_ETH0.value;
			ingr = f.IN_RL_ETH0.value; 
			egr = f.EN_RL_ETH0.value;			
	}

	s = n + ';' + port_cfgstr(n).split(';')[1] + ';'
	+ port_cfgstr(n).split(';')[2] + ';'
	+ au + ';'+ sp + ';'+ dup + ';'
	+ flow + ';' + pri + ';'
	+ ingr + ';' + egr + ';'	
	+ port_cfgstr(n).split(';')[10] + ';'
	+ port_cfgstr(n).split(';')[11];
			
	return s;	
}


function apply()
{
	var i = 0;
	var f=document.forms[0];	
	var cfg_eth0 = get_port_config('eth0');
	var cfg_eth1 = get_port_config('eth1');
	
	if (cfg_eth0 != port_cfgstr('eth0')){
			diffCfg('eth_' + i, 'eth_cfg', cfg_eth0);
			i++;
	}
	if (cfg_eth1 != port_cfgstr('eth1')){
			diffCfg('eth_' + i, 'eth_cfg', cfg_eth1);
	}
		
	subForm(f,'goform/command','SYS_ETH', "port_prop.asp");
}

function rate_options()
{
	var str= ' <option value=0>Disabled</option>'
		str += '<option value=13172>128 Kbps</option>'
		str += '<option value=262144>256 Kbps</option>'
		str += '<option value=524288>512 Kbps</option>'
		str += '<option value=1048576>1 Mbps</option>'
		str += '<option value=2097152>2 Mbps</option>'
		str += '<option value=4194304>4 Mbps</option>'
		str += '<option value=8388608>8 Mbps</option>'
		return str;
}

</script>
</head>

<body onload=init()>
<script>pageHead(webVars,PROD_MODEL);</script>
<form name=system_man_ip action=goform/command method=post>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1
	class=box_tn>
	<tr>
		<td class=bggrey2 align=center><font size=4>Ethernet1</font></td>
	</tr>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1
			bgcolor="#FFFFFF">
			<tr>
				<td class=bgblue>速率/双工模式</td>
				<td class=bggrey>
					<select name=SPDDUP_ETH0>
					<option value="auto">Auto</option>
					<option value="1000full">1000Mbps Full Duplex</option>
					<option value="100full">100Mbps Full Duplex</option>
					<option value="100half">100Mbps Half Duplex</option>
					<option value="10full">10Mbps Full Duplex</option>
					<option value="10half">10Mbps Half Duplex</option>
					</select>
				</td>
			</tr>
			<tr>
				<td class=bgblue>优先级</td>
				<td class=bggrey>
					<select name=PRI_ETH0>
					<option value="off">Disabled</option>
					<option value=0>0</option>
					<option value=1>1</option>
					<option value=2>2</option>
					<option value=3>3</option>
					<option value=4>4</option>
					<option value=5>5</option>
					<option value=6>6</option>
					<option value=7>7</option>					
					</select>
				</td>
			</tr>
			<tr>
				<td class=bgblue>流控</td>
				<td class=bggrey>
					<select name=FLOW_ETH0>
					<option value='0'>Disabled</option>
					<option value='1'>Enabled</option>
					</select>
				</td>
			</tr>
			<tr>
				<td class=bgblue>带宽限制</td>
				<td class=bggrey>
					Ingress
					<select name=IN_RL_ETH0>
					<script>document.write(rate_options())</script>
					</select>&nbsp;&nbsp;
					Egress
					<select name=EN_RL_ETH0>
					<script>document.write(rate_options())</script>
					</select>
				</td>
			</tr>
		</table>
		</td>
	</tr>
	<tr>
		<td class=bggrey2 align=center><font size=4>Ethernet2</font></td>
	</tr>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1
			bgcolor="#FFFFFF">
			<tr>
				<td class=bgblue>速率/双工模式</td>
				<td class=bggrey>
					<select name=SPDDUP_ETH1>
					<option value="auto">Auto</option>
					<option value="1000full">1000Mbps Full Duplex</option>
					<option value="100full">100Mbps Full Duplex</option>
					<option value="100half">100Mbps Half Duplex</option>
					<option value="10full">10Mbps Full Duplex</option>
					<option value="10half">10Mbps Half Duplex</option>
					</select>
				</td>
			</tr>
			<tr>
				<td class=bgblue>优先级</td>
				<td class=bggrey>
					<select name=PRI_ETH1>
					<option value="off">Disabled</option>
					<option value=0>0</option>
					<option value=1>1</option>
					<option value=2>2</option>
					<option value=3>3</option>
					<option value=4>4</option>
					<option value=5>5</option>
					<option value=6>6</option>
					<option value=7>7</option>							
					</select>
				</td>
			</tr>
			<tr>
				<td class=bgblue>流控</td>
				<td class=bggrey>
					<select name=FLOW_ETH1>
					<option value=0>Disabled</option>
					<option value=1>Enabled</option>
					</select>
				</td>
			</tr>
			<tr>
				<td class=bgblue>带宽限制</td>
				<td class=bggrey>
					Ingress
					<select name=IN_RL_ETH1>
					<script>document.write(rate_options())</script>
					</select>&nbsp;&nbsp;
					Egress
					<select name=EN_RL_ETH1>
					<script>document.write(rate_options())</script>
					</select>
				</td>
			</tr>
		</table>
		</td>
	</tr>
</table>
<p>
<input type=button value='  确定  ' onclick=apply() >
<input type=button value='  取消  ' onclick=init() >
</p>
</form>

<script>pageTail(str_copyright);</script>
</body>
</html>
