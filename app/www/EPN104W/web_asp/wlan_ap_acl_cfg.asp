<HTML><HEAD>
<META NAME=Copyright CONTENT="Copyright (c) 2007 H3C Corporation. All Rights Reserved.">
<META http-equiv=Content-Type content=text/html;charset=gb2312>
<META HTTP-EQUIV=Pragma CONTENT=no-cache>
<TITLE>MAC����</TITLE>
<LINK href=globe.css type=text/css rel=stylesheet>
<SCRIPT src=icg_helpScript.js></SCRIPT>
<SCRIPT language=JavaScript src=icg_utils.js></SCRIPT>
<SCRIPT src=icg_navigatortab.js></SCRIPT>
<SCRIPT src=icg_contrlButton.js></SCRIPT>
<SCRIPT>

var mac_list = new Array( <% MAC_LIST(); %>);
var a = window.dialogArguments;
var tar='';

window.returnValue=false;

function cancelIt() {
	window.returnValue=false;
	window.close();
}

function init(f)
{
	if (a)
	{
		f.mac_addr.value=a[1];
		f.mac_note.value=a[2];
		f.amend.value="�޸�";
	}

}

function note_check(P)
{
}

function add_edit_mac(f)
{
	var i;
	var msg;

    if (check_mac_noempty(f.mac_addr,"MAC��ַ"))
	{
		return;
	}

    if( 0!=name_check_with_chiness(f.mac_note,"����"))
    {
        return ;
    }
    
	if ((Number(mf_curnum)>=Number(mf_maxnum)) && (f.amend.value!="�޸�"))
	{
		alert("MAC���˱�������������������!");
		return;
	}

	for (i = 0; i < mac_list.length; i++)
	{
		tmpmac=mac_list[i].split(";");
		if(f.amend.value=="�޸�")
		{
			/* ��Ӧ�ı༭���� */
			if(a[0] == tmpmac[0])
			{
				if((tmpmac[1]==f.mac_addr.value) && (tmpmac[2] == f.mac_note.value))
				{
					cancelIt();
					return;
				}
				else
				{
					continue;
				}
			}
		}
		
	    if(tmpmac[1].toLowerCase() == f.mac_addr.value.toLowerCase())
	    {
            alert("�������������������ظ�������������!");
            f.mac_addr.select();
            return;
	    }
	}
	
	if (f.amend.value!="�޸�")
	{
		f.CMD.value='AddWlanMacList';
        msg = "-1;" + f.mac_addr.value.toUpperCase() + ";" + f.mac_note.value + ";";
	}
	else
	{
		f.CMD.value='EditWlanMacList';
        msg = a[0] + ";" + f.mac_addr.value.toUpperCase() + ";" + f.mac_note.value + ";";
	}
	f.GO.value=cPage;
	f.param.value=msg;
	f.action="/goform/aspForm";
	f.submit();
	window.returnValue=true;
	setTimeout("window.close()",100);
}

</SCRIPT>
</HEAD>
<BODY bgcolor=#FFFFFF leftMargin=0 topMargin=0 MARGINWIDTH=0 MARGINHEIGHT=0 BORDER=0 onLoad=init(document.frmSetup);>
<table>
<tr><td colspan=4>&nbsp;
<tr>
<form name=frmSetup action='/goform/aspForm' method=post target='hf'>
		<td width=10>&nbsp;
			<input type=hidden name=CMD>
			<input type=hidden name=GO>
			<input type=hidden name=param>
		</td>
		<iframe name="hf" src="arp_tmp.htm" width=0 height=0>
		</iframe>
		<td width=500 covalign=top>
		<TABLE cellSpacing=0 cellPadding=2 border=0 width=100%>						
					
					<TR id=mac_addr>
						<TD width=100 class=nameTextCell>MAC��ַ��</TD>
						<TD width=10>
						<TD class='textCell' width=450><input name="mac_addr" type="text" size="25" maxlength="17"></TD></TR>
					<TR class='textCell' id=mac_note>
						<TD width=100 class=nameTextCell>������</TD>
						<TD width=10>
						<TD class='textCell' width=450><input name="mac_note" type="text" size="25" maxlength=15>&nbsp;(��ѡ, ��Χ:1~15���ַ�)</TD></TR>

					<TR class='textCell'><td colspan=3>&nbsp;	
					<TR class='textCell'>
					<TD width=150 class=nameTextCell></TD>
					<TD width=30>
					<TD class='textCell' width=300>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<INPUT type=button class=button name=amend value=���� onClick=add_edit_mac(frmSetup) onKeyPress=Disable_Enter(event,this)>
						<INPUT type=button class=button name=ipmacInput value=ȡ�� onClick="cancelIt()"></TD></TR>
				</table>
		</TABLE>
</tr>
</table>

</FORM>
</BODY></HTML>






