<HTML><HEAD>
<META NAME=Copyright CONTENT="">
<META http-equiv=Content-Type content=text/html;charset=gb2312>
<META HTTP-EQUIV=Pragma CONTENT=no-cache>
<TITLE>无线安全</TITLE>
<link rel="stylesheet" href="style.css" type="text/css">
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=valid.js></SCRIPT>
<SCRIPT language=JavaScript src=navigatortab.js></SCRIPT>
<script language=JavaScript>
var str_copyright =<%CGI_GET_COPYRIGHT();%>
var PROD_MODEL = <%CGI_GET_MODEL();%>
</SCRIPT>
<script language=JavaScript>
pageTitle();

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var ssid_list = new Array(<% CGI_SSID_LIST(); %>); 
var wlan_basic = new Array(<% CGI_WLAN_BASIC(); %>);
/*
var webVars = new Array( 'EPN104', '0', '0');
var wlan_basic = new Array('1;5;2;13;24');
var ssid_list = new Array( 
					'1;SSID-2;1;SSID-name2;1;1;1;1;1;1;8888888888;0', 
					'1;SSID-3;1;MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM;1;1;2;ssid-888;2;100;2',
					'1;SSID-4;1;SSID-name4;1;1;3;ls654;2;100;;1');
*/					
/*'(unkonw);SSID;ssid_en(0:dis,1:en);ssid_name;islate:(0-1);ssid_bcast(0-1);enprypt(0-4)'*/	
/*
0:, 1:SSID, 2:ssid_en, 3:name,4:端口隔离(0-1),5:广播使能(0-1),6:加密方式(0-4)

 加密方式为：0：不加密, 不取6之后的数据
			1：WEP ，取： 7：group(1-4), 8:len(1-2), 9:type(1-2), 10:code
			2-4:WPA-PSK 取： 7：code, 8:protocol(1-2);9:time(); 10:NONE  
			11:authtype(0:Open system, 1:Shared key,2:Auto)
*/	
var wlan_en = GetVal(wlan_basic, 0, 0);
var mode = GetVal(wlan_basic, 0, 1);


var cookie_ssid = "cookie_ssid";				
var a = new Array(20);
var a_old = new Array(20);


function get_ssid()
{
	var DEFAULT_SSID = GetVal(ssid_list, 0, 1);
	var ssid = getCookie(cookie_ssid);
	return (ssid=(ssid==null?DEFAULT_SSID:ssid)); 
}

function get_ssid_paras(ssid, ssid_array, a)
{
	var len = ssid_array.length;
	var i = 0;
	var j = 0
	var count = 0;

	if(ssid=='')
	{
		alert('SSID is null!');
		return false;
	}
	
	for(i=0; i<len; i++)
	{
		if(ssid==GetVal(ssid_array, i, 1))
		{
			count = ssid_array[i].length;
			for(j=0; j<count; j++)
			{
				a[j] = GetVal(ssid_array, i, j);
			}
			
			return true;
		}
	}
	return false;
}



function ssid_onchange(f)
{
	var cur_ssid = f.SSID_num.value; 
	if(false==get_ssid_paras(cur_ssid, ssid_list, a))
	{
		alert("Can't get ssid paras!");
		return;
	}
	if (a)
	{
		if(a[2] == 1)
			f.ssid_en.checked = true;
		else
			f.ssid_en.checked = false;
		restore_value(f, a);
 		f.amend.value="确定";
	}
	
	enable_ssid(f);
	
}

function restore_value(f, a)
{	
	f.ssid_name.value = a[3];
	f.SSID_num.value = a[1];

	if(a[4] == 1)
		f.sta_sep[1].selected = true;
	else
		f.sta_sep[0].selected = true;

	if(a[5] == 1)
		f.ssid_broad[1].selected = true;
	else
		f.ssid_broad[0].selected = true;

	f.ssid_enc[a[6]].selected = true;

	change_ssid_enc(f);

	if(a[6] == 0)
	{
		
	}
	else if(a[6] == 1)
	{
		document.getElementById("wep_group").value = a[7];
		document.getElementById("wep_key_len").value = a[8];
		document.getElementById("wep_key_type").value = a[9];
		document.getElementById("wep_key").value = a[10];
		document.getElementById("authtype").value = a[11];
	}
	else
	{
		document.getElementById("wpa_key").value = a[7];
		document.getElementById("wpa_key_type").value = a[8];
		document.getElementById("wpa_key_period").value = a[9];
	}

}



