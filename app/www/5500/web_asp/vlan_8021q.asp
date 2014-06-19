<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 802.1Q VLAN设置 </title>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
<script language=javascript src=script/valid.js></script>
<script language=javascript src=script/pageview.js></script>
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
env['sys.time'] = '946685088';
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
//var vlan_list = new Array('0;eth1,eth0,eth2,eth3;1');
var vlan_list =new Array(<% CGI_GET_VLAN_CONFIG(); %>);
//var vlan_options = new Array('0','1','cpu0;access','eth0;access','eth1;trunk','cab0;trunk','cab1;trunk','cab2;trunk','cab3;trunk');
var vlan_options=new Array( <% CGI_GET_VLAN_OPTIONS(); %>);

/* port const functions */
function port_if_cpu(sh)
{
	var re_cpu = /^cpu[0-9]{1,}$/;
	return re_cpu.test(sh);
}

function port_if_cable(sh)
{
	var re_cab = /^cab[0-9]{1,}$/;
	return re_cab.test(sh);
}

function port_fancy_name(sh)
{
	/*
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
	base += ((i != -1) ? i : "");/*/
	return sh;
}

/* end port const functions */

function default_members()
{
	var str = "";
	for (var i = 2; i < vlan_options.length; i ++ ){
		var p = vlan_options[i].split(';')[0];
		if(port_if_cpu(p)) continue; // remove cpu port
		if(str != "") str += ",";
		str += p;
	}
	return str;
}

var vlan_ids = new Array();

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



function update_vlan_ids()
{
	var num = 0;
	vlan_ids = new Array();	
	for(var i = 0; i < vlan_list.length; i ++) {
		var vs=parse_vlan(vlan_list[i].split(';')[2]);//.split(',');	
		for (var j=0; j < vs.length; j++){
			vlan_ids[parseInt(vs[j], 10)]=parseInt(vs[j], 10);
		}
	}
	for (var j=0; j < 4096; j++){
		if (vlan_ids[j]){
			num ++;
		}
	}	
	return num;
}

function new_vlan()
{
	var f = document.forms[0];
    var input = f.vid.value;
    var t = /^\s*$/;

    if (1 == t.test (input))
    {
        alert ("请输入VLAN ID字串！");
        f.vid.focus();
        return;
    }

    if(!isValidVlanString(input))
    {
        alert ("VLAN ID字串包含非法字符！");
        return;
    }
 
 if(isAnyVlanExist(input, vlan_ids))
 {
    if (!confirm("一些VLAN已存在VLAN列表,确定要继续吗?"))return;	
 }

 var mems = default_members();
// alert("defalut=" + mems);
 diffCfg('vlan_0', 'vlan_add', '0;'+ mems + ';' + input);
 subForm(f, 'goform/command', 'VLAN_NEW', 'vlan_8021q.asp');
}

function delete_edit()
{
	var f = document.forms[0];
    var input = f.vid.value;
    var t = /^\s*$/;
    
    if (1 == t.test (input))
    {
        alert ("请输入VLAN ID字串！");
        f.vid.focus();
        return;
    }

    if(!isValidVlanString(input))
    {
        alert ("VLAN ID字串包含非法字符！");
        return;
     }
 
     if(isAnyVlanNotExist(input, vlan_ids))
     {
 			alert("要删除的VLAN不存在！");
 			return;	
    }
    
	var vin = new Array();
	var vs=parse_vlan(input);
	for (var j=0; j < vs.length; j++){
		vin[parseInt(vs[j], 10)]=parseInt(vs[j], 10);
	}	

	if(isAnyVlanExist("1", vin))
	{
 		alert("VLAN 1 是默认VLAN,不能被删除 !");
 		return;			 	
	}
	if(isAnyVlanExist(vlan_mvid(), vin))
	{
		if (!confirm("VLAN " + vlan_mvid() + " 是管理VLAN,它被删除后,管理VLAN将自动恢复到默认配置?")) return;			 	
	}		 
     
    var mems = default_members();  		
    diffCfg('vlan_0', 'vlan_del', '0;'+mems + ';' + input);
    subForm(f, 'goform/command', 'VLAN_DEL', 'vlan_8021q.asp');
}

function delete_vlan()
{
	for(var i = 0; i < vlan_list.length; i ++){ 
		if(pageview_ischecked(i)) {
			diffCfg('vlan_' + i, 'vlan_del', vlan_list[i]);
		}
  }
	subForm(document.forms[0],'goform/command', 'VLAN_DEL', 'vlan_8021q.asp');
}

