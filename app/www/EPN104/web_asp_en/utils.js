function dayWriteMap(v1,v2,v3,v4,v5,v6,v7) {
   return (v1|(v2<<1)|(v3<<2)|(v4<<3)|(v5<<4)|(v6<<5)|(v7<<6));	
}

function dayReadMap(v, i) {
  return((v>>i)&1);
}

function macsCheck(I,s){
    var m = /[0-9a-fA-F\:]{17}/;
    if(I.length!=17 || !m.test(I)){
   	    alert(s+" includes invalid characters!!!");
   	    I.value = I.defaultValue;
   	    return false
    }
    return true
}

function validNumCheck(v,m) {
	var t = /[^0-9]{1,}/;
	if (t.test(v.value))
	{
		alert(m+" is not a number!!!") ;
		v.value=v.defaultValue;
		return 0;
	}
	return 1 ;
}

function rangeCheck(v,a,b,s) {

   if (!validNumCheck(v,s)) return 0;          
   if ((v.value<a)||(v.value>b)) {	
      alert(s+" overflow !!!") ;
      v.value=v.defaultValue ;
      return 0 ;
   } else return 1 ;
}

function pmapCheck(v,m){
	var t = /[^0-9,-]{1,}/;
	if (t.test(v.value)) {
		alert("Invalid code at "+m);
		return 0;
	}             
	return 1 ;
}

function strCheck(s,msg) {
	return true;
}

function scCheck(s,msg) {
	var ck=/[\;]/;
	if (ck.test(s.value))
	{
		alert(msg+" includes invalid characters: \;");
		return false;
	}
	return true;
}

function refresh(destination) {
   window.location = destination ;		
}

function decomList(str,len,idx,dot) {  
	var t = str.split(dot);
	return t[idx];
}

function decomListLen(str, dot){
	var t = str.split(dot);
	return t.length;
}

function typeToIdx(type) {
	if (type=="tcp/udp")
		return 2 ;
	else if(type=="udp")
     	return 1 ;
	else 
		return 0 ;	
}

function IdxToType(idx) {
	if (idx == 2)
		return "tcp/udp" ;
	else if(idx == 1)
		return "udp" ;	
	else
		return "tcp";
}

function boolToType(bool) {
   if (bool)
      return "tcp" ;
    else return "udp" ;	
}

function boolToStr(bool) {
   if (bool)
      return "1" ;
    else return "0" ;	
}

function keyCheck(F){

   	var ok = 1 ;
	var cmplen;
	var i;

	for (i=1;i<5;i++) if (F.WEPDefKey[i-1].checked) break;
	var k=eval('F.key'+i);
  
   	if (F.wep_type.selectedIndex==0)
		cmplen=10;
	else
		cmplen=26;

	if (k.value.length!=cmplen) {alert("Length of Key "+i+" must be "+cmplen); ok=0 ;}
   	return ok ;
}

function valueToDayIdx(value) {   
   return (value/86400) ;		
}

function valueToTimeIdx(value) {   
   return ((value/3600)%24) ;		
}

function setCheckValue(t) {   	
   if (t.checked) t.value=1 ;
   else t.value=0 ;	
}

function preLogout() {	
   if ((confirm('Do you want to logout ?'))) {                    
     window.location = "login.htm";   	     
   }
}	

function showHidden(len) {
   var s = "" ;
   for (i=0;i<len;i++)
      s=s+"*" ;
   return s ;      			
}

function combinIP(d1,d2,d3,d4)
{
    var ip=d1.value+"."+d2.value+"."+d3.value+"."+d4.value;
    if (ip=="...")
        ip="";
    return ip;
}

function combinMAC(m1,m2,m3,m4,m5,m6)
{
    var mac=m1.value+":"+m2.value+":"+m3.value+":"+m4.value+":"+m5.value+":"+m6.value;
    if (mac==":::::")
        mac="";
    return mac;
}

function verifyIP0(ipa,msg)
{
	var ip=combinIP2(ipa);
	if (ip=='' || ip=='0.0.0.0') return true;
	return verifyIP2(ipa,msg);
}
function verifyIP2(ipa,msg,subnet)
{
	ip = new Array();
	if (ipa.length==4)
	{
		for (var i=0;i<4;i++)
			ip[i]=ipa[i].value;
	}
	else
		ip=ipa.value.split(".");
	if (ip.length!=4)
	{
        	alert(msg+" includes invalid characters!!!");
		return false;
	}

    for (var i = 0; i < 4; i++)
    {
        d = ip[i];
        if (d < 256 && d >= 0)
        {
			if (i!=3 || subnet==1)
				continue;
			else
			{
 				if (d != 255 && d !=0 )
					continue;
			}
        }	
        alert(msg+" includes invalid characters!!!");
        return false;
    }
    return true;
}