function init(f)
{	
	if(ssid_list.length==0)
	{	
		f.SSID_num.disabled = true;
		f.ssid_name.disabled = true;
		f.ssid_name.value = '';
		f.SSID_num_0.disabled = wlan_en=='0'?true:false;
		f.ssid_name_0.disabled = wlan_en=='0'?true:false;
		f.ssid_en.disabled = true;
		f.sta_sep.disabled = true;
		f.ssid_broad.disabled = true;
		f.ssid_enc.disabled = true;
		f.amend.disabled = true;
		return;
	}
	else 
	{	
		get_ssid_paras(get_ssid(), ssid_list, a);
		a_old = a; 
		if (a)
		{
			f.ssid_en.checked = (a[2] == 1?true:false);	
			restore_value(f, a_old);
		}

		enable_ssid(f);
		//enable_ssid(f);
	}
}


function enable_ssid(f)
{
	f.SSID_num.disabled = wlan_en=='0'?true:false;
	f.ssid_name.disabled = wlan_en=='0'?true:false;
	f.ssid_en.disabled = wlan_en=='0'?true:false;
	f.amend.disabled = wlan_en=='0'?true:false;
	
	f.amend_0.disabled = wlan_en=='0'?true:false;
	f.SSID_num_0.disabled = wlan_en=='0'?true:false;
	f.ssid_name_0.disabled = wlan_en=='0'?true:false;
	
	if(wlan_en=='1' && f.ssid_en.checked == true)
	{
		//f.ssid_name.disabled = false;
		f.sta_sep.disabled = false;
		f.ssid_broad.disabled = false;
		f.ssid_enc.disabled = false;
		document.getElementById("SSID_NAME_LINE").disabled=false;
		document.getElementById("STA_SEP_LINE").disabled=false;
		document.getElementById("SSID_BROAD_LINE").disabled=false;
		document.getElementById("SSID_ENC_LINE").disabled=false;

		if(f.ssid_enc[1].selected == true)
		{
			document.getElementById("authtype").disabled = false;
			document.getElementById("wep_group").disabled = false;
			document.getElementById("wep_key_len").disabled = false;
			document.getElementById("wep_key_type").disabled = false;
			document.getElementById("wep_key").disabled = false;
			document.getElementById("WEP_GROUP_LINE").disabled=false;
			document.getElementById("WEP_KEY_LEN_LINE").disabled=false;
			document.getElementById("WEP_KEY_TYPE_LINE").disabled=false;
			document.getElementById("WEP_KEY_LINE").disabled=false;
		}
		else if(f.ssid_enc[0].selected == true)
		{
		}
		else
		{
			document.getElementById("wpa_key").disabled = false;
			document.getElementById("wpa_key_type").disabled = false;
			document.getElementById("wpa_key_period").disabled = false;
			document.getElementById("WPA_KEY_LINE").disabled=false;
			document.getElementById("WPA_KEY_TYPE_LINE").disabled=false;
			document.getElementById("WPA_KEY_PERIOD_LINE").disabled=false;
		}
	}
	else if(wlan_en=='0' || f.ssid_en.checked == false)
	{
		//f.ssid_name.disabled = true;
		f.sta_sep.disabled = true;
		f.ssid_broad.disabled = true;
		f.ssid_enc.disabled = true;
		document.getElementById("SSID_NAME_LINE").disabled=true;
		document.getElementById("STA_SEP_LINE").disabled=true;
		document.getElementById("SSID_BROAD_LINE").disabled=true;
		document.getElementById("SSID_ENC_LINE").disabled=true;

		if(f.ssid_enc[1].selected == true)
		{
			document.getElementById("authtype").disabled = true;
			document.getElementById("wep_group").disabled = true;
			document.getElementById("wep_key_len").disabled = true;
			document.getElementById("wep_key_type").disabled = true;
			document.getElementById("wep_key").disabled = true;
			document.getElementById("WEP_GROUP_LINE").disabled=true;
			document.getElementById("WEP_KEY_LEN_LINE").disabled=true;
			document.getElementById("WEP_KEY_TYPE_LINE").disabled=true;
			document.getElementById("WEP_KEY_LINE").disabled=true;
		}
		else if(f.ssid_enc[0].selected == true)
		{
		}
		else
		{
			document.getElementById("wpa_key").disabled = true;
			document.getElementById("wpa_key_type").disabled = true;
			document.getElementById("wpa_key_period").disabled = true;
			document.getElementById("WPA_KEY_LINE").disabled=true;
			document.getElementById("WPA_KEY_TYPE_LINE").disabled=true;
			document.getElementById("WPA_KEY_PERIOD_LINE").disabled=true;
		}
	}
}


