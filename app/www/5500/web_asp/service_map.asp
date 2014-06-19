<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 网络拓扑 </title>
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
env['sys.time'] = '946685140';
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
var client_list = new Array ();
var user_list = new Array();
var max_users = 256;
var white_list = new Array( );
var max_whites = 256;

var show_list=new Array();

function dev_string(alias)
{
	return alias;
}

function link_string(lnk)
{
	switch(parseInt(lnk, 10)) {
	case 1:
		return '<img src=images/on.png>';
	case 2:
		return '<img src=images/off.png>';
	case 3: // loop
		return '<img src=images/on.png>';		
	case 4:
		return '<img src=images/off.png>';
	default:
		return '<img src=images/alert.png>';
	}
}

function sta_info(item)
{
	location='service_sta_prop.asp?mac=' + cnu_mac(show_list[item]);
}

function user_edit(item)
{
	location='user_edit.asp?user_mac=' + cnu_mac(show_list[item]);
}

function redirect_to_link_stats(item)
{
	if((cnu_link(show_list[item]) == 'up')
	   || (cnu_link(show_list[item]) == 'loop'))
		location='service_pcab_stat.asp?mac=' + cnu_mac(show_list[item]);
	else {
		alert("设备不在线，无法获取其链路信息!");	
	}
}

var color_loop = "#FFFA96";
var color_rejected = "#FF7070";
var color_whitelist = "#F0F0F0";
var color_blacklist = "#FFB0B0";
var color_anonymous = "#C0C0F0";

function set_row_color(index)
{
	if(cnu_link(show_list[index]) == 'loop') return color_loop;
	if(cnu_auth(show_list[index]) == 'accept') return color_whitelist;
	if(cnu_auth(show_list[index]) == 'deny') return color_blacklist;	
	if(cnu_link(show_list[index]) == 'deny') return color_rejected;
	
	return color_anonymous;
}

function cnu_refresh()
{
	var f = document.forms[0];
	subForm(f,'goform/command', "REFRESH", "service_map.asp");	
}

function cnu_refresh_clear()
{
	var f = document.forms[0];
	subForm(f,'goform/command', "REFRESH_CLEAR", "service_map.asp");	
}





function checkbox_on_click(item)
{
	for(var i = 0; i < show_list.length; i ++) { 
		if(pageview_ischecked(i)) break;
	}
	document.forms[0].btn_wht.disabled = !(i < show_list.length);
	document.forms[0].btn_blk.disabled = !(i < show_list.length);	
	document.forms[0].btn_remove.disabled = !(i < show_list.length);	
	document.forms[0].btn_reset.disabled = !(i < show_list.length);
	document.forms[0].btn_reconfig.disabled = !(i < show_list.length);
	document.forms[0].btn_forcedown.disabled = !(i < show_list.length);		
	document.forms[0].btn_resetboard.disabled = !(i < show_list.length);	
}


function cnu_user_auth(cnu)
{
	var auth = cnu_auth(cnu);
	if (auth == 'accept'){
		return 'acc';
	}	else if (auth == 'deny'){
		return 'blk';	
	}	
	return 'any';
}

function devid_to_baseid(devid)
{
	for(var i = 0; i < alldev.length; i ++) { 
		if ((dev_devid(alldev[i])) == devid){
			 return dev_baseid(alldev[i]);
		}
	}
	return -1;		
}

function devid_to_tmplid(devid)
{
	for(var i = 0; i < alldev.length; i ++) { 
		if ((dev_devid(alldev[i])) == devid){
			 return dev_tmplid(alldev[i]);
		}
	}
	return -1;		
}


function show_user_name(str)
{
	return uft8_string_show_brief(str, 14);
}

function id_string(mac)
{
	for (var i = 0; i < show_list.length; i ++){
		if (cnu_mac(show_list[i])== mac){
			return cnu_clt_id(show_list[i]) + '/' + cnu_index(show_list[i]);	
		}	
	}	
	return 'N/A';
}

