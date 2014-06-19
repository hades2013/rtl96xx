<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<META HTTP-EQUIV=Pragma CONTENT=no-cache>
<TITLE>MAC显示</TITLE>
<LINK href=style.css type=text/css rel=stylesheet>
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
<SCRIPT language=javascript>

var sear_mac = "0000-0000-0000";


var if_search = 0;
var sear_vlan = 0;
var mac_mac_setting = new Array(<% CGI_GET_MAC_SETTING(); %>);
//var mac_mac_setting = new Array('1;45');
var aging_en = GetVal(mac_mac_setting, 0, 0);
var aging_val = GetVal(mac_mac_setting, 0, 1);

var mac_addr_arr=new Array(<% CGI_GET_MAC(); %>);
//var mac_addr_arr=new Array("0060-F321-3B41",0,1,9,0,"D4BE-D9D2-7DA8",0,1,9,0,"6C50-4D4C-D4E4",0,1,2,0,"BCAE-C589-FF37",0,1,21,0,"D4BE-D9C9-8DFE",0,1,2,0,"0000-C500-8300",0,1,2,0,"F83D-FFB9-8FFB",0,1,2,0,"0016-4130-1402",0,1,12,0,"3C97-0E70-D387",0,1,17,0,"0060-F321-3984",0,1,9,0,"0007-E3DC-7061",0,1,2,0,"DC0E-A1EA-5E8A",0,1,1,0,"00E0-4C68-0281",0,1,9,0,"0015-17E9-5AE6",0,1,2,0,"BCAE-C58A-00B7",0,1,21,0,"0");
var listnum = 5;
var user_level=1;
var vlan_type=1;
var igsp_status=new Array(0,105,10,2,260);
//var firstidx = 1;
//var numMacs = 33;
//var mac_dis_para = new Array(33,1);
var mac_dis_para=new Array(<% CGI_GET_MAC_PARA(); %>);
var set_err = 0;


numMacs = parseInt(mac_dis_para[0]);
firstidx =parseInt(mac_dis_para[1]);
//top.contents.urlcookie.get_cur_url("mac_dis.cgi");

var mac_num=parseInt((mac_addr_arr.length)/listnum);

var lastidx = 0;
lastidx=firstidx + 14;
if(lastidx > numMacs)
{
    lastidx= numMacs;
}
var perpage;
var allPageNum;
if (numMacs > 0)
{
    if (1 == (firstidx%15))
    {
        perpage = parseInt(firstidx/15) + 1;
        if (numMacs > 15)
        {
            if (0 == (numMacs%15))
                allPageNum = parseInt(numMacs/15);
            else
                allPageNum = parseInt(numMacs/15) + 1;
        }
        else
            allPageNum = 1;
    }
    else
    {
        if (0 == firstidx%15)
            perpage = parseInt(firstidx/15) + 1;
        else
            perpage = parseInt(firstidx/15) + 2;
        if (numMacs == lastidx)
            allPageNum = parseInt((numMacs - lastidx)/15) + perpage;
        else
            allPageNum = parseInt((numMacs - lastidx)/15) + 1 + perpage;
    }
}
else
{
    perpage = 0;
    allPageNum = 0;
	//alert(allPageNum);
}

function GURL(x)
{
    location=x;
}



function aging_time_en_func()
{
	var f = document.forms['form0'];
	f.aging_time_val.disabled = !f.aging_time_en.checked;
}
function init()
{
    var origin_value = "";
    //document.getElementById("smac").focus();

	if(1 == set_err)
	{ 
	    origin_value = get_cookie("smac_VALUE");
	    if(null != origin_value)
	    { 
	        document.mac_query.smac.value = origin_value; 
	    }

	    origin_value = get_cookie("svlan_VALUE");
	    if(null != origin_value)
	    { 
	        document.mac_query.svlan.value = origin_value; 
	    }
	}
	origin_value = get_cookie("searMac_VALUE");
    if(null != origin_value)
    { 
         sear_mac = origin_value; 
    }
    
	delete_temporary_cookie("smac_VALUE");
	delete_temporary_cookie("svlan_VALUE");
	delete_temporary_cookie("searMac_VALUE");

	delete_temporary_cookie("mMac_VALUE");
	delete_temporary_cookie("mType_VALUE");
	delete_temporary_cookie("mVlan_VALUE");
	delete_temporary_cookie("mPort_VALUE");
	delete_temporary_cookie("disp_VALUE");

    if (1 == if_search)
    {
        set_cookie("searMac_VALUE", escape(sear_mac), null, "/");
    }
	if (0 == user_level)
	{
	    document.mac_query.smac.disabled = true;
	    document.mac_query.svlan.disabled = true;
	    document.mac_query.search.disabled = true;
	}

	document.forms['form0'].aging_time_en.checked = (aging_en == '1'?true:false);
	document.forms['form0'].aging_time_val.disabled = (aging_en == '1'?false:true);
	document.forms['form0'].aging_time_val.value = (aging_en == '1'?aging_val:'300');

}

