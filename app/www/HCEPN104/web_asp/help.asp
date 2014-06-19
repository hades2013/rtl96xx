<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<link rel="stylesheet" href="style.css" type="text/css">
<title></title>
<style type="text/css">
<!--
.style1 {color: #666666}
-->
</style>
</head>
<body>
<table width="90%" border="0" cellspacing="0" cellpadding="0" align="center">
	<tr>
		<td valign="top"><p><font face="Arial, Helvetica, sans-serif" size="4"><b><br>
		</b></font><b class="bluetextbold"><font size="4">Broadband Router BASICS</font></b></p>
		<ol>
		<li> Check power connection</li>
		<li> Verify power light status</li>
		<li> Check connections for networking interface(s)</li>
		<li> If you are unable to access the Broadband Router's user interface, please chec
			  your computer's network settings - verify that your&nbsp;&nbsp; TCP/IP settings
		are correct. Check User Guide for more details.</li>
		<li> Ensure that the latest firmware is installed in the Broadband Router<br>
		</li>
		</ol>
		<p>&nbsp;</p>
			<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
			<tr>
				<td><b><a name="password" class="help-a"> Administrator Settings</a>&nbsp;</b></font></td>
			</tr>
			</table>
			Use this menu to restrict management access based on a specific
			password. By default, there is no password. So please assign a password to
			the Administrator as soon as possible, and store it in a safe place.<br>
			Passwords can contain from 3-12 alphanumeric characters, and are
			case sensitive.
			<ul>
			<li><b>Administrator Time-out</b> - The amount of time of inactivity before
				the Broadband Router will automatically close the Administrator session.
				Set this to zero to disable it. </li>
			<li><b>Remote Management</b> - By default, management access is only available
				to users on your local network. However, you can also manage the Broadband
				Router from a remote host by adding the IP address of an administrator to this screen.</li>
			</ul>
		<p><a href="#">top</a></p>
		<p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><b><a name="time" class="help-a">System Time</a></b></font></td>
		</tr>
		</table>
		Set the time zone for the Broadband Router and 
		connecting to a Simple Network Time Protocol(SNTP) server allows the Broadband Router to synchronize
		the system clock to the global Internet. The synchronized clock in the Broadband Router is used to 
		recored the security log and control client filtering. <br>      
		<a href="#">top</a>
		<p>&nbsp;</p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><b><a name="lan" class="help-a"> LAN Settings</a></b></font></td>
		</tr>
		</table>
		Configure the gateway address of the Broadband Router. To dynamically assign
		the IP address for client PCs, enable the DHCP Server, set the lease time,
		and then specify the address range.&nbsp;
		<p>Valid IP addresses consist of four numbers, and are separated by periods.
		The first three fields are the network portion, and can be from 0-255,
		while the last field is the host portion and can be from 1-254.<br>
		<a href="#">top</a></p>
		<p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><a name="lan_dhcp_list" class="help-a"><b>DHCP Client List</b></a></font></td>
		</tr>
		</table>
		The DHCP client list allows you to see which clients are connected to the Broadband Router via IP
		address, host name, and MAC address.<br>
		<a href="#">top</a>
		<p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
		<td><font size="3"><b><a name="wan" class="help-a"> WAN</a></b></font></td>
		</tr>
		</table>
		Specify the WAN connection type required by your Internet Service
		Provider, then click &quot;Apply Button&quot; to provide detailed
		configuration parameters for the selected connection type.<br>
		<a href="#">top</a>
		<p>&nbsp;</p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><b> <a name="dhcp" class="help-a"> WAN | Dynamic IP Address</a></b></font></td>
		</tr>
		</table>
		The Host Name is optional, but may be required by some ISPs. The default
		MAC address is set to the WAN's physical interface on the Broadband Router. Use
		this address when registering for Internet service, and do not change it
		unless required by your ISP, You can use the &quot;Clone MAC Address&quot;
		button to copy the MAC address of the Ethernet Card installed by your ISP
		and replace the WAN MAC address with this MAC address.<br>
		<a href="#">top</a>
		<p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><b> <a name="fixed" class="help-a"> WAN | Fixed IP</a></b></font></td>
		</tr>
		</table>
		If your Internet Service Provider has assigned a fixed address, enter
		the assigned address and subnet mask for the Broadband Router, then enter the
		gateway address of your ISP.<br>
		<a href="#">top</a>
		<p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><b><a name="pppoe" class="help-a"> WAN | PPPoE</a></b></font></td>
		</tr>
		</table>
		Enter the PPPoE user name and password assigned by your Service Provider.  The Service 
		Name is normally optional, it may be required by some service providers. 
		Enter a Maximum Idle Time (in minutes) to define a maximum period of time for which
		the Internet connection is maintained during inactivity.
		If the connection is inactive for longer than the defined Maximum Idle Time, then it will be dropped.
		You can enable the Auto-reconnect option to automatically re-establish the connection
		as soon as you attempt to access the Internet again.
		<ul>
		<li><b><i>Maximum Idle time</i></b> - 
			The amount of time of inactivity before disconnecting your PPPoE session.
			Either set this to zero or enable Auto-reconnect to disable this feature.</li>
		<li><b><i>Auto-reconnect</i></b> - 
			If enabled, the Broadband Router will automatically connect to your ISP after your system
			is restarted or the connection is dropped. </li>
		<li><b><i>&lt;Connect&gt;</i></b> - 
			Clicking the Connect button will trigger the Broadband Router to initiate a PPPoE connection.</li>
		<li><b><i>&lt;Disconnect&gt;</i></b> - 
			Clicking the Disconnect button will trigger the Broadband Router to cut-off the PPPoE connection.</li>
		</ul>
		<p><a href="#">top</a><font face="Arial, Helvetica, sans-serif" size="2"></p>
		<p>&nbsp;</p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><b> <a name="pptp" class="help-a"> WAN | PPTP</a></b></font></td>
		</tr>
		</table>
		Enter the PPTP user name and password assigned by your Service Provider. 
		Enter a Maximum Idle Time (in minutes) to define a maximum period of time for which
		the Internet connection is maintained during inactivity.
		If the connection is inactive for longer than the defined Maximum Idle Time, then it will be dropped.
		You can enable the Auto-reconnect option to automatically re-establish the connection
		as soon as you attempt to access the Internet again.<br>
		<ul>
		<li><b><i>Maximum Idle time</i></b> - 
			The amount of time of inactivity before disconnecting your PPPoE session.
			Either set this to zero or enable Auto-reconnect to disable this feature.</li>
		<li><b><i>Auto-reconnect</i></b> - 
			If enabled, the Broadband Router will automatically connect to your ISP after your system
			is restarted or the connection is dropped. </li>
		<li><b><i>&lt;Connect&gt;</i></b> - 
			Clicking the Connect button will trigger the Broadband Router to initiate a PPTP connection.</li>
		<li><b><i>&lt;Disconnect&gt;</i></b> - 
			Clicking the Disconnect button will trigger the Broadband Router to cut-off the PPTP connection.</li>
		</ul>
		<a href="#">top</a>
		<p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><b><a name="dns" class="help-a"> DNS</a></b></font></td>
		</tr>
		</table>
		Domain Name Servers are used to map an IP address to the equivalent
		domain name(e.g.www.yahoo.com). Your ISP should provide the IP address for one or more domain
		name servers.<br>
		<a href="#">top</a>
		<p>&nbsp;</p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><b><a name="dyndns" class="help-a"> Dynamic DNS</a></b></font></td>
		</tr>
		</table>
		Dynamic DNS (Domain Name Service) is a method of keeping a domain name linked to a changing (dynamic) IP address.
		With most Cable and DSL connections, you are assigned a dynamic IP address and that address is used only for the
		duration of that specific connection. With the router, you can setup your DDNS service and the router will automatically
		update your DDNS server every time it receives a different IP address. <br>
		<a href="#">top</a>
		<p>&nbsp;</p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><b><a name="virserver" class="help-a"> Virtual Server</a></b></font></td>
		</tr>
		</table>
		If you configure the Broadband Router as a virtual server, remote users accessing
		services such as Web or FTP at your local site via public IP addresses can
		be automatically redirected to local servers configured with private IP
		address. In other words, depending on the requested service (TCP/UDP port
		number), the Broadband Router redirects the external service request to the
		appropriate server.
		<p><b>Example:</b>
		</p>
		<TABLE BORDER="0" CELLPADDING=3 CELLSPACING=1 bgcolor="#999999">
		<TR ALIGN=CENTER><TD bgcolor="#CCCCCC">ID</TD>
		<TD bgcolor="#CCCCCC">Private IP</TD>
		<TD bgcolor="#CCCCCC">Private Port</TD>
		<TD bgcolor="#CCCCCC">Type</TD>
		<TD bgcolor="#CCCCCC">Comment</TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF">1</TD>
		<TD bgcolor="#FFFFFF">192.168.2.20</TD>
		<TD bgcolor="#FFFFFF">80</TD>
		<TD bgcolor="#FFFFFF">TCP</TD>
		<TD bgcolor="#FFFFFF">Web Server</TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF">2</TD>
		<TD bgcolor="#FFFFFF">192.168.2.12</TD>
		<TD bgcolor="#FFFFFF">21</TD>
		<TD bgcolor="#FFFFFF">TCP</TD>
		<TD bgcolor="#FFFFFF">FTP Server</TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF">3</TD>
		<TD bgcolor="#FFFFFF">192.168.2.28</TD>
		<TD bgcolor="#FFFFFF">23</TD>
		<TD bgcolor="#FFFFFF">TCP</TD>
		<TD bgcolor="#FFFFFF">Telnet Server</TD>
		</TR>
		</table>
		<p><a href="#">top</a></p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><b><a name="specapp"> Special Application</a></b></font></td>
		</tr>
		</table>
		Some applications require multiple connections, such as Internet
		gaming, video conferencing, Internet telephony and others. These
		applications cannot work when Network Address Translation (NAT) is
		enabled. If you need to run applications that require multiple
		connections,-specify the port normally associated with an application in the
		&quot;Trigger Port&quot; field,select the protocol type as TCP or UDP,
		then enter the public ports associated with the trigger port to open them
		for inbound traffic.<br>
		<br><b>Example:</b><br><br>
		<TABLE BORDER="0" CELLPADDING=3 CELLSPACING=1 bgcolor="#999999">
		<TR ALIGN=CENTER><TD bgcolor="#CCCCCC">ID</TD>
		<TD bgcolor="#CCCCCC">Trigger Port</TD>
		<TD bgcolor="#CCCCCC">Trigger Type</TD>
		<TD bgcolor="#CCCCCC">Public Port</TD>
		<TD bgcolor="#CCCCCC">Public Type</TD>
		<TD bgcolor="#CCCCCC">Comment</TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF">1</TD>
		<TD bgcolor="#FFFFFF">28800</TD>
		<TD bgcolor="#FFFFFF">UDP</TD>
		<TD bgcolor="#FFFFFF">2300-2400,<br>47624</TD>
		<TD bgcolor="#FFFFFF">UDP</TD>
		<TD bgcolor="#FFFFFF">MSN Game Zone</TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF">2</TD>
		<TD bgcolor="#FFFFFF">28800</TD>
		<TD bgcolor="#FFFFFF">UDP</TD>
		<TD bgcolor="#FFFFFF">2300-2400,<br>47624</TD>
		<TD bgcolor="#FFFFFF">TCP</TD>
		<TD bgcolor="#FFFFFF">MSN Game Zone</TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF">3</TD>
		<TD bgcolor="#FFFFFF">6112</TD>
		<TD bgcolor="#FFFFFF">UDP</TD>
		<TD bgcolor="#FFFFFF">6112</TD>
		<TD bgcolor="#FFFFFF">UDP</TD>
		<TD bgcolor="#FFFFFF">Battle.net</TD>
		</TR>      
		</table>
		<p><a href="#">top</a></p>
		<p>&nbsp; </p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><b><a name="firewall"> Firewall</a></b></font></td>
		</tr>
		</table>
		The Broadband Router provides extensive firewall protection by restricting connection
		parameters to limit the risk of intrusion and defending against a wide array
		of common hacker attacks. However, for applications that require unrestricted access
		to the Internet, you can configure a specific client/server as a demilitarized zone (DMZ).
		<br>
		<a href="#">top</a>
		<p>&nbsp;</p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><b>
			<font size="3"><a name="discard_ping"> Discard PING from WAN side</a></font>
			<a name="discard_ping"></a></b>
			</td>
		</tr>
		</table>
		When this feature is enabled, hosts on the WAN cannot ping the Broadband Router.<br>
		<a href="#">top</a>
		<p>&nbsp; </p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><b><a name="filter"> Client Filtering</a></b></font></td>
		</tr>
		</table>
		You can filter Internet access for local clients based on IP
		addresses, application types, (i.e., HTTP port), and time of day.<br>
		For example, this screen shows that clients in the address range
		192.168.2.50-99 are permanently restricted from using FTP (Port 21), while
		clients in the address range 192.168.2.110-119 are blocked from browsing
		the Internet from Monday through Friday.<br>
		<a href="#">top</a>
		<p>&nbsp; </p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><a name="mac_control"> <b>MAC Control</b></a></font></td>
		</tr>
		</table>
		You can block certain client PCs accessing the Internet based on MAC address.<br>
		<a href="#">top</a>
		<p>&nbsp; </p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><a name="urlfilter"> <b>URL Filter</b></a></font></td>
		</tr>
		</table>
		URL Filter is used to deny LAN computers from accessing specific web sites by its URL. 
		A URL is a specially formatted text string that defines a location on the Internet. If 
		any part of the URL contains the blocked word, the site will not be accessible. If any 
		part of the URL contains the blocked word, the web page will not display.<br>
		<a href="#">top</a>
		<p>&nbsp; </p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><a name="dmz"> <b>Virtual DMZ Host</b></a></font></td>
		</tr>
		</table>
		<a href="#"></a>If you have a client PC that cannot run Internet application properly
		from behind the NAT firewall or after configuring the Special Applications
		function, then you can open the client up to unrestricted two-way Internet
		access.<br>Enter the IP address of a DMZ host to this screen. Adding a client to the
		DMZ (Demilitarized Zone) may expose your local network to a variety of
		security risks, so only use this option as a last resort.<br>
		<a href="#">top</a>
		<p>&nbsp;</p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><a name="route_static"> <b>Static Routing</b></a></font></td>
		</tr>
		</table>
		<a href="#"></a> A static route is a pre-determined pathway that network information 
		must travel to reach a specific host or network.<br>
		<a href="#">top</a>
		<p>&nbsp;</p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><font size="3"><b><a name="status">Status</a></b></font></td>
		</tr>
		</table>
		You can use the Status screen to see the connection status for the
		Broadband Routers' WAN/LAN interfaces, firmware and hardware version numbers,
		and the number of connected clients to your network.<br><br>
		The following items are included in this screen:
		<ul>
		<li><b>INTERNET</b> - Displays WAN connection type and status.</li>
		<li><b>GATEWAY</b> - Displays system IP settings, as well as DHCP, NAT and Firewall status.</li>
		<li><b>INFORMATION</b> - Displays the number of connected clients, as well as the
			Broadband Router's hardware and firmware version numbers.&nbsp;</li>
		</ul>
		<p><a href="#">top</a> </p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><b><a name="log"><font size="3">Security Logs</font></a></b></td>
		</tr>
		</table>
		<b>Security Log</b> - Displays any illegal attempts to access your network.
		<p><b>Example :</b> </p>
		<TABLE BORDER="0" CELLPADDING=3 CELLSPACING=1 bgcolor="#666666">
		<TR ALIGN=CENTER><TD bgcolor="#CCCCCC"><font face="Arial, Helvetica, sans-serif" size="1">Date</font></TD>
		<TD bgcolor="#CCCCCC"><font face="Arial, Helvetica, sans-serif" size="1">Time</font></TD>
		<TD bgcolor="#CCCCCC"><font face="Arial, Helvetica, sans-serif" size="1">Event Type</font></TD>
		<TD bgcolor="#CCCCCC"><font face="Arial, Helvetica, sans-serif" size="1">Packet Type</font></TD>
		<TD bgcolor="#CCCCCC"><font face="Arial, Helvetica, sans-serif" size="1">Source IP/Port</font></TD>
		<TD bgcolor="#CCCCCC"><font face="Arial, Helvetica, sans-serif" size="1">&nbsp;</font></TD>
		<TD bgcolor="#CCCCCC"><font face="Arial, Helvetica, sans-serif" size="1">Destination IP/Port</font></TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">1970/01/01</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">00:02:50</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">** UDP Port LoopBack **</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">&lt;IP/UDP&gt;</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.36:1090</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">->></font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.4:88</font></TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">1970/01/01</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">00:02:50</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">** Snork **</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">&lt;IP/UDP&gt;</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.36:1090</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">->></font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.4:88</font></TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">1970/01/01</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">00:02:50</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">** Smurf **</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">&lt;IP/ICMP&gt;</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.36:1090</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">->></font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.4:88</font></TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">1970/01/01</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">00:02:50</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">** TCP Null Scan **</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">&lt;IP/TCP&gt;</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.36:1090</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">->></font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.4:88</font></TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">1970/01/01</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">00:02:50</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">** TCP SYN Flooding **</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">&lt;IP/TCP&gt;</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.36:1090</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">->></font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.4:88</font></TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">1970/01/01</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">00:02:50</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">** IP Spoofing **</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">&lt;IP/TCP&gt;</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.36:1090</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">->></font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.4:88</font></TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">1970/01/01</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">00:02:50</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">** Ping of Death **</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">&lt;IP/ICMP&gt;</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.36:1090</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">->></font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.4:88</font></TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">1970/01/01</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">00:02:50</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">** IP with Zero Length **</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">&lt;IP/TCP&gt;</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.36:1090</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">->></font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.4:88</font></TD>
		</TR>
		<TR ALIGN=CENTER><TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">1970/01/01</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">00:02:50</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">** Unauthorized HTTP Access **</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">&lt;IP/TCP&gt;</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.36:1090</font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">->></font></TD>
		<TD bgcolor="#FFFFFF"><font face="Arial, Helvetica, sans-serif" size="1">10.1.16.4:88</font></TD>
		</TR>
		</table>
		<p><br><b>DHCP Client Log</b> - Displays information on all DHCP clients on	your network.<br>
		<a href="#">top</a>
		<br>
		</p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><b><a name="reset"><font size="3">Reset</font></a></b></td>
		</tr>
		</table>
		<b>Reset Broadband Router</b> - Reboot this device. And retain all existing configuration settings.<br>
		<a href="#">top</a>
		<p>&nbsp;</p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><b><a name="wtools"><font size="3">Tools</font></a></b></td>
		</tr>
		</table>
		<p><b><i>Restore Factory Defaults</i></b> - Reset the settings of this device to the factory default values.<br>
		<b><i>Backup Settings</i></b> - Save the settings of this device to a file.<br>
		<b><i>Restore Settings</i></b> - Restore the settings of this device to the backup settings.<br>
		<font size="1" face="Arial, Helvetica, sans-serif" color="#0000ff"><a href="#">top</a></font></p>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#F0F0F0">
		<tr>
			<td><b><a name="glos">GLOSSARY OF TERMS</a></b></td>
		</tr>
		</table>
		<p><b><font color="#00cc00"><br></font></b><a href="#">top</a></p>
		<p><b>DHCP</b>
		Dynamic Host Configuration Protocol. This protocol automatically
		configures the TCP/IP settings of every computer on your home network.<br>
		<a href="#">top</a></p>
		<p><b>DNS Server
		Address</b> DNS stands for Domain Name System, which allows Internet
		host computers to have a domain name (such as www.yahoo.com) and one or more
		IP addresses (such as 66.218.71.81). A DNS server keeps a database of
		host computers and their respective domain names and IP addresses, so
		that when a domain name is requested (as in typing &quot; www.yahoo.com&quot;
		into your Internet browser), the user is sent to the proper IP address.
		The DNS server address used by the computers on your home network is the
		location of the DNS server your ISP has assigned.<br>
		<a href="#">top</a></p>
		<p><b>DSL Modem</b>
		DSL stands for Digital Subscriber Line. A DSL modem uses your existing
		phone lines to transmit data at high speeds.<br>
		<a href="#">top</a></p>
		<p><b>Ethernet</b>
		A standard for computer networks. Ethernet networks are connected by
		special cables and hubs, and move data around at up to 100 million bits
		per second (Mbps).<br>
		<a href="#">top</a></p>
		<p><b>IP Address</b>
		IP stands for Internet Protocol. An IP address consists of a series of
		four numbers separated by periods, that identifies an single, unique 
		Internet computer host. Example: 66.218.71.81.<br>
		<a href="#">top</a></p>
		<p><b>ISP Gateway Address</b>
		(see ISP for definition). The ISP Gateway Address is an IP
		address for the Internet router located at the ISP's office. This
		address is required only when using a cable or DSL modem.<br>
		<a href="#">top</a></p>
		<p><b>ISP</b>
		Internet Service Provider. An ISP is a business that provides
		connectivity to the Internet for individuals and other businesses or organizations.<br>
		<a href="#">top</a></p>
		<p><b>LAN</b>
		Local Area Network. A LAN is a group of computers and devices connected 
		together in a relatively small area (such as a house or an office). Your 
		home network is considered a LAN.<br> 
		<a href="#">top</a></p>
		<p><b>MAC Address</b>
		MAC stands for Media Access Control. A MAC address is the hardware 
		address of a device connected to a network.<br>
		<a href="#">top</a></p>
		<p><b>NAT</b>
		Network Address Translation. This process allows all of the computers on your
		home network to use one IP address. The NAT capability of the Broadband Router,
		allows you to access the Internet from any
		computer on your home network without having to purchase more IP
		addresses from your ISP.<br>Network Address Translation can be used to give
		multiple users access to the Internet with a single user account, or to map
		the local address for an IP server(such as Web or FTP) to a public address.
		This secures your network from direct attack by hackers, and provides more 
		flexible management by allowing you to change internal IP addresses without
		affecting outside access to your network. NAT must be enabled to provide 
		multi-user access to the Internet or to use the Virtual Server function.<br>
		<a href="#">top</a></p>
		<p><b>PPPoE</b>
		Point-to-Point Protocol over Ethernet. Point-to-Point Protocol is a                               
		method of secure data transmission originally created for dial-up      
		connections. PPPoE is for Ethernet connections.<br>                              
		<a href="#">top</a></p>                              
		<p><b>Subnet Mask </b>A                               
		subnet mask, which may be a part of the TCP/IP information provided by                               
		your ISP, is a set of four numbers configured like an IP address. It is                               
		used to create IP address numbers used only within a particular network                               
		(as opposed to valid IP address numbers recognized by the Internet.<br>                             
		<a href="#">top</a></p>                            
		<p><b>TCP/IP</b>                             
		Transmission Control Protocol/Internet Protocol. This is the standard                             
		protocol for data transmission over the Internet.<br>                            
		<a href="#">top</a></p>
		<p><b>WAN</b>
		Specify the WAN connection type
		required by your Internet Service Provider, then click "Apply" to provide detailed configuration
		parameters for the selected connection type. Specify one of the first five options to configure a
		WAN connection through the RJ-45 port. <br>
		<a href="#">top</a></p>
		</td>
	</tr>
	</table>
</body>
</html>