function verifyMAC(ma,s,sp){
    var t = /[0-9a-fA-F]{2}/;
	m = new Array();
	if (ma.length==6)
	{
		for (var i=0;i<6;i++)
			m[i]=ma[i].value;
	}
	else
		m=ma.value.split(":");

	if (sp) { if (m.toString()==',,,,,' ) return true; }
	for (var i=0;i<6;i++)
	{
		if (!t.test(m[i])) { alert(s+" includes invalid characters!!!");  return false; }
    }
    return true
}

function decomMAC2(ma,macs,nodef)
{
    var re = /^[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}$/;
    if (re.test(macs)||macs=='')
    {
		if (ma.length!=6)
		{
			ma.value=macs;
			return true;
		}
	if (macs!='')
        	var d=macs.split(":");
	else
		var d=['','','','','',''];
        for (i = 0; i < 6; i++)
		{
            ma[i].value=d[i];
			if (!nodef) ma[i].defaultValue=d[i];
		}
        return true;
    }
    return false;
}

function decomIP2(ipa,ips,nodef)
{
    var re = /^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$/;
    if (re.test(ips))
    {
        var d =  ips.split(".");
        for (i = 0; i < 4; i++)
		{
            ipa[i].value=d[i];
			if (!nodef) ipa[i].defaultValue=d[i];
		}
        return true;
    }
    return false;
}

function combinIP2(d)
{
	if (d.length!=4) return d.value;
    var ip=d[0].value+"."+d[1].value+"."+d[2].value+"."+d[3].value;
    if (ip=="...")
        ip="";
    return ip;
}
function combinMAC2(m)
{
    var mac=m[0].value+":"+m[1].value+":"+m[2].value+":"+m[3].value+":"+m[4].value+":"+m[5].value;
	mac=mac.toUpperCase();
    if (mac==":::::")
        mac="";
    return mac;
}

function ipMskChk(mn,str)
{
	var m=new Array();
	if (mn.length==4)
		for (i=0;i<4;i++) m[i]=mn[i].value;
	else
	{
		m=mn.value.split('.');
		if (m.length!=4) { alert(str+" includes invalid characters!!!") ; return 0; }
	}

	var t = /[^0-9]{1,}/;
	for (var i=0;i<4;i++)
	{
		if (t.test(m[i])||m[i]>255) { alert(str+" includes invalid characters!!!") ; return 0; }
	}

	var v=(m[0]<<24)|(m[1]<<16)|(m[2]<<8)|(m[3]);

   	var f=0 ;	  
   	for (k=0;k<32;k++)
	{
		if ((v>>k)&1) f = 1;
		else if (f==1)
		{
			alert(str+" includes invalid characters!!!") ;
			//for(var i=0; i<4; i++) m[i].value=m[i].defaultValue;
			return 0 ;
		}
	}
	if (f==0) { alert(str+" includes invalid characters!!!") ; return 0; }
	return 1;	
}

function Cfg(i,n,v)
{
	this.i=i;
    this.n=n;
    this.v=this.o=v;
}

var CA = new Array() ;

function addCfg(n,i,v)
{
	CA.length++;
    CA[CA.length-1]= new Cfg(i,n,v);
}

function newCfg(n,i,v)
{
	CA.length++;
  CA[CA.length-1]= new Cfg(i,n,v);
  CA[CA.length-1].o='';  
}

function idxOfCfg(kk)
{
    if (kk=='undefined') { alert("undefined"); return -1; }
    for (var i=0; i< CA.length ;i++)
    {

        if ( CA[i].n != 'undefined' && CA[i].n==kk )
            return i;
    }
    return -1;
}

function getCfg(n)
{
	var idx=idxOfCfg(n)
	if ( idx >=0)
		return CA[idx].v ;
	else
		return "";
}

function setCfg(n,v)
{
	var idx=idxOfCfg(n)
	if ( idx >=0)
	{
		CA[idx].v = v ;
	}
}

