

var system_snmp = new Array(
	'Agent设置','system_snmp.asp',
	'Trap设置','system_snmp_trap.asp',
	'组设置','system_snmp_group.asp',
	'用户设置','system_snmp_user.asp')



function buildNavigatorTab(arrGroup, curTab)
{	
	var itemActive = '<td bgcolor=black align=center nowrap class=navigatorActive><A HREF=%URL% class=select>%TEXT%</a></td>';
	var itemNormal = '<td nowrap align=center class=navigator><A HREF=%URL% class=unselect>%TEXT%</a></td>';
	var itemActLeft = '<td align=right valign=top nowrap bgcolor=#CC0000 class=navigatorNormal><img src=tab_l.gif></TD>';
	var itemActRight= '<td align=right valign=top nowrap bgcolor=#CC0000 class=navigatorNormal><img src=tab_r.gif></TD>';
	var itemLeft = '<td align=right valign=top nowrap bgcolor=#dfdfdf class=navigatorNormal><img src=tab_l.gif></TD>';
	var itemRight= '<td align=right valign=top nowrap bgcolor=#dfdfdf class=navigatorNormal><img src=tab_r.gif></TD>';
/*	
	var navigatorBanner = 
'<table width=100% height=18 border=0 cellpadding=0 cellspacing=0>' +
'<tr><td nowrap class=navigatorNormal></TD>' +
'%ITEM%' +
'<td width=100% nowrap class=navigatorNormal>&nbsp;</TD>' +
'</tr></table>';
*/
///*
	var navigatorBanner = 
//'<tr><td>' + 
'<table width="100%" cellSpacing=1 cellPadding=1>' + 
	'<tr class=titleCell width=100%>' +
		'%ITEM%' +
	'</tr>' +
'</table>' ;
//'</td></tr>';
//*/
	var itemCount = arrGroup.length / 2;
	var tdStr = '';
	for (i = 1; i < itemCount + 1; i ++)
	{	j = (i - 1) * 2;
		if (i == curTab)
		{
			myItemStr = itemActive.replace('%URL%', arrGroup[j + 1]);
			myItemStr = myItemStr.replace('%TEXT%', arrGroup[j]);
			//tdStr += itemActLeft;
			tdStr += myItemStr;
			//tdStr += itemActRight;
		}
		else 
		{
			myItemStr = itemNormal.replace('%URL%', arrGroup[j + 1]);
			myItemStr = myItemStr.replace('%TEXT%', arrGroup[j]);
			//tdStr += itemLeft;
			tdStr += myItemStr;
			//tdStr += itemRight;
		}
	}
	navigatorBanner = navigatorBanner.replace('%ITEM%', tdStr);
	
	document.write(navigatorBanner);
}
