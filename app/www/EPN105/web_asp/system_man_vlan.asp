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

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

var sysip = new Array(<% CGI_SYS_IP(); %>);
var security = new Array(<% CGI_SYS_SECURITY(); %>);

var vlan_list = new Array(<% CGI_GET_VLAN_CONFIG(); %>);
var vlan_options = new Array(<% CGI_GET_VLAN_MODE_CONFIG(); %>);

var vlan_ids = new Array();

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
addCfg("IP", "vlan_interface_1_ip",  get_ip(get_ip_proto()));


pageTitle();

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
    input = document.system_man_ip.MVID.value;
    var re = /^\s+|\s+$/g;
    var t = /^\s*$/;

    if (1 == t.test (input))
    {
        alert ("VLAN ID 不能为空！");
        return false;
    }
    input = input.replace (re, "");
    if(!isNumber(input))
    {
    	alert("非法的VLAN ID,请输入有效数字");
    	return false;
    }
    var iInput=parseInt(input);
    if(iInput<1 || iInput>4094)
    {
        alert ("非法的VLAN ID,请输入值(1-4094)！");
        return false;
    }
    
     if(vlan_mode() == '0' && isAnyVlanNotExist(input, vlan_ids))
     {
 			alert("VLAN " + iInput + " 不存在,请重新输入!");
 			return;	
    }
    
    if (getCfg('MVID') != iInput){
	    if(get_linktype('eth0') == 'access') { 
	        if(!confirm('端口 ' + port_name('eth0') + ' 的PVID将变成 ' + iInput + ' , 确认?'))
	            return;
	    }
	    if(get_linktype('eth1') == 'access') { 
	        if(!confirm('端口 ' + port_name('eth1') + ' 的PVID将变成 ' + iInput + ' , 确认?'))
	            return;
	    }
	    if(get_linktype('cab0') == 'access') { 
	        if(!confirm('端口 ' + port_name('eth0') + ' 的PVID将变成 ' + iInput + ' , 确认?'))
	            return;
	    }
	    if(get_linktype('cab1') == 'access') { 
	        if(!confirm('端口 ' + port_name('eth1') + ' 的PVID将变成 ' + iInput + ' , 确认?'))
	            return;
	    }     	
    }
    return true;
}

function ssh_port_show()
{
	document.forms[0].MSSH_PORT.disabled = true;
}

function tel_port_show()
{
	document.forms[0].MTEL_PORT.disabled = true;
}

function update_vlan_ids()
{
	vlan_ids = new Array();	
	for(var i = 0; i < vlan_list.length; i ++) {
		var vs=parse_vlan(vlan_list[i].split(';')[2]);//.split(',');	
		for (var j=0; j < vs.length; j++){
			vlan_ids[parseInt(vs[j])]=parseInt(vs[j]);
		}
	}
}

function init()
{
	var f=document.forms[0];
	 	
	cfg2Form(f);
	ssh_port_show();
	tel_port_show();
	update_vlan_ids();
}

function apply()
{
	//document.title = "Hello";
	var nextpage = cPage;
	if(!CheckForm())return;
	var f=document.forms[0];
	if(f.MSSL.value != getCfg('MSSL')) 
		if(f.MSSL.value == '1') 
			nextpage = 'https://' + getCfg('IP') + '/index.asp';
		else
			nextpage = 'http://' + getCfg('IP') + '/index.asp';
	form2Cfg(f);
	subForm(f, 'goform/command', 'SYS_MANSEC', nextpage);
}

</script>
</head>

<body onload=init()>
<script>pageHead(webVars,PROD_MODEL);</script>
<form name=system_man_ip action=goform/command method=post>

<table width=100% border=0 align=center cellpadding=0 cellspacing=0
	class=box_tn>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1
			bgcolor="#FFFFFF">
			<tr>
				<td class=bgblue>管理VLAN</td>
				<td class=bggrey><input name=MVID size=10 maxlength=5>
				&nbsp;&nbsp;(1 - 4094)
				</td>
			</tr>
			<tr>
				<td class=bgblue>HTTPS</td>
				<td class=bggrey>
					<select name="MSSL">
						<option value = 0>禁止</option>
						<option value = 1>使能</option>
					</select>
				</td>
			</tr>
			<tr>
				<td class=bgblue>SSH</td>
				<td class=bggrey>
					<select name="MSSH" onchange="ssh_port_show();">
						<option value = 0>禁止</option>
						<option value = 1>使能</option>
					</select>
					&nbsp;&nbsp;&nbsp;端口: <input name="MSSH_PORT" size=10 maxlength=10></input>
					&nbsp;&nbsp;<!--(1 - 65535, 默认端口: 22)-->
				</td>
			</tr>
			<tr>
				<td class=bgblue>TELNET</td>
				<td class=bggrey>
					<select name="MTEL" onchange="tel_port_show();">
						<option value = 0>禁止</option>
						<option value = 1>使能</option>
					</select>
					&nbsp;&nbsp;&nbsp;端口: <input name="MTEL_PORT" size=10 maxlength=10></input>
					&nbsp;&nbsp;<!--(1 - 65535, 默认端口: 23)-->
				</td>
			</tr>
		</table>
		</td>
	</tr>
</table>
<p>
<input type=button value="确认" onclick=apply() >
<input type=button value="取消" onclick=init() >
</p>
</form>

<script>pageTail(str_copyright);</script>
</body>
</html>
