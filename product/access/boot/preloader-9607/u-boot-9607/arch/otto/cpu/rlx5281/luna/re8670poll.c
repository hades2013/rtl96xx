/* re8670pool.c - RealTek re8670 Fast Ethernet interface header */
/* History:
 *  2007/03/06  SH  Added Dcache write invalidate for CPU LX4181
 *
 */
#include <malloc.h>
#include <common.h>
#include "re8670poll.h"
#include "swCore.h"
//#include "iob.h"
#include <asm/arch/bspchip.h>

unsigned char descriptor_tx[TX_DESC_NUM*sizeof(NIC_TXFD_T)+256];
unsigned char descriptor_rx[RX_DESC_NUM*sizeof(NIC_RXFD_T)+256];

NIC_TXFD_T *pTxBDPtr;
NIC_RXFD_T *pRxBDPtr;
unsigned int	txBDhead = 0;	// index for system to release buffer
unsigned int	txBDtail = 0;	// index for system to set buf to BD
unsigned int	rxBDtail = 0;	// index for system to set buf to BD
unsigned int regbase = 0xB8012000;

int isEthDbgLv(unsigned int eth_dbg_lv){
	unsigned int current =getenv_ulong( "eth_dbg_lv", 16, 0);
	if(current & eth_dbg_lv){
		return 1;
	}
	else{
		return 0;
	}
}

#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))
#define MACReg(offset, val)		(WRITE_MEM32(SWITCH_BASE + offset, val))
static int analog_patch_flag=0;

