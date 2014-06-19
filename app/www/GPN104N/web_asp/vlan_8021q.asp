<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="content-type" content="text/html;charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=JavaScript src=pageview.js></SCRIPT>
<script language=JavaScript>

pageTitle();

var vlan_list = new Array(<% CGI_GET_VLAN_CONFIG(); %>);
var vlan_options = new Array(<% CGI_GET_VLAN_MODE_CONFIG(); %>);
var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

var vlan_ids = new Array();

function vlan_mode()
{
	if (vlan_options.length > 0){
		return vlan_options[0];	
	}	
	return '0';
}

function vlan_mvid()
{
	if (vlan_options.length > 1){
		return vlan_options[1];	
	}	
	return '1';		
}



function update_vlan_ids()
{
	vlan_ids = new Array();	
	for(var i = 0; i < vlan_list.length; i ++) {
		var vs=parse_vlan(vlan_list[i].split(';')[2]);//.split(',');	
		for (var j=0; j < vs.length; j++){
			vlan_ids[parseInt(vs[j])]=parseInt(vs[j]);
		}
	}
}

function new_vlan()
{
	var f = document.forms[0];
    var input = f.vid.value;
    var t = /^\s*$/;

    if (1 == t.test (input))
    {
        alert ("请输入VLAN ID字串！");
        f.vid.focus();
        return;
    }

    if(!isValidVlanString(input))
    {
        alert ("VLAN ID字串包含非法字符！");
        return;
    }
 
 if(isAnyVlanExist(input, vlan_ids))
 {
    if (!confirm("一些VLAN已存在VLAN列表,确定要继续吗?"))return;	
 }

   if(!f.eth0.checked && !f.eth1.checked) {
    alert("请至少选择一个端口!");
    return;
   }

  var mems = 'cab0,cab1';
 if(f.eth0.checked) mems += ',eth0';
 if(f.eth1.checked) mems += ',eth1';
 diffCfg('vlan_0', 'vlan_add', '0;'+mems + ';' + input);
 subForm(f, 'goform/command', 'VLAN_NEW', 'vlan_8021q.asp');
}

function delete_edit()
{
	var f = document.forms[0];
    var input = f.vid.value;
    var t = /^\s*$/;
    
    if (1 == t.test (input))
    {
        alert ("请输入VLAN ID字串！");
        f.vid.focus();
        return;
    }

    if(!isValidVlanString(input))
    {
        alert ("VLAN ID字串包含非法字符！");
        return;
     }
 
     if(isAnyVlanNotExist(input, vlan_ids))
     {
 			alert("一些VLAN已存在VLAN列表,确定要继续吗?");
 			return;	
    }
    
		var vin = new Array();
		var vs=parse_vlan(input);
		for (var j=0; j < vs.length; j++){
			vin[parseInt(vs[j])]=parseInt(vs[j]);
		}	

		 if(isAnyVlanExist("1", vin))
		 {
 			alert("VLAN 1 是默认VLAN,不能被删除!");
 			return;			 	
		 }
		 if(isAnyVlanExist(vlan_mvid(), vin))
		 {
 			if (!confirm("VLAN " + vlan_mvid() + " 是管理VLAN,它被删除后,管理VLAN将自动恢复到默认配置?")) return;			 	
		 }		 
     
    var mems = 'cab0,cab1';  		
    if(f.eth0.checked) mems += ',eth0';
    if(f.eth1.checked) mems += ',eth1';
    diffCfg('vlan_0', 'vlan_del', '0;'+mems + ';' + input);
    subForm(f, 'goform/command', 'VLAN_DEL', 'vlan_8021q.asp');
}

function delete_vlan()
{
	for(var i = 0; i < vlan_list.length; i ++){ 
		if(pageview_ischecked(i)) {
			diffCfg('vlan_' + i, 'vlan_del', vlan_list[i]);
		}
  }
	subForm(document.forms[0],'goform/command', 'VLAN_DEL', 'vlan_8021q.asp');
}

