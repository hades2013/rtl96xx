<HTML><HEAD>
<META NAME=Copyright CONTENT="Copyright (c) 2007 H3C Corporation. All Rights Reserved.">
<META http-equiv=Content-Type content=text/html;charset=GB2312>
<META HTTP-EQUIV=Pragma CONTENT=no-cache>
<TITLE></TITLE>
<link rel="stylesheet" href="style.css" type="text/css">
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=navigatortab.js></SCRIPT>
<script language=JavaScript>
var str_copyright =<%CGI_GET_COPYRIGHT();%>
var PROD_MODEL = <%CGI_GET_MODEL();%>
</SCRIPT>
<SCRIPT language=JavaScript>
pageTitle();

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
//var webVars = new Array( 'EPN104', '0', '0');

var pattern=/\d+;([\w:]+)/;
var turn = 1;
var initclass=["no"];

//var search_val=getparastr("search_key");
//var search_item=getparastr("search_item");
//var max_row=parseInt(getparastr("max_row"));

var search_val='';
var search_item='10';
var max_row=MAXROWS;
var baklist;
var totallist;
var sortlist;
var intname;
var showflag=false;

var selectflag=0;
var mf_curnum = 0;

function init(f)
{	
	cfg2Form(f);

	lblRecordCount.innerHTML=baklist.length;
	
	if (max_row)
	{
	   document.getElementById("txtMaxRows").value = max_row;
	}
	
	//sortFunc(1);
	//changekey();

	if (search_item && search_val)
	{
		document.getElementById("search_item").value=search_item;
		//changekey();
		document.getElementById("search_val").value=decodeURIComponent(search_val);
	}

	showflag=false;
}

function getrefresh()
{
	GURL(cPage+"?search_key="+encodeURIComponent(document.getElementById('search_val').value)+"&search_item="+document.getElementById('search_item').value+"&max_row="+document.getElementById('txtMaxRows').value+"");
}

function getrefresh()
{
	GURL(cPage+"?search_key="+encodeURIComponent(document.getElementById('search_val').value)+"&search_item="+document.getElementById('search_item').value+"&max_row="
	+document.getElementById('txtMaxRows').value+"&last_page="+document.getElementById('txtCurPageIndex').value+"");
}


function go(maxrows, pageIndex, list)
{
	var pages = 0;
	
	if (selectflag)
	{
		selectall();
	}
	else
	{
		for (j=0;j<dhcpd_client_frame.checklist.length;j++)
		{	
		    if (dhcpd_client_frame.checklist[j]==1)
		    {
		    	dhcpd_client_frame.checklist[j]=0;
		    }
		}
	}
	
    list=sortlist;
	
    if (maxrows > baklist.length)
    {
        maxrows=baklist.length;
    }
    
	if (baklist.length%maxrows == 0)
	{
	    pages = parseInt(baklist.length/maxrows);
	}
	else
	{
	    pages = parseInt(baklist.length/maxrows) + 1;
	}
	
    if (pageIndex > pages)
    {
        pageIndex=pages;
		document.getElementById("txtCurPageIndex").value=pageIndex;
    }

	if (baklist.length == 0)
	{
		lblCurPage.innerHTML=1;
	    lblTotalPage.innerHTML=1;
	}
	else
	{
		lblCurPage.innerHTML=pageIndex;
		if (baklist.length%maxrows == 0)
		{
			lblTotalPage.innerHTML=parseInt(baklist.length/maxrows);
		}
		else 
		{
			lblTotalPage.innerHTML=parseInt(baklist.length/maxrows)+1;
		}
	}
    
    mf_curnum = lblRecordCount.innerHTML=baklist.length;
    
    document.getElementById("dhcpd_client_frame").style.height=22+21*(maxrows);
   
    newlist=list.slice(maxrows*(pageIndex-1),maxrows*pageIndex);
    list=newlist; 
	
    dhcpd_client_frame.show_dhcpd_client_list(list);

}

