<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD>
<META http-equiv=Content-Type content="text/html; charset=gb2312">
<META HTTP-EQUIV=Pragma CONTENT=no-cache>
<TITLE>QoS配置</TITLE>
<LINK href=style.css type=text/css rel=stylesheet>
<script type="text/javascript" src="images/iepngfix_tilebg.js"></script> 
<style type="text/css">img, div, input { behavior: url("images/iepngfix.htc") } </style>  
<link rel="stylesheet" href="theme/cvnchina/style.css" type="text/css">
<script language=javascript src=script/env.js></script>
<script language=javascript src=script/utils.js></script>
<script language=javascript src=script/valid.js></script>
<script language=javascript src=script/pageview.js></script>
<script language=javascript src=theme/cvnchina/main.js></script>

<script language=javascript>
var env=new Array();var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

env['vendor.name'] = 'Shanghai Cloud Vision Networks Co., Ltd.';
env['vendor.link'] = 'http://www.cvnchina.com';
env['vendor.copyright'] = '2013-2020';
env['vendor.id'] = 'cvnchina';
env['vendor.logo'] = 'default';

env['sys.theme'] = 'cvnchina';
env['sys.lang'] = 'cn';
env['sys.model'] = 'OCN-OUTDOOR';
env['sys.name'] = 'EOC System';
env['sys.version'] = 'R1.0.5';
env['sys.type'] = 'eoc';
env['sys.mib'] = 'nscrtv-hc';
env['sys.ip'] = '192.168.0.100';
env['sys.user'] = 'admin';
env['sys.time'] = '946684845';
env['sys.linkup'] = '0';
env['sys.linkoff'] = '0';
env['sys.busy'] = '0';
env['sys.ipc'] = '0';
env['sys.vlan'] = '0';
env['sys.unsaved'] = webVars[1];
env['sys.feature'] = 'vlanpool;mme_drop_ports;slave_access_mgmt;bcmp;';
env['web.page'] = current_page();env['sys.parent_name'] = '';
pageRedirectCheck(env);
</script>

<script language="JavaScript" type="text/javascript">
var simulation = false;
//var Qosconfig = new Array('1','0','1;3;5;7');
var Qosconfig = new Array(<% CGI_GET_SYS_QOS(); %>);
var numQOS = 4;
var weightNum = 31;
var numTags = 8;
var qosMode = 0;
var queue_mod = 0;
var prioArray = new Array(1, 0, 0, 1, 2, 2, 3, 3);
var wrr_weight = new Array(1, 2, 4, 8);
var admin = 1;
	

function get_qosmod()
{
	if (Qosconfig.length > 0){
		var vs = Qosconfig[0].split(';');
		if (vs.length > 0) return vs[0];
	}
	return '0';
}

function get_queuemod()
{
	if (Qosconfig.length > 0){
		var vs = Qosconfig[1].split(';');
		if (vs.length > 0) return vs[0];
	}
	return '0';
}

function get_queueweight()
{
	if (Qosconfig.length > 0){
	    for(i = 0; i < numQOS; i++)
	    {
		    wrr_weight[3-i] = Qosconfig[2].split(';')[i];
	    }	
	}
	return "";
}

function init_config()
{
    qosMode = get_qosmod();
    queue_mod = get_queuemod();
    get_queueweight();
	/*for DSCP display*/
	if(qosMode == 1)
	{
		numTags = 4;
		prioArray[0] = 0;
		prioArray[1] = 1;
		prioArray[2] = 2;
		prioArray[3] = 3;
		
	}
}
/*excute before onload*/
init_config();

function loadConfiguration()
{	
	if (queue_mod == 0)
	    document.getElementById('WRR').checked = true;
    else
        document.getElementById('HQWRR').checked = true;
    if((wrr_weight[3]>1)&&(wrr_weight[3]<=31))
    {
        document.qosschedule.queweight1.selectedIndex = wrr_weight[3]-1;  //High
    }
    else
    {
        document.qosschedule.queweight1.selectedIndex = 0;
    }
    if((wrr_weight[2]>1)&&(wrr_weight[2]<=31))
    {
        document.qosschedule.queweight2.selectedIndex = wrr_weight[2]-1;  //High
    }
    else
    {
        document.qosschedule.queweight2.selectedIndex = 0;
    }
    if((wrr_weight[1]>1)&&(wrr_weight[1]<=31))
    {
        document.qosschedule.queweight3.selectedIndex = wrr_weight[1]-1;  //High
    }
    else
    {
        document.qosschedule.queweight3.selectedIndex = 0;
    }
    if((wrr_weight[0]>1)&&(wrr_weight[0]<=31))
    {
        document.qosschedule.queweight4.selectedIndex = wrr_weight[0]-1;  //High
    }
    else
    {
        document.qosschedule.queweight4.selectedIndex = 0;
    }
    document.qosselect.qosmode.selectedIndex = qosMode;
	
}

