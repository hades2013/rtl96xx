<HTML><HEAD>
<META NAME=Copyright CONTENT="Copyright (c) 2007 H3C Corporation. All Rights Reserved.">
<META http-equiv=Content-Type content=text/html;charset=GB2312>
<META HTTP-EQUIV=Pragma CONTENT=no-cache>
<TITLE>���߸߼�����</TITLE>

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
        alert(str+"����Ϊ����ֵ��������ż��ֵ!");
        P.select();
        return 1;
    }

	return 0;
}
/*
function submitF(f) 
{
	if(check_range_noempty(f.wlan_beacon_time,"��������������",40,1000))
	{
	    return ;
	}
	if(check_range_noempty(f.wlan_rts_threshold,"��������RTS��ֵ",256,2346))
	{
	    return ;
	}
	if(check_range_noempty(f.wlan_frag_threshold,"���������Ƭ��ֵ",256,2346))
	{
	    return ;
	}
	if(check_even(f.wlan_frag_threshold, "���������Ƭ��ֵ"))
	{
	    return ;
	}	
	if(check_range_noempty(f.wlan_dtim_time,"��������DTIM���",1,15))
	{
	    return ;
	}
	
	if((0 != f.wlan_tx_power_mode.value) && 
		check_range_noempty(f.wlan_tx_power,"���߷��书��",1,20))
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
        alert("����ѡ�������Ϊ��!");
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
        alert("�������ֵ������Χ!");
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
        alert("RTS��ֵ������Χ!");
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
        alert("������256-2346��ż��!");
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
        alert("DTIM ���������Χ!");
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
        alert("���书�ʳ�����Χ!");
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
	<TR><TD class=titleCell colSpan=3>��������߼�����</TD></TR>
		<td width=100% align=top>
		<table>
		
		<TR><TD class=textCell colSpan=3>��ҳ��������������ĸ߼����ԣ�����רҵ��Ա��ָ�������ã�����Ӱ������ʹ��Ч����</TD></TR>
		<tr class=spaceLine><td colspan=3 class=spaceLine>
		<tr class=spaceLine><td colspan=3 class=spaceLine>
		
		<TR class=textCell id=line_Beacon>
			<TD class=nameTextCell width=120>���������
			<td width=30>&nbsp;
	        <TD class=textCell>
	        	<INPUT maxLength=4 size=8 value=100 id=txt_wlBeaconInterval name=txt_wlBeaconInterval >&nbsp;����(��Χ:20~1024��ȱʡֵ:100)</TD></TR>

		<TR class=textCell id=line_RTS>
			<TD class=nameTextCell>RTS ��ֵ��</TD>
			<td width=30>
			<TD class=textCell>
				<INPUT maxLength=4 size=8 value=2347 id=txt_wlRTSThreshold name=txt_wlRTSThreshold >&nbsp;(��Χ:0~2347��ȱʡֵ:2347)</TD></TR>

		<TR class=textCell id=line_Frag>
			<TD class=nameTextCell>��Ƭ��ֵ��</TD>
			<td width=30>
			<TD class=textCell>
				<INPUT maxLength=4 size=8 value=2346 id=txt_wlFragThreshold name=txt_wlFragThreshold >&nbsp;(��Χ:256~2346����ż����ȱʡֵ:2346)</TD></TR>

		<TR class=textCell id=line_DTIM>
			<TD class=nameTextCell>DTIM �����</TD>
			<td width=30>
			<TD class=textCell>
				<INPUT maxLength=3 size=8 value=1 id=txt_wlDTIMInterval name=txt_wlDTIMInterval>&nbsp;(��Χ:1~255��ȱʡֵ:1)</TD></TR>

		<TR  id=line_TXPOWER>
			<TD class=nameTextCell>���书�ʣ�</TD>
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
			<TD class=nameTextCell >WMM��
			<td width=30>	
			<TD class=textCell>
			    <SELECT name=sel_wlWmm id=sel_wlWmm> 
			    <OPTION value=0 >����
			    <OPTION value=1>���� </OPTION>
			    </SELECT></TD></TR>	
				
		<tr><td><br></td>
    	<tr><td><b>˵��:</b></td> 
		<TR><TD class=textCell colSpan=3 style="font-size:12">(1)�������������������Beacon֡�ķ��ͼ����</TD></TR>
		<TR><TD class=textCell colSpan=3 style="font-size:12">(2)RTS��ֵ�������õ����ݰ���С��������ʱ����RTS֡��</TD></TR>
		<TR><TD class=textCell colSpan=3 style="font-size:12">(3)��Ƭ��ֵ�������õ����ݰ���С��������ʱ�����ݰ����з�Ƭ����</TD></TR>
		<TR><TD class=textCell colSpan=3 style="font-size:12">(4)DTIM��������������߿ͻ��˽����鲥���ĵ�ʱ������</TD></TR>
		<TR><TD class=textCell colSpan=3 style="font-size:12">(5)���书�����ڶ����߷��书�ʽ��е��ڣ�</TD></TR>
		<TR><TD class=textCell colSpan=3 style="font-size:12">(6)WMM�����Ƿ�������QoS���ܡ�</TD></TR>
        				    
		<tr class=spaceLine><td colspan=3 class=spaceLine>
		<tr class=spaceLine><td colspan=3 class=spaceLine>
		<!--<tr><td colspan=3 align=center><INPUT class=button type=button id=id_confirm value=Ӧ�� onclick=submitF(this.form)>-->
		<TR class='textCell'>
			<TD width=140 class=nameTextCell>				
			    <INPUT type=button class=button name=op_apply value=Ӧ�� onClick="submitF(document.frmSetup);" onKeyPress=Disable_Enter(event,this)>
				<INPUT type=button class=button name=op_cancle value=ȡ�� onClick="init(document.frmSetup);">
			<TD width=30>
			<TD class='textCell' width=360>
		</TABLE>
</FORM>
</table>
</BODY>
<script>pageTail(str_copyright);</script>
</HTML>






