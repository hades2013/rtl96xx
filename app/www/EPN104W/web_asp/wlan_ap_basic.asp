<HTML><HEAD>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<TITLE>无线基本设置</TITLE>

<link rel="stylesheet" href="style.css" type="text/css">
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=navigatortab.js></SCRIPT>
<SCRIPT language=JavaScript src=valid.js></SCRIPT>
<script language=JavaScript>
var str_copyright =<%CGI_GET_COPYRIGHT();%>
var PROD_MODEL = <%CGI_GET_MODEL();%>
</SCRIPT>
<script language=JavaScript>
pageTitle();

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var wlan_basic = new Array(<% CGI_WLAN_BASIC(); %>);

//var webVars = new Array( 'EPN104', '0', '0');
//var wlan_basic = new Array('1;5;2;13;24;1');
/*'wlan_en(0,1);wlan_mode(1:b,2:g,3:b+g,4:n,5:b+g+n);wlan_width(1:20M,2:40M);
	wlanChannel(1-13);wlanRx(0:auto,1-24.);'*/

var ssid_list=new Array();
var all_ssid_list=new Array();

var wlan_en = GetVal(wlan_basic, 0, 0);
var wlan_mode = GetVal(wlan_basic, 0, 1);
var wlan_width = GetVal(wlan_basic, 0, 2);
var wlanChannel = GetVal(wlan_basic, 0, 3);
var wlanRx = GetVal(wlan_basic, 0, 4);
var wlan_vlan = GetVal(wlan_basic, 0, 5);


//当以cmccadmin登录时login_level为1，当以admin登录时login_level为0

var login_level=1;

//var mode_old = getCfg("wlanMode");
var mode_old = '0';

//列出了当前用户可见的SSID表项，对于admin用户只可见SSID为xxx这个表项
/*
var ssid_list=new Array()  
*/
//列出了所有的SSID表项
/*
var all_ssid_list=new Array();  
*/

var search_val='';
var search_item='5';
var last_page=parseInt('1');
var max_row=MAXROWS;

var baklist = '';
var totallist = '';
var sortlist = '';

var turn = 0;
var initclass=["no","no"];

var pattern=/(.+);(.*);\d+/;

var selectflag=0;
var mf_curnum = 0;
var showflag=true;

function myhelp()
{
	popupHelp('help.htm#wlan_ap_basic');
}

function checked_num()
{
	var num = 0;
	var list = wlan_ap_list.document.getElementsByName("checkbox_list");
	var len = list.length;
	var i = 0;
	var num_checked = 0;
	var msg = '';
	for (i=0;i<len;i++)
	{
		if(list[i].checked == true)
			num++;
	}

	return num;
}


function fun_wlan_en(f)
{
    if(f.wlanEn.checked == false)
	{
		f.wlanMode.disabled=true;
		f.wlanWidth.disabled=true;
		f.wlanChannel.disabled=true;
		f.wlanTxRate.disabled=true;
		f.up_ser_vlan.disabled=true;
		document.getElementById("MACF_MODE1").disabled=true;
		document.getElementById("MACF_MODE2").disabled=true;
		document.getElementById("MACF_MODE3").disabled=true;
		document.getElementById("MACF_MODE4").disabled=true;
		document.getElementById("MACF_MODE5").disabled=true;
		//document.getElementById("SSID_num").disabled=true;
		//document.getElementById("essid").disabled=true;
		document.getElementById("op_del_ssid").disabled=true;
		f.wlanEn.checked = false;
	}
	else
	{
		f.wlanMode.disabled=false;
		f.wlanWidth.disabled=false;
		f.wlanChannel.disabled=false;
		f.wlanTxRate.disabled=false;
		f.up_ser_vlan.disabled=false;
		document.getElementById("MACF_MODE1").disabled=false;
		document.getElementById("MACF_MODE2").disabled=false;
		document.getElementById("MACF_MODE3").disabled=false;
		document.getElementById("MACF_MODE4").disabled=false;
		document.getElementById("MACF_MODE5").disabled=false;
		//document.getElementById("SSID_num").disabled=false;
		//document.getElementById("essid").disabled=false;
		document.getElementById("op_del_ssid").disabled=(checked_num()>0?false:true);
		fun_wlan_mode(f);
	}
	//enalbeElementByid("op_apply", f.wlanEn.checked);
}