function go_specific_page(type)
{
	var pageIndex = document.getElementById("txtCurPageIndex").value;
	var maxrows = document.getElementById("txtMaxRows").value;
	var pages = 0;
	
	if (maxrows == '' || 0 == maxrows)
	{
		return;
	}
    else if(!/(^[0-9]{1,3}$)/.test(maxrows))
    {	
   		return;	
    }

    if (pageIndex == '' || 0 == pageIndex)
    {
        return;
    }
    else if(!/(^[0-9]{1,3}$)/.test(pageIndex))
    {
        return;
    }
	pageIndex = parseInt(pageIndex);/*trip the '0' if exist*/
	maxrows = parseInt(maxrows);    /*trip the '0' if exist*/
	
	
	if (baklist.length%maxrows == 0)
	{
	    pages = parseInt(baklist.length/maxrows);
	}
	else
	{
	    pages = parseInt(baklist.length/maxrows) + 1;
	}

	switch(type)
	{
		case 1:  // first page
			pageIndex = 1;
			break;		
		case 2:  //previous
			if(pages==0)
				pageIndex = 1;
			else	
				pageIndex = pageIndex==1?pages:(pageIndex-1);//1:(pageIndex-1);
			break;		
		case 3:  //next
			if(pages==0)
				pageIndex = 1;
			else
				pageIndex = pageIndex==pages?1:(pageIndex+1);//pages:(pageIndex+1)
			break;		
		case 4: //last
			pageIndex = pages==0?1:pages;
			break;
		case 5: //all
			pageIndex = 1;
			maxrows = baklist.length;
			break;	
	}

	document.getElementById("txtCurPageIndex").value = pageIndex;
	document.getElementById("txtMaxRows").value = maxrows;
	go(maxrows, pageIndex, dhcpd_client_frame.dhcpd_client_list);
}

function gopage()
{
    var pageIndex = document.getElementById("txtCurPageIndex").value;
	var maxrows=document.getElementById("txtMaxRows").value;
	
	if (maxrows == '' || 0 == maxrows)
	{
		return;
	}
    else if(!/(^[0-9]{1,3}$)/.test(maxrows))
    {	
   		return;	
    }

    if (pageIndex == '' || 0 == pageIndex)
    {
        return;
    }
    else if(!/(^[0-9]{1,3}$)/.test(pageIndex))
    {
        return;
    }
	pageIndex = parseInt(pageIndex);/*trip the '0' if exist*/
	maxrows = parseInt(maxrows);    /*trip the '0' if exist*/

	go(maxrows, pageIndex, dhcpd_client_frame.dhcpd_client_list);

    dhcpd_client_frame.dhcpd_client_list=newlist;
    dhcpd_client_frame.show_dhcpd_client_list(dhcpd_client_frame.dhcpd_client_list);
}

function search_go(f, pattern)
{
	var search_pattern = parseInt(f.search_item.value);  
	var key = f.search_val.value;
	var key_tmp = key;
	var search_mac_list = new Array();
	var len = baklist.length;
	var list = baklist;
	var i = 0;
	var find_num = 0;
	var baklist_tmp = baklist;
	var sortlist_tmp = sortlist;
	var pageIndex = 1;
	var maxrows = SEARCH_MAXROWS;
	var value = '';
	
	if(key=='')
		return;
	for (i=0; i<len; i++)
    {	
		value = GetVal(list, i, search_pattern);
		value = (search_pattern==1?value.toUpperCase():value);
		key = (search_pattern==1?key.toUpperCase():key);

		if(key == value)
		{
			search_mac_list.push(list[i]);	
			find_num++;
		}
			
	}
	if(0==find_num)
	{
		alert('根据关键字("'+key_tmp +'")没有找到相应的表项！');
	}

	document.getElementById("txtCurPageIndex").value = pageIndex;
	document.getElementById("txtMaxRows").value = maxrows;
	sortlist = baklist = search_mac_list;
	go(maxrows, pageIndex, search_mac_list);
	baklist = baklist_tmp;
	sortlist = sortlist_tmp;
}

/*
function generate_ints()
{
  preint='';
  var intlist=new Array();
  for (i=0;i<totallist.length;i++)
  {
    var pattern=/VLAN\d+/;;
    tmp=totallist[i];
    tt=tmp.match(pattern);
    if (preint != tt[0])
    {
      intlist.push(tt[0]);
      preint=tt[0];
    }
  }
  return datelist;
}

function changekey()
{
	var m='';
	f=document.frmSetup;
	switch (Number(f.search_item.value)){
	case 0:
	    m+='';
	    f.search_start.disabled=true;
	    break;
	case 1:
	    m+='<input name="search_val" id="search_val" type="text" size="15" maxlength=64 onkeydown= "if (event.keyCode==13) {search_go(f,pattern);after_search()}">';
	    f.search_start.disabled=false;
	    break;
	case 2:
	    m+='<input name="search_val" id="search_val" type="text" size="15" maxlength=64 onkeydown= "if (event.keyCode==13) {search_go(f,pattern);after_search()}">';
	    f.search_start.disabled=false;
	    break;
	case 3:
	    m+='<input name="search_val" id="search_val" type="text" size="15" maxlength=64 onkeydown= "if (event.keyCode==13) {search_go(f,pattern);after_search()}">';
	    f.search_start.disabled=false;
	    break;

	//case 4:
    //    m+='<select name="search_val" id="search_val" >';
     //   intname = generate_ints();
	 //   for (i=0;i<intname.length;i++)
	 //   {
	  //    m+='<option value='+intname[i]+'>'+intname[i]+'</option>';
	 //   }
	  //  m+='</select>';
	  //  f.search_start.disabled=false;
	  //  break;

	default:
	    m+='<input name="search_val" id="search_val" type="text" size="15" maxlength=64>';
	    f.search_start.disabled=false;
	    break;
	
	}
	document.getElementById("keywords").innerHTML = m;
}
*/

