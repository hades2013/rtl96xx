
/*This is an vendor specified javascript file,
It is created by vendor.sh automatically*/

var str_copyright = 'COPYRIGHT ©';
var PROD_MODEL = '';
var logo_name = 'images/logo_';



function showlogo()
{
	var str;
	str="<table  cellSpacing=1 cellPadding=1 border=0>";
	//str+="<tr><td valign='top'><img src='images/logo.png'></td></tr>";
	str+="<tr><td valign='top'><img src=";
	str+=logo_name;
	str+="></td></tr>";
	str+="</table>";
	return str;
}

var cPage=window.location.toString().replace(/.*\//,'');
var DIV;
cPage=cPage.replace(/\?.*/,'');

function Group(s)
{
	this.s=s;		/* Group Name (menu bar) */
	this.pid=0;
}

var Groups=new Array(
new Group('')
,new Group('System Setting')
,new Group('Network Setting')
,new Group('Membership')
,new Group('Users')
);



var wizGroups=new Array(
new Group('')
,new Group('&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1. Time Zone')
,new Group('&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2. WAN Type')
,new Group('&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;3. WAN Settings')
,new Group('&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;4. DNS')
);

var mNum=0;
var focusPage;
function Page(g,s,p,t,d,h,f,ti)
{
    this.g=g;			/* Group */
	this.s=s;			/* Text */
	this.p=p;			/* html file to link */
	this.t=t;			/* Banner */
	this.d=d;
	this.h=h;
	this.ti=ti;
	this.idx=mNum++;
	if (f)
	{
		this.f=f;
		if (f==1)
		{
			Groups[g].pid=this.idx;
		}
	}
	else
		this.f=0;	/* flag */
}

var index_desc ='';
var advanced_desc = ''
var	system_desc = 	'';
var system_man_ip_desc = '';
var	system_man_vlan_desc =''
var time_desc = '';
var snmp_desc = '';
var systemlog_desc='';
var dev_desc = '';
var port_prop_desc = '';
var port_stat_desc = ''
var port_cable_desc = '';
var service_desc = '';
var service_pcab_stat_desc = '';

var password_desc = '';
	
var upgrade_desc = '';
var configure_desc = '';
var status_desc = 
	'You can use the Status screen to see the connection status \
	for the Broadband Router\'s WAN/LAN interfaces, firmware and hardware version numbers, \
	and the number of connected clients to your network.';
var lan_desc = 
	'The Broadband Router must have an IP address for the local network. You can also enable DHCP service \
	for dynamic IP address allocation to your clients, \
	or configure filtering functions based on specific clients or protocols.';
var lan_dhcp_desc = 'You can enable DHCP to dynamically allocate IP addresses to your client PCs.';

var vlan_desc = '';
var vlan_8021q_desc = '';
var vlan_trunk_desc='';
var vlan_hybrid_desc='';
var user_desc = '';
var service_map_desc = '';
var user_white_desc = '';
var user_black_desc = '';
var user_anon_desc = '';
var user_list_desc='';
var user_template_desc='';
var temp_list='';
var file_desc = '';
var user_new_desc='';
var service_sta_prop_desc = '';
var vlan_mode_desc = '';

var Pages=new Array(
new Page(0,'Login','login.asp','System Login','','',3)
,new Page(0,'LoginErr','loginerr.asp','Login','','',3)
,new Page(0,'Duplicate','duplicate.asp','Duplicate Administrator','','',3)
,new Page(0,'OK','do_cmd.asp','Message','','',2)
,new Page(0,'OK','do_upgrade.asp','Message','','',2)
,new Page(0,'','index.asp','Setting',system_desc,'',2)
,new Page(0,'Save Updates','system_config_save.asp', 'Save Updates', configure_desc, '')
,new Page(0,'Advanced','frame_setup.asp','Advanced',advanced_desc,'',1)
,new Page(1,'System settings','system_status.asp','System Status',system_desc,'',1)
,new Page(1,'System Status','system_status.asp', 'System Status', system_desc, '',1)
,new Page(1,'IP Address', 'system_man_ip.asp', 'Management IP Address', system_man_ip_desc, '')
,new Page(1,'Security', 'system_man_vlan.asp', ' Security Setting', system_man_vlan_desc, '')
,new Page(1,'Date & Time','system_time.asp','Date & Time',time_desc, '')
,new Page(1,'SNMP','system_snmp.asp', 'SNMP Setting', snmp_desc, '')
,new Page(1,'Administrator','system_password.asp', 'Administrator', password_desc, '')
,new Page(1,'Configuration','system_configure.asp', 'Configuration', configure_desc, '')
,new Page(1,'Reset','system_reset.asp','Reset','','')
,new Page(1,'Firmware Upgrade','system_upgrade.asp', 'Firmware Upgrade', upgrade_desc, '')
,new Page(1,'System Log','system_log.asp', 'System Log', systemlog_desc, '')
,new Page(2,'Configuration','port_prop.asp', 'Configuration Ethernet Ports', port_prop_desc, '', 1)
,new Page(2,'Ethernet Ports','port_prop.asp', 'Configuration Ethernet Ports', port_prop_desc, '')
,new Page(2,'Cable Port','port_cable_prop.asp', 'Configuration Cable Port', port_prop_desc, '')
,new Page(2,'VLAN Mode', 'vlan_mode.asp', 'Configuration VLAN Mode', vlan_mode_desc, '')
,new Page(2,'802.1Q VLAN', 'vlan_8021q.asp', 'Configuration 802.1Q VLAN', vlan_8021q_desc, '')
,new Page(2,'Trunk', 'vlan_trunk.asp', 'Configuration Trunk', vlan_trunk_desc, '')
,new Page(2,'Hybrid', 'vlan_hybrid.asp', 'Configuration Hybrid', vlan_hybrid_desc, '')
,new Page(2,'Port Status','port_stat.asp', 'Port Status', port_stat_desc, '')
,new Page(3,'Management','service_map.asp', 'Network Topology', service_map_desc, '', 1)
,new Page(3,'Network Topology','service_map.asp', 'Network Topology', service_map_desc, '')
,new Page(3,'Device','service_sta_prop.asp', 'Device', service_sta_prop_desc, '', 2)
,new Page(3,'Templates','service_templates.asp', 'Templates List', temp_list, '')
,new Page(3,'xx Template','service_template_prop.asp', 'xx Template', user_template_desc, '', 2)
,new Page(3,'Cable Statistics', 'service_pcab_stat.asp', 'Cable Statistics', service_pcab_stat_desc, '', 2)
,new Page(4,'Users','user_list.asp', 'Users List', user_list_desc, '', 1)
,new Page(4,'Users List','user_list.asp', 'Users List', user_list_desc, '')
,new Page(4,'User Info','user_edit.asp', 'User Info', user_new_desc, '', 2)
,new Page(4,'Anonymous','user_anon.asp', 'Anonymous', user_anon_desc, '')
,new Page(4,'Whitelist','user_white.asp', 'Whitelist', user_white_desc, '')
,new Page(4,'Blacklist','user_black.asp', 'Blacklist', user_black_desc, '')

);

function GURL(x){location=x;}

function findPage()
{
	focusPage = Pages[0];
	
	for (var i=0;i<Pages.length;i++)
	{
		if(DIV)
		{
			if (Pages[i].p==DIV)
			{
				focusPage = Pages[i];
				break;
			}
		}
		else
		{
			if (Pages[i].p==cPage)
			{
				focusPage = Pages[i];
				break;
			}
		}
	}
		
	return focusPage;
}

function wizleftMenu()
{
	var m = '';
	m += '<td valign=top><img src=images/cat_quicksetup.gif border=0>';
	m += '<table border=0 align=center cellpadding=0 cellspacing=0>';
	
	/* Show menu */
	for (var i=1;i<wizGroups.length;i++)
	{
		m += '<tr>';
		m += '<td><img src=images/spacer.gif width=8 height=8></td>';
		m += '</tr>';
		m += '<tr>';
		m += '<tr>';
		m += '<td background=images/';
		if(focusPage.g >= i+6)
		{
			m += 'wizbut1.gif';
		}
		else
		{
			m += 'wizbut0.gif';
		}
		m += ' valign=middle align=left width=144 height=40>'
			+'<span class=btn_w>&nbsp;&nbsp;&nbsp;' + wizGroups[i].s + '</span></a></td>';
		m += '</tr>';
	}
	
	for (var j=0; j<3; j++)
		m += '<tr><td><img src=images/spacer.gif width=8 height=8></td></tr>';
	
	/* Show Home and Logout */
	m += '<tr><td>';
	m += '<table border=0 align=center cellpadding=0 cellspacing=0>';
	m += '<tr>';
	m += '<td></td>';
	m += '<td><input class=btn_o type=button value=Home onclick=GURL("index.asp")></td>';
	m += '<td></td>';
	m += '</tr>';
	m += '<tr>';
	m += '<td></td>';
	m += '<td><img src=images/spacer.gif width=8 height=8></td>';
	m += '<td></td>';
	m += '</tr>';
	m += '<tr>';
	m += '<td></td>';
	m += '<td><input class=btn_o type=button value=Logout onclick=GURL("login.asp")></td>';
	m += '<td></td>';
	m += '</tr>';
	m += '</table>';
	m += '</td></tr>';
	
	m += '</table>';
	m += '<p>&nbsp;</p>';
	m += '</td>';
	
	return m;
}

function leftMenu(update)
{
var m = '';
//	m += '<td valign=top><img src=images/cat_advancedsetup.gif border=0>';
	m += '<td valign=top>';
	m += '<table border=0 align=center cellpadding=0 cellspacing=2>';
	
	/* Show menu */
	for (var i=1;i<Groups.length;i++)
	{
		m += '<tr>';
		m += '<td><img src=images/spacer.gif width=8 height=8></td>';
		m += '</tr>';
		m += '<tr>';
		m += '<td><input class=btn_o type=button value=\'' + Groups[i].s +'\' onclick=GURL(\'' + Pages[Groups[i].pid].p + '\')></td>';
		m += '</tr>';
		  
		if (focusPage.g == i)
		{
			m += '<tr>';
			m += '<td>';
			m += '<table width=100% border=0 cellpadding=0 cellspacing=0 class=submenubg>';
			m += '<tr>';
			m += '<td colspan=2><img src=images/but_top.gif width=144 height=3></td>';
			m += '</tr>';
			m += '<tr>';
			m += '<td colspan=2>';
			m += '<table width=100% border=0 cellpadding=2>';
			
			for (var j=0;j<Pages.length;j++)
			{
				if (Pages[j].g == i && Pages[j].f != 2 && Pages[j].f != 1)
				{
					m += '<tr>';
					m += '<td width=13% align=right valign=top><b><font color=#FFFFFF>&bull; </font></b></td>';
					m += '<td width=87% valign=top>'
						+ '<a href=' + Pages[j].p + ' class=submenus>'
						+ Pages[j].s + '</a></td>';
					m += '</tr>';
				}
			}
			
			m += '</table>';
			m += '</td>';
			m += '</tr>';
			m += '<tr>';
			m += '<td colspan=2><img src=images/but_bottom.gif width=144 height=3></td>';
			m += '</tr>';
			m += '</table>';
			m += '</td>';
			m += '</tr>';
		}
	}

	for (var j=0; j<3; j++)
		m += '<tr><td><img src=images/spacer.gif width=8 height=8></td></tr>';
	
	var bstr = update ? 'Save Updates<span style="color:red;font:bold 20px">*</span>' : 'Save Updates';
	
	/* Show Home and Logout */
	m += '<tr><td>';
	m += '<table border=0 align=center cellpadding=0 cellspacing=0>';
	m += '<tr>';
	m += '<td></td>';
	m += "<td><button class=btn_o onclick=GURL('system_config_save.asp')> " + bstr +  "</button></td>";	
	m += '<td></td>';
	m += '</tr>';
	m += '<tr>';
	m += '<td></td>';
	m += '<td><img src=images/spacer.gif width=8 height=8></td>';
	m += '<td></td>';
	m += '</tr>';
	m += '<tr>';
	m += '<td></td>';
	m += '<td><input class=btn_o type=button value=Exit onclick=GURL("login.asp")></td>';
	m += '<td></td>';
	m += '</tr>';
	m += '</table>';
	m += '</td></tr>';
	
	m += '</table>';
	m += '<p>&nbsp;</p>';
	m += '</td>';
	
	return m;
}

function rightHead()
{
var m =	'';
	m += '<td width=80% valign=top style="padding-top:12px">';
	m += '<table width=100% border=0 cellpadding=0 cellspacing=0>';
	m += '<tr>';
	m += '<td width=10><img src=images/c2_tl.gif width=10 height=10></td>';
	m += '<td background=images/bg2_t.gif><img src=images/spacer.gif width=10 height=10></td>';
	m += '<td width=10><img src=images/c2_tr.gif width=10 height=10></td>';
	m += '</tr>';
	m += '<tr>';
	m += '<td background=images/bg2_l.gif><img src=images/spacer.gif width=10 height=10></td>';
	
	/* Text area start */
	if (focusPage.f != 2)
		m += '<td height=400 valign=top bgcolor=#FFFFFF>';
	else
		m += '<td valign=top bgcolor=#FFFFFF>';
	
	/* Banner */
	m += '<table width=100% border=0 cellpadding=3 cellspacing=0>';
	m += '<tr>';
	m += '<td class=headerbg><div class=headerbg id="page_header">' + focusPage.t + '</div></td>';
	
	if (focusPage.h != '')
	{
		m += '<td width=15% class=headerbg>'
			+'<input type=button value=Help'
			+' onClick=window.open("help.asp#' + focusPage.h + '","help","toolbar=0,status=0,menubar=0,scrollbars=1,resizable=1,width=530,height=400,left=150,top=150");'
			+' class=btn_o></td>';
	}
		
	m += '</tr>';
	m += '</table>';
	
	/* Online description */
	m += '<table width="100%" border="0" cellpadding="5" cellspacing="1" bgcolor="#FFFFFF">';
	m += '<tr>';
	m += '<td width=100% colspan="2">'
	m += '<div id="page_descript">' + focusPage.d + '</div>';
	
	/* Start of form */
	return m;
}

function rightTail()
{
var m = '';
	
	/* End of form */
	m += '</td></tr></table>';
	
	m += '</td>';
	/* Text area end */
	
	m += '<td background=images/bg2_r.gif><img src=images/spacer.gif width=10 height=10></td>';
	m += '</tr>';
	m += '<tr>';
	m += '<td><img src=images/c2_bl.gif width=10 height=10></td>';
	m += '<td background=images/bg2_b.gif><img src=images/spacer.gif width=10 height=10></td>';
	m += '<td><img src=images/c2_br.gif width=10 height=10></td>';
	m += '</tr>';
	m += '</table>';
	m += '</td>';
	
	return m;
}

function pageHead()
{
	var v = new Array(PROD_MODEL);
	pageHead(v);
}	

function pageHead(webVars)
{
var model = (webVars.length > 0) ? webVars[0] : PROD_MODEL;
var config_changes = false;
if (webVars.length > 1){
	config_changes = (webVars[1] == 1) ? true : false;
}
var p = findPage();
var m = '';
	m += '<table width=850 border=0 cellpadding=0 cellspacing=0>';
	
	/* Top conner */
	m += '<tr>';
  m += '<td width=21><img src=images/c1_tl.gif width=21 height=21></td>';
  m += '<td width=808 background=images/bg1_t.gif><img src=images/top_1.gif width=390 height=21></td>';
  m += '<td width=21><img src=images/c1_tr.gif width=21 height=21></td>';
	m += '</tr>';
	
	/* Middle */
	m += '<tr>';
  m += '<td valign=top background=images/bg1_l.gif><img src=images/top_2.gif width=21 height=69></td>';
	
	/* logo */
	m += '<td background=images/bg.gif>';
	m += '<table width=100% border=0 cellpadding=0 cellspacing=0>';
	m += '<tr>';
	m += '<td width=30%>';
	m += showlogo();
	m += '</td>';
  m += '<td width=87% align=right><font size=5 color="#FFFFFF">';
  m += model;
  m += ' EoC Master Controller</font>';
	m += '</tr>';
	m += '</table>';
	
	/* lef and right */
	m += '<table width=100% border=0 cellpadding=0 cellspacing=0>';
	m += '<tr>';
	
	if(DIV)
	{
		m += wizleftMenu();
		m += '<td width=2%><img src=images/spacer.gif width=15 height=15></td>';
	}
	else
	{
		if (p.f==0 || p.f==1 || p.f==2)
		{
			m += leftMenu(config_changes);
			m += '<td width=2%><img src=images/spacer.gif width=15 height=15></td>';
		}
	}
	
	m += rightHead();
	
	document.writeln(m);
}

function pageTail()
{
var m = rightTail();
	
	m += '</tr>';
	m += '</table>';
	/* End left and right */
	
	m += '</td>';
	m += '<td background=images/bg1_r.gif>&nbsp;</td>';
	m += '</tr>';
	
	/* Bottom */
	m += '<tr>';
	m += '<td><img src=images/c1_bl.gif width=21 height=20></td>';
	m += '<td align=right background=images/bg1_b.gif><font color=white>' + str_copyright + '</font></td>';
	m += '<td><img src=images/c1_br.gif width=21 height=20></td>';
	m += '</tr>';
	
	m += '</table>';
	
	document.writeln(m);
}

function show_hide(el,shownow)
{
	if (document.all)
		document.all(el).style.display = (shownow) ? "block" : "none";
	else if (document.getElementById)
		document.getElementById(el).style.display = (shownow) ? "block" : "none";
}

function MM_preloadImages() { //v3.0
  var d=document; if(d.images){ if(!d.MM_p) d.MM_p=new Array();
    var i,j=d.MM_p.length,a=MM_preloadImages.arguments; for(i=0; i<a.length; i++)
    if (a[i].indexOf("#")!=0){ d.MM_p[j]=new Image; d.MM_p[j++].src=a[i];}}
}

function MM_swapImgRestore() { //v3.0
  var i,x,a=document.MM_sr; for(i=0;a&&i<a.length&&(x=a[i])&&x.oSrc;i++) x.src=x.oSrc;
}

function MM_findObj(n, d) { //v4.01
  var p,i,x;  if(!d) d=document; if((p=n.indexOf("?"))>0&&parent.frames.length) {
    d=parent.frames[n.substring(p+1)].document; n=n.substring(0,p);}
  if(!(x=d[n])&&d.all) x=d.all[n]; for (i=0;!x&&i<d.forms.length;i++) x=d.forms[i][n];
  for(i=0;!x&&d.layers&&i<d.layers.length;i++) x=MM_findObj(n,d.layers[i].document);
  if(!x && d.getElementById) x=d.getElementById(n); return x;
}

function MM_swapImage() { //v3.0
  var i,j=0,x,a=MM_swapImage.arguments; document.MM_sr=new Array; for(i=0;i<(a.length-2);i+=3)
   if ((x=MM_findObj(a[i]))!=null){document.MM_sr[j++]=x; if(!x.oSrc) x.oSrc=x.src; x.src=a[i+2];}
}

var timeTable=[[0,"-12:00","Enewetak, Kwajalein"],
[1,"-11:00","Midway Island, Samoa"],
[2,"-10:00","Hawaii"],
[3,"-09:00","Alaska"],
[4,"-08:00","Pacific Time (US &amp; Canada);Tijuana"],
[5,"-07:00","Arizona"],
[6,"-07:00","Mountain Time (US &amp; Canada)"],
[7,"-06:00","Central Time (US &amp; Canada)"],
[8,"-06:00","Mexico City, Tegucigalpa"],
[9,"-06:00","Saskatchewan"],
[10,"-05:00","Bogota, Lima, Quito"],
[11,"-05:00","Eastern Time (US &amp; Canada)"],
[12,"-05:00","Indiana (East)"],
[13,"-04:00","Atlantic Time (Canada)"],
[14,"-04:00","Caracas, La Paz"],
[15,"-04:00","Caracas, La Paz"],
[16,"-03:00","Newfoundland"],
[17,"-03:00","Brasilia"],
[18,"-03:00","Buenos Aires, Georgetown"],
[19,"-02:00","Mid-Atlantic"],
[20,"-01:00","Azores, Cape Verde Is."],
[21,"-01:00","Casablanca, Monrovia"],
[22,"+00:00","Greenwich Mean Time: Dublin, Edinburgh"],
[23,"-01:00","Greenwich Mean Time: Lisbon, London"],
[24,"+01:00","Amsterdam, Berlin, Bern, Rome"],
[25,"+01:00","Stockholm, Vienna, Belgrade"],
[26,"+01:00","Bratislava, Budapest, Ljubljana"],
[27,"+01:00","Prague,Brussels, Copenhagen, Madrid"],
[28,"+01:00","Paris, Vilnius, Sarajevo, Skopje"],
[29,"+01:00","Sofija, Warsaw, Zagreb"],
[30,"+02:00","Athens, Istanbul, Minsk"],
[31,"+02:00","Bucharest"],
[32,"+02:00","Cairo"],
[33,"+02:00","Harare, Pretoria"],
[34,"+02:00","Helsinki, Riga, Tallinn"],
[35,"+02:00","Helsinki, Riga, Tallinn"],
[36,"+03:00","Baghdad, Kuwait, Nairobi, Riyadh"],
[37,"+03:00","Moscow, St. Petersburg"],
[38,"+03:00","Tehran"],
[39,"+04:00","Abu Dhabi, Muscat, Tbilisi, Kazan"],
[40,"+04:00","Volgograd, Kabul"],
[41,"+05:00","Islamabad, Karachi, Ekaterinburg"],
[42,"+06:00","Almaty, Dhaka"],
[43,"+07:00","Bangkok, Jakarta, Hanoi"],
[44,"+08:00","Beijing, Chongqing, Urumqi"],
[45,"+08:00","Hong Kong, Perth, Singapore, Taipei"],
[46,"+09:00","Toyko, Osaka, Sapporo, Yakutsk"],
[47,"+10:00","Brisbane"],
[48,"+10:00","Canberra, Melbourne, Sydney"],
[49,"+10:00","Guam, Port Moresby, Vladivostok"],
[50,"+10:00","Hobart"],
[51,"+11:00","Magadan, Solamon, New Caledonia"],
[52,"+12:00","Fiji, Kamchatka, Marshall Is."],
[53,"+12:00","Wellington, Auckland"]];

function genTimeOpt()
{
	var s='';
	for (i=0; i<timeTable.length; i++)
	{
		var t=timeTable[i];
		s+='<option value='+t[0]+'> (GMT'+t[1]+') '+t[2]+'</option>\n';
	}
	document.write(s);
}

function show_month_option()
{
	var str=new String("");
	var i;

	for(i=1;i<=12;i++)
	{
		str="";
		str+="<option value="+i+">"+month[i-1]+"</option>";
		document.write(str);
	}
}
function show_day_option()
{
	var str=new String("");
	var i;

	for(i=1;i<=31;i++)
	{
		str="";
		str+="<option value="+i+">"+ (i<10 ? "0" : "") +i+"</option>";
		document.write(str);
	}
}

function setCln(f,m)
{
	f.clnEn.value=1;
	decomMAC2(f.WMAC, m, 1);
	f.cln.value='Clone MAC Address';
}

function clrCln(f)
{
	f.clnEn.value=0;
	decomMAC2(f.WMAC, '', 1);
	f.cln.value='Duplicate MAC address from the customer end';
}

function clnMac(f)
{
	if  (f.clnEn.value=='1') clrCln(f); else setCln(f,cln_MAC);
}

function evalDnsFix(ds1,ds2)
{
	if ((ds1==''||ds1=='0.0.0.0')&&(ds2==''||ds2=='0.0.0.0')) return 0;
	return 1;
}

function isNumber(n)
{
    if (n.length==0) return false;
    for (var i=0;i < n.length;i++)
    {
        if (n.charAt(i) < '0' || n.charAt(i) > '9') return false;
    }
    return true;
}

function isBlank(s)
{
    var i=0;
    for (i=0; i < s.length;i++)
    {
        if (s.charCodeAt(i)!=32) break;
    }
    if (i==s.length) return true; 
   
    return false;
}
function chksc(n, msg)
{
	var ck=/[\;]/;
	if (ck.test(n))
	{
		alert(msg+" Include the invalid character: \;");
		return false;
	}
	return true;
}


function pageTitle()
{
	var p = findPage();
	document.title= PROD_MODEL + ' - ' + p.t;
}

function geturlparam()
{
var s=window.location.href;
var start=s.indexOf("?v=");
var end=s.length;
var str=s.substring(start,end);
str=str.substring(3);
return str;

}


