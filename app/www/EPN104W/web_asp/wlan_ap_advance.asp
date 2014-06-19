<HTML><HEAD>
<META NAME=Copyright CONTENT="Copyright (c) 2007 H3C Corporation. All Rights Reserved.">
<META http-equiv=Content-Type content=text/html;charset=GB2312>
<META HTTP-EQUIV=Pragma CONTENT=no-cache>
<TITLE>无线高级设置</TITLE>

<link rel="stylesheet" href="style.css" type="text/css">
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=navigatortab.js></SCRIPT>
<SCRIPT language=JavaScript src=valid.js></SCRIPT>
<script language=JavaScript>
var str_copyright =<%CGI_GET_COPYRIGHT();%>
var PROD_MODEL = <%CGI_GET_MODEL();%>
</SCRIPT>
<SCRIPT language=JavaScript>
pageTitle();

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var wlan_ap_advance = new Array( <% CGI_WLAN_AP_ADVANCE(); %>); 
//var webVars = new Array( 'EPN104', '0', '0');
//var wlan_ap_advance = new Array('1;3;50;2300;2100;15;100;1');
 /*wlan_en(0-1);wlanMode(0-5);wlBeaconInterval;wlRTSThreshold;wlFragThreshold;wlDTIMInterval;
	wlTxPower;wlWmm(0-1)*/

var wlan_en = GetVal(wlan_ap_advance, 0, 0);
var wlanmode = GetVal(wlan_ap_advance, 0, 1);
var wlBeaconInterval = GetVal(wlan_ap_advance, 0, 2);
var wlRTSThreshold = GetVal(wlan_ap_advance, 0, 3);
var wlFragThreshold = GetVal(wlan_ap_advance, 0, 4);
var wlDTIMInterval = GetVal(wlan_ap_advance, 0, 5);
var wlTxPower = GetVal(wlan_ap_advance, 0, 6);
var wlWmm = GetVal(wlan_ap_advance, 0, 7);
	
function init(f)
{
	f.txt_wlBeaconInterval.value = wlBeaconInterval;
	f.txt_wlRTSThreshold.value = wlRTSThreshold;
	f.txt_wlFragThreshold.value = wlFragThreshold;
	f.txt_wlDTIMInterval.value = wlDTIMInterval;
	f.txt_wlTcPower.value = wlTxPower;
	f.sel_wlWmm.value = wlWmm;
	
	if(wlan_en=='1')
	{
		if((wlanmode == 4) || (wlanmode == 5))
		{
			f.txt_wlRTSThreshold.disabled=true;
			document.getElementById("line_RTS").disabled=true;
			f.txt_wlFragThreshold.disabled=true;
			document.getElementById("line_Frag").disabled=true;
		}
		else
		{
			f.txt_wlRTSThreshold.disabled=false;
			document.getElementById("line_RTS").disabled=false;
			f.txt_wlFragThreshold.disabled=false;
			document.getElementById("line_Frag").disabled=false;
		}
		f.txt_wlBeaconInterval.disabled = false;
		f.txt_wlDTIMInterval.disabled = false;
		f.txt_wlTcPower.disabled = false;
		f.sel_wlWmm.disabled = false;
		f.op_apply.disabled = false;
	}
	else
	{	
		f.txt_wlBeaconInterval. disabled = true;
		f.txt_wlRTSThreshold.disabled = true;
		f.txt_wlFragThreshold.disabled = true;
		f.txt_wlDTIMInterval.disabled = true;
		f.txt_wlTcPower.disabled = true;
		f.sel_wlWmm.disabled = true;
		f.op_apply.disabled = true;
	}
}

function myhelp()
{
	popupHelp('help.htm#wlan_ap_advance');
}