function wlan_en_change(f)
{
    if(f.wlanEn.checked == false)
	{
    	cfg2Form(f);
    	f.wlanEn.checked = false;
		f.wlanMode.disabled=true;
		f.wlanWidth.disabled=true;
		f.wlanChannel.disabled=true;
		f.wlanTxRate.disabled=true;
		f.up_ser_vlan.disabled=true;
		document.getElementById("MACF_MODE1").disabled=true;
		document.getElementById("MACF_MODE2").disabled=true;
		document.getElementById("MACF_MODE3").disabled=true;
		document.getElementById("MACF_MODE4").disabled=true;
		document.getElementById("MACF_MODE5").disabled=true;
		//document.getElementById("SSID_num").disabled=true;
		//document.getElementById("essid").disabled=true;
		document.getElementById("op_del_ssid").disabled=true;
		f.wlanEn.checked = false;
	}
	else
	{
		f.wlanMode.disabled=false;
		f.wlanWidth.disabled=false;
		f.wlanChannel.disabled=false;
		f.wlanTxRate.disabled=false;
		f.up_ser_vlan.disabled=false;
		document.getElementById("MACF_MODE1").disabled=false;
		document.getElementById("MACF_MODE2").disabled=false;
		document.getElementById("MACF_MODE3").disabled=false;
		document.getElementById("MACF_MODE4").disabled=false;
		document.getElementById("MACF_MODE5").disabled=false;
		//document.getElementById("SSID_num").disabled=false;
		//document.getElementById("essid").disabled=false;
		document.getElementById("op_del_ssid").disabled=(checked_num()>0?false:true);

        fun_wlan_mode(f);
	}
	
	//enalbeElementByid("op_apply", f.wlanEn.checked);
	enalbe_checkbox_list(f.wlanEn.checked);
}

function wlan_width_change(f)
{
//	show_channel(f);
	show_tx_rate(f);
}

function wlan_mode_change(f)
{
	fun_wlan_mode(f);
//	show_channel(f);
	show_tx_rate(f);
}

function fun_wlan_mode(f)
{
	if((f.wlanMode[0].selected == true)
	|| (f.wlanMode[1].selected == true)
	|| (f.wlanMode[2].selected == true))
	{
		f.wlanWidth[0].selected = true;
		f.wlanWidth.disabled=true;
		document.getElementById("MACF_MODE2").disabled=true;
	}
	else
	{
		if(f.wlanWidth[1].value == getCfg("wlanWidth"))
		{
			f.wlanWidth[1].selected = true;
		}
		f.wlanWidth.disabled=false;
		document.getElementById("MACF_MODE2").disabled=false;
	}
}