function aging_time_check()
{
    var valid;
	var j = 0;
	
    var input = document.forms['form0'].aging_time_val.value; 
    var t = /^\s*$/;
   	var re = /^[1-9][0-9]*$/;    

    if (1 == t.test (input))
    {
        alert ("老化时间不能为空 ！");
        return false;
    }

    if (!re.test(input)){
    	alert("非法的老化时间,请输入有效数字");
    	return false;
    }
	   
    var iInput=parseInt(input, 10);
    if(iInput<45 || iInput>458)
    {
        alert ("非法的老化时间,请输入值(45-458)！");
        return false;
    }
	return true;
}

function aging_time_apply()
{
	var f = document.forms['form0'];
	var en = f.aging_time_en.checked==true?1:0;
	var val = f.aging_time_val.value;
	
	
	if(en==1 && !aging_time_check())
	{
		return false;
	}
	diffCfg("aging_time_en", "agint_time_en", en);
	diffCfg("aging_time_val", "agint_time", val);

	subForm(f, 'goform/command', 'MAC_SETTING', 'mac_dispaly.asp');
}

/*type : 1 数值,   2 ip地址,  3 mac地址*/ 
function validateInput(event,type) 
{
    if(type == 1)
    {
        if( event.keyCode < 48 || event.keyCode > 57 )
        {
            event.returnValue = false; 
        }
    }
    else if(type == 2)
    {
        if(( event.keyCode < 48 || event.keyCode > 57 ) && ( event.keyCode != 46)) 
        { 
            event.returnValue = false;
        }
    }
    else if(type == 3)
    { 
        //允许输0~9,a~f,A~F,'-'
        if( ( event.keyCode < 48 || event.keyCode > 57 )
            &&  ( (event.keyCode < 65) || (event.keyCode > 70))
            &&  ( (event.keyCode < 97) || (event.keyCode > 102))  
            && ( event.keyCode != 45 )) 
        {
            event.returnValue = false; 
        } 
    }
    else if(type == 4)
    {
        if( event.keyCode < 48 || event.keyCode > 57 )
        {
		    event.returnValue = false;
        }
        if (event.keyCode == 48)
        {
            if (document.mac_dis.txtCurPageIndex.value == 0)
            {
                event.returnValue = false;
            }
        }
    }
}

var styleName;
if(1==user_level)
{
    styleName = "textCellLinkAll";
}
else
{
    styleName = "textCellNoLink";
}
function selectModifyPort(index)
{
    var port  = 0;
    if (4 == mac_addr_arr[index*listnum+1])
        port = 1;
    else
        port = mac_addr_arr[index*listnum+3];
    
    if(1==user_level)
    {
        /*标记是从那个页面进入修改页面的:全局显示和端口显示，这里是全局显示*/
        set_cookie("disp_VALUE", 50, null, "/");
        set_cookie("mMac_VALUE", escape(mac_addr_arr[index*listnum+0]), null, "/"); 
        set_cookie("mPort_VALUE", port, null, "/");
        set_cookie("mVlan_VALUE", escape(mac_addr_arr[index*listnum+2]), null, "/");
        set_cookie("mType_VALUE", escape(mac_addr_arr[index*listnum+1]), null, "/");
        //GURL('mac_modify.htm');
    	GURL('mac_modify.cgi?act=modify&modtype='+mac_addr_arr[index*listnum+1]+'&modmac='+mac_addr_arr[index*listnum+0]+'&modport='+port+'&modvlan='+mac_addr_arr[index*listnum+2]+'&disType='+1);
    }
}