function myhelp()
{
	popHelp('help.htm#wlan_ap_client');
}

function after_search()
{
    turn = 1;
    initclass=["no","up","no","no"];
    //sortFunc(2,ip_sort);
}



</SCRIPT></head>
<body bgcolor=#ffffff leftMargin=0 topMargin=0 MARGINWIDTH=0 MARGINHEIGHT=0 border=0 onload=init(document.frmSetup)>
<script>pageHead(webVars,PROD_MODEL);</script>
<SCRIPT language=JavaScript>buildNavigatorTab(wlan_ap, 5)</SCRIPT>
<table>
	<form name=frmSetup method=POST action='goform/command'>
		<TR><TD class=titleCell colSpan=3>无线网络接入客户端列表</TD></TR>
		<td width=540 align=top>
		<table>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>

			<TR id=search_index class="textCell">
					<TD colSpan=3 align="left">按关键字过滤：
						<select name=search_item id=search_item ><!--onchange=changekey()-->
							<option value=1>MAC地址
						</select>
						关键字：
	   					<input type=text name="search_val" id="search_val" size=20 maxlength=63  onkeydown="if (event.keyCode==13) search_go(frmSetup,pattern)">
						<INPUT type=button name=search_start id=search_start class=button value=查询 onClick="search_go(frmSetup, pattern);">
						<INPUT type=button name=search_stop id=search_stop class=middlebutton value=显示全部 onClick="go_specific_page(5);"></TD>
						

			<TR><td width=350 colSpan=3 align=center>
				<iframe name="dhcpd_client_frame" id="dhcpd_client_frame" src="wlan_ap_client_list.asp" width=600 height=232 align=left scrolling=no ></iframe>
			<tr><td width=30>
			   <table border="0" cellspacing="0" cellpadding="0" width="100%">
					<tr class="textCell">
						<td noWrap="noWrap" align="right">
							<span id="lblSecordHead">第</span>
							<span id="lblCurPage">1</span>
							<span >/页/共</span>
							<span id="lblTotalPage">1</span>
							<span id="lblFirstHead">页 共</span>
							<span id="lblRecordCount">0</span>
							<span id="lblPageHead"> 条记录 每页</span>
							<input name="txtMaxRows" value='10' type="text" id="txtMaxRows" style="BORDER-TOP-WIDTH:1px; BORDER-LEFT-WIDTH:1px; IME-MODE:disabled; BORDER-BOTTOM-WIDTH:1px; HEIGHT:18px; TEXT-ALIGN:center; BORDER-RIGHT-WIDTH:1px" maxlength="3" size="4" validate="+INT" onkeydown="if (event.keyCode==13) gopage()"/>
							<span id="lblPerPage">行&nbsp;</span>
						</td>
						<td align="right" style="width:5px" valign="middle"><IMG name="imgFirst" id="imgFirst" title=第一页 src="images/First-D.gif" onclick=go_specific_page(1)></td><td width=1>&nbsp;</td>
						<td align="right"  style="width:5px" valign="middle"><IMG name="imgPrev" id="imgPrev" title=上一页 src="images/Previous-D.gif" onclick=go_specific_page(2)></td><td width=1>&nbsp;</td>
						<td  align="right"  style="width:5px" valign="middle"><input name="txtCurPageIndex" type="text" id="txtCurPageIndex" style="BORDER-TOP-WIDTH:1px; BORDER-LEFT-WIDTH:1px; IME-MODE:disabled; BORDER-BOTTOM-WIDTH:1px; HEIGHT:18px; TEXT-ALIGN:center; BORDER-RIGHT-WIDTH:1px" maxlength="3" size="4" value="1" validate="+INT" 
							onkeydown="if (event.keyCode==13) gopage()" /></td><td width=1>&nbsp;</td>
						<td  align="right"  style="width:5px" valign="middle"><input type="button" name="btnGoPage" value="Go" id="btnGoPage" onclick="gopage()"></td><td width=1>&nbsp;</td>
						<td  align="right"  style="width:5px" valign="middle"> <IMG name="imgNext" id="imgNext" title=下一页 src="images/Next.gif" onclick=go_specific_page(3) ></td><td width=1>&nbsp;</td>
						<td  align="right" style="width:5px" valign="middle"><IMG name="imgLast" id="imgLast" title=最后一页 src="images/Last.gif" onclick=go_specific_page(4)></td>
					</tr>
				</table>
		</TABLE>
			
	</FORM>
</table>
</BODY>
<script>pageTail(str_copyright);</script>
</HTML>






