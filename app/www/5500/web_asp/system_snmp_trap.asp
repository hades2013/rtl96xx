<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title> OCN-OUTDOOR - SNMP代理设置 </title>
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
env['sys.time'] = '946685430';
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

<script language=JavaScript>
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var syssnmp_trap = new Array(<% CGI_SYS_SNMP_TRAP(); %>);

/*
var webVars = new Array( 'EPN104', '0', '0');
var syssnmp_trap = new Array('1;192.168.0.3;162;2;public1;public2;public3;public4;user1;3;user2;2;user3;2;user4;3;2');
*/
var EN_INDEX = 0;
var IP_INDEX = 1;
var COM_INDEX = 2;
var VERSION_INDEX = 3;
var COMMUNITY_INDEX = 4;
var USER_INDEX = 8;
var LAST_SAVE_INDEX = 16;

var MAX_USER_COUNT=4;
var MAX_COMMUNITY_COUNT=4;
var snmp_community_name=new Array(MAX_COMMUNITY_COUNT);
var snmp_user_name=new Array(MAX_USER_COUNT);
var snmp_user_safe=new Array(MAX_USER_COUNT);


function get_value(INDEX)
{
	if (syssnmp_trap.length > 0)
	{
		var vs = syssnmp_trap[0].split(';');
		if (vs.length > 0) return vs[INDEX];
	}
	return false;
}



function get_build(num)
{	
	for(var i=0;i<num;i++)
	{
		snmp_community_name[i] = get_value(COMMUNITY_INDEX+i);
		snmp_user_name[i] = get_value(USER_INDEX +i*2);
		snmp_user_safe[i] = get_value(USER_INDEX+1 +i*2 );
	}
}


function sele_safe(sele_safe_name_tmp,version)
{
	var str=new String("");
	var f=document.forms[0];
	get_build(4);
	
	str ='<table width=100% cellpadding=5 cellspacing=0>';
	//str +='<TR><TD height=0></TD></TR>';
	str +='<TR >';
	str +='<td class=tb_title>安全级别</td>';
	str +='<td class=tb_content><select id="slct2" name="TRAP_SAFE" style="WIDTH: 228px">';	
	if(1==version || 2==version)
	{
		str+='<option value=1>不认证不加密</option>';
	}
	else if(3==version )
	{	
		if(snmp_user_safe[sele_safe_name_tmp] ==0)
		{
			str+='<option value=0>不认证不加密</option>';	
		}
		else if(snmp_user_safe[sele_safe_name_tmp] ==1)
		{
			str+='<option value=1>只认证不加密</option>';
		}
		else if(snmp_user_safe[sele_safe_name_tmp] ==2)
		{
			str+='<option value=2>既认证又加密</option>';
		}	
	}	
	str +='</select></td>';
	str +='</TR>';		    
    //str +='<TR><TD height=25></TD></TR>';  
	str +='</TABLE>';
	document.getElementById('sele_safe_id').innerHTML = str;
}
function selet_user()
{
	var	f = document.forms[0];
	if(f.TRAP_VERSION.value==3)
	{
		for(var i=0;i<4;i++)
		{
			if(f.TRAP_CM.value==snmp_user_name[i])
			{
				sele_safe(i,f.TRAP_VERSION.value);
			}
		}
		f.TRAP_SAFE.disabled = false;
	}
	else
	{
		sele_safe(0,f.TRAP_VERSION.value);
		f.TRAP_SAFE.disabled = true;
	}
}
function sele_safename(last_save_safe_num,version)
{
	var str=new String("");
	get_build(4);

	str ='<table width=100% cellpadding=5 cellspacing=0>';
	//str +='<TR><TD height=0></TD></TR>';
	str +='<TR class=tb_title >';
	str +='<td class=tb_title>安全名 (1~32字符)</td>';
	str +='<td  class=tb_content><select id="slct1" name="TRAP_CM" style="WIDTH: 228" onChange="selet_user()">';	
	if(1==version || 2==version)
	{
		
		if(-1<last_save_safe_num && last_save_safe_num <4)//初始的时候,显示上一次提交的选择项
		{
			if(snmp_community_name[last_save_safe_num])
			str+='<option value='+snmp_community_name[last_save_safe_num]+'>'+snmp_community_name[last_save_safe_num]+'</option>';
		}
		for(var i=0;i<MAX_COMMUNITY_COUNT;i++)
		{	
			if(snmp_community_name[i])
			{
				if(last_save_safe_num!=i)
				str+='<option value='+snmp_community_name[i]+'>'+snmp_community_name[i]+'</option>';
			}
		}
	}
	else if(3==version )
	{
		if(-1<last_save_safe_num && last_save_safe_num <4)//初始的时候,显示上一次提交的选择项
		{
			if(snmp_user_name[last_save_safe_num])
			str+='<option value='+snmp_user_name[last_save_safe_num]+'>'+snmp_user_name[last_save_safe_num]+'</option>';
		}
		for(var i=0;i<MAX_USER_COUNT;i++)
		{
			if(snmp_user_name[i])
			{
				if(last_save_safe_num!=i)
				str+='<option value='+snmp_user_name[i]+'>'+snmp_user_name[i]+'</option>';
			}
		}
	}	
	str +='</select></td>';
	str +='</TR>';		    
	str +='</TABLE>';
	document.getElementById('sele_safe_name').innerHTML = str;
}

