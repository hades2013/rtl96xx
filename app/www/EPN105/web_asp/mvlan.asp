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
var mvlan_value = new Array(<% CGI_MVLAN(); %>);

/* just for test
var webVars = new Array();
var mvlan_value = new Array('100');
*/
function get_mvlan()
{
	if (mvlan_value.length > 0)
	{
		var vs = mvlan_value[0].split(';');
		if (vs.length > 0) return vs[0];
	}
	return 'N/A';	
}

function init()
{	
	var f=document.forms[0];
	f.MVLAN.value = get_mvlan();
}

function mvlan_onpress_check(event)
{	
	var keynum;
    if(window.event) // IE
    {
		keynum = event.keyCode
    }
	else if(e.which) // Netscape/Firefox/Opera
    {
		keynum = event.which
	}
    if( keynum < 48 || keynum > 57 )
    {
		event.returnValue = false;
		return false;
    }
}


function valid_mvlan_input( input )
{
    var valid = true;
    var inputtemp;
    var ch;
    var cnt;
    var len;
	
    inputtemp = input.toString();
	len = inputtemp.length;
	if ( 0 == len )
	{
		valid = false;
        return valid;
	}
	for ( cnt = len - 1; cnt >= 0; cnt-- )
	{
		ch = inputtemp.charAt( cnt );
		if ( ' ' == ch )
		{
			len--;
		}
		else
	    {
		    break;
	    }	
	}
    
    for ( cnt = 0; cnt < len; cnt++ )
    {
        ch=inputtemp.charAt(cnt);
        if((ch > '9')||(ch < '0'))
        {
            valid = false;
            return valid;
        }
    }
    if((input > 4094) || (input < 1))
    {
        valid = false;
    }
    return valid;
}


function check_all_input()
{
	var f = document.forms[0];
	if(false == valid_mvlan_input(f.MVLAN.value))
	{
		alert("请输入提示范围内的整数！");
		return false;
	}
	return true;
}

function apply_all()
{	
	var nextpage;
	var newip = 0;
	var f=document.forms[0];
	
	if (!check_all_input())
	{	
		return false;
	}
	diffCfg("MVLAN", "mvlan", f.MVLAN.value);
	subForm(f,'goform/command','SYS_MVLAN', 'mvlan.asp');
}


</script>
</head>

<body onload=init()>
<script>pageHead(webVars,PROD_MODEL);</script>

<form name=mvlan action=goform/command method=post>


<table width=100% border=0 align=center cellpadding=5 cellspacing=1 class=box_tn>
<tr>
	<td colspan=2 class=greybluebg>管理VLAN</td>
</tr>
<tr>
	<td class=bgblue>管理VLAN</td>
	<td class=bggrey>
	<input name=MVLAN size=6 onkeypress="mvlan_onpress_check(event);" onpaste="return false;" style="ime-mode:disabled" maxlength=4>(1-4094)
	<input style="display:none" mce_style="display:none"><!--只有一个input类型为text的,使其回车不提交表单-->
	</td>
</tr> 
</table>

<table>
 <TR>
<br><br />
<input type=button value="确认" onclick=apply_all() >
<input type=button value="取消" onclick=init() >
</TR>
 <br><br />
 <TR>
    <td><p><b>说明：</b>&nbsp;配置管理vlan过程需要一段时间，在此期间请勿操作页面！</p> </td>
</TR>

</table>
</form>


<script>pageTail(str_copyright);</script>
</body>
</html>
