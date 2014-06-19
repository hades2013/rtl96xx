<HTML><HEAD>
<META NAME=Copyright CONTENT="Copyright (c) 2009 H3C Corporation. All Rights Reserved.">
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<TITLE>无线用户列表</TITLE>
<script>

var dhcpd_client_list = new Array( <% CGI_CLIENT_LIST(); %>);
/*
var dhcpd_client_list=new Array('SSID-1;f8:3d:ff:b9:8f:f1;0.000;0.000','SSID-1;f8:3d:ff:b9:8f:fb;0.000;0.000',
								'SSID-1;f8:3d:ff:b9:8f:f2;0.000;0.000','SSID-1;f8:3d:ff:b9:8f:fb;0.000;0.000',
								'SSID-1;f8:3d:ff:b9:8f:f3;0.000;0.000','SSID-1;f8:3d:ff:b9:8f:fb;0.000;0.000',
								'SSID-1;f8:3d:ff:b9:8f:f4;0.000;0.000','SSID-1;f8:3d:ff:b9:8f:fb;0.000;0.000',
								'SSID-1;f8:3d:ff:b9:8f:f4;0.000;0.000','SSID-1;f8:3d:ff:b9:8f:fb;0.000;0.000',
								'SSID-1;f8:3d:ff:b9:8f:f4;0.000;0.000','SSID-1;f8:3d:ff:b9:8f:fb;0.000;0.000',
								'SSID-1;f8:3d:ff:b9:8f:f5;0.000;0.000','SSID-1;f8:3d:ff:b9:8f:fb;0.000;0.000',
								'SSID-1;f8:3d:ff:b9:8f:f5;0.000;0.000','SSID-1;f8:3d:ff:b9:8f:fb;0.000;0.000',
								'SSID-1;f8:3d:ff:b9:8f:f1;0.000;0.000','SSID-1;f8:3d:ff:b9:8f:fb;0.000;0.000',
								'SSID-1;f8:3d:ff:b9:8f:fb;0.000;0.000','SSID-1;f8:3d:ff:b9:8f:fb;0.000;0.000');
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
	m+='<table name="s" id="disableclick" valign=top id=dhcpstatic_table cellspacing=1 style="table-layout:fixed">'+
	'<a name=line0></a>';

	m+='<TR class=smalltabletitle >'+
	'<TD  width=10% align=center ><nobr>序号</nobr></TD>'+
	'<TD  width=10% align=center ><nobr>所属SSID</nobr></TD>'+
	'<TD  width=40% align=center style="cursor:pointer" class='+parent.initclass[0]+'"><nobr>MAC地址</nobr></TD>'+
	//'<TD  width=30% align=center ><nobr>Tx/Rx速率</nobr></TD>'+
	'</TR>';
	for(;current_number < count;current_number++)
	{
		tmp=list[current_number];

		tt=tmp.split(';');
		tx_rx_s = tt[2]+"Mbps/"+tt[3]+"Mbps";
		m+='<TR class=tableOddCell id=mytable'+current_number+'>';
		
		m+='<TD width=10% align=center>'+(maxrows*(pageIndex-1)+current_number+1)+'</TD>';
		m+='<TD width=10% align=center>'+ tt[0]+'</TD>';
		m+='<TD width=40% align=center>'+ tt[1].toUpperCase() +'</TD>';
		//m+='<TD width=300% align=center>'+ tx_rx_s+'</TD>';
		m+='</TR>';

	}
	m += '<a name=line'+(count+1)+'></a></table>';
	document.getElementById("dhcpd_client_listp").innerHTML = m; 
	parent.document.getElementById("dhcpd_client_frame").style.height=30+21*(current_number);
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
    <tr><td width=455 align=left colspan=3 valign=top>
     <div id=dhcpd_client_listp style="overflow:static;"></div>
   </tr>
</table>
</form>
</tr>

</body>
</html>






