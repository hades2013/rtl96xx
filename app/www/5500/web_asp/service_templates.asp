<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 模板管理 </title>
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
env['sys.time'] = '946685198';
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
var supdev = new Array('CNU101E;2;1;1;32769',
'CE-T52X4E;5;2;2;32770',
'CE-T52X3I;4;3;3;32771',
'CB203A;4;4;4;32772',
'CE-T50X4E;5;5;5;32773',
'CE-T50X2E;3;6;6;32774',
'CE-T5202I;3;7;7;32775');
var alldev = new Array('CNU101E;2;1;1;32769',
'CE-T52X4E;5;2;2;32770',
'CE-T52X3I;4;3;3;32771',
'CB203A;4;4;4;32772',
'CE-T50X4E;5;5;5;32773',
'CE-T50X2E;3;6;6;32774',
'CE-T5202I;3;7;7;32775',
'CB201A;2;1;129;32769',
'CB203;4;4;130;32772',
'CNU101;2;1;131;32769',
'CNU104;5;2;132;32770',
'STB103;4;3;133;32771',
'CNU204;5;2;134;32770',
'STB203;4;3;135;32771',
'CE-T50X4;5;5;136;32773',
'CE-T50X2;3;6;137;32774',
'STB203G;3;7;138;32775',
'CE-T5003I;4;3;139;32771');
var tmpl_list = new Array('32769;1;Default;Default template for CNU101E Serials;1,1',
'32770;2;Default;Default template for CE-T52X4E Serials;1,1,1,1,1',
'32771;3;Default;Default template for CE-T52X3I Serials;1,1,1,1',
'32772;4;Default;Default template for CB203A Serials;1,1,1,1',
'32773;5;Default;Default template for CE-T50X4E Serials;1,1,1,1,1',
'32774;6;Default;Default template for CE-T50X2E Serials;1,1,1',
'32775;7;Default;Default template for CE-T5202I Serials;1,1,1');
var max_tmpls = 264;


var show_list=new Array();


function user_templates_num()
{
	var count = 0;
	for (var i = 0; i < tmpl_list.length; i ++){
		if (tmpl_index(tmpl_list[i]) <= max_tmpls){
			count ++;
		}	
	}
	return count;
}

function extent_devices_string(baseid)
{
	var str = '';				
	for (var i=0; i < alldev.length; i ++){
		if (dev_baseid(alldev[i]) == baseid){
			if (str == ''){
				str = str + dev_alias(alldev[i]);
			}else {
				str = str + ',' + dev_alias(alldev[i]);	
			}
		}	
	}
	return str;
}

function dev_string(devid)
{
	for (var d = 0; d < supdev.length; d ++){
		if (dev_devid(supdev[d]) == devid){
			return dev_alias(supdev[d]);	
		}	
	}
	return 'Unknown';
}
function tmpl_edit(index)
{
	var tid = tmpl_index(show_list[index]);
	location='service_template_prop.asp?tmpl_id=' + tid;
}

function new_tmpl()
{
	if (user_templates_num() >= max_tmpls){
		alert("模板数量已达到最大值，不能再添加新模板！");
		return;
	}
	location='service_template_prop.asp?tmpl_id=-1';
}


function checkbox_click(item)
{
	for(var i = 0; i < show_list.length; i ++) 
		if(pageview_ischecked(i)) break;
	document.forms[0].button_del.disabled = !(i < show_list.length);
}

function show_devices(index)
{
	var devid = tmpl_devid(show_list[index]);
	var str = '包括以下设备类型：\n';
	str += extent_devices_string(devid);
	confirm(str);	
}

function show_tmpl_name(str)
{
	return uft8_string_show(str, 24);
}

function show_tmpl_desc(str)
{
	return uft8_string_show(str, 60);
}



function search_apply()
{
	show_list = tmpl_search(tmpl_list, document.forms[0].input_search.value);
	pageview_reset();
	pageview_add('模板名称', 2, '30%', true, show_tmpl_name, "tmpl_edit");
	pageview_add('支持设备', 1, '15%', true, dev_string, "show_devices");
	pageview_add('模板描述', 3, '50%', false, show_tmpl_desc);
	pageview_init(show_list, 10, document.getElementById('list_table'), true, checkbox_click, null, '模板', checkbox_click);
}

function init()
{
	if (is_for_nscrtv_hc(env)){
		document.getElementById("p_buttons").style.display = "none";
	}
	document.forms[0].input_search.value='';
	search_apply();
}


function apply_tmpl_supp()
{
	var f = document.forms[0];
	setCfg('TMPL_DIS', f.template_disable.value);
	subForm(f, 'goform/command', "TMPL_EN", current_page());
}

function apply()
{
	var f = document.forms[0];
	for(var i = 0; i < show_list.length; i ++)
		if(pageview_ischecked(i)) {
			if(tmpl_index(show_list[i]) > max_tmpls) {
				alert('默认模板不能被删除！');
				for(var j = 0; j < show_list.length; j ++) {
					pageview_clearselected(j);
				}
				checkbox_click();
				return 0;
			}
			diffCfg('tmpl_' + i, 'tmpl_del', tmpl_index(show_list[i]));
		}
	subForm(f,'goform/command', "TMPL_DEL", current_page());
}



</script>
</head>
<body onload="init();">
<script>pageHead(env, env['web.page']);</script>

<form name=system_status action=goform/command method=post onsubmit="return false;">

<div id="searchBar">
    <input class="searchInput" name=input_search > &nbsp;&nbsp;&nbsp;
    <input class="searchButton" type=button name=button_search value=' 查 找 ' onclick="search_apply();">&nbsp;&nbsp;&nbsp;
    <input class="searchButton" type=button name=button_reset value=' 清 空 ' onclick="init();">
</div>

<div id="list_table"></div>
<div id=p_buttons>
	<p>
		<INPUT name="button_del" class="button" type=button value="删除选择模板" onclick="apply();" disabled=true>
		&nbsp;&nbsp;&nbsp;<INPUT class="button" type=button value="新建模板" onclick="new_tmpl();">
	</p>
</div>
</form>
<script>pageTail();</script>
</body>
</html>