function diffCfg(n,i,v)
{
	var idx=idxOfCfg(n)
	if ( idx >=0)
	{
		CA[idx].i = i ;
		CA[idx].v = v ;
	}else newCfg(n, i, v);
}


function cfg2Form(f)
{
    for (var i=0;i<CA.length;i++)
    {
        var e=eval('f.'+CA[i].n);
        if ( e )
		{
			if (e.name=='undefined') continue;
			if ( e.length && e[0].type=='text' )
			{
				if (e.length==4) decomIP2(e,CA[i].v);
				else if (e.length==6) decomMAC2(e,CA[i].v);
			}
			else if ( e.length && e[0].type=='radio')
			{
				for (var j=0;j<e.length;j++)
					e[j].checked=e[j].defaultChecked=(e[j].value==CA[i].v);
			}
			else if (e.type=='checkbox')
				e.checked=e.defaultChecked=Number(CA[i].v);
			else if (e.type=='select-one')
			{
				for (var j=0;j<e.options.length;j++)
					 e.options[j].selected=e.options[j].defaultSelected=(e.options[j].value==CA[i].v);
			}
			else
				e.value=getCfg(e.name);
			if (e.defaultValue!='undefined')
				e.defaultValue=e.value;
		}
    }
}

var frmExtraElm='';
function form2Cfg(f)
{

    for (var i=0;i<CA.length;i++)
    {
        var e=eval('f.'+CA[i].n);
		if ( e )
		{
			if (e.disabled) continue;
			if ( e.length && e[0].type=='text' )
			{
				if (e.length==4) CA[i].v=combinIP2(e);
				else if (e.length==6) CA[i].v=combinMAC2(e);
			}
			else if ( e.length && e[0].type=='radio')
			{
				for (var j=0;j<e.length;j++)
					if (e[j].checked) { CA[i].v=e[j].value; break; }
			}
			else
			if (e.type=='checkbox')
				setCfg(e.name, Number(e.checked) );
			else
				setCfg(e.name, e.value);
		}
    }
}

var OUTF;
function frmHead(na,to,cmd,go)
{
	OUTF="<FORM name="+na+" action="+to+" method=POST>\n"+
	"<INPUT type=hidden name=CMD value="+cmd+">\n"+
	"<INPUT type=hidden name=GO value="+go+">\n";
}

function frmEnd()
{
	OUTF+="</FORM>\n";
}