function check_even(P, str)
{
    if (1 == P.value%2)
    {
        alert(str+"不能为奇数值，请输入偶数值!");
        P.select();
        return 1;
    }

	return 0;
}
/*
function submitF(f) 
{
	if(check_range_noempty(f.wlan_beacon_time,"无线网络点亮间隔",40,1000))
	{
	    return ;
	}
	if(check_range_noempty(f.wlan_rts_threshold,"无线网络RTS阈值",256,2346))
	{
	    return ;
	}
	if(check_range_noempty(f.wlan_frag_threshold,"无线网络分片阈值",256,2346))
	{
	    return ;
	}
	if(check_even(f.wlan_frag_threshold, "无线网络分片阈值"))
	{
	    return ;
	}	
	if(check_range_noempty(f.wlan_dtim_time,"无线网络DTIM间隔",1,15))
	{
	    return ;
	}
	
	if((0 != f.wlan_tx_power_mode.value) && 
		check_range_noempty(f.wlan_tx_power,"无线发射功率",1,20))
	{
	    return ;
	}
	
	alert("1111")
	form2Cfg(f);
	Disable_button_byid("id_confirm");
	Disable_button_byid("id_cancel"); 
	//subForm(f,'/goform/aspForm','WAN',cPage);
	subForm(f,'goform/command', 'WAN', 'wlan_ap_advance.asp');
}
*/



function valid_check(f)
{
    if ((f.txt_wlBeaconInterval.value == "") || (f.txt_wlRTSThreshold.value == "")
        || (f.txt_wlFragThreshold.value == "") || (f.txt_wlDTIMInterval.value == "") || (f.txt_wlTcPower.value == ""))
    {
        alert("所有选项均不能为空!");
        return -1;
    }

    var ret;
    ret = Beacon_Interval_Check(f.txt_wlBeaconInterval);
    ret += RTS_Threshold_Check(f.txt_wlRTSThreshold);
    ret += Frag_Threshold_Check(f.txt_wlFragThreshold);
    ret += DTIM_Interval_Check(f.txt_wlDTIMInterval);
    //ret+= TXPOWER_Check(f.txt_wlTcPower);

    return ret;        
}

function Beacon_Interval_Check(P)
{
    if (0 != number_check(P))
    {
        return -1;
    }
    
    if (P.value < 20 || P.value > 1024)
    {
        alert("点亮间隔值超出范围!");
        return -1;
    }
    return 0;
}

function RTS_Threshold_Check(P)
{
	if (0 != number_check(P))
    {
        return -1;
	}
    
    if (P.value < 0 || P.value > 2347)
    {
        alert("RTS阈值超出范围!");
        return -1;
    }
    return 0;
}

function Frag_Threshold_Check(P)
{
	if (0 != number_check(P))
    {
        return -1;
	}
    
    if (P.value < 256 || P.value > 2346 || ((P.value % 2) != 0))
    {
        alert("请输入256-2346的偶数!");
        return -1;
    }
    return 0;
}

function DTIM_Interval_Check(P)
{
	if (0 != number_check(P))
    {
        return -1;
	}
    
    if (P.value < 1 || P.value > 255)
    {
        alert("DTIM 间隔超出范围!");
        return -1
    }
    return 0;
}

function TXPOWER_Check(P)
{
	if (0 != number_check(P))
    {
        return -1;
	}
    
    if (P.value < 1 || P.value > 20)
    {
        alert("发射功率超出范围!");
        return -1
    }
    return 0;
}

