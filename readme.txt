
编译：

  1. 进入目录：product/access

  2. make EPN105

     如果执行错误，运行make clean后再执行make EPN105

     编译过程中，如果提示yaffs2 file system support (YAFFS_FS) [N/m/y/?] (NEW) 选N，
     我们不需要这个文件系统，我们用的是squashfs文件系统

  3. 如果之前已经执行过make EPN105，那么后面需要编译是只需要执行make all


如果编译过程中报错，需要手动解决，一般情况下只需要增加几个文件夹：
  rm app/bin -rf
  mkdir app/bin

  mkdir app/epon_oam/bin

  mkdir app/lib


升级：

  1. 串口升级

     进入product/access/release/EPN105目录，将文件EPN105V100R006.bin拷贝到tftp根目录下，并将tftp的IP设置为192.168.2.58

     启动目标板，接好串口和网线，按CTRL + E进入boot模式，密码是EPN105，并接着按 4->2 直接升级。


CPU侧的flash厂家：CFEON ，器件名：EN25QH64@SOP8

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



5. 在文件系统product/access/config/EPN105/rootfs.tgz中增加mtd8 mtd8ro mtdblock8

   mknod -m 755 mtd8 c 90 16 
   mknod -m 755 mtd8ro c 90 17
   mknod -m 755 mtdblock8 c 31 8


6. kernel分区大小设定：
   在product/access/config/EPN105/rule.mk文件中：

    dd bs=65536 count=18 if=$(RELEASE_DIR)/$(CONFIG_RELEASE_VER).bin of=$(RELEASE_DIR)/linux.bin

    修改成：

    dd bs=65536 count=22 if=$(RELEASE_DIR)/$(CONFIG_RELEASE_VER).bin of=$(RELEASE_DIR)/linux.bin


    这样修改后，通过页面升级后，可以正常重启。

    如果需要修改kernel分区的大小，需要同步修改这里的count，

   现在系统的分区是：
    0x00000000-0x00040000   ---boot      size=0x40000
    0x00040000-0x00050000   ---bootenv   size=0x10000
    0x00050000-0x00060000   ---expcfg    size=0x10000
    0x00060000-0x001c0000   ---kernel1   size=0x160000
    0x001c0000-0x003f0000   ---rootfs1   size=0x230000
    0x003f0000-0x00430000   ---nvram1    size=0x40000
    0x00430000-0x00470000   ---nvram2    size-0x40000
    0x00470000-0x005d0000   ---kernel2   size=0x160000
    0x005d0000-0x00800000   ---rootfs2   size=0x230000

   由以上分区，我们设定kernel的大小为0x160000，所以

   bs*count = 65536*22 = 0x160000


7. 几处定义与应用代码冲突，解决办法是增加#ifndef BOARD_EPN105
   见文件：
   rtl9607/app/include/lw_drv_req.h

       #ifndef BOARD_EPN105
       #define vlan_mode     para2_u.uiValue
       #endif

   rtl9607/app/include/lw_drv_pub.h

	#ifndef BOARD_EPN105
	#define LINK_TYPE_ACCESS 0
 	....

8. 应用程序传入kernel的phy并不是真正的phy,而是kernel定义的logic phy：

#define    CPU_PORT_NUMBER     7  //to kernel : 7,and true phy - 6
#define    ETH0_PORT_NUMBER    1  //to kernel : 1,and true phy - 0
#define    ETH1_PORT_NUMBER    3  //to kernel : 3,and true phy - 2
#define    ETH2_PORT_NUMBER    4  //to kernel : 4,and true phy - 3

#define    CLT0_PORT_NUMBER    5  //to kernel : 5,and true phy - 5

------------------------------------------------------------------
inline int toLogicPort(int phy)//phy是由kernel直接传上来的真正的phy,
{
	int portmap[] = {
		ETH0_PORT_NUMBER, // phy 0 
		0, // phy 1
        	ETH1_PORT_NUMBER, // phy 2
        	ETH2_PORT_NUMBER, // phy 3
		0, // phy 4
		CLT0_PORT_NUMBER, // phy 5
		CPU_PORT_NUMBER,  // phy 6 	
	};

	if (phy < sizeof(portmap)/sizeof(portmap[0])){
		return portmap[phy];
	}
	return 0;
}


在kernel的定义：

PORT_TYPE_S g_szPortType[LOGIC_PORT_NO+3] = {
/* 0 */    {INVALID_PORT, INVALID_PORT, 0},
/* 1 */    {TO_USERPORT(1, 0, 1),  TOPHYID(0,0),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_1000)},
/* 2 */    {TO_USERPORT(1, 0, 2),  TOPHYID(0,1),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_1000)},
/* 3 */    {TO_USERPORT(1, 0, 3),  TOPHYID(0,2),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_1000)},
/* 4 */    {TO_USERPORT(1, 0, 4),  TOPHYID(0,3),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_1000)},
/* 5 */    {TO_USERPORT(1, 0, 5),  TOPHYID(0,5),     PORT_TYPE_DEF(0,TP_CABLE, UD_DOWNLINK, SP_1000)},
/* 6 */    {TO_USERPORT(1, 0, 6),  TOPHYID(0,4),     PORT_TYPE_DEF(0,TP_COPPER, UD_DOWNLINK, SP_1000)},
/* rtl9607 CPU port is mac 6 and it's port id is port 6. */
/*LGC_CPU*/{TO_USERPORT(1, 0, 7),  TOPHYID(0,6),     PORT_TYPE_DEF(0, TP_CPU, 0, 0)}
};



9.  在rc脚本中，已经启用了br0，所以在master进程中，可以通过br0获取mac和MME报文也可以通过这个br0

	ifconfig eth0 up
	brctl addbr br0

	vconfig add eth0 1
	brctl addif br0 eth0.1
	ifconfig eth0.1 0.0.0.0
	ifconfig br0 up 192.168.0.100 
	ifconfig br0 mtu 1496


10. rtl9607/app/share/Makefile文件中增加：

    	LIB_SWAPI = libswapi.a

	SW_SRCS += $(shell find ./drv -name \*.c)
	OBJS_SW += $(SW_SRCS:.c=.o)
 
swapi:
	$(AR) -r $(LIB_SWAPI) $(OBJS_SW)



11. 增加 #define Ioctl_GetVlanPvid(_port_number, _pvid)



