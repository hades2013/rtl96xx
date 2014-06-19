/* 
	Copyright 2013-2020, 
	Shenzhen Hexicom Technologies Co., Ltd. 
	All rights reserved.
	Author: Einsn Liu (liuchuansen@hexicomtech.com)
	Date: 2013-03-25 
*/


function Page(page, name)
{
    this.page = page;
    this.name = name;    
}

function Menu(page, name, submenu)
{
    this.page = page;     
    this.name = name;
    this.submenu = submenu;
}

/* Default menu system */

var subMenuSystem = new Array(
    new Page("system_status.asp", "System Status"),
    new Page("system_time.asp", "Date & Time"),
    new Page("system_configure.asp", "Configuration"),
    new Page("system_reset.asp", "Reboot"),
    new Page("system_upgrade.asp", "Upgrade"),
    new Page("system_log.asp", "System Log")
);

var subMenuManage = new Array(
	new Page('system_man_ip.asp', 'IP Setting'),
	new Page('system_man_vlan.asp', 'Security'),
	new Page('system_snmp.asp', 'SNMP'),
	new Page('system_password.asp', 'Administrator')
);

var subMenuDevice = new Array(
	//new Page('port_table.asp', 'Interface'),
	//new Page('port_storm.asp', 'StormFilter'),
	new Page('vlan_mode.asp', 'VLAN Mode'),
	new Page('vlan_8021q.asp', '802.1Q VLAN'),
	new Page('vlan_access.asp', 'Access VLAN'),
	new Page('vlan_trunk.asp', 'Trunk VLAN'),
	new Page('vlan_hybrid.asp', 'Hybrid VLAN'),
	new Page('qos.asp', 'QoS'),
	new Page('mac_dispaly.asp', 'MAC'),
	new Page('mac_port_dispaly.asp', 'MAC端口显示')
);

var subMenuPort = new Array(
	new Page('port_stat.asp', '端口设置'),
	//new Page('port_table.asp', 'Interface'),
	new Page('port_storm.asp', 'StormFilter'),
	new Page('port_mirror.asp', '端口镜像'),
	new Page('port_linerate.asp', '端口限速'),
	new Page('port_conf.asp', ''),
	new Page('port_statistic.asp', ''),
	new Page('port_stormconf.asp', ''),
	new Page('port_rateset.asp', '')
);

var subMenuBusiness = new Array(
	new Page('service_main.asp', 'Master'),
	new Page('service_map.asp', 'Topology'),
	new Page('service_templates.asp', 'Template')
);

var subMenuUsers = new Array(
	new Page('user_list.asp', 'UserList'),
	new Page('user_history.asp', 'History'),
	new Page('user_white.asp', 'Whitelist'),
	new Page('user_black.asp', 'Blacklist')
);

var menuDefault = new Array(
	new Menu('system_status.asp', 'Sytem Setting', subMenuSystem),
	new Menu('system_man_ip.asp', 'Management', subMenuManage),
	new Menu('vlan_mode.asp', 'Switch Setting', subMenuDevice),
	new Menu('port_stat.asp', 'Port Setting', subMenuPort),
	new Menu('service_main.asp', 'Network Topology', subMenuBusiness),
	new Menu('user_list.asp', 'User List', subMenuUsers)
);

/* Changes made for NSCRTV-HC */

var subMenuManage_nscrtvhc = new Array(
	new Page('system_man_ip.asp', 'IP Setting'),
	new Page('system_man_vlan.asp', 'Security'),
	//new Page('n_system_snmp.asp', 'SNMP'),
	new Page('system_snmp.asp', 'SNMP_AGENT'),
	new Page('system_snmp_group.asp', 'SNMP_GROUP'),
	new Page('system_snmp_user.asp', 'SNMP_USER'),
	new Page('system_snmp_trap.asp', 'SNMP_TRAP'),
	new Page('system_password.asp', 'Administrator')
);

var subMenuBusiness_nscrtvhc = new Array(
	new Page('service_main.asp', 'Master'),
	new Page('service_map.asp', 'Topology'),
	new Page('n_user_white.asp','Whitelist'),
	new Page('n_service.asp', 'Service'),
	new Page('n_vlanpool.asp', 'VLAN Pool'),
	new Page('service_templates.asp', 'Template'),
	new Page('pon_stat.asp', 'PON状态')
);

var menuNscrtvHC = new Array(
	new Menu('system_status.asp', 'Sytem Setting', subMenuSystem),
	new Menu('system_man_ip.asp', 'Management', subMenuManage_nscrtvhc),
	new Menu('vlan_mode.asp', 'Switch Setting', subMenuDevice),
	new Menu('port_stat.asp', 'Port Setting', subMenuPort),
	new Menu('service_main.asp', 'Network Topology', subMenuBusiness_nscrtvhc)
);


function getMenu(env_in)
{
    if (env_in['sys.mib'] == "nscrtv-hc"){
        return menuNscrtvHC;
    }
    return menuDefault;
}

function pageInMenu(menu/* sub menu*/, page)
{
    for (var i = 0; i < menu.length; i ++){
        if (menu[i].page == page){
            return true;
        }
    }
    return false;
}


function getSubMenu(menu/*menu*/, page)
{
    for (var i = 0; i < menu.length; i ++){
        if (pageInMenu(menu[i].submenu, page)){
            return menu[i].submenu;
        }
    }
    return menu[0].submenu;
}


function pageRedirectCheck(env)
{
  if (env['sys.ipc'] == '1'){
      location = 'debug.asp';
  }else if (env['sys.vlan'] == '1'){
      location = 'do_vlan_apply.asp';         
  }
}


function is_for_nscrtv(env)
{
    if ((env['sys.mib'] == "nscrtv")
        || (env['sys.mib'] == "nscrtv-hc")
        || (env['sys.mib'] == "nscrtv-sc")){
        return true;
    }
    return false;
}

function is_for_nscrtv_hc(env)
{
    return (env['sys.mib'] == "nscrtv-hc") ? true : false;
}

function is_for_nscrtv_sc(env)
{
    return (env['sys.mib'] == "nscrtv-sc") ? true : false;
}


function env_has(env, feature)
{
		if (env['sys.feature']){
				var vs = env['sys.feature'].split(';');				
				for (var i = 0; i < vs.length; i ++){
					if (vs[i] == feature){
						return true;	
					}	
				}
		}
		return false;
}
