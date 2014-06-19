<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<script language=JavaScript>
var str_copyright =<%CGI_GET_COPYRIGHT();%>
var PROD_MODEL = <%CGI_GET_MODEL();%>
</SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<script language=JavaScript>
	pageTitle();
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

</script>
</head>
<body>
<script>pageHead(webVars,PROD_MODEL);</script>
<script>pageTail(str_copyright);</script>
</body>
</html>
