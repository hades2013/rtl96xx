<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<META HTTP-EQUIV=Pragma CONTENT=no-cache>
<TITLE>端口镜像</TITLE>
<LINK href=globe.css type=text/css rel=stylesheet>

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

<SCRIPT src=helpScript.js></SCRIPT>
<SCRIPT src=navigatortab.js></SCRIPT>
<SCRIPT src=utils.js></SCRIPT>
<script src="js/vmenu.js" type="text/javascript"></script>
<script src="js/cookie.js" type="text/javascript"></script> 
<script LANGUAGE="JavaScript" type="text/javascript" >
    var pno;
    var gno;
    var mirrorPort = 0;
    //var numPorts = 4;
	var numPorts = new Array(<% CGI_GET_PORT_LOGICNUM(); %>);
    //var mirro_port_dir = new Array('1','2','4','3');
	var mirro_port_dir = new Array(<% CGI_GET_MIRROR(); %>);
	var portname = new Array(<% CGI_GET_PORT_NAME(); %>);
	//var portname =new Array('GE1','GE2','CAB1','CAB2');
	var user_level= 1;
	var set_err = 0; 
	//top.contents.urlcookie.get_cur_url("mirror.htm");
	
	var m= 0;
	for(m=0; m<numPorts; m++)
	{
		if(mirro_port_dir[m] == 4)
		{
		   mirrorPort=m+1;
		}
	}

	function portName(pno)
    {
	    return portname[(pno - 1)];
    }
	
    function parseDec(input) 
    {
        return parseInt(input,10);
    }
	
	function GURL(x)
	{
	    location=x;
	}

    function mirrorPortSelected(i) 
    {
        if(mirrorPort == i)
        {
            return 'selected';
        } 
        else 
        {
            return '';
        }
    }

    function getRowNo(portNo)
    {
        if(portNo/2 == parseDec(portNo/2))
        {
            return parseDec(portNo/2);
        }
        else
        {
            return parseDec(portNo/2) + 1;
        }
    }

    function checkMirror(selectObj) 
    {
        mirrorPort = selectObj.value;
		mirror_en(mirrorPort);		
    }
    function mirrDirSlected(pno,value)
    {
        if(mirro_port_dir[pno-1] == value)
        {
            return "selected";
        }
        else
        {
            return "";
        }
    }
	function mirror_en(port_num)
	{
	    var i, j;
	
	    if (0 == port_num)
		{
		    for (i = 0; i < numPorts; i++)
		    {
		        eval("document.mirror.R"+ (0x10+i).toString(16)+".value=0;");
				eval("document.mirror.R"+ (0x10+i).toString(16)+".disabled=true;");
		    }
		}
		else
		{
		    i = port_num - 1;
		    eval("document.mirror.R"+ (0x10+i).toString(16)+".value=0;");
			eval("document.mirror.R"+ (0x10+i).toString(16)+".disabled=true;");
			for ( j = 0; j < numPorts; j++ )
			{
			    if ( j == i )
			    {
			        continue;
			    }
			    eval("document.mirror.R"+ (0x10+j).toString(16)+".disabled=false;");
			}
		}
	}
	function mirror_dis()
	{
	    var i;
		
	    eval("document.mirror.Port.disabled=true;");
		for (i = 0; i < numPorts; i++)
		{
		    eval("document.mirror.R"+ (0x10+i).toString(16)+".disabled=true;");
		}
	}
	function init_mirror(pNum)
	{
	    if (1 == user_level)
	    {
	        mirror_en(pNum);
        	if(1 == set_err)
        	{
        	    var origin_value = "";
        	    var origin_value1 = "";
        	    
        	  //  origin_value = get_cookie("Port_VALUE");
        	    if(null != origin_value)
        	    { 
            	   // origin_value1 = get_cookie("PortList_VALUE");
            	    if(null != origin_value1)
            	    { 
            	        document.mirror.Port.selectedIndex = origin_value;
            	        mirror_en(origin_value);
            	        mirro_port_dir = origin_value1.split(',');
            	        //alert(origin_value);
            	        var j;
            	        for ( j = 0; j < numPorts; j++ )
            	        {
							if(4 == mirro_port_dir[j])
							{
								mirro_port_dir[j] = 0;
							}
            	            eval("document.mirror.R"+ (0x10+j).toString(16)+".selectedIndex="+mirro_port_dir[j]+";");
            	        }
            	    }
        	    }
        	}
        //	delete_temporary_cookie("Port_VALUE");
        //	delete_temporary_cookie("PortList_VALUE");
	    }
		else
		{
		    mirror_dis();
		}
	}

