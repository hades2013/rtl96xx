<HTML><HEAD>
<META NAME=Copyright CONTENT="Copyright (c) 2009 H3C Corporation. All Rights Reserved.">
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<TITLE>wlan_ap_ssid LIST</TITLE>
<LINK href="globe.css" type=text/css rel="stylesheet">
<SCRIPT language=JavaScript src='wareless/icg_utils.js'></SCRIPT>
<SCRIPT language=JavaScript src='wareless/icg_listContrlButton.js'></SCRIPT>
<SCRIPT language=JavaScript src='wareless/icg_contrlButton.js'></SCRIPT>

<script>
/* list 格式: 选中标志;ssid编号;essid;启用开关 */
var ssid_list = new Array(<% CGI_SSID_LIST(); %>);  
/*
var ssid_list = new Array('1;SSID-1;1;SSID-name1;1;1;0;1;1;2;ls789;ls654;2;100', 
					'2;SSID-2;1;SSID-name2;1;1;1;1;1;2;ls789;ls654;2;100', 
					'3;SSID-3;1;MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM;1;1;2;1;1;2;ssid-789;ssid-654;2;100',
					'4;SSID-4;1;SSID-name4;1;1;3;1;1;2;ls789;ls654;2;100',
					'5;SSID-5;1;SSID-name5;1;1;4;1;1;2;ls789;ls654;2;100',
					'6;SSID-6;1;SSID-name6;1;1;1;2;1;2;ls789;ls654;2;100',
					'7;SSID-7;1;SSID-name7;1;1;1;1;1;2;ls789;ls654;2;100',
					'8;SSID-8;1;SSID-name8;1;1;1;1;1;2;ls789;ls654;2;100',
					'9;SSID-9;1;SSID-name9;1;1;1;1;1;2;ls789;ls654;2;100',
					'10;SSID-10;1;SSID-name10;1;1;1;1;1;2;ls789;ls654;2;100',
					'11;SSID-11;1;SSID-name11;1;1;1;1;1;2;ls789;ls654;2;100',
					'12;SSID-12;1;SSID-name12;1;1;1;1;1;2;ls789;ls654;2;100',
					'13;SSID-13;1;SSID-name13;1;1;1;1;1;2;ls789;ls654;2;100',
					'14;SSID-14;1;SSID-name13;1;1;1;1;1;2;ls789;ls654;2;100',
					'15;SSID-15;1;SSID-name13;1;1;1;1;1;2;ls789;ls654;2;100',
					'16;SSID-16;1;SSID-name14;1;1;1;1;1;2;ls789;ls654;2;100',
					'17;SSID-17;1;SSID-name15;1;1;1;1;1;2;ls789;ls654;2;100',
					'18;SSID-18;1;SSID-name16;1;1;1;1;1;2;ls789;ls654;2;100',
					'19;SSID-19;1;SSID-name17;1;1;1;1;1;2;ls789;ls654;2;100',
					'20;SSID-20;1;SSID-name18;1;1;1;1;1;2;ls789;ls654;2;100',
					'21;SSID-21;1;SSID-name19;1;1;1;1;1;2;ls789;ls654;2;100',
					'22;SSID-22;1;SSID-name20;1;1;1;1;1;2;ls789;ls654;2;100',
					'23;SSID-23;1;SSID-name21;1;1;1;1;1;2;ls789;ls654;2;100');
//*/						
/*'(curr_count);SSID;ssid_en(0:dis,1:en);ssid_name;islate:(0-1);ssid_bcast(0-1);enprypt(0-4)'*/						
parent.baklist=parent.totallist=ssid_list;
parent.sortlist=parent.baklist.concat(1);
parent.sortlist.pop();
parent.ssid_list=ssid_list;

var checklist=new Array();

var display = 'no';

function disable_pages()
{	
	var f = document.forms[0];	
	
	if(display=="no")
	{
		for(i=0;i<f.elements.length;i++)
			f.elements[i].disabled=true;
		document.getElementById("wlan_ap_list").disabled=true;
		//parent._disable_allButton();
	}
}

function fun_select(event,id)
{
	var list = document.getElementsByName("checkbox_list");
	var len = list.length;
	var i = 0;
	var num_checked = 0;

	for(i=0; i<len; i++)
	{
		if(list[i].checked == true)
		{	
			num_checked++; 	
			//parent.del_mac_list += event.value+';';
		}
	}
	//var len_tmp = parent.del_mac_list.length; 
	//parent.del_mac_list = parent.del_mac_list.substr(0, len_tmp-1); alert("len:"+len+" 111: "+parent.del_mac_list) //trim the last ';'
	
	parent.enalbe_del_bt(num_checked>0?1:0);
	parent.enalbe_edit_bt(num_checked==1?1:0);
	
	//setLineColor(id);
}


