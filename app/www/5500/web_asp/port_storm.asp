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
env['web.page'] = current_page();env['sys.parent_name'] = '';
pageRedirectCheck(env);
</script>

<script LANGUAGE="JavaScript" type="text/javascript">

    var portstat = new Array(<% CGI_GET_STORMCTL_CONFIG(); %>);
	//var numPorts = 4;
	var numPorts = new Array(<% CGI_GET_PORT_LOGICNUM(); %>);
	var portname = new Array(<% CGI_GET_PORT_NAME(); %>);
	var admin =  1;
	var varnum = 4;
	//var portstat = new Array('1','1','1','10','1','1','1','100','1','0','1','10','1','1','0','10');
    //var portname =new Array('GE1','GE2','CAB1','CAB2');
    var port = Array(numPorts); 
    for (k = 0; k < numPorts; k++) 
    { 
        port[k] = 0;   
    }
	
function portName(pno)
{
	return portname[(pno - 1)];
}
	
function GURL(x)
{
    location=x;
}

function modeSelected(pno) 
{ 
    var mode = 0;
    mode = parseInt(portstat[2 + (pno - 1)*varnum])+(parseInt(portstat[3 + (pno - 1)*varnum])<<2); 
    switch(mode) 
    { 
        case 0: 
			return "10/HDX"; 
        case 1: 
			return "100/HDX"; 
        case 2: 
			return "1000/HDX"; 
        case 3: 
			return "AUTO/HDX"; 
        case 4: 
            return "10/FDX"; 
        case 5: 
            return "100/FDX"; 
        case 6: 
            return "1000/FDX"; 
        case 7: 
            return "AUTO/FDX"; 
        case 8: 
			return "10/AUTO"; 
        case 9: 
			return "100/AUTO"; 
        case 10: 
			return "1000/AUTO"; 
        case 11: 
			return "自协商"; 
        default: 
            return "Unknown"; 
    } 
}

function linkmodeSelected(pno) 
{ 
    var mode = 0;

	//linkdown
	if (portstat[10 + (pno - 1)*varnum] == '0') 
    {
		mode  = 7;
    }
	else
    {
		mode = parseInt(portstat[8 + (pno - 1)*varnum])+(parseInt(portstat[9 + (pno - 1)*varnum])<<2); 
    }
	
    switch(mode) 
    { 
		case 4:
			return "10/FDX";
		case 0:
			return "10/HDX"; 
		case 5:
			return "100/FDX";
		case 1:
			return "100/HDX"; 
		case 6:
			return "1000/FDX"; 
		case 2:
			return "1000/HDX";
		case 7:
			return "未连接"; 
		default: 
	        return "Unknown";
    } 
}



function curTag(pno)
{
    var tempstormRate = parseInt(tag[pno-1]);
    switch(tempstormRate)
    {
        case 0:
            return "untag";
        case 1:
            return "tag";
        default:
            return "untag";
    }
}
function portDes(pno)
{
	return portstat[(pno - 1)*varnum];
}

function curState(pno) 
{ 
    switch(parseInt(portstat[8 + (pno - 1)*varnum]))
    {
        case 0:
            return "未连接";
        case 1:
            return "xxxx";
        default:
            return "xxxx";
    }
}
                
function curFc(pno) 
{ 
    if (portstat[4 + (pno - 1)*varnum]== '1') 
    { 
        return "开启";
    } 
    else 
    { 
        return "关闭";
    } 
}

function IsolateEn(pno) 
{ 
    if (portstat[5 + (pno - 1)*varnum]== '1') 
    { 
        return "开启";
    } 
    else 
    { 
        return "关闭";
    } 
}

function Mdix(pno) 
{ 
	switch(parseInt(portstat[6 + (pno - 1)*varnum]))
    {
        case 0:
            return "ACROSS";
        case 1:
            return "AUTO";
        case 2:
            return "NORMAL";
        default:
            return "AUTO";
    }
}

function portPri(pno)
{
	return portstat[7 +(pno - 1)*varnum];
}
                
function curPhy(pno) 
{ 
    switch(portstat[1 + (pno - 1)*varnum]) 
    { 
        case '0':
            return "关闭";
        case '1':
            return "开启";
        default:
            return "unknown"; 
    } 
}

function selectAll(obj)
{
    if(obj.checked == true)
    {
        for (i=0 ; i< numPorts; i++) 
        {
            
            //eval("document.forms[0].cc"+(i+1).toString(16)+".checked = true");
			document.forms[0].cbox[i].checked = true;
        }
    }
    else
    {
        for (i=0 ; i< numPorts; i++) 
        {
            //eval("document.forms[0].cc"+(i+1).toString(16)+".checked = false");
			document.forms[0].cbox[i].checked = false;
        }
    }
}
function Applyall()
{
	var f=document.forms[0];
	var pmask = 0;
	var port_selected = false;
	
    for (i=0 ; i< numPorts; i++) 
    {
       //if(document.forms[0].cc+'(i+1)'.checked == true)
	     if(document.forms[0].cbox[i].checked == true)
       {
          pmask = pmask + (1<<i)
		      port_selected = true;
       }
    }

    if(false == port_selected)
    {
        alert("请选择所要配置的端口!");
        return;
    }
	setCookie(cookie_dontShowDoCMDPage, '1');
	document.forms[0].portmask.value = pmask;
	
	diffCfg('portmask', 'portmask', f.portmask.value);
	subForm(f,'goform/command','PORT_MASK','port_stormconf.asp');
}
function mouseOnPort(index)
{
    //if(1==user_level)
    {
	    document.getElementById('port'+index).style.textDecoration= 'underline';
	    document.getElementById('port'+index).style.color='#3030F0';
    }
}