void Lan_RXENABLE(void)
{
#if 0
	//IO_CMD |= (RE << 5);
	int flag;
	if(0 == analog_patch_flag) {
		unsigned int SOC_SUB_TYPE;

		//printf("%s %d\n", __func__, __LINE__);
		printf("6266 swcore_init\r\n");
		SOC_SUB_TYPE=(REG32(BOND_CHIP_MODE)& 0xff);
		
		do
		{
			flag = *((volatile unsigned int *)0xb8000044) & 0x2;
		} while (flag == 0);
		
		//phy patch: port set phy-reg port 4 page 0 register 0 data 0x1340
		MACReg(0x0c, 0x00001340);
		MACReg(0x10, 0x0064a400);
		  //port 0: auto neg capability 10h/10f/100h/100f flow contrl enable
		MACReg(0x0c, 0x00000de1);
		MACReg(0x10, 0x0060a408);
		MACReg(0x0c, 0x00000c00);
		MACReg(0x10, 0x0060a412);
		MACReg(0x0c, 0x00001340);
		MACReg(0x10, 0x0060a400);
		  //port 1: auto neg capability 10h/10f/100h/100f flow contrl enable
		MACReg(0x0c, 0x00000de1);
		MACReg(0x10, 0x0061a408);
		MACReg(0x0c, 0x00000c00);
		MACReg(0x10, 0x0061a412);
		MACReg(0x0c, 0x00001340);
		MACReg(0x10, 0x0061a400);
		  //port 2: auto neg capability 10h/10f/100h/100f flow contrl enable
		MACReg(0x0c, 0x00000de1);
		MACReg(0x10, 0x0062a408);
		MACReg(0x0c, 0x00000c00);
		MACReg(0x10, 0x0062a412);
		MACReg(0x0c, 0x00001340);
		MACReg(0x10, 0x0062a400);
		//port 3: auto neg capability 10h/10f/100h/100f flow contrl enable
		MACReg(0x0c, 0x00000de1);
		MACReg(0x10, 0x0063a408);
		MACReg(0x0c, 0x00000c00);
		MACReg(0x10, 0x0063a412);
		MACReg(0x0c, 0x00001340);
		MACReg(0x10, 0x0063a400);
	
		  //patch phy done
		MACReg(0x88, 1);
		  //SVLAN uplink port
		MACReg(0x23110, 0);
		  //port isolation
		MACReg(0x27000, 0x3ffffff);
		MACReg(0x27004, 0x3ffffff);
		MACReg(0x27008, 0x3ffffff);
		MACReg(0x2700c, 0x1fff);
		//cpu port force mode
		MACReg(0x120, 0x40);
		//cpu port ability
		MACReg(0xa8, 0x196);
		//meter set tick-token tick-period 53 token 58
		MACReg(0x25000, 0x1353a);
		  //meter set pon-tick-token tick-period 53 token 58
		MACReg(0x25108, 0x1353a);
		  //vlan set state disable
		MACReg(0x13108, 0x00000000);
		  //vlan set tag-mode port 0-6 keep-format
		MACReg(0x20030, 0x00000001);
		MACReg(0x20430, 0x00000001);
		MACReg(0x20830, 0x00000001);
		MACReg(0x20c30, 0x00000001);
		MACReg(0x21030, 0x00000001);
		MACReg(0x21430, 0x00000001);
		MACReg(0x21830, 0x00000001);
		  //cpu set trap-insert-tag state disable
		MACReg(0x23064, 0x00000000);
		  //MOCIR_FRC_MD, 0711
		MACReg(0x2D8F8, 0xffffffff);
		  //#MOCIR_FRC_VAL, 0711
		MACReg(0x2D8FC, 0xffffffff);

		//phy4 select
		MACReg(0x170, 0x2);
		//tcont 0 scheduling queue 0-7
		MACReg(0x23280, 0xb28);
		MACReg(0x2de44, 0x1);
		MACReg(0x2dd08, 0xff);
		MACReg(0x2de34, 0x0);
		MACReg(0x2d908, 0x0);
		MACReg(0x2d90c, 0x0);
		MACReg(0x2d910, 0x0);
		MACReg(0x2d914, 0x0);
		MACReg(0x2d918, 0x0);
		MACReg(0x2d91c, 0x0);
		MACReg(0x2d920, 0x0);
		MACReg(0x2d924, 0x0);
		MACReg(0x2db08, 0x1ffff);
		MACReg(0x2db0c, 0x1ffff);
		MACReg(0x2db10, 0x1ffff);
		MACReg(0x2db14, 0x1ffff);
		MACReg(0x2db18, 0x1ffff);
		MACReg(0x2db20, 0x1ffff);
		MACReg(0x2db24, 0x1ffff);
		MACReg(0x1c3b8, 0xffffffff);
		
		*((volatile unsigned int *)0xb8000044) = *((volatile unsigned int *)0xb8000044) | 1;

		analog_patch_flag = 1;
	}
#endif
}

void Lan_RXDISABLE(void)
{
	//IO_CMD &= ~(RE << 5);
}

int Lan_Receive(char** ppData, int* pLen)
{
	if(isEthDbgLv(ETH_DBG_RX_IN)){
		printf("%s %d\n", __func__, __LINE__);
	}
	EISR = 0xffff;  //reset RDU flag to start rx again

	if(pRxBDPtr[rxBDtail].StsLen & OWN_BIT){
		//printf("%s %d\n", __func__, __LINE__);
		return -1;
	}

	if(isEthDbgLv(ETH_DBG_RX_DESC)){
		unsigned long tmp = (unsigned long)(&pRxBDPtr[rxBDtail]);
		printf("pRxBDPtr[rxBDtail] @ 0x%lx\n", tmp);
		print_buffer(tmp, (void*)tmp, 4, sizeof(NIC_RXFD_T), 4);
	}

	*ppData = (char*)pRxBDPtr[rxBDtail].DataPtr + RX_SHIFT;
	*pLen = (pRxBDPtr[rxBDtail].StsLen & 0xfff) - 4;

	pRxBDPtr[rxBDtail].StsLen &= ~0xfff;
	pRxBDPtr[rxBDtail].StsLen |= RX_DESC_BUFFER_SIZE;

	pRxBDPtr[rxBDtail].StsLen |= OWN_BIT;
	EthrntRxCPU_Des_Num = rxBDtail;

	if(isEthDbgLv(ETH_DBG_RX_DATA)){
		unsigned long tmp = (unsigned long)(*ppData);
		printf("rx data @ 0x%lx, len %d\n", tmp, *pLen);
		print_buffer(tmp, (void*)tmp, 2, (*pLen)/2, 0);
	}
	
	rxBDtail++;
	rxBDtail %= RX_DESC_NUM;

	return 0;
}