function fun_select(event,id)
{
	var list = document.getElementsByName("checkbox_list");
	var len = list.length;
	var i = 0;
	var num_checked = 0;

	for(i=0; i<len; i++)
	{
		if(list[i].checked == true)
		{	
			num_checked++; 	
		}
	}
	parent.enalbe_del_bt(num_checked>0?1:0);
}

function show_wlan_ap_list(list)
{
	var maxrows=parent.document.getElementById("txtMaxRows").value;
	var pageIndex=parent.document.getElementById("txtCurPageIndex").value;
	var current_number=0;
	var count=list.length;
	var m='';
	
	m+='<div name=sitelist_block id=sitelist_block>';
	m+='<table name="s" id="disableclick" border=0 valign=top cellspacing=0 style="table-layout:fixed" >'+
	'<a name=line0></a><TR class=smalltabletitle style="font-size:14">'+
	'<TD width=20%   align=center><nobr>选择</nobr></TD>'+
	'<TD width=20%  align=center><nobr>序号</nobr></TD>'+
	'<TD width=20%   align=center><nobr>SSID编号</nobr></TD>'+
	'<TD width=20%   align=center><nobr>状态</nobr></TD>'+
	'<TD width=20%  align=center><nobr>SSID名称</nobr></TD>'+
	'<TD width=20%   align=center><nobr>客户端隔离</nobr></TD>'+
	'<TD width=20%   align=center><nobr>SSID广播</nobr></TD>'+
	'<TD   align=center><nobr>加密方式</nobr></TD>'+
	'</TR>';

	for(current_number=0;current_number<count;current_number++)
	{
		tmp=list[current_number];		

		tt=tmp.split(';');
		m+='<TR class=tableOddCell style="font-size:13" id=mytable'+current_number+'>';
		m+='<TD width=30 align=center>';
		m+='<input type="checkbox" id="ssid'+current_number+'"  value='+tt[1]+' name="checkbox_list" onclick="fun_select(this, 0);"></input>';
		m+='</TD>';
		m+='<TD width=50 align=center>'+(maxrows*(pageIndex-1)+current_number+1)+'</TD>';
		m+='<TD width=80 align=center>'+tt[1]+'</TD>';
		if(tt[2] == 1)
			m+='<TD width=50 align=center >启用</TD>'; 
		else		
			m+='<TD width=50 align=center >禁用</TD>'; 

		m+='<TD width=180 align=center class=ellipsis_div onmouseover="this.title=this.innerText||this.textContent">'+tt[3]+'</TD>'; 

		if(tt[4] == 1)
			m+='<TD width=80 align=center >启用</TD>'; 
		else		
			m+='<TD width=80 align=center >禁用</TD>'; 
		if(tt[5] == 1)
			m+='<TD width=80 align=center >启用</TD>'; 
		else		
			m+='<TD width=80 align=center >禁用</TD>'; 
		if(tt[6] == 0)
			m+='<TD width=100% align=center >不加密</TD>'; 
		else if(tt[6] == 1)
			m+='<TD width=100% align=center >WEP</TD>'; 
		else if(tt[6] == 2)
			m+='<TD width=100% align=center >WPA-PSK</TD>'; 
		else if(tt[6] == 3)
			m+='<TD width=100% align=center >WPA2-PSK</TD>'; 
		else if(tt[6] == 4)
			m+='<TD width=100% align=center class=ellipsis_div onmouseover="this.title=this.innerText||this.textContent">WPA-PSK/WPA2-PSK</TD>'; 
	}
	m += '<a name=line'+(count+1)+'></a></table>';
	m+='</div>';
	document.getElementById("wlan_ap_list").innerHTML = m;
	parent.document.getElementById("wlan_ap_list").style.height=30+22*(current_number);
	document.getElementById("wlan_ap_list").style.height=20+21*(current_number);
	

}

function init()
{
	if (!(parent.showflag))
	{
		parent.gopage();
	}
	//disable_pages();
	//show_wlan_ap_list(ssid_list);
}

</script>

<STYLE type=text/css>
.ellipsis_div{   
overflow:hidden;   
text-overflow:ellipsis;   
white-space:nowrap;   
word-break:keep-all;
} 
</STYLE>
</head>
<body bgcolor=white topmargin="0" leftmargin="0" onload=init()>
<tr><td width=650 align=left valign=top>
<form name=frmsetup action='goform/command' method=post>
<table width=100% border=0 cellspacing=1>
    <tr><td width=100% align=left colspan=3 valign=top>
     <div id=wlan_ap_list style=position:relative;></div>
   </tr>
</table>
</form>
</tr>

</body>
</html>