function dis_en_ssid()
{
	var f = document.forms[0];
	if(f.ssid_en.checked == false)
	{
		restore_value(f, a_old);
	}

	enable_ssid(f);
}


function change_ssid_enc(f)
{
	var m = '';
	
	if(f.ssid_enc[1].selected == true)
	{
		m += '<TABLE border=0>';
		m += '<TR id=line_authtype>'+
			 '<TD width=140 class="nameTextCell">认证类型：'+
			 '<td width=30>'+	
			 '<TD width=360  class=textCell>'+
		     '<SELECT name=authtype id=authtype>'+ 
		     '<OPTION value=0 >Open system'+
		     '<OPTION value=1>Shared key </OPTION>'+
		     '<OPTION value=2>Auto</OPTION>'+
		     '</SELECT></TD></TR>';	
		m += '<TR ID=WEP_GROUP_LINE>';
		m += '<TD width=140 class=nameTextCell>默认传输密钥：<TD width=48>';
		m += '<TD width=360 class=textCell><select name=wep_group id=wep_group>';
		m += '<OPTION value=1  selected>1<OPTION value=2>2<OPTION value=3>3<OPTION value=4>4';
		m += '</TR>';

		m += '<TR ID=WEP_KEY_LEN_LINE>';
		m += '<TD width=140 class=nameTextCell>密钥长度：<TD width=30>';
		m += '<TD width=360 class=textCell><select name=wep_key_len id=wep_key_len>';
        m += '<OPTION value=1 >64位 - 10个HEX字符或者5个ASCII字符'; 
        m += '<OPTION value=2 >128位 - 26个HEX字符或者13个ASCII字符';                           
		m += '</TR>';

		m += '<TR ID=WEP_KEY_TYPE_LINE>';
		m += '<TD width=140 class=nameTextCell>密钥类型：<TD width=30>';
		m += '<TD width=360 class=textCell><select name=wep_key_type id=wep_key_type>';
        m += '<OPTION value=1 >HEX'; 
        m += '<OPTION value=2 >ASCII';                           
		m += '</TR>';

		m += '<TR ID=WEP_KEY_LINE>';
		m += '<TD width=140 class=nameTextCell>密钥：<TD width=30>';
		m += '<TD width=360 class=textCell><input type=text id=wep_key name=wep_key size=22 maxlength=26 value="">';
		m += '</TR>';
	}

	if((f.ssid_enc[2].selected == true)||(f.ssid_enc[3].selected == true)||(f.ssid_enc[4].selected == true))
	{
		m += '<TABLE border=0>';

		m += '<TR ID=WPA_KEY_LINE>';
		m += '<TD width=136 class=nameTextCell>共享密钥：<TD width=52>';
		m += '<TD width=360 class=textCell><input type=text id=wpa_key name=wpa_key size=22 maxlength=63 value="">&nbsp;(范围:8~63个字符)';
		m += '</TR>';

		m += '<TR ID=WPA_KEY_TYPE_LINE>';
		m += '<TD width=136 class=nameTextCell>加密协议：<TD width=28>';
		m += '<TD width=360 class=textCell><select name=wpa_key_type id=wpa_key_type>';
		//if(4 != mode)
		{
	        m += '<OPTION value=1 >TKIP'; 
		}
        m += '<OPTION value=2 selected="selected">AES';                           
		//if((f.ssid_enc[2].selected != true)&&(4 != mode))
		{
	        m += '<OPTION value=3 >TKIP+AES';
		}
		m += '</TR>';

		m += '<TR ID=WPA_KEY_PERIOD_LINE>';
		m += '<TD width=136 class=nameTextCell>群组密钥更新周期：<TD width=28>';
		m += '<TD width=360 class=textCell><input type=text id=wpa_key_period name=wpa_key_period size=8 maxlength=4 value="3600">&nbsp;秒(范围:1~3600，缺省值:3600)';
		m += '</TR>';
	}

	document.getElementById("ssid_enc_mode").innerHTML=m;
	return;
}