int Lan_Transmit(void * buff, unsigned int length)
{
	if(isEthDbgLv(ETH_DBG_TX_IN)){
		printf("%s %d\n", __func__, __LINE__);
	}

	if(pTxBDPtr[txBDtail].StsLen & OWN_BIT){
		printf("%s %d\n", __func__, __LINE__);
		return -1;
	}
	
	pTxBDPtr[txBDtail].DataPtr = (unsigned int)buff | UNCACHE_MASK;
	if(length < 60)
		length = 60;
	pTxBDPtr[txBDtail].StsLen &= ~0xfff;
	pTxBDPtr[txBDtail].StsLen |= length;

	// trigger to send
	pTxBDPtr[txBDtail].StsLen |= OWN_BIT|FS_BIT|LS_BIT|(1<<23);
	//tylo, for 8672 fpga, cache write-back
	__asm__ volatile(
			"mtc0 $0,$20\n\t"
			"nop\n\t"
			"nop\n\t"
			"li $8,512\n\t"
			"mtc0 $8,$20\n\t"
			"nop\n\t"
			"nop\n\t"
			"mtc0 $0,$20\n\t"
			"nop"
			: /* no output */
			: /* no input */
			);
	if(isEthDbgLv(ETH_DBG_TX_DESC)){
		unsigned long tmp = (unsigned long)(&pTxBDPtr[txBDtail]);
		printf("pTxBDPtr[txBDtail] @ 0x%lx\n", tmp);
		print_buffer(tmp, (void*)tmp, 4, sizeof(NIC_TXFD_T), 4);
	}

	if(isEthDbgLv(ETH_DBG_TX_DATA)){
		unsigned long tmp = (unsigned long)(buff);
		printf("tx data @ 0x%lx\n", tmp);
		print_buffer(tmp, (void*)tmp, 4, length, 4);
	} 

	IO_CMD |= (1<<0);

	#if 0//really need this?
	mdelay(1);
	while(pTxBDPtr[txBDtail].StsLen & OWN_BIT)
		mdelay(1);
	#endif

	/* advance one */
	txBDtail++;
	txBDtail %= TX_DESC_NUM;

	return 0;
}

int Lan_Initialed = 0;

//11/09/05' hrchen, disable NIC
void Lan_Stop(void)
{
	//printf("%s %d\n", __func__, __LINE__);
	CR = 0x01;	 /* Reset */	
	while ( CR & 0x1 );		
	IO_CMD = 0;
	IO_CMD1 = 0;

	/* Interrupt Register, ISR, IMR */
	EIMR = 0;
	EISR = 0xffff;

	Lan_Initialed = 0;
}

void Lan_WriteMac(char* mac){
	// set MAC address
	unsigned long mac_reg0, mac_reg1;
	//mac_reg0 = (mac[0] << 24) | (mac[1] << 16) | (mac[2] << 8) | (mac[3] << 0);
	//mac_reg1 = (mac[4] << 24) | (mac[5] << 16);
	memcpy(&mac_reg0, mac, 4);
	memcpy(&mac_reg1, &mac[4], 2);
	NIC_ID0 = mac_reg0;
	NIC_ID1 = mac_reg1;	
	//printf("%s %d: 0x%x 0x%x\n", __func__, __LINE__, NIC_ID0, NIC_ID1);
}

