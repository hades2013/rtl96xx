<HTML><HEAD>
<META NAME=Copyright CONTENT="Copyright (c) 2009 H3C Corporation. All Rights Reserved.">
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<TITLE>日志信息</TITLE>
<script>

var dhcpd_client_list = new Array( <% CGI_GET_SYSLOG(); %>);

/*var dhcpd_client_list=new Array( 'Jan  1 00:03:41;user;notice;WEB; WEB: user logged out from 192.168.0.33.','Jan  1 00:03:49;user;notice;WEB; WEB:admin user logged in from 192.168.0.33.','Jan  1 00:07:17;user;notice;WEB; WEB: user logged out from 192.168.0.33.','Jan  1 00:07:18;user;notice;WEB; WEB:admin user logged in from 192.168.0.33.','Jan  1 00:10:53;user;notice;WEB; WEB: user logged out from 192.168.0.33.','Jan  1 00:11:00;user;notice;WEB; WEB:admin user logged in from 192.168.0.33.','Jan  1 00:26:23;user;info;LOOP; LOOP:loop enable now.','Jan  1 00:26:29;user;info;LOOP; LOOP:loop diabled now.','Jan  1 00:26:34;user;notice;WEB; WEB: user logged out from 192.168.0.33.','Jan  1 00:26:35;user;notice;WEB; WEB:admin user logged in from 192.168.0.33.','Jan  1 00:26:52;user;info;LOOP; LOOP:loop enable now.','Jan  1 00:26:55;user;info;LOOP; LOOP:loop diabled now.','');
//*/
var checklist=new Array();
parent.baklist=parent.totallist=dhcpd_client_list; 
parent.sortlist=parent.baklist.concat(1); 
parent.sortlist.pop();

function show_dhcpd_client_list(list)
{
	var maxrows=parent.document.getElementById("txtMaxRows").value;
	var pageIndex=parent.document.getElementById("txtCurPageIndex").value;
	var current_number=0;
	var tmp;
	var tx_rx_s = '';
	
	count=list.length; 
	var m='';
	m+='<table cellspacing="0" cellpadding="0"  style="font-size: 10.5pt">'+
	'<a name=line0></a>';

	m+='<TR class=smalltabletitle >'+
	'<TD  width=20% align=center ><nobr>日期/时间</nobr></TD>'+
	'<TD  width=7% align=center ><nobr>类型</nobr></TD>'+
	'<TD  width=5% align=center ><nobr>级别</nobr></TD>'+
	'<TD  width=11% align=center ><nobr>模块</nobr></TD>'+
	'<TD  width=90% align=center ><nobr>消息</nobr></TD>'+
	'<TD class="def_lastCol">&nbsp;</TD>'; +
	'</TR>';
	for(;current_number < count;current_number++)
	{
		tmp=list[current_number];
		if("" != tmp)
		{

		tt=tmp.split(';');
		//tx_rx_s = tt[2]+"Mbps/"+tt[3]+"Mbps";
		m+='<TR class=tableOddCell id=mytable'+current_number+'>';
		
		m+='<TD width=20% align=center><span>'+tt[0]+'</span></TD>';
		m+='<TD width=7% align=center><span>'+ tt[1]+'</span></TD>';
		m+='<TD width=5% align=center><span>'+ tt[2]+'</span></TD>';
		m+='<TD width=11% align=center><span>'+ tt[3]+'</span></TD>';
		m+='<TD width=90% align=left><span>'+ tt[4]+'</span></TD>';
		m+='<TD >&nbsp;</TD>'; 
		m+='</TR>';
		}
	}
	m += '<a name=line'+(count+1)+'></a></table>';
	document.getElementById("dhcpd_client_listp").innerHTML = m; 
	parent.document.getElementById("dhcpd_client_frame").style.height=30+20*(current_number);
}
function init()
{
	if (!parent.showflag)
	{
		parent.gopage();
	}
	//show_dhcpd_client_list(dhcpd_client_list);
}

</script>

</head>

<STYLE type=text/css>
.ellipsis_div{   
overflow:hidden;   
text-overflow:ellipsis;   
white-space:nowrap;   
word-break:keep-all;
} 
</STYLE>

<style type="text/css">
.fixedtd{
    position:relative;
	top: expression(this.parentElement.parentElement.parentElement.parentElement.scrollTop-1);
	z-index:1;
}
</style>

<body bgcolor=white topmargin="0" leftmargin="0" onload=init()>
<form name=frmsetup action='goform/command' method=post>
<table width=100% border=0 cellspacing=0>
    <tr><td width=700 align=left colspan=3 valign=top>
     <div id=dhcpd_client_listp style="overflow:static;"></div>
   </tr>
</table>
</form>
</tr>

</body>
</html>






