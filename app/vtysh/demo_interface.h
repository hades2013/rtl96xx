/*
 * demo_interface.h
 *
 *  Created on: Oct 30, 2010
 *      Author: ryan
 */

#ifndef DEMO_INTERFACE_H_
#define DEMO_INTERFACE_H_

void demo_if_init(void);
//int get_interface_config(const char *arg, const char *cmd, switch_interface_t *ifp);
char *if_fancy_name(char *ifname);
char *if_fancy_mode(int mode);

#endif /* DEMO_INTERFACE_H_ */