function tagMapping(qos, tag)
{
    if (prioArray[tag] == qos)
    {
        return "checked";  
    }
    else
    {
        return "";
    }
}

function qosModify()
{
    if (confirm("确定修改优先级类型?")) 
    { 
        apply1(); 
    } 
    else  
    {
        document.qosselect.qosmode.selectedIndex = qosMode;
        return;
    }
}

function GURL(x)
{
    location=x;
}

function apply1()
{
	var f=document.forms[0];
	diffCfg('qosmode', 'qosmode', f.qosmode.value);
	subForm(f,'goform/command','SYS_QOSMODE','qos.asp');	
}

function apply2()
{
	var v=document.forms[1];
	if(true == document.getElementById('WRR').checked)
	{
		/*wrr queue mode is selected*/
	    diffCfg('schedule', 'schedule', v.schedule[1].value);
	}
	else
	{
		/*Hq_wrr queue mode is selected*/
	    diffCfg('schedule', 'schedule', v.schedule[0].value);
	}
	diffCfg('queweight1', 'queweight1', v.queweight1.value);
	diffCfg('queweight2', 'queweight2', v.queweight2.value);
    diffCfg('queweight3', 'queweight3', v.queweight3.value);
	diffCfg('queweight4', 'queweight4', v.queweight4.value);	
	subForm(v,'goform/command','SYS_QOSWEIGHT','qos.asp');	
}

</script>
</HEAD>

<BODY  leftMargin=0 topMargin=0 MARGINWIDTH=0 MARGINHEIGHT=0 BORDER=0 onload="loadConfiguration();">
<script>pageHead(env, env['web.page']);</script>

<table cellSpacing=0 cellPadding=2 border=0 width=100%>
<TR><td class=tb_head colspan=2><div class=tb_head>QoS</div></td></TR>
</tr>
<tr>

<td>
<TABLE cellSpacing=0 cellPadding=0 border=0 width=100%>

    <FORM name="qosselect" action="goform/command" method="post">
    <tr>
    <td class="tb_head" colspan="10">&nbsp;优先级类型选择</td>
    </tr>
    <tr>
    <td colspan="10" class=textCell>
      <SCRIPT language=JavaScript>
          document.write('<select name="qosmode" id="qosmode" style="WIDTH: 100px" onChange="qosModify()"');
          if (1 != admin)
          {
              document.write(' disabled');
          }
		  document.write('>');
      </SCRIPT>
      <option value=0>COS</option>
      <option value=1>DSCP</option>
      </select>
    </td></tr>	
    <TR><TD height=12></TD></TR>
    </FORM>
    <FORM name="qosschedule" method="POST" action="goform/command">
    <tr>
    <td class="tb_head" colspan="10">&nbsp;调度模式</td>
    </tr>
    <tr>
    <td colspan="10" class=textCell>
        <SCRIPT language=JavaScript>
        document.write('<input name="schedule" id="HQWRR" type="radio" value="1"');
	    if (1 != admin)
        {
            document.write(' disabled');
        }
		document.write('>HQ-WRR');
        document.write('<input name="schedule" id="WRR" type="radio" value="0"');
		if (1 != admin)
		{
		    document.write(' disabled');
		}
		document.write('>WRR');
        
        </SCRIPT>
    </td></tr>	
  
    <TR><TD height=12></TD></TR>	
  	
    <TR class=tb_head>
        <TD width=18%><div align="center">优先级</div></TD>
		<SCRIPT language=JavaScript>
		if (qosMode == 1)
		{
		    document.write('<TD width=18%><div align="center">0-15</div></TD>');
			document.write('<TD width=18%><div align="center">16-31</div></TD>');
			document.write('<TD width=18%><div align="center">32-47</div></TD>');
			document.write('<TD width=18%><div align="center">48-63</div></TD>');
		}
		else
		{
		    document.write(' <TD width=8%><div align="center">0</div></TD>');
			document.write('<TD width=8%><div align="center">1</div></TD>');
			document.write('<TD width=8%><div align="center">2</div></TD>');
			document.write('<TD width=8%><div align="center">3</div></TD>');
			document.write('<TD width=8%><div align="center">4</div></TD>');
			document.write('<TD width=8%><div align="center">5</div></TD>');
			document.write('<TD width=8%><div align="center">6</div></TD>');
            document.write('<TD width=8%><div align="center">7</div></TD>');
		}
		</SCRIPT>
        <TD width=18%><div align="center">权重</div></TD>
    </TR>
    <TR><TD height=8></TD></TR>

