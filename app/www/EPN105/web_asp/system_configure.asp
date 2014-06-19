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
var cfgName = <% CGI_GET_BAKUP_CFG_NAME(); %>;
function loadConfirm()
{
	var f = document.forms[0];
	var file_name=f.files.value;
	
	if( !confirm('确定导入新的配置文件?') )
		return;	
	if (file_name=="")
	{
		alert("请选择一个配置文件!");
		return ;
	}
	var len=file_name.toString().length;
	var name_ext=file_name.toString().substring(len-7, len);
	if(name_ext != ".config")
	{
	    alert("文件类型错误！");
		return;
	}
	f.encoding="multipart/form-data";
	f.action="goform/upldForm"
	
	f.submit();
}

function configSave()
{
	f = document.forms[0];
	//f.action = "config.bin";
	f.action = cfgName;
	f.submit();
}

function restoreConfirm()
{
	f = document.forms[0];
	
	if( !confirm('恢复出厂设置将失去现有的所有配置,请确认?') )
		return;
	diffCfg('config_0', 'config', 'restore');	
	subForm(f,'goform/command','CONFIG', cPage);	
}


</script>
</head>

<body>
<script>pageHead(webVars,PROD_MODEL);</script>

<form name=system_configure action=goform/command method=post>
<INPUT type=hidden name=CMD value=CONFIG>
<INPUT type=hidden name=GO value=index.asp>
<input type=hidden name=TYPE value=CONFIG>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td width=100% class=greybluebg>恢复出厂设置</td>
</tr>
<tr>
	<td width=100% class=bggrey>
		<p align=center>
			<input type=button value="恢复出厂设置" name=restore onclick=restoreConfirm()>
		</p>
	</td>
</tr>
</table>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td width=100% class=greybluebg>导出配置文件</td>
</tr>
<tr>
	<td width=100% class=bggrey>
		<p align=center>
		<input type=button value="导出" name=save onClick=configSave()>
		</p>
	</td>
</tr>
</table>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td width=100% class=greybluebg>导入配置文件</td>
</tr>
<tr>
	<td width=100% class=bggrey>
		<p align=center>
			<input type=file size=25 maxlength=31 name=files>
			<input type=button value="导入" name=load onclick=loadConfirm()>
			<input style="display:none" mce_style="display:none"><!--只有一个input类型为text的,使其回车不提交表单-->
		</p>
	</td>
</tr>
</table>
</form>

<script>pageTail(str_copyright);</script>
</body>
</html>