function mouseOn_modifyPort(index)
{
    if(1==user_level)
    {
	    document.getElementById('mac'+index).style.textDecoration= 'underline';
	    document.getElementById('mac'+index).style.color='#3030F0';
	    document.getElementById('vlan'+index).style.textDecoration= 'underline';
	    document.getElementById('vlan'+index).style.color='#3030F0';
	    document.getElementById('port'+index).style.textDecoration= 'underline';
	    document.getElementById('port'+index).style.color='#3030F0';
	    document.getElementById('type'+index).style.textDecoration= 'underline';
	    document.getElementById('type'+index).style.color='#3030F0';
	    document.getElementById('bind'+index).style.textDecoration= 'underline';
	    document.getElementById('bind'+index).style.color='#3030F0';
    }
}

function mouseOut_modifyPort(index)
{
    if(1==user_level)
    {
	    document.getElementById('mac'+index).style.textDecoration= 'none';
	    document.getElementById('mac'+index).style.color='#000000';
	    document.getElementById('vlan'+index).style.textDecoration= 'none';
	    document.getElementById('vlan'+index).style.color='#000000';
	    document.getElementById('port'+index).style.textDecoration= 'none';
	    document.getElementById('port'+index).style.color='#000000';
	    document.getElementById('type'+index).style.textDecoration= 'none';
	    document.getElementById('type'+index).style.color='#000000';
	    document.getElementById('bind'+index).style.textDecoration= 'none';
	    document.getElementById('bind'+index).style.color='#000000';
    }
}

function checkPageNum()
{
    var pageIndex = document.mac_dis.txtCurPageIndex.value;
    //if (pageIndex > allPageNum)
    {
        //alert("该页号不存在，总共"+allPageNum+"页，请重新输入!");
        //document.mac_dis.txtCurPageIndex.value = "";
        //return false;
    }
    return true;
}


function search_mac()
{
//del by feihuaxin
}

function ApplyPage()
{
   var v=document.forms[0];
   setCookie(cookie_dontShowDoCMDPage, '1');
   diffCfg('req_mac_idx', 'req_mac_idx', document.mac_dis.idx.value);
   subForm(v,'goform/command','MAC_DIS','mac_dispaly.asp');	
   
}
function selectPage(index)
{
    if(index > perpage)
    {
        document.mac_dis.idx.value = firstidx + 15*(index - perpage);
    }
    else if (index == 1)
    {
        document.mac_dis.idx.value=1;
    }
    else
    {
        document.mac_dis.idx.value = firstidx - 15*(perpage - index);
    }
    // document.mac_dis.act.value="page";
    //document.mac_dis.ifsearch.value = if_search;
    //document.mac_dis.searVlan.value = sear_vlan;
	//document.mac_dis.submit();
	ApplyPage();
}

function goto()
{
    var flag  = 0;
    var pageIndex = document.mac_dis.txtCurPageIndex.value;
    if (pageIndex == "")
    {
        flag = 1;
    }
    else if(!/(^[0-9]{1,3}$)/.test(pageIndex))
    {
        flag = 1;
    }
    else if (0 == pageIndex)
    {
        flag = 1;
    }

    if (1 == flag)
    {
        document.mac_dis.txtCurPageIndex.focus();
        document.mac_dis.txtCurPageIndex.select();
        return false;
    }
    else
    {
        if (pageIndex > allPageNum)
        {
            pageIndex = allPageNum;
        }
        selectPage(pageIndex);
    }
}
function selectAll(obj)
{
    if(obj.checked == true)
    {
        document.getElementById("r").checked = true;
        for (var i=0 ; i < mac_num; i++) 
        {
            if(document.getElementById(i).disabled == false)
            {
                document.getElementById(i).checked = true;
            }
        }
    }
    else
    {
        document.getElementById("r").checked = false;
        for (var i=0 ; i < mac_num; i++) 
        {
            document.getElementById(i).checked = false;            
        }
    }
}


function bindAll()
{
	//del by feihuaxin
}    
function deleteAll(type)
{
	//del by feihuaxin
}