<script type="text/javascript">  

var qos, tag, nmtag, nmwrr,queue;
    	
for (qos = 0; qos < numQOS; qos++) 
{
    queue = qos + 1;
    document.write('<tr class="textCell">');
    document.write('<td><div align="center">队列' + queue + '(');
    if(queue == 4)    
        document.write('最高');
    if(queue == 3)    
        document.write('较高');
    if(queue == 2)    
        document.write('普通');
    if(queue == 1)    
        document.write('最低');
    document.write(')<\/div><\/td>');
    for (tag = 0; tag < numTags; tag++) {
    	
    	nmtag = tag + 20;     
      document.write('<td align="center">');

      document.write('<input type=radio value=' + qos + 
        ' name="R' + (nmtag<10 ? "0" + nmtag:nmtag) + '" ' + tagMapping(qos, tag) + ' disabled><\/td>');
    }
    nmwrr = qos+1;
    document.write('<td align="center"><select name="queweight'+ nmwrr +'" style="WIDTH: 60px"');
	if (1 != admin)
	{
	    document.write(' disabled');
	}
	document.write('>');
    for (var i=1; i<=weightNum; i++)
    {
        document.write('<option value='+i+'>'+i+'<\/option>');
    }
    document.write('<\/select><\/td>');
    document.write('<\/tr>');
}
</script>
<TD><input type="hidden" value="Apply" name="Submit"></TD>
<TR><TD height=8></TD></TR>
</FORM>
<TR>
  <TD class=noteCell colspan=10>	  
	<HR>
	<b>说明:</b><br>
	<script type="text/javascript">  
	if(qosMode == 1)
	{
	    document.write("1  DSCP的64个优先级被分为4组，每组16个优先级并对应一个调度队列，优先级和调度队列的对应关系：");
	    document.write("优先级0-15对应队列1、优先级16-31对应队列2、优先级32-47对应队列3、优先级48-63对应队列4。<BR>");
	}
	else
	{
	    document.write("1  COS的8个优先级被分为4组，每组2个优先级并对应一个调度队列，优先级和调度队列的对应关系：");
	    document.write("优先级0-1对应队列1、优先级2-3对应队列2、优先级4-5对应队列3、优先级6-7对应队列4。<BR>");
	}
	document.write("2  交换机的4个调度队列都可以配置权重，权重分为31个级别。");
	</script>
	<p>
	<script type="text/javascript"> 
    if(1 == admin)
    {
        document.write('<input class="button" type=button value=" 确  认 " onclick="apply2();">&nbsp;&nbsp;&nbsp;&nbsp;');
        document.write('<input class="button" type=button value=" 取  消 " onclick=GURL(\'qos.asp\') >');
	}
	</script>
    </p>
  </TD>
  </TR>
 </TABLE>
</td>
<!--
<td width=100 align=center valign=top>
<p>&nbsp;</p>

<script type="text/javascript"> 
if(1 == admin)
{
    document.write('<p><INPUT class=actButton name="_Submit" id="_Submit" type="button" value="确 定" onclick="configSubmit();"><\/p>');
    document.write('<p><input name=button2 type=button class=actButton onClick=GURL(\'qos.htm\') value="取 消"><\/p>');
}
</script>
</td>
-->
</tr>
</table>
<script>pageTail();</script>
</BODY></HTML>

