

function displaylay(dis)
{
	if(dis==0)
	{
		//document.getElementById('MyScheduleIf').style.display='none';
		document.getElementById('MyScheduleDiv').style.display='none';	
	}
	else
	{
		//document.getElementById('MyScheduleIf').style.display='block';
		document.getElementById('MyScheduleDiv').style.display='block';	
	}
    return;      
}

function DisScheduleDiv()
{
	document.write('<div id="MyScheduleIf" class="MyScheduleIf" >');
	document.write('<table align=center width=100% style="font-size:20pt;">');
	document.write(' <tr><td height=50></td></tr>');
	document.write('  <tr>');
	document.write('   <td style="COLOR: #ff0000;FONT-FAMILY: verdana,arial,sans-serif;FONT-SIZE: 20px;TEXT-ALIGN: left">');
	document.write('   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;正在处理数据，请勿进行其它操作</td>');
	document.write(' </tr>');
	//document.write(' <tr>');
	//document.write('    <td align="center" id=totaltime name=totaltime style="FONT-FAMILY: verdana,arial,sans-serif;FONT-SIZE: 14px;">大约60s');
	//document.write(' </tr>');
	document.write(' </table>');
}

function DisCover()
{
	document.write('</div>');
	document.write('<div id="MyScheduleDiv" class="MyScheduleDiv" >');
	document.write('</div>');
}