function time_string(r)
{
	var d,h,m, s;
	d = Math.floor(r / (60*60*24));
	r = (r % (60*60*24));
	h = Math.floor(r / (60*60));
	r = (r % (60*60));
	m = Math.floor(r / 60);
	s = Math.floor(r % 60);
	
	if (d < 1){
		return 	h + "时" + m + "分" + s + "秒";
	}else {
		return  d + "天" + h + "时" + m + "分";
	}
}

function rate_string(rate)
{
	if (is_for_nscrtv(env)){
    	var vs = rate.split('/');
    	if (vs.length >= 2){
    		return vs[0] + ' / ' + vs[1];
    	}else{
    	return rate;
    	}
	}
	if(parseInt(rate, 10) == 0) return "N/A";
	return rate;
}

Number.prototype.toFixed=function(len)
{
	var add = 0;
	var s, temp;
	var s1=this + "";
	var start = s1.indexOf(".");
	if (s1.substr(start+len+1,1)>=5)add=1;
	temp = Math.pow(10,len);
	s = Math.floor(this*temp) + add;
	return s/temp;	
}

function atten_string(str)
{
	if (is_for_nscrtv(env)){
      if (str == '0/0'){
    	  return "N/A";	
      }else {
      	var vs = str.split('/');
      	if (parseInt(vs[0], 10) >= 60){
      		vs[0] = '≥60';
      	}
  	vs[1] = parseInt(vs[1]);
	  return vs[0] +'dB / '+(vs[1] * 0.01).toFixed(1) + 'dB';	
  }		
}

	var v = parseInt(str, 10);
	if (v >= 60){
		return "≥60dB";
	}else if (v > 0){
		return v + 'dB';
	}else{
		return "N/A";
	}
}


function config_string(str)
{
	if (str == '0'){
		return "白名单";	
	}else if (str == '-1'){
		return "无";	
	}else if (str == '-2'){
		return '手动输入';
	}else {
		return '模板';
	}
}


function get_version_brief(version)
{
	var re = /^[A-Za-z0-9]+-[A-Za-z0-9]+-[0-9]+-[0-9]+-[0-9]+-[0-9]+-[0-9]+-[0-9]+-[A-Za-z0-9]+/;	
	
	if (re.test(version)){
		var vs = version.split('-');
		return /*vs[4] +'-'+*/vs[7];
	}

	var re1 = /^[A-Za-z0-9]+-[A-Za-z0-9]+-\S+/;	
	if (re1.test(version)){
		var vs = version.split('-');
		if (vs[2].length <= 13){
			return vs[2];	
		}else {
			return vs[2].substring(0, 10) + '..';	
		}
	}
	
	if (version.length <= 13){
		return version;	
	}else {
		return version.substring(0, 10) + '..';	
	}	
}

function search_apply()
{
	show_list = cnu_search(client_list, document.forms[0].input_search.value);
	pageview_reset();	
	pageview_add('ID', 2, '5%', false, id_string);
	if (is_for_nscrtv(env)){
	  pageview_add('MAC 地址', 2, '14%', false, null, "sta_info");
	  pageview_add('型号', 6, '12%', false, dev_string);
	  pageview_add('固件版本', 14, '10%', false, get_version_brief);	
	  pageview_add('状态', 7, '6%', false, link_string);
	  pageview_add('衰减 / SNR', 13, '14%', false, atten_string, "redirect_to_link_stats");
	  pageview_add('TX / RX', 4, '10%', false, rate_string);
	  pageview_add('配置', 11, '8%', false, config_string);
	  pageview_add('在线时间', 12, '14%', false, time_string);
	}else {
	  pageview_add('MAC 地址', 2, '15%', true, null, "sta_info");
	  pageview_add('型号', 6, '15%', true, dev_string);
	  pageview_add('状态', 7, '8%', false, link_string);
	  pageview_add('衰减', 13, '10%', true, atten_string, "redirect_to_link_stats");
	  pageview_add('TX', 4, '8%', false, rate_string);
	  pageview_add('RX', 5, '8%', false, rate_string);
	  pageview_add('用户名', 9, '20%', false, show_user_name, "user_edit");
	}
	pageview_init(show_list, 10, document.getElementById('list_table'), true, checkbox_on_click, set_row_color, '设备列表', checkbox_on_click);	
	return false;
}


