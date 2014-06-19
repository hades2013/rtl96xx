<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<META HTTP-EQUIV=Pragma CONTENT=no-cache>
<TITLE>端口配置</TITLE>
<LINK href=style.css type=text/css rel=stylesheet>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
<script language=javascript src=script/valid.js></script>
<script language=javascript src=script/pageview.js></script>
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
env['sys.time'] = '946684845';
env['sys.linkup'] = '0';
env['sys.linkoff'] = '0';
env['sys.busy'] = '0';
env['sys.ipc'] = '0';
env['sys.vlan'] = '0';
env['sys.unsaved'] = webVars[1];
env['sys.feature'] = 'vlanpool;mme_drop_ports;slave_access_mgmt;bcmp;';
env['web.page'] = current_page();
env['sys.parent_name'] = '端口设置';
//pageRedirectCheck(env);
</script>
<SCRIPT language="javascript" type="text/javascript">
 
//portmask, portdes, enable  speed duplex  fc IsolateEn  midx  pri
//var Portinfo = new Array('4','Cable1','1','2','1','1','0','0','0');
var Portinfo = new Array(<% CGI_GET_SINGLE_PORT(); %>);
var numPorts = 4;
var pMsk = Portinfo[0];
var pno = 0;

function getPortdescription()
{
	return Portinfo[1];
}

function getMDIXSelection()
{
	return parseInt(Portinfo[7]);
}

function getIsolateSelection()
{
	return parseInt(Portinfo[6]);
}

function getSpeedSelection()
{
    switch(Portinfo[3])
    {
        case "0":
            return 0;
        case "1":
            return 1;
        case "2":
            return 2;
        case "3":
            return 3;
        default:
            return 0;
    }
}
function getStormRateSelection(pno)
{
    switch(stormRate[pno-1])
    {
        case "5":
            return 0;
        case "10":
            return 1;
        case "20":
            return 2;
        case "100":
            return 3;
        default:
            return 3;
    }
}
function getDuplexSelection()
{
    switch(Portinfo[4])
    {
        case "0":
            return 0;
        case "1":
            return 1;
        case "2":
            return 2;
        default:
            return 0;
    }
    
}
function getPhyStatusSelection()
{
    switch(Portinfo[2])
    {
        case "0":
            return 0;
        case "1":
            return 1;
        default:
            return 1;
    }
}
function getPortPrioSelection()
{
    switch(Portinfo[8])
    {
        case "0":
            return 0;
        case "1":
            return 1;
        case "2":
            return 2;
        case "3":
            return 3;
        case "4":
            return 4;
        case "5":
            return 5;
        case "6":
            return 6;
        case "7":
            return 7;
        default:
            return 0;
    }
}
function getFlowContrlSelection()
{

    switch(Portinfo[5])
    {
        case "0":
            return 0;
        case "1":
            return 1;
        default:
            return 0;
    }
}
function initPortValue()
{
    for(var i=0 ; i< numPorts; i++)
    {
        if(pMsk & (1<<i))
        {
            if(pno == 0)
            {
                pno = i+1;
            }
            else
            {
            	/*set more than one port */
                pno = 0xff;
            }
        }
    }
	
	document.forms[0].portmask.value = pMsk;

    if((pno == 0xff)||(pno == 0))
    {
	 	document.forms[0].des.value = "port description";
        document.forms[0].speed.selectedIndex = 3;
        document.forms[0].duplex.selectedIndex = 1;
        document.forms[0].admin.selectedIndex = 1;
        document.forms[0].fc.selectedIndex = 1;
        document.forms[0].pri.selectedIndex = 0;
        document.forms[0].MDIX.selectedIndex = 2;
		document.forms[0].isolate.selectedIndex = 1;
    }
    else
    {
		//document.getElementById("R30").value = parseInt(policer[pno-1]);
		//document.forms[0].R55.selectedIndex = getStormRateSelection(pno);
	 	document.forms[0].des.value = getPortdescription();
        document.forms[0].speed.value = getSpeedSelection();
        document.forms[0].duplex.value = getDuplexSelection();
        document.forms[0].admin.selectedIndex = getPhyStatusSelection();
        document.forms[0].fc.selectedIndex = getFlowContrlSelection();
        document.forms[0].pri.selectedIndex = getPortPrioSelection();
        document.forms[0].MDIX.selectedIndex = getMDIXSelection();
		document.forms[0].isolate.selectedIndex = getIsolateSelection();
        //document.forms[0].R57.selectedIndex = pvid[pno-1];
    }
}
 