function ip_multicast(mac_addr,igsp)
{
    var sub_mac;
    var sub_value;
    
    sub_mac = mac_addr.substring(0, 7);
    if(0 == igsp)
    {
        return false;
    }
        
    if("0100-5E" != sub_mac)
    {
        return false;
    }
    sub_value = mac_addr.charAt(7);
    if((sub_value != '7') 
    && (sub_value != '6') 
    && (sub_value != '5') 
    && (sub_value != '4') 
    && (sub_value != '3') 
    && (sub_value != '2') 
    && (sub_value != '1') 
    && (sub_value != '0'))
    {
        return false;
    }
    
    return true;
}

</SCRIPT>
</HEAD>
<BODY bgcolor=#ffffff leftMargin=0 topMargin=0 MARGINWIDTH=0 MARGINHEIGHT=0 BORDER=0 onload="init();" ><!--onclick="top.hideMenu(1);"-->
<script>pageHead(env, env['web.page']);</script>
<table border=0>
<tr>
  <td colSpan=3>&nbsp;</td>
</tr>
<tr>
  <td width=24 valign=top>&nbsp;</td>
  <td width=720 valign=top>
  
  <form method="post" name="form0" action="goform/command"> 
  <TABLE width=100%>
  <TR> <TD colspan=6 class=tb_head>&nbsp;MAC老化时间设置</TD> </TR>
  <tr>
  <td >老化时间状态:<input name='aging_time_en' value=search type=checkbox onclick="aging_time_en_func();">&nbsp;启用</td>
  <td>老化时间:<input name='aging_time_val' type=text size=7 maxlength=3><font size=2>(45-458s, 默认为300s)</font></td> 
  <TD><div align="right">
      <INPUT name="search" type="button" style="WIDTH: 80px" class="button" value="确定" onclick='aging_time_apply();'></div></TD> 