function Trapversion()
{
	var	f = document.forms[0];
	var str = '';
	sele_safename(5,f.TRAP_VERSION.value);
	selet_user();
	if(f.TRAP_VERSION.value == 1 || f.TRAP_VERSION.value == 2) 
	{
		f.TRAP_SAFE.disabled = true;
	}
	else
	{
		f.TRAP_SAFE.disabled = false;
	}	
}

function on_change()
{
	var	f = document.forms[0];
	
	f.TRAP_EN.disabled = false;
	if(f.TRAP_EN.value == 0) 
	{	
		f.TRAP_IP.disabled = true;
		f.TRAP_COM.disabled = true;
		f.TRAP_CM.disabled = true;
		f.TRAP_VERSION.disabled = true;
		f.TRAP_SAFE.disabled = true;
	}
	else
	{	
		var	f = document.forms[0];
		var str = '';
		f.TRAP_IP.disabled = false;
		f.TRAP_COM.disabled = false;
		f.TRAP_CM.disabled = false;
		f.TRAP_VERSION.disabled = false;

		if(f.TRAP_VERSION.value == 1 || f.TRAP_VERSION.value == 2) 
		{
			//f.TRAP_SAFE.value =1;
			f.TRAP_SAFE.disabled = true;
		}
		else
		{
			f.TRAP_SAFE.disabled = false;
		}
	}
}

function valid_ip(ip)
{	
	var ips = ip.split('.');
	for (var i = 0; i < 4; i ++)
	{	
		if (ips[i] == '') return false;
		if (parseInt(ips[i]) > 255) return false;	
	}
	var a0 = parseInt(ips[0]);
	if ((a0 == 0) || (a0 == 127) || (a0 > 223)){
		return false;	
	}
	return true;
}

function valid_ip_items(p)
{
  var re = /^\d*$/;
	if (!re.test(p.value))
	{
		return false;	
	}
  return true;
}

function check_all_input()
{
	var f = document.forms[0];
	var string = "";
	var reg;
	
	string = f.TRAP_IP.value;
	reg = /^(([1-9]|[1-9]\d|1\d\d|2[0-1]\d|22[0-3])\.){1}(([01]?\d?\d|2[0-4]\d|25[0-5])\.){2}([1-9]|[1-9]\d|1\d\d|2[0-4]\d|25[0-4])$/;
	var vs = string.split('.');		
	if((-1 == string.search(reg)) || (127 == vs[0]))
	{
		alert("目标主机IP地址非法！");
		return false;		
	}
		
	string = f.TRAP_COM.value;
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
	if((string == 80) || (string == 23) || (string == 2323) ||(string == 514) ||(string == 22))
	{
		alert('端口号'+string+'已经被使用，请重新输入!');
		return false;		
	}
	
	string = f.TRAP_CM.value;
	if(string=='')
	{
		alert("请先建团体名或用户名");
		return false;
	}
	/*reg = /^([0-9a-zA-Z,\,,\~,\!,\@,\#,\$,\%,\^,\&,\*,\(,\),-,\_,\+,\=,\{,\},\[,\],\:,\|,\.,\?]{1,32})$/;
	if(-1 == string.search(reg))
	{
		alert("安全名不能输入非法字符:\\,\/,\",\',\;,空字符串,空格以及中文字符！");
		return false;		
	}*/	
	 return true;
}



