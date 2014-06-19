<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="content-type" content="text/html;charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=javascript src=pageview.js></script>
<SCRIPT language=javascript src=valid.js></SCRIPT>
<script language=JavaScript>
pageTitle();
/*
Einsn do some changes:
 when in edit mode, only changes will be submitted 
 but now ,we should submit all even if some of them have not any changes
*/

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

var supdev = new Array(<% CGI_GET_DEVINFO(); %>);
var tmpl_list = new Array(<% CGI_GET_TEMPLATES("brief"); %>);
<% CGI_GET_TEMPLATES("tmpl_id", "tmpl_id"); %>

var action;
var tmpl_id = 0;



function valid_cab_rate(r)
{
  var re = /^\d*$/;
	if (!re.test(r)){
			return false;	
	}
  r = parseInt(r);  	
 
	if (r > 16384) return false;
	if ((r % 64) != 0) return false;
	return true;
}


function valid_vlanid(v)
{
  var re = /^\d*$/;
	if (!re.test(v)){
			return false;	
	}
  v = parseInt(v);  	

	if (v < 1 || v > 4094 ){
			return false;
	}	
	return true;
}


function valid_maclimit(v)
{
  var re = /^\d*$/;
	if (!re.test(v)){
			return false;	
	}
  v = parseInt(v);  	

	if (v < 0 || v > 255 ){
			return false;
	}	
	return true;	
}


function ether_config(ports, devname)
{
	var str = '';
	var i, j;
	var pl = 0;
	var single_port = (ports <= 2);
	if(devname == 'CB203' || devname == 'STB103') pl = 1;
	str = '<table width=100% border=0 align=center cellpadding=3 cellspacing=1 class=box_tn>';
	str += '<tr>';
	str += '<td width=10% class=titleCell>端口</td>';
	str += '<td width=10% class=titleCell>速率</td>';
	str += '<td width=10% class=titleCell>双工模式</td>';
	str += '<td width=12% class=titleCell>优先级</td>';
	str += '<td width=15% class=titleCell>流控</td>';
	str += '<td width=15% class=titleCell>入口限速</td>';
	str += '<td width=15% class=titleCell>出口限速</td>';
	str += '<td width=13% class=titleCell>端口状态</td>';
	str += '</tr>';
	for(i = 1; i < ports; i++) {
		str += '<tr>';
		if(i == 0)
			str += '<td width=10% class=bggrey>Cable</td>';
		else {
			var x = i - 1 + pl;
			str += '<td width=10% class=bggrey>Ethernet' + x + '</td>';
		}
		str += '<td width=10% class=bggrey><select name="SPD' + i + '"' + (single_port ? ' disabled=true>' : '>') + 
			'<option value=auto>自动</option>' + 
			'<option value=100>100M</option>' +
			'<option value=10>10M</option></select></td>';
		str += '<td width=10% class=bggrey><select name="DUP' + i + '"' + (single_port ? ' disabled=true>' : '>') + 
			'<option value=auto>自动</option>' +
			'<option value=full>全双工</option>' +
			'<option value=half>半双工</option></select></td>';
		str += '<td width=12% class=bggrey><select name="PRI' + i + '"' + (single_port ? ' disabled=true>' : '>');
		for(j = 0; j < 8; j ++)
			str += '<option value=' + j + '>' + j + '</option>';
		str += '</option></td>';
		str += '<td width=15% class=bggrey><select name="FLOW' + i + '"' + (single_port ? ' disabled=true>' : '>') + 
			'<option value=on>启用</option>' +
			'<option value=off>禁用</option></select></td>';
		str += '<td width=15% class=bggrey><select name="INLIM' + i + '"' + (single_port ? ' disabled=true>' : '>') + 
			'<option value=0>禁用</option>';
		var lim = Array('128 Kbps', '256 Kbps', '512 Kbps', '1 Mbps', 
				'2 Mbps', '4 Mbps', '8 Mbps');
		for(j = 1; j < 8; j ++) 
			str += '<option value=' + j + '>' + lim[j - 1] + '</option>';
		str += '</select></td>';
		str += '<td width=15% class=bggrey><select name="ENLIM' + i + '"' + (single_port ? ' disabled=true>' : '>') + 
			'<option value=0>禁用</option>';
		for(j = 1; j < 8; j ++) 
			str += '<option value=' + j + '>' + lim[j - 1] + '</option>';
		str += '</select></td>';
		str += '<td width=13% class=bggrey><select name="PORTEN' + i + '"' + (single_port ? ' disabled=true>' : '>') + 
			'<option value=ena>启用</option>' +
			'<option value=dis>禁用</option></select></td>';
		str += '</tr>';
	}
	str += '</table>';
	
	return str;
}

