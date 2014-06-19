/* 
	Copyright 2013-2020, 
	Shenzhen Hexicom Technologies Co., Ltd. 
	All rights reserved.
	Author: Einsn Liu (liuchuansen@hexicomtech.com)
	Date: 2013-03-25 
*/

/* Global environment */
var env = new Array();
/* Menu List, get from env.js */
var menu;


/* Language define */
var lang_cn = new Array();

lang_cn['Sytem Setting'] = '系统管理';
lang_cn['Management'] = '管理参数';
lang_cn['Switch Setting'] = '设备管理';
lang_cn['Port Setting'] = '端口设置';
lang_cn['Network Topology'] = '业务管理';
lang_cn['User List'] = '用户管理';

lang_cn['System Status'] = '系统状态';
lang_cn['Date & Time'] = '日期及时间';
lang_cn['Configuration'] = '配置管理';
lang_cn['Reboot'] = '系统重启';
lang_cn['Upgrade'] = '软件升级';
lang_cn['System Log'] = '系统日志';

lang_cn['IP Setting'] = '管理IP设置';
lang_cn['Security'] = '管理安全设置';
//lang_cn['SNMP'] = 'SNMP设置';
lang_cn['SNMP_AGENT'] = 'SnmpAgent设置';
lang_cn['SNMP_GROUP'] = 'Snmp组设置';
lang_cn['SNMP_USER'] = 'Snmp用户设置';
lang_cn['SNMP_TRAP'] = 'Trap设置';
lang_cn['Administrator'] = '管理员';

lang_cn['Interface'] = '端口管理';
lang_cn['StormFilter'] = '广播风暴抑制';
lang_cn['VLAN Mode'] = 'VLAN模式设置';
lang_cn['802.1Q VLAN'] = '802.1Q VLAN';
lang_cn['Trunk VLAN'] = 'Trunk VLAN';
lang_cn['Hybrid VLAN'] = 'Hybrid VLAN';
lang_cn['QoS'] = 'QoS';

lang_cn['Master'] = '局端管理';
lang_cn['Topology'] = '网络拓扑';
lang_cn['Template'] = '模板管理';
	
lang_cn['UserList'] = '用户列表';
lang_cn['History'] = '上线记录';
lang_cn['Whitelist'] = '白名单';
lang_cn['Blacklist'] = '黑名单';

lang_cn['VLAN Pool'] = 'VLAN池管理';
lang_cn['Service'] = '服务管理';

lang_cn['Online'] = '在线';
lang_cn['Offline'] = '离线';
lang_cn['Save'] = '保存更新';
lang_cn['Exit'] = '退出';
lang_cn['Are you sure to store all configurations?'] = "确定要保存所有配置吗?";
lang_cn['Total:'] = '总数：';

function L(str)
{
  if ((env['sys.lang'] == 'cn')
    && (lang_cn[str])){
    return lang_cn[str];  
  }
  return str;   
}

/* end Language define */


/* add left menu item */
function addLeftMenu(name, link, iscurrent)
{
	var style_current = 'background:url(theme/' + env['sys.theme'] + '/images/main_25.gif)';
	var style_static =  'background:url(theme/' + env['sys.theme'] + '/images/main_36.gif)';
	
	var h = '';

	h += '	      <tr>';
	h += '					<td height="22" style="' + (iscurrent ? style_current : style_static) + '"> ';
	h += '						<table width="100%" border="0" cellspacing="0" cellpadding="0">';
	h += '	      			<tr>';
	h += '	         		 <td width="13%">&nbsp;</td>		';
	h += '	         			 <td width="72%" height="20"><div align="center">		';
	h += '	            		<table width="78%" height="21px" border="0" cellpadding="0" cellspacing="0">';
	h += '	              		<tr>';
	h += '	               		 <td><div align="center"></div></td>';
	h += '	               			 <td valign="middle"><div align="center"><a class="leftmenu" href="' + link + '"/>' + name + '</a></div></td>';
	h += '	              	</tr>';
	h += '	           		 </table>';
	h += '	          	</div></td>';
	h += '	          <td width="15%">&nbsp;</td>';
	h += '	      </tr>';
	h += '	      </table></td>';
	h += '	    </tr>		';
	return h;
}

