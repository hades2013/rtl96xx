<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<script language=JavaScript>
var str_copyright =<%CGI_GET_COPYRIGHT();%>
var PROD_MODEL = <%CGI_GET_MODEL();%>
//var str_copyright ='Copyright (c) 2013 xxx Technology Corp';
//var PROD_MODEL = 'EPN104N';

</SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=navigatortab.js></SCRIPT>
<script language=JavaScript>
pageTitle();
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
//var webVars = new Array( 'EPN104N', '0', '0');


var pattern=/\d+;([\w:]+)/;
var turn = 1;
var initclass=["no"];

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

	showflag=false;
}
				  
function preDownload() 
{
	var f = document.forms[0];
	f.action = PROD_MODEL+"-syslog.bin";
	f.submit();
}

function preClear() 
{
	var f = document.forms[0];
	diffCfg('config_0', 'config', 'syslog');	
	subForm(f,'goform/command','CONFIG', cPage);	
}
/*
function GetVal(strArray, primary, index)
{	
	if (strArray.length > 0)
	{
		var vs = strArray[primary].split(';');
		if (vs.length > 0) 
		{	
			return vs[index];
		}
	}
	return false;
}
function writeMssge()
{

	var str = '';
	var i = 0;
	var INTERNAL = 5;
	var NO;
	var Timestamp;
	var Category;
	var Severity;
	var Message;
	var modules;
	var info = strArray[0].split(";");
	var len = info.length-1; 
	for(i=0; i< len; i=i+INTERNAL)
	{
		//NO = GetVal(strArray, 0, i);
		Timestamp = GetVal(strArray, 0, i);
		Category = GetVal(strArray, 0, i+1);
		Severity = GetVal(strArray, 0, i+2);
		modules= GetVal(strArray, 0, i+3);
		Message = GetVal(strArray, 0, i+4);//alert(Timestamp)
		
		if(-1 != Timestamp.indexOf("#"))
		{
			Timestamp = Timestamp.replace('#','');//alert(Timestamp)
		}

		str +=	'<TR class="alt0" onmouseover="repeatTRMouseOver(this);" onmouseout="repeatTRMouseOut(this);" >' +
				//'<TD class=intcol id=1_1_1> <span>'+NO+'</span></TD>' +
				'<TD class=def id=1_1_2> <span> '+Timestamp+'</span></TD>' +
				'<TD class=enumcol id=1_1_3> <span> '+Category+'</span></TD>' +
				'<TD class=def id=1_1_4> <span> '+Severity+' </span></TD>' +
				'<TD class=def id=1_1_4> <span> '+modules+' </span></TD>' +
				'<TD class=def id=1_1_5 name = result> <span> '+Message+'</span></TD>' +
				'<TD class="def_lastCol">&nbsp;</TD>' +
				'</TR>';		
	}
	document.write(str);
}
*/
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
</script>
</head>

<body onload=init(document.frmSetup)>
<script>pageHead(webVars,PROD_MODEL);</script>
<SCRIPT language=JavaScript>buildNavigatorTab(system_syslog, 2)</SCRIPT>
<form name=system_log action=goform/command method=post>
<INPUT type=hidden name=CMD value=SYS_LOG>
<INPUT type=hidden name=GO value=system_log.asp>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td width=100% class=greybluebg>日志信息</td>
</tr>
<!--
<tr>
<table width=100% >
  <tr>
    <th scope="col">日期/时间</th>
    <th scope="col">类型</th>
    <th scope="col">级别</th>
    <th scope="col">模块</th>
    <th scope="col">消息</th>
  </tr>
<tbody id='xuiRepeatTableBody1'>
<script>writeMssge();</script>
</tbody>
</table>

<!--
	<td class=bggrey>

<textarea rows=20 cols=105 style='font-size:12;FONT-FAMILY:Arial,Helvetica, sans-serif' readonly>
&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp日期/时间&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp类型/级别&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp 模块 &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp消息

</textarea>

	</td>
   
</tr> -->
			<TR><td width=350 colSpan=3 align=center>
				<iframe name="dhcpd_client_frame" id="dhcpd_client_frame" src="syslog_msg.asp" width=600 height=232 align=left scrolling=no ></iframe>
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
</table>
</form>
<input type=button value=" 导出 " onclick=preDownload()>
<input type=button value=" 清除 " onclick=preClear()>
<input type=button value=" 刷新 " onclick=refresh(cPage)></td>

<script>pageTail(str_copyright);</script>
</body>
</html>
