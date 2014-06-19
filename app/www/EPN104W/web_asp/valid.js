/* valid host name
 * args : host_name: host name
 * Return :  if success return true else return false.
 */
function valid_xname(n) {
	if (n.indexOf(" ") > 0 || n.indexOf(";") >0 || n.indexOf('"') >0 || n.indexOf("'") >0 || n.indexOf("\\") >0 ) {
		return false;
	}
	return true;
}

function valid_description(n) {
	if (n.indexOf(";") >0 || n.indexOf('"') >0 || n.indexOf("'") >0 || n.indexOf("\\") >0 ) {
		return false;
	}
	return true;
}
/*
function valid_host(host_name) {
	var reg =/^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z]|[A-Za-z][A-Za-z0-9\-]*[A-Za-z0-9])$/;
	if (!reg.test(host_name)) {
		return false;
	} 
	
	return true;
}
*/

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


/* combine into a hardware address string.
 * args : hwaddr_name :hwaddr object name
		  split: ":" or "-"
 * Return :  string ("00:01:02:03:04:05")
 */

function combine_hwaddr(hwaddr_name, split) {
	var obj;
	var mac = "";
	var empty = "";
	var tmp;
	for (i=0;i<6;i++) {
		obj = get_by_name(hwaddr_name)[i];
		obj.value = obj.value.toUpperCase();
		tmp = obj.value;
		if (tmp.length == 1)
			obj.value = '0' + obj.value;
		mac += obj.value;
		if (i<5)
			if (split)
				mac += split;
			else
				mac += ":";
	}
	if (split)
		for (i=0;i<5;i++)
			empty += split;
		
	if (mac == ":::::" || mac == empty)
		mac = "";
	return mac;
}

/* combine into a IP address string.
 * args : ipaddr_name :ipaddr object name
 * Return :  string ("192.168.1.1")
 */
function combine_ipaddr(ipaddr_name) {
	var obj;
	var ip = "";
	for (i=0;i<4;i++) {
		obj = get_by_name(ipaddr_name)[i];
		ip += obj.value;
		if (i<3)
			ip += ".";
	}
	if (ip == "...") 
		ip = "";
	return ip;
}

/* valid hardware addresss
 * args : hwaddr_name: hwaddr object name.
 * Return :  if success return true else return false.
 */
function valid_hwaddr(mac) {
	if (mac == "") {
		return false;
	}
	if (mac == "FF:FF:FF:FF:FF:FF")
		return false;
	if (mac == "00:00:00:00:00:00")
		return false;
	
    var re = /^[0-9a-fA-F]{2}[:-][0-9a-fA-F]{2}[:-][0-9a-fA-F]{2}[:-][0-9a-fA-F]{2}[:-][0-9a-fA-F]{2}[:-][0-9a-fA-F]{2}$/;
	
    if (re.test(mac))
    {
        return true;
    }
    return false;
}

/* valid netmask
 * args : netmask_name: netmask object name.
 * Return :  if success return true else return false.
 */
function valid_netmask(netmask_name)
{
	var obj = get_by_name(netmask_name);
	if (obj.length != 4) {
		return false;
	}
	var msk = combine_ipaddr(netmask_name);

    var re = /(254|252|248|240|224|192|128)\.0\.0\.0|255\.(254|252|248|240|224|192|128|0)\.0\.0|255\.255\.(254|252|248|240|224|192|128|0)\.0|255\.255\.255\.(254|252|248|240|224|192|128|0)/;
    if (!re.test(msk))
    {
        return false;
    }
    return true;
}

/* valid ipaddr
 * args : netmask_name: netmask object name.
 * Return : if success return true else return false.
 */
function valid_ipaddr_c(ipaddr_name) {
	var obj = get_by_name(ipaddr_name);
	if (!valid_range(obj[0], 1, 223)) {
		obj[0].focus();
		return false;
	}
	if (!valid_range(obj[1], 0, 255)) {
		obj[1].focus();
		return false;
	}
	if (!valid_range(obj[2], 0, 255)) {
		obj[2].focus();
		return false;
	}
	if (!valid_range(obj[3], 1, 254)) {
		obj[3].focus();
		return false;
	}
	return true;
}

/* valid ipaddr
 * args : netmask_name: netmask object name.
 * Return : if success return true else return false.
 */