function show_tx_rate(f)
{
    var i = 0;
    var txrate = getCfg("wlanTxRate");
    
	if(f.wlanMode[0].selected == true)
	{
		f.wlanTxRate.options.length=0;
		f.wlanTxRate.options[0]=new Option('AUTO','0',false,false);
		f.wlanTxRate.options[1]=new Option('1','1',false,false);
		f.wlanTxRate.options[2]=new Option('2','2',false,false);
		f.wlanTxRate.options[3]=new Option('5.5','3',false,false);
		f.wlanTxRate.options[4]=new Option('11','7',false,false);
	}
	else if(f.wlanMode[1].selected == true)
	{
		f.wlanTxRate.options.length=0;
		f.wlanTxRate.options[0]=new Option('AUTO','0',false,false);
		f.wlanTxRate.options[1]=new Option('6','4',false,false);
		f.wlanTxRate.options[2]=new Option('9','6',false,false);
		f.wlanTxRate.options[3]=new Option('12','8',false,false);
		f.wlanTxRate.options[4]=new Option('18','11',false,false);
		f.wlanTxRate.options[5]=new Option('24','13',false,false);
		f.wlanTxRate.options[6]=new Option('36','16',false,false);
		f.wlanTxRate.options[7]=new Option('48','19',false,false);
		f.wlanTxRate.options[8]=new Option('54','21',false,false);
	}
	else if(f.wlanMode[2].selected == true)
	{
		f.wlanTxRate.options.length=0;
		f.wlanTxRate.options[0]=new Option('AUTO','0',false,false);
		f.wlanTxRate.options[1]=new Option('1','1',false,false);
		f.wlanTxRate.options[2]=new Option('2','2',false,false);
		f.wlanTxRate.options[3]=new Option('5.5','3',false,false);
		f.wlanTxRate.options[4]=new Option('6','4',false,false);
		f.wlanTxRate.options[5]=new Option('9','6',false,false);
		f.wlanTxRate.options[6]=new Option('11','7',false,false);
		f.wlanTxRate.options[7]=new Option('12','8',false,false);
		f.wlanTxRate.options[8]=new Option('18','11',false,false);
		f.wlanTxRate.options[9]=new Option('24','13',false,false);
		f.wlanTxRate.options[10]=new Option('36','16',false,false);
		f.wlanTxRate.options[11]=new Option('48','19',false,false);
		f.wlanTxRate.options[12]=new Option('54','21',false,false);
	}
	else if(f.wlanMode[3].selected == true)
	{
		f.wlanTxRate.options.length=0;
		if(f.wlanWidth[0].selected == true)
		{
			f.wlanTxRate.options[0]=new Option('AUTO','0',false,false);
			f.wlanTxRate.options[1]=new Option('6.5','5',false,false);
			f.wlanTxRate.options[2]=new Option('13','9',false,false);
			f.wlanTxRate.options[3]=new Option('19.5','12',false,false);
			f.wlanTxRate.options[4]=new Option('26','14',false,false);
			f.wlanTxRate.options[5]=new Option('39','17',false,false);
			f.wlanTxRate.options[6]=new Option('52','20',false,false);
			f.wlanTxRate.options[7]=new Option('58.5','22',false,false);
			f.wlanTxRate.options[8]=new Option('65','24',false,false);
		}
		else
		{
			f.wlanTxRate.options[0]=new Option('AUTO','0',false,false);
			f.wlanTxRate.options[1]=new Option('13.5','10',false,false);
			f.wlanTxRate.options[2]=new Option('27','15',false,false);
			f.wlanTxRate.options[3]=new Option('40.5','18',false,false);
			f.wlanTxRate.options[4]=new Option('54','21',false,false);
			f.wlanTxRate.options[5]=new Option('81','25',false,false);
			f.wlanTxRate.options[6]=new Option('108','26',false,false);
			f.wlanTxRate.options[7]=new Option('121.5','27',false,false);
			f.wlanTxRate.options[8]=new Option('135','28',false,false);	
		}
	}
	else if(f.wlanMode[4].selected == true)
	{
		f.wlanTxRate.options.length=0;
		if(f.wlanWidth[0].selected == true)
		{
			f.wlanTxRate.options[0]=new Option('AUTO','0',false,false);
			f.wlanTxRate.options[1]=new Option('1','1',false,false);
			f.wlanTxRate.options[2]=new Option('2','2',false,false);
			f.wlanTxRate.options[3]=new Option('5.5','3',false,false);
			f.wlanTxRate.options[4]=new Option('6','4',false,false);
			f.wlanTxRate.options[5]=new Option('6.5','5',false,false);
			f.wlanTxRate.options[6]=new Option('9','6',false,false);
			f.wlanTxRate.options[7]=new Option('11','7',false,false);
			f.wlanTxRate.options[8]=new Option('12','8',false,false);
			f.wlanTxRate.options[9]=new Option('13','9',false,false);
			f.wlanTxRate.options[10]=new Option('18','11',false,false);
			f.wlanTxRate.options[11]=new Option('19.5','12',false,false);
			f.wlanTxRate.options[12]=new Option('24','13',false,false);
			f.wlanTxRate.options[13]=new Option('26','14',false,false);
			f.wlanTxRate.options[14]=new Option('36','16',false,false);
			f.wlanTxRate.options[15]=new Option('39','17',false,false);
			f.wlanTxRate.options[16]=new Option('48','19',false,false);
			f.wlanTxRate.options[17]=new Option('52','20',false,false);
			f.wlanTxRate.options[18]=new Option('54','21',false,false);
			f.wlanTxRate.options[19]=new Option('58.5','22',false,false);
			f.wlanTxRate.options[20]=new Option('65','24',false,false);
		}
		else
		{
			f.wlanTxRate.options[0]=new Option('AUTO','0',false,false);
			f.wlanTxRate.options[1]=new Option('1','1',false,false);
			f.wlanTxRate.options[2]=new Option('2','2',false,false);
			f.wlanTxRate.options[3]=new Option('5.5','3',false,false);
			f.wlanTxRate.options[4]=new Option('6','4',false,false);
			f.wlanTxRate.options[5]=new Option('9','6',false,false);
			f.wlanTxRate.options[6]=new Option('11','7',false,false);
			f.wlanTxRate.options[7]=new Option('12','8',false,false);
			f.wlanTxRate.options[8]=new Option('13.5','10',false,false);
			f.wlanTxRate.options[9]=new Option('18','11',false,false);
			f.wlanTxRate.options[10]=new Option('24','13',false,false);
			f.wlanTxRate.options[11]=new Option('27','15',false,false);
			f.wlanTxRate.options[12]=new Option('36','16',false,false);
			f.wlanTxRate.options[13]=new Option('40.5','18',false,false);
			f.wlanTxRate.options[14]=new Option('48','19',false,false);
			f.wlanTxRate.options[15]=new Option('54','21',false,false);
			f.wlanTxRate.options[16]=new Option('81','25',false,false);
			f.wlanTxRate.options[17]=new Option('108','26',false,false);
			f.wlanTxRate.options[18]=new Option('121.5','27',false,false);
			f.wlanTxRate.options[19]=new Option('135','28',false,false);
		}
	}

    
    for(i = 0; i < f.wlanTxRate.options.length; i++)
    {
        if(f.wlanTxRate.options[i].value == txrate)
        {
            f.wlanTxRate.options[i].selected = true;
            return;
        }
    }
    f.wlanTxRate.options[0].selected = true;
    return;
}


function check_wpa_tkip(f)
{
	var i=0;
	var count=ssid_list.length;
	var tmp, tt;
	var flag = 0;

	if(f.wlanMode.value != 4)
	{
		return 2;
	}

	for(i=0;i<count;i++)
	{
		tmp=ssid_list[i];		
		tt=tmp.split(';');
		if((tt[6] > 1) && (tt[8] != 2))
		{
			if(true == confirm("11n模式不支持TKIP加密，是否将其修改为AES加密?"))
			{
				return 1;
			}
			else
			{
			    return 0;
			}
		}
	}	

	return 2;
}


