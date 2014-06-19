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
var file_list = new Array (
		<% CGI_FS_DIR(); %>
);

function fchk(idx)
{
	var f=document.forms[0];
	f.facc.value = idx;
}

function show_file_list()
{
	var str = '';
	for(var i = 0; i < file_list.length; i ++) {
		var fl = file_list[i];
		var s = fl.split(';');
		str += '<tr>';
		str += '<td width=2% class=bggrey><input type="checkbox" name=fcb' + i + ' onclick="fchk(' + i + ');"></td>';
		str += '<td width=30% class=bggrey>' + s[0] + '</td>';
		str += '<td width=10% class=bggrey align=right>' + s[1] + '</td>';		
		str += '<td width=40% class=bggrey>' + s[2] + '</td>';		
		str += '<td width=10% class=bggrey>' + s[3] + '</td>';		
		str += '<td width=10% class=bggrey>' + s[4] + '</td>';
		str += '</tr>';
	}
	document.writeln(str);		
}

function select_file_list()
{
	var str = '';
	for(var i = 0; i < file_list.length; i ++) {
		var fl = file_list[i];
		var s = fl.split(';');
		str += '<option value=' + i + '>' + s[0] + '</option>';
	}
	document.writeln(str);
}

function init()
{
	var f=document.forms[0];
	cfg2Form(f);
	snmp_endis();
}

function apply()
{
	//document.title = "Hello";
	var f=document.forms[0];
	form2Cfg(f);
	subForm(f,'goform/command','SYS',cPage);
}

function preSubmit(idx) 
{
	var f=document.forms[0];

	f.action.value=idx
	f.submit() ;
}

function renamecopy()
{
	var f=document.forms[0];
	var srcfn = file_list[f.facc.value].split(';')[0];
	var dstfn = f.renamecopy_new.value;
	if(dstfn.length <= 0) {
		alert('目的文件名不能为空');
		return;
	}
	f.nowait.value = 1;
	if(f.rename_copy.value == 0) {
		if(confirm('确认将文件 ' + srcfn + ' 重命名为 ' + dstfn)) {
			f.ACTION.value = 'rename';
			f.SRCFILE.value = srcfn;
			f.DSTFILE.value = dstfn;
			f.submit();
		}
	} 
	else {
		if(confirm('确认将文件 ' + srcfn + ' 复制为 ' + dstfn)) {
			f.ACTION.value = 'copy';
			f.SRCFILE.value = srcfn;
			f.DSTFILE.value = dstfn;
			f.submit();
		}
	}
		
}

function delfile()
{
	
}

function upload() 
{
	f=document.forms[1];
	if (f.upfile.value=="")
    {
		alert("Please select the file to upload!");
		return ;
	}
    
	if (confirm('Do you really want to upload ?'))
	{
		f.FILENAME.value = f.upfile.value;
		f.submit() ;
		alert("It may take some time to complete the upload process.\n Please don't turn off the Broadband Router before the upload is complete.");
	}
}
</script>
</head>

<body onload=init()>
<script>pageHead(webVars,PROD_MODEL);</script>
<form name=file_main action=goform/command method=post>

<INPUT type=hidden name=CMD value=FS>
<INPUT type=hidden name=GO value="file_main.asp">
<INPUT type=hidden name=ACTION>
<INPUT type=hidden name=SRCFILE>
<INPUT type=hidden name=DSTFILE>
<INPUT type=hidden name=nowait>

<table width=100% border=0 align=center cellpadding=5 cellspacing=1
	class=box_tn>
	<tr>
		<td colspan=2 class=greybluebg>文件列表</td>
	</tr>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1	bgcolor="#FFFFFF">
			<tr>
				<td width=5% class=bgblue2></td>
				<td width=80% class=bgblue2 align=center>文件名</td>
				<td width=20% class=bgblue2>大小</td>
				<td width=20% class=bgblue2>创建或更改日期</td>
				<td width=20% class=bgblue2>类型</td>
				<td width=20% class=bgblue2>适用范围</td>
			</tr>
			<script>show_file_list();</script>
		</table>
		</td>
	<tr>
		<td colspan=2 class=greybluebg>文件复制/重命名</td>
	</tr>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1	bgcolor="#FFFFFF">
			<tr>
				<td class=bggrey2>源文件</td>
				<td class=bggrey2>操作</td>
				<td class=bggrey2>目的文件</td>
				<td class=bggrey2></td>
			</tr>
			<tr>
				<td class=bggrey>
					<select name=facc>
					<script>select_file_list();</script>
					</select>
					</td>
				<td class=bggrey align=center>
					<select name=rename_copy>
						<option value=0>重命名</option>
						<option value=1>复制为</option>
					</select>
				<td class=bggrey><input name=renamecopy_new size=20></td>
				<td class=bggrey><input type=button value="执行" onclick="renamecopy();"></td>
			</tr>
		</table>
		</td>
	</tr>
	<tr>
		<td colspan=2 class=greybluebg>删除文件</td>
	</tr>
	<tr>
		<td>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1	bgcolor="#FFFFFF">
			<tr>
				<td class=bggrey align=center><input type=button value="        删除选中的文件        " onclick="delfile();"></td>
			</tr>
		</table>
		</td>
	</tr>
</form>
	<tr>
		<td colspan=2 class=greybluebg>上传文件</td>
	</tr>
	<tr>
		<td>
<form name="file_upload" action=goform/command method=post>
		<INPUT type=hidden name=FILENAME>
		<table width=100% border=0 align=center cellpadding=5 cellspacing=1	bgcolor="#FFFFFF">
			<tr>
				<td class=bggrey>源文件</td>
				<td class=bggrey><input type=file size=50 name=upfile></td>
			</tr>
			<tr>
				<td class=bggrey>操作 </td>
				<td class=bggrey align=center><input type=button value="                上传                " onclick="upload();"></td>
			</tr>			
		</table>
		</td>
	</tr>
</table>
</form>
<script>pageTail(str_copyright);</script>
</body>
</html>
