
<HTML><HEAD>
<META NAME=Copyright CONTENT="Copyright (c) 2007 H3C Corporation. All Rights Reserved.">
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<TITLE>ACL MAC LIST</TITLE>
<script>
var mac_list = new Array( <% CGI_MAC_LIST(); %>);
/*
var mac_list=new Array("0;00:11:22:33:44:FF;MAC4", 
						"1;00:11:22:33:44:bb;MAC12", 
						"2;00:11:22:33:44:cc;MAC13", 
						"3;00:11:22:33:44:dd;MAC14", 
						"4;00:11:22:33:44:ee;MAC15", 
						"5;00:11:22:33:44:00;MAC5", 
						"6;00:11:22:33:44:00;MAC8", 
						"7;00:11:22:33:44:00;MAC9", 
						"8;00:11:22:33:44:00;MAC10", 
						"9;00:11:22:33:44:00;MAC11", 
						"10;00:11:22:33:44:ff;MAC16", 
						"11;00:11:22:33:44:ab;MAC17", 
						"12;00:11:22:33:44:ac;MAC18",
						"13;00:11:22:33:44:33;MAC1", 
						"14;00:11:22:33:44:44;MAC1", 
						"15;00:11:22:33:44:55;MAC1", 						
						"16;00:11:22:33:44:ad;MAC19", 
						"17;00:11:22:33:44:ae;MAC20",
						"18;00:11:22:33:44:af;MAC21",
						"19;00:11:22:33:44:tt;MAC1", 
						"20;00:11:22:33:44:tt;MAC1", 
						"21;00:11:22:33:44:tt;MAC1", 
						"22;00:11:22:33:44:ba;MAC22");
//*/						

parent.baklist=parent.totallist=mac_list;
parent.sortlist=parent.baklist.concat(1);
parent.sortlist.pop();

var checklist=new Array();

function disable_pages(f)
{
	if(display=="no")
	{
		for(i=0;i<f.elements.length;i++)
			f.elements[i].disabled=true;
		document.getElementById("wlan_ap_mac_list").disabled=true;
		parent._disable_allButton();
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
		}
	}
	parent.enalbe_del_bt(num_checked>0?1:0);
	parent.enalbe_edit_bt(num_checked==1?1:0);
}

function show_acl_mac_list(mac_list)
{
	var maxrows=parent.document.getElementById("txtMaxRows").value;
	var pageIndex=parent.document.getElementById("txtCurPageIndex").value;

	var current_number=0;
	count=mac_list.length;
	var m='';
	
	m+='<div name=sitelist_block id=sitelist_block>';
	m+='<table name="s" id="disableclick" border=0 valign=top cellspacing=0 style="table-layout:fixed">'+ 
	'<a name=line0></a><TR class=smalltabletitle>'+
	'<TD width=30  align=center><nobr>操作</nobr></TD>'+
	'<TD width=50  align=center><nobr>序号</nobr></TD>'+
	//'<TD width=200 align=center title="点击排序" style="cursor:pointer" class='+parent.initclass[0]+' onclick=parent.sortFunc(1)><nobr>MAC地址</nobr></TD>'+
	//'<TD width=280 align=center title="点击排序" style="cursor:pointer" class='+parent.initclass[1]+' onclick=parent.sortFunc(2)><nobr>描述</nobr></TD>'+
	'<TD width=200 align=center style="cursor:pointer" class='+parent.initclass[0]+' ><nobr>MAC地址</nobr></TD>'+
	'<TD width=280 align=center style="cursor:pointer" class='+parent.initclass[1]+' ><nobr>描述</nobr></TD>'+
	'</TR>';

	for(current_number=0;current_number<count;current_number++)
	{
		tmp=mac_list[current_number];		

		tt=tmp.split(';');

		//if(current_number%2 == 0)
		//	m+='<TR class=tableEvevCell title="单击选中，双击编辑" id=mytable'+current_number+'>';
		//else
		//	m+='<TR class=tableOddCell title="单击选中，双击编辑" id=mytable'+current_number+'>';
		
		m+='<TR class=tableOddCell id=mytable'+current_number+'>';
		m+='<TD width=30 align=center>';
		//m+='<input id=edit  type=radio name=edit'+current_number+'>';
		//m+='<input type=checkbox border=0 name=edit style="WIDTH: 15;HEIGHT:15;cursor:pointer" onclick=editIt(mac_list,"wlan_ap_acl_cfg.asp",550,180,event) alt="编辑">';
		m+='<input  value="'+ tt[0] +'" type="checkbox" onclick="fun_select(this, 0);" name="checkbox_list"  id="checkbox_list'+current_number+'"></input>';
		m+='</TD>';
		m+='<TD width=50 align=center>'+(maxrows*(pageIndex-1)+current_number+1)+'</TD>';
		m+='<TD width=200 align=center >'+ tt[1].toUpperCase() +'</TD>'; 
		m+='<TD width=280 align=center class=ellipsis_div onmouseover="if(this.innerText||this.textContent) this.title=this.innerText||this.textContent">'+tt[2]+'</TD>'; 
	}
	m += '<a name=line'+(count+1)+'></a></table>';
	m+='</div>';
	document.getElementById("wlan_ap_mac_list").innerHTML = m;
	parent.document.getElementById("wlan_ap_mac_list").style.height=30+21*(current_number);
	document.getElementById("wlan_ap_mac_list").style.height=20+21*(current_number);
}

function init(f)
{	
	if(!(parent.showflag))
	{
		parent.gopage();
	}
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
<body bgcolor=white topmargin="0" leftmargin="0" onload=init(document.frmsetup)>
<tr><td width=600 align=left valign=top>
<form name=frmsetup action='/goform/command' method=post>
<table width=100% border=0 cellspacing=1>
    <tr><td width=100% align=left colspan=3 valign=top>
     <div id=wlan_ap_mac_list style=position:relative;></div>
   </tr>
</table>
</form>
</tr>

</body>
</html>