function ether_vlan_config(ports, devname)
{
	var str = '';
	var pl = 0;
	var single_port = (ports <= 2);
	if(devname == 'CB203' || devname == 'STB103') pl = 1;
	str = '<table width=100% border=0 align=center cellpadding=3 cellspacing=1 class=box_tn>';
	str += '<tr>';
	str += '<td width=30% class=titleCell>端口</td>';
	str += '<td width=30% class=titleCell>VLAN 模式</td>';
	str += '<td width=30% class=titleCell>PVID</td></tr>';
	for(i = 0; i < ports; i++) {
		str += '<tr>';
		if(i == 0)
			str += '<td width=30% class=bggrey>Cable</td>';
		else {
			var x = i - 1 + pl;
			str += '<td width=30% class=bggrey>Ethernet' + x + '</td>';
		}
		str += '<td width=30% class=bggrey><select name="TAG' + i + '"' + (single_port ? ' disabled=true>' : '>') + 
			'<option value=off>Untag</option>' +
			'<option value=on>Tag</option></select></td>';
		str += '<td width=10% class=bggrey>' +
			'<input name="PVID' + i + '" size=10 maxlength=10 ' + ((single_port && i == 0) ? ' disabled=true' : '') + '></td>';
		str += '</tr>';
	}
	str += '</table>';	
	return str;
}

function ether_update_value(ports, tmpl)
{
	var f = document.forms[0];
	var i, s;
	for(i = 0; i < ports; i ++) {
		s = tmpl[i].split(';');
		if(i > 0) {
			document.getElementsByName('SPD' + i)[0].value = s[1];
			document.getElementsByName('DUP' + i)[0].value = s[2];
			document.getElementsByName('PRI' + i)[0].value = s[3];
			document.getElementsByName('FLOW' + i)[0].value = s[4];
			document.getElementsByName('INLIM' + i)[0].value = s[5];
			document.getElementsByName('ENLIM' + i)[0].value = s[6];
			document.getElementsByName('PORTEN' + i)[0].value = s[9];
		}
		document.getElementsByName('TAG' + i)[0].value = s[7];
		document.getElementsByName('PVID' + i)[0].value = s[8];
	}
}

function change_device()
{
	update_show(document.forms[0].DEV.value);
 // add by Einsn	
	var f = document.forms[0];
	var sd = tmpl_list[document.forms[0].DEV.value].split(';');
	f.TMP_NAME.value = sd[2];	
	f.TMP_DESC.value = sd[3];	
}

function update_show(index)
{
	var s = supdev[index].split(';'); 
	document.getElementById('template_ports').innerHTML = ether_config(s[1], s[0]);
	document.getElementById('template_vlan').innerHTML = ether_vlan_config(s[1], s[0]);
	ether_update_value(s[1], template_port);
	
}