function formsubmit()
{
    //var obj = document.mirror;
    
    //if(obj.Port.value != "")
    //{ 
       // set_cookie("Port_VALUE", escape(obj.Port.value), null, "/"); 
       // var portlist = new Array();
		//for (i = 0; i < numPorts; i++)
		//{
		//        eval("portlist["+portlist.length+"] = document.mirror.R"+ (0x10+i).toString(16)+".value;");
		//}
      //  set_cookie("PortList_VALUE", escape(portlist), null, "/");
    //}


	var f=document.forms[0];
	var i=0;

	for(i=0; i<numPorts; i++)
	{
		if(f.Port.value == (i+1))
		{
            switch(i)
            {
				case 0:
					f.R10.value = 4;
					break;
				case 1:
					f.R11.value = 4;
					break;
				case 2:
					f.R12.value = 4;
					break;
				//case 3:
				//	f.R13.value = 4;
				//	break;
			}
		}
	}
	
	//alert(f.R10.value+","+f.R11.value+","+f.R12.value+","+f.R13.value);
	diffCfg('mirrorport1', 'mirrorport1', f.R10.value);
	diffCfg('mirrorport2', 'mirrorport2', f.R11.value);
	diffCfg('mirrorport3', 'mirrorport3', f.R12.value);
	//diffCfg('mirrorport4', 'mirrorport4', f.R13.value);
	subForm(f,'goform/command','PORT_MIRROR','port_mirror.asp');
	
}
</SCRIPT>
</HEAD>
<BODY bgcolor=#ffffff leftMargin=0 topMargin=0 MARGINWIDTH=0 MARGINHEIGHT=0 BORDER=0 onload="" >
<script>pageHead(env, env['web.page']);</script>
<FORM action="goform/command" method="post" name="mirror">
<table border=0 >
<tr><td colSpan=3>&nbsp;</td></tr>
<tr>
<td width=24 >&nbsp;</td>
<td width=720 valign=top>

