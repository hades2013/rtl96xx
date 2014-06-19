/* 
	Copyright 2013-2020, 
	Shenzhen Hexicom Technologies Co., Ltd. 
	All rights reserved.
	Author: Einsn Liu (liuchuansen@hexicomtech.com)
	Date: 2013-03-25 
*/

function utf8_charlen(utf8_str, index)
{
	var code = utf8_str.charCodeAt(index);
	if (code < 0x080){
		return 1;
	}else if (code < 0x0800){
		return 2;	
	} else {
		return 3;	
	}
}


function utf8_strlen(utf8_str)
{
	var cnt = 0;
	for (i = 0; i < utf8_str.length; i ++){
		var val = utf8_str.charCodeAt(i);
		if (val < 0x080){
			cnt += 1;	
		}	else if (val < 0x0800){
			cnt += 2;	
		} else {
			cnt += 3;	
		}
	}
	return cnt;	
}

function uft8_string_show(utf8_str, row_size)
{
	var i, line = '', line_cnt = 0, line_out = '', char_len;
	
	for (i = 0; i < utf8_str.length; i ++){
			char_len = (utf8_charlen(utf8_str, i) > 1) ? 2 : 1;
			if (line_cnt + char_len <= row_size){
				line += utf8_str.charAt(i);	
				line_cnt += char_len;
			}else {
				line_out += line + '</br>';
				line = utf8_str.charAt(i);
				line_cnt = char_len;		
			}
	}
	if (line_cnt){
		line_out += line;
	}
	return line_out;	
}

function uft8_string_show_brief(utf8_str, row_size)
{
	var i, len = 0, line = '', line_cnt = 0, char_len;		
	for (i = 0; i < utf8_str.length; i ++){
		len += (utf8_charlen(utf8_str, i) > 1) ? 2 : 1; 
	}
	if (len > row_size){
	  for (i = 0; i < utf8_str.length; i ++){
			  char_len = (utf8_charlen(utf8_str, i) > 1) ? 2 : 1;
			  if (line_cnt + char_len <= row_size){
				  line += utf8_str.charAt(i);	
				  line_cnt += char_len;
			  }else {
					break;
			  }
	  }	
	  return line + '...';	
	}else {
		return utf8_str;
	}
}

 
 
function valid_xname(n) {
	if (n.indexOf(";") >=0 || n.indexOf('"') >=0 || n.indexOf("'") >=0 || n.indexOf("\\") >=0 ) {
		return false;
	}
	var t = /^\s*$/;
	if (t.test(n)){
			return false;	
	}	
	return true;
}

function valid_description(n) {
	if (n.indexOf(";") >=0 || n.indexOf('"') >=0 || n.indexOf("'") >=0 || n.indexOf("\\") >=0 ) {
		return false;
	}
	return true;
}


function valid_xname(n, l) {

	var len = utf8_strlen(n);
        if (len > l){
          return false;	
        }
	 
	if (n.indexOf(";") >=0 || n.indexOf('"') >=0 || n.indexOf("'") >=0 || n.indexOf("\\") >=0 ) {
		return false;
	}
	var t = /^\s*$/;
	if (t.test(n)){
			return false;	
	}	
	return true;
}

function valid_description(n, l) {
	
	var len = utf8_strlen(n);
        if (len > l){
          return false;	
        }	
	
	if (n.indexOf(";") >=0 || n.indexOf('"') >=0 || n.indexOf("'") >=0 || n.indexOf("\\") >=0 ) {
		return false;
	}
	return true;
}


function valid_host(host_name) {
	var reg =/^([a-zA-Z])/;
	var reg1 = /[\u4E00-\u9FA5]|[\uFE30-\uFFA0]/i;
	if (!reg.test(host_name)) 
	{
		return -1;
	} 

if (reg1.test(host_name) || host_name.indexOf(" ") > 0 || host_name.indexOf(";") >0 
	|| host_name.indexOf("'") >0 || host_name.indexOf("\\") >0 || host_name.indexOf(")") >0 
	|| host_name.indexOf("(") >0|| host_name.indexOf("|") >0|| host_name.indexOf("`") >0 
	|| host_name.indexOf("~") >0 || host_name.indexOf("&") >0|| host_name.indexOf("<") >0
	|| host_name.indexOf(">") >0 )
	{
		return -2;
	}
	return true;
}

/* valid hardware addresss
 * args : hwaddr_name: hwaddr object name.
 * Return :  if success return true else return false.
 */
