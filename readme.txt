
编译：

  1. 进入目录：product/access
  2. 确保rootfs的内容是正确的，如果不正确，需要解压rootfs.tgz到rootfs目录
     或者把rootfs目录删除，然后运行：make rootfs
  3. make clean;make EPN105

如果编译过程中报错，需要手动解决，一般情况下只需要增加几个文件夹：
  rm app/bin -rf
  mkdir app/bin

  mkdir app/epon_oam/bin

  mkdir app/lib


升级：

  1. 串口升级

     进入product/access/release/EPN105目录，将文件EPN105V100R006.bin拷贝到tftp根目录下，并将tftp的IP设置为192.168.2.58

     启动目标板，接好串口和网线，按CTRL + E进入boot模式，密码是EPN105，并接着按 4->2 直接升级。



//access/config/EPN105/rule.mk
image: makeapp
	cp -f $(PRODUCTDIR)/release/$(ENV_PRO)/u-boot.bin ./u-boot.bin
	cp -f $(PRODUCTDIR)/release/$(ENV_PRO)/$(CONFIG_RELEASE_VER).bin ./temp_for_app.bin
	./makeapp 101 $(CONFIG_RELEASE_VER) temp_for_app.bin u-boot.bin 0 $(ENV_PRO)
	rm -f u-boot.bin temp_for_app.bin
	mv *.img $(PRODUCTDIR)/release/$(ENV_PRO)
	rm -f makeapp.c makeapp

	if [ -d /opt/tftpboot ]; then chmod 666 $(RELEASE_DIR)/*.bin; cp -f $(RELEASE_DIR)/*.bin /opt/tftpboot; fi


re8670_rx_skb 函数需要处理管理vlan的报文。

	if((skb->vlan_tci & VLAN_VID_MASK)!=0)
	{
		skb_push_qtag(skb,skb->vlan_tci & VLAN_VID_MASK,0);
	}
	else
	{
        if(!strncmp(skb->dev->name,"eth0",strlen("eth0"))){
            printk("no puah 8021q tag!\n");
        }else{
		    skb_push_qtag(skb,s_ui_management_vlan,0);
		    skb->vlan_tci = s_ui_management_vlan;
        }
	}


//文件： product/access/kernel/linux-2.6.x/drivers/net/re8686.h
#if defined(CONFIG_VLAN_8021Q)
#define CP_VLAN_TAG_USED 0
#endif