function cnu_reset(cmd)
{
	var i, cnt = 0, unsupported = 0, down = 0;
	var f = document.forms[0];
	var uset = new Array();
	
	for(i = 0; i < show_list.length; i ++){
		if(pageview_ischecked(i)) {
			if ((cnu_link(show_list[i]) != 'up')
			  && (cnu_link(show_list[i]) != 'loop')){
			 	down ++;
			 	continue; 	
			 }

			if ((cnu_auth(show_list[i]) != 'anon_accept')
				&& (cnu_auth(show_list[i]) != 'accept')){
					unsupported ++;
					continue;
			}
			uset[cnt] = cnu_mac(show_list[i]);
			cnt ++;
		}
	}	
	if (down > 0){
		if (!confirm("一些选择的设备处于离线状态，本次操作将忽略这些设备！")){
			return;
		}	
	}	
	
	if (unsupported > 0){
		if (!confirm("一些选择的设备处于未授权状态，本次操作将忽略这些设备！")){
			return;
		}	
	}
	
	if (cnt == 0){
		alert("请选择至少一个有效的设备！");
		return;
	}
			
	for (i = 0; i < uset.length; i ++){
			diffCfg('cnu_reset_' + i, 'cnu_reset', uset[i]);
	}
  if (cnt > 0) {
  	subForm(f,'goform/command', cmd, current_page());
  }
}

function show_counters()
{
	var online = 0, offline = 0, rejected = 0, loop = 0, unknown = 0; 	
	for (var i=0; i < client_list.length; i ++){
		switch(cnu_link(client_list[i])) {
		case 'up':
		  online ++;
			break;
		case 'down':
		  offline ++;
			break;
		case 'loop':
		  online ++;
		  loop ++;
			break;
		case 'deny':
			rejected ++;
			break;
		default:
			unknown ++;
			break;
		}					
	}
	document.getElementById('td_online').innerHTML = '&nbsp;' + online;
	document.getElementById('td_offline').innerHTML = '&nbsp;' + offline;
	document.getElementById('td_rejected').innerHTML = '&nbsp;' + rejected;
	document.getElementById('td_loop').innerHTML = '&nbsp;' + loop;
	document.getElementById('td_unknown').innerHTML = '&nbsp;' + unknown;
}





function init()
{
    //color_loop = getBackground("color_loop");
    //color_rejected = getBackground("color_rejected");
    //color_whitelist = getBackground("color_whitelist");
    //color_blacklist = getBackground("color_blacklist");
    //color_anonymous = getBackground("color_anonymous");
        
	var f = document.forms[0];
	f.input_search.value='';
	if (!is_for_nscrtv(env)){
		f.btn_remove.style.display="none";
	}
	search_apply();
	show_counters();
}


function apply_remove()
{
	var uanon = 0;
	var i, cnt = 0;
	var f = document.forms[0];
	var uset = new Array();
	
	for(i = 0; i < show_list.length; i ++){
		if(pageview_ischecked(i)) {
			if (cnu_user_auth(show_list[i]) == 'any'){
				uanon ++;	
			}else {
				uset[cnt] = cnu_mac(show_list[i]);
				cnt ++;				
			}
		}
	}
	if (uanon > 0){
		if (!confirm("一些选择的设备匿名设备，本次操作将忽略这些设备！")){
			return;
		}	
	}
	
	if (cnt == 0){
		alert("请选择至少一个有效的设备！");
		return;
	}

	for (i = 0; i < uset.length; i ++){
			diffCfg('user_' + i, 'remove', uset[i]);
	}
  if (cnt > 0) {
  	subForm(f,'goform/command', "NSCRTV_TOWHITE", current_page());
  }
}