function enalbe_checkbox_list(action)
{	
	var list = wlan_ap_list.document.getElementsByName("checkbox_list");
	var len = list.length;
	var i = 0; 
	for (i=0;i<len;i++)
	{
		list[i].disabled = !action;
	}
}

function up_ser_vlan_check(P)
{
	if (0 != number_check(P))
    {
        return -1;
	}
    
    if (P.value < 1 || P.value > 4094)
    {
        alert("上行业务vlan:vlan超出范围!");
        return -1
    }
    return 0;
}

function enalbe_del_bt(action)
{
	document.getElementById("op_del_ssid").disabled=action==1?false:true;
}

function init_val(f)
{	
	f.wlanEn.checked = (wlan_en=='1'?true:false);
	f.wlanMode.value = wlan_mode;
	f.wlanWidth.value= wlan_width;
	f.wlanChannel.value=wlanChannel;
	f.up_ser_vlan.value = wlan_vlan;
}

function Init(f)
{
    cfg2Form(f);
//	show_channel(f);
	init_val(f);
    show_tx_rate(f);
	f.wlanTxRate.value=wlanRx;
    fun_wlan_en(f);
	
	if (max_row)
	{
	   document.getElementById("txtMaxRows").value = max_row;
	}
	
    if (baklist.length == 0)
    {
        //document.getElementById("op_selctall").disabled = true;
        //document.getElementById("table_output").disabled = true;
    }

	if (search_item && search_val)
	{
		document.getElementById("search_item").value=search_item;
		document.getElementById("search_val").value=decodeURIComponent(search_val);
	}
	
	if (last_page)
	{
		document.getElementById("txtCurPageIndex").value = last_page;
	}
	
	gopage();

	showflag=false;
	return;
}


