<HTML><HEAD>
<META NAME=Copyright CONTENT="Copyright (c) 2007 H3C Corporation. All Rights Reserved.">
<META http-equiv=Content-Type content=text/html;charset=GB2312>
<META HTTP-EQUIV=Pragma CONTENT=no-cache>
<TITLE>无线接入控制</TITLE>

<link rel="stylesheet" href="style.css" type="text/css">
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=navigatortab.js></SCRIPT>
<SCRIPT language=JavaScript src=valid.js></SCRIPT>
<script language=JavaScript>
var str_copyright =<%CGI_GET_COPYRIGHT();%>
var PROD_MODEL = <%CGI_GET_MODEL();%>
</SCRIPT>
<SCRIPT language=JavaScript>
pageTitle();
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var wareless_mac_filter = new Array(<% CGI_MAC_FILTER(); %>); 

//var webVars = new Array( 'EPN104', '0', '0', '24');
//var wareless_mac_filter = new Array('1;1;50;5;1'); /*filter_en;mode(1:only_allow,0:only_forbidden;wmf_maxnum;wmf_curr_num;wlan_en*/


//var search_val=getparastr("search_key");
//var search_item=getparastr("search_item");
//var last_page=parseInt(getparastr("last_page"));
//var max_row=parseInt(getparastr("max_row"));

var search_val='';
var search_item=3;
var last_page=1;
var max_row=MAXROWS;

var baklist;
var totallist;
var sortlist;

var selectflag=0;
var mf_curnum = 0;


var pattern=/\d+;(.+);(.*);/;
var turn = 1;
var initclass=["up","no"];

var del_mac_list = '';
var showflag=true;

var mac_filter_en = GetVal(wareless_mac_filter, 0, 0);
var mac_filter_mode = GetVal(wareless_mac_filter, 0, 1);
var wmf_maxnum  = parseInt(GetVal(wareless_mac_filter, 0, 2));
var wlan_en  = parseInt(GetVal(wareless_mac_filter, 0, 4));

function myhelp()
{
	popupHelp('help.htm#wlan_ap_acl');
}


function Init_val(f)
{	
	f.wmf_en_box.checked = mac_filter_en=='1'?true:false;
	f.wmf_mode_box[0].checked=mac_filter_mode=='1'?true:false;
	f.wmf_mode_box[1].checked=mac_filter_mode=='0'?true:false;
}


function init(f)
{
	Init_val(f);
    cfg2Form(f);
   
    
	lblRecordCount.innerHTML=baklist.length;
	if (max_row)
	{
	   document.getElementById("txtMaxRows").value = max_row;
	}
	/*
    if (baklist.length == 0)
    {
        document.getElementById("op_selctall").disabled = true;
    }
	*/
	if (search_item && search_val)
	{
		document.getElementById("search_item").value=search_item;
		document.getElementById("search_val").value=decodeURIComponent(search_val);
	}
	 
	if (last_page)
	{
		document.getElementById("txtCurPageIndex").value = last_page;
	}
	f.wmf_mac_addr_box.value = '';
	f.wmf_comment_box.value = '';
	showflag=false;
	gopage();
	enable_option(f);
	return;
}

function checked_num()
{
	var num = 0;
	var list = wlan_ap_mac_list.document.getElementsByName("checkbox_list");
	var len = list.length;
	var i = 0;
	var num_checked = 0;
	var msg = '';
	for (i=0;i<len;i++)
	{
		if(list[i].checked == true)
			num++;
	}
	return num;
}

