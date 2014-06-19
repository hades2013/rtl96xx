//#ifndef REALTEK_POWER_SEQUENCE_8812
//#define REALTEK_POWER_SEQUENCE_8812

#ifdef CONFIG_RTL_8812_SUPPORT

#ifdef BIT
#undef BIT
#endif

#define BIT(x)	(1 << (x))

/* 
	Check document WB-110628-DZ-RTL8195 (Jaguar) Power Architecture-R04.pdf
	There are 6 HW Power States:
	0: POFF--Power Off
	1: PDN--Power Down
	2: CARDEMU--Card Emulation
	3: ACT--Active Mode
	4: LPS--Low Power State
	5: SUS--Suspend

	The transision from different states are defined below
	TRANS_CARDEMU_TO_ACT
	TRANS_ACT_TO_CARDEMU
	TRANS_CARDEMU_TO_SUS
	TRANS_SUS_TO_CARDEMU
	TRANS_CARDEMU_TO_PDN
	TRANS_ACT_TO_LPS
	TRANS_LPS_TO_ACT	

	TRANS_END
*/
#define	RTL8812_TRANS_CARDEMU_TO_ACT_STEPS	15
#define	RTL8812_TRANS_ACT_TO_CARDEMU_STEPS	15
#define	RTL8812_TRANS_CARDEMU_TO_SUS_STEPS	15
#define	RTL8812_TRANS_SUS_TO_CARDEMU_STEPS	15
#define	RTL8812_TRANS_CARDEMU_TO_PDN_STEPS	15
#define	RTL8812_TRANS_PDN_TO_CARDEMU_STEPS	15
#define	RTL8812_TRANS_ACT_TO_LPS_STEPS	15
#define	RTL8812_TRANS_LPS_TO_ACT_STEPS	15	
#define	RTL8812_TRANS_END_STEPS	1


#define RTL8812_TRANS_CARDEMU_TO_ACT 														\
	/* format */																\
	/* { offset, cut_msk, fab_msk|interface_msk, base|cmd, msk, value }, // comments here*/								\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(2), 0},/* disable SW LPS 0x04[10]=0*/	\
	{0x0006, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, BIT(1), BIT(1)},/* wait till 0x04[17] = 1    power ready*/	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(7), 0},/* disable HWPDN 0x04[15]=0*/ \
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(3), 0},/* disable WL suspend*/	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(0), BIT(0)},/* polling until return 0*/	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, BIT(0), 0},/**/	

#define RTL8812_TRANS_ACT_TO_CARDEMU													\
	/* format */																\
	/* { offset, cut_msk, fab_msk|interface_msk, base|cmd, msk, value }, // comments here*/								\
	{0x001F, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0},/*0x1F[7:0] = 0 turn off RF*/	\
	/*{0x004E, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT7, 0},//0x4C[23] = 0x4E[7] = 0, switch DPDT_SEL_P output from register 0x65[2] */	\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(0)|BIT(1), 0}, /* 0x02[1:0] = 0	reset BB*/	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(1), BIT(1)}, /*0x04[9] = 1 turn off MAC by HW state machine*/	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, BIT(1), 0}, /*wait till 0x04[9] = 0 polling until return 0 to disable*/	

#define RTL8812_TRANS_CARDEMU_TO_SUS													\
	/* format */								\
	/* { offset, cut_msk, fab_msk|interface_msk, base|cmd, msk, value }, // comments here*/								\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(3), BIT(3)}, /*0x04[11] = 2b'11 enable WL suspend for PCIe*/	

#define RTL8812_TRANS_SUS_TO_CARDEMU													\
	/* format */								\
	/* { offset, cut_msk, fab_msk|interface_msk, base|cmd, msk, value }, // comments here*/								\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(3), 0}, /*0x04[11] = 2b'01enable WL suspend*/


#define RTL8812_TRANS_CARDEMU_TO_CARDDIS													\
	/* format */																\
	/* { offset, cut_msk, fab_msk|interface_msk, base|cmd, msk, value }, // comments here*/								\
	/**{0x0194, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT0, 0}, //0x194[0]=0 , disable 32K clock*/	\
	/**{0x0093, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0x94}, //0x93=0x94 , 90[30] =0 enable 500k ANA clock .switch clock from 12M to 500K , 90 [26] =0 disable EEprom loader clock*/	\
	{0x0003, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(2), 0}, /*0x03[2] = 0, reset 8051*/	\
	{0x0080, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0X05}, /*0x80=05h if reload fw, fill the default value of host_CPU handshake field*/	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(3), BIT(3)}, /*0x04[11] = 2b'01 enable WL suspend*/	\
	{0x0007, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0x31}, /*0x07=0x31 , SOP option to disable BG/MB*/	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(2), BIT(2)}, /*0x04[10] = 1, enable SW LPS*/

#define RTL8812_TRANS_CARDDIS_TO_CARDEMU													\
	/* format */																\
	/* { offset, cut_msk, fab_msk|interface_msk, base|cmd, msk, value }, // comments here*/							\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(2), 0}, /*0x04[10] = 0, enable SW LPS PCIE only*/	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(3), 0}, /*0x04[11] = 2b'01enable WL suspend*/	\
	{0x0003, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(2), BIT(2)}, /*0x03[2] = 1, enable 8051*/	\
	{0x0301, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0},/*PCIe DMA start*/