function valid_ipaddr(ipaddr_name) {
	var obj = get_by_name(ipaddr_name);
	for (i=0;i<4;i++) {
		if (!valid_range(obj[i], 0, 255)) {
			obj[i].focus();
			return false;
		}
	}
	return true;
}

function valid_ipaddr_wan(ipaddr, netmask) {
	var ips1 = ipaddr.split('.');
	var ipv1 = (parseInt(ips1[0],10)<<24)|(parseInt(ips1[1],10)<<16)|(parseInt(ips1[2])<<8)|parseInt(ips1[3]);
 	var m1 = netmask.split('.');
	var msk1 = (parseInt(m1[0],10)<<24)|(parseInt(m1[1],10)<<16)|(parseInt(m1[2],10)<<8)|parseInt(m1[3],10);
	if ((ipv1 == 0) || (ipv1 == -1)) {
		return false;
	}
	if (((ipv1 | msk1) == -1) || (ipv1 | msk1) == msk1) 
		return false; 
	else 
		return true;

}

/* valid password
 * args : password
 * Return : if success return true else return false.
 */
function valid_password(password){
//	var reg =/^[a-zA-Z0-9.@_]*$/;
	var reg =/^([\x20-\x21]|[\x23-\x5b]|[\x5d-\x7e])*$/;
	if (!reg.test(password)) {
		return false;
	}
	return true;
}

/* verify if two password are the same
 * args : pwd1, pwd2 :two password;
 * Return : if they are the same return true else return false.
 */
function valid_password_verify(pwd1, pwd2){
	if (pwd1 != pwd2){
		return false;
	}
	return true;
}

/* valid ascii 
 * args : string 
 * Return : if the string is ascii then return true else return false.
 */
function valid_ascii(str) {
	var reg =/^[\x21-\x7e]*$/;
	if (!reg.test(str)) {
		return false;
	}
	return true;
}

/* valid if the str is digital
 * args : 
 *		obj_name:name of object
 * Return : if the string is digital then return true else return false.
 */
function valid_digit(obj_name) {
	var reg =/^[0-9]{1,99}$/;
	if (!reg.test(obj_name)) {
		return false;
	}
	obj_name.value = parseInt(obj_name.value, 10).toString();
	return true;
}

/* If the str is digital and str is between the start and the end 
 * args :obj_name : name of object 
 *		 start:
 *		 end:
 *		 msg:alert message 
 * Return : if the string is between the start and the end then return true else return false.
 */
function valid_range(obj_name, start,end, msg) {
	if (!valid_digit(obj_name)) {
		if (msg)
			alert(msg);
		return false;
	}
	d = parseInt(obj_name, 10);
	if ( !(d<=end && d>=start) ) {
		if (msg)
			alert(msg);
		return false;
	}
	return true;
}

/* valid two ipaddrs are in the same network
 * args :ip1:
 *		 ip2:
 *		 mask1: netmask 
 *		 mask2: netmask 
 * Return : if they are in the same network then return true else return false.
 */
function valid_same_network(ip1, mask1, ip2, mask2)
{
	var ips1 = ip1.split('.');
	var ips2 = ip2.split('.');
	var m1 = mask1.split('.');
	var m2 = mask2.split('.');
	var ipv1 = (parseInt(ips1[0],10)<<24)|(parseInt(ips1[1],10)<<16)|(parseInt(ips1[2])<<8)|parseInt(ips1[3]);
	var ipv2 = (parseInt(ips2[0],10)<<24)|(parseInt(ips2[1],10)<<16)|(parseInt(ips2[2])<<8)|parseInt(ips2[3]);
	var msk1 = (parseInt(m1[0],10)<<24)|(parseInt(m1[1],10)<<16)|(parseInt(m1[2],10)<<8)|parseInt(m1[3],10);
	var msk2 = (parseInt(m2[0],10)<<24)|(parseInt(m2[1],10)<<16)|(parseInt(m2[2],10)<<8)|parseInt(m2[3],10);
	if((ipv1 & msk1) == (ipv2 & msk2)) 
		return true; 
	else 
		return false;
}

/* valid port
 * args :port
 * Return : if success then return true else return false.
 */
function valid_port(port){
    var temp_port = port.split(" ");

    if (isNaN(port) || port == "" || temp_port.length > 1
    		|| (parseInt(port) < 0 || parseInt(port) > 65535) || port.indexOf(".")>=0 || port.indexOf("-")>=0) {
        return false;
    }
    return true;
}

