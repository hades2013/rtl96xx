/*
 * Copyright 2012, Realtek Semiconductor Corp.
 *
 * drivers/usb/host/rtl8672-res.c
 *
 * $Author: cathy $
 * 
 * USB resource for OHCI or EHCI hcd
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <bspchip.h>

MODULE_DESCRIPTION("Realtek RTL8672 USB platform device driver");
MODULE_LICENSE("GPL");

static void usb_release(struct device *dev)
{
	/* normally not freed */
	dev->parent = NULL;
}

#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
static struct resource rtl8672_ohci_resources[] = {
	{
		.start	= BSP_OHCI_BASE,
		.end	= BSP_OHCI_BASE + 0x00000FFF,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= BSP_USB_H_IRQ,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device rtl8672_ohci = {
	.name = "rtl8672-ohci",
	.id	= -1,
	.dev = {
		.release = usb_release,
		.dma_mask = (void *)~0,
		.coherent_dma_mask = ~0,
	},
	.num_resources = ARRAY_SIZE(rtl8672_ohci_resources),
	.resource = rtl8672_ohci_resources,
};
#endif //CONFIG_USB_OHCI_HCD || CONFIG_USB_OHCI_HCD_MODULE

#if defined(CONFIG_USB_EHCI_HCD) || defined(CONFIG_USB_EHCI_HCD_MODULE)
static struct resource rtl8672_ehci_resources[] = {
	{
		.start	= BSP_EHCI_BASE,
		.end	= BSP_EHCI_BASE + 0x0000EFFF,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= BSP_USB_H_IRQ,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device rtl8672_ehci = {
	.name = "rtl8672-ehci",
	.id	= -1,
	.dev = {
		.release = usb_release,
		.dma_mask = (void *)~0,
		.coherent_dma_mask = ~0,
	},
	.num_resources = ARRAY_SIZE(rtl8672_ehci_resources),
	.resource = rtl8672_ehci_resources,
};
#endif //CONFIG_USB_EHCI_HCD || CONFIG_USB_EHCI_HCD_MODULE

static int __init rtl8672_hcd_cs_init (void) 
{
	int retval = 0;

#if defined(CONFIG_USB_EHCI_HCD) || defined(CONFIG_USB_EHCI_HCD_MODULE)
	retval = platform_device_register(&rtl8672_ehci);
	if (retval) {
		printk("%s: fail to register rtl8672_ehci!\n", __FUNCTION__);
		return retval;
	}
	printk("%s: register rtl8672_ehci ok!\n", __FUNCTION__);
#endif //CONFIG_USB_EHCI_HCD || CONFIG_USB_EHCI_HCD_MODULE

#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
	retval = platform_device_register(&rtl8672_ohci);
	if (retval) {		
	#if defined(CONFIG_USB_EHCI_HCD) || defined(CONFIG_USB_EHCI_HCD_MODULE)
		platform_device_unregister(&rtl8672_ehci);
	#endif //CONFIG_USB_EHCI_HCD || CONFIG_USB_EHCI_HCD_MODULE
		printk("%s: fail to register rtl8672_ohci!\n", __FUNCTION__);
		return retval;
	}
	printk("%s: register rtl8672_ohci ok!\n", __FUNCTION__);
#endif //CONFIG_USB_OHCI_HCD || CONFIG_USB_OHCI_HCD_MODULE

	return retval;
}
module_init (rtl8672_hcd_cs_init);

static void __exit rtl8672_hcd_cs_exit(void)
{
#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
	platform_device_unregister(&rtl8672_ohci);
#endif //CONFIG_USB_OHCI_HCD || CONFIG_USB_OHCI_HCD_MODULE

#if defined(CONFIG_USB_EHCI_HCD) || defined(CONFIG_USB_EHCI_HCD_MODULE)
	platform_device_unregister(&rtl8672_ehci);
#endif //CONFIG_USB_EHCI_HCD || CONFIG_USB_EHCI_HCD_MODULE
	return;
}
module_exit(rtl8672_hcd_cs_exit);