int Lan_Initialize(char *mac)
{
	int i;

	//printf("%s %d\n", __func__, __LINE__);

	if(Lan_Initialed)
		return 0;

	//printf("%s %d\n", __func__, __LINE__);

	//initIOB();

	Lan_Stop();

	pTxBDPtr = (NIC_TXFD_T *)((((unsigned int)(descriptor_tx+0xff))& 0xffffff00)|UNCACHE_MASK);
	pRxBDPtr = (NIC_RXFD_T *)((((unsigned int)(descriptor_rx+0xff))& 0xffffff00)|UNCACHE_MASK);

	/* setup descriptor */
	RxFDP = (unsigned int)pRxBDPtr;
	RxCDO = 0;
	TxFDP1 = (unsigned int)pTxBDPtr;
	TxCDO1 = 0;	
	// init xmt BD
	for(i=0;i<TX_DESC_NUM;i++)
	{
		pTxBDPtr[i].StsLen = 0;
		pTxBDPtr[i].VLan = 0;
		pTxBDPtr[i].DataPtr = 0;
	}
	pTxBDPtr[TX_DESC_NUM-1].StsLen |= EOR_BIT;

	for(i=0;i<RX_DESC_NUM;i++)
	{
		char *pBuf;
		//if ( (pBuf = getIOB()) == 0 ) {
		if ( (pBuf = UNCACHED_MALLOC(RX_DESC_BUFFER_SIZE)) == 0 ) {
			printf("%s %d\n", __func__, __LINE__);
			return -1;
		}

		pRxBDPtr[i].StsLen = OWN_BIT + RX_DESC_BUFFER_SIZE;
		pRxBDPtr[i].VLan = 0;
		//pRxBDPtr[i].DataPtr = (unsigned long) IOB_PKT_PTR(pBuf);
		pRxBDPtr[i].DataPtr = (unsigned int)pBuf;
	}
	pRxBDPtr[RX_DESC_NUM-1].StsLen |= EOR_BIT;
#if 0
	// set MAC address
	{
		unsigned long mac_reg0, mac_reg1;
		//mac_reg0 = (mac[0] << 24) | (mac[1] << 16) | (mac[2] << 8) | (mac[3] << 0);
		//mac_reg1 = (mac[4] << 24) | (mac[5] << 16);
		memcpy(&mac_reg0, mac, 4);
		memcpy(&mac_reg1, &mac[4], 2);
		NIC_ID0 = mac_reg0;
		NIC_ID1 = mac_reg1;	
	}
#endif
	/* RCR, don't accept error packet */
	RCR = NoErrAccept;
	//	RCR = NoErrPromiscAccept;

	/* TCR: IFG, Mode */
	TCR = (unsigned long)(TCR_IFG<<TCR_IFG_OFFSET)|(TCR_NORMAL<<TCR_MODE_OFFSET);

	Rx_Pse_Des_Thres = RX_FC_THRESHOLD;

	/* Rx descriptor Size */	
	EthrntRxCPU_Des_Num = RX_DESC_NUM-1;

	//RxRingSize = 0x00;	// 16 descriptor
	RxRingSize = RX_DESC_NUM-1;	// 16 descriptor

	/* Flow Control */
	MSR &= ~(TXFCE | RXFCE);

	/* cpu tag control*/
	CPUtagCR = (CTEN_RX | 2<<CT_RSIZE_L | 3<<CT_TSIZE |CT_APPLO |CTPM_8370 | CTPV_8370);
	CPUtag1CR = 0;

	/* Ethernet IO CMD */
	IO_CMD1 = CMD_CONFIG1;
	IO_CMD = CMD_CONFIG;

#ifdef SUPPORT_MULT_UPGRADE
	MAR0 = 0xffffffff;
	MAR4 = 0xffffffff;
#endif	

	Lan_Initialed = 1;
	txBDtail = 0;	// index for system to set buf to BD
	rxBDtail = 0;	// index for system to set buf to BD
//#if 0 //move to software patch
	swCore_init();
//#endif
	return 0;
}

