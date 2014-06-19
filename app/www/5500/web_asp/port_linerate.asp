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

	//var numPorts = 4;
	var numPorts = new Array(<% CGI_GET_PORT_LOGICNUM(); %>);
	var admin =  1;
	var varnum = 2;
	//var portstat = new Array('0','0','128','64','0','0','1280','0');
    var portstat = new Array(<% CGI_GET_LINERATE(); %>);
	var portname = new Array(<% CGI_GET_PORT_NAME(); %>);
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
	subForm(f,'goform/command','PORT_MASK','port_rateset.asp');
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


function lintRatein(pno)
{
	  var inrate = portstat[(pno - 1)*varnum];
    if(inrate == 0)
    {
        return "---"
    }
    else
    {
       return inrate;
    }
}
function lintRateout(pno)
{
	  var outrate = portstat[1 + (pno - 1)*varnum];
    if(outrate == 0)
    {
        return "---"
    }
    else
    {
       return outrate;
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
                <TD width= 8%><div align="center">端口</div></TD>
                <SCRIPT language=JavaScript>
                if (1 == admin)
                {
                    document.write('<TD width= 7%><div align="center">选择</div></TD>');
                }
                </SCRIPT>
	         	<TD width=40%><div align="center">入端口限速值（kbps）</div></TD>
                <TD width=40%><div align="center">出端口限速值 (kbps)</div></TD>
                <SCRIPT language=JavaScript>
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
		            document.write('<td class="tb_content"><div align="center">' + lintRatein(pno) + '<\/div><\/td>');
                    document.write('<td class="tb_content"><div align="center">' + lintRateout(pno) + '<\/div><\/td>');
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
	<p><INPUT class=actButton type="button" value="刷 新" onclick="GURL('port_linerate.asp');"></p>
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


