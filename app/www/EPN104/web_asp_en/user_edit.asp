<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="content-type" content="text/html;charset=utf-8">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<SCRIPT language=JavaScript src=utils.js></SCRIPT>
<SCRIPT language=JavaScript src=spec.js></SCRIPT>
<SCRIPT language=javascript src=pageview.js></script>
<SCRIPT language=javascript src=valid.js></script>
<script language=JavaScript>
pageTitle();

var webVars = new Array( <% CGI_GET_WEB_CONST(); %>);

var supdev = new Array(<% CGI_GET_DEVINFO(); %>);
var tmpl_list = new Array(<% CGI_GET_TEMPLATES("brief"); %>);
var client_list = new Array(<% CGI_GET_USER_CONFIG(); %>);

var user_id = 0; 
//addCfg('user_' + user_id, 'user_' + user_id, client_desc);
var action;
var user_mac = '';
var dev_id = '';

function dev_string(index)
{
	var str = supdev[index].split(';')[0];
	return str;
}

function mac_exist(mac)
{
		for(var i = 0; i < client_list.length; i ++) {
			if (mac == client_list[i].split(';')[0]){
					return true;
				}
		}	
		return false;
}

function get_user_desc(mac)
{
		for(var i = 0; i < client_list.length; i ++) {
			if (mac == client_list[i].split(';')[0]){
					return client_list[i];
				}
		}	
		return ';0;0;acc;;;'	
}


function dev_change()
{
	var f = document.forms[0];
	var str = '';
	var model_id;
	if(dev_id == '') 
		model_id = f.DEV.value;
	else
		model_id = dev_id;
	str = '<select name="TMPL">';
	for(var i = 0; i < tmpl_list.length; i ++) {
		if(tmpl_list[i].split(';')[1] == model_id) {
			str += '<option value=' + i + '>' +
				tmpl_list[i].split(';')[3] + '(' + 
				tmpl_list[i].split(';')[2] + ')</option>';
		}
	}
	str += '</select>';
	document.getElementById('config_tmpl').innerHTML = str;
}

function show_device(dev_id)
{
	var str = '';
	if( (dev_id == '') || (action == 'edit') ) {
		str = '<select name="DEV" onchange="dev_change();">';
		for(var i = 0; i < supdev.length; i ++) {
			str += '<option value=' + i + '>' +
				dev_string(i) + '</option>';
		}
		str += '</select>';
	}
	else 
		str = '<input name="DEV" readonly="true" size=10 maxlength=10 value="' + 
			dev_string(dev_id) + '">';
	document.getElementById('dev_model').innerHTML = str;
}
	
function apply()
{
	var substr = '';
	var dev_id_str = '';
	var f = document.forms[0];
	if(dev_id == '') 
		dev_id_str = f.DEV.value;
	else
		dev_id_str = dev_id;
	if(action == 'new') {
		user_mac = f.MAC.value.toUpperCase();
		if(!valid_hwaddr(user_mac)) {
			alert('Invalid MAC address, please input again.');
			f.MAC.focus(); 
			return;
		}
		if(mac_exist(user_mac)){
			alert('MAC address has already existed in user list, please input again.');
			f.MAC.focus(); 
			return;				
		}		
	}
	if(!valid_xname(f.ALIAS.value)) {
		alert('Invalid user name, please input again.');
		return;
	}
	if(!valid_description(f.DESC.value)) {
		alert('Invalid user description. please input again.');
		return;
	}
	substr = 
		user_mac + ';' +
		f.TMPL.value + ';' +
		dev_id_str + ';' + 
		f.ACC.value + ';' +
		f.ALIAS.value + ';' + 
		f.DESC.value;
	diffCfg('user_' + user_id, 'user_' + user_id,  substr);
	subForm(f,'goform/command', (action == 'new') ? 'USER_NEW' : 'USER_PROP', 'user_list.asp');
}

function init()
{
	var f = document.forms[0];
	action = QueryString('action');
	user_mac = QueryString('user_mac');	
	dev_id = QueryString('dev_id');
 client_desc = get_user_desc(user_mac);		
	if(action == 'new') {
		document.title = 'Create User';
		document.getElementById('page_descript').innerHTML ='Add a new user';
		document.getElementById('page_header').innerHTML ='Create User';
		if(user_mac != '') {
			f.MAC.value = user_mac;
			f.MAC.readOnly = true;
		}
		show_device(dev_id);
		dev_change();
	}
	else {	
		document.title = 'Edit User';
		document.getElementById('page_descript').innerHTML ='Change user settings';
		document.getElementById('page_header').innerHTML ='Edit User';
		f.MAC.value = user_mac;
		f.MAC.readOnly = true;
		show_device(dev_id);
		f.DEV.value = client_desc.split(';')[2];
		dev_change();
		f.TMPL.value = client_desc.split(';')[1];
		f.ALIAS.value = client_desc.split(';')[4];
		f.DESC.value = client_desc.split(';')[5];
		f.ACC.value = client_desc.split(';')[3];
	}
}

</script>
</head>
<body onload=init()>
<script>pageHead(webVars);</script>

<form name=system_status action=goform/command method=post>
<table width=100% border=0 align=center cellpadding=5 cellspacing=1	class=box_tn>
	<tr>
	<td width=30% class=bgblue>MAC Address</td>
	<td width=70% class=bggrey><input name=MAC size=25 maxlength=31></td>
	</tr>
	<tr>
	<td width=30% class=bgblue>Name</td>
	<td width=70% class=bggrey><input name=ALIAS size=25 maxlength=31></td>
	</tr>
	<tr>
	<td width=30% class=bgblue>Description</td>
	<td width=70% class=bggrey><input name=DESC size=31 maxlength=31></td>
	</tr>
	<tr>
	<td width=30% class=bgblue>Device Model</td>
	<td width=70% class=bggrey>
		<div id="dev_model"></div>
	</td>
	</tr>
	<tr>
	<td width=30% class=bgblue>Template</td>
	<td width=70% class=bggrey><div id="config_tmpl"></div>
	</td>
	</tr>
	<tr>
	<td width=30% class=bgblue>Access Control</td>
	<td width=70% class=bggrey>
		<select name=ACC>
			<option value=acc>Authorised (Whitelist)</option>
			<option value=blk>Forbidden (Blacklist)</option>
		</select>
	</td>
	</tr>
</table>
<br></br>
<table width=100% border=0 align=center cellpadding=0 cellspacing=1>
	<tr><td>
	<input id="btn_wht" type=button value="  OK  " onclick="apply();">&nbsp;&nbsp;&nbsp;&nbsp;
	<input id="btn_blk" type=button value="Cancel" onclick="init();">
	</td></tr>
</table>

</form>

<script>pageTail();</script>
</body>
</html>
