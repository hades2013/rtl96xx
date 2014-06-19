/*
 * vlan_utils.h
 *
 *  Created on: Mar 18, 2011
 *      Author: root
 */

#ifndef VLAN_UTILS_H_
#define VLAN_UTILS_H_

//#include "switch.h"

#define sizeof_vlan_group(c) ((int)(((switch_vlan_group_t*)0)->vlans) + (c) * sizeof(switch_vlan_t))

uint32_t sizeof_vlan_interface(switch_vlan_group_t *vlan_groups, int count);
int is_vlan_range(switch_vlan_t *vlan);

switch_vlan_group_t *vlan_group_to_next(switch_vlan_group_t *vg);
switch_vlan_interface_t *vlan_interface_to_next(switch_vlan_interface_t *vi);
uint32_t vlan_group_mem_size(switch_vlan_group_t *vg, uint16_t count);
uint32_t vlan_interface_mem_size(switch_vlan_interface_t *vi, uint16_t count);

int vlan_group_sprintf(char *buffer, switch_vlan_group_t *vlan_group);
int get_group_vlans(switch_vlan_group_t *group, switch_vlan_t *vlans, int size);
int get_group_vlans_hash(switch_vlan_group_t *group, switch_vlan_t *vlans, int size);
int all_vlan_in_groups(switch_vlan_group_t *vg, switch_vlan_group_t *vgs, int vgs_count);
int any_vlan_in_groups(switch_vlan_group_t *vg, switch_vlan_group_t *vgs, int vgs_count);
int vlan_id_in_groups( int vlanid, switch_vlan_group_t *vgs, int vgs_count);

switch_vlan_group_t *vlan_group_new_from_string_simple(const char* vstr);
switch_vlan_group_t *vlan_group_new_from_string(const char* vstr);
switch_vlan_group_t *vlan_group_new_from_hash(switch_vlan_t *vlans, int count);

switch_vlan_interface_t *vlan_interface_new(char *name, int mode, switch_vlan_group_t *vlan0, switch_vlan_group_t * vlan1);
void vlan_interface_free(switch_vlan_interface_t *vlanif);

//int clt_ifmode_int2str(int mode);
//int clt_ifmode_str2int(char *str);

int speed_atoi(char *spd);
char *speed_itoa(int spd);	

char *switch_vlan_string(switch_vlan_t *vlan);
char *switch_vlan_string_h(switch_vlan_t *vlan);
int switchport_atoi(const char *mode);
char *switchport_itoa(int mode);
//void switch_mibs2stats(uint64_t *mibs, switch_interface_stats_t *stats);

//void switch_port_mibs_dump(uint64_t *mibs);
//void switch_port_stats_dump(switch_interface_stats_t *stats);



#endif /* VLAN_UTILS_H_ */