function ApllyAll(f)
{
/*
	var param = '';
	var ret = 0;

	var f=document.forms[0];
    if(f.wlanEn.checked == false)
    {
    	cfg2Form(f);
    	f.wlanEn.checked = false;
		param = 'EditSsidNone';
    }
    //当以cmccadmin登录时
	else if(1 == login_level)
	{
		//无线工作模式由非n_only改为n_only时，如果之前的工作模式下各个SSID中有使用WEP或TKIP(以及混合模式)
		//那么给出告警，提示用户n_only不支持WEP和TKIP
		if (4 != mode_old && 4 == f.wlanMode.value)
		{
			for(i=0; i < all_ssid_list.length; i++)
			{
				tmp = all_ssid_list[i];
				tt = tmp.split(";");

				
				//如果加密方式是WEP
				if (tt[6] == 1)
				{
					alert("n_only不支持WEP加密，请先修改加密方式!");
					document.getElementById("wlanMode").focus();
					return;
				}
				
				//如果加密方式为WPA-PSK,WPA2-PSK或WPA-PSK/WPA2-PSK
				if (tt[6] == 2 || tt[6] == 3 || tt[6] == 4)
				{
					//如果加密协议为TKIP
					if(tt[8] == 1)
					{
						alert("n_only不支持TKIP加密，请先修改加密方式!");
						document.getElementById("wlanMode").focus();						
						return;
					}
				}
				
				//如果加密方式为WPA2-PSK或WPA-PSK/WPA2-PSK
				if (tt[6] == 3 || tt[6] == 4)
				{
					//如果加密协议为混合加密
					if(tt[8] == 3)
					{
						alert("n_only只支持AES加密，请先修改加密方式!");
						document.getElementById("wlanMode").focus();						
						return;
					}
				}				
			
			}
		}
		
		//无线工作模式由b-only,b-only或b+g，改为b+g+n时，如果之前的工作模式下各个SSID中有使用WEB或TKIP
		//那么给出告警，提示用户因为n_only不支持WEP和TKIP，所以保留WEP和TKIP的话设备会工作在低速模式
		if (5 == f.wlanMode.value)
		{		
			//提示信息只输出一次
			var flag_show = 0;
			var result = 0;			
			
			for(i=0; i < all_ssid_list.length; i++)
			{
				tmp = all_ssid_list[i];
				tt = tmp.split(";");
				
				//如果加密方式是WEP
				if (tt[6] == 1)
				{
					flag_show = 1;				
				}
				
				//如果加密方式为WPA-PSK,WPA2-PSK或WPA-PSK/WPA2-PSK,且加密协议为TKIP
				if ((tt[6]==2 || tt[6]==3 || tt[6]==4) && (tt[8] == 1))
				{
					flag_show = 1;	
				}		
			}

			if (1 == flag_show)
			{
				result = window.confirm("11n不支持WEP和TKIP，继续操作会使11n无线客户端最高工作在54Mbps。\n                您确信要修改吗?");
				if (0 == result)
				{
					return;
				}
			}
		}		
		param = 'EditSsidNone';		
	}
	//当以admin登录时		
	else if(0 == login_level)		
	{
		//无线工作模式由非n_only改为n_only时，如果之前的工作模式下各个SSID中有使用WEP或TKIP(以及混合模式)
		//那么给出告警，提示用户n_only不支持WEP和TKIP。要注意的是WEP或TKIP(以及混合模式)出现在admin可见的
		//SSID中和不可见的SSID中时，告警输出不同。
		if (4 != mode_old && 4 == f.wlanMode.value)
		{
			for(i=0; i < all_ssid_list.length; i++)
			{
				tmp = all_ssid_list[i];
				tt = tmp.split(";");

				
				//如果加密方式是WEP
				if (tt[6] == 1)
				{
					if (i < ssid_list.length)
					{
						alert("n_only不支持WEP加密，请先修改加密方式!");						
					}
					else
					{
						alert("依据系统管理员的设置，您无权进行本操作!");					
					}
					document.getElementById("wlanMode").focus();					
					return;
				}
				
				//如果加密方式为WPA-PSK,WPA2-PSK或WPA-PSK/WPA2-PSK
				if (tt[6] == 2 || tt[6] == 3 || tt[6] == 4)
				{
					//如果加密协议为TKIP
					if(tt[8] == 1)
					{
						if (i < ssid_list.length)
						{
							alert("n_only不支持TKIP加密，请先修改加密方式!");					
						}
						else
						{
							alert("依据系统管理员的设置，您无权进行本操作!");					
						}					
						document.getElementById("wlanMode").focus();						
						return;
					}
				}		
				//如果加密方式为WPA2-PSK或WPA-PSK/WPA2-PSK
				if (tt[6] == 3 || tt[6] == 4)
				{
					//如果加密协议为混合加密
					if(tt[8] == 3)
					{
						if (i < ssid_list.length)
						{
							alert("n_only只支持AES加密，请先修改加密方式!");					
						}
						else
						{
							alert("依据系统管理员的设置，您无权进行本操作!");					
						}					
						document.getElementById("wlanMode").focus();						
						return;
					}
				}				
			
			}
		}
		
		//无线工作模式由b-only,b-only或b+g，改为b+g+n时，如果之前的工作模式下admin用户可见的SSID中(仅有SSID名为H3C的这一个)
		//那么给出告警，提示用户因为n_only不支持WEP和TKIP，所以保留WEP和TKIP的话设备会工作在低速模式
		if (5 == f.wlanMode.value)
		{
			for(i=0; i < ssid_list.length; i++)
			{
				tmp = ssid_list[i];
				tt = tmp.split(";");
				
				//如果加密方式是WEP
				if (tt[6] == 1)
				{
					var result_wep = window.confirm("11n不支持WEP和TKIP，继续操作会使11n无线客户端最高工作在54Mbps。\n                您确信要修改吗?");
					if (0 == result_wep)
					{
						return;
					}
				}
				
				//如果加密方式为WPA-PSK,WPA2-PSK或WPA-PSK/WPA2-PSK,且加密协议为TKIP
				if ((tt[6]==2 || tt[6]==3 || tt[6]==4) && (tt[8] == 1))
				{				
					var result_tkip = window.confirm("11n不支持WEP和TKIP，继续操作会使11n无线客户端最高工作在54Mbps。\n                您确信要修改吗?");						
					if (0 == result_tkip)
					{
						return;
					}
				}		
			
			}
		
		}	

		param = 'EditSsidNone';	
	}	

	form2Cfg(f);
	Disable_button_byid("id_confirm");
	Disable_button_byid("id_cancel"); 
	//subForm(f,'/goform/aspForm',param,cPage);
	subForm(f,'goform/command', param, 'wlan_ap_basic.asp');

	return;
	*/
	if(0!=up_ser_vlan_check(f.up_ser_vlan))
	{
		return -1;
	}
	diffCfg("wlanEn", "wlanEn",  f.wlanEn.checked==true?1:0);
	diffCfg("wlanMode", "wlanMode",  f.wlanMode.value);
	diffCfg("wlanWidth", "wlanWidth",  f.wlanWidth.value);
	diffCfg("wlanChannel", "wlanChannel",  f.wlanChannel.value);
	diffCfg("wlanTxRate", "wlanTxRate",  f.wlanTxRate.value);
	diffCfg("up_ser_vlan", "up_ser_vlan",  f.up_ser_vlan.value);

	subForm(f,'goform/command', 'WLAN_AP_BASIC', 'wlan_ap_basic.asp');
}

function Del(f)
{
	var ssid_num = 0;
	var ssid_str = '';
	var del_list = 0;
	
				if(true==confirm("确定要删除所有选中表项吗?"))
			{
				var list = wlan_ap_list.document.getElementsByName("checkbox_list");
				var len = list.length;
				var i = 0;
				var del_coumt = 0;
				for (i=0;i<len;i++)
				{
					if(list[i].checked==true)
					{
						del_coumt++;
						ssid_str = list[i].value;
						ssid_num = parseInt(ssid_str.replace(/[^(0-9)]/g, ""))-1; 
						del_list |= 1<<ssid_num;
					}					
				}
				
				if(del_list=='')
				{
					alert("请选择要删除的表项！");
					return;
				}
				if(del_list>32)
				{
					alert("一次最多只能删除32个表项！");
					return;				
				}
				mf_curnum = (mf_curnum-del_coumt);
				//del_mac = del_mac.substr(0, del_mac.length-1); 
				diffCfg("param", "del_list", del_list);
				subForm(f,'goform/command', 'WLAN_AP_BASIC', 'wlan_ap_basic.asp');
			}
}