function mouseOutPort(index)
{
    //if(1==user_level)
    {
	    document.getElementById('port'+index).style.textDecoration= 'none';
	    document.getElementById('port'+index).style.color='#000000';
    }
}

function selectPort(index)
{
    var pmask = 0;
    var f=document.forms[0];

    pmask = pmask + (1<<index);
    document.forms[0].portmask.value = pmask;
	
	diffCfg('portmask', 'portmask', f.portmask.value);
	subForm(f,'goform/command','PORT_MASK','port_statistic.asp');
}


function uucast_storm(pno) 
{ 
    if (portstat[(pno - 1)*varnum]== '1') 
    { 
        return "开启";
    } 
    else 
    { 
        return "关闭";
    } 
}
function mcast_storm(pno) 
{ 
    if (portstat[1 + (pno - 1)*varnum]== '1') 
    { 
        return "开启";
    } 
    else 
    { 
        return "关闭";
    } 
}

function bcast_storm(pno) 
{ 
    if (portstat[2 + (pno - 1)*varnum]== '1') 
    { 
        return "开启";
    } 
    else 
    { 
        return "关闭";
    } 
}

function curStormRate(pno)
{
    var tempstormRate = parseInt(portstat[3 + (pno - 1)*varnum]);
    switch(tempstormRate)
    {
        case 5:
            return "5%";
        case 10:
            return "10%";
        case 20:
            return "20%";
        case 100:
            return "100%";
        default:
            return "100%";
    }
}

</SCRIPT>
</HEAD> 

<BODY leftMargin=0 topMargin=0 MARGINWIDTH=0 MARGINHEIGHT=0 BORDER=0 > 
<script>pageHead(env, env['web.page']);</script>
<SCRIPT language=JavaScript>
</SCRIPT>
<FORM method=post name=portnum action="goform/command"> 
<table cellpadding=2 cellspacing=0 border=0 width=100%> 
    <TR><td class=tb_head colspan=2><div class=tb_head>端口设置</div></td></TR>
    <tr><td><table width=100% cellpadding=0 cellspacing=0 border=0 bordercolor=blue>
    <tr> 
        <input type=hidden name=maskindex value=1> 
        <td width=100%> 
            <TABLE cellSpacing=0 cellPadding=2 border=0 width=100%> 
                <TBODY>
                <TR class=tb_head> 
                <TD width= 5%><div align="center">端口</div></TD>
                <SCRIPT language=JavaScript>
                if (1 == admin)
                {
                    document.write('<TD width= 5%><div align="center">选择</div></TD>');
                }
                </SCRIPT>
			         	<TD width=13%><div align="center">未知单播抑制</div></TD>
                <TD width=10%><div align="center">组播抑制</div></TD>
                <TD width=10%><div align="center">广播抑制</div></TD>
                <TD width=10%><div align="center">风暴抑制比</div></TD>
                <SCRIPT language=JavaScript>
                //if("0"==vlanMode)
                //{
                //	document.write('<TD width=8%><div align="center">Tag/Untag<\/div><\/TD>'); 
               	//document.write('<TD width=9%><div align="center">pvid<\/div><\/TD> <\/TR>');
                //}
                </SCRIPT>
                <script LANGUAGE="JavaScript" type="text/javascript">
                for (var pno = 1; pno <= numPorts; pno++) 
                {
                    document.write('<tr>');
                    document.write('<td class="tb_content"><div align="center">' + portName(pno) + '<\/div><\/td>');
					if (1 == admin)
					{
						if(portName(pno) == "CAB1")
						{
							document.write('<td class="tb_content"><div align="center"><input type=checkbox name="cbox" disabled ondblclick="selectAll(this);"> <\/div><\/td> '); 
						}
						else
						{
							document.write('<td class="tb_content"><div align="center"><input type=checkbox name="cbox" ondblclick="selectAll(this);"> <\/div><\/td> '); 
						}
					}
					document.write('<td class="tb_content"><div align="center">' + uucast_storm(pno) + '<\/div><\/td>');
                    document.write('<td class="tb_content"><div align="center">' + mcast_storm(pno) + '<\/div><\/td>');
                    document.write('<td class="tb_content"><div align="center">' + bcast_storm(pno) + '<\/div><\/td>');
                    document.write('<td class="tb_content"><div align="center">' + curStormRate(pno)  + '<\/div><\/td>');
                    document.write('<\/tr><tr><\/tr>');
                } 
                </script>
                </TBODY>
            </TABLE>
        </td>        
    </tr>
  </table>
</table>
<TABLE cellSpacing=0 cellPadding=2 border=0 width=100%> 
    <tr>
    <p><input name="portmask" type="hidden" value=""></p>
    <td align="right">
	<p><INPUT class=actButton type="button" value="刷 新" onclick="GURL('port_storm.asp');"></p>
	</td>
	<td align="left">
	<p><SCRIPT language="JavaScript">
	if (1 == admin)
	{
	    document.write('<INPUT class=actButton type="button" value="配 置" onclick="Applyall();">');
		//document.write('<INPUT class=actButton type="submit" value="配 置" onclick="GURL('port_statistics.asp');">');
	}
	</script></p>
  </td>
  </tr>
</table>
</FORM> 
<SCRIPT language="JavaScript">
pageTail();
</script>
</BODY>
</HTML>