/* valid url
 * args :url
 * Return : if success then return true else return false.
 */
function valid_url(url) {
	var reg =/^([\x20-\x21]|[\x23-\x5b]|[\x5d-\x7e])*$/;
	if (!reg.test(url)) {
		return false;
	}
	return true;
}

/* valid name
 * args :name
 * Return : if success then return true else return false.
 */
function valid_name(name) {
	var reg =/^([\x20-\x21]|[\x23-\x5b]|[\x5d-\x7e])*$/;
	if (!reg.test(name)) {
		return false;
	} 
	return true;
}

function valid_admin(name) {
	var reg =/^([\x21]|[\x23-\x5b]|[\x5d-\x7e])*$/;
	if (!reg.test(name)) {
		return false;
	} 
	if(name.length <= 0) return false;
	return true;
}

function valid_username(name) {
	var reg =/^[A-Za-z0-9_]{3,12}$/;
	if(name.length <= 0) return false;	
	return reg.test(name);
}

function valid_password(name) {
	return valid_username(name);
}


function password_match(pwd0, pwd1) {
	if ((pwd1.length == 0) || (pwd0.length == 0)){
		return false;	
	}
	return (pwd1 == pwd0);	
}

function valid_computer_name(computer_name) {
	var reg = /^[a-zA-Z0-9\-]{1,20}$/;
	if (!reg.test(computer_name)) {
		return false;
	}
	if (valid_digit(computer_name)) {
		return false;
	}
	return true;
	
}

/* valid wep key
 * args :key
 *		 wbit: "64" and "128"
 * Return : if success then return true else return false.
 */
function valid_wep(key, wbit) {
	if (wbit == "64") {
		if (key.length == 10) {
			var m = /[0-9a-fA-F]{10}/;
			if (!m.test(key)) {
				alert(WLAN.WEP_KEY_ERROR + WLAN.WEP_KEY_ERROR2);
				return false;
			}
			else {
				return true;
			}
		}
		else if (key.length != 5) {
			alert(WLAN.WEP_KEY_ERROR + WLAN.WEP_KEY_ERROR3);
			return false;
		}
		else {
			if (!valid_password(key)) {
				alert(WLAN.WEP_KEY + SHARE.INVALID);
				return false;
			}
			else {
				return true;
			}
		}
	}
	else {
		if (key.length == 26) {
			var m = /[0-9a-fA-F]{26}/;
			if (!m.test(key)) {
				alert(WLAN.WEP_KEY_ERROR + WLAN.WEP_KEY_ERROR2);
				return false;
			}
			else {
				return true;
			}
		}
		else if (key.length != 13) {
			alert(WLAN.WEP_KEY_ERROR + WLAN.WEP_KEY_ERROR4);
			return false;
		}
		else {
			if (!valid_password(key)) {
				alert(WLAN.WEP_KEY + SHARE.INVALID);
				return false;
			}
			else {
				return true;
			}
		}
	}
	return false;
}

/* valid wpa key
 * args :key
 * Return : if success then return true else return false.
 */
function valid_wpa(key) {
	if (key.length == 64) {
		var m = /[0-9a-fA-F]{64}/;
		if (!m.test(key)) {
			alert(WLAN.WPA_KEY_ERROR);
			return false;
		}
	}
	else if (key.length >=8 && key.length <=63) {
		if (!valid_password(key)) {
			alert(WLAN.WPA_KEY + SHARE.INVALID);
			return false;
		}
	}
	else {
		alert(WLAN.WPA_KEY_ERROR2);
		return false;
	}
	return true;
}

/* valid wps pin
 * args :pin
 * Return : if success then return true else return false.
 */
function valid_wps_pin(pin)
{
	var i, c, csum = 0;

	if (pin.length != 8) return false;
	for (i=0; i < pin.length; i++)
	{
		c = pin.charAt(i);
		if (c > '9' || c < '0') return false;
		csum += parseInt(c,[10]) * (((i%2)==0) ? 3:1);
	}
	return ((csum % 10)==0) ? true : false;
}

function valid_ssid(ssid) {
	var reg =/^([\x20-\x21]|[\x23-\x7e])*$/;
	if (!reg.test(ssid)) {
		return false;
	} 
	return true;
}