function frmAdd(n,v)
{
	set1="<input type=hidden name="+n+" value=\"";
	v=v.replace(/\"/g,"&quot;");
	var r=new RegExp(set1+".*\n","g");
	if (OUTF.search(r) >= 0)
		OUTF=OUTF.replace(r,(set1+v+"\">\n"));
	else
		OUTF += (set1+v+"\">\n");
}

function genForm(n,a,d,g,u)
{
	frmHead(n,a,d,g);
	var sub=0;
    for (var i=0;i<CA.length;i++)
	{
			if (u && (CA[i].v == CA[i].o)){
				continue;
			}
			frmAdd("SET"+sub,String(CA[i].i)+"="+CA[i].v);
			sub++;
	}
	if (frmExtraElm.length)
		OUTF+=frmExtraElm;
	frmExtraElm=''; //reset
	frmEnd();
	return OUTF;
}

function _subForm(f1,a,d,g,u)
{
	var msg=genForm('OUT',a,d,g,u);

	// Debug option
//	if (!confirm(msg)) return;
 
	var newElem = document.createElement("div");
	newElem.innerHTML = msg ;
	f1.parentNode.appendChild(newElem);
	f=document.OUT;
	f.submit();
}

function subForm(f1,a,d,g)
{
	_subForm(f1,a,d,g,0);
}

function subFormChanges(f1,a,d,g)
{
	_subForm(f1,a,d,g,1);
}

function addFormElm(n,v)
{
	var set1='<input type=hidden name='+n+' value="'+v+'">\n';
	frmExtraElm += set1;
}

function pwdSame(p,p2)
{
   	if (p != p2) { alert("Passwords unmatch!") ; return 0 ; }
   	else return 1 ;
}

function chkPwdUpdate(p,pv,c)
{
	if (c.value=='0') return true;
    // modified
    if (!pwdSame(p.value,pv.value)) return false;
    if (!confirm('Password change?'))
    {
        c.value=0 ;
        p.value=pv.value=p.defaultValue;
        return false;
    }
    return true;
}

function chkPwd1Chr(p,pv,c)
{
   	if (c.value=='0')
   	{
  		p.value=pv.value=""; // reset to null;
  		c.value='1';
	}
}

function chkPwd1Chr2(po,p,pv,c)
{
   	if (c.value=='0')
   	{
  		po.value=p.value=pv.value=""; // reset to null;
  		c.value='1';
	}
}

function chkStrLen(s,m,M,msg)
{
	var str=s.value;
	if ( str.length < m || str.length > M )
	{
		alert(msg+" is too long!!");
		return false;
	}
    return true;
}

function isIE()
{
	var agt = navigator.userAgent.toLowerCase();
	return (agt.indexOf("msie") != -1); // ie
}

function fit2(n)
{
	var s=String(n+100).substr(1,2);
	return s;
}

function timeStr(t)
{
	if(t < 0)
	{
		str='00:00:00';
		return str;
	}
	var s=t%60;
	var m=parseInt(t/60)%60;
	var h=parseInt(t/3600)%24;
	var d=parseInt(t/86400);

	var str='';
	if (d > 999) { return 'Forever'; }
	if (d) str+=d+' days ';
	str+=fit2(h)+':';
	str+=fit2(m)+':';
	str+=fit2(s);
	return str;
}

// auto,NA,IC,ETS,SP,FR,JP
var dmnRng= new Array(16383,2047,2047,8191,1536,7680,16383);

function chanList(Opt,dn)
{
	var j = 0;
	for(var i=1;i<=14;i++)
	{
		if(dmnRng[dn] & (1<<(i-1)))
        {
			var fr;
			if (i!=14) fr=i*0.005+2.407;
			else fr=2.484;
			var opn = new Option(i+" - "+fr+"GHz",i);
			Opt.options[j++] = opn;
		}
	}
}

function rmEntry(a,i)
{
	if (a.splice)
		a.splice(i,1);
	else
	{
		if (i>=a.length) return;
		for (var k=i+1;k<=a.length;k++)
			a[k-1]=a[k];
		a.length--;
	}
}


function getStyle(objId) {
	var obj=document.getElementById(objId);
	if (obj) return obj.style;
	else return 0;
}

function setStyle(id, v) {
    var st = getStyle(id);
    if(st) { st.visibility = v; return true; } else return false;
} 

function setIdVal(id,val)
{
document.getElementById(id).innerHTML=val;

}






function TestBit(mask, bitNum)
{
	return (mask & (1 << bitNum))
}
function switchPage(c, n) 
{
  	if (getStyle(c) && getStyle(n)) 
	{
		lastPage=c;
		setStyle(c, "hidden");
		setStyle(n, "visible");
	}

}







function DisByEtherMask(portMask,etherMask,count)
{
	var port = 0;
	var n = 0;
	var h = false;
	var first = true;
	var cableStart = 0;
	var cableNum = 0;

	for(port=0; port<count; port++)
	{
		if(port%32 == 0 && port !=0)
		{
			n++;
		}

		if(TestBit(etherMask[n], port%32))
		{
			if(first)
			{
				cableStart = port;
				first = false;
			}
			cableNum++;
		}
	}

	first = true;
	n = 0;

	for(port=0; port<cableStart; port++)
	{
		if(port%32 == 0 && port !=0)
		{
			n++;
		}

		if(TestBit(portMask[n], port%32))
		{
			if( h )
			{
				if(port+1<cableStart && TestBit(portMask[n], (port+1)%32))
				{
					continue;
				}
				else
				{
					document.write('-');
					h = false;
				}
			}
			else
			{
				if(port+1<cableStart && TestBit(portMask[n], (port+1)%32))
				{
					h = true;
				}

				if(!first)
				{
					document.write(',');
				}
			}

			document.write("Cable");

			first = false;
		}
	}

	for(port=cableStart; port<count; port++)
	{
		if(port%32 == 0 && port !=0)
		{
			n++;
		}

		if(TestBit(portMask[n], port%32))
		{
			if( h )
			{
				if(port+1<count && TestBit(portMask[n], (port+1)%32))
				{
					continue;
				}
				else
				{
					document.write('-');
					h = false;
				}
			}
			else
			{
				if(port+1<count && TestBit(portMask[n], (port+1)%32))
				{
					h = true;
				}

				if(!first)
				{
					document.write(',');
				}
			}

			document.write("Ethernet");
			if(5 == count)
			{
				document.write(port);
			}

			first = false;
		}
	}	

}


function check_input(input,min,max)
{
    var valid=true;
    var inputtemp;
    var ch;
    var cnt;
	var len;
    inputtemp=input.toString();
	len=inputtemp.length;
	if(0==len)
	{
		valid=false;
        return valid;
	}
	for(cnt=len-1;cnt>=0;cnt--)
	{
		ch=inputtemp.charAt(cnt);
		if(' '==ch)
		{
		    len--;
		}
		else
	    {
		    break;
	    }	
	}
    for(cnt=0;cnt<len;cnt++)
    {
        ch=inputtemp.charAt(cnt);
        if((ch>'9')||(ch<'0'))
        {
            valid=false;
            return valid;
        }
    }
    if((input>max)||(input<min))
    {
        valid=false;
    }
    return valid;
}
function isValidDec(dec)
{	
	//检测字符串是否有非法字符
    	result = dec.match(/\s*[0-9]+\s*/);

	if(result != null)//匹配到字符串
	{		
		if(result[0] == dec)//字符串中无非法字符
		{
			return true;
		}
		else
		{
			return false;
		}
	}   
	
	return false; 
}


function macsCheck(I)
{
	var m = /(^([A-Fa-f0-9]{2}:){5}[A-Fa-f0-9]{2}$)/;
	if(I.length!=17 || !m.test(I))
	{
	    alert("Invalid MAC address, please input again.");
	    return false;
	}
	if(I == "00:00:00:00:00:00")
	{
		alert("Invalid MAC address, please input again.");
		return false;
	}
	var u = I.split(":");
	var wrong = /[fF]{2}/;
	if(wrong.test(u[0]))
	{
		if( wrong.test(u[1]) && wrong.test(u[2]) && wrong.test(u[3]) && wrong.test(u[4]) && wrong.test(u[5]))
		{
			alert("Invalid MAC address, please input again.");
			return false;
		}
	}
	n=parseInt(u[0],16);
	if (n&0x1)
	{
		alert("Invalid MAC address, please input again.");
	   	return false;
	}
	return true;
}

function QueryString(qry)
{
	var parm = geturlparam();
	if ( parm.indexOf("?") > -1 ) {
//		var strQueryString = parm.substr(parm.indexOf("?") + 1).toLowerCase();
		var strQueryString = parm.substr(parm.indexOf("?") + 1);
		var s = strQueryString.split('&');
		for(var i = 0; i < s.length; i ++) {
			var s1 = s[i].split('=');
			if(s1[0] == qry && qry != "GO") return s1[1];
			if(s1[0] == "GO") return parm.substr(parm.indexOf(qry) + 3);
		}
	}
	return '';
}

function gotoUrl(url)
{
	location = url;
}

function port_name(s)
{
	if(s == 'eth0') return 'Ethernet 1';
	if(s == 'eth1') return 'Ethernet 2';
	if(s == 'cab0') return 'Cable 1';
	if(s == 'cab1') return 'Cable 2';
}



function isValidVlanItem(i)
{
	var mi, ma;
	var v=i.split('-');
	if (v.length == 2){
			mi = parseInt(v[0]);
			ma = parseInt(v[1]);
			if (mi >= ma) return false;
			if ((ma > 0) && (ma < 4095)) return true;
			return false;
	}else {
		mi = parseInt(i);	
	}
  if ((mi > 0) && (mi < 4095)) return true;
	return false;
}

function isValidVlanString(s)
{	
	/*1,2-45,100-960*/
  var vstr= /^((\d{1,4}\-\d{1,4}|\d{1,4})\,)*(\d{1,4}\-\d{1,4}|\d{1,4})$/; 
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
			mi = parseInt(v[0]);
			ma = parseInt(v[1]);
			if (mi <= ma){
				for(;mi <= ma; mi++){
					ret[cnt++] = mi;
				}
			}
	}else {
			ret[0] = parseInt(i);

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
		vin=parseInt(vs[j]);
		if (vlan_ids[vin] != null) return true;
	}	
	return false;
}


function isAnyVlanNotExist(vstr, vlan_ids)
{
	var vin;
	var vs=parse_vlan(vstr);
	for (var j=0; j < vs.length; j++){
		vin=parseInt(vs[j]);
		if (vlan_ids[vin] == null) return true; 
	}	
	return false;
}

