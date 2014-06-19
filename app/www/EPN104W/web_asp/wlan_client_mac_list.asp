<HTML><HEAD>
<META NAME=Copyright CONTENT="Copyright (c) 2007 H3C Corporation. All Rights Reserved.">
<META http-equiv=Content-Type content=text/html;charset=gb2312>
<META HTTP-EQUIV=Pragma CONTENT=no-cache>
<TITLE>无线接入客户MAC列表</TITLE>
<LINK href=globe.css type=text/css rel=stylesheet>
<SCRIPT language=JavaScript src=icg_utils.js></SCRIPT>
<SCRIPT language=JavaScript src=icg_listContrlButton.js></SCRIPT>
<script>

var acl_mf_list=new Array();
var wlan_mac_list=new Array();
var mac_list=new Array();
var selectflag=0;

var checklist=new Array();

function mac_sort()
{
	var count = 0;
	var find = 0;
	for (i =0; i < wlan_mac_list.length; i++)
	{
		find = 0;
		tt = wlan_mac_list[i].split(";");
		for (k=0; k < mac_list.length; k++)
		{
			mf_entry = mac_list[k].split(";");
			if (tt[1].toUpperCase() == mf_entry[1].toUpperCase())
			{
				find = 1;
				break;
			}
		}
		if (find == 0)
		{
			acl_mf_list[count] = wlan_mac_list[i];
			count++;
		}
	}

	for (i =0; i < wlan_mac_list.length; i++)
	{
		find = 0;
		tt = wlan_mac_list[i].split(";");
		for (k=0; k < mac_list.length; k++)
		{
			mf_entry = mac_list[k].split(";");
			if (tt[1].toUpperCase() == mf_entry[1].toUpperCase())
			{
				find = 1;
				break;
			}
		}
		if (find == 1)
		{
			tmp = wlan_mac_list[i].split(";");
			acl_mf_list[count] = "1" +";" + tmp[1] + ";";
			//alert(acl_mf_list[count]);
			count++;
		}
	}
}
function init(f)
{
	mac_sort();
	document.getElementById("op_del").disabled=false;
	document.getElementById("import_arp_mac").disabled=true;
	show_mac_list();
}

function myselect(event)
{
	selectIt(acl_mf_list, event);
	for (j=0;j<acl_mf_list.length;j++)
	{
	    if (checklist[j]==1)
	    {
	        document.getElementById("import_arp_mac").disabled=false;
	        return;
	    }
	}
	document.getElementById("import_arp_mac").disabled=true;
}

function show_mac_list()
{
	var maxrows=acl_mf_list.length;
	var pageIndex=1;

	var current_number=0;
	count=acl_mf_list.length;
	var m='';
	
	m+='<div name=sitelist_block id=sitelist_block>';
	m+='<table name="s" id="disableclick" border=0 valign=top cellspacing=1 style="table-layout:fixed" onclick=myselect(event)>'+
	'<a name=line0></a><TR class=smalltabletitle>'+
	'<TD width=60  ><nobr>序号</nobr></TD>'+
	'<TD width=250 ><nobr>MAC地址</nobr></TD>'+
	'</TR>';

	for(current_number=0; current_number<count;current_number++)
	{
		tmp=acl_mf_list[current_number];

		tt=tmp.split(';');

		if (tt[0] == "1")
		{
			m+='<TR class=tableEvevCell style="color:blue" title="单击选中" id=mytable'+current_number+'>';
		}
		else
		{
			if(current_number%2 == 0)
				m+='<TR class=tableEvevCell title="单击选中" id=mytable'+current_number+'>';
			else
				m+='<TR class=tableOddCell title="单击选中" id=mytable'+current_number+'>';
		}
		m+='<TD width=60 align=center>'+(maxrows*(pageIndex-1)+current_number+1)+'</TD>';

		m+='<TD width=250 align=center>'+ tt[1].toUpperCase() +'</TD>'; 
	}

	m += '<a name=line'+(count+1)+'></a></table>';
	m+='</div>';
	
	document.getElementById("acl_arpmac_list").innerHTML = m;
	document.getElementById("acl_arpmac_list").style.height=30+21*(current_number);
	document.getElementById("acl_arpmac_list").style.height=20+21*(current_number);
}

