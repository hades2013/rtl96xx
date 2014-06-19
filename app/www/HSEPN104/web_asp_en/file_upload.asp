<!--REAL
<%
	int rc=0;
	if (req->method == METHOD_POST)
	{
		rc = CGI_file_upload(req);
		CGI_file_upload_free();
	}
%>
REAL-->
<html>
<head> 
<meta http-equiv=Content-Type content="text/html; charset=iso-8859-1">
<link rel="stylesheet" href="style.css" type="text/css">
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<script>
	document.write("<title>"+ui_title+" | System | Upgrade Result</title>");
</script>

<script language=JavaScript>

var err_msg = ["Error","Save Error","Out of Range", "Timed Out","Not Administrator","Invalid File"];

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);
var rc=<% WEB_printf("req", ""%d"", "rc"); %>

function gurl(u) {location='file_main.asp';}

var count=0;
var rods=['|','/','-','\\'];

function show_result()
{
	var msg,btn;

	if (rc>=0) 
	{
		msg="OK";
		btn='<input type=button class=btn_o value=" Continue " onClick="gurl();">';
	}
	else
	{
		var i=-1-rc;
		if (i>=err_msg.length)
			i=0;

		msg=err_msg[i];
		btn='<input type=button class=btn_o value=" Continue " onClick=history.go(-1)>';
	}

	document.getElementById("MSG").innerHTML=msg+" !";
	document.getElementById("BTN").innerHTML=btn;
}

function chk_upgrade()
{
	if (count < 16)
	{
		setTimeout("chk_upgrade()", 500);
		document.getElementById("MSG").innerHTML = rods[(count++)&3];
		return;
	}

	show_result();
}

function init()
{
	document.getElementById("STS").innerHTML = "Uploading..";
	chk_upgrade();
}
</script>
</head>

<body onLoad=init();>
<script>pageHead(webVars);</script>

<table height=250 width=100% border=0 cellpadding=0 cellspacing=0>
<tr>
	<td align=center>
		<div id=STS></div>
		<div id=MSG></div></b>
		<div id=BTN align=center></div>
	</td>
</tr>
</table>
</body>

<script>pageTail();</script>
</html>
