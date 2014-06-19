<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=navigatortab.js></SCRIPT>
<script language=JavaScript>
pageTitle();

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var syssnmp_group = new Array(<% CGI_SYS_SNMP_GROUP(); %>);

/* just for test
var webVars = new Array( 'EPN104', '0', '0');
var syssnmp_group = new Array('3;group0;0;1;group1;0;1;hmj;0;0;;0;0','user1;group1;user2;group1;;group1;user4;group4;');
//*/


var CURRENT_COUNT_INDEX = 0;
var GROUP_NAME0_INDEX = 1;
var USER_NAME0_INDEX = 0;
var MAX_GROUP_COUNT = 4;
var MAX_USER_COUNT = 4;

function get_value(INDEX)
{
	if (syssnmp_group.length > 0)
	{
		var vs = syssnmp_group[0].split(';');
		if (vs.length > 0) return vs[INDEX];
	}
	return false;
}

function get_user_value(INDEX)
{
	if (syssnmp_group.length > 0)
	{
		var vs = syssnmp_group[1].split(';');
		if (vs.length > 0) return vs[INDEX];
	}
	return false;
}

function parse_security(type_index)
{
	switch(type_index)
	{
		case 0: return "不认证不加密";
		case 1: return "只认证不加密";
		case 2: return "既认证又加密";
	}
}

var snmp_group_current_count = get_value(CURRENT_COUNT_INDEX); 
var snmp_group_name = new Array(MAX_GROUP_COUNT);
var snmp_group_security = new Array(MAX_GROUP_COUNT);
var snmp_group_mode = new Array(MAX_GROUP_COUNT);

var snmp_user_name = new Array(MAX_USER_COUNT);
var snmp_user_group_name = new Array(MAX_USER_COUNT);

var num = 0;
var index = GROUP_NAME0_INDEX;
for(num=0; num< MAX_GROUP_COUNT; num++)
{	
	snmp_group_name[num] = get_value(index);
	snmp_group_security[num] = parse_security(parseInt(get_value(++index)));
	snmp_group_mode[num] = (parseInt(get_value(++index))==0? "只读":"读写");
	index++;
}

index = USER_NAME0_INDEX;
for(num=0; num< MAX_USER_COUNT; num++)
{	
	snmp_user_name[num] = get_user_value(index);
	snmp_user_group_name[num] = get_user_value(++index);;
	index++;
}


function init()
{
	var f=document.forms["form_group"];
	
	f.SNMP_GROUP_NAME.value = "";
	f.SNMP_GROUP_SECURITY.value = 0;
	f.SNMP_GROUP_MODE.value = 0;
}

function CheckForm(f)
{
	if(f.TRAP_EN.value == '0') return true;
	if(!verifyIP2(f.TRAP_IP, "IP 地址", f.NM)) return false;
   	return true;
}

function find_snmp_add_group_which_num()
{	
	var gruop_index = 0;
	for(gruop_index=0; gruop_index < MAX_GROUP_COUNT; gruop_index++)
	{
		if(!snmp_group_name[gruop_index])
		{
			return gruop_index
		}
	}
}
		
function apply_all(type, group_index)
{	
	var f=document.forms["form_group"];


	if(0 == type)  //确定
	{	
		var string = f.SNMP_GROUP_NAME.value;
		var reg = /^([0-9a-zA-Z,\,,\~,\!,\@,\$,\%,\^,\&,\*,\(,\),-,\_,\+,\=,\{,\},\[,\],\:,\|,\.,\?,\<,\>]{1,32})$/;
		if(-1 == string.search(reg))
		{
			alert("组名不能输入非法字符:\\,\/,\",\',\;,#,空字符串,空格以及中文字符！");
			return false;		
		}
	
		if(snmp_group_current_count >= MAX_GROUP_COUNT)
		{
			alert("最多只支持创建"+MAX_GROUP_COUNT +"个组！");
			return false;
		}

		
		for(num=0; num< MAX_GROUP_COUNT; num++)
		{	
			if(snmp_group_name[num])
			{	
				if(string == snmp_group_name[num])
				{
					alert("组名" +string+"已经存在!");
					return false;	
				}
			}
		}
		
		after_add_count = ++snmp_group_current_count;
		diffCfg("SNMP_GROUP_COUNT", "snmp_group_count", after_add_count); 
		snmp_group_current_count--; 
		
		var which_num = find_snmp_add_group_which_num();
		diffCfg("SNMP_ADD_GROUP_WHICH_NUM", "snmp_add_group_which_num", which_num);  
		
		diffCfg("SNMP_GROUP_NAME", "snmp_group_name", f.SNMP_GROUP_NAME.value);
		diffCfg("SNMP_GROUP_SECURITY", "snmp_group_security",f.SNMP_GROUP_SECURITY.value);
		diffCfg("SNMP_GROUP_MODE", "snmp_group_mode", f.SNMP_GROUP_MODE.value);

		subForm(f,'goform/command','SNMP_GROUP', 'system_snmp_group.asp');
	}
	else  			//删除
	{	
		string = snmp_group_name[group_index];
		for(num=0; num< MAX_USER_COUNT; num++)
		{	
			if(snmp_user_name[num])
			{
				if(string == snmp_user_group_name[num])
				{
					alert("删除组" +string+ "之前，必须先将该组所包含的用户" +snmp_user_name[num] +"删除! ");
					return false;					
				}				
			}
		}
		
		diffCfg("SNMP_DEL_GROUP_WHICHNUM", "snmp_del_group_which_num", group_index);
		subForm(f,'goform/command','SNMP_GROUP', 'system_snmp_group.asp');
	}	

}