function getrefresh()
{
	//GURL(cPage+"?search_key="+encodeURIComponent(document.getElementById('search_val').value)+"&search_item="+document.getElementById('search_item').value+"&max_row="
	//+document.getElementById('txtMaxRows').value+"&last_page="+document.getElementById('txtCurPageIndex').value+"");
}



function go(maxrows, pageIndex, list)
{
	var pages = 0;
	
	if (selectflag)
	{
		selectall();
	}
	else
	{
		for (j=0;j<wlan_ap_list.checklist.length;j++)
		{	
		    if (wlan_ap_list.checklist[j]==1)
		    {
		    	wlan_ap_list.checklist[j]=0;
		    }
		}
	}
	
    list=sortlist;
	
    if (maxrows > baklist.length)
    {
        maxrows=baklist.length;
    }
    
	if (baklist.length%maxrows == 0)
	{
	    pages = parseInt(baklist.length/maxrows);
	}
	else
	{
	    pages = parseInt(baklist.length/maxrows) + 1;
	}
	
    if (pageIndex > pages)
    {
        pageIndex=pages;
		document.getElementById("txtCurPageIndex").value=pageIndex;
    }

	if (baklist.length == 0)
	{
		lblCurPage.innerHTML=1;
	    lblTotalPage.innerHTML=1;
	}
	else
	{
		lblCurPage.innerHTML=pageIndex;
		if (baklist.length%maxrows == 0)
		{
			lblTotalPage.innerHTML=parseInt(baklist.length/maxrows);
		}
		else 
		{
			lblTotalPage.innerHTML=parseInt(baklist.length/maxrows)+1;
		}
	}
    
    mf_curnum = lblRecordCount.innerHTML=baklist.length;
    
    document.getElementById("wlan_ap_list").style.height=22+21*(maxrows);
   
    newlist=list.slice(maxrows*(pageIndex-1),maxrows*pageIndex);
    list=newlist; 
	
    wlan_ap_list.show_wlan_ap_list(list);
	enalbe_checkbox_list(document.frmSetup.wlanEn.checked);
}

function go_specific_page(type)
{
	var pageIndex = document.getElementById("txtCurPageIndex").value;
	var maxrows = document.getElementById("txtMaxRows").value;
	var pages = 0;
	
	if (maxrows == '' || 0 == maxrows)
	{
		return;
	}
    else if(!/(^[0-9]{1,3}$)/.test(maxrows))
    {	
   		return;	
    }

    if (pageIndex == '' || 0 == pageIndex)
    {
        return;
    }
    else if(!/(^[0-9]{1,3}$)/.test(pageIndex))
    {
        return;
    }
	pageIndex = parseInt(pageIndex);/*trip the '0' if exist*/
	maxrows = parseInt(maxrows);    /*trip the '0' if exist*/
	
	
	if (baklist.length%maxrows == 0)
	{
	    pages = parseInt(baklist.length/maxrows);
	}
	else
	{
	    pages = parseInt(baklist.length/maxrows) + 1;
	}

	switch(type)
	{
		case 1:  // first page
			pageIndex = 1;
			break;		
		case 2:  //previous
			if(pages==0)
				pageIndex = 1;
			else	
				pageIndex = pageIndex==1?pages:(pageIndex-1);//1:(pageIndex-1);
			break;		
		case 3:  //next
			if(pages==0)
				pageIndex = 1;
			else
				pageIndex = pageIndex==pages?1:(pageIndex+1);//pages:(pageIndex+1)
			break;		
		case 4: //last
			pageIndex = pages==0?1:pages;
			break;
		case 5: //all
			pageIndex = 1;
			maxrows = baklist.length;
			break;	
	}

	document.getElementById("txtCurPageIndex").value = pageIndex;
	document.getElementById("txtMaxRows").value = maxrows;
	go(maxrows, pageIndex, wlan_ap_list.ssid_list);
}

function gopage()
{
    var pageIndex = document.getElementById("txtCurPageIndex").value;
	var maxrows=document.getElementById("txtMaxRows").value;
	
	if (maxrows == '' || 0 == maxrows)
	{
		return;
	}
    else if(!/(^[0-9]{1,3}$)/.test(maxrows))
    {	
   		return;	
    }

    if (pageIndex == '' || 0 == pageIndex)
    {
        return;
    }
    else if(!/(^[0-9]{1,3}$)/.test(pageIndex))
    {
        return;
    }
	pageIndex = parseInt(pageIndex);/*trip the '0' if exist*/
	maxrows = parseInt(maxrows);    /*trip the '0' if exist*/

	go(maxrows, pageIndex, wlan_ap_list.ssid_list);
    wlan_ap_list.ssid_list=newlist;
}

