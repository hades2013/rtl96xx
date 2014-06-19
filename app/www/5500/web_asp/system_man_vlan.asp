<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 管理安全设置 </title>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
<script language=javascript src=script/valid.js></script>
<script language=javascript src=theme/cvnchina/main.js></script>

<script language=javascript>
var env=new Array();var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
//var webVars = new Array('5500', '1');

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
env['sys.time'] = '946685408';
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
var security = new Array( <% CGI_SYS_SECURITY(); %>); 
//var security = new Array('4','1','1','22','0', '23', '', '', '1');

var sysip = new Array('Master;00:00:62:EE:EE:EE;static','ok;192.168.0.100;255.255.255.0;0.0.0.0;0.0.0.0;0.0.0.0;0.0.0.0','stop;0.0.0.0;0.0.0.0;0.0.0.0;0.0.0.0;0.0.0.0;0.0.0.0;0;');

var vlan_list = new Array(<% CGI_GET_VLAN_CONFIG(); %>);
//var vlan_list = new Array('0;cab0,cab1,cab2,cab3,eth0,eth1;1');

var vlan_options=new Array( <% CGI_GET_VLAN_OPTIONS(); %>);
//var vlan_options = new Array('1','1','cpu0;access','eth0;access','eth1;trunk','cab0;trunk','cab1;trunk','cab2;trunk','cab3;trunk');

var vlan_ids = new Array();

var ssh_en = security[2];
var ssh_port = security[3];
var ssh_support = security[8];


function get_ip_proto()
{
	if (sysip.length > 0){
		var vs = sysip[0].split(';');
		if (vs.length > 2) return vs[2];
	}
	return 'static';
}


function get_ip(type)
{
	var item = (type == 'static') ? 1 : 2;
	if (sysip.length > item){
		var vs = sysip[item].split(';');
		if (vs.length > 1) return vs[1];		
	}
	return '0.0.0.0';
}


addCfg("MVID", "vlan_man_vid",  security[0]);
addCfg("MSSL", "web_ssl_enabled",  security[1]);
addCfg("MSSH", "sys_ssh_enabled",  security[2]);
addCfg("MSSH_PORT", "sys_ssh_port",  security[3]);
addCfg("MTEL", "sys_telnet_enabled",  security[4]);
addCfg("MTEL_PORT", "sys_telnet_port",  security[5]);
addCfg("IP", "lan_ip_addr",  get_ip(get_ip_proto()));

function has_bcmp()
{
	if (env_has(env, 'bcmp') && (security.length > 7)){
		return true;	
	}
	return false;
}

function bcmp_setid()
{
	if(security[7] == '0'){
		return "None";	
	}
	return security[7];
}


if (has_bcmp()){
	addCfg("BCMP", "sys_bcmp_en",  security[6]);	
}


function vlan_mode()
{
	if (vlan_options.length > 0){
		return vlan_options[0];	
	}	
	return '0';
}

function vlan_mvid()
{
	if (vlan_options.length > 1){
		return vlan_options[1];	
	}	
	return '1';		
}

function get_linktype(n)
{
	for (var i = 0; i < vlan_options.length; i ++){
		var vs = vlan_options[i].split(';');
		if ((vs.length > 1) && (vs[0] == n)){
				return vs[1];
		}	
	}
	return 'trunk';
}


function CheckForm()
{
    var input;
    var valid;
	var j = 0;
	
    input = document.system_man_ip.MVID.value;
    var t = /^\s*$/;
   	var re = /^[1-9][0-9]*$/;    

    if (1 == t.test (input))
    {
        alert ("VLAN ID 不能为空 ！");
        return false;
    }

    if (!re.test(input)){
    	alert("非法的VLAN ID,请输入有效数字");
    	return false;
    }
	   
    var iInput=parseInt(input, 10);
    if(iInput<1 || iInput>4094)
    {
        alert ("非法的VLAN ID,请输入值(1-4094)！");
        return false;
    }
   
	/*
    if(vlan_mode() == '0' && isAnyVlanNotExist(input, vlan_ids))
    {
 			alert("VLAN " + iInput + " 不存在,请重新输入!");
 			return;	
    }
    */
	for (var j = 0; j < vlan_ids.length; j++)
		if (vlan_ids[j]== input) break;
	if (j == vlan_ids.length){
		alert("VLAN " + input + " 不存在,请重新输入!");
		document.system_man_ip.MVID.focus();
		return false;	
	}
	
	
	var string = document.forms[0].MSSH_PORT.value;
	reg = /^([1-9][0-9]{0,4})$/;
	if(-1 == string.search(reg))
	{
		alert("端口号设置非法，请检查输入数字的长度(1-5位,以正整数开始)和范围！");
		return false;		
	}
	if((string > 65535) || (string < 1))
	{
		alert("请输入提示范围内的整数！");
		return false;		
	}
	if((string == 80) || (string == 23) || (string == 2323) ||(string == 514) ||(string == 162))
	{
		alert('端口号'+string+'已经被使用，请重新输入!');
		return false;		
	}
    return true;
}