function MAC_Check(I)
{	
	var ck= /([0-9a-fA-F]{2}\:){5}[0-9a-fA-F]{2}/;
	msg="非法的MAC地址!";
	if(I.value=="") return 1;
	if (!ck.test(I.value))
	{
		alert("请输入 xx:xx:xx:xx:xx:xx 格式的合法MAC地址!");
		return 1;
	}

	tt=I.value.split(":");

	if ((parseInt(tt[0],16)&1)==1)
	{
		alert(msg);
		return 1;
	}

	if((parseInt(tt[0],16)==255) &&(parseInt(tt[1],16)==255) &&(parseInt(tt[2],16)==255) &&(parseInt(tt[3],16)==255)&&(parseInt(tt[4],16)==255)&&(parseInt(tt[5],16)==255))
	{
		alert(msg);
		return 1;
	}

	if (I.value=="00:00:00:00:00:00")
	{
		alert(msg);
		return 1;
	}
	return 0;
}


function number_check(I)
{
	var ck = /[^0-9]{1,}/;
	
      if(I.value=="") 
            return 1;

	if(ck.test(I.value))
	{
		alert("非法字符,请重新输入!");
             //I.value="";
		return 2;
	}

	var realvalue = "";
	var ch = "", flag = 0;
	for (var count=0; count<I.value.length; count++)
	{
		ch = I.value.charAt(count);
		if (ch=="0" && flag==0) 
		{
			continue;
		}
		else
		{
			realvalue += ch;
			flag = 1;
		}
	}
	if (flag == 0)
	{
		realvalue += "0";
	} 
	I.value = realvalue;
	return 0;
}



function chinese_check(I)
{
	var ck=/[^\x00-\xff]/;
		if(I.value=="") return 1;
	if(ck.test(I.value))
	{
		alert("请不要输入 \\ \' \" < > ; & `这些字符以及中文字符和空格!");
		return 1;
	}
	
	if (specilachars_check(I) == 1)
	{
		return 1;
	}
	return 0;
}

function chinese_check2(I)
{
	var ck=/[^\x00-\xff]/;
		if(I.value=="") return 1;
	if(ck.test(I.value))
	{
        alert("请不要输入 \\ \" ;这些字符以及中文字符和空格!");
		//I.value="";
		return 1;
	}
	
	if (specilachars_check2(I) == 1)
	{
		return 1;
	}
	return 0;
}

function chinese_check3(I)
{
	var ck=/[^\x00-\xff]/;
		if(I.value=="") return 1;
	if(ck.test(I.value))
	{
        alert("请不要输入 \\ \" ;这些字符以及中文字符!");
		return 1;
	}
	
	if (specilachars_check3(I) == 1)
	{
		return 1;
	}
	return 0;
}

/*check chinese only*/
function chinese_check4(I)
{
	var ck=/[^\x00-\xff]/;
		if(I.value=="") return 1;
	if(ck.test(I.value))
	{
        alert("请不要输入中文字符!");
		I.value="";
		return 1;
	}

	return 0;
}

/* check chinese only, and nothing to alert */
function chinese_check_only(v)
{
    var ck = /[^\x00-\xff]/;

    if (v=="" ||ck.test(v)) {
        return 1;
    }
    
    return 0;
}

function name_check_strickly(I)
{
    var ck=/[^\x00-\xff]/;
    var strlen = 0;
	var ch = "";
    
	if(I.value=="") 
	    return 1;
	if(ck.test(I.value))
	{
		alert("请输入英文、数字或- _ 字符!");
		I.value="";
		return 1;
	}

    strlen = I.value.length;
	ch="";
	for(var k=0; k<strlen; k++ )
	{
		ch = I.value.charAt(k);
		if ((ch=='\'') || (ch=='\"') || (ch==' ') || (ch=='<') || (ch=='>') 
		    || (ch==';') || (ch=='\\') ||(ch=='&') ||(ch=='`')
		    || (ch=='!') || (ch=='@') ||(ch=='#') ||(ch=='$')
		    || (ch=='^') || (ch=='*') ||(ch=='(') ||(ch=='|')
		    || (ch==')') || (ch=='+') ||(ch=='=') ||(ch==':')
		    || (ch=='{') || (ch=='}') ||(ch=='[') ||(ch==']')
		    || (ch=='~') || (ch=='?') ||(ch=='/') ||(ch==',')
		    || (ch=='.') ||(ch=='%'))
		{
			alert("请输入英文、数字或- _ 字符!");
			I.value="";
			return 1;
		}
	}
	/* added liuburong */
		return 0;
}