function search_go(f, pattern)
{
	var search_pattern = parseInt(f.search_item.value);  
	var key = f.search_val.value;
	var key_tmp = key;
	var search_mac_list = new Array();
	var len = baklist.length;
	var list = baklist;
	var i = 0;
	var find_num = 0;
	var baklist_tmp = baklist;
	var sortlist_tmp = sortlist;
	var pageIndex = 1;
	var maxrows = SEARCH_MAXROWS;
	var value = '';
	
	if(key=='')
		return;
	for (i=0; i<len; i++)
    {	
		value = GetVal(list, i, search_pattern);
		if(key == value)
		{
			search_mac_list.push(list[i]);	
			find_num++;
		}
			
	}
	if(0==find_num)
	{
		alert('根据关键字("'+key_tmp +'")没有找到相应的表项！');
	}

	document.getElementById("txtCurPageIndex").value = pageIndex;
	document.getElementById("txtMaxRows").value = maxrows;
	sortlist = baklist = search_mac_list;
	go(maxrows, pageIndex, search_mac_list);
	baklist = baklist_tmp;
	sortlist = sortlist_tmp;
}

</SCRIPT>

</HEAD>
<BODY  bgColor=#ffffff leftMargin=0 topMargin=0 MARGINWIDTH=0 MARGINHEIGHT=0 BORDER=0 onLoad=Init(document.frmSetup)>