function ssh_port_show(ssh_en)
{
	document.forms[0].MSSH_PORT.disabled = (ssh_en=='1'?false:true);
	document.forms[0].MSSH_PORT.value = ssh_en=='1'?ssh_port:'22';
}

function ssh_port_show_change()
{
	ssh_port_show(document.forms[0].MSSH.value);
}

function tel_port_show()
{
	document.forms[0].MTEL_PORT.disabled = true;
}

function update_vlan_ids()
{
	for(var i = 0; i < vlan_list.length; i ++) {
		var vs=parse_vlan(vlan_list[i].split(';')[2]);//.split(',');	
		for (var j=0; j < vs.length; j++){
			vlan_ids[parseInt(vs[j], 10)]=parseInt(vs[j], 10);
		}
	}
}

function init()
{
	var f=document.forms[0];
	if (!has_bcmp()){
		document.getElementById('tr_bcmp').style.display = "none";		
  }			 	
	cfg2Form(f);
	if(ssh_support == 1)
		ssh_port_show(ssh_en);
	tel_port_show(); 
	update_vlan_ids();
}

function apply()
{
	//document.title = "Hello";
	var nextpage = current_page();
	if(!CheckForm())return;
	var f=document.forms[0];
	/*
	if(f.MSSL.value != getCfg('MSSL')) 
		if(f.MSSL.value == '1') 
			nextpage = 'https://' + getCfg('IP') + '/index.asp';
		else
			nextpage = 'http://' + getCfg('IP') + '/index.asp';
	form2Cfg(f);
	*/
	//subForm(f, 'goform/command', 'SYS_MANSEC', nextpage);
	if(security[0] != f.MVID.value)
	{
		if(!confirm('如果要设置的管理VLAN'+f.MVID.value+'不包含当前端口,网页的连接将会中断, 确定要继续?')) 
			return false; 
	}
	
	diffCfg("MVID", "vlan_man_vid", f.MVID.value);
	diffCfg("MSSL", "web_ssl_enabled", f.MSSL.value);
	diffCfg("MSSH", "sys_ssh_enabled", f.MSSH.value);
	diffCfg("MSSH_PORT", "sys_ssh_port", f.MSSH_PORT.value);
	diffCfg("MTEL", "sys_telnet_enabled", f.MTEL.value);
	diffCfg("MTEL_PORT", "sys_telnet_port", f.MTEL_PORT.value);
	
	subForm(f, 'goform/command', 'SYS_MANSEC', 'system_man_vlan.asp');
}

</script>
</head>

<body onload=init()>
<script>pageHead(env, env['web.page']);</script>
<form name=system_man_ip action=goform/command method=post onsubmit="return false;">

		<table width=100% border=0 align=center cellpadding=5 cellspacing=0>
			<tr ><td colspan=2 class="tb_head"> 管理安全设置 </td> </tr>				
	<tr>
				<td class="tb_title">管理VLAN</td>
				<td class="tb_content"><input name=MVID size=6 maxlength=4>
				&nbsp;&nbsp;(1 - 4094)
				</td>
			</tr>
			<tr>
				<td class="tb_title">HTTPS</td>
				<td class="tb_content">
					<select name="MSSL">
						<option value = 0>禁用</option>
						<option value = 1>启用</option>
					</select>
				</td>
			</tr>
			
			<script>
				var str = ('0' == ssh_support?'<tr style="display:none">':'<tr>');
				document.write(str);
			</script>
				<td class="tb_title">SSH</td>
				<td class="tb_content">
				<select name="MSSH" onchange="ssh_port_show_change();">
				<option value = 0>禁用</option>
				<option value = 1>启用</option>
				</select>
				&nbsp;&nbsp;&nbsp;端口： <input name="MSSH_PORT" size=7 maxlength=5></input>
				(1~65535,默认为:22)
				</td>
			</tr>
	
			<tr>
				<td class="tb_title">TELNET</td>
				<td class="tb_content">
					<select name="MTEL" onchange="tel_port_show();">
						<option value = 0>禁用</option>
						<option value = 1>启用</option>
					</select>
					&nbsp;&nbsp;&nbsp;端口： <input name="MTEL_PORT" size=10 maxlength=10></input>
					&nbsp;&nbsp;<!--(1 - 65535, 默认端口: 23)-->
				</td>
			</tr>
			<!--
			<tr id="tr_bcmp">
				<td class="tb_title">BCMP</td>
				<td class="tb_content">
					<select name="BCMP">
						<option value = 0>禁用</option>
						<option value = 1>启用</option>
					</select>
					&nbsp;&nbsp;&nbsp;使用设置集： <script> document.write(bcmp_setid()); </script>
				</td>
			</tr>
			-->			
		</table>
<p>
<input class="button" type=button value="确  认" onclick=apply() >&nbsp;&nbsp;&nbsp;	
<input  class="button" type=button value="取  消" onclick=init() >
</p>
</form>
<script>pageTail();</script>
</body>
</html>