function apply_nscrtv(cmd)
{
	var auth, unew = 0;
	var i, cnt = 0, set = 0;
	var f = document.forms[0];
	var uset = new Array();
	var set_cmd;
	if (cmd == 'ADD_TO_WHITE'){
		auth = 'acc';
		set_cmd = 'towhite';
	}else {
		auth = 'blk';
		set_cmd = 'toblack';
	}
	
	for(i = 0; i < show_list.length; i ++){
		if(pageview_ischecked(i)) {
			if (cnu_devid(show_list[i]) == '-1'){
				alert('不支持设备[' + cnu_mac(show_list[i]) + '],无法添加到用户列表');
				return;	
			}
			if (cnu_user_auth(show_list[i]) == auth){
					set ++;
					continue;
			}
			if (cnu_user_auth(show_list[i]) == 'any'){
				unew ++;	
			}
			uset[cnt] = cnu_mac(show_list[i]);
			cnt ++;
		}
	}
	if (set > 0){
		if (!confirm("一些选择的设备已存在于相应的用户列表中，本次操作将忽略这些设备！")){
			return;
		}	
	}
	
	if ((cnt == 0)&&(set > 0)){
		alert("选择的设备已存在于相应的用户列表中，请返回！");
		return;
	}
				
	if (unew + white_list.length > max_whites){
		alert("白名单中已有" + user_list.length +"items in whitelist, system could not accept all selected devices.Please remove some and try again");	
		return;
	}
	for (i = 0; i < uset.length; i ++){
			diffCfg('user_' + i, set_cmd, uset[i]);
	}
  if (cnt > 0) {
  	subForm(f,'goform/command', "NSCRTV_TOWHITE", current_page());
  }
}


function apply(cmd)
{
	if (is_for_nscrtv(env)){
		return apply_nscrtv(cmd);
	}

	var nstr, mac, auth, unew = 0;
	var i, cnt = 0, set = 0;
	var f = document.forms[0];
	var umac, utmpl, udev, uname, udesc, uid;
	var uset = new Array();
	var in_userlist;
	
	if (cmd == 'ADD_TO_WHITE'){
		auth = 'acc';
	}else {
		auth = 'blk';
	}	
	
	for(i = 0; i < show_list.length; i ++){
		if(pageview_ischecked(i)) {
			if (cnu_devid(show_list[i]) == '-1'){
				alert('不支持设备[' + cnu_mac(show_list[i]) + ']无法添加到用户列表');
				return;	
			}
			if (cnu_user_auth(show_list[i]) == auth){
					set ++;
					continue;
			}
			
			in_userlist = false;
			for (var u = 0; u < user_list.length; u ++){
				if (cnu_mac(show_list[i]) == user_mac(user_list[u])){
				 		umac = user_mac(user_list[u]);	
						utmpl = user_tmplid(user_list[u]);
						udev = user_devid(user_list[u]);
						uname = user_name(user_list[u]);	 		
						udesc = user_desc(user_list[u]);
						uid = user_index(user_list[u]);
						in_userlist = true;
				 		break;
				 }
			}
			
			if (in_userlist == false){
				umac = cnu_mac(show_list[i]);
				utmpl = devid_to_tmplid(cnu_devid(show_list[i]));
				if (utmpl == -1){
					alert("Internal error: unknown device id :" + cnu_devid(show_list[i]));	
					continue;
				}
				udev = cnu_devid(show_list[i]);
				uname = 'user' + umac.split(':')[3] + umac.split(':')[4] + umac.split(':')[5];
				udesc = '';
				uid = '0'
				unew ++;	
			}
			uset[cnt] = umac + ';' + utmpl + ';' + udev + ';' + auth + ';' + uname + ';' + udesc + ';' + uid;
			cnt ++;
		}
	}
	if (set > 0){
		if (!confirm("一些选择的设备已存在于相应的用户列表中，本次操作将忽略这些设备！")){
			return;
		}	
	}
	
	if ((cnt == 0)&&(set > 0)){
		alert("选择的设备已存在于相应的用户列表中，请返回！");
		return;
	}
				
	if (unew + user_list.length > max_users){
		alert("用户列表中已有" + user_list.length +"items in userlist, system could not accept all selected devices.Please remove some and try again");	
		return;
	}
	for (i = 0; i < uset.length; i ++){
			diffCfg('user_' + i, 'user_' + i, uset[i]);
	}
  if (cnt > 0) {
  	subForm(f,'goform/command', "USER_NEW", current_page());
  }
}

