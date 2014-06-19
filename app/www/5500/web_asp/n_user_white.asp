<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 白名单设置 </title>
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
env['sys.time'] = '946685149';
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

/*id 0; MAC 1; Auth 2;IGMP 3; down pir 4; up pir 5; MAC limit 6*/

var white_list = new Array( );

var show_list=new Array();

var max_white = 256;

function set_row_color(item)
{
	return (white_auth(show_list[item]) == '0') ? '#FFB0B0' : '#F0F0F0';
}

function checkbox_click(item)
{
	var f = document.forms[0];
	for(var i = 0; i < show_list.length; i ++) {
		if(pageview_ischecked(i)) break;
	}	
	f.button_del.disabled = !(i < show_list.length); 
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
		return '禁用';	
	}else if (r < 1024){
		return r + "Kbps";	
	}else {
		return r + "Kbps (" + ((r*1.0)/(1024)).toFixed(3) +"Mbps)";
	}
}

function show_status(item)
{
	var opt = parseInt(item, 10);
	return opt ? ("开启：" + opt) : "禁用";
}

function show_enable(item)
{
	return (item=="1")?"启用":"禁用";
}

function show_auth(item)
{
	return (item == '1') ? "授权" : "未授权";
}

function white_goto(item)
{
	location='n_white_edit.asp?white_mac=' + white_mac(show_list[item]);
}

function white_search(list,filter)
{
	var show=new Array();
	var re=/^\s*$/;
	var fact=filter.toUpperCase();		
	if (re.test(filter)) return list;
	for ( var i = 0; i < list.length ; i ++ )
	{
		var m = white_mac(list[i]).toUpperCase();
		if (m.indexOf(fact) != -1){
			show[show.length] = list[i];
			continue;
		}
		var m1 = m.split(':');
		var m2 = '';
		for (var j= 0; j < m1.length; j ++){
			m2 += m1[j];	
		}
		if (m2.indexOf(fact) != -1){
			show[show.length] = list[i];
			continue;
		}
		var m = white_index(list[i]);
		if ( m == filter )
		{
			show[show.length] = list[i];
			continue;	
		}
	}
	return show;
}


function search_apply()
{
	show_list = white_search(white_list, document.forms[0].input_search.value);
	pageview_reset();	
	pageview_add('ID', 0, '6%', true);
	pageview_add('MAC 地址', 1, '18%', true, null, "white_goto");
	pageview_add('授权状态', 2, '12%', false,show_auth);	
	pageview_add('IGMP监听', 3, '12%', false,show_enable);
	pageview_add('Down-rate', 4, '15%', false, show_rate);
	pageview_add('Up-rate', 5, '15%', false, show_rate);
	pageview_add('MAC限制', 6, '12%', false,show_status);
	pageview_init(show_list, 10, document.getElementById('list_table'), true, checkbox_click, set_row_color, '白名单', checkbox_click);
}

function init()
{
	document.forms[0].input_search.value='';
	search_apply();
}
/*
<FORM name=OUT action=goform/command method=POST>
<INPUT type=hidden name=CMD value=WHITE_DEL>
<INPUT type=hidden name=GO value=n_user_white.asp>
<input type=hidden name=SET0 value="n_white=4">
</FORM>

*/

function apply_delete()
{
	var f = document.forms[0];
	for(var i = 0; i < show_list.length; i ++){
		if(pageview_ischecked(i)) {
			diffCfg('n_white_' + i, 'n_white', white_index(show_list[i]));
		}	
	}
	subForm(f,'goform/command',"WHITE_DEL",current_page() );
}

function new_white()
{	
	if( white_list.length >= max_white ){
		alert("Number of services reachs up to maximum value, can not accept any more!");
		return;
	}
	location='n_white_edit.asp';
}


</script>
</head>
<body onload="init();">
<script>pageHead(env, env['web.page']);</script>

<form name=white_table action=goform/command method=post onsubmit="return false;">

<div id="searchBar">
    <input class="searchInput" name=input_search > &nbsp;&nbsp;&nbsp;
    <input class="searchButton" type=button name=button_search value=' 查 找' onclick="search_apply();">&nbsp;&nbsp;&nbsp;
    <input class="searchButton" type=button name=button_reset value=' 清 空 ' onclick="init();">
</div>

<div id="list_table"></div>
<p>
<table width=60% border=0 align=center cellpadding=2 cellspacing=0>
	<tr>
		<td bgcolor=#F0F0F0 width=30>&nbsp;&nbsp;</td>
		<td style="padding:0 0 0 4px; text-align:left" >授权</td>
		<td bgcolor=#FFB0B0 width=30>&nbsp;&nbsp;</td>
		<td style="padding:0 0 0 4px; text-align:left">未授权</td>
	</tr>
</table>
</p>

<p>
<INPUT class="button" name=button_del type=button value=" 删除 " onclick="apply_delete();" disabled=true>
&nbsp;&nbsp;&nbsp;
<INPUT class="button" name=button_new type=button value=" 新建白名单 " onclick="new_white();">
</p>
</form>

<script>pageTail();</script>
</body>
</html>