function Enable_Check(f)
{                   
    if (wlan_en == 0)
    {
        document.getElementById("txt_wlBeaconInterval").disabled = true;
        document.getElementById("txt_wlRTSThreshold").disabled    = true;
        document.getElementById("txt_wlFragThreshold").disabled   = true;
		document.getElementById("txt_wlTcPower").disabled   = true;	
		document.getElementById("sel_wlWmm").disabled   = true;	
		document.getElementById("line_Wmm").disabled   = true;		
        document.getElementById("txt_wlDTIMInterval").disabled   = true;
        document.getElementById("line_TXPOWER").disabled     = true;
       // document.getElementById("sel_wlTxRate").disabled = true;
        
        document.getElementById("txt_wlBeaconInterval").disabled  = true;
        document.getElementById("txt_wlRTSThreshold").disabled    = true;
        document.getElementById("txt_wlFragThreshold").disabled   = true;
        document.getElementById("txt_wlDTIMInterval").disabled    = true;
    }
    else
    {
        document.getElementById("txt_wlBeaconInterval").disabled = false;
        document.getElementById("txt_wlRTSThreshold").disabled    = false;
		document.getElementById("txt_wlTcPower").disabled   = false;
		document.getElementById("sel_wlWmm").disabled   = false;
		document.getElementById("line_Wmm").disabled   = false;
        document.getElementById("txt_wlFragThreshold").disabled   = false;
        document.getElementById("txt_wlDTIMInterval").disabled   = false;
        document.getElementById("line_TXPOWER").disabled     = false;
        //document.getElementById("sel_wlTxRate").disabled = false;
        
        document.getElementById("txt_wlBeaconInterval").disabled  = false;
        document.getElementById("txt_wlRTSThreshold").disabled    = false;
        document.getElementById("txt_wlFragThreshold").disabled   = false;
        document.getElementById("txt_wlDTIMInterval").disabled    = false;
    }
}

function submitF(f) 
{
	var wl_beaconinterval = wlBeaconInterval;
	var wl_rtsthreshold = wlRTSThreshold;
	var wl_fragthreshold = wlFragThreshold;
	var wl_dtiminterval = wlDTIMInterval;
	var wl_txpower = wlTxPower;
	var wl_wmm = wlWmm;
	
    if (0 != valid_check(f))
    {
        return ;
    }
	
	wl_beaconinterval = f.txt_wlBeaconInterval.value;
	wl_rtsthreshold = f.txt_wlRTSThreshold.value;
	wl_fragthreshold = f.txt_wlFragThreshold.value;
	wl_dtiminterval = f.txt_wlDTIMInterval.value;
	wl_txpower = f.txt_wlTcPower.value;
	wl_wmm = f.sel_wlWmm.value;
	
	diffCfg("txt_wlBeaconInterval", "wl_beaconinterval", wl_beaconinterval);
	diffCfg("txt_wlRTSThreshold", "wl_rtsthreshold", wl_rtsthreshold);
	diffCfg("txt_wlFragThreshold", "wl_fragthreshold", wl_fragthreshold);
	diffCfg("txt_wlDTIMInterval", "wl_dtiminterval", wl_dtiminterval);
	diffCfg("txt_wlTcPower", "wl_txpower", wl_txpower);
	diffCfg("sel_wlWmm", "wl_wmm", wl_wmm);
	
	subForm(f,'goform/command', 'WLAN_AP_ADVANCE', 'wlan_ap_advance.asp');
}