</tr>
 </TABLE>
 </form>  
	
	
  <TABLE width=100%>
  <TR> <TD colspan=6 class=tb_head>&nbsp;MAC地址查询</TD> </TR>
  <!--
  <form method="post" name="
  " action="mac_dis.cgi"> 
  <input name=act value=search type=hidden>
  <input name=ifsearch type=hidden >
  <input name=searVlan type=hidden >
    <TR> <TD colspan=6 class=titleCell>&nbsp;MAC地址查询</TD> </TR>
    <TR> <TD height=8></TD> </TR>
    <TR class=textCell> 
      <TD width=18%><div align="center">MAC (HH-HH-HH)</div></TD>
      <TD width=22%><div align="left"> 
      <INPUT type="text" maxLength="15" name="smac" id="smac" size="15" onkeypress="validateInput(event,3);onEnterSub_Firefox(event,search_mac,user_level);" onpaste="return true;" STYLE="ime-mode:disabled"> 
      </TD>
      <TD width=15%><div align="center">VLAN (1~4094)</div></TD>
      <TD width=10%><div align="left"> 
      <INPUT type="text" maxLength="4" name="svlan"  id="svlan" size="5" onkeypress="onEnterSub_Firefox(event,search_mac,user_level);" onpaste="return true;" STYLE="ime-mode:disabled"> 
      </TD>
	-->
      <TD width=35%><div align="right">
      <INPUT name="search" type="button" style="WIDTH: 80px" class="button" value=刷新 onclick=GURL('mac_dispaly.asp')></div></TD> 
	<!--
    </TR>
    <TR class=textCellRed> 
      <TD width=18%></TD>
      <TD width=22% id=mac></TD>
      <TD width=25% colspan=2 id=vlan></TD>
      <TD width=35% id=sch align="center"></TD>
    </TR>
  </form>  
  -->
  </TABLE>
  <FORM method=post name=mac_dis action=goform/command>
  <input name=idx type=hidden>
  <!--
  <input name=act type=hidden>
  <input name=ifsearch type=hidden>
  <input name=searVlan type=hidden>
  -->
  <TABLE cellSpacing=0 cellPadding=0 border=0 width=100%>
  <TR>
    <TD height=25>
    <TABLE cellSpacing=1 cellPadding=3 border=0 width=100%>
      <TR class=textCellRed>
	  <!--
      <SCRIPT>
			if(1==user_level)
			{
                document.write('<TD colspan=7 align = left>');
    			document.write('<b>&nbsp;注意:</b>&nbsp;&nbsp;只有开启端口MAC过滤功能，绑定条目才会生效，请到"<A href=javascript:linkHref("mac_bind.htm")>端口MAC过滤</A>"页面进行配置。');
			}
			else
			{
                document.write('<TD colspan=6 align = left>');
    			document.write('<b>注意:</b>&nbsp;&nbsp;只有开启端口MAC过滤功能，绑定条目才会生效。');
			}
      </SCRIPT>
	  -->
        </td>
      </TR>
    <TR class=tb_head>
	  <!--
      <TD width=5%><div align=center><input type=checkbox name=r id=r  onclick="selectAll(this);" ></div></TD>
	  -->
      <TD width=30%><div align=center>MAC地址</div></TD>
	  <!--
      <TD width=13%><div align=center>类型</div></TD>
	  -->
      <TD width=13%><div align=center>VLAN</div></TD>
      <TD width=15%><div align=center>端口</div></TD>
	  <!--
      <TD width=12%><div align=center>状态</div></TD>
	  -->
	  <!--
      <SCRIPT>
      if(1==user_level)
          document.write('<TD width=12%><div align=center>删除</div></TD>');
      </SCRIPT>
	  -->
    </TR>
    <SCRIPT>
    if (numMacs > 0)
	
    {   
    var i=0;
    for(;i<mac_num;i++)
    {
        if (0 == i%2)
            document.write('<TR class=textCell>');
        else
            document.write('<TR class=tb_contentgrey>');
        //document.write('<TD><div align=center><input type=checkbox name='+i+' id='+i+'  ondblclick="selectAll(this);" ><\/div><\/TD>');
        /*动态和组播条目不允许修改。 0:动态 1:静态  2:组播(静态)  3:组播(动态)  4:黑洞*/
        if ((0 == mac_addr_arr[listnum*i+1]) || (3==mac_addr_arr[listnum*i+1]) || (2==mac_addr_arr[listnum*i+1]) ||
            (true == ip_multicast(mac_addr_arr[listnum*i+0],igsp_status[0])))
        {
			
            document.write('<TD><div align=center>'+ mac_addr_arr[listnum*i+0] +'<\/div><\/TD>');
			
           // if (2==mac_addr_arr[5*i+1])
           // {
           //     document.write('<TD><div align=center>'+ '静态' + '<\/div><\/TD>');
           // }
           // else
           // {
           //     document.write('<TD><div align=center>'+ '动态' + '<\/div><\/TD>');
          //  }
            document.write('<TD><div align=center>');
    		if(1==vlan_type)
    		{
                document.write(mac_addr_arr[listnum*i+2]);
    		}
    		else
    		{
    		    document.write('--');
    		}
            document.write('<\/div><\/TD><TD><div align=center>');
            if((2==mac_addr_arr[listnum*i+1]) || (3==mac_addr_arr[listnum*i+1]))
            {
                var portArray = new Array();
                portArray = mac_addr_arr[listnum*i+3].split(",");
                if (portArray.length < 10)
                {
                    document.write(mac_addr_arr[listnum*i+3]);
                }
                else
                {
                    for (var j=0; j < (portArray.length - 1); j++)
                    {
                        document.write(portArray[j]+",");
                        if (9 == j%10)
                        {
                            document.write("<br>");
                        }
                    }
                    document.write(portArray[portArray.length - 1]);
                }
            }
            else
            {
                document.write(mac_addr_arr[listnum*i+3]);
            }
            
            /*2、3表示组播MAC，4表示黑洞MAC，这些都不允许绑定*/
            /*if ((2==mac_addr_arr[5*i+1]) || (3==mac_addr_arr[5*i+1]))
            {
        		document.write('<TD><div align=center>'+ '不支持' + '<\/div><\/TD>');
    		}    
    		else if(0==mac_addr_arr[5*i+1])
    		{
        		document.write('<TD><div align=center>'+ '未绑定' + '<\/div><\/TD>');
    		}
    		else
    		{
    		    document.write('<TD><div align=center>'+ '已绑定' + '<\/div><\/TD>');
    		}
			*/
        }
        else
        {
            if (0 == i%2)
            {
                document.write('<TD align=center id="mac'+(i)+'" class="'+styleName+'1" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + mac_addr_arr[listnum*i+0] + '<\/div><\/TD>');
                /*组播和动态条目在上面显示，这里不会有动态条目显示*/
                if(1==mac_addr_arr[listnum*i+1])
                {
                    document.write('<TD align=center id="type'+(i)+'" class="'+styleName+'1" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + '静态' + '<\/div><\/TD>');
                }
        		else if(4==mac_addr_arr[listnum*i+1])
        		{
        		    document.write('<TD align=center id="type'+(i)+'" class="'+styleName+'1" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + '黑洞' + '<\/div><\/TD>');
        		}
        		
        		if(1==vlan_type)
        		{
        		    document.write('<TD align=center id="vlan'+(i)+'" class="'+styleName+'1" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + mac_addr_arr[listnum*i+2] + '<\/div><\/TD>');
        		}
        		else
        		{
        		    document.write('<TD align=center id="vlan'+(i)+'" class="'+styleName+'1" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + '--' + '<\/div><\/TD>');
        		}                
            }
            else
            {
                document.write('<TD align=center id="mac'+(i)+'" class="'+styleName+'2" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + mac_addr_arr[listnum*i+0] + '<\/div><\/TD>');
                if(1==mac_addr_arr[listnum*i+1])
                {
                    document.write('<TD align=center id="type'+(i)+'" class="'+styleName+'2" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + '静态' + '<\/div><\/TD>');
                }
        		else if(4==mac_addr_arr[listnum*i+1])
        		{
        		    document.write('<TD align=center id="type'+(i)+'" class="'+styleName+'2" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + '黑洞' + '<\/div><\/TD>');
        		}
        		if(1==vlan_type)
        		{
        		    document.write('<TD align=center id="vlan'+(i)+'" class="'+styleName+'2" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + mac_addr_arr[listnum*i+2] + '<\/div><\/TD>');
        		}
        		else
        		{
        		    document.write('<TD align=center id="vlan'+(i)+'" class="'+styleName+'2" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + '--' + '<\/div><\/TD>');
        		}
            }

            var str = "";
		    if(4==mac_addr_arr[listnum*i+1])
            {
                str += "--";
            }
            else
            {
                str += mac_addr_arr[listnum*i+3];
            }
            
            if (0 == i%2)
    		    document.write('<TD align=center id="port'+(i)+'" class="'+styleName+'1" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + str + '<\/div><\/TD>');
            else
                document.write('<TD align=center id="port'+(i)+'" class="'+styleName+'2" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + str + '<\/div><\/TD>');

            /*2、3表示组播MAC，4表示黑洞MAC，这些都不允许绑定*/
            if (4==mac_addr_arr[listnum*i+1])
            {
                if (0 == i%2)
        		    document.write('<TD align=center id="bind'+(i)+'" class="'+styleName+'1" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + '不支持' + '<\/div><\/TD>');
                else
                    document.write('<TD align=center id="bind'+(i)+'" class="'+styleName+'2" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + '不支持' + '<\/div><\/TD>');
    		}    
    		else
    		{
    		    if (0 == i%2)
        		    document.write('<TD align=center id="bind'+(i)+'" class="'+styleName+'1" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + '已绑定' + '<\/div><\/TD>');
                else
                    document.write('<TD align=center id="bind'+(i)+'" class="'+styleName+'2" onClick="selectModifyPort('+(i)+');" onmouseover="mouseOn_modifyPort('+(i)+');" onmouseout="mouseOut_modifyPort('+(i)+');"><div align="center">' + '已绑定' + '<\/div><\/TD>');
    		}
        }

		/*
		if(1==user_level)
        {
		    document.write('<TD><div align=center>');
            if ((false == ip_multicast(mac_addr_arr[5*i+0],igsp_status[0])) || ((2 != mac_addr_arr[5*i+1]) && (3 != mac_addr_arr[5*i+1])))
            {
                var idx1 = firstidx;
                if ((idx1 > 1) && (1 == mac_num))
                {
                    idx1 = 1;
                }
                document.write('<input type=button style="WIDTH: 60px" class="button" name="d'+i+'" id="d'+i+'" value="删除" ');
                document.write('onclick=GURL("mac_dis.cgi?act=delete&addr='+mac_addr_arr[5*i+0]+'&vid='+mac_addr_arr[5*i+2]+'&idx='+idx1+'&ifsearch='+if_search+"&searVlan="+sear_vlan+'")><\/div><\/TD>');
            }
            else
            {
                document.write('<input type=button style="WIDTH: 60px" class="button" name="d'+i+'" id="d'+i+'" value="删除"><\/div><\/TD>');
                document.getElementById("d"+i).disabled = true;
            }
		}*/
		document.write('<\/TR>');
    }
    }
    </SCRIPT>
    </TABLE>

    <TABLE cellSpacing=1 cellPadding=3 border=0 width=100%>
    <SCRIPT language=JavaScript>
    document.write('<TR class="textCell">');
    if(numMacs > 0)  
    {
        document.write('<TD class="textCell" width=33%><div align=left>共 '+numMacs+' 个, '+allPageNum+' 页, 显示 '+firstidx+' - '+lastidx+' <\/div><\/TD>');
    
    document.write('<TD class="textCell" width=50%><div align=right>');
    
    var i = 1;
    var lastPage = allPageNum;
    if (perpage > 1)
    {
        document.write('<a HREF="javascript:selectPage('+1+')" title="第一页">&laquo;<\/a>');
        document.write('<a HREF="javascript:selectPage('+(perpage - 1)+')"> 上一页 <\/a>');
    }
    if (perpage > 4)
    {
        document.write('<a HREF="javascript:selectPage('+(perpage - 4)+')"> ... <\/a>');
        i = perpage - 3 ;
    }
    if ((allPageNum - perpage) > 3)
    {
        lastPage = perpage + 3;
    }
    if (allPageNum > 1)
    {
        for (; i <= lastPage; i++)
        {
            if (i == perpage)
            {
                document.write(' '+i+' ');
            }
            else
            {
                document.write('<a HREF="javascript:selectPage('+i+')">'+' '+i+' '+'<\/a>');
            }
        }
    }
    if ((allPageNum - perpage) > 4)
    {
        document.write('<a HREF="javascript:selectPage('+(perpage+4)+')"> ... <\/a>');
    }
    if (perpage < allPageNum)
    {
        document.write('<a HREF="javascript:selectPage('+(perpage+1)+')"> 下一页 <\/a>');
        document.write('<a HREF="javascript:selectPage('+allPageNum+')" title="最后一页"> &raquo;<\/a>');
    }
    document.write('<\/div><\/TD>');
    document.write('<TD class="textCell" width=17%><div align=center style="vertical-align:top" >');
    if (allPageNum > 1)
    {   
        document.write('转到 ');
        document.write('<input name="txtCurPageIndex" type="text" id="txtCurPageIndex" maxlength="3" size="3" value="1" validate="+INT" onkeypress="validateInput(event,4);onEnterSub_Firefox(event,goto,1);" onkeyup="checkPageNum();">');
        document.write(' ');
        document.write('<input type=button name="btnGoPage" value="Go" id="btnGoPage" style="width:25PX" onclick="goto();" >');
        document.write('<input type="text" name="btnGoPage1" value="" size="1" style="visibility: hidden; width:2PX">');
        document.write('<\/div><\/TD>');
    }
    }
    </script>
  </TABLE>
    
    </TD>
  </TR>
  </TABLE>
  </td>
  </FORM>
  <!--
  <td width=100 align=center valign=top>
  <p>&nbsp;</p>
  <p><INPUT class=actButton type=button value=帮助 onclick=popHelp("help.htm#mac_dis")>
  <script>
  if ( 1 == user_level)
  {
      document.write('<p><INPUT class=actButton type=button value=添加 onclick=GURL("mac_add.htm")></p>');
      document.write('<p><INPUT class=actButton type=button value=绑定 onclick="bindAll();"></p>');
      document.write('<p><INPUT class=actButton type=button value=全部删除 onclick="deleteAll(2);"></p>');
      document.write('<p><INPUT class=actButton type=button value=批量删除 onclick="deleteAll(1);"></p>');
  } 
  </script>
  <p><INPUT class=actButton type=button value=刷新 onclick=GURL('mac_dis.cgi')></p>
  </TD>    
  <td width=100 align=center valign=bottom>
  <p><INPUT class=actButton type=button value=刷新 onclick=GURL('system_mac.asp')></p>
  </TD> 
    -->
</tr>
</table>
<script>pageTail();</script>
</BODY>
</HTML>