function apply()
{
	var f = document.forms[0];
	var nPorts;
	var return_page;

	if(!valid_xname(f.TMP_NAME.value)) {
		alert('Invalid name for template, please input again.');
		return;
	}		

	if(!valid_description(f.TMP_DESC.value)) {
		alert('Invalid description for template, please input again.');
		return;
	}
	
	if (!valid_cab_rate(f.CAB_UPS.value) || !valid_cab_rate(f.CAB_DNS.value)){
		alert('Invalid Rate limitation for Cable port, please input again.');
		return;		
	}
	
	if (!valid_maclimit(f.MAC_LIM.value))
	{
		alert('Invalid number for MAC limitation, please input again.');
		return;
	}	
	
	{
		var dev_id;
		if(action == 'new') {
			dev_id = f.DEV.value;
			for(var i = 0; i < tmpl_list.length; i ++) {
				if(tmpl_list[i].split(';')[2] == f.TMP_NAME.value) {
					alert('Template name "' + f.TMP_NAME.value + '" exists，please input again!');
					f.TMP_NAME.focus();
					return;
				}
			}
			
			return_page = 'service_templates.asp';
		}
		else {
			dev_id = tmpl_list[tmpl_id].split(';')[1];
			return_page = 'service_template_prop.asp?action=edit&tmpl_id=' + tmpl_id;
		}
		var tmpl_desc = 
			tmpl_id + ';' +
			dev_id + ';' +
			f.TMP_NAME.value + ';' +
			f.TMP_DESC.value ;
		diffCfg('tmpl_' + tmpl_id, 'tmpl_' + tmpl_id,  tmpl_desc);
		if(action == 'new') {
			nPorts = supdev[f.DEV.value].split(';')[1];
		}
		else {
			nPorts = supdev[tmpl_list[tmpl_id].split(';')[1]].split(';')[1];
		}
		var tmpl_ports = Array();
		tmpl_ports.length = nPorts;
		for(var i = 0; i < nPorts; i ++) {
			var pvid = document.getElementsByName('PVID' + i)[0].value; 
			if (!valid_vlanid(pvid)){
				alert('Invalid PVID "' + pvid +'", please input again.');
				return;								
			}
			
			tmpl_ports[i] = 
				i + ';';
			if(i > 0) { 
				tmpl_ports[i] +=
				document.getElementsByName('SPD' + i)[0].value + ';' +
				document.getElementsByName('DUP' + i)[0].value + ';' +
				document.getElementsByName('PRI' + i)[0].value + ';' +		
				document.getElementsByName('FLOW' + i)[0].value + ';' +		
				document.getElementsByName('INLIM' + i)[0].value + ';' +		
				document.getElementsByName('ENLIM' + i)[0].value + ';';
			}
			else {
				tmpl_ports[i] += '100;full;0;off;0;0;';
			}
			tmpl_ports[i] += 
				document.getElementsByName('TAG' + i)[0].value + ';' +
				document.getElementsByName('PVID' + i)[0].value + ';';
			if(i > 0)
				tmpl_ports[i] += document.getElementsByName('PORTEN' + i)[0].value;
			else
				tmpl_ports[i] += 'ena';
			diffCfg('tmpl_' + tmpl_id + '_' + i, 'tmpl_' + tmpl_id + '_' + i, tmpl_ports[i]);
		}
		var tmpl_misc = f.CAB_UPS.value + ';' + 
			f.CAB_DNS.value + ';' + 
			f.BROS.value + ';' + f.MAC_LIM.value;
		diffCfg('tmpl_' + tmpl_id + '_misc', 'tmpl_' + tmpl_id + '_misc', tmpl_misc);
		subForm(f,'goform/command', (action == 'new') ? 'TMPL_NEW' : 'TMPL_PROP', return_page);
	}
}

function dev_string(index)
{
	var str = supdev[index].split(';')[0];
	return str;
}

function init()
{	
	var f = document.forms[0];
	var str = '';
	var sdesc = '';
	
	action = QueryString('action');

	if(action == 'new') {
		tmpl_id = 0;
		addCfg('tmpl_id', 'tmpl_id', tmpl_id);				
		addCfg('tmpl_' + tmpl_id, 'tmpl_' + tmpl_id, '');
		for(var i = 0; i < template_port.length; i ++) {
			addCfg('tmpl_' + tmpl_id + '_' + i, 'tmpl_' + tmpl_id + '_' + i, '');
		}
		addCfg('tmpl_' + tmpl_id + '_misc', 'tmpl_' + tmpl_id + '_misc', '');	

		sdesc = tmpl_list[0].split(';');
		str = '<select name="DEV" onchange="change_device();">';
		for(var i = 0; i < supdev.length; i ++)
			str += '<option value=' + i + '>' + dev_string(i) + '</option>';
		str += '</select>';

		document.title = '新建模板';
		document.getElementById('page_descript').innerHTML ='创建新模板';
		document.getElementById('page_header').innerHTML ='新建模板';
		document.getElementById('dev_model').innerHTML = str;

		f.DEV.value = sdesc[1];
		f.TMP_NAME.value = sdesc[2];
		f.TMP_DESC.value = sdesc[3];
		f.CAB_UPS.value = template_misc.split(';')[0];
		f.CAB_DNS.value = template_misc.split(';')[1];
		f.BROS.value = template_misc.split(';')[2];
		f.BROS.disabled = supdev[sdesc[1]].split(';')[1] <= 2;
		f.MAC_LIM.value = template_misc.split(';')[3];
		update_show(sdesc[1]);
		ether_update_value(supdev[sdesc[1]].split(';')[1], template_port);
	} 
	else {
		tmpl_id = QueryString('tmpl_id');
		addCfg('tmpl_id', 'tmpl_id', tmpl_id);				
		addCfg('tmpl_' + tmpl_id, 'tmpl_' + tmpl_id, tmpl_list[tmpl_id]);
		for(var i = 0; i < template_port.length; i ++) {
			addCfg('tmpl_' + tmpl_id + '_' + i, 'tmpl_' + tmpl_id + '_' + i, template_port[i]);
		}
		addCfg('tmpl_' + tmpl_id + '_misc', 'tmpl_' + tmpl_id + '_misc', template_misc);	

		tmpl_id = QueryString('tmpl_id');
		sdesc = tmpl_list[tmpl_id].split(';');
		str += '<input name="DEV" readonly=true size=19 maxlength=19>';

		document.title = '模板修改';
		document.getElementById('page_descript').innerHTML ='修改模板';
		document.getElementById('page_header').innerHTML ='编辑模板';
		document.getElementById('dev_model').innerHTML = str;

		f.DEV.value = dev_string(sdesc[1]);
		f.DEV.readOnly = true;
		f.TMP_NAME.value = sdesc[2];
		f.TMP_DESC.value = sdesc[3];
		f.CAB_UPS.value = template_misc.split(';')[0];
		f.CAB_DNS.value = template_misc.split(';')[1];
		f.BROS.value = template_misc.split(';')[2];
		f.BROS.disabled = supdev[sdesc[1]].split(';')[1] <= 2;
		if(template_misc.split(';').length > 3)
			f.MAC_LIM.value = template_misc.split(';')[3];
		else
			f.MAC_LIM.value = '0';
		update_show(sdesc[1]);
		ether_update_value(supdev[sdesc[1]].split(';')[1], template_port);
	} 
}

