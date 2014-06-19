<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD>
<META NAME=Copyright CONTENT="Copyright (c) 2007 H3C Corporation. All Rights Reserved.">
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<TITLE>ssid List</TITLE>
<LINK href="globe.css" type=text/css rel="stylesheet">
<SCRIPT language=JavaScript src='wareless/icg_utils.js'></SCRIPT>
<SCRIPT language=JavaScript src='wareless/icg_listContrlButton.js'></SCRIPT>
<script>
top.user_expire_time = "60";

 var ssidseg_list=new Array("0;1;LOSOTECH;1"
); 

/* list 格式: 选中标志;ssid编号;essid;启用开关 */
/* var ssidseg_list=new Array("0;1;wireless-test;1","0;2;wireless-test;0"); */

var tmp = getparastr("position");
var position = (tmp==null) ? "-1" : tmp;    

if (Number(position)>= 1)
{
    tmp = Number(position) - Number(position)%7;
    position = tmp;
}
var lineposition = '#line' + position;

LIST_LENGTH=ssidseg_list.length;
LIST_NAME='ssid';
LINE_NAME='allline';

tmp = getparastr("display");
var display = (tmp==null) ? "yes" : tmp;

tmp = getparastr("refreshParent");
var refreshParent = (tmp==null) ? "-1" : tmp;

function disable_pages()
{
    if (display == "no")
    {
                f=document.frmsetup;
		for(i=0;i<f.elements.length;i++)
			f.elements[i].disabled=true;
	        document.getElementById("ssid_list_div").disabled = true;
	  	parent._disable_allButton();

    }
}

function fun_select(f,id)
{
	f.CMD.value='SelectSsidList';
	f.GO.value='ssid_list.asp?position='+(id+1);	
	f.param.value=id;
		
	setLineColor(id);
	f.submit();
}

function show_ssid_list()
{
	var current_number=0;
	count=ssidseg_list.length;
	var m='';
	m+='<table name="s" border=0 valign=top>'+
	'<a name=line0></a><TR class=smalltabletitle>'+
	'<TD width=25 align=center><nobr>选择</nobr></TD>'+
	'<TD width=25 align=center><nobr>序号</nobr></TD>'+
	'<TD width=150 align=center><nobr>SSID编号</nobr></TD>'+
	'<TD width=300 align=center><nobr>SSID</nobr></TD>'+
/*	'<TD width=100 align=center><nobr>SSID使能</nobr></TD>'+ */
	'</TR>';

	for(;current_number < count;current_number++)
	{
		tmp=ssidseg_list[current_number];
	
		tt=tmp.split(';');
		
		//m+=writeListHead(current_number,tt[0],25,25);
		if (tt[1]==1)
		{
			m+='<TD width=150 align=center>SSID-1'+'</TD>';
		}
		else if (tt[1]==2)
		{
			m+='<TD width=150 align=center>SSID-2'+'</TD>';
		}
		else if (tt[1]==3)
		{
			m+='<TD width=150 align=center>SSID-3'+'</TD>';
		}
		else if (tt[1]==4)
		{
			m+='<TD width=150 align=center>SSID-4'+'</TD>';
		}		
		else
		{
			alert("SSID编号载入出错！");
		}				
		
		m+='<TD width=300 align=center>'+tt[2]+'</TD>';
	/*	if (tt[3] == 0)
		{
			m+='<TD width=100 align=center>禁用'+'</TD>'+
		'</TR>';		
		}
		else
		{
			m+='<TD width=100 align=center>启用'+'</TD>'+
		'</TR>';			
		} */
	}
	m += '<a name=line'+(count+1)+'></a></table>';
	document.getElementById("ssid_list_div").innerHTML = m;
}
function init()
{	
	tt=parent.cur_ssidseg_list.split(';');	
	EDIT_EN=Number(tt[0]);
	
	show_ssid_list();
    display_position();
    contrl_button();
    disable_pages();
   /* if (refreshParent == 1)
    {
        parent.GURL('wlan_basic.asp');  
    } */
}
</script>

</head>

<body bgcolor=white topmargin="0" leftmargin="0" onload=init()>
<tr><td width=485 align=left valign=top>
<form name=frmsetup action='/goform/aspForm' method=post>
<table width=99% border=0 cellspacing=1>
    <tr><td width=485 align=left colspan=3 valign=top>
     <div id=ssid_list_div style=position:relative;></div>
   </tr>
      <TR  class='textCell'><TD colspan=2 >&nbsp; </TD>
      <TD><input type=hidden name=CMD><input type=hidden name=GO><input type=hidden name=param>
      </TD>
     </TR>
</table>
</form>
</tr>

</body>
</html>