function Apply()
{
	var f=document.forms[0];
	diffCfg('portmask', 'portmask', f.portmask.value);
	diffCfg('description', 'description', f.des.value);
	diffCfg('speed', 'speed', f.speed.value);
	diffCfg('duplex', 'duplex', f.duplex.value);
	diffCfg('admin', 'admin', f.admin.value);
	diffCfg('fc', 'fc', f.fc.value);
	diffCfg('priority', 'priority', f.pri.value);
	diffCfg('MDIX', 'MDIX', f.MDIX.value);
	diffCfg('isolate', 'isolate', f.isolate.value);
	subForm(f,'goform/command','PORT_SET','port_stat.asp');
}

function resetform()
{
	document.forms[0].portmask.value = pMsk;

    if((pno == 0xff)||(pno == 0))
    {
	 	document.forms[0].des.value = "port description";
        document.forms[0].speed.selectedIndex = 3;
        document.forms[0].duplex.selectedIndex = 1;
        document.forms[0].admin.selectedIndex = 1;
        document.forms[0].fc.selectedIndex = 1;
        document.forms[0].pri.selectedIndex = 0;
        document.forms[0].MDIX.selectedIndex = 2;
		document.forms[0].isolatse.selectedIndex = 1;
    }
    else
    {
	 	document.forms[0].des.value = getPortdescription();
        document.forms[0].speed.value = getSpeedSelection();
        document.forms[0].duplex.value = getDuplexSelection();
        document.forms[0].admin.selectedIndex = getPhyStatusSelection();
        document.forms[0].fc.selectedIndex = getFlowContrlSelection();
        document.forms[0].pri.selectedIndex = getPortPrioSelection();
        document.forms[0].MDIX.selectedIndex = getMDIXSelection();
		document.forms[0].isolate.selectedIndex = getIsolateSelection();
    }

}

</SCRIPT>
</HEAD>
<body bgcolor=#ffffff leftMargin=0 topMargin=0 MARGINWIDTH=0 MARGINHEIGHT=0 BORDER=0 onload="initPortValue();">
<script>pageHead(env, env['web.page']);</script>
<table cellpadding=0 cellspacing=0 border=0 width=100%>
<tr><td>
<FORM name="portconf" action="goform/command" method="post">
<script LANGUAGE="JavaScript" TYPE="text/javascript"> 
    for (i=0; i< numPorts; i++) 
    { 
        document.write('<INPUT name="R'+ (0x10+i).toString(16) +'" type="hidden" value="0">'); 
    } 
</script>

<TABLE cellpadding=2 cellspacing=0 border=0 width=100%>
  <TBODY>
  <TR><td class=tb_head colspan=2><div class=tb_head>端口设置</div></td></TR>
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
  