</script>
</head>
<body onload=init()>
<script>pageHead(webVars);</script>

<form name=system_status action=goform/command method=post>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1
	class=box_tn>
	<tr>
		<td colspan=2 class=titleCell>模板信息</td>
	</tr>
	<tr>
		<td width=25% class=bgblue>设备型号</td>
		<td width=75% class=bggrey><div id=dev_model></div></td>
	</tr>
	<tr>
		<td width=25% class=bgblue>模板名称</td>
		<td width=75% class=bggrey><input name="TMP_NAME" size=31 maxlength=31></td>
	</tr>
	<tr>
		<td width=25% class=bgblue>模板描述</td>
		<td width=75% class=bggrey><input name="TMP_DESC" size=50 maxlength=63></td>
	</tr>
</table>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1
	class=box_tn>
	<tr>
		<td colspan=2 class=titleCell>Cable 速率限制</td>
	</tr>
	<tr>
		<td width=25% class=bgblue>上行速率</td>
		<td width=75% class=bggrey><input name="CAB_UPS" size=10 maxlength=10>&nbsp;Kbps &nbsp;(0~16384 multiple of 64,0 means disabled )</td>
	</tr>
	<tr>
		<td width=25% class=bgblue>下行速率</td>
		<td width=75% class=bggrey><input name="CAB_DNS" size=10 maxlength=10>&nbsp;Kbps &nbsp;(0~16384 multiple of 64,0 means disabled)</td>
	</tr>
</table>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1
	class=box_tn>
	<tr>
		<td colspan=2 class=titleCell>广播风暴抑制</td>
	</tr>
	<tr>
		<td width=25% class=bgblue>广播风暴抑制</td>
		<td width=75% class=bggrey><select name="BROS">
		<option value=off>禁止</option>
		<option value=on>使能</option>
		</select>
		</td>
	</tr>
</table>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1
	class=box_tn>
	<tr>
		<td colspan=2 class=titleCell>MAC数量限制</td>
	</tr>
	<tr>
		<td width=25% class=bgblue>MAC数量限制</td>
		<td width=75% class=bggrey>
			<input name="MAC_LIM" size=10 maxlength=10></input>
			The number of devices which are allow to access the services. (0-255), 0 means disabled
		</td>
	</tr>
</table>


<table width=100% border=0 align=center cellpadding=0 cellspacing=1
	class=box_tn>
	<tr>
		<td colspan=2 class=titleCell>&nbsp;&nbsp;Ports and VLAN</td>
	</tr>
	<tr><td><div id="template_ports"></div></td></tr>
	<tr><td><div id="template_vlan"></div></td></tr>
</table>
<INPUT type=hidden name=CMD value=TMPL_PROP>
<INPUT type=hidden name=GO value=service_template.asp>
<br></br>
<table width=100% border=0 align=center cellpadding=0 cellspacing=1>
	<tr><td>
	<input id="btn_wht" type=button value="确定" onclick="apply();">&nbsp;&nbsp;&nbsp;&nbsp;
	<input id="btn_blk" type=button value="取消" onclick="init();">
	</td></tr>
</table>
</form>

<script>pageTail();</script>
</body>
</html>