function init()
{
	var f=document.forms[0];
	var last_save_safe_num;
	get_build(4);
	f.TRAP_EN.value = get_value(EN_INDEX);
	f.TRAP_IP.value = get_value(IP_INDEX);
	f.TRAP_COM.value = get_value(COM_INDEX);
	f.TRAP_VERSION.value = get_value(VERSION_INDEX);
	
	last_save_safe_num=get_value(LAST_SAVE_INDEX);//获取提交的安全名，用于初始化
	sele_safename(last_save_safe_num,f.TRAP_VERSION.value);//安全名
	sele_safe(last_save_safe_num,f.TRAP_VERSION.value);//安全级别
	on_change();
}
function apply()
{
	var f=document.forms[0];
	var last_save_safe_num=0;
	if (!check_all_input())return;
	
	diffCfg("TRAP_EN", "snmp_trap_en", f.TRAP_EN.value);
	diffCfg("TRAP_IP", "snmp_trap_ip", f.TRAP_IP.value);
	diffCfg("TRAP_COM", "snmp_trap_com", f.TRAP_COM.value);
	diffCfg("TRAP_VERSION", "snmp_trap_version", f.TRAP_VERSION.value);
	diffCfg("TRAP_CM", "snmp_trap_community", f.TRAP_CM.value);
	diffCfg("TRAP_SAFE", "snmp_trap_safe", f.TRAP_SAFE.value);
	
	for(var i=0;i<4;i++)
	{	
		if(f.TRAP_VERSION.value==3)
		{
			if(f.TRAP_CM.value==snmp_user_name[i])
			{
				last_save_safe_num=i;
				break;
			}
			else
			last_save_safe_num=0;
		}
		else 
		{
			if(f.TRAP_CM.value==snmp_community_name[i])
			{
				last_save_safe_num=i;
				break;
			}
			else
			last_save_safe_num=0;			
		}
	}
	diffCfg("TRAP_SAFE_NUM", "snmp_last_save_safe_num", last_save_safe_num);

	
	subForm(f,'goform/command','SNMP_TRAP','system_snmp_trap.asp');
}



</script>
</head>

<body onload=init()>
<script>pageHead(env, env['web.page']);</script>
<form name=snmp_trap action=goform/command method=post onsubmit="return false;">

<table width=100% cellpadding=5 cellspacing=0>
    <TR>
    <TD colspan=3 class="tb_head">&nbsp;Trap使能设置</TD>
    </TR>
	<tr>
            <td  class=tb_title >SNMP Trap功能</td>
			<td  align=left class=tb_content >
				<select name="TRAP_EN" id="status" onchange="on_change()"  >
					<option value=0>关闭</option>
					<option value=1>开启</option>
				</select>
            </td>
	</tr>			
	<TR>
    <TD colspan=3 class="tb_head">&nbsp;Trap目标主机设置</TD>
    </TR> 
	<tr>
		<td class=tb_title>目标主机IP地址</td>
		<td class=tb_content><input type="text" size="32" id="ip" name="TRAP_IP" maxlength="15" ></td>
	</tr>	
	<tr>
		<td class=tb_title>端口号 (1~65535)</td>
		<td class=tb_content><input type="text" size="32" id="port_no" name="TRAP_COM" maxlength="5"></td>
	</tr>		

    <TR class=tb_title >
		<td class=tb_title>Trap版本</td>
		<td class=tb_content>
		<select id="slct1" name="TRAP_VERSION" style="WIDTH: 228" onChange="Trapversion()">
		<option value=1>V1</option>
		<option value=2>V2C</option>
		<option value=3>V3</option>
		</select></td>
    </TR>
 </TABLE>	
  <div id="sele_safe_name"></div>
  <div id="sele_safe_id"></div>
<p>
<input type=button value="确认" onclick=apply() >
<input type=button value="取消" onclick=init() >
</p>
</form>

<script>pageTail();</script>
</body>
</html>