<script language="javascript" type="text/javascript">
    document.write('<tr>' +
      '<td class=tb_head>端口速率</td> <td class=tb_content><select name="speed"  style="WIDTH: 150px">');
	if(pMsk=='4')
		document.write('<option value="2" >1000Mbps<\/option>');
	else
		document.write('<option value="0" >10Mbps<\/option>' +
      '<option value="1" >100Mbps<\/option>' +
      '<option value="2" >1000Mbps<\/option>' +
      '<option value="3" >自协商<\/option>');
	document.write('<\/select><\/td>' +
      '<\/tr>' +
      '<tr>' +
      '<td class=tb_head>端口描述</td> <td class=tb_content><input name=des  style="WIDTH: 150px"></td>' +
      '<\/tr>' +
      '<tr>' +
      '<td class=tb_head>双工模式</td> <td class=tb_content><select name="duplex"  style="WIDTH: 150px">');
	if(pMsk=='4')
		document.write('<option value="1" >全双工<\/option>');
	else
		document.write('<option value="0" >半双工<\/option>' +  
      '<option value="1" >全双工<\/option>' +
      '<option value="2" >自协商<\/option>');
	document.write('<\/select><\/td>' +
      '<\/tr>' +
      '<tr>' +
      '<tr>' +
      '<td class=tb_head>优先级</td> <td class=tb_content><select name="pri"  style="WIDTH: 150px">' +
      '<option value="0" >0<\/option>' +
      '<option value="1" >1<\/option>' +
      '<option value="2" >2<\/option>' +
      '<option value="3" >3<\/option>' +
      '<option value="4" >4<\/option>' +
      '<option value="5" >5<\/option>' +
      '<option value="6" >6<\/option>' +
       '<option value="7" >7<\/option>' +
      '<\/select><\/td>' +
      '<\/tr>' +
      '<tr>' +
      '<td class=tb_head>开启/关闭端口</td> <td class=tb_content><select name="admin"  style="WIDTH: 150px">' +
      '<option value="0" >关闭<\/option>' +
      '<option value="1" >开启<\/option>' +
      '<\/select><\/td>' +
      '<\/tr>' +

      '<tr>' +
      '<td class=tb_head>进行流控</td> <td class=tb_content><select name="fc"  style="WIDTH: 150px">' +
      '<option value="0" >否<\/option>' +
      '<option value="1" >是<\/option>' +
      '<\/select><\/td>' +
      '<\/tr>' +
      '<tr>' +
      '<td class=tb_head>MDIX</td> <td class=tb_content><select name="MDIX"  style="WIDTH: 150px">' +
      '<option value="0" >ACROSS<\/option>' +
      '<option value="1" >AUTO<\/option>' +
      '<option value="2" >NORAML<\/option>' +
      '<\/select><\/td>' +
      '<\/tr>' +
      '<tr>' +
      '<td class=tb_head>隔离</td> <td class=tb_content><select name="isolate"  style="WIDTH: 150px">' +
      '<option value="0" >关闭<\/option>' +
      '<option value="1" >开启<\/option>' +
      '<\/select><\/td>' +
      '<\/tr>' +
      '<tr>');
	/*
	if("0"==vlanMode)
	{
	      document.write('<tr class="textCell">' +
	      '<td class=tb_head>Tag/Untag</td> <td class=tb_content><select name="R56"  style="WIDTH: 150px">' +
	      '<option value="0" >untaged<\/option>' +
	      '<option value="1" >taged<\/option>' +
	      '<\/select><\/td>' +
	      '<tr class="textCell">' +
	      '<td class=tb_head>pvid</td> <td class=tb_content><INPUT type=text maxLength=4  name="R57" onkeypress="validateInput(event, 1)" onpaste="return false;" style="WIDTH: 150px">' +
	      '<\/tr>');
	}
	*/
</script>
</table>
</td>
  <td align=center valign=top width=100>
  	<p>&nbsp;</p>
	<p><input name="portmask" type="hidden" value=""></p>
	<p><INPUT class=actButton name="Submit" type="button" value="确 定" onclick = "Apply();"></input></p>
	<p><INPUT class=actButton type=button value="取 消"  onclick=resetform();></input></p>	
  </td>
</tr>
</TBODY></TABLE>
<p>&nbsp;</p>
<p>&nbsp;</p>
</FORM>
</tr></td>
</table>
<script>pageTail();</script>
</BODY></HTML>