<TABLE width=100% border=0 cellPadding=3 cellSpacing=1 name=ta>
<script LANGUAGE="JavaScript" type="text/javascript">
    document.write('<TR><TD>'   +
                   '<TABLE cellSpacing="0" cellPadding="2" border="0" width="100%" id="table1">' +
                   '<TBODY>'    +
                   '<TR><TD colspan="2" class="tb_head">&nbsp;监控端口<\/TD><\/TR>' +
                   '<TR><TD height="8"><\/TD><\/TR>' +
                   '<TR class="textCell">' +
                   '<TD>监控端口<\/TD>' +
                   '<TD width="80%"><select name="Port" onchange="checkMirror(this);">');
    document.write('<option value="' + 0 +'">' + '不镜像' + '<\/option>');
    for(i = 1; i <= numPorts; i++) 
    {

        document.write('<option value="' + i +'" ' + mirrorPortSelected(i) + '>' + portName(i) + '<\/option>');

    }               
    document.write('<\/select><\/TD><\/TR>' +
                   '<TR><TD height="8"><\/TD><\/TR>' +
                   '<\/TBODY>' +
                   '<\/TABLE>' +  
                   '<\/TD>' +
                   '<\/TR>');
    
    document.write('<TR><TD class=noteCell>'   +
                   '<b>注意:</b>&nbsp;配置较多的被镜像端口，当被镜像端口流量较大时可能导致监控端口拥塞，影响端口转发性能。' +
                   '<\/TD>' +
                   '<\/TR>');

    
    document.write('<TR><TD>'  +
                   '<TABLE cellSpacing="1" cellPadding="2" border="0" width="100%" id="table2">' +
                   '<TBODY>'  +
                   '<TR class="tb_head">' +
                   '<TD><div align="center">端口<\/div><\/TD>'   +
                   '<TD><div align="center">镜像方向<\/div><\/TD>'  +
                   '<TD><div align="center">端口<\/div><\/TD>'  +
                   '<TD><div align="center">镜像方向<\/div><\/TD>'  +
                   '<\/TR>');
    
    for (pno = 1; pno <= getRowNo(numPorts); pno++) 
    {
        document.write('<tr class="textCell">');
        document.write('<td align="center">' + portName(pno) + '<\/td>');
		if (1 == user_level)
		{
			if(portName(pno) == "CAB1")
			{
				document.write('<td align="center"><select name="R' + (0x10+pno-1).toString(16) + '"  disabled style="WIDTH: 120px">');
			}
			else
			{
                document.write('<td align="center"><select name="R' + (0x10+pno-1).toString(16) + '" style="WIDTH: 120px">');
		    }
		}
		else
		{
			document.write('<td align="center"><select name="R' + (0x10+pno-1).toString(16) + '"  disabled style="WIDTH: 120px">');
		}
        document.write('<option VALUE="0"' + mirrDirSlected(pno,0) + '>不镜像<\/option>');
        document.write('<option VALUE="1"' + mirrDirSlected(pno,1) + '>镜像入端口<\/option>');
        document.write('<option VALUE="2"' + mirrDirSlected(pno,2) + '>镜像出端口<\/option>');
        document.write('<option VALUE="3"' + mirrDirSlected(pno,3) + '>镜像入和出端口<\/option>');
		document.write('<option hidden="hidden" VALUE="4"' + mirrDirSlected(pno,4) + '>监控端口<\/option>');
        document.write('<\/select><\/td>');
        if((getRowNo(numPorts)+ pno) <= numPorts)
        {
            document.write('<td align="center">' + portName(getRowNo(numPorts)+ pno) + '<\/td>');
			if (1 == user_level)
		    {
				if(portName(getRowNo(numPorts)+ pno) == "CAB1")
				{
					document.write('<td align="center"><select name="R' + (0x10+(getRowNo(numPorts)+ pno)-1).toString(16) + '" disabled  style="WIDTH: 120px">');
				}
				else
				{
					document.write('<td align="center"><select name="R' + (0x10+(getRowNo(numPorts)+ pno)-1).toString(16) + '" style="WIDTH: 120px">');
				}
			}
			else
			{
				document.write('<td align="center"><select name="R' + (0x10+(getRowNo(numPorts)+ pno)-1).toString(16) + '" disabled style="WIDTH: 120px">');
			}
            
            document.write('<option VALUE="0"' + mirrDirSlected((getRowNo(numPorts)+ pno),0) + '>不镜像<\/option>');
            document.write('<option VALUE="1"' + mirrDirSlected((getRowNo(numPorts)+ pno),1) + '>镜像入端口<\/option>');
            document.write('<option VALUE="2"' + mirrDirSlected((getRowNo(numPorts)+ pno),2) + '>镜像出端口<\/option>');
            document.write('<option VALUE="3"' + mirrDirSlected((getRowNo(numPorts)+ pno),3) + '>镜像入和出端口<\/option>');
            document.write('<option hidden="hidden" VALUE="4"' + mirrDirSlected((getRowNo(numPorts)+ pno),4) + '>监控端口<\/option>');
			document.write('<\/select><\/td>');
        }
        document.write('<\/tr>');
    }
       
    document.write('<\/TABLE><\/TD><\/TR>');                 
</script>
</TABLE>

</td>
</tr>
</table>

<table width=744 border=0 >
<tr>
<td width=24 >&nbsp;</td>
<SCRIPT language=JavaScript>
if (1 == user_level)
{
    document.write('<td align= "right"><p><INPUT class=actButton name="Submit" type="button" value="确定" onclick="formsubmit()" ><\/p></td>');
	document.write('<td align= "left"><p><input name=button2 type=button class=actButton onClick=GURL(\'port_mirror.asp\') value="取消"><\/p></td>');
}
</SCRIPT>
</tr>
</table>

</FORM>
<SCRIPT language=JavaScript>
pageTail();
init_mirror(mirrorPort);
</SCRIPT>
</BODY></HTML>




