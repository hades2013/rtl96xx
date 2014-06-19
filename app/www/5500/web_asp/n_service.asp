<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 服务管理 </title>
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
env['sys.time'] = '946685162';
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


var service_list = new Array(  );

var show_list=new Array();

var max_service = 256;


function set_row_color(item)
{
	if(user_auth(show_list[item]) == 'acc') return '#F0F0F0';
	if(user_auth(show_list[item]) == 'blk') return '#FFB0B0';
	if(user_auth(show_list[item]) == 'any') return '#C0C0F0';
}

function checkbox_click(item)
{
	var f = document.forms[0];
	for(var i = 0; i < show_list.length; i ++) {
		if(pageview_ischecked(i)) break;
	}	
	f.button_del.disabled = !(i < show_list.length); 
}

function show_name(str)
{
	return uft8_string_show(str, 26);
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



function show_rate(str)
{
	var r = parseInt(str, 10);
	if (r == 0){
		return "Disabled";	
	}else if (r < 1024){
		return r + "Kbps";	
	}else {
		return r + "Kbps (" + ((r*1.0)/(1024)).toFixed(3) +"Mbps)";
	}
}

function service_goto(item)
{
	location='n_service_edit.asp?id=' + service_index(show_list[item]);
}

function service_search(list,filter)
{
	var show=new Array();
	var re=/^\s*$/;
	var fact=filter.toUpperCase();		
	if (re.test(filter)) return list;
	for ( var i = 0; i < list.length ; i ++ )
	{
		var m = service_name(list[i]).toUpperCase();
		if (m.indexOf(fact) != -1){
			show[show.length] = list[i];
			continue;
		}
		var m = service_index(list[i]);
		if ( m == filter )
		{
			show[show.length] = list[i];
			continue;	
		}
	}
	return show;
}


function init()
{
	document.forms[0].input_search.value='';
	search_apply();
}

function search_apply()
{
	show_list = service_search(service_list, document.forms[0].input_search.value);
	pageview_reset();	
	pageview_add('ID', 0, '8%', true);
	pageview_add('服务名称', 1, '25%', true, show_name, "service_goto");
	//pageview_add('优先级', 3, '10%', true);
	pageview_add('下行限速', 6, '25%', true, show_rate);
	pageview_add('上行限速', 7, '25%', true, show_rate);
	pageview_init(show_list, 10, document.getElementById('list_table'), true, checkbox_click, null, '服务列表', checkbox_click);
}


function apply_delete()
{
	var f = document.forms[0];
	for(var i = 0; i < show_list.length; i ++){
		if(pageview_ischecked(i)) {
			diffCfg('n_service_' + i, 'n_service', service_index(show_list[i]));
		}	
	}
	subForm(f,'goform/command',"SERVICE_DEL",current_page());
}

function new_service()
{
	if( service_list.length >= max_service ){
		alert("服务数量已达到最大值，不能再添加新服务！");
		return;
	}
	location='n_service_edit.asp?id=-1';
		
}


</script>
</head>
<body onload="init();">
<script>pageHead(env, env['web.page']);</script>

<form name=service_table action=goform/command method=post onsubmit="return false;">

<div id="searchBar">
    <input class="searchInput" name=input_search > &nbsp;&nbsp;&nbsp;
    <input class="searchButton" type=button name=button_search value=' 查 找' onclick="search_apply();">&nbsp;&nbsp;&nbsp;
    <input class="searchButton" type=button name=button_reset value=' 清 空 ' onclick="init();">
</div>

<div id="list_table"></div>
<p>
<INPUT class="button" name=button_del type=button value="删除所选服务" onclick="apply_delete();" disabled=true>
&nbsp;&nbsp;&nbsp;
<INPUT class="button" name=button_new type=button value="新建服务" onclick="new_service();">
</p>
</form>

<script>pageTail();</script>
</body>
</html>