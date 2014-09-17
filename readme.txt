
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



修改记录：

1. 修改文件：access/config/EPN105/rule.mk
image: makeapp
	cp -f $(PRODUCTDIR)/release/$(ENV_PRO)/u-boot.bin ./u-boot.bin
	cp -f $(PRODUCTDIR)/release/$(ENV_PRO)/$(CONFIG_RELEASE_VER).bin ./temp_for_app.bin
	./makeapp 101 $(CONFIG_RELEASE_VER) temp_for_app.bin u-boot.bin 0 $(ENV_PRO)
	rm -f u-boot.bin temp_for_app.bin
	mv *.img $(PRODUCTDIR)/release/$(ENV_PRO)
	rm -f makeapp.c makeapp

	if [ -d /opt/tftpboot ]; then chmod 666 $(RELEASE_DIR)/*.bin; cp -f $(RELEASE_DIR)/*.bin /opt/tftpboot; fi
      ---将bin文件拷贝到tftpboot目录，以便boot下载。

2. 修改管理vlan设定：
        re8670_rx_skb 函数需要处理管理vlan的报文。

	if((skb->vlan_tci & VLAN_VID_MASK)!=0)
	{
		skb_push_qtag(skb,skb->vlan_tci & VLAN_VID_MASK,0);
	}
	else
	{
		 //skb_push_qtag(skb,s_ui_management_vlan,0);
		 //skb->vlan_tci = s_ui_management_vlan;
	}


    //修改文件： product/access/kernel/linux-2.6.x/drivers/net/re8686.h
    #if defined(CONFIG_VLAN_8021Q)
    #define CP_VLAN_TAG_USED 0  //修改为0，不需要使用这个宏，需要重新定义管理vlan
    #endif



3. 对于cable来的MME 报文，需要增加rtl_tag
   头文件中定义cable所连接的PHY：
#define CLT_PORT_MAX 1  //如果需要增加cable 口，这里需要定义好
#define CLT0_PORT 5 //cable 0 port,use phy 5
#define CLT1_PORT (-1)// phy==-1 means no use it now  //这里指定cable 口的phy
#define CLT2_PORT (-1)// phy==-1 means no use it now
#define CLT3_PORT (-1)// phy==-1 means no use it now

   增加函数：product/access/kernel/linux-2.6.x/drivers/net/re8686.c
   void skb_push_cputag(struct sk_buff *pSkb, u32 phy)

   修改：product/access/kernel/linux-2.6.x/drivers/net/eth.c
   eth_type_trans 函数中增加判断0x8899


   增加定义：product/access/kernel/linux-2.6.x/include/linux/netdevice.h

  typedef struct __packed {
    u16 rtl_eth_type;    // 0x8899
    u8 protocol;
    u8 reason;
    u16 pritag;
    union{
        u16 txmask;
        struct {
            u8 rsvd;
            u8 rxport;
        };
    };
  }cpu_tag_t;/

   这个定义与master进程一致，主要目的是要知道，一个MME 报文收到时，要明确它是从哪个cable口上来的


4. 开机启动master进程和页面根目录：

在product/access/config/rc中增加定义：

	mkdir -p /etc/snmp

	echo "Preparing web pages..."

	mkdir -p /tmp/www
	if [ -f /www.tar ];then
	echo "Found www.tar"
	tar xf /www.tar -C /tmp/www
	elif [ -f /www.tar.gz ];then
	echo "Found www.tar.gz"
	tar xf /www.tar.gz -C /tmp/www
	fi
 
	
	if [ -x /usr/sbin/master ];then
	master
	fi





 