function edit_vlan(s)
{
	var f = document.forms[0];
	for(var i = 0; i < vlan_list.length; i ++) 
		if(pageview_ischecked(i)) {
			f.vid.value = vlan_list[i].split(';')[2];	
			var vs =  vlan_list[i].split(';')[1].split(',');
			f.eth0.checked = false;
			f.eth1.checked = false;	
			for(var j = 0; j < vs.length; j ++) {
				if(vs[j] == 'eth0')f.eth0.checked = true;
				if(vs[j] == 'eth1')f.eth1.checked = true;
    }
    }
            }


function vlan_group(s)
{
	if (s == '') return " ";
	var vs = s.split(',');
	var vstr = '';
	for(var i = 0; i < vs.length; i ++) {
		vstr += port_name(vs[i]);
		if(i != vs.length - 1) vstr += ', ';
  }
	return  vstr;
}


function checkbox_click(item)
{
	var cnt = 0;
	for(var i = 0; i < vlan_list.length; i ++) {
		if(pageview_ischecked(i)){
			cnt ++;	
    }
	}
	document.getElementById('btn_del').disabled = !(cnt > 0);
	document.getElementById('btn_edit').disabled = !(cnt == 1);
}

function vmchange()
{
	if(vlan_mode() == '0') {
		document.getElementById('vlan_1q').style.display = 'block';
		document.getElementById('desc').style.display = 'none';
	}
	else {
		document.getElementById('vlan_1q').style.display = 'none';
		document.getElementById('desc').style.display = 'block';
	}		
}

function init()
{
	pageview_add('VLAN ID', 2, '55%', true, null);
	pageview_add('端口成员', 1, '40%', true, vlan_group);
	pageview_init(vlan_list, 10, document.getElementById('vlan_list'), true, checkbox_click, null, 'VLAN列表');
	vmchange();
	update_vlan_ids();
}
</script>
</head>

<body onload=init()>
<script>pageHead(webVars);</script>

<form name=vlan_8021q action=goform/command method=post>
<INPUT type=hidden name=CMD value=WAN_CON>
<INPUT type=hidden name=GO value=vlan_8021q.asp>
<INPUT type=hidden name=action>

<div id="desc">系统VLAN在透传模式 ,
请点击<a href="vlan_mode.asp">更改VLAN模式</a>配置802.1QVLAN</div>

<div id="vlan_1q">
<table width=100% border=0 align=center cellpadding=5 cellspacing=1	class=box_tn>
	<tr><td colspan=2 class=titleCell>添加或修改VLAN</td></tr>
	<tr><td width=20% class=bgblue>VLAN ID</td>
		<td width=80% class=bggrey><input name="vid" size=40 maxlength=64></td>
	</tr>
	<tr><td width=20%   class=bgblue>VLAN成员</td>
	<td width=80% class=bggrey>
		<input type="checkbox" name="eth0" value ="eth0"> Ethernet1
		<input type="checkbox" name="eth1" value ="eth1"> Ethernet2
	</td></tr>
	<tr><td colspan=2 align=center bgcolor="#9EA2A5">
	<table>
	<tr>
	<td width=30 valign=top>
		<input type=button name="btn_new" value=" 创建 "  onclick="new_vlan();"></td>
	<td width=30 valign=top>
		<input type=button name="btn_deledit" value=" 删除 "  onclick="delete_edit();"></td>
	</tr>
	</table>
	</td></tr>
</table>
<br>
<div id="vlan_list"></div>
<p>
</p>
<table align=center >
	<tr>
	<td width=30 valign=top>
		<INPUT id = "btn_del" type=button value="删除所选VLAN" onclick="delete_vlan();" disabled=true>
	</td>
	<td></td> 
	<td width=30 valign=top>
		<INPUT id = "btn_edit" type=button value="编辑所选VLAN" onclick="edit_vlan();" disabled=true>
	</td>
	</tr>
</table>
</div>
</form>
<script>pageTail();</script>
</body>
</html>
