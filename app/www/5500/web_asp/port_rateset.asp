<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - 广播风暴抑制 </title>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
<script language=javascript src=script/valid.js></script>
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
env['sys.time'] = '946684828';
env['sys.linkup'] = '0';
env['sys.linkoff'] = '0';
env['sys.busy'] = '0';
env['sys.ipc'] = '0';
env['sys.vlan'] = '0';
env['sys.unsaved'] = webVars[1];
env['sys.feature'] = 'vlanpool;mme_drop_ports;slave_access_mgmt;bcmp;';


env['web.page'] = current_page();env['sys.parent_name'] = '端口限速';

pageRedirectCheck(env);
</script>
<!-- end header -->

<script language=JavaScript>

var storm_limit = new Array('0;0;0;0');

var linerate = new Array(<% CGI_GET_SINGLE_LINERATE(); %>);
var numPorts = 4;
//var linerate = new Array('7','1','20');
var pMsk = linerate[0];

function storm_uucast()
{
	var vs;
	if (storm_limit.length > 0){
		vs = storm_limit[0].split(';');
		if (vs.length > 0){
			return vs[0];	
		}
	}	
	return '0';
}


function storm_umcast()
{
	var vs;
	if (storm_limit.length > 0){
		vs = storm_limit[0].split(';');
		if (vs.length > 1){
			return vs[1];	
		}
	}	
	return '0';
}


function storm_bcast()
{
	var vs;
	if (storm_limit.length > 0){
		vs = storm_limit[0].split(';');
		if (vs.length > 2){
			return vs[2];	
		}
	}	
	return '0';
}


function storm_mcast()
{
	var vs;
	if (storm_limit.length > 0){
		vs = storm_limit[0].split(';');
		if (vs.length > 3){
			return vs[3];	
		}
	}	
	return '0';
}

function rate_in_mbps(rate)
{
	return !(rate % 1024)? true: false;	
}

function rate_on_change()
{
	var f=document.forms[0];
	f.cfg_uucast_rate_value.disabled = (f.cfg_uucast_rate.value == '0') ? true : false;
	f.cfg_umcast_rate_value.disabled = (f.cfg_umcast_rate.value == '0') ? true : false;	
	f.cfg_bcast_rate_value.disabled = (f.cfg_bcast_rate.value == '0') ? true : false;
	//f.cfg_mcast_rate_value.disabled = (f.cfg_mcast_rate.value == '0') ? true : false;	
}



function linerate_init()
{
	var f=document.forms[0];
	
	f.inrate.value = linerate[1];
	f.outrate.value = linerate[2];
}

function init()
{
	var sh, obj;
	var f=document.forms[0];
	linerate_init();

/*	
	for (var i = 0; i < port_config.length; i ++){
		sh = port_config[i].split(';')[0];
		if (port_if_cpu(sh)) continue;
		
		obj = document.getElementsByName('cfg_port_' + sh)[0];
		if (obj){
			obj.checked = (port_storm_limit(sh) == 'on')? true : false;	
		}
	}
*/	
}

/* functions for submit */

function valid_port_rate(rate)
{
	return ((rate >= 8) && (rate <=1048568) && ((rate % 8) == 0)) ? true : false;
}

function number_check(val)
{
  var re = /^[1-9][0-9]*$/; 
  return re.test(val);	
}


function storm_rate_check()
{
	var f=document.forms[0];
	var rate;
	
	if (f.cfg_uucast_rate.value == '1'){
		if (number_check(f.cfg_uucast_rate_value.value) == false) return false;
	 	if (f.uucast_rate_unit[0].checked == true){
	 		rate = f.cfg_uucast_rate_value.value * 1024;
		}else {
			rate = f.cfg_uucast_rate_value.value;	
		}	
	 	if (valid_port_rate(rate) == false){
	 		return false;	
	 	}			
	}
	
	if (f.cfg_umcast_rate.value == '1'){
		if (number_check(f.cfg_umcast_rate_value.value) == false) return false;
	 	if (f.umcast_rate_unit[0].checked == true){
	 		rate = f.cfg_umcast_rate_value.value * 1024;
		}else {
			rate = f.cfg_umcast_rate_value.value;	
		}	
	 	if (valid_port_rate(rate) == false){
	 		return false;	
	 	}			
	}	
	
	if (f.cfg_bcast_rate.value == '1'){
		if (number_check(f.cfg_bcast_rate_value.value) == false) return false;
	 	if (f.bcast_rate_unit[0].checked == true){
	 		rate = f.cfg_bcast_rate_value.value * 1024;
		}else {
			rate = f.cfg_bcast_rate_value.value;	
		}	
	 	if (valid_port_rate(rate) == false){
	 		return false;	
	 	}			
	}
	/*
	if (f.cfg_mcast_rate.value == '1'){
		if (number_check(f.cfg_mcast_rate_value.value) == false) return false;
	 	if (f.mcast_rate_unit[0].checked == true){
	 		rate = f.cfg_mcast_rate_value.value * 1024;
		}else {
			rate = f.cfg_mcast_rate_value.value;	
		}	
	 	if (valid_port_rate(rate) == false){
	 		return false;	
	 	}			
	}	*/

	return true;
}