</script>
</head>

<body onload=init()>
<script>pageHead(webVars);</script>
<SCRIPT language=JavaScript>buildNavigatorTab(system_snmp, 3)</SCRIPT>
<form name="form_group" action=goform/command method=post>

<table width=100% border=0 align=center cellpadding=0 cellspacing=0
	class=box_tn>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1
			bgcolor="#FFFFFF">
		<tr>
				<td width=100% class=greybluebg colspan=2>新建组</td>
		</tr>
		<tr>
				<td class=bgblue>组名</td>
 				<td class=bggrey><input type="text" size="20" id="txt2" name="SNMP_GROUP_NAME" id="SNMP_GROUP_NAME" maxlength="32" onkeypress="onEnterSub_Firefox(event,formSubmit,user_level)">(1~32字符)</TD> 
		</tr>
		
		<tr>
			<td class=bgblue>安全级别</td>
 			<td class=bggrey><select name="SNMP_GROUP_SECURITY" style="WIDTH: 152px"><option value=0> 不认证不加密</option><option value=1>只认证不加密</option> <option value=2>既认证又加密</option></select></td>
		</tr>
		
		<tr>
			<td class=bgblue>v3访问模式</td>
 			<td class=bggrey><select name="SNMP_GROUP_MODE" style="WIDTH: 152px"><option value=0> 只读</option><option value=1>读写</option></select></td>
		</tr>
		<tr>
		<input type="hidden" name="SNMP_GROUP_COUNT" value="0"> 
		</tr>
		<tr>
		<input type="hidden" name="SNMP_ADD_GROUP_WHICH_NUM" value="0"> 
		</tr>
		<tr>
				<td width=100% class=bggrey colspan=2>&nbsp</td>
			</tr>		
		</table>
		
		<tr><td >
			<table width="100%" cellSpacing=1 cellPadding=1 >
							<tr  width=100% class=greybluebg colspan=2>
								<td width=25% align='center'><font size=3>组名 </font></td>
								<td width=25% align='center'><font size=3>安全级别 </font></td>
								<td width=25% align='center'><font size=3>v3访问模式 </font></td>
								<td width=25% align="center"><font size=3>删除</font></td>
						
							</tr>
							<script language="JavaScript">
							if(0 != snmp_group_current_count)
							{	
								var gruop_index = 0;
								for(gruop_index=0; gruop_index < MAX_GROUP_COUNT; gruop_index++)
								{		
									
									if(snmp_group_name[gruop_index])
									{	
										document.write('<tr  width=100% class=bggrey>');
										document.write('<td width=25% align="center">' + snmp_group_name[gruop_index] + '</td> ');
										document.write('<td width=25% align="center">' + snmp_group_security[gruop_index] + '</td> ');
										document.write('<td width=25% align="center">' + snmp_group_mode[gruop_index] + '</td> ');
										document.write('<td width=25% align="center"><input type=button value="删除" onclick=apply_all(1,' +gruop_index +') ></td></tr>');
									}
								}
							}
							</script>
			</table>
			<tr>
			<td><input type="hidden" name="SNMP_DEL_GROUP_WHICHNUM" value="0"></td>
			</tr>
		</td></tr>
		</td>
	</tr>
</table>
<p>
<input type=button value="确定" onclick=apply_all(0,0) >
<input type=button value="取消" onclick=init() >
</p>
</form>

<script>pageTail();</script>
</body>
</html>