function enable_option(f)
{	
    if(wlan_en=='0' || f.wmf_en_box.checked == false )
	{
		cfg2Form(f);
		f.wmf_mode_box[0].disabled=true;
		f.wmf_mode_box[1].disabled=true;
		document.getElementById("wmf_mac_addr_box").disabled=true;
		document.getElementById("wmf_comment_box").disabled=true;
		document.getElementById("ipmacInput").disabled=true;
		document.getElementById("ipmacOutput").disabled=true;
		document.getElementById("op_del").disabled=true;
		document.getElementById("op_edit").disabled=true;
		f.wmf_en_box.disabled = (wlan_en=='0'?true:false);
		enalbe_checkbox_list(0);
	}
	else if(wlan_en=='1' && f.wmf_en_box.checked == true )
	{
		f.wmf_mode_box[0].disabled=false;
		f.wmf_mode_box[1].disabled=false;
		document.getElementById("wmf_mac_addr_box").disabled=false;
		document.getElementById("wmf_comment_box").disabled=false;
		document.getElementById("ipmacInput").disabled=false;
		document.getElementById("ipmacOutput").disabled=false;
		document.getElementById("op_del").disabled=(checked_num()>0?false:true);
		document.getElementById("op_edit").disabled=(checked_num()==1?false:true);
		enalbe_checkbox_list(1);
	}
	document.getElementById("wmf_add_button").disabled=wlan_en=='0'?true:false;	
}

function editThem(f)
{
	var list = wlan_ap_mac_list.document.getElementsByName("checkbox_list");
	var len = list.length;
	var i = 0;
	var num_checked = 0;
	var msg = '';
	var mac_dec = '';
	var index = '';
	var value = '';
	
	for (i=0;i<len;i++)
	{
		if(list[i].checked==true)
		{	
			index = list[i].value;
			break;
		}
	}
	
	for (i=0;i<wlan_ap_mac_list.mac_list.length;i++)
	{
		if(GetVal(wlan_ap_mac_list.mac_list, i, 0)==index)	
		{
			msg = GetVal(wlan_ap_mac_list.mac_list, i, 1);
			msg = msg.toUpperCase();
			mac_dec = GetVal(wlan_ap_mac_list.mac_list, i, 2);	
			break;
		}
	}

	f.wmf_mac_addr_box.value = msg;
	f.wmf_comment_box.value = mac_dec;
}

function getrefresh()
{
	GURL(cPage+"?search_key="+encodeURIComponent(document.getElementById('search_val').value)+"&search_item="+document.getElementById('search_item').value+"&max_row="
	+document.getElementById('txtMaxRows').value+"&last_page="+document.getElementById('txtCurPageIndex').value+"");
}

function enalbe_checkbox_list(action)
{
	var list = wlan_ap_mac_list.document.getElementsByName("checkbox_list");
	var len = list.length;
	var i = 0; 
	for (i=0;i<len;i++)
	{
		list[i].disabled=(action==1?false:true);
	}
}


function enalbe_del_bt(action)
{
	document.getElementById("op_del").disabled=action==1?false:true;
}

function enalbe_edit_bt(action)
{
	document.getElementById("op_edit").disabled=action==1?false:true;
}


function go(maxrows, pageIndex, mac_list)
{
	var pages = 0;
	
	if (selectflag)
	{
		selectall();
	}
	else
	{
		for (j=0;j<wlan_ap_mac_list.checklist.length;j++)
		{	
		    if (wlan_ap_mac_list.checklist[j]==1)
		    {
		    	wlan_ap_mac_list.checklist[j]=0;
		    }
		}
	    document.getElementById("op_del").disabled=true;
	}
	
    mac_list=sortlist;
	
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
    
    document.getElementById("wlan_ap_mac_list").style.height=22+21*(maxrows);
   
    newlist=mac_list.slice(maxrows*(pageIndex-1),maxrows*pageIndex);
    mac_list=newlist; 
	
    wlan_ap_mac_list.show_acl_mac_list(mac_list);
	enalbe_checkbox_list(document.frmSetup.wmf_en_box.checked==true?1:0);
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
	go(maxrows, pageIndex, wlan_ap_mac_list.mac_list);
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

	go(maxrows, pageIndex, wlan_ap_mac_list.mac_list);
}



function selectall()
{
    //document.getElementById("op_selctall").value=selectflag?"全选":"取消";
    for (i=0;i<wlan_ap_mac_list.mac_list.length;i++)
    {
    	tmpid="mytable"+i;
    	wlan_ap_mac_list.document.getElementById(tmpid).style.backgroundColor=selectflag?"":"#80b2f0";
    	wlan_ap_mac_list.checklist[i]=selectflag?0:1;
    }
    document.getElementById("op_del").disabled=selectflag?true:false;
    
    selectflag=!selectflag;
}