<script>pageHead(webVars,PROD_MODEL);</script>
<SCRIPT language=JavaScript>buildNavigatorTab(wlan_ap, 1)</SCRIPT>
<table width="100%">  
	<form name=frmSetup action='goform/command' method=post>
		<TR><TD colspan=3 class=titleCell>无线网络基本设置</TD></TR>
		<td width=540 align=top>
	<table class=globtable>
		<tr><td colSpan=4>&nbsp; 
		<tr>
		<input type=hidden name=LANIP>
		<input type=hidden name=LANMASK>
		<input type=hidden name=CMD>
		<input type=hidden name=GO>
		<input type=hidden name=param>
	<td width=30>&nbsp;<td width=500 valign=top>
	<TABLE cellSpacing=0 cellPadding=2 border=0 width=100%>
			<TR class=spaceLine><td  colSpan=3 align=center>
			<TR id=line_enable>
				<TD width='45%' class=nameTextCell>启用无线网络：
				<td  class='textCell' width=500><input name=wlanEn  type=checkbox onclick=wlan_en_change(frmSetup)>
				<TD width=30>&nbsp;</TD></tr>
			<tr class=spaceLine><td colspan=4 class=spaceLine>
			<TR id =MACF_MODE1>
				<TD width=250 class=nameTextCell>无线网络模式:&nbsp;&nbsp;&nbsp;
				<TD class='textCell'  width=300>
					<SELECT name=wlanMode id=wlanMode onchange=wlan_mode_change(frmSetup)>
							<OPTION value=1>b-only
							<OPTION value=2>g-only
							<OPTION value=3>b+g
							<OPTION value=4>n-only
							<OPTION value=5 selected>b+g+n
							</SELECT>
				<TD width=30>&nbsp;</TD></tr>
			<tr class=spaceLine><td colspan=4 class=spaceLine>

			<TR id =MACF_MODE2>
					<TD width=250 class=nameTextCell>无线网络信道频宽:&nbsp;&nbsp;&nbsp;
					<TD class='textCell'  width=300>
						    <SELECT name=wlanWidth onchange=wlan_width_change(frmSetup)>
							<OPTION value=1>20M
							<OPTION value=2>40M
							</SELECT>&nbsp;Hz
					<TD width=30>&nbsp;</TD></tr>
			<tr class=spaceLine><td colspan=4 class=spaceLine>
			<TR id =MACF_MODE3>
					<TD width=250 class=nameTextCell>无线信道:&nbsp;&nbsp;&nbsp;
   					<TD class=textCell width=300>
    					 <SELECT name=wlanChannel > 
								<OPTION value=0>AUTO </OPTION> 
								<OPTION value=1>1 - 2.412GHz </OPTION> 
								<OPTION value=2>2 - 2.417GHz </OPTION> 
								<OPTION value=3>3 - 2.422GHz </OPTION> 
								<OPTION value=4>4 - 2.427GHz </OPTION> 
								<OPTION value=5>5 - 2.432GHz </OPTION> 
								<OPTION value=6>6 - 2.437GHz </OPTION> 
								<OPTION value=7>7 - 2.442GHz </OPTION> 
								<OPTION value=8>8 - 2.447GHz </OPTION> 
								<OPTION value=9>9 - 2.452GHz </OPTION> 
								<OPTION value=10>10 - 2.457GHz</OPTION> 
								<OPTION value=11>11 - 2.462GHz</OPTION> 
								<OPTION value=12>12 - 2.467GHz</OPTION> 
								<OPTION value=13>13 - 2.472GHz</OPTION>
    					    </SELECT></TD></TR>	
			<TR id =MACF_MODE4>
					<TD width=250 class=nameTextCell>无线发送速率:&nbsp;&nbsp;&nbsp;
   					<TD class=textCell width=300>
    					    <SELECT name=wlanTxRate > 
    					    </SELECT>&nbsp;Mbps</TD>	
					<TD width=30>&nbsp;</TD></TR>		
			<TR id =MACF_MODE5>
					<TD width=250 class=nameTextCell>上行业务vlan:&nbsp;&nbsp;&nbsp;
   					<TD class=textCell width=300>
    					   <INPUT maxLength=4 size=5 id=up_ser_vlan name=up_ser_vlan>&nbsp;(范围:1-4094，缺省值:4094)</TD></TR>	
					<TD width=30>&nbsp;</TD>
		</tr>			
    </table>

	</table>
	<table>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>		
			<tr class=textCell>
				<TD width=150>
					<input type=button class=button name=op_apply id=op_apply value=确定 onClick="ApllyAll(this.form);">
					<input type=button class=button name=op_cancel id=op_cancel value=取消 onClick="Init(frmSetup);">
				</TD>				
		</table>
	<table>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			
			<TR><TD class=titleCell colSpan=3>无线网络SSID列表</TD></TR>
			
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			
			<TR id=search_index class="textCell">
					<TD colSpan=3 align="left">按关键字过滤：
						<select name=search_item id=search_item >
							<option value=1>SSID编号
						</select>
						关键字：
						<input type=text name="search_val" id="search_val" size=20 maxlength=63  onkeydown="if (event.keyCode==13) search_go(frmSetup,pattern)">
						<INPUT type=button name=search_start id=search_start class=button value=查询 onClick="search_go(frmSetup, pattern);">
						<INPUT type=button name=search_stop id=search_stop class=middlebutton value=显示全部  onClick="go_specific_page(5);"></TD>
						

	<TR><td width=100% colSpan=3 align=center>
		<iframe name="wlan_ap_list" id="wlan_ap_list" src="wlan_ap_ssid_list.asp" width=100% height=0 align=left scrolling=no ></iframe>
		<tr><td width=30>&nbsp;<td width=100%>
		<table border="0" cellspacing="0" cellpadding="0" width="100%">
		<tr class="textCell">
			<td noWrap="noWrap" align="right">
				<span id="lblSecordHead">第</span>
				<span id="lblCurPage">1</span>
				<span >页/共</span>
				<span id="lblTotalPage">1</span>
				<span id="lblFirstHead">页 共</span>
				<span id="lblRecordCount">0</span>
				<span id="lblPageHead">条记录 每页</span>
				<input name="txtMaxRows" type="text" id="txtMaxRows" style="BORDER-TOP-WIDTH:1px; BORDER-LEFT-WIDTH:1px; IME-MODE:disabled; BORDER-BOTTOM-WIDTH:1px; HEIGHT:18px; TEXT-ALIGN:center; BORDER-RIGHT-WIDTH:1px" maxlength="3" size="4" value="8" validate="+INT" onkeydown="if (event.keyCode==13) gopage()"/>
				<span id="lblPerPage">行&nbsp;</span>
			</td>
			<td align="right" style="width:5px" valign="middle"><IMG name="imgFirst" id="imgFirst" title=首页 src="images/First-D.gif" onclick=go_specific_page(1)></td><td width=1>&nbsp;</td>
			<td align="right"  style="width:5px" valign="middle"><IMG name="imgPrev" id="imgPrev" title=上一页 src="images/Previous-D.gif" onclick=go_specific_page(2)></td><td width=1>&nbsp;</td>
			<td  align="right"  style="width:5px" valign="middle"><input name="txtCurPageIndex" type="text" id="txtCurPageIndex" style="BORDER-TOP-WIDTH:1px; BORDER-LEFT-WIDTH:1px; IME-MODE:disabled; BORDER-BOTTOM-WIDTH:1px; HEIGHT:18px; TEXT-ALIGN:center; BORDER-RIGHT-WIDTH:1px" maxlength="3" size="4" value="1" validate="+INT" 
			onkeydown="if (event.keyCode==13) gopage()" /></td><td width=1>&nbsp;</td>
			<td  align="right"  style="width:5px" valign="middle"><input type="button" name="btnGoPage" value="Go" id="btnGoPage" onclick="gopage()"></td><td width=1>&nbsp;</td>
			<td  align="right"  style="width:5px" valign="middle"> <IMG name="imgNext" id="imgNext" title=下一页 src="images/Next.gif" onclick=go_specific_page(3)></td><td width=1>&nbsp;</td>
			<td  align="right" style="width:5px" valign="middle"><IMG name="imgLast" id="imgLast" title=末页 src="images/Last.gif" onclick=go_specific_page(4)></td>
			</tr>
		</table>
		<TR class='textCell'><td colSpan=3>
	
	</TABLE>
	<table border=0>
          <TR>
			<TD>
			<INPUT type=button name=op_confirm id=op_del_ssid class=button value=删除 onClick="Del(this.form);">
    </table>
</form>
</table>
<script>pageTail(str_copyright);</script>
</body></html>