</script>
</head>

<body onload=init()>
<script>pageHead(env, env['web.page']);</script>

<form name=system_status action=goform/command method=post onsubmit="return false;">

<div id="searchBar">
    <input class="searchInput" name=input_search > &nbsp;&nbsp;&nbsp;
    <input class="searchButton" type=button name=button_search value=' 查 找 ' onclick="search_apply();">&nbsp;&nbsp;&nbsp;
    <input class="searchButton" type=button name=button_reset value=' 清 空 ' onclick="init();">
</div>

<p style="text-align:right">
<table width="100%" cellspacing=0 cellpadding=0 border=0 >
	<tr>
		<td style="text-align:left" > <input class="button" type=button value=" 刷新" onclick=cnu_refresh();>&nbsp;&nbsp; 
		<input class="button" type=button value="清空列表" onclick=cnu_refresh_clear();> </td>
		<td>
    		<table width=100% cellspacing=0 cellpadding=0 border=0 style="text-align:left">
    		<tr>
    		<td width=80 ></td>
    		<td class="statistics_block online">在线</td>
    		<td width=32 id=td_online></td>
    		<td class="statistics_block offline">离线</td>
    		<td width=32 id=td_offline></td>
    		<td class="statistics_block rejected">拒绝</td>
    		<td width=32 id=td_rejected></td>
    		<td class="statistics_block loop">自环</td>
    		<td width=32 id=td_loop></td>		
    		<td class="statistics_block unknown">未知</td>
    		<td width=32 id=td_unknown></td>
            </tr>
            </table>
		</td>
	</tr>
</table>
</p>
<div id="list_table"></div>
<p>
<table width=80% border=0 align=center cellpadding=1 cellspacing=2>
	<tr>
		<td id="color_whitelist" class="status_block whitelist">&nbsp;&nbsp;</td>
		<td>白名单</td>
		<td id="color_blacklist" class="status_block blacklist">&nbsp;&nbsp;</td>
		<td>黑名单</td>
		<td id="color_anonymous" class="status_block anonymous">&nbsp;&nbsp;</td>
		<td>匿名设备</td>
		<td id="color_rejected" class="status_block rejected">&nbsp;&nbsp;</td>
		<td>禁用设备</td>
		<td id="color_loop" class="status_block loop">&nbsp;&nbsp;</td>
		<td>自环设备</td>
	</tr>
</table>
</p>
<table width=100% border=0  cellpadding=0 cellspacing=0>
	<tr><td style="text-align:center">
	<input name="btn_wht" class="button" type=button value="添加到白名单" onclick="apply('ADD_TO_WHITE');" disabled=true>&nbsp;	
	<input name="btn_blk" class="button" type=button value="添加到黑名单" onclick="apply('ADD_TO_BLACK');" disabled=true>&nbsp;
	<input name="btn_remove" class="button" type=button value="从白名单移除" onclick="apply_remove();" disabled=true>&nbsp;	
	<input name="btn_reset" class="button" type=button value=" 复位 " onclick="cnu_reset('CNU_RESET');" disabled=true>&nbsp;
	<input name="btn_reconfig" class="button" type=button value="重新配置" onclick="cnu_reset('CNU_RECONFIG');" disabled=true>&nbsp;
	<input name="btn_forcedown" class="button" type=button value="强制下线" onclick="cnu_reset('CNU_FORCEDOWN');" disabled=true>&nbsp;	
	<script> 
		if (env['vendor.id'] == 'innflicks'){
		 document.write("<input name='btn_resetboard' type=button value='Remote-Reset' onclick=cnu_reset('CNU_RSTBOARD'); disabled=true>&nbsp;");	
		}else {
		 document.write("<input name='btn_resetboard' type=hidden>");
		}
	</script>
	</td></tr>
</table>
</form>

<script>pageTail();</script>
</body>
</html>