function check_hex_key(I, num)
{
	var ret = 0;
	var msg = "WEP密钥错误，请输入" + num + "个HEX字符(0~9,a~f,A~F)!";
	
	if(I.value.length != num)
	{
		alert(msg);
		I.select();
		return 1;
	}

	if(10 == num)
	{
		ret = HEX_Check10(I);
	}
	else if(26 == num)
	{
		ret = HEX_Check26(I);
	}
	
	if(ret)
	{
		alert(msg);
		return 1;
	}

	return 0
}

function check_ascii_key(I, num)
{
	var msg = "WEP密钥错误，请输入" + num + "个ASCII字符!";

	if(I.value.length != num)
	{
		alert(msg);
		I.select();
		return 1;
	}
	if(chinese_check(I))
	{
		return 1;
	}

	return 0;
}

function check_wep_input(f)
{
	//check wep key index
	var i = 0;
	var count=ssid_list.length;

	for(i=0;i<count;i++)
	{
		tmp=ssid_list[i];		
		tt=tmp.split(';');

		if(tt[1] == a[1])
			continue;
		
		if(tt[6] != 1)
			continue;
	
		if(tt[7] == document.getElementById("wep_group").value)
		{
			alert("默认传输密钥与其他SSID的默认传输密钥冲突，请重新设置!");
			return 1;
		}
	}

	var len = document.getElementById("wep_key_len").value;
	var type = document.getElementById("wep_key_type").value; 

	// check wep key value 
	if((len == 1)&&(type == 1))
	{
		if(check_hex_key(document.getElementById("wep_key"), 10))
			return 1;
	}
	else if((len == 1)&&(type == 2))
	{
		if(check_ascii_key(document.getElementById("wep_key"), 5))
			return 1;
	}
	else if((len == 2)&&(type == 1))
	{
		if(check_hex_key(document.getElementById("wep_key"), 26))
			return 1;
	}
	else if((len == 2)&&(type == 2))
	{
		if(check_ascii_key(document.getElementById("wep_key"), 13))
			return 1;
	}

	return 0;
}


function group_key_period_check(I)
{
	if (I.value == "")
	{
		alert("\"群组密钥更新周期\"不能为空");
		return 1;
	}
	if (0 != number_check(I))
	{
		return 1;
	}
	if (I.value < 1 || I.value > 3600)
	{
		alert("\"群组密钥更新周期\"值超出范围");
		return 1;
	}

	return 0;
}

function check_wpa_input(f)
{
	if((document.getElementById("wpa_key").value.length < 8) || (document.getElementById("wpa_key").value.length > 63))
	{
		alert("WPA密钥错误，请输入8~63个字符!");
		//f.wpa_key.select();
		return 1;
	}
	if(chinese_check(document.getElementById("wpa_key")))
	{
		return 1;
	}
	
	if(group_key_period_check(document.getElementById("wpa_key_period")))
	{
		return 1;
	}
	return 0;
}

function name_check(P)
{
	if(P.value == '')
	{
		alert("ssid 名称不能为空!");
        return false;
	}
    if (chinese_check(P))
    {	
        return false;
    }
    if (P.value.length > 31)
    {
        P.value = "";
        alert("ssid 名称不能超过31字节!");
        return false;
    }
	
	return true;
}