function valid_hwaddr(mac) {
	
  var re = /^[0-9a-fA-F]{2}[:-][0-9a-fA-F]{2}[:-][0-9a-fA-F]{2}[:-][0-9a-fA-F]{2}[:-][0-9a-fA-F]{2}[:-][0-9a-fA-F]{2}$/;

  if (!re.test(mac))
  {
      return false;
  }
	if (mac == "FF:FF:FF:FF:FF:FF")
		return false;
	if (mac == "00:00:00:00:00:00")
		return false;
  var vs=mac.split(':');
  if (parseInt(vs[0], 16) & 0x01){
  	return false;
  }
  
  return true;
}



function valid_username(name) {
	var reg =/^[A-Za-z0-9_]{3,12}$/;
	if(name.length <= 0) return false;	
	return reg.test(name);
}

function valid_password(name) {
  if (name.indexOf(';') >=0 || name.indexOf('"') >=0 || name.indexOf("'") >=0 || name.indexOf("\\") >=0 ) {
    	return false;
  }	
  var reg = /^([\x21-\x5d]|[\x60-\x7e]){3,12}$/;
  return reg.test(name);		
}


function password_match(pwd0, pwd1) {
	if ((pwd1.length == 0) || (pwd0.length == 0)){
		return false;	
	}
	return (pwd1 == pwd0);	
}



function isValidVlanItem(i)
{
	var mi, ma;
	var v=i.split('-');
	if (v.length == 2){
			mi = parseInt(v[0], 10);
			ma = parseInt(v[1], 10);
			if (mi >= ma) return false;
			if ((ma > 0) && (ma < 4095)) return true;
			return false;
	}else {
		mi = parseInt(i, 10);	
	}
  if ((mi > 0) && (mi < 4095)) return true;
	return false;
}

function isValidVlanString(s)
{	
/*1,2-45,100-960*/
  var vstr= /^(([1-9][0-9]{0,3}\-[1-9][0-9]{0,3}|[1-9][0-9]{0,3})\,)*([1-9][0-9]{0,3}\-[1-9][0-9]{0,3}|[1-9][0-9]{0,3})$/;   
  if (!vstr.test(s)) return false;
  // check every vlans
	var vs = s.split(',');
	for (var i = 0; i < vs.length; i ++){
		if (!isValidVlanItem(vs[i])){
				return false;
		}	
	}  
  return true;
}



function parse_vlan_item(i)
{
	var mi, ma,cnt = 0;
	var ret = new Array();
	var v=i.split('-');
	if (v.length == 2){
			mi = parseInt(v[0], 10);
			ma = parseInt(v[1], 10);
			if (mi <= ma){
				for(;mi <= ma; mi++){
					ret[cnt++] = mi;
				}
			}
	}else {
			ret[0] = parseInt(i, 10);

	}
	return ret;
}


function parse_vlan(v)
{
	var vlans=new Array();
	var cnt = 0;
	var vs = v.split(',');
	for (var i = 0; i < vs.length; i ++){
		 vi = parse_vlan_item(vs[i]);
		 for (var j=0; j < vi.length; j++){
		 		vlans[cnt++]=vi[j];
		 }
	}
	return vlans;
}



function isAnyVlanExist(vstr, vlan_ids)
{
	var vin;
	var vs=parse_vlan(vstr);
	for (var j=0; j < vs.length; j++){
		vin=parseInt(vs[j], 10);
		if (vlan_ids[vin] != null) return true;
	}	
	return false;
}


function isAnyVlanNotExist(vstr, vlan_ids)
{
	var vin;
	var vs=parse_vlan(vstr);
	for (var j=0; j < vs.length; j++){
		vin=parseInt(vs[j], 10);
		if (vlan_ids[vin] == null) return true; 
	}	
	return false;
}

function vlan_valid(input)
{
    var valid;
	var j = 0;
	
    input = parseInt(input);
    var t = /^\s*$/;
   	var re = /^[1-9][0-9]*$/;    

    if (1 == t.test (input))
    {
        alert ("VLAN ID 不能为空 ！");
        return false;
    }

    if (!re.test(input)){
    	alert("非法的VLAN ID,请输入有效数字");
    	return false;
    }
	   
    var iInput=parseInt(input, 10);
    if(iInput<1 || iInput>4094)
    {
        alert ("非法的VLAN ID,请输入值(1-4094)！");
        return false;
    }
	
	return true;
}