/* add top menu item */
function addTopMenu(name, link, iscurrent)
{
	var style_current = "background:#4BAAA6; border-left:solid 1px #10637b;border-top:solid 1px #10637b;border-right:solid 1px #10637b;"
	var style_static = "background:url(theme/" + env['sys.theme'] + "/images/right_06.gif); border-left:solid 1px #82bcbd;border-top:solid 1px #82bcbd;border-right:solid 1px #82bcbd;";
	if(name== '') return '';

	var h = '';	
	h += '<td width="102px" height="26px" valign="bottom">';
	h += '	<table width="100%" border="0" align="center" cellpadding="0" cellspacing="0">';
	h += '		<tr>';
	h += '			<td height="25px" '; 
	h += ' 				style="' + (iscurrent ? style_current : style_static) + '">';
	h += '						<div align="center">&nbsp;';
	h += '								<a class="topmenu" href="' + link + '">' + name + '</a>';
	h += '						</div>';
	h += '				</td>';
	h += '		</tr>';
	h += '	</table>';
	h += '</td>	';
	return h;
}

/* a script for link 'save' blink and apply */
function saveHtml()
{
    var h = '';
    h += '<script language="javascript"> ';
    
    /* a function for config saving */	
    h += 'function save_apply()';
    h += '{ ';
    h += '	var f = document.forms[0];';
    h += '	if( !confirm("' + L("Are you sure to store all configurations?") + '"))';
    h += '		return;';
    h += '	diffCfg("config_0", "config", "save");	';
    h += '	subForm(f,"goform/command", "CONFIG", "index.asp");	';
    h += '}	';
    
    /* a function for font blinking */	
    h += 'var control = 0;';
    h += 'function fontBlink()';
    h += '{ ';
    h += '	var obj = document.getElementById("blink");';
    h += '	if (obj){';
    h += '		control ++;';
    h += '		obj.style.color = (control & 0x01) ? "black" : "red";';
    h += '	}';
    h += '} ';
    h += 'setInterval("fontBlink()", 300); ';
    h += '</script>';
    return h;
}

function tableLeftMenu(menuPage)
{	
    var h = '';
    h += '<table width="156" height="100%" border="0" cellpadding="0" cellspacing="0">';
    h += '		<tr>';
    h += '			<td align="center" valign="top">';
    h += ' 				<table width="100%" height="100%" border="0" cellpadding="0" cellspacing="0">';
    h += '	     		<tr>';
    h += '		        <td height="33" background="theme/' + env['sys.theme'] + '/images/main_21.gif">&nbsp;</td>';
    h += '		   		</tr>';
    
    for (var i = 0; i < menu.length; i ++){
        var iscurrent = pageInMenu(menu[i].submenu, menuPage);
        h += addLeftMenu(L(menu[i].name), menu[i].page, iscurrent);
    }

    h += '	<tr>	';
    h += '		<td align="center" valign="top">';
    h += '			<table width="145px" border="0" align="center" cellpadding="0" cellspacing="0">';
    h += ' 	  		<tr>';
    h += '  				<td>&nbsp;</td>';
    h += '				</tr>';
    h += '  		</table>';
    h += '		</td>';
    h += '  </tr> ';
    h += ' </table></td>';
    h += ' </tr>';
    h += '</table> ';
	return h;	
}

function tableTopMenu(menuPage)
{
    var h = '';
    h += '			<table width="100%" border="0" cellspacing="0" cellpadding="0" style="table-layout:fixed;">';
    h += '			  <tr>';
    h += '			    <td height="28" valign="bottom" background="theme/' + env['sys.theme'] + '/images/right_03.gif">';
    h += '			    	<table width="100%" border="0" cellspacing="0" cellpadding="0">';
    h += '			      	<tr>';
    h += '			        	<td width="1%" height="26" style="width:5px;">&nbsp;</td>';
    h += '			        	<td width="99%" valign="bottom">';
    h += '			        		<table width="100%" border="0" cellspacing="0" cellpadding="0">';
    h += '        						<tr>	';
	
    var topMenu = getSubMenu(menu, menuPage);
    for (var i = 0; i < topMenu.length; i ++){
		
		if(env['sys.parent_name']==topMenu[i].name)
		{
			var current_page = true;
		}
		else
		{
        	var current_page = (menuPage && (menuPage == topMenu[i].page)) ? true : false;
		}
        h += addTopMenu(L(topMenu[i].name), topMenu[i].page, current_page);
    }

    h += '	            				<td>&nbsp; </td>'; // a dummy row
    h += '	          				</tr>';
    h += '	        				</table>';
    h += '	        			</td>';
    h += '								</tr>';
    h += '					    </table>';
    h += '					   </td>';
    h += '					  </tr>';
    h += '					  <tr>';
    h += '					    <td background="theme/' + env['sys.theme'] + '/images/right_09.gif" style="line-height:5px;">&nbsp;</td>';
    h += '					  </tr>	';
    h += '					</table> ';
    return h;
}