function add_edit_ssid(f, action)
{
	var i;
	var count=ssid_list.length;	
	var msg;
	var ssid_num = 0;
	var ssid_name = '';
	var ssid_en = 0;
	var sta_sep = 0;
	var ssid_broad = 0;
	var ssid_enc = 0;
	
	var wep_group = 0;
	var wep_key_len = 0;
	var wep_key_type = 0;
	var wep_key = 0;
	
	var wpa_key = 0;
	var wpa_key_type = 0;
	var wpa_key_period = 0;

	if(action == 0)  //add ssid
	{	
		for(i=0; i<ssid_list.length; i++)
		{
			if(GetVal(ssid_list, i, 1) == f.SSID_num_0.value)
			{
				alert(f.SSID_num_0.value+'已经存在！');
				return;	
			}
			if(GetVal(ssid_list, i, 3) == f.ssid_name_0.value)
			{
				alert('名称("'+f.ssid_name_0.value+'")已被其他SSID使用，请重新设置!');
				return;	
			}
		}
		
		if(!name_check(f.ssid_name_0))
			return;
		ssid_num = f.SSID_num_0.value;
		setCookie(cookie_ssid, ssid_num);
		ssid_num = ssid_num.replace(/[^(0-9)]/g, "");
		ssid_num = parseInt(ssid_num)-1; 
		ssid_name = f.ssid_name_0.value;
		
		diffCfg("SSID_num", "ssid_num",  ssid_num);
		diffCfg("ssid_name", "ssid_name",  ssid_name);
	}
	else//edit ssid		
	{
		if(!name_check(f.ssid_name))
			return;
		ssid_num = f.SSID_num.value;
		ssid_en = f.ssid_en.checked==true?1:0;
		ssid_name = f.ssid_name.value;
		a[1] = f.SSID_num.value;
		a[3] = f.ssid_name.value;
		//alert("a[1]:"+a[1]+" a[3]:"+a[3])
		if(f.ssid_en.checked == false)
		{	
			sta_sep =  a[4];
			ssid_broad = a[5];
			ssid_enc = parseInt(a[6]);
			//msg = a[0] + ";"+ a[1] + ";" + "0;" + a[3] + ";" + a[4] + ";" + a[5] + ";" + a[6] +";";
			if(a[6] == 0)
			{
				//msg += "wep-none;"
			}
			else if(a[6] == 1)
			{
				//msg += a[7] + ";" + a[8] + ";" + a[9] + ";" + a[10] + ";";
				wep_group = a[7];
				wep_key_len = a[8];
				wep_key_type = a[9];
				wep_key = a[10];
				authtype = a[11];
			}
			else
			{
				//msg += a[7] + ";" + a[8] + ";" + a[9] + ";";
				wpa_key = a[7];
				wpa_key_type = a[8];
				wpa_key_period = a[9];
			}
			
		}
		else
		{	
			//msg = a[0] + ";"+ a[1] + ";" + "1;";
			//msg += f.ssid_name.value + ";" + f.sta_sep.value + ";" + f.ssid_broad.value + ";" + f.ssid_enc.value + ";";
			sta_sep =  f.sta_sep.value;
			ssid_broad = f.ssid_broad.value;
			ssid_enc = parseInt(f.ssid_enc.value);
			//选择的加密方式是不加密
			if(ssid_enc == 0)
			{
				msg += "wep-none;"
			}
			//选择的加密方式是WEP加密		
			else if(ssid_enc == 1)
			{	
				if(check_wep_input(f))
					return;	
				//如果当前的无线工作模式是n_only
				if(4 == mode)
				{
					alert("n_only不支持WEP加密，请使用其他加密方式!");
					document.getElementById("ssid_enc").focus();
					return;
				}		
				//如果当前的无线工作模式是b+g+n		
				if(5 == mode)
				{
					var result_tkip = window.confirm("11n不支持WEP和TKIP，继续操作会使11n无线客户端最高工作在54Mbps。\n                您确信要修改吗?");						
					if (0 == result_tkip)
					{
						return;
					}				
				}
				//msg += document.getElementById("wep_group").value + ";";
				//msg += document.getElementById("wep_key_len").value + ";";
				//msg += document.getElementById("wep_key_type").value + ";";
				//msg += document.getElementById("wep_key").value + ";";
				authtype = document.getElementById("authtype").value;
				wep_group = document.getElementById("wep_group").value;
				wep_key_len = document.getElementById("wep_key_len").value;
				wep_key_type = document.getElementById("wep_key_type").value;
				wep_key = document.getElementById("wep_key").value;		
			}
			//选择的加密方式是WPA-PSK,WPA2-PSK或WPA-PSK/WPA2-PSK				
			else
			{
				if(check_wpa_input(f))
					return;		
				//如果当前的无线工作模式是b+g+n		
				if(5 == mode && 1 == document.getElementById("wpa_key_type").value)
				{
					var result_tkip = window.confirm("11n不支持WEP和TKIP，继续操作会使11n无线客户端最高工作在54Mbps。\n您确信要修改吗?");						
					if (0 == result_tkip)
					{
						return;
					}			
				}			
				//msg += document.getElementById("wpa_key").value + ";" + document.getElementById("wpa_key_type").value + ";" + document.getElementById("wpa_key_period").value + ";";
				wpa_key = document.getElementById("wpa_key").value;
				wpa_key_type = document.getElementById("wpa_key_type").value;
				wpa_key_period = document.getElementById("wpa_key_period").value;
			}
		}
		setCookie(cookie_ssid, ssid_num);
		ssid_num = ssid_num.replace(/[^(0-9)]/g, "");
		ssid_num = parseInt(ssid_num)-1; 
		diffCfg("SSID_num", "ssid_num",  ssid_num);
		diffCfg("ssid_name", "ssid_name",  ssid_name);
		diffCfg("ssid_en", "ssid_en",  ssid_en);
		diffCfg("sta_sep", "ssid_isolate_en",  sta_sep);
		diffCfg("ssid_broad", "ssid_broad_en",  ssid_broad);
		diffCfg("ssid_enc", "ssid_enc",  ssid_enc);
		
		if(ssid_enc==1)
		{
			diffCfg("authtype", "authtype",  authtype);
			diffCfg("param1", "wep_group",  wep_group);
			diffCfg("param2", "wep_key_len",  wep_key_len);
			diffCfg("param3", "wep_key_type",  wep_key_type);
			diffCfg("param4", "wep_key",  wep_key);
		}
		else if(ssid_enc>1)
		{	
			diffCfg("param5", "wpa_key",  wpa_key);
			diffCfg("param6", "wpa_key_type",  wpa_key_type);
			diffCfg("param7", "wpa_key_period",  wpa_key_period);
		}
	}
	
	
	subForm(f,'goform/command', 'SSID_LIST', 'wlan_ap_ssid_config.asp');
}