function get_storm_config(n)
{
	var f=document.forms[0];
	var uucast, umcast, bcast, mcast;
	
	if (f.cfg_uucast_rate.value == '0'){
		uucast = 0;	
	}else if (f.uucast_rate_unit[0].checked == true){
		uucast = f.cfg_uucast_rate_value.value * 1024;
	}else {
		uucast = f.cfg_uucast_rate_value.value;	
	}

	if (f.cfg_umcast_rate.value == '0'){
		umcast = 0;	
	}else if (f.umcast_rate_unit[0].checked == true){
		umcast = f.cfg_umcast_rate_value.value * 1024;
	}else {
		umcast = f.cfg_umcast_rate_value.value;	
	}

	if (f.cfg_bcast_rate.value == '0'){
		bcast = 0;	
	}else if (f.bcast_rate_unit[0].checked == true){
		bcast = f.cfg_bcast_rate_value.value * 1024;
	}else {
		bcast = f.cfg_bcast_rate_value.value;	
	}
/*
	if (f.cfg_mcast_rate.value == '0'){
		mcast = 0;	
	}else if (f.mcast_rate_unit[0].checked == true){
		mcast = f.cfg_mcast_rate_value.value * 1024;
	}else {
		mcast = f.cfg_mcast_rate_value.value;	
	}		
	*/
	mcast = umcast;
	return uucast + ';' + umcast + ';' + bcast + ';' + mcast;	
}



function rate_check(rate)
{
	if(rate == 0)
	{	
		return true;
	}
	
	reg = /^([0-9]{0,7})$/;
	if(-1 == rate.search(reg))
	{
		alert("无效的速率限制值,请重新输入!");
		return false;		
	}
	
	if(rate<0 || rate>1024000 || (rate%64)!=0)
	{
		alert("无效的速率限制值,请重新输入!");
		return false;
	}

	return true;
}

function apply()
{
	var f=document.forms[0];	
	var config;
	
	/*
	if (storm_rate_check() == false){
			alert("无效的速率限制值,请重新输入!");
			return;
	}
	*/
	/*
	config = get_storm_config();
	if (storm_limit.length > 0){
		if (storm_limit[0] == config){
			alert("配置没有改变，请返回！");
			return;	
		}	
	}
	*/
	f.portmask.value = pMsk;
    var inrate = f.inrate.value;// * 64;
    var outrate = f.outrate.value;// * 64; 
	
	if(false == rate_check(inrate) ||  false == rate_check(outrate))
	{
		return false;
	}
	diffCfg('portmask', 'portmask', f.portmask.value);	
	diffCfg('inrate', 'inrate', inrate);	
	diffCfg('outrate', 'outrate', outrate);	
	subForm(f,'goform/command','LINE_RATE', "port_linerate.asp");
}




</script>
</head>
<body onload="init();">
<script>pageHead(env, env['web.page']);</script>
<form name=form_port_config action=goform/command method=post  onsubmit="return false;">
<INPUT type=hidden name=CMD>
<INPUT type=hidden name=GO>
<INPUT type=hidden name=action>

<table width=100% cellpadding=5 cellspacing=0>
	<tr ><td colspan=2 class="tb_head">端口限速设置</td> </tr>
	<tr><td><table width=100% cellpadding=2 cellspacing=1 border=0 >			
	  <TR>
    <TD class=tb_head>端口名 </TD>
    <TD class=tb_content><LABEL maxLength=15 name=pridns size=15 >
    <script language="javascript" type="text/javascript">
    var row_portNo = 0;
    for(i=0 ; i< numPorts; i++)
    {
        if(pMsk & (1<<i))
        {
            document.write("端口 " + (i+1)+"");
            row_portNo++;
            if(0==(row_portNo%4))
            {
                document.write("<br>");
            }
            else
            {
                document.write("&nbsp;");
            }
        }
    }
    </script>
    </TD></TR>
	<tr>
		<td width=40% class=tb_title>入端口限速</td>
    <td class="tb_content"><input name=inrate size=15 maxlength=7>&nbsp;&nbsp;(0 - 1024000，单位kbps, 0表示不限速）</td>
	</tr>
	<tr>
		<td width=40% class=tb_title>出端口限速</td>
    <td class="tb_content"><input name=outrate size=15 maxlength=7>&nbsp;&nbsp;(0 - 1024000，单位kbps, 0表示不限速）</td>
	</tr>	
	<!--
	<tr>
		<td class=tb_title>组播报文抑制</td>
		<td class=tb_content>
			<select name=cfg_mcast_rate onchange="rate_on_change();">
			<option value="0">禁用</option>
			<option value="1">启用</option>
			</select>
			<input name=cfg_mcast_rate_value size=8 maxlength=8>
			<input type=radio name=mcast_rate_unit value="Mbps"> Mbps
			<input type=radio name=mcast_rate_unit value="Kbps"> Kbps
		</td>
	</tr>	
	<tr>
		<td class=tb_title>应用端口</td>
		<td class=tb_content>
		<table width=100% border=0 align=center cellpadding=0 cellspacing=0>
			<script>
				document.write(port_checkbox_string());
				</script>	
		</table>
		</td>
	</tr>	-->	
	<tr>
		<!--
		<td colspan=2><font size=2 color=blue>注意:带宽限制使用Mbps输入时,范围为1-1023;<br>
			使用Kbps输入时,范围为8-1048568,且须为8的倍数.</font></td>
		-->
	</tr>	
</table>
<p><input name="portmask" type="hidden" value=""></p>
<p>
<input class="button" type=button value='  确  认 ' onclick=apply() >
&nbsp;&nbsp;&nbsp;
<input class="button" type=button value=' 取  消 ' onclick=init() >
</p>
</form>
<script>pageTail();</script>
</body>
</html>