function rowContent(menuPage)
{
    var h = '';
    h += '  <tr> ';
    h += ' <td width="156" noWrap align="center" valign="top" style="background:#ECFFFF; border-right:solid 1px #00655F; border-left:solid 0px #00655F">';
    h += tableLeftMenu(menuPage);
    h += ' </td> ';
    h += ' <td align="center" valign="top" style="background:#D2E9FF; border-right:solid 0px #00655F">';
    h += tableTopMenu(menuPage);
    h += '<div style="width:95%; text-align:center; padding:10px;">';
    /* ending </td> and </tr> in tailer */
    return h;
}

function rowHead()
{
    var logo_tag = '&nbsp;';
    if (env['vendor.logo'] != 'none'){
        logo_tag = '<img src="/theme/' + env['sys.theme'] + '/images/logo.png"/>';
    }
    
    var h = '';
    h += '	<tr> ';
    h += '		<td colspan="2" style="height:80px; border-bottom:solid 2px #00655F" >';
    h += '			<div style="height:100%" class="RoundedCorner">';
    h += '				<b class="rtop"><b class="r1"></b> <b class="r2"></b> <b class="r3"></b><b class="r4"></b> </b>';
    h += '				<table width=100% height=100% border="0" cellpadding="0" cellspacing="0">';
    h += '	 			<tr>';
    h += '	 				<td rowspan=3 width=30% style="text-align:left; padding:4px;"> ' + logo_tag + '</td>	';
    h += '	 				<td> &nbsp; </td>	';
    h += '	 		  </tr>';
    h += '	 			<tr> ';
    h += '	 				<td style="padding-right: 10px; text-align:right; color:black; font-size:20px; font-weight:bold" > ';
    h += env['sys.model'] + '&nbsp;' + env['sys.name'];
    h += '  				</td>';
    h += '	 			</tr>';
    h += '	 			<tr> ';
    h += '	 				<td style="padding: 0 10px 5px 0;font-color:red; text-align:right;">';
    h += '	 					<a ' + ((env['sys.unsaved'] == '1') ? 'id="blink"' : "") + ' href="javascript:save_apply()"> ' + L("Save") + ' </a> &nbsp;&nbsp;&nbsp;&nbsp;';
    h += '	 					<a href="login.asp">' + L("Exit") + '</a>';
    h += '	 				</td>';
    h += '	 			</tr>';
    h += '	 			</table>';
    h += '			</div>';
    h += '		</td>';
    h += '	</tr>';
    return h;
}

function pageHead(env_in, menuPage/*'system_status.asp'*/)
{
    env = env_in;
    menu = getMenu(env);
    var m = '';

    m += saveHtml();

    m += '<table width="860px" height="550px" align="center" border="0" cellpadding="0" cellspacing="0">';
    
    m += rowHead();
    
    m += rowContent(menuPage);
    
    document.writeln(m);	
    /* ending </table> is in pageTail() */
}


function pageTail()
{
    var h = '';
    h +='</div>';
    h +=' </td>';
    h +=' </tr>';
    h +=' <tr>';
    h +=' <td colspan="2" style="height:15px; border-top:solid 2px #00655F">';
    h +='  <div style="height:100%" class="RoundedCorner">			';
    h +='   <table border=0px width=100% height=100% cellspacing="0" cellpadding="0">';
    h +='    <tr>';
    h +='      <td style="padding-right:5px; font-size:12px; text-align:right">COPYRIGHT © ' + env['vendor.copyright'] + ' <a href="' + env['vendor.link'] + '"> ' + env['vendor.name'] + ' </a> ALL RIGHTS RESERVED.</td>';
    h +='    </tr> ';
    h +='   </table> ';
    h +='   <b class="rbottom"><b class="r4"></b><b class="r3"></b><b class="r2"></b><b class="r1"></b></b> ';
    h +='  </div> ';
    h +=' </td> ';
    h +='</tr> ';
    h +='</table> ';
    
    document.writeln(h);
}

