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
</SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<script language=JavaScript>

pageTitle();	


var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var debug_info = new Array(<% CGI_SYS_SETDEBUG(); %>);
/* 
var webVars = new Array( 'EPN104', '0', '0');
var debug_info=new Array('0.2.0.0','0.0.1.1');
*/


var messageitem=new Array(
"MAC 地址",				
"生产序列号"
);
function show_version(f) 
{
	var m='';
	for(i=0;i<messageitem.length;i++)
	{
	    if(!(debug_info[i]))debug_info[i]="";
	    m+='<tr class=\'textCell\' align=center >'+
			'<TD width=200 align=center class=textcellC>'+messageitem[i]+'</TD>'+
			'<td width=400 align=center class=textcellC>'+debug_info[i]+'</td>'+		
		'</tr>';
	}
	document.write(m);	
}	

function apply(dat)
{
	var f=document.forms[0];
	if(dat==0)
	{
		diffCfg("MAC", "sys_mac", f.MAC.value);
	}
	else
	{	
		diffCfg("MANUCODE", "SN", f.MANUCODE.value);
	}	
	subForm(f,'goform/command','SET_DEBUG','setdebug.asp');
}



</script>
</head>
<body>
<script>pageHead(webVars,PROD_MODEL);</script>
<form name=system_debug action=goform/command method=post>
	<TABLE cellSpacing=1 cellPadding=2 border=1 width=100%>
		<tr class=item1 align=center height=30>
			<TD class=smalltable width=200 align=center>项目</TD>
			<td class=smalltable width=400 align=center>值</td>
		</tr>		
		<script>
		show_version(this.form);
		</script>
		
	</TABLE>
  <TABLE cellSpacing=1 cellPadding=2 border=0 width=100%> 
	<TR>
    </TR> 
    <TR><TD height=8></TD></TR>
    <TR class=textCell >
		<TD width="300">MAC 地址(xxxxxxxxxxxx)</TD>
		<TD width=30%><input type="text" size="64" id="mac" name="MAC"  ></TD> 
		<td class=textCellRed align = left id=checkip></td>
		
		<TD>
		<input type=button value="设置 MAC 地址" onclick=apply(0) >
		</TD>
	</TR>
    <TR class=textCell >
		<TD width="300">生产序列号 (EPNxxxxxxxxxx)</TD>
		<TD><input type="text" size="64" id="code" name="MANUCODE" ></TD> 
		<td class=textCellRed align = left id=checkport></td>
		
		<TD>
		<input type=button value="设置生产序列号" onclick=apply(1) >
		</TD>
    </TR>
 </TABLE>	
</form>

<script>pageTail(str_copyright);</script>
</body>
</html>