function specilachars_check(str)
{
	var strlen = str.value.length;
	var ch="";
	
	for(var k=0; k<strlen; k++ )
	{
		ch = str.value.charAt(k);
		if ((ch=='\'') || (ch=='\"') || (ch==' ') || (ch=='<') || (ch=='>') 
		    || (ch==';') || (ch=='\\') ||(ch=='&') ||(ch=='`'))
		{
			//alert("请不要输入中文、空格及以下字符 \\ \' \" < > ; & !");
			alert("请不要输入 \\ \' \" < > ; & `这些字符以及中文字符和空格!");
			return 1;
		}
	}
	return 0;
}

function specilachars_check2(str)
{
	var strlen = str.value.length;
	var ch="";
	
	for(var k=0; k<strlen; k++ )
	{
		ch = str.value.charAt(k);
		if ( (ch=='\"') || (ch==' ') || (ch==';') || (ch=='\\'))
		{
			//alert("请不要输入中文、空格及以下字符 \\ \' \" < > ; & !");
			alert("请不要输入 \\ \" ;这些字符以及中文字符和空格!");
			return 1;
		}
	}
	return 0;
}

function specilachars_check3(str)
{
	var strlen = str.value.length;
	var ch="";
	
	for(var k=0; k<strlen; k++ )
	{
		ch = str.value.charAt(k);
		if ( (ch=='\"')  || (ch==';') || (ch=='\\'))
		{
			alert("请不要输入 \\ \" ;这些字符以及中文字符!");
			return 1;
		}
	}
	return 0;
}



function isHex(ipField)
{
	var j;
	for (i=0;i<ipField.length;i++)
	{
	 	j = parseInt(ipField.substring(i,i+1),16);
		if ( (j == 0) || (j == 1)|| (j == 2)|| (j == 3)|| (j == 4)|| (j == 5)|| (j == 6)
		|| (j == 7)|| (j == 8)|| (j == 9)|| (j == 10)|| (j == 11)|| (j == 12)|| (j == 13)|| (j == 14)|| (j == 15) )
			continue;
		else return false;
	}
	return true;
}

function checkKEY(keyValue,maxlen)
{	
      if (chinese_check2(keyValue) == 1)
      {
           return false;
       }

	var len=keyValue.value.length;
      
        ch1 = keyValue.value.charAt(0);
        ch2 = keyValue.value.charAt(1);
        if(ch1=='0' && (ch2=='x' ||ch2=='X'))
        {
		if(false == isHex(keyValue.value.substring(2,keyValue.value.length)))						
		{				
			alert('十六进制模式下只能输入\'0-9\' 和 \'A-F\'');								
			return false;				
		}
		len-=2;
		if (len != maxlen)		
		{			
			alert('十六进制模式下KEY的 长度必须是'+maxlen);			
			return false;		
		}
		return true;			
	}	
	else	
	{	
		if (len != maxlen/2)		
		{			
			alert('ASCII 字符串模式下KEY的长度必须是 '+maxlen/2);			
			return false;		
		}
		return true;	
	}
}

function checkKEY2(keyValue,maxlen)
{	
	var len=keyValue.value.length;
      
        ch1 = keyValue.value.charAt(0);
        ch2 = keyValue.value.charAt(1);
        if(ch1=='0' && (ch2=='x' ||ch2=='X'))
        {
		if(false == isHex(keyValue.value.substring(2,keyValue.value.length)))						
		{				
			return false;				
		}
		len-=2;
		if (len != maxlen)		
		{			
			return false;		
		}
		return true;			
	}	
	else	
	{	
		if (len != maxlen/2)		
		{			
			return false;		
		}
		return true;	
	}
}


function HEX_Check10(I)
{
    var ck = /[0-9a-fA-F]{10}/;
    
    if (I.value=="")
    {
        return 1;
    }
    
    if (!ck.test(I.value))
    {
        return 1;
    }
    
    return 0;
}

function HEX_Check26(I)
{
    var ck = /[0-9a-fA-F]{26}/;  /* liuburong */
    
    if (I.value=="")
    {
        return 1;
    }
    
    if (!ck.test(I.value))
    {
        return 1;
    }
    
    return 0;
}