function edit_vlan(s)
{
	var f = document.forms[0];
	for(var i = 0; i < vlan_list.length; i ++) 
		if(pageview_ischecked(i)) {
			f.vid.value = vlan_list[i].split(';')[2];	
			var vs =  vlan_list[i].split(';')[1].split(',');
    }
}


function vlanid_string(s)
{
	var thr = 54;
	if (s.length > thr){
		var ss = s.split(',');
		var i, base = 0, r = '';
		for (i = 0; i < ss.length; i ++){
			if ((s.indexOf(ss[i]) + ss[i].length - base) > thr){
				base = s.indexOf(ss[i]) + ss[i].length;
				if (r != '')r += ',</br>';
			}else {
				if (r != '')r += ',';				
			}	
			r += ss[i];							
		}	
		return r;
	}else {
		return s;
	}
}

function vlan_group(s)
{
	if (s == '') return " ";
	var vs = s.split(',');
	var vstr = '';
	for(var i = 0; i < vs.length; i ++) {
		vstr += port_fancy_name(vs[i]);
		if(i != vs.length - 1) vstr += ', ';
  }
	return  vstr;
}


function checkbox_click(item)
{
	var cnt = 0;
	for(var i = 0; i < vlan_list.length; i ++) {
		if(pageview_ischecked(i)){
			cnt ++;	
    }
	}
	document.getElementById('btn_del').disabled = !(cnt > 0);
	document.getElementById('btn_edit').disabled = !(cnt == 1);
}

function vmchange()
{
	if(vlan_mode() == '0') {
		document.getElementById('vlan_1q').style.display = 'block';
		document.getElementById('desc').style.display = 'none';
	}
	else {
		document.getElementById('vlan_1q').style.display = 'none';
		document.getElementById('desc').style.display = 'block';
	}		
}

function total_string()
{
	return '总数 ' + update_vlan_ids();	
}

function init()
{
	pageview_add('VLAN ID', 2, '55%', true, vlanid_string);
	pageview_add('端口成员', 1, '40%', true, vlan_group);
	pageview_init(vlan_list, 10, document.getElementById('vlan_list'), true, checkbox_click, null, 'VLAN列表', checkbox_click, total_string());
	vmchange();
	//update_vlan_ids();
}
</script>
</head>

<body onload=init()>
<script>pageHead(env, env['web.page']);</script>

<form name=vlan_8021q action=goform/command method=post onsubmit="return false;">
<INPUT type=hidden name=CMD>
<INPUT type=hidden name=GO>
<INPUT type=hidden name=action>

<div id="desc">系统VLAN在透传模式 ,
请点击<a href="vlan_mode.asp">更改VLAN模式</a>配置802.1QVLAN</div>

<div id="vlan_1q">
<table width=100% cellpadding=5 cellspacing=0>
	<tr ><td class="tb_head">添加或修改VLAN</td> </tr>		
	<tr>
		<td class=tb_content><input name="vid" size=75> &nbsp;&nbsp;
		<input class="button" type=button name="btn_new" value=" 创建 "  onclick="new_vlan();">&nbsp;&nbsp;
		<input class="button" type=button name="btn_deledit" value=" 删除 "  onclick="delete_edit();">			
		</td>
	</tr>
	</table>
</br>
<div id="vlan_list"></div>
<p>
</p>
<table align=center >
	<tr>
	<td width=30 valign=top>
		<INPUT id = "btn_del" class="button"  type=button value="删除所选VLAN" onclick="delete_vlan();" disabled=true>&nbsp;&nbsp;&nbsp;
	</td>
	<td></td> 
	<td width=30 valign=top>
		<INPUT id = "btn_edit" class="button"  type=button value="编辑所选VLAN" onclick="edit_vlan();" disabled=true>
	</td>
	</tr>
</table>
</div>
</form>
<script>pageTail();</script>
</body>
</html>
 div id="vlan_1q">
<table width=100% cellpadding=5 cellspacing=0>
	<tr ><td class="tb_head">添加或修改VLAN</td> </tr>		
	<tr>
		<td class=tb_content><input name="vid" size=75> &nbsp;&nbsp;
		<input class="button" type=button name="btn_new" value=" 创建 "  onclic
