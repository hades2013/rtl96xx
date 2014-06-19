<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - VLAN池管理 </title>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
<script language=javascript src=script/valid.js></script>
<script language=javascript src=script/pageview.js></script>
<script language=javascript src=script/cnu_mgmt.js></script>
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
env['sys.time'] = '946685178';
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


var vlanpool_list = new Array( '1;DefaultPort1;1;0;4094;4094;0;1-4094',
'2;DefaultPort2;1;0;4094;4094;0;1-4094',
'3;DefaultPort3;1;0;4094;4094;0;1-4094',
'4;DefaultPort4;1;0;4094;4094;0;1-4094' );

var show_list;

var max_vlanpools = 16;

function checkbox_click(item)
{
	var f = document.forms[0];
	for(var i = 0; i < show_list.length; i ++) {
		if(pageview_ischecked(i)) break;
	}	
	f.button_del.disabled = !(i < show_list.length); 
}

function vlanpool_goto(item)
{
	location='n_vlanpool_edit.asp?id=' + vlanpool_index(show_list[item]);
}

function show_enable(item)
{
	return (item == "1") ? "启用" : "禁用";
}

function show_name(str)
{
	return uft8_string_show(str, 16);
}

function show_vlans(s)
{
	var thr = 28;
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

/* index ; name ; enable ; flag ; count ; available ; prio ; vlan */
function init()
{
		show_list = vlanpool_list;
		pageview_reset();	
		pageview_add('ID', 0, '5%', true, null);
		pageview_add('名字', 1, '15%', true, show_name, "vlanpool_goto");
		pageview_add('状态', 2, '8%', false, show_enable);
		pageview_add('唯一特性', 3, '12%', false, show_enable);		
		pageview_add('总数', 4, '8%', false, null);
		pageview_add('可用数', 5, '8%', false, null);
		pageview_add('优先级', 6, '8%', false, null);
		pageview_add('VLAN', 7, '30%', false, show_vlans);
		pageview_init(show_list, 10, document.getElementById('vlan_list'), true, checkbox_click, null, 'VLAN池列表', checkbox_click, null);
}

function apply_delete()
{
	var pool_id;
	var f = document.forms[0];
	for(var i = 0; i < show_list.length; i ++){
		pool_id = parseInt(vlanpool_index(show_list[i]));
		if (pageview_ischecked(i) && (pool_id >= 1) && (pool_id <= 4)){
			alert("VLAN池\"" + vlanpool_name(show_list[i]) + "(" + pool_id + ")" + "\"是默认VLAN池，不能被删除！");	
			return;
		}
	}
		
	for(var i = 0; i < show_list.length; i ++){
		if(pageview_ischecked(i)) {
			diffCfg('n_vlanpool_' + i, 'n_vlanpool', vlanpool_index(show_list[i]));
		}	
	}
	subForm(f,'goform/command',"VLANPOOL_DEL", current_page());
}

function new_vlanpool()
{
	if( vlanpool_list.length >= max_vlanpools ){
		alert("VLAN池数量已达到最大值，不能再添加新的VLAN池！");
		return;
	}
	location='n_vlanpool_edit.asp?id=-1';
}

</script>
</head>
<body onload=init()>
<script>pageHead(env, env['web.page']);</script>

<form name=vlan_pool action=goform/command method=post onsubmit="return false;">
<div id="vlan_list"></div>
<p>
<INPUT class="button" name=button_del type=button value="删除所选VLAN池" onclick="apply_delete();" disabled=true>
&nbsp;&nbsp;&nbsp;
<INPUT class="button" name=button_new type=button value="新建VLAN池" onclick="new_vlanpool();">
</p>
</form>

<script>pageTail();</script>
</body>
</html>