#define RTL8812_TRANS_CARDEMU_TO_PDN												\
	/* format */																\
	/* { offset, cut_msk, fab_msk|interface_msk, base|cmd, msk, value }, // comments here*/	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(7), BIT(7)},/* 0x04[15] = 1*/

#define RTL8812_TRANS_PDN_TO_CARDEMU												\
	/* format */																\
	/* { offset, cut_msk, fab_msk|interface_msk, base|cmd, msk, value }, // comments here*/	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(7), 0},/* 0x04[15] = 0*/

#define RTL8812_TRANS_ACT_TO_LPS														\
	/* format */																\
	/* { offset, cut_msk, fab_msk|interface_msk, base|cmd, msk, value }, // comments here*/								\
	{0x0301, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0xFF},/*PCIe DMA stop*/	\
	{0x0522, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0x7F},/*Tx Pause*/		\
	{0x05F8, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, 0xFF, 0},/*Should be zero if no packet is transmitting*/	\
	{0x05F9, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, 0xFF, 0},/*Should be zero if no packet is transmitting*/	\
	{0x05FA, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, 0xFF, 0},/*Should be zero if no packet is transmitting*/	\
	{0x05FB, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, 0xFF, 0},/*Should be zero if no packet is transmitting*/	\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(0), 0},/*CCK and OFDM are disabled,and clock are gated*/	\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_DELAY, 0, PWRSEQ_DELAY_US},/*Delay 1us*/	\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(1), 0},/*Whole BB is reset*/			\
	{0x0100, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0x03},/*Reset MAC TRX*/			\
	{0x0101, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(1), 0},/*check if removed later*/		\
	{0x0553, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(5), BIT(5)},/*Respond TxOK to scheduler*/	


#define RTL8812_TRANS_LPS_TO_ACT															\
	/* format */																\
	/* { offset, cut_msk, fab_msk|interface_msk, base|cmd, msk, value }, // comments here*/	\
	{0x0080, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_SDIO,PWR_CMD_WRITE, 0xFF, 0x84}, /*SDIO RPWM*/	\
	{0xFE58, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0x84}, /*USB RPWM*/	\
	{0x0361, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0x84}, /*PCIe RPWM*/	\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_DELAY, 0, PWRSEQ_DELAY_MS}, /*Delay*/	\
	{0x0008, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(4), 0}, /*.	0x08[4] = 0		 switch TSF to 40M*/	\
	{0x0109, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, BIT(7), 0}, /*Polling 0x109[7]=0  TSF in 40M*/			\
	{0x0029, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(6)|BIT(7), 0}, /*.	0x29[7:6] = 2b'00	 enable BB clock*/	\
	{0x0101, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(1), BIT(1)}, /*.	0x101[1] = 1*/					\
	{0x0100, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0xFF}, /*.	0x100[7:0] = 0xFF	 enable WMAC TRX*/	\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, BIT(1)|BIT(0), BIT(1)|BIT(0)}, /*.	0x02[1:0] = 2b'11	 enable BB macro*/	\
	{0x0522, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0}, /*.	0x522 = 0*/

#define RTL8812_TRANS_END																\
	/* format */																\
	/* { offset, cut_msk, fab_msk|interface_msk, base|cmd, msk, value }, // comments here*/		\
	{0xFFFF, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,0,PWR_CMD_END, 0, 0}, //


extern WLAN_PWR_CFG rtl8812_power_on_flow[RTL8812_TRANS_CARDEMU_TO_ACT_STEPS+RTL8812_TRANS_END_STEPS];
extern WLAN_PWR_CFG rtl8812_radio_off_flow[RTL8812_TRANS_ACT_TO_CARDEMU_STEPS+RTL8812_TRANS_END_STEPS];
extern WLAN_PWR_CFG rtl8812_card_disable_flow[RTL8812_TRANS_ACT_TO_CARDEMU_STEPS+RTL8812_TRANS_CARDEMU_TO_PDN_STEPS+RTL8812_TRANS_END_STEPS];
extern WLAN_PWR_CFG rtl8812_card_enable_flow[RTL8812_TRANS_ACT_TO_CARDEMU_STEPS+RTL8812_TRANS_CARDEMU_TO_PDN_STEPS+RTL8812_TRANS_END_STEPS];
extern WLAN_PWR_CFG rtl8812_suspend_flow[RTL8812_TRANS_ACT_TO_CARDEMU_STEPS+RTL8812_TRANS_CARDEMU_TO_SUS_STEPS+RTL8812_TRANS_END_STEPS];
extern WLAN_PWR_CFG rtl8812_resume_flow[RTL8812_TRANS_ACT_TO_CARDEMU_STEPS+RTL8812_TRANS_CARDEMU_TO_SUS_STEPS+RTL8812_TRANS_END_STEPS];
extern WLAN_PWR_CFG rtl8812_hwpdn_flow[RTL8812_TRANS_ACT_TO_CARDEMU_STEPS+RTL8812_TRANS_CARDEMU_TO_PDN_STEPS+RTL8812_TRANS_END_STEPS];
extern WLAN_PWR_CFG rtl8812_enter_lps_flow[RTL8812_TRANS_ACT_TO_LPS_STEPS+RTL8812_TRANS_END_STEPS];
extern WLAN_PWR_CFG rtl8812_leave_lps_flow[RTL8812_TRANS_LPS_TO_ACT_STEPS+RTL8812_TRANS_END_STEPS]; 

#endif //CONFIG_RTL_8812_SUPPORT