function note_check(P)
{
	if(chinese_check(P))
		P.value="";
}

function open_macfilter_config()
{
	if (Number(mf_curnum)>=Number(mf_maxnum))
	{
		alert("MAC接入控制表项已达到"+Number(mf_maxnum)+"条上限，不能再增加!");
		return;
	}	
	
	rtconfigWin=window.showModalDialog("wlan_ap_acl_cfg.asp?datetime="+new Date().toString(),"","center=yes;dialogWidth=550px;dialogHeight=180px;status=no;scroll=no;help=no;resizable=yes");
	if (rtconfigWin == true)
	{
		disable_all_ops(wlan_ap_mac_list, "disableclick");
		GURL(cPage+"?search_key="+encodeURIComponent(document.getElementById('search_val').value)+"&search_item="+document.getElementById('search_item').value+"&max_row="
		+document.getElementById('txtMaxRows').value+"&last_page=9999"+"");
	}
}

function open_wlan_client_client(f)
{
	if (Number(mf_curnum)>=Number(mf_maxnum))
	{
		alert("MAC接入控制表项已达到"+Number(mf_maxnum)+"条上限，不能再增加!");
		return;
	}

	rtconfigWin=window.showModalDialog("wlan_client_mac_list.asp?datetime="+new Date().toString(),"","center=yes;dialogWidth=450px;dialogHeight=400px;status=no;scroll=yes;help=no;resizable=yes");
	if (rtconfigWin == true)
	{
		GURL(cPage);
	}
}