</SCRIPT>
</HEAD>
<BODY bgcolor=#FFFFFF leftMargin=0 topMargin=0 MARGINWIDTH=0 MARGINHEIGHT=0 BORDER=0 onLoad=init(document.frmSetup);>
<script>pageHead(webVars,PROD_MODEL);</script>
<SCRIPT language=JavaScript>buildNavigatorTab(wlan_ap, 2)</SCRIPT>
<TABLE width=100%>
<form name=frmSetup action='goform/command' method=post target='hf'>

		<td >
			<input type=hidden name=CMD>
			<input type=hidden name=GO>
			<input type=hidden name=param1>
			<input type=hidden name=param2>
			<input type=hidden name=param3>
			<input type=hidden name=param4>
			<input type=hidden name=param5>
			<input type=hidden name=param6>
			<input type=hidden name=param7>
		</td>

		<TR><TD class=titleCell colSpan=3>SSID添加</TD></TR>
		<td width=540 align=top>
		<TABLE cellSpacing=0 cellPadding=2 border=0 width=100%>						
 
 
 			<tr class=spaceLine ><TD colspan=3  class=spaceLine>
 			<tr class=spaceLine ><TD colspan=3  class=spaceLine>
 			<tr class=spaceLine ><TD colspan=3  class=spaceLine>
 			<tr class=spaceLine ><TD colspan=3  class=spaceLine>
			
			<TR class='textCell' id=line_ssidnum_enable_0>
				<TD class=nameTextCell>SSID 编号：
				<td width=30>  
				<TD class=textCell><SELECT name=SSID_num_0 id=SSID_num_0">
					<OPTION value=SSID-1  selected>SSID-1
				    <OPTION value=SSID-2>SSID-2
				    <OPTION value=SSID-3>SSID-3
				    <OPTION value=SSID-4>SSID-4&nbsp&nbsp
				    </OPTION>
				    </SELECT></TD>				
			</TR>
			
			<TR id=SSID_NAME_LINE_0>
				<TD width=140 class='nameTextCell'>SSID名称：</TD>
				<TD width=30>
				<TD width=360 class='textCell'>
					<input name=ssid_name_0 type="text" size="22" maxlength="31">(1-31个字符)</TD></TR>
			<TR class='textCell'><td colspan=3>&nbsp;	
			
			<TR class='textCell'>
			<TD width=140 class=nameTextCell>				
			    <INPUT type=button class=button name=amend_0 value='添加' onClick="add_edit_ssid(this.form, 0);" >
			<TD width=30>
			<TD class='textCell' width=360>
			<TR class='textCell'><td colspan=3>&nbsp;	
	
		</table>
		
		<TR><TD class=titleCell colSpan=3>SSID参数编辑</TD></TR>
		<td width=540 align=top>
		<TABLE cellSpacing=0 cellPadding=2 border=0 width=100%>						
 
 
 			<tr class=spaceLine ><TD colspan=3  class=spaceLine>
 			<tr class=spaceLine ><TD colspan=3  class=spaceLine>
 			<tr class=spaceLine ><TD colspan=3  class=spaceLine>
 			<tr class=spaceLine ><TD colspan=3  class=spaceLine>
			
			<TR class='textCell' id=line_ssidnum_enable>
				<TD class=nameTextCell>SSID 编号：
				<td width=30>  
				<TD class=textCell><SELECT name=SSID_num id=SSID_num onchange="ssid_onchange(this.form)">
				   <script>
					var str = '';
					var i = 0;
					
					for(i=0; i<ssid_list.length; i++)
						str +='<OPTION value='+GetVal(ssid_list, i, 1)+'  selected>'+GetVal(ssid_list, i, 1)+'</option>';
					document.write(str);
					</script>
				    </SELECT></TD>				
			</TR>
			
			<TR id=SSID_NAME_LINE>
				<TD width=140 class='nameTextCell'>SSID名称：</TD>
				<TD width=30>
				<TD width=360 class='textCell'>
					<input name=ssid_name type="text" size="22" maxlength="31">(1-31个字符)</TD></TR>
			<TR >
				<TD width=140 class='nameTextCell'>启用SSID：	
				<TD width=30 >
				<TD width=360 class='textCell'>
					<input name="ssid_en" type=checkbox onclick=dis_en_ssid(document.frmSetup)>		

			<TR id=STA_SEP_LINE>
				<TD width=140 class='nameTextCell'>客户端隔离：</TD>
				<TD width=30>
				<TD width=360 class='textCell'>
					<SELECT name=sta_sep> 
    				    <OPTION value=0  selected>禁用
    				    <OPTION value=1>启用

			<TR id=SSID_BROAD_LINE>
				<TD width=140 class='nameTextCell'>SSID广播：</TD>
				<TD width=30>
				<TD width=360 class='textCell'>
					<SELECT name=ssid_broad > 
    				    <OPTION value=0  selected>禁用
    				    <OPTION value=1>启用

			    
			<TR id=SSID_ENC_LINE>
				<TD width=140 class='nameTextCell'>加密方式：</TD>
				<TD width=30>
				<TD width=360 class='textCell'>
					<SELECT name=ssid_enc id=ssid_enc onchange=change_ssid_enc(document.frmSetup)> 
    				    <OPTION value=0  selected>不加密
    				    <OPTION value=1>WEP
    				    <OPTION value=2>WPA-PSK
    				    <OPTION value=3>WPA2-PSK
    				    <OPTION value=4>WPA-PSK/WPA2-PSK

			<TR id=SSID_ENC_CONFIG_LINE>
				<TD colspan=3 class=textCell>
				<div name=ssid_enc_mode id=ssid_enc_mode></div>

 
			<TR class='textCell'><td colspan=3>&nbsp;	
			
			<TR class='textCell'>
			<TD width=140 class=nameTextCell>				
			    <INPUT type=button class=button name=amend value=确定 onClick="add_edit_ssid(this.form, 1);" >
				<INPUT type=button class=button name=ipmacInput value=取消 onClick="init(document.frmSetup);">
			<TD width=30>
			<TD class='textCell' width=360>

	
		</table>

</FORM>
</table>
<script>pageTail(str_copyright);</script>
</BODY></HTML>