</SCRIPT></head>
<body bgcolor=#ffffff leftMargin=0 topMargin=0 MARGINWIDTH=0 MARGINHEIGHT=0 border=0 onload=init(document.frmSetup)>
<script>pageHead(webVars,PROD_MODEL);</script>
<SCRIPT language=JavaScript>buildNavigatorTab(wlan_ap, 4)</SCRIPT>
<table width=100%>
<form name=frmSetup method=POST action='goform/command'>
	<TR><TD class=titleCell colSpan=3>无线网络高级设置</TD></TR>
		<td width=100% align=top>
		<table>
		
		<TR><TD class=textCell colSpan=3>本页面设置无线网络的高级属性，请在专业人员的指导下设置，以免影响网络使用效果。</TD></TR>
		<tr class=spaceLine><td colspan=3 class=spaceLine>
		<tr class=spaceLine><td colspan=3 class=spaceLine>
		
		<TR class=textCell id=line_Beacon>
			<TD class=nameTextCell width=120>点亮间隔：
			<td width=30>&nbsp;
	        <TD class=textCell>
	        	<INPUT maxLength=4 size=8 value=100 id=txt_wlBeaconInterval name=txt_wlBeaconInterval >&nbsp;毫秒(范围:20~1024，缺省值:100)</TD></TR>

		<TR class=textCell id=line_RTS>
			<TD class=nameTextCell>RTS 阈值：</TD>
			<td width=30>
			<TD class=textCell>
				<INPUT maxLength=4 size=8 value=2347 id=txt_wlRTSThreshold name=txt_wlRTSThreshold >&nbsp;(范围:0~2347，缺省值:2347)</TD></TR>

		<TR class=textCell id=line_Frag>
			<TD class=nameTextCell>分片阈值：</TD>
			<td width=30>
			<TD class=textCell>
				<INPUT maxLength=4 size=8 value=2346 id=txt_wlFragThreshold name=txt_wlFragThreshold >&nbsp;(范围:256~2346，仅偶数，缺省值:2346)</TD></TR>

		<TR class=textCell id=line_DTIM>
			<TD class=nameTextCell>DTIM 间隔：</TD>
			<td width=30>
			<TD class=textCell>
				<INPUT maxLength=3 size=8 value=1 id=txt_wlDTIMInterval name=txt_wlDTIMInterval>&nbsp;(范围:1~255，缺省值:1)</TD></TR>

		<TR  id=line_TXPOWER>
			<TD class=nameTextCell>发射功率：</TD>
			<td width=30>
			<TD class=textCell>
				<SELECT name=txt_wlTcPower id=txt_wlTcPower> 
					<OPTION value=100 >100%</OPTION>
					<OPTION value=70 >70%</OPTION>
					<OPTION value=50 >50%</OPTION>
					<OPTION value=35 >35%</OPTION>
					<OPTION value=10 >10%</OPTION>
			    </SELECT>
			    </TD></TR>	

		<TR class=textCell id=line_Wmm>
			<TD class=nameTextCell >WMM：
			<td width=30>	
			<TD class=textCell>
			    <SELECT name=sel_wlWmm id=sel_wlWmm> 
			    <OPTION value=0 >禁用
			    <OPTION value=1>启用 </OPTION>
			    </SELECT></TD></TR>	
				
		<tr><td><br></td>
    	<tr><td><b>说明:</b></td> 
		<TR><TD class=textCell colSpan=3 style="font-size:12">(1)点亮间隔用于配置无线Beacon帧的发送间隔；</TD></TR>
		<TR><TD class=textCell colSpan=3 style="font-size:12">(2)RTS阈值用于配置当数据包大小超过多少时发送RTS帧；</TD></TR>
		<TR><TD class=textCell colSpan=3 style="font-size:12">(3)分片阈值用于配置当数据包大小超过多少时对数据包进行分片处理；</TD></TR>
		<TR><TD class=textCell colSpan=3 style="font-size:12">(4)DTIM间隔用于配置无线客户端接收组播报文的时间间隔；</TD></TR>
		<TR><TD class=textCell colSpan=3 style="font-size:12">(5)发射功率用于对无线发射功率进行调节；</TD></TR>
		<TR><TD class=textCell colSpan=3 style="font-size:12">(6)WMM决定是否开启无线QoS功能。</TD></TR>
        				    
		<tr class=spaceLine><td colspan=3 class=spaceLine>
		<tr class=spaceLine><td colspan=3 class=spaceLine>
		<!--<tr><td colspan=3 align=center><INPUT class=button type=button id=id_confirm value=应用 onclick=submitF(this.form)>-->
		<TR class='textCell'>
			<TD width=140 class=nameTextCell>				
			    <INPUT type=button class=button name=op_apply value=应用 onClick="submitF(document.frmSetup);" onKeyPress=Disable_Enter(event,this)>
				<INPUT type=button class=button name=op_cancle value=取消 onClick="init(document.frmSetup);">
			<TD width=30>
			<TD class='textCell' width=360>
		</TABLE>
</FORM>
</table>
</BODY>
<script>pageTail(str_copyright);</script>
</HTML>