function search_go(f, pattern)
{
	var search_pattern = parseInt(f.search_item.value);  //alert("search_pattern:"+search_pattern)//1:mac, 2:mac_dec
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

function name_check(P)
{
    if (chinese_check(P))
    {	
        return false;
    }
	return true;
}

function form_check(f)
{	
	if(MAC_Check(f.wmf_mac_addr_box))
		return false;
	
	if(f.wmf_comment_box.value=='')
	{
		alert("MAC描述不能为空！")
		return false;
	}
	else if(false==name_check(f.wmf_comment_box))
	{
		return false;
	}
		
	return true;	
}


var ACTION_MAC_MODE =0;
var ACTION_MAC_MODE_AND_ADD =1;
var ACTION_MAC_DEL = 2;
var ACTION_MAC_UPLOAD_MAC = 3;
var ACTION_MAC_DOWNLOAD_MAC = 4;

function mac_exist(f)
{
	var mac = f.wmf_mac_addr_box.value;
	mac = mac.toUpperCase();
	var val = '';
	for (i=0;i<wlan_ap_mac_list.mac_list.length;i++)
	{
		val = GetVal(wlan_ap_mac_list.mac_list, i, 1);
		val = val.toUpperCase();
		if(val==mac)	
		{	
			return true;
		}
	}
	return false;
}


function submitF(f, action)
{	
	var mac_filter_en = 0;
	var mac_filter_mode = 0;
	var mac_filter_mac = '';
	var mac_filter_mac_dec = '';
	var now_coumt = 0;	
	var del_mac = '';
	var upload_mac_filename = '';
	var download_mac_filename = 'mac_list.config';
	
	switch(parseInt(action))
	{
		case ACTION_MAC_MODE:
			if(f.wmf_mac_addr_box.value!='' || f.wmf_comment_box.value!='')
			{	
				if(false==form_check(f)) 
					return;	
				if(0==mac_exist(f))
				{
					mf_curnum++;
					if(mf_curnum>wmf_maxnum)
					{
						alert('MAC过滤表项最多支持'+wmf_maxnum+'个MAC地址，不能再增加!');
						return;
					}				
				}				

				action = ACTION_MAC_MODE_AND_ADD;
			}
			/*	
			//if(Number(mf_curnum)==0 && (f.wmf_mode_box[0].checked==true))
			if(Number(mf_curnum)==0 && (f.wmf_mode_box[0].checked==true))
			{
				ret=confirm("仅允许条件下，如果MAC接入控制表为空，则会禁止所有MAC地址接入无线网络，确定要提交?");
				if (ret != true)
				{
					return;
				}
			}
			*/
			mac_filter_en = f.wmf_en_box.checked==true?1:0;
			mac_filter_mode = f.wmf_mode_box[0].checked==true?1:0;
			mac_filter_mac = f.wmf_mac_addr_box.value;
			mac_filter_mac_dec = f.wmf_comment_box.value;
			diffCfg("wmf_en_box", "mac_filter_en", mac_filter_en);
			diffCfg("wmf_mode_box", "mac_filter_mode", mac_filter_mode);
			if(mac_filter_mac!='' && mac_filter_mac_dec!='')
			{
				diffCfg("wmf_mac_addr_box", "mac_filter_mac", mac_filter_mac);
				diffCfg("wmf_comment_box", "mac_filter_mac_dec", mac_filter_mac_dec);			
			}

			break;

		case ACTION_MAC_DEL:
			if(true==confirm("确定要删除所有选中表项吗?"))
			{
				var list = wlan_ap_mac_list.document.getElementsByName("checkbox_list");
				var len = list.length;
				var i = 0;
				var del_coumt = 0;
				var del_list = 0
				for (i=0;i<len;i++)
				{
					if(list[i].checked==true)
					{
						del_coumt++
						del_list |= 1<<(parseInt(list[i].value));
					}					
				}
				if(del_list=='')
				{
					alert("请选择要删除的表项！");
					return;
				}
				if(del_coumt>32)
				{
					alert("一次最多只能删除32个表项！");
					return;				
				}
				now_coumt = mf_curnum = (mf_curnum-del_coumt);
				diffCfg("param", "del_list", del_list);
			}
			break;
			
		case ACTION_MAC_UPLOAD_MAC:
			upload_mac_filename = "mac_list.config";
			diffCfg("upload_mac", "upload_mac_file", upload_mac_filename);
			break;
			
		case ACTION_MAC_DOWNLOAD_MAC:
			diffCfg("download_mac", "download_file", download_mac_filename);
			break;	
			
		default:
			return;
	}	
	subForm(f,'goform/command', 'MAC_FILTER', 'wlan_ap_acl.asp');
}
/*
function submitF(f) 
{
	if (Number(wmf_curnum)==0 && Number(wmf_mode)==1 &&  Number(wmf_en)==1)
	{
	 	alert("仅允许条件下，MAC列表不能为空!");
		return;
	}

	form2Cfg(f);
	Disable_button_byid("id_confirm");
	Disable_button_byid("id_cancel");
	subForm(f,'/goform/aspForm','UpdateWlanMacFilterList',cPage);
}

function init_wlan_acl_page(f)
{
    var wlan_enable = getCfg("wlanEnable");

	if (wlan_enable == '0')
    {
        f.wmf_en_box.disabled = true;
        document.getElementById("line_enable").disabled = true;
    }
    else
    {
        f.wmf_en_box.disabled = false;
        document.getElementById("line_enable").disabled = false;
    }
    
	if (f.wmf_en_box.checked == false || wlan_enable == '0')
	{
		f.wmf_mode_box[0].disabled         = true;
		f.wmf_mode_box[1].disabled         = true;
		f.wmf_mac_addr_box.disabled     = true;
		f.wmf_comment_box.disabled      = true;
		f.wmf_add_button.disabled       = true;
		f.checkbox_select_all.disabled = true;
		f.wmf_refresh_button.disabled  = true;
		f.wmf_edit_button.disabled     = true;
		f.wmf_del_button.disabled      = true;
		f.wmf_upload_button.disabled   = true;
		f.wmf_download_button.disabled = true;
                //_getrefresh("wlan_acl_mac_list.asp", "wlan_acl_mac_list","display=no");
		document.getElementById("wlan_acl_mac_list").src='wlan_acl_mac_list.asp?display=no';
	}
	else
	{
		f.wmf_mode_box[0].disabled         = false;
		f.wmf_mode_box[1].disabled         = false;
		f.wmf_mac_addr_box.disabled     = false;
		f.wmf_comment_box.disabled      = false;	
		f.wmf_add_button.disabled       = false;
		f.checkbox_select_all.disabled = false;
		f.wmf_refresh_button.disabled  = false;
		f.wmf_edit_button.disabled     = false;
		f.wmf_del_button.disabled      = false;
		f.wmf_upload_button.disabled   = false;
		f.wmf_download_button.disabled = false;
		//_contrlOutputButton(wmf_curnum);
                //_getrefresh("wlan_acl_mac_list.asp", "wlan_acl_mac_list","display=yes");
		document.getElementById("wlan_acl_mac_list").src='wlan_acl_mac_list.asp?display=yes';
	}
}

function fun_select_all()
{
	if (Number(wmf_curnum) != wlan_acl_mac_list.wmf_list.length)
	{
		getrefresh();
		setTimeout("fun_select_all()",100);
	}
	else
	{		
    	f = document.frmSetup;
    	f.action = '/goform/aspForm';
    	f.CMD.value = 'SelectAllWlanMacFilterList';
    	f.GO.value = cPage;
    	f.param.value = 1;
    	wlan_acl_mac_list.fun_select_all(true,Number(wmf_curnum));
    	document.getElementById("checkbox_select_all").onclick = fun_cancel_all;
    	f.submit();
	}
}

function fun_cancel_all()
{
	if (Number(wmf_curnum) != wlan_acl_mac_list.wmf_list.length)
	{
		getrefresh();
		setTimeout("fun_cancel_all()",100);		
	}
	else
	{
    	f = document.frmSetup;
    	f.action = '/goform/aspForm';
    	f.CMD.value = 'SelectAllWlanMacFilterList';
    	f.GO.value = cPage;
    	f.param.value = 0;
    	wlan_acl_mac_list.fun_select_all(false,Number(wmf_curnum));
    	document.getElementById("checkbox_select_all").onclick = fun_select_all;
    	f.submit();
	}
}

function add_wmf(f)
{
	if (f.wmf_mac_addr_box.value == "")
	{
		alert("请输入MAC地址!");
		return;
	}
	else if (MAC_Check(f.wmf_mac_addr_box) != 0)
	{
		f.wmf_mac_addr_box.value = "";
		return;
	}

	if ((Number(wmf_curnum) >= Number(wmf_maxnum)) && (f.wmf_add_button.value != "修改"))
	{
		alert("MAC过滤表项已满，不能再增加!");
		return;
	}
	
	var msg     = f.wmf_mac_addr_box.value+' '+f.wmf_comment_box.value;
	f.CMD.value = 'AddWlanMacFilterList';
	f.GO.value  = cPage;
	f.param.value   = msg;
	f.action="/goform/aspForm";
	f.submit();
}

function uploadFile(f)
{

	if(window.win && !window.win.closed)
	{
		 window.win.close();
	}
	win = window.open('pop_import_wlan_macfilter.asp','导入MAC地址过滤表',
          'width=360,height=220,toolbar=no,status=no,scrollbars=no,resizable=no,menubar=no');
	var x=screen.availWidth / 4;
	var y=screen.availHeight / 4;
	win.moveTo(x,y);

}

function downloadFile(f)
{

	f.action="ER3280g_wlan_macfilter.cfg";
	f.GO.value=cPage;
	f.CMD.value="";
	f.submit();
	
}
*/

</SCRIPT></head>
<body bgcolor=#ffffff leftMargin=0 topMargin=0 MARGINWIDTH=0 MARGINHEIGHT=0 border=0 onload=init(document.frmSetup)>
<SCRIPT>pageHead(webVars, PROD_MODEL);</SCRIPT>
<SCRIPT language=JavaScript>buildNavigatorTab(wlan_ap, 3)</SCRIPT>
<table width="100%">
	<form name=frmSetup method=POST action='goform/command'>
		<TR><TD class=titleCell colSpan=3>无线网络接入控制</TD></TR>
		<td width=540 align=top>
	<table class=globtable>
	<tr><td colSpan=4>&nbsp;
	<tr>
		<input type=hidden name=CMD>
		<input type=hidden name=GO>
		<input type=hidden name=now_coumt>
		<input type=hidden name=param>
		<input type=hidden name=upload_mac>
		<input type=hidden name=download_mac>
	<td width=30>&nbsp;<td width=500 valign=top>
		<TABLE cellSpacing=0 cellPadding=2 border=0 width=100%>
			<TR class=spaceLine><TD colspan=3 class=spaceLine>
			<TR id=line_enable>
				<TD width='500' >启用MAC地址过滤功能：</TD>
				<TD class='textCell'><input name="wmf_en_box" type=checkbox onclick=enable_option(frmSetup)> 
				<TD width=20>&nbsp;</TD></tr>
			<TR class='textCell' id=wmf_block_line0>
				<TD width=350 class=nameTextCell>仅允许MAC地址列表中的MAC接入：</TD>						
				<TD class='textCell' width=150><input name="wmf_mode_box"  type="radio"><!-- onclick=change_wmf_mode(frmSetup)-->
				<TD width=30>
						
			<TR class='textCell' id=wmf_block_line1>
				<TD width=300 class=nameTextCell>仅禁止MAC地址列表中的MAC接入：</TD>						
				<TD class='textCell' width=150><input name="wmf_mode_box" type="radio" ><!--onclick=change_wmf_mode(frmSetup)-->
				<TD width=30>
					   
			<TR class='textCell' id=wmf_block_line2>
				<TD width=150 class=nameTextCell></TD>					
				<TD class='textCell' width=150>&nbsp;
				<TD width=30>
						
			<TR class='textCell' id=wmf_table_line>
				<TD width=300 class=nameTextCell align=left><b>增加/修改MAC过滤表项：</b></TD>				
				<TD class='textCell' width=150>
				<TD width=30>
			<TR class='textCell' id=wmf_block_line3>
				<TD width=300 class=nameTextCell>MAC地址：</TD>
				<TD class='textCell' width=150><input name="wmf_mac_addr_box" id="wmf_mac_addr_box" type="text" size="19" maxlength="17"></TD>
				<TD >(xx:xx:xx:xx:xx:xx)
					
			<TR class='textCell' id=wmf_comment_line>
				<TD width=300 class=nameTextCell>描述：</TD>
				<TD class='textCell' width=150><input name="wmf_comment_box" id="wmf_comment_box" type="text" size="19" maxlength=31></TD>			
				<TD >(1-31个字符)					
				
		</TABLE>
		<TR class=spaceLine><TD colspan=3 class=spaceLine></tr>
	</table>
		<table>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>		
			<tr class=textCell>
				<TD width=150>
					<input type=button class=button name=wmf_add_button id=wmf_add_button value=确定 onClick="submitF(frmSetup, ACTION_MAC_MODE);">
					<input type=button class=button name=wmf_cancel_button id=wmf_cancel_button value=取消 onClick="init(frmSetup);">
				</TD>				
		</table>
		<table>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			
			<TR><TD class=titleCell colSpan=3>MAC地址列表</TD></TR>
			
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			<tr class=spaceLine><TD colspan=3  class=spaceLine>
			
			<TR id=search_index class="textCell">
					<TD colSpan=3 align="left">按关键字过滤：
						<select name=search_item id=search_item ><!--onchange=changekey()-->
							<option value=1>MAC地址
							 <option value=2 >描述
						</select>
						关键字：
						<input type=text name="search_val" id="search_val" size=20 maxlength=63  onkeydown="if (event.keyCode==13) search_go(frmSetup,pattern)">
						<INPUT type=button name=search_start id=search_start class=button value=查询 onClick="search_go(frmSetup, pattern);">
						<INPUT type=button name=search_stop id=search_stop class=middlebutton value=显示全部  onClick="go_specific_page(5);"></TD>
						

	<TR><td width=100% colSpan=3 align=center>
		<iframe name="wlan_ap_mac_list" id="wlan_ap_mac_list" src="wlan_ap_acl_list.asp" width=100% height=0 align=left scrolling=no ></iframe><!--onload=displyTitle()-->	
		<tr><td width=30>&nbsp;<td width=100%>
		<table border="0" cellspacing="0" cellpadding="0" width="100%">
		<tr class="textCell">
		<td noWrap="noWrap" align="right">
			<span id="lblSecordHead">第</span>
			<span id="lblCurPage">1</span>
			<span >页/共</span>
			<span id="lblTotalPage">1</span>
			<span id="lblFirstHead">页 共</span>
			<span id="lblRecordCount">0</span>
			<span id="lblPageHead"> 条记录 每页</span>
			<input name="txtMaxRows" type="text" id="txtMaxRows" style="BORDER-TOP-WIDTH:1px; BORDER-LEFT-WIDTH:1px; IME-MODE:disabled; BORDER-BOTTOM-WIDTH:1px; HEIGHT:18px; TEXT-ALIGN:center; BORDER-RIGHT-WIDTH:1px" maxlength="3" size="4" value="8" validate="+INT" onkeydown="if (event.keyCode==13) gopage()"/>
			<span id="lblPerPage">行&nbsp;</span>
		</td>
		<td align="right" style="width:5px" valign="middle"><IMG name="imgFirst" id="imgFirst" title=首页 src="images/First-D.gif" onclick=go_specific_page(1)></td><td width=1>&nbsp;</td>
		<td align="right"  style="width:5px" valign="middle"><IMG name="imgPrev" id="imgPrev" title=上一页 src="images/Previous-D.gif" onclick=go_specific_page(2)></td><td width=1>&nbsp;</td>
		<td  align="right"  style="width:5px" valign="middle"><input name="txtCurPageIndex" type="text" id="txtCurPageIndex" style="BORDER-TOP-WIDTH:1px; BORDER-LEFT-WIDTH:1px; IME-MODE:disabled; BORDER-BOTTOM-WIDTH:1px; HEIGHT:18px; TEXT-ALIGN:center; BORDER-RIGHT-WIDTH:1px" maxlength="3" size="4" value="1" validate="+INT" 
		onkeydown="if (event.keyCode==13) gopage()" /></td><td width=1>&nbsp;</td>
		<td  align="right"  style="width:5px" valign="middle"><input type="button" name="btnGoPage" value="Go" id="btnGoPage" onclick="gopage()"></td><td width=1>&nbsp;</td>
		<td  align="right"  style="width:5px" valign="middle"> <IMG name="imgNext" id="imgNext" title=下一页 src="images/Next.gif" onclick=go_specific_page(3)></td><td width=1>&nbsp;</td>
		<td  align="right" style="width:5px" valign="middle"><IMG name="imgLast" id="imgLast" title=末页 src="images/Last.gif" onclick=go_specific_page(4)></td>
		</tr>
		</table>
		<TR class='textCell'><td colSpan=3>
	
		</TABLE>
		<table width=100% border=0>
		<tr class=textCell>
<!-- 			<TD align=left valign=top width=150>
			<INPUT type=button name=op_selctall id=op_selctall class=button value=全选 onClick=selectall()> -->
			<TD width=50%>
			<INPUT type=button name=op_del id=op_del class=button value=删除  onClick="submitF(frmSetup, ACTION_MAC_DEL);">	
			<INPUT type=button name=op_edit id=op_edit class=button value=编辑  onClick="editThem(frmSetup);">	
			</TD>
			<TD width=50% align="right">
				<INPUT type=button class=longlongbutton name=ipmacInput id=ipmacInput value=导入 onClick="submitF(frmSetup, ACTION_MAC_UPLOAD_MAC);">
				<INPUT type=button class=longlongbutton name=ipmacOutput id=ipmacOutput value=导出 onClick="submitF(frmSetup, ACTION_MAC_DOWNLOAD_MAC);">
			</TD>
            </tr>
		</table>
			
	</FORM>
</table>
</BODY>
<script>pageTail(str_copyright);</script>
</HTML>