function add_wlan_list_macs()
{ 
	var f=document.frmSetup;
    var msg = '';
    var j=0;
    var find = 0;
    var tmp = Number(mf_curnum);

    if(tmp == Number(mf_maxnum))
    {
        alert("MAC接入控制表项已达到"+Number(mf_maxnum)+"条上限，不能再增加!");	
        return; 
    }

    ret=confirm("确定要导入所有选中的表项吗?");
    if (ret==true)
    {
    	for (i=0;i<acl_mf_list.length;i++)
    	{
    	    if(checklist[i]==1)
    	    {
    	    	find = 0;
    	    	tt=acl_mf_list[i].split(";");
    	    	if (tt[0]=="0")
    	    	{
                    tmp++;
                   
                    if(tmp <= Number(mf_maxnum))
                    {
                         j++;
	    	    	     msg +=tt[1].toUpperCase()+';';
                    }
                    else
                    {
				        alert("MAC接入控制表项已达到"+Number(mf_maxnum)+"条上限，不能再增加!");	
                        break;
                    }
    	    	}
    	    	else
    	    	{
    	    		continue;
    	    	}
    	    }
    	}

        if (j>0)
        {
    		f.CMD.value='UpdateWlanMacList';
    		f.GO.value=cPage;
    		f.param.value=msg;
    		f.action="/goform/aspForm";

			//document.getElementById("reresh_arp_mac").disabled=true;
			document.getElementById("import_arp_mac").disabled=true;
			document.getElementById("op_selctall").disabled=true;
			document.getElementById("page_close").disabled=true;
    		f.submit();
			window.returnValue=true;
			setTimeout("window.close()",1000);
        }
        else
        {
			window.returnValue=true;
			window.close();
        }
	}
}

function selectall()
{
    document.getElementById("op_selctall").value=selectflag?"全选":"取消";
    for (i=0;i<acl_mf_list.length;i++)
    {
    	tmpid="mytable"+i;
    	document.getElementById(tmpid).style.backgroundColor=selectflag?"":"#80b2f0";
    	checklist[i]=selectflag?0:1;
    }
    document.getElementById("op_del").disabled=selectflag?true:false;
	document.getElementById("import_arp_mac").disabled=selectflag?true:false;
    
    selectflag=!selectflag;
}

function getrefresh(f)
{
}
function pageclose()
{
	if(parent.acl_mac_list)
	{
		parent.open("Wlan_ap_acl_list.asp?refreshParent=1","wlan_ap_mac_list");
	}
	window.returnValue=true;
	setTimeout("window.close()", 100);
	return;
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
<tr><td width=400 align=left valign=top>
<form name=frmSetup action='/goform/aspForm' method=post target='hf'>
	<tr><TD>
		<input type=hidden name=CMD>
		<input type=hidden name=GO>
		<input type=hidden name=param>
	<tr><td class=spaceLine></tr>
	<iframe name="hf" src="arp_tmp.asp" width=0 height=0>
	</iframe>
	<table border=0 align=center>

	<td width=400>
	<INPUT type=hidden name=op_del id=op_del>
	<!--
	<INPUT type=button name=reresh_arp_mac id=reresh_arp_mac width=160 class=longbutton value=刷新MAC列表 onClick=getrefresh(this.form)>
	-->
	<INPUT type=button name=op_selctall id=op_selctall class=button value=全选 onClick=selectall()>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
	<INPUT type=button name=import_arp_mac id=import_arp_mac width=150 class=longlongbutton id=toIPMACTable value=导入到MAC地址过滤表 onClick=add_wlan_list_macs(this.form)>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
	<INPUT class=Button name=page_close id=page_close type=button value=关闭 onclick=pageclose()>
	<HR>
	<TR><TD class=noteCell><B>提示：</B>如果表项显示为蓝色，则说明MAC接入控制表中已有该表项。</td>
	</table>

	<table width=400 border=0 cellspacing=0 align=center>
	    <tr><td width=400 colspan=3 valign=top>
	     <div id=acl_arpmac_list style=position:relative;></div>
	   </tr>
	</table>

	<tr class=spaceLine><td colspan=4 class=spaceLine></tr>
</form>
</td>
</tr>
</body>
</html>



