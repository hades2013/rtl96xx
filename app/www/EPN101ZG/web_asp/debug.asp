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
var debug_info = new Array(<% CGI_SYS_DEBUG(); %>);
/*
var webVars = new Array( 'EPN104', '0', '0');
var debug_info=new Array(
'0.2.0.0','0.0.1.1','ER3280gV100R001','ER3280gV100D003','2.6.16.26','#984 Wed Dec 15 09:55:26 CST 2010','Wed Dec 15 09:37:07 2010'
);
 */


var messageitem=new Array(
"硬件版本号",
"bootload版本号",
"外部版本号",
"内部版本号",
"内核版本号",
"内核编译时间",				
"文件系统编译时间"
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
</form>

<script>pageTail(str_copyright);</script>
</body>
</html>
