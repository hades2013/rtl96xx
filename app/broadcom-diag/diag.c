/*
 * diag.c - GPIO interface driver for Broadcom boards
 *
 * Copyright (C) 2006 Mike Baker <mbm@openwrt.org>,
 * Copyright (C) 2006-2007 Felix Fietkau <nbd@openwrt.org>
 * Copyright (C) 2008 Andy Boyett <agb@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/kmod.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <linux/workqueue.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/kobject.h>
#include <net/sock.h>
extern u64 uevent_next_seqnum(void);

#include "gpio.h"
#include "diag.h"
#define getvar(str) (nvram_get(str)?:"")

static inline int startswith (char *source, char *cmp) { return !strncmp(source,cmp,strlen(cmp)); }
static int fill_event(struct event_t *);
static unsigned int gpiomask = 0;
module_param(gpiomask, int, 0644);

enum {
	CLT502,
};

static void __init clt502_init(void)
{
	return;
}


static struct platform_t __initdata platforms[] = {
	[CLT502] = {
		.name		= "Hexicom CLT502",
		.buttons	= {
			{ .name = "reset",	.gpio = 1 << 3 },
		},
		.leds		= {
			{ .name = "diag",	.gpio = 1 << 2 },
		},
		.platform_init = clt502_init,
	},
};

static struct platform_t __init *platform_detect(void)
{
	return &platforms[CLT502];
}

static void register_buttons(struct button_t *b)
{
	for (; b->name; b++)
		platform.button_mask |= b->gpio;

	platform.button_mask &= ~gpiomask;

	gpio_outen(platform.button_mask, 0);
	gpio_control(platform.button_mask, 0);
	platform.button_polarity = gpio_in() & platform.button_mask;
	gpio_intpolarity(platform.button_mask, platform.button_polarity);
	gpio_setintmask(platform.button_mask, platform.button_mask);

	gpio_set_irqenable(1, button_handler);
}

static void unregister_buttons(struct button_t *b)
{
	gpio_setintmask(platform.button_mask, 0);

	gpio_set_irqenable(0, button_handler);
}


static void add_msg(struct event_t *event, char *msg, int argv)
{
	char *s;

	if (argv)
		return;

	s = skb_put(event->skb, strlen(msg) + 1);
	strcpy(s, msg);
}

static void hotplug_button(struct work_struct *work)
{
	struct event_t *event = container_of(work, struct event_t, wq);
	char *s;

	event->skb = alloc_skb(2048, GFP_KERNEL);

	s = skb_put(event->skb, strlen(event->action) + 2);
	sprintf(s, "%s@", event->action);
	fill_event(event);

	NETLINK_CB(event->skb).dst_group = 1;
	broadcast_uevent(event->skb, 0, 1, GFP_KERNEL);

	kfree(event);
}


static int fill_event (struct event_t *event)
{
	static char buf[128];

	add_msg(event, "HOME=/", 0);
	add_msg(event, "PATH=/sbin:/bin:/usr/sbin:/usr/bin", 0);
	add_msg(event, "SUBSYSTEM=button", 0);
	snprintf(buf, 128, "ACTION=%s", event->action);
	add_msg(event, buf, 0);
	snprintf(buf, 128, "BUTTON=%s", event->name);
	add_msg(event, buf, 0);
	snprintf(buf, 128, "SEEN=%ld", event->seen);
	add_msg(event, buf, 0);
	snprintf(buf, 128, "SEQNUM=%llu", uevent_next_seqnum());
	add_msg(event, buf, 0);

	return 0;
}


static irqreturn_t button_handler(int irq, void *dev_id)
{
	struct button_t *b;
	u32 in, changed;

	in = gpio_in() & platform.button_mask;
	gpio_intpolarity(platform.button_mask, in);
	changed = platform.button_polarity ^ in;
	platform.button_polarity = in;

	changed &= ~gpio_outen(0, 0);

	for (b = platform.buttons; b->name; b++) {
		struct event_t *event;

		if (!(b->gpio & changed)) continue;

		b->pressed ^= 1;

		if ((event = (struct event_t *)kzalloc (sizeof(struct event_t), GFP_ATOMIC))) {
			event->seen = (jiffies - b->seen)/HZ;
			event->name = b->name;
			event->action = b->pressed ? "pressed" : "released";
			INIT_WORK(&event->wq, (void *)(void *)hotplug_button);
			schedule_work(&event->wq);
		}

		b->seen = jiffies;
	}
	return IRQ_HANDLED;
}

static void register_leds(struct led_t *l)
{
	struct proc_dir_entry *p;
	u32 mask = 0;
	u32 oe_mask = 0;
	u32 val = 0;

	leds = proc_mkdir("led", diag);
	if (!leds)
		return;

	for(; l->name; l++) {
		if (l->gpio & gpiomask)
			continue;

		if (l->gpio & GPIO_TYPE_EXTIF) {
			l->state = 0;
			set_led_extif(l);
		} else {
			if (l->polarity != INPUT) oe_mask |= l->gpio;
			mask |= l->gpio;
			val |= (l->polarity == NORMAL)?0:l->gpio;
		}

		if (l->polarity == INPUT) continue;

		if ((p = create_proc_entry(l->name, S_IRUSR, leds))) {
			l->proc.type = PROC_LED;
			l->proc.ptr = l;
			p->data = (void *) &l->proc;
			p->proc_fops = &diag_proc_fops;
		}
	}

	gpio_outen(mask, oe_mask);
	gpio_control(mask, 0);
	gpio_out(mask, val);
	gpio_setintmask(mask, 0);
}

static void unregister_leds(struct led_t *l)
{
	for(; l->name; l++)
		remove_proc_entry(l->name, leds);

	remove_proc_entry("led", diag);
}

static void set_led_extif(struct led_t *led)
{
	gpio_set_extif(led->gpio, led->state);
}

static void led_flash(unsigned long dummy) {
	struct led_t *l;
	u32 mask = 0;
	u8 extif_blink = 0;

	for (l = platform.leds; l->name; l++) {
		if (l->flash) {
			if (l->gpio & GPIO_TYPE_EXTIF) {
				extif_blink = 1;
				l->state = !l->state;
				set_led_extif(l);
			} else {
				mask |= l->gpio;
			}
		}
	}

	mask &= ~gpiomask;
	if (mask) {
		u32 val = ~gpio_in();

		gpio_outen(mask, mask);
		gpio_control(mask, 0);
		gpio_out(mask, val);
	}
	if (mask || extif_blink) {
		mod_timer(&led_timer, jiffies + FLASH_TIME);
	}
}

static ssize_t diag_proc_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	struct proc_dir_entry *dent = PDE(file->f_dentry->d_inode);
	char *page;
	int len = 0;

	if ((page = kmalloc(1024, GFP_KERNEL)) == NULL)
		return -ENOBUFS;

	if (dent->data != NULL) {
		struct prochandler_t *handler = (struct prochandler_t *) dent->data;
		switch (handler->type) {
			case PROC_LED: {
				struct led_t * led = (struct led_t *) handler->ptr;
				if (led->flash) {
					len = sprintf(page, "f\n");
				} else {
					if (led->gpio & GPIO_TYPE_EXTIF) {
						len = sprintf(page, "%d\n", led->state);
					} else {
						u32 in = (gpio_in() & led->gpio ? 1 : 0);
						u8 p = (led->polarity == NORMAL ? 0 : 1);
						len = sprintf(page, "%d\n", ((in ^ p) ? 1 : 0));
					}
				}
				break;
			}
			case PROC_MODEL:
				len = sprintf(page, "%s\n", platform.name);
				break;
			case PROC_GPIOMASK:
				len = sprintf(page, "0x%04x\n", gpiomask);
				break;
		}
	}
	len += 1;

	if (*ppos < len) {
		len = min_t(int, len - *ppos, count);
		if (copy_to_user(buf, (page + *ppos), len)) {
			kfree(page);
			return -EFAULT;
		}
		*ppos += len;
	} else {
		len = 0;
	}

	kfree(page);
	return len;
}


#define	EOC_RST_GPIO	(1 << 0)
static void eoc_do_reset(unsigned long dummy)
{
	if(dummy == 1) {
		gpio_outen(EOC_RST_GPIO, EOC_RST_GPIO);
		gpio_control(EOC_RST_GPIO, 0);
		gpio_out(EOC_RST_GPIO, 0);
		mod_timer(&eoc_reset_timer, jiffies + EOC_RST_TIME);
	}
	else {
		gpio_outen(EOC_RST_GPIO, EOC_RST_GPIO);
		gpio_control(EOC_RST_GPIO, 0);
		gpio_out(EOC_RST_GPIO, 1);
	}
}


static ssize_t diag_proc_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	struct proc_dir_entry *dent = PDE(file->f_dentry->d_inode);
	char *page;
	int ret = -EINVAL;

	if ((page = kmalloc(count + 1, GFP_KERNEL)) == NULL)
		return -ENOBUFS;

	if (copy_from_user(page, buf, count)) {
		kfree(page);
		return -EINVAL;
	}
	page[count] = 0;

	if (dent->data != NULL) {
		struct prochandler_t *handler = (struct prochandler_t *) dent->data;
		switch (handler->type) {
			case PROC_LED: {
				struct led_t *led = (struct led_t *) handler->ptr;
				int p = (led->polarity == NORMAL ? 0 : 1);

				if (page[0] == 'f') {
					led->flash = 1;
					led_flash(0);
				} else {
					led->flash = 0;
					if (led->gpio & GPIO_TYPE_EXTIF) {
						led->state = p ^ ((page[0] == '1') ? 1 : 0);
						set_led_extif(led);
					} else {
						gpio_outen(led->gpio, led->gpio);
						gpio_control(led->gpio, 0);
						gpio_out(led->gpio, ((p ^ (page[0] == '1')) ? led->gpio : 0));
					}
				}
				break;
			}
			case PROC_GPIOMASK:
				gpiomask = simple_strtoul(page, NULL, 0);

				if (platform.buttons) {
					unregister_buttons(platform.buttons);
					register_buttons(platform.buttons);
				}

				if (platform.leds) {
					unregister_leds(platform.leds);
					register_leds(platform.leds);
				}
				break;
			case PROC_EOC_RESET:
				if(page[0] == '1') {
					eoc_do_reset(1);
				}
				break;
		}
		ret = count;
	}

	kfree(page);
	return ret;
}

static int __init diag_init(void)
{
	static struct proc_dir_entry *p;
	static struct platform_t *detected;

	detected = platform_detect();
	if (!detected) {
		printk(MODULE_NAME ": Router model not detected.\n");
		return -ENODEV;
	}
	memcpy(&platform, detected, sizeof(struct platform_t));

	printk(MODULE_NAME ": Detected '%s'\n", platform.name);
	if (platform.platform_init != NULL) {
		platform.platform_init();
	}

	if (!(diag = proc_mkdir("diag", NULL))) {
		printk(MODULE_NAME ": proc_mkdir on /proc/diag failed\n");
		return -EINVAL;
	}

	if ((p = create_proc_entry("model", S_IRUSR, diag))) {
		p->data = (void *) &proc_model;
		p->proc_fops = &diag_proc_fops;
	}

	if ((p = create_proc_entry("gpiomask", S_IRUSR | S_IWUSR, diag))) {
		p->data = (void *) &proc_gpiomask;
		p->proc_fops = &diag_proc_fops;
	}

	if (platform.buttons)
		register_buttons(platform.buttons);

	if (platform.leds)
		register_leds(platform.leds);

	if ((p = create_proc_entry("eoc_reset", S_IRUSR | S_IWUSR, diag))) {
		p->data = (void *) &proc_eoc_reset;
		p->proc_fops = &diag_proc_fops;
	}

	return 0;
}

static void __exit diag_exit(void)
{
	del_timer(&led_timer);

	if (platform.buttons)
		unregister_buttons(platform.buttons);

	if (platform.leds)
		unregister_leds(platform.leds);

	remove_proc_entry("model", diag);
	remove_proc_entry("gpiomask", diag);
	remove_proc_entry("diag", NULL);
	remove_proc_entry("eoc_reset", diag);
}

module_init(diag_init);
module_exit(diag_exit);

MODULE_AUTHOR("Mike Baker, Felix Fietkau / OpenWrt.org");
MODULE_LICENSE("GPL");
