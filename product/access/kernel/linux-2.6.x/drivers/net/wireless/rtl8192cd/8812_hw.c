#ifdef CONFIG_RTL_8812_SUPPORT
#ifndef _8812_HW_C_
#define _8812_HW_C_

#include "8192cd.h"
#include "8192cd_cfg.h"
#include "8192cd_util.h"

#include "8192c_reg.h"
#include "8812_reg.h"
#include "8812_vht_gen.h"

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif
//eric_8812 #include "8192cd_debug.h"
#include "8192cd_headers.h"

#define MAX_NUM_80M 2

unsigned char available_channel_8812_80m[MAX_NUM_80M][4] = 
{
	{36, 40, 44, 48},
	{52, 56, 60, 64},
};

void UpdateBBRFVal8812(struct rtl8192cd_priv *priv, unsigned char channel)
{
	unsigned char current_is_5g = 0, switch_bw = 0;
	unsigned char bTmp = 0;
	unsigned int dwTmp = 0;
	unsigned int eRFPath, curMaxRFPath;
	
	curMaxRFPath = RF92CD_PATH_MAX;
	
	//check CCK_CHECK_en BIT7
	if( RTL_R8(0x454) & BIT(7) )
		current_is_5g = 0;
	else
		current_is_5g = 1;

	if(((channel > 14) && (current_is_5g == 0)) || ((channel <= 14) && (current_is_5g == 1)))
		switch_bw = 1;

	//printk("\n\n eric-current_is_5g = %d switch_bw = %d pre= %d\n\n", current_is_5g, switch_bw, priv->pshare->pre_channel);

	if(channel > 14 && (switch_bw || (priv->pshare->pre_channel == 0))) //2.4G to 5G
		RTL_W8(0x454, (RTL_R8(0x454) & ~(BIT(7))));

	//Set fc_area 0x860
	dwTmp = RTL_R32(0x860);
	dwTmp &= ~(BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28));
	
	if(channel<=14)
		dwTmp |= (0x96a << 17);
	else if(channel<=48)
		dwTmp |= (0x494 << 17);
	else if(channel<=64)
		dwTmp |= (0x453 << 17);
	else if(channel<=116)
		dwTmp |= (0x452 << 17);
	else if(channel<=165)
		dwTmp |= (0x412 << 17);

	RTL_W32(0x860, dwTmp);

	//Set RF MOD AG rf_0x18
	for(eRFPath = RF92CD_PATH_A; eRFPath < curMaxRFPath; eRFPath++) {
		dwTmp = PHY_QueryRFReg(priv, eRFPath, rRfChannel, bMask20Bits, 1);
		dwTmp &= ~(BIT(8)|BIT(9)|BIT(16)|BIT(17)|BIT(18));

		if(channel<=14)
			dwTmp |= 0x0;
		else if(channel<=64)
			dwTmp |= (BIT(8)|BIT(16));
		else if(channel<=140)
			dwTmp |= (BIT(8)|BIT(16)|BIT(17));
		else
			dwTmp |= (BIT(8)|BIT(16)|BIT(18));

		PHY_SetRFReg(priv, eRFPath, rRfChannel, bMask20Bits, dwTmp);
	}

	//Set BB registers 
	if(switch_bw)
	{
		if(!current_is_5g)
		{
			bTmp = RTL_R8(0x82c);
			bTmp &= ~(BIT(0) | BIT(1));
			bTmp |= BIT(0);
			RTL_W8(0x82c, bTmp);

			RTL_W32(0xcb8, RTL_R32(0xcb8)|BIT(12));
			RTL_W32(0xeb8, RTL_R32(0xcb8)|BIT(12));
			RTL_W32(0xcb8, RTL_R32(0xcb8)& (~BIT(15)));
			RTL_W32(0xeb8, RTL_R32(0xeb8)& (~BIT(15)));
			RTL_W32(0x808, RTL_R32(0x808)& (~BIT(28)));
			RTL_W32(0x8c0, RTL_R32(0x8c0)& (~BIT(17)));
		}
		else
		{
			bTmp = RTL_R8(0x82c);
			bTmp &= ~(BIT(0) | BIT(1));
			RTL_W8(0x82c, bTmp);

			RTL_W32(0xcb8, RTL_R32(0xcb8)& (~BIT(12)));
			RTL_W32(0xeb8, RTL_R32(0xcb8)& (~BIT(12)));
			RTL_W32(0xcb8, RTL_R32(0xcb8)|BIT(15));
			RTL_W32(0xeb8, RTL_R32(0xeb8)|BIT(15));
			RTL_W32(0x808, RTL_R32(0x808)|BIT(28));
			RTL_W32(0x8c0, RTL_R32(0x8c0)|BIT(17));
		}
	}
	else if(priv->pshare->pre_channel == 0) //first launch
	{
		if(channel > 14)
		{
			bTmp = RTL_R8(0x82c);
			bTmp &= ~(BIT(0) | BIT(1));
			bTmp |= BIT(0);
			RTL_W8(0x82c, bTmp);

			RTL_W32(0xcb8, RTL_R32(0xcb8)|BIT(12));
			RTL_W32(0xeb8, RTL_R32(0xcb8)|BIT(12));
			RTL_W32(0xcb8, RTL_R32(0xcb8)& (~BIT(15)));
			RTL_W32(0xeb8, RTL_R32(0xeb8)& (~BIT(15)));
			RTL_W32(0x808, RTL_R32(0x808)& (~BIT(28)));
			RTL_W32(0x8c0, RTL_R32(0x8c0)& (~BIT(17)));
		}
		else
		{
			bTmp = RTL_R8(0x82c);
			bTmp &= ~(BIT(0) | BIT(1));
			RTL_W8(0x82c, bTmp);

			RTL_W32(0xcb8, RTL_R32(0xcb8)& (~BIT(12)));
			RTL_W32(0xeb8, RTL_R32(0xcb8)& (~BIT(12)));
			RTL_W32(0xcb8, RTL_R32(0xcb8)|BIT(15));
			RTL_W32(0xeb8, RTL_R32(0xeb8)|BIT(15));
			RTL_W32(0x808, RTL_R32(0x808)|BIT(28));
			RTL_W32(0x8c0, RTL_R32(0x8c0)|BIT(17));
		}
	}

	if(channel <= 14 && (switch_bw || (priv->pshare->pre_channel == 0)))
		RTL_W8(0x454, (RTL_R8(0x454) | BIT(7)));

	priv->pshare->pre_channel = channel; 

}

signed char convert_diff(signed char value)
{
	// range from -8 ~ 7
	if(value <= 7)
		return value;
	else
		return (value - 16); 
}

void Write_OFDM_A(struct rtl8192cd_priv *priv, unsigned int writeVal)
{
	RTL_W32(rTxAGC_A_Ofdm18_Ofdm6_JAguar, writeVal);
	RTL_W32(rTxAGC_A_Ofdm18_Ofdm6_JAguar, writeVal);
}

void Write_1S_A(struct rtl8192cd_priv *priv, unsigned int writeVal)
{
	RTL_W32(rTxAGC_A_MCS3_MCS0_JAguar, writeVal);
	RTL_W32(rTxAGC_A_MCS7_MCS4_JAguar, writeVal);
	RTL_W32(rTxAGC_A_Nss1Index3_Nss1Index0_JAguar, writeVal);
	RTL_W32(rTxAGC_A_Nss1Index7_Nss1Index4_JAguar, writeVal);
	RTL_W32(rTxAGC_A_Nss2Index1_Nss1Index8_JAguar, writeVal);
}

void Write_2S_A(struct rtl8192cd_priv *priv, unsigned int writeVal)
{
	RTL_W32(rTxAGC_A_MCS11_MCS8_JAguar, writeVal);
	RTL_W32(rTxAGC_A_MCS15_MCS12_JAguar, writeVal);
	RTL_W32(rTxAGC_A_Nss2Index5_Nss2Index2_JAguar, writeVal);
	RTL_W32(rTxAGC_A_Nss2Index9_Nss2Index6_JAguar, writeVal);
	writeVal = (writeVal & 0xffff0000) | (RTL_R32(rTxAGC_A_Nss2Index1_Nss1Index8_JAguar)& 0xffff);
	RTL_W32(rTxAGC_A_Nss2Index1_Nss1Index8_JAguar, writeVal);
}

void Write_OFDM_B(struct rtl8192cd_priv *priv, unsigned int writeVal)
{
	RTL_W32(rTxAGC_B_Ofdm18_Ofdm6_JAguar, writeVal);
	RTL_W32(rTxAGC_B_Ofdm18_Ofdm6_JAguar, writeVal);
}


void Write_1S_B(struct rtl8192cd_priv *priv, unsigned int writeVal)
{
	RTL_W32(rTxAGC_B_MCS3_MCS0_JAguar, writeVal);
	RTL_W32(rTxAGC_B_MCS7_MCS4_JAguar, writeVal);
	RTL_W32(rTxAGC_B_Nss1Index3_Nss1Index0_JAguar, writeVal);
	RTL_W32(rTxAGC_B_Nss1Index7_Nss1Index4_JAguar, writeVal);
	RTL_W32(rTxAGC_B_Nss2Index1_Nss1Index8_JAguar, writeVal);
}

void Write_2S_B(struct rtl8192cd_priv *priv, unsigned int writeVal)
{
	RTL_W32(rTxAGC_B_MCS11_MCS8_JAguar, writeVal);
	RTL_W32(rTxAGC_B_MCS15_MCS12_JAguar, writeVal);
	RTL_W32(rTxAGC_B_Nss2Index5_Nss2Index2_JAguar, writeVal);
	RTL_W32(rTxAGC_B_Nss2Index9_Nss2Index6_JAguar, writeVal);
	writeVal = (writeVal & 0xffff0000) | (RTL_R32(rTxAGC_A_Nss2Index1_Nss1Index8_JAguar)& 0xffff);
	RTL_W32(rTxAGC_B_Nss2Index1_Nss1Index8_JAguar, writeVal);
}

void use_DefaultOFDMTxPower_8812(struct rtl8192cd_priv *priv)
{

		unsigned int def_power = 0x20202020;//0x12121212;

		//printk("NO Calibration data, use default OFDM power = 0x%x\n", def_power);
		
		RTL_W32(rTxAGC_A_Ofdm18_Ofdm6_JAguar, def_power);
		RTL_W32(rTxAGC_A_Ofdm54_Ofdm24_JAguar, def_power);
		RTL_W32(rTxAGC_A_MCS3_MCS0_JAguar, def_power);
		RTL_W32(rTxAGC_A_MCS7_MCS4_JAguar, def_power);
		RTL_W32(rTxAGC_A_MCS11_MCS8_JAguar, def_power);
		RTL_W32(rTxAGC_A_MCS15_MCS12_JAguar, def_power);
		RTL_W32(rTxAGC_A_Nss1Index3_Nss1Index0_JAguar, def_power);
		RTL_W32(rTxAGC_A_Nss1Index7_Nss1Index4_JAguar, def_power);
		RTL_W32(rTxAGC_A_Nss2Index1_Nss1Index8_JAguar, def_power);
		RTL_W32(rTxAGC_A_Nss2Index5_Nss2Index2_JAguar, def_power);
		RTL_W32(rTxAGC_A_Nss2Index9_Nss2Index6_JAguar, def_power);
	
		RTL_W32(rTxAGC_B_Ofdm18_Ofdm6_JAguar, def_power);
		RTL_W32(rTxAGC_B_Ofdm54_Ofdm24_JAguar, def_power);
		RTL_W32(rTxAGC_B_MCS3_MCS0_JAguar, def_power);
		RTL_W32(rTxAGC_B_MCS7_MCS4_JAguar, def_power);
		RTL_W32(rTxAGC_B_MCS11_MCS8_JAguar, def_power);
		RTL_W32(rTxAGC_B_MCS15_MCS12_JAguar, def_power);
		RTL_W32(rTxAGC_B_Nss1Index3_Nss1Index0_JAguar, def_power);
		RTL_W32(rTxAGC_B_Nss1Index7_Nss1Index4_JAguar, def_power);
		RTL_W32(rTxAGC_B_Nss2Index1_Nss1Index8_JAguar, def_power);
		RTL_W32(rTxAGC_B_Nss2Index5_Nss2Index2_JAguar, def_power);
		RTL_W32(rTxAGC_B_Nss2Index9_Nss2Index6_JAguar, def_power);

}


void Cal_OFDMTxPower_5G(struct rtl8192cd_priv *priv, unsigned char ch_idx)
{
	unsigned char tmp_TPI = 0;
	unsigned char pwr_40_1s = 0;
	signed char diff_ofdm_1t = 0; 
	signed char diff_bw40_2s = 0;
	signed char diff_bw20_1s = 0;
	signed char diff_bw20_2s = 0;
	signed char diff_bw80_1s = 0;
	signed char diff_bw80_2s = 0;

	unsigned int  writeVal = 0;

	if((priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_A[ch_idx]==0)
		||(priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_B[ch_idx]==0))
	{
		use_DefaultOFDMTxPower_8812(priv);
		return;
	}

	// PATH A, OFDM
	pwr_40_1s = priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_A[ch_idx];
	diff_ofdm_1t = (priv->pmib->dot11RFEntry.pwrdiff_5G_20BW1S_OFDM1T_A[ch_idx] & 0x0f);
	diff_ofdm_1t = convert_diff(diff_ofdm_1t);
	tmp_TPI = pwr_40_1s + diff_ofdm_1t;
	writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
	Write_OFDM_A(priv, writeVal);
	
	//printk("Write_OFDM_A %d = %d + %d \n", tmp_TPI, pwr_40_1s , diff_ofdm_1t);


	// PATH B, OFDM
	pwr_40_1s = priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_B[ch_idx];
	diff_ofdm_1t = (priv->pmib->dot11RFEntry.pwrdiff_5G_20BW1S_OFDM1T_B[ch_idx] & 0x0f);
	diff_ofdm_1t = convert_diff(diff_ofdm_1t);
	tmp_TPI = pwr_40_1s + diff_ofdm_1t;
	writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
	Write_OFDM_B(priv, writeVal);

	//printk("Write_OFDM_A %d = %d + %d \n", tmp_TPI, pwr_40_1s , diff_ofdm_1t);
	
	
	if(priv->pmib->dot11nConfigEntry.dot11nUse40M == 0)
	{	
		//PATH A, BW20-1S
		pwr_40_1s = priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_A[ch_idx];
		diff_bw20_1s = ((priv->pmib->dot11RFEntry.pwrdiff_5G_20BW1S_OFDM1T_A[ch_idx] & 0xf0) >> 4);
		diff_bw20_1s = convert_diff(diff_bw20_1s);
		tmp_TPI = pwr_40_1s + diff_bw20_1s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_1S_A(priv, writeVal);

		//printk("Write_1S_A %d = %d + %d \n", tmp_TPI, pwr_40_1s , diff_bw20_1s);


		//PATH A, BW20-2S
		diff_bw20_2s = (priv->pmib->dot11RFEntry.pwrdiff_5G_40BW2S_20BW2S_A[ch_idx] & 0x0f);
		diff_bw20_2s = convert_diff(diff_bw20_2s);
		tmp_TPI = pwr_40_1s + diff_bw20_1s + diff_bw20_2s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_2S_A(priv, writeVal);

		//printk("Write_2S_A %d = %d + %d + %d\n", tmp_TPI, pwr_40_1s , diff_bw20_1s, diff_bw20_2s);

		// ==== //
		
		//PATH B, BW20-1S
		pwr_40_1s = priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_B[ch_idx];
		diff_bw20_1s = ((priv->pmib->dot11RFEntry.pwrdiff_5G_20BW1S_OFDM1T_B[ch_idx] & 0xf0) >> 4);
		diff_bw20_1s = convert_diff(diff_bw20_1s);
		tmp_TPI = pwr_40_1s + diff_bw20_1s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_1S_B(priv, writeVal);

		//printk("Write_1S_B %d = %d + %d \n", tmp_TPI, pwr_40_1s , diff_bw20_1s);

		//PATH B, BW20-2S
		diff_bw20_2s = (priv->pmib->dot11RFEntry.pwrdiff_5G_40BW2S_20BW2S_B[ch_idx] & 0x0f);
		diff_bw20_2s = convert_diff(diff_bw20_2s);
		tmp_TPI = pwr_40_1s + diff_bw20_1s + diff_bw20_2s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_2S_B(priv, writeVal);

		//printk("Write_2S_B %d = %d + %d + %d\n", tmp_TPI, pwr_40_1s , diff_bw20_1s, diff_bw20_2s);
		
	}
	else if(priv->pmib->dot11nConfigEntry.dot11nUse40M == 1)
	{
		//PATH A, BW40-1S
		pwr_40_1s = priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_A[ch_idx];
		tmp_TPI = pwr_40_1s ;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_1S_A(priv, writeVal);

		//printk("Write_1S_A %d = %d \n", tmp_TPI, pwr_40_1s);


		//PATH A, BW40-2S
		diff_bw40_2s = ((priv->pmib->dot11RFEntry.pwrdiff_5G_40BW2S_20BW2S_A[ch_idx] & 0xf0) >> 4);
		diff_bw40_2s = convert_diff(diff_bw40_2s);
		tmp_TPI = pwr_40_1s + diff_bw40_2s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_2S_A(priv, writeVal);

		//printk("Write_2S_A %d = %d + %d \n", tmp_TPI, pwr_40_1s , diff_bw40_2s);

		// ==== //
		
		//PATH B, BW40-1S
		pwr_40_1s = priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_B[ch_idx];
		tmp_TPI = pwr_40_1s ;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_1S_B(priv, writeVal);

		//printk("Write_1S_B %d = %d \n", tmp_TPI, pwr_40_1s);


		//PATH A, BW40-2S
		diff_bw40_2s = ((priv->pmib->dot11RFEntry.pwrdiff_5G_40BW2S_20BW2S_B[ch_idx] & 0xf0) >> 4);
		diff_bw40_2s = convert_diff(diff_bw40_2s);
		tmp_TPI = pwr_40_1s + diff_bw40_2s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_2S_B(priv, writeVal);

		//printk("Write_2S_B %d = %d + %d \n", tmp_TPI, pwr_40_1s , diff_bw40_2s);
		
	}
	else if(priv->pmib->dot11nConfigEntry.dot11nUse40M == 2)
	{
		//PATH A, BW80-1S
		pwr_40_1s = (priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_A[ch_idx-4]+ 
						priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_A[ch_idx+4]) / 2  ;
		diff_bw80_1s = ((priv->pmib->dot11RFEntry.pwrdiff_5G_80BW1S_160BW1S_A[ch_idx] & 0xf0) >> 4);
		diff_bw80_1s = convert_diff(diff_bw80_1s);
		tmp_TPI = pwr_40_1s + diff_bw80_1s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_1S_A(priv, writeVal);

		//printk("Write_1S_A %d = %d + %d \n", tmp_TPI, pwr_40_1s , diff_bw80_1s);


		//PATH A, BW80-2S
		diff_bw80_2s = ((priv->pmib->dot11RFEntry.pwrdiff_5G_80BW2S_160BW2S_A[ch_idx] & 0xf0) >> 4);
		diff_bw80_2s = convert_diff(diff_bw80_2s);
		tmp_TPI = pwr_40_1s + diff_bw80_1s + diff_bw80_1s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_2S_A(priv, writeVal);

		//printk("Write_2S_A %d = %d + %d + %d \n", tmp_TPI, pwr_40_1s , diff_bw80_1s, diff_bw80_1s);


		//PATH B, BW80-1S
		pwr_40_1s = (priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_B[ch_idx-4]+ 
						priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_B[ch_idx+4]) / 2  ;
		diff_bw80_1s = ((priv->pmib->dot11RFEntry.pwrdiff_5G_80BW1S_160BW1S_B[ch_idx] & 0xf0) >> 4);
		diff_bw80_1s = convert_diff(diff_bw80_1s);
		tmp_TPI = pwr_40_1s + diff_bw80_1s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_1S_B(priv, writeVal);

		//printk("Write_1S_B %d = %d + %d \n", tmp_TPI, pwr_40_1s , diff_bw80_1s);


		//PATH B, BW80-2S
		diff_bw80_2s = ((priv->pmib->dot11RFEntry.pwrdiff_5G_80BW2S_160BW2S_B[ch_idx] & 0xf0) >> 4);
		diff_bw80_2s = convert_diff(diff_bw80_2s);
		tmp_TPI = pwr_40_1s + diff_bw80_1s + diff_bw80_1s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_2S_B(priv, writeVal);

		//printk("Write_2S_B %d = %d + %d + %d \n", tmp_TPI, pwr_40_1s , diff_bw80_1s, diff_bw80_1s);

	}
}

void Cal_OFDMTxPower_2G(struct rtl8192cd_priv *priv, unsigned char ch_idx)
{
	unsigned char tmp_TPI = 0;
	unsigned char pwr_40_1s = 0;
	signed char diff_ofdm_1t = 0; 
	signed char diff_bw40_2s = 0;
	signed char diff_bw20_1s = 0;
	signed char diff_bw20_2s = 0;

	unsigned int  writeVal = 0;

	if((priv->pmib->dot11RFEntry.pwrlevelHT40_1S_A[ch_idx]==0)
		||(priv->pmib->dot11RFEntry.pwrlevelHT40_1S_B[ch_idx]==0))
	{
		use_DefaultOFDMTxPower_8812(priv);
		return;
	}

	// PATH A, OFDM
	pwr_40_1s = priv->pmib->dot11RFEntry.pwrlevelHT40_1S_A[ch_idx];
	diff_ofdm_1t = (priv->pmib->dot11RFEntry.pwrdiff_20BW1S_OFDM1T_A[ch_idx] & 0x0f);
	diff_ofdm_1t = convert_diff(diff_ofdm_1t);
	tmp_TPI = pwr_40_1s + diff_ofdm_1t;
	writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
	Write_OFDM_A(priv, writeVal);


	// PATH B, OFDM
	pwr_40_1s = priv->pmib->dot11RFEntry.pwrlevelHT40_1S_B[ch_idx];
	diff_ofdm_1t = (priv->pmib->dot11RFEntry.pwrdiff_20BW1S_OFDM1T_B[ch_idx] & 0x0f);
	diff_ofdm_1t = convert_diff(diff_ofdm_1t);
	tmp_TPI = pwr_40_1s + diff_ofdm_1t;
	writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
	Write_OFDM_B(priv, writeVal);
	
	if(priv->pmib->dot11nConfigEntry.dot11nUse40M == 0)
	{	
		//PATH A, BW20-1S
		pwr_40_1s = priv->pmib->dot11RFEntry.pwrlevelHT40_1S_A[ch_idx];
		diff_bw20_1s = ((priv->pmib->dot11RFEntry.pwrdiff_20BW1S_OFDM1T_A[ch_idx] & 0xf0) >> 4);
		diff_bw20_1s = convert_diff(diff_bw20_1s);
		tmp_TPI = pwr_40_1s + diff_bw20_1s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_1S_A(priv, writeVal);


		//PATH A, BW20-2S
		diff_bw20_2s = (priv->pmib->dot11RFEntry.pwrdiff_40BW2S_20BW2S_A[ch_idx] & 0x0f);
		diff_bw20_2s = convert_diff(diff_bw20_2s);
		tmp_TPI = pwr_40_1s + diff_bw20_1s + diff_bw20_2s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_2S_A(priv, writeVal);

		// ==== //
		
		//PATH B, BW20-1S
		pwr_40_1s = priv->pmib->dot11RFEntry.pwrlevelHT40_1S_B[ch_idx];
		diff_bw20_1s = ((priv->pmib->dot11RFEntry.pwrdiff_20BW1S_OFDM1T_B[ch_idx] & 0xf0) >> 4);
		diff_bw20_1s = convert_diff(diff_bw20_1s);
		tmp_TPI = pwr_40_1s + diff_bw20_1s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_1S_B(priv, writeVal);

		//PATH B, BW20-2S
		diff_bw20_2s = (priv->pmib->dot11RFEntry.pwrdiff_40BW2S_20BW2S_B[ch_idx] & 0x0f);
		diff_bw20_2s = convert_diff(diff_bw20_2s);
		tmp_TPI = pwr_40_1s + diff_bw20_1s + diff_bw20_2s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_2S_B(priv, writeVal);
		
	}
	else if(priv->pmib->dot11nConfigEntry.dot11nUse40M == 1)
	{
		//PATH A, BW40-1S
		pwr_40_1s = priv->pmib->dot11RFEntry.pwrlevelHT40_1S_A[ch_idx];
		tmp_TPI = pwr_40_1s ;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_1S_A(priv, writeVal);


		//PATH A, BW40-2S
		diff_bw40_2s = ((priv->pmib->dot11RFEntry.pwrdiff_40BW2S_20BW2S_A[ch_idx] & 0xf0) >> 4);
		diff_bw40_2s = convert_diff(diff_bw40_2s);
		tmp_TPI = pwr_40_1s + diff_bw40_2s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_2S_A(priv, writeVal);

		// ==== //
		
		//PATH B, BW40-1S
		pwr_40_1s = priv->pmib->dot11RFEntry.pwrlevelHT40_1S_B[ch_idx];
		tmp_TPI = pwr_40_1s ;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_1S_B(priv, writeVal);


		//PATH A, BW40-2S
		diff_bw40_2s = ((priv->pmib->dot11RFEntry.pwrdiff_40BW2S_20BW2S_B[ch_idx] & 0xf0) >> 4);
		diff_bw40_2s = convert_diff(diff_bw40_2s);
		tmp_TPI = pwr_40_1s + diff_bw40_2s;
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		Write_2S_B(priv, writeVal);
		
	}
	
}


void PHY_SetOFDMTxPower_8812(struct rtl8192cd_priv *priv, unsigned char channel)
{
	unsigned char ch_idx = 0;
	unsigned char tmp_TPI = 0;
	unsigned char phy_band = 0;

	if(channel > 0)
		ch_idx = (channel - 1);
	else
	{
		printk("Error Channel !!\n");
		return;
	}

	if(channel > 14)
		phy_band = PHY_BAND_5G;
	else
		phy_band = PHY_BAND_2G;

#if 0
	printk("pwrlevel5GHT40_1S_A[%d]= %d \n", channel, priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_A[ch_idx]);
	printk("pwrdiff_5G_20BW1S_OFDM1T_A[%d]= 0x%x \n", channel, priv->pmib->dot11RFEntry.pwrdiff_5G_20BW1S_OFDM1T_A[ch_idx]);
	printk("pwrdiff_40BW2S_20BW2S_A[%d]= 0x%x \n", channel, priv->pmib->dot11RFEntry.pwrdiff_40BW2S_20BW2S_A[ch_idx]);
	printk("pwrdiff_5G_20BW1S_OFDM1T_A[%d]= 0x%x \n", channel, priv->pmib->dot11RFEntry.pwrdiff_5G_20BW1S_OFDM1T_A[ch_idx]);
	printk("pwrdiff_5G_40BW2S_20BW2S_A[%d]= 0x%x \n", channel, priv->pmib->dot11RFEntry.pwrdiff_5G_40BW2S_20BW2S_A[ch_idx]);
	printk("pwrdiff_5G_80BW1S_160BW1S_A[%d]= 0x%x \n", channel, priv->pmib->dot11RFEntry.pwrdiff_5G_80BW1S_160BW1S_A[ch_idx]);
	printk("pwrdiff_5G_80BW2S_160BW2S_A[%d]= 0x%x \n", channel, priv->pmib->dot11RFEntry.pwrdiff_5G_80BW2S_160BW2S_A[ch_idx]);

	printk("pwrlevel5GHT40_1S_B[%d]= %d \n", channel, priv->pmib->dot11RFEntry.pwrlevel5GHT40_1S_B[ch_idx]);
	printk("pwrdiff_5G_20BW1S_OFDM1T_B[%d]= 0x%x \n", channel, priv->pmib->dot11RFEntry.pwrdiff_5G_20BW1S_OFDM1T_B[ch_idx]);
	printk("pwrdiff_40BW2S_20BW2S_B[%d]= 0x%x \n", channel, priv->pmib->dot11RFEntry.pwrdiff_40BW2S_20BW2S_B[ch_idx]);
	printk("pwrdiff_5G_20BW1S_OFDM1T_B[%d]= 0x%x \n", channel, priv->pmib->dot11RFEntry.pwrdiff_5G_20BW1S_OFDM1T_B[ch_idx]);
	printk("pwrdiff_5G_40BW2S_20BW2S_B[%d]= 0x%x \n", channel, priv->pmib->dot11RFEntry.pwrdiff_5G_40BW2S_20BW2S_B[ch_idx]);
	printk("pwrdiff_5G_80BW1S_160BW1S_B[%d]= 0x%x \n", channel, priv->pmib->dot11RFEntry.pwrdiff_5G_80BW1S_160BW1S_B[ch_idx]);
	printk("pwrdiff_5G_80BW2S_160BW2S_B[%d]= 0x%x \n", channel, priv->pmib->dot11RFEntry.pwrdiff_5G_80BW2S_160BW2S_B[ch_idx]);
#endif

	if(phy_band == PHY_BAND_5G)
		Cal_OFDMTxPower_5G(priv, ch_idx);
	else if(phy_band == PHY_BAND_2G)
		Cal_OFDMTxPower_2G(priv, ch_idx);
	
}

void PHY_SetCCKTxPower_8812(struct rtl8192cd_priv *priv, unsigned char channel)
{
	unsigned int def_power = 0x20202020;//0x12121212;
	unsigned char ch_idx = 0;
	unsigned char tmp_TPI = 0;
	unsigned char phy_band = 0;
	unsigned int  writeVal = 0;

	if(channel > 0)
		ch_idx = (channel - 1);
	else
	{
		printk("Error Channel !!\n");
		return;
	}

	if(channel > 14)
		phy_band = PHY_BAND_5G;
	else
		phy_band = PHY_BAND_2G;

	if((priv->pmib->dot11RFEntry.pwrlevelCCK_A[ch_idx]==0)
		||(priv->pmib->dot11RFEntry.pwrlevelCCK_B[ch_idx]==0))
	{
		//printk("NO Calibration data, use default CCK power = 0x%x\n", def_power);
		RTL_W32(rTxAGC_A_CCK11_CCK1_JAguar, def_power);
		RTL_W32(rTxAGC_B_CCK11_CCK1_JAguar, def_power);
	}

	if(phy_band == PHY_BAND_2G)
	{
		//printk("pwrlevelCCK_A[%d]= %d \n", ch_idx, priv->pmib->dot11RFEntry.pwrlevelCCK_A[ch_idx]);
		//printk("pwrlevelCCK_B[%d]= %d \n", ch_idx, priv->pmib->dot11RFEntry.pwrlevelCCK_B[ch_idx]);
	
		//PATH A
		tmp_TPI = priv->pmib->dot11RFEntry.pwrlevelCCK_A[ch_idx];
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		RTL_W32(rTxAGC_A_CCK11_CCK1_JAguar, writeVal);

		//PATH B
		tmp_TPI = priv->pmib->dot11RFEntry.pwrlevelCCK_B[ch_idx];
		writeVal = (tmp_TPI << 24) | (tmp_TPI << 16) | (tmp_TPI << 8) | tmp_TPI;
		RTL_W32(rTxAGC_B_CCK11_CCK1_JAguar, writeVal);
	}

}


void get_txsc_8812(struct rtl8192cd_priv *priv, unsigned char channel)
{
	unsigned char tmp, i, found =0;

	if(priv->pshare->CurrentChannelBW == HT_CHANNEL_WIDTH_80)
	{		
		for(tmp = 0; tmp <MAX_NUM_80M; tmp ++)
		{
			for(i = 0; i<4; i++)
			{
				if(channel == available_channel_8812_80m[tmp][i])
				{
					found = 1;
					//printk("found channel[%d] at [%d][%d]\n", channel, tmp, i);
					break;
				}
			}
			
			if(found)
				break;
		}

		switch(i)
		{	
			case 0:
				//printk("case 0 \n");
				priv->pshare->txsc_20 = _20_B_40_B;
				priv->pshare->txsc_40 = _40_B;
				priv->pshare->offset_2nd_chan = HT_2NDCH_OFFSET_ABOVE;
				break;
			case 1:
				priv->pshare->txsc_20 = _20_A_40_B;
				priv->pshare->txsc_40 = _40_B;
				priv->pshare->offset_2nd_chan = HT_2NDCH_OFFSET_BELOW;
				break;		
			case 2:
				priv->pshare->txsc_20 = _20_B_40_A;
				priv->pshare->txsc_40 = _40_A;
				priv->pshare->offset_2nd_chan = HT_2NDCH_OFFSET_ABOVE;
				break;		
			case 3:
				priv->pshare->txsc_20 = _20_A_40_A;
				priv->pshare->txsc_40 = _40_A;
				priv->pshare->offset_2nd_chan = HT_2NDCH_OFFSET_BELOW;
				break;
			default:
				break;
		}
	}
	else if(priv->pshare->CurrentChannelBW == HT_CHANNEL_WIDTH_20_40)
	{
		if(priv->pshare->offset_2nd_chan == HT_2NDCH_OFFSET_BELOW)
			priv->pshare->txsc_20 = 1;
		else
			priv->pshare->txsc_20 = 2;

		priv->pshare->txsc_40 = 0;
	}
	else if(priv->pshare->CurrentChannelBW == HT_CHANNEL_WIDTH_20)
	{
		priv->pshare->txsc_20 = 0;
		priv->pshare->txsc_40 = 0;
	}

	//printk("get_txsc_8812= %d %d \n", priv->pshare->txsc_20, priv->pshare->txsc_40);

}



// Firmware

#define MAX_PAGE_SIZE				4096	// @ page : 4k bytes
#define FW_START_ADDRESS			0x1000
#define FWDL_ChkSum_rpt				BIT(2)

extern unsigned char *data_rtl8812fw_start, *data_rtl8812fw_end;


VOID
_8051Reset8812(
	struct rtl8192cd_priv *priv
)
{
	u1Byte	u1bTmp;
	u1bTmp = RTL_R8(REG_SYS_FUNC_EN_8812+1);
	RTL_W8(REG_SYS_FUNC_EN_8812+1, u1bTmp&(~BIT2));
	RTL_W8( REG_SYS_FUNC_EN_8812+1, u1bTmp|(BIT2));
//	RT_TRACE(COMP_INIT, DBG_LOUD, ("=====> _8051Reset8812(): 8051 reset success .\n"));
}


VOID
_FWDownloadEnable_8812(
	struct rtl8192cd_priv *priv,
		BOOLEAN			enable
	)
{
	u1Byte	tmp;

	if(enable)
	{
		// MCU firmware download enable.
		RTL_W8( REG_MCUFWDL_8812, 0x05);	

		// Clear Rom DL enable
		tmp = RTL_R8( REG_MCUFWDL_8812+2);
		RTL_W8( REG_MCUFWDL_8812+2, tmp&0xf7);
	}
	else
	{
		// MCU firmware download enable.
		tmp = RTL_R8( REG_MCUFWDL_8812);
		RTL_W8( REG_MCUFWDL_8812, tmp&0xfe);
	}
}

VOID
_FillDummy_8812(
	pu1Byte		pFwBuf,
	pu4Byte		pFwLen
	)
{
	u4Byte	FwLen = *pFwLen;
	u1Byte	remain = (u1Byte)(FwLen%4);
	remain = (remain==0)?0:(4-remain);

	while(remain>0)
	{
		pFwBuf[FwLen] = 0;
		FwLen++;
		remain--;
	}

	*pFwLen = FwLen;
}



// BlockWrite:
// 92DU----------use 64-Byte/8-Byte/1-Byte (PlatformIOWriteNByte)
// 92CU/8723U----use 4-Byte/1-Byte  (PlatformIOWriteNByte)
// PCI/SDIO------use 4-Byte/1-Byte  (PlatformEFIOWrite4Byte)
// 92CU [MacOS]-- use 196-Byte/8-Byte/1-Byte  (PlatformIOWriteNByte)

VOID
_BlockWrite_8812(
	struct rtl8192cd_priv 	*priv,
	IN		PVOID			buffer,
	IN		u4Byte			buffSize
	)
{
	u4Byte			blockSize_p1 = 4;	// (Default) Phase #1 : PCI muse use 4-byte write to download FW
	u4Byte			blockSize_p2 = 8;	// Phase #2 : Use 8-byte, if Phase#1 use big size to write FW.
	u4Byte			blockSize_p3 = 1;	// Phase #3 : Use 1-byte, the remnant of FW image.
	u4Byte			blockCount_p1 = 0, blockCount_p2 = 0, blockCount_p3 = 0;
	u4Byte			remainSize_p1 = 0, remainSize_p2 = 0;
	pu1Byte			bufferPtr	= (pu1Byte)buffer;
	u4Byte			i=0, offset=0;


	//3 Phase #1
	blockCount_p1 = buffSize / blockSize_p1;
	remainSize_p1 = buffSize % blockSize_p1;


	for(i = 0 ; i < blockCount_p1 ; i++){
		RTL_W32( (FW_START_ADDRESS + i * blockSize_p1), cpu_to_le32(*((pu4Byte)(bufferPtr + i * blockSize_p1))));
	}

	//3 Phase #2
	if(remainSize_p1){
		offset = blockCount_p1 * blockSize_p1;

		blockCount_p2=remainSize_p1/blockSize_p2;
		remainSize_p2=remainSize_p1%blockSize_p2;

	}
	
	//3 Phase #3
	if(remainSize_p2)
	{
		offset=(blockCount_p1 * blockSize_p1)+(blockCount_p2*blockSize_p2);		
		blockCount_p3 = remainSize_p2 /blockSize_p3;

//		RT_TRACE(COMP_INIT,DBG_LOUD,("_BlockWrite_8812[P3]  ::buffSize_p3( %d) blockSize_p3( %d) blockCount_p3( %d) \n",(buffSize-offset),blockSize_p3, blockCount_p3));	
		 
		for(i = 0 ; i < blockCount_p3 ; i++){
			RTL_W8( (FW_START_ADDRESS + offset + i), *(bufferPtr +offset+ i));
		}	
	}
}


VOID
_PageWrite_8812(
	struct rtl8192cd_priv 	*priv,
	IN		u4Byte			page,
	IN		PVOID			buffer,
	IN		u4Byte			size
	)
{
	u1Byte value8;
	u1Byte u8Page = (u1Byte) (page & 0x07) ;

	value8 = (RTL_R8(REG_MCUFWDL_8812+2)& 0xF8 ) | u8Page ;
	RTL_W8(REG_MCUFWDL_8812+2,value8);

	_BlockWrite_8812(priv,buffer,size);
}


VOID
_WriteFW_8812(
	struct rtl8192cd_priv *priv,
	IN		PVOID			buffer,
	IN		u4Byte			size
	)
{
	// Since we need dynamic decide method of dwonload fw, so we call this function to get chip version.
	// We can remove _ReadChipVersion from ReadAdapterInfo8192C later.
	u4Byte 			pageNums,remainSize ;
	u4Byte 			page,offset;
	pu1Byte			bufferPtr = (pu1Byte)buffer;

#if 1//DEV_BUS_TYPE==RT_PCI_INTERFACE
	// 20100120 Joseph: Add for 88CE normal chip. 
	// Fill in zero to make firmware image to dword alignment.
//	_FillDummy_8812(bufferPtr, &size);
#endif
	
	pageNums = size / MAX_PAGE_SIZE ;		

//	RT_ASSERT((pageNums <= 8), ("Page numbers should not greater then 8 \n"));	
	
	remainSize = size % MAX_PAGE_SIZE;		
	
	for(page = 0; page < pageNums;  page++){
		offset = page *MAX_PAGE_SIZE;
		_PageWrite_8812(priv,page, (bufferPtr+offset),MAX_PAGE_SIZE);			
	}
	if(remainSize){
		offset = pageNums *MAX_PAGE_SIZE;
		page = pageNums;
		_PageWrite_8812(priv,page, (bufferPtr+offset),remainSize);
	}	
//	RT_TRACE(COMP_INIT, DBG_LOUD, ("_WriteFW_8812 Done- for Normal chip.\n"));
}


RT_STATUS
_FWFreeToGo8812(
	struct rtl8192cd_priv *priv

	)
{
	u4Byte	counter = 0;
	u4Byte	value32;

	// polling CheckSum report
	do{
		value32 = RTL_R32( REG_MCUFWDL_8812);
	}while((counter ++ < 6000) && (!(value32 & FWDL_ChkSum_rpt  )));	

	if(counter >= 6000){	
//		RT_TRACE(COMP_INIT, DBG_SERIOUS, ("_FWFreeToGo8812:: chksum report faill ! REG_MCUFWDL:0x%08x .\n",value32));		
		return RT_STATUS_FAILURE;
	}
//	RT_TRACE(COMP_INIT, DBG_LOUD, ("_FWFreeToGo8812:: Checksum report OK ! REG_MCUFWDL:0x%08x .\n",value32));

	value32 = RTL_R32( REG_MCUFWDL_8812);
	value32 |= MCUFWDL_RDY;
	value32 &= ~WINTINI_RDY;
	RTL_W32( REG_MCUFWDL_8812, value32);

	_8051Reset8812(priv);
	
	// polling for FW ready
	counter = 0;
	do
	{
		if(RTL_R32( REG_MCUFWDL_8812) & WINTINI_RDY){
//			RT_TRACE(COMP_INIT, DBG_SERIOUS, ("Polling FW ready success!! REG_MCUFWDL:0x%08x in %d times.\n",PlatformEFIORead4Byte(Adapter, REG_MCUFWDL_8812),counter));
			return RT_STATUS_SUCCESS;
		}
//		PlatformStallExecution(5);
		delay_us(5);
	}while(counter++ < 6000);

	panic_printk("Polling FW ready fail!! REG_MCUFWDL:0x%08x .\n",RTL_R32( REG_MCUFWDL_8812) );
	return RT_STATUS_FAILURE;
}


RT_STATUS
FirmwareDownload8812(
	struct rtl8192cd_priv *priv
)
{	
	RT_STATUS		rtStatus = RT_STATUS_SUCCESS;	

	u4Byte			FwImageLen=0;
	u1Byte			*pFirmwareBuf;
	u4Byte			FirmwareLen;

	RTL_W8( 0xf0, (RTL_R8(0xf0)& ~BIT(7)));

	pFirmwareBuf = data_rtl8812fw_start+32;
	FirmwareLen = data_rtl8812fw_end - data_rtl8812fw_start-32;

// panic_printk("%s, %d%x, %x\n", __FUNCTION__, __LINE__, pFirmwareBuf, FirmwareLen);


	{
		if(RTL_R8( REG_MCUFWDL_8812)&BIT7) //8051 RAM code
		{	
			RTL_W8(REG_MCUFWDL_8812, 0x00);
			_8051Reset8812(priv);
		}
	}



	_FWDownloadEnable_8812(priv, TRUE);

	_WriteFW_8812(priv, pFirmwareBuf,FirmwareLen);

	_FWDownloadEnable_8812(priv, FALSE);


	rtStatus = _FWFreeToGo8812(priv);	
	
	if(RT_STATUS_SUCCESS != rtStatus){
		panic_printk("Firmware is not ready to run!\n") ;		
		goto Exit;
	}
	

Exit:

	panic_printk(" <=== FirmwareDownload8812()\n");
	return rtStatus;

}


// 8812 H2C

BOOLEAN
CheckFwReadLastH2C_8812(
	struct rtl8192cd_priv *priv,
	IN	u1Byte			BoxNum
)
{
	u1Byte	valHMETFR;
	BOOLEAN	Result = FALSE;
	
	valHMETFR = RTL_R8( REG_HMETFR_8812);

	// Do not seperate to 91C and 88C, we use the same setting. Suggested by SD4 Filen. 2009.12.03.
	if(((valHMETFR>>BoxNum)&BIT0) == 0)
		Result = TRUE;
		
	return Result;
}


u1Byte
FillH2CCmd8812(
	struct rtl8192cd_priv *priv,
	IN	u1Byte 		ElementID,
	IN	u4Byte 		CmdLen,
	IN	pu1Byte		pCmdBuffer
	)
{

	u1Byte 	ioStatus = 0;
	u1Byte	BoxNum;
	u2Byte	BOXReg=0, BOXExtReg=0;
	u1Byte	U1btmp; //Read 0x1bf
	BOOLEAN bFwReadClear=FALSE;
	u1Byte	BufIndex=0;
	u1Byte	WaitH2cLimmit=0;
	u1Byte	BoxContent[4], BoxExtContent[4];
	u1Byte	idx=0;

	if(!GET_ROOT(priv)->bFWReady)
		return 1;

	// 1. Find the last BOX number which has been writen.
	BoxNum = priv->pshare->fw_q_fifo_count;	//pHalData->LastHMEBoxNum;
	switch(BoxNum)
	{
		case 0:
			BOXReg = REG_HMEBOX_0_8812;
			BOXExtReg = REG_HMEBOX_EXT0_8812;
			break;
		case 1:
			BOXReg = REG_HMEBOX_1_8812;
			BOXExtReg = REG_HMEBOX_EXT1_8812;
			break;
		case 2:
			BOXReg = REG_HMEBOX_2_8812;
			BOXExtReg = REG_HMEBOX_EXT2_8812;
			break;
		case 3:
			BOXReg = REG_HMEBOX_3_8812;
			BOXExtReg = REG_HMEBOX_EXT3_8812;
			break;
		default:
			break;
	}

	// 2. Check if the box content is empty.
	while(!bFwReadClear)
	{
		bFwReadClear = CheckFwReadLastH2C_8812(priv, BoxNum);
		if(WaitH2cLimmit++ >= 100)
		{
			ioStatus = 1;
			return ioStatus;
		}
		else if(!bFwReadClear)
		{	
			delay_us(10); //us
		}
	}

	// 4. Fill the H2C cmd into box 	
	memset(BoxContent, 0, sizeof(BoxContent));
	memset(BoxExtContent, 0, sizeof(BoxExtContent));
	
	BoxContent[0] = ElementID; // Fill element ID
//	RTPRINT(FFW, FW_MSG_H2C_CONTENT, ("[FW], Write ElementID BOXReg(%4x) = %2x \n", BOXReg, ElementID));

	switch(CmdLen)
	{
	case 1:
	case 2:
	case 3:
	{
		//BoxContent[0] &= ~(BIT7);
		memcpy((pu1Byte)(BoxContent)+1, pCmdBuffer+BufIndex, CmdLen);
		//For Endian Free.
		for(idx= 0; idx < 4; idx++)
		{
			RTL_W8(BOXReg+idx, BoxContent[idx]);
		}
		break;
	}
	case 4: 
	case 5:
	case 6:
	case 7:
	{
		//BoxContent[0] |= (BIT7);
		memcpy((pu1Byte)(BoxExtContent), pCmdBuffer+BufIndex+3, (CmdLen-3));
		memcpy((pu1Byte)(BoxContent)+1, pCmdBuffer+BufIndex, 3);
		//For Endian Free.
		for(idx = 0 ; idx < 4 ; idx ++)
		{
			RTL_W8( BOXExtReg+idx, BoxExtContent[idx]);
		}		
		for(idx = 0 ; idx < 4 ; idx ++)
		{
			RTL_W8( BOXReg+idx, BoxContent[idx]);
		}
		break;
	}
	
	default:
//		RTPRINT(FFW, FW_MSG_H2C_STATE, ("[FW], Invalid command len=%d!!!\n", CmdLen));
		ioStatus = 2;
		return ioStatus;
		break;
	}

	if (++priv->pshare->fw_q_fifo_count > 3)
		priv->pshare->fw_q_fifo_count = 0;

//	RTPRINT(FFW, FW_MSG_H2C_CONTENT, ("[FW], pHalData->LastHMEBoxNum = %d\n", pHalData->LastHMEBoxNum));
	return ioStatus;
}







VOID
SetBcnCtrlReg_8812(
	struct rtl8192cd_priv *priv,
	IN	u1Byte		SetBits,
	IN	u1Byte		ClearBits
	)
{
	u1Byte tmp = RTL_R8(REG_BCN_CTRL_8812);

	tmp |=  SetBits;
	tmp &= ~ClearBits;

	RTL_W8(REG_BCN_CTRL_8812, tmp);
}


u1Byte
MRateIdxToARFRId8812(
	struct rtl8192cd_priv *priv,
	u1Byte			RateIdx,
	u1Byte			RfType
)
{
	u1Byte Ret = 0;
	
	switch(RateIdx){

	case RATR_INX_WIRELESS_NGB:
		if(RfType == MIMO_1T1R)
			Ret = 1;
		else 
			Ret = 0;
		break;

	case RATR_INX_WIRELESS_N:
	case RATR_INX_WIRELESS_NG:
		if(RfType == MIMO_1T1R)
			Ret = 5;
		else
			Ret = 4;
		break;

	case RATR_INX_WIRELESS_NB:
		if(RfType == MIMO_1T1R)
			Ret = 3;
		else 
			Ret = 2;
		break;

	case RATR_INX_WIRELESS_GB:
		Ret = 6;
		break;

	case RATR_INX_WIRELESS_G:
		Ret = 7;
		break;	

	case RATR_INX_WIRELESS_B:
		Ret = 8;
		break;

	case RATR_INX_WIRELESS_MC:
		if (!(priv->pmib->dot11BssType.net_work_type & WIRELESS_11A))
			Ret = 6;
		else
			Ret = 7;
		break;
	case RATR_INX_WIRELESS_AC_N:
		if(RfType == MIMO_1T1R)
			Ret = 10;
		else
			Ret = 9;
		break;

	default:
		Ret = 0;
		break;
	}	

	return Ret;
}

u1Byte
Get_RA_BW(
	BOOLEAN 	bCurTxBW80MHz, 
	BOOLEAN		bCurTxBW40MHz
)
{
	u1Byte	BW = 0;
	if(bCurTxBW80MHz)
		BW = 2;
	else if(bCurTxBW40MHz)
		BW = 1;
	else
		BW = 0;

	return BW;
}




typedef enum _WIRELESS_MODE {
	WIRELESS_MODE_UNKNOWN = 0x00,
	WIRELESS_MODE_A = 0x01,
	WIRELESS_MODE_B = 0x02,
	WIRELESS_MODE_G = 0x04,
	WIRELESS_MODE_AUTO = 0x08,
	WIRELESS_MODE_N_24G = 0x10,
	WIRELESS_MODE_N_5G = 0x20,
	WIRELESS_MODE_AC_5G = 0x40
} WIRELESS_MODE;

#define MIMO_PS_STATIC				0
#define MIMO_PS_DYNAMIC			1
#define MIMO_PS_NOLIMIT			3

#define FillOctetString(_os,_octet,_len)		\
	(_os).Octet=(pu1Byte)(_octet);			\
	(_os).Length=(_len);


u1Byte
Get_VHT_ENI(
	u4Byte			IOTAction,
	u1Byte			WirelessMode,
	u4Byte			ratr_bitmap 
	)
{
	u1Byte Ret = 0;
	if(WirelessMode < WIRELESS_MODE_N_24G)
		Ret =  0;
	else if(WirelessMode == WIRELESS_MODE_N_24G || WirelessMode == WIRELESS_MODE_N_5G)
	{

//if(IOTAction == HT_IOT_VHT_HT_MIX_MODE)
		{
			if(ratr_bitmap & BIT20)	// Mix , 2SS
				Ret = 3;
			else 					// Mix, 1SS
				Ret = 2;
		}

	}
	else if(WirelessMode == WIRELESS_MODE_AC_5G)
		Ret = 1;						// VHT

	return (Ret << 4);
}



BOOLEAN 
Get_RA_ShortGI(	
	struct rtl8192cd_priv *priv,
	struct stat_info *		pEntry,
	IN	WIRELESS_MODE		WirelessMode,
	IN	u1Byte				ChnlBW
)
{	
	BOOLEAN						bShortGI;

	BOOLEAN	bShortGI20MHz = FALSE,bShortGI40MHz = FALSE, bShortGI80MHz = FALSE;
	
	if(	WirelessMode == WIRELESS_MODE_N_24G || 
		WirelessMode == WIRELESS_MODE_N_5G 		)
	{
		if (pEntry->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_40M_)
			&& priv->pmib->dot11nConfigEntry.dot11nShortGIfor40M) {
			bShortGI40MHz = TRUE;
		} else if (pEntry->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_20M_) &&
			priv->pmib->dot11nConfigEntry.dot11nShortGIfor20M) {
			bShortGI20MHz = TRUE;
		}
	}
	else if(WirelessMode == WIRELESS_MODE_AC_5G)
	{
		{
			if( cpu_to_le32(pEntry->vht_cap_buf.vht_cap_info) & BIT(SHORT_GI80M_E))
				bShortGI80MHz = TRUE;
		}
	}

	switch(ChnlBW){
		case HT_CHANNEL_WIDTH_20_40:
			bShortGI = bShortGI40MHz;
			break;
		case HT_CHANNEL_WIDTH_80:
			bShortGI = bShortGI80MHz;
			break;
		default:case HT_CHANNEL_WIDTH_20:
			bShortGI = bShortGI20MHz;
			break;
	}		
	return bShortGI;
}


u4Byte
RateToBitmap_2SSVHT(
	pu1Byte			pVHTRate
)
{

	u1Byte	i,j , tmpRate;
	u4Byte	RateBitmap = 0;
		
	for(i = j= 0; i < 4; i+=2, j+=10)
	{
		tmpRate = (pVHTRate[0] >> i) & 3;

		switch(tmpRate){
		case 2:
			RateBitmap = RateBitmap | (0x03ff << j);		
			break;
		case 1:
			RateBitmap = RateBitmap | (0x01ff << j);
		break;

		case 0:
			RateBitmap = RateBitmap | (0x00ff << j);
		break;

		default:
			break;
		}
	}

	return RateBitmap;
}


u4Byte
Get_VHT_HT_Mix_Ratrbitmap(
	u4Byte					IOTAction,
	WIRELESS_MODE			WirelessMode,
	u4Byte					HT_ratr_bitmap,
	u4Byte					VHT_ratr_bitmap
	)
{
	u4Byte	ratr_bitmap = 0;
	if(WirelessMode == WIRELESS_MODE_N_24G || WirelessMode == WIRELESS_MODE_N_5G)
	{
/*	
		if(IOTAction == HT_IOT_VHT_HT_MIX_MODE)
			ratr_bitmap = HT_ratr_bitmap | BIT28 | BIT29;
		else
			ratr_bitmap =  HT_ratr_bitmap;
*/		
	}
	else
		ratr_bitmap =  VHT_ratr_bitmap;

	return ratr_bitmap;
}


VOID
UpdateHalRAMask8812(
	struct rtl8192cd_priv *priv,
	struct stat_info 		*pEntry,
	u1Byte				rssi_level
	)
{

	u1Byte						WirelessMode= WIRELESS_MODE_A;
	u1Byte						BW = HT_CHANNEL_WIDTH_20;
	u2Byte		RateSet=0, i;
	u1Byte		MimoPs=MIMO_PS_NOLIMIT, ratr_index = 8, H2CCommand[7] ={ 0};
	u4Byte		EntryRateSet = 0xffffffff, ratr_bitmap = 0, IOTAction = 0;
	BOOLEAN		bShortGI=FALSE, bCurTxBW80MHz=FALSE, bCurTxBW40MHz=FALSE;
	struct 		stat_info *pstat = pEntry;
	u1Byte 		rf_mimo_mode = get_rf_mimo_mode(priv);

	if(pEntry == NULL)		
	{
		return;
	}
	{
		if(pEntry->MIMO_ps & _HT_MIMO_PS_STATIC_)
                      MimoPs = MIMO_PS_STATIC;
		else if(pEntry->MIMO_ps & _HT_MIMO_PS_DYNAMIC_)
			MimoPs = MIMO_PS_DYNAMIC;
	
		add_RATid(priv, pEntry);		
		ratr_bitmap =  0xfffffff;
//
//		if(pstat->vht_cap_len && ( priv->pshare->rf_ft_var.support_11ac)) {
		if(pstat->vht_cap_len && (priv->pmib->dot11BssType.net_work_type & WIRELESS_11AC)) {
			WirelessMode = WIRELESS_MODE_AC_5G;
			if(((le32_to_cpu(pstat->vht_cap_buf.vht_support_mcs[0])>>2)&3)==3)
				rf_mimo_mode = MIMO_1T1R;
		} else if ((priv->pmib->dot11BssType.net_work_type & WIRELESS_11N) && pstat->ht_cap_len && (!should_restrict_Nrate(priv, pstat))) {
				if(priv->pmib->dot11BssType.net_work_type & WIRELESS_11A)
				WirelessMode = WIRELESS_MODE_N_5G;
			else
				WirelessMode = WIRELESS_MODE_N_24G;					
			if((pstat->tx_ra_bitmap & 0xff00000) == 0)
				rf_mimo_mode = MIMO_1T1R;
		}
		else if (((priv->pmib->dot11BssType.net_work_type & WIRELESS_11G) && isErpSta(pstat)))
		{
				WirelessMode = WIRELESS_MODE_G;		
		} else if ((priv->pmib->dot11BssType.net_work_type & WIRELESS_11A) &&
				((OPMODE & WIFI_AP_STATE) || (priv->pmib->dot11RFEntry.phyBandSelect & PHY_BAND_5G))) {
				WirelessMode = WIRELESS_MODE_A;		
		}
		else if(priv->pmib->dot11BssType.net_work_type & WIRELESS_11B){
			WirelessMode = WIRELESS_MODE_B;		
		}

		if(WirelessMode == WIRELESS_MODE_AC_5G) {
			ratr_bitmap &= 0xfff;
			ratr_bitmap |= RateToBitmap_2SSVHT(&(pstat->vht_cap_buf.vht_support_mcs[0])) << 12;
//			ratr_bitmap &= 0x3FCFFFFF;
			if(rf_mimo_mode == MIMO_1T1R)
				ratr_bitmap &= 0x000fffff;
			else
				ratr_bitmap &= 0x3FCFFFFF;			// Test Chip...	2SS MCS7

			if(pstat->tx_bw==HT_CHANNEL_WIDTH_80)
				bCurTxBW80MHz = TRUE;
		}
		if (priv->pshare->is_40m_bw && (pstat->tx_bw == HT_CHANNEL_WIDTH_20_40))
			bCurTxBW40MHz = TRUE;
	}

	// assign band mask and rate bitmap
	switch (WirelessMode)
	{
		case WIRELESS_MODE_B:
		{
			ratr_index = RATR_INX_WIRELESS_B;
			if(ratr_bitmap & 0x0000000c)		//11M or 5.5M enable				
				ratr_bitmap &= 0x0000000d;
			else
				ratr_bitmap &= 0x0000000f;
		}
		break;

		case WIRELESS_MODE_G:
		{
			ratr_index = RATR_INX_WIRELESS_GB;
			
			if(rssi_level == 1)
				ratr_bitmap &= 0x00000f00;
			else if(rssi_level == 2)
				ratr_bitmap &= 0x00000ff0;
			else
				ratr_bitmap &= 0x00000ff5;
		}
		break;
			
		case WIRELESS_MODE_A:
		{
			ratr_index = RATR_INX_WIRELESS_G;
			ratr_bitmap &= 0x00000ff0;
		}
		break;
			
		case WIRELESS_MODE_N_24G:
		case WIRELESS_MODE_N_5G:
		{
			if(WirelessMode == WIRELESS_MODE_N_24G)
				ratr_index = RATR_INX_WIRELESS_NGB;
			else
				ratr_index = RATR_INX_WIRELESS_NG;

//			if(MimoPs <= MIMO_PS_DYNAMIC)
			if(MimoPs < MIMO_PS_DYNAMIC)
			{
				if(rssi_level == 1)
					ratr_bitmap &= 0x00070000;
				else if(rssi_level == 2)
					ratr_bitmap &= 0x0007f000;
				else
					ratr_bitmap &= 0x0007f005;
			}
			else
			{
				if (rf_mimo_mode == MIMO_1T1R)
				{
					if (bCurTxBW40MHz)
					{
						if(rssi_level == 1)
							ratr_bitmap &= 0x000f0000;
						else if(rssi_level == 2)
							ratr_bitmap &= 0x000ff000;
						else
							ratr_bitmap &= 0x000ff015;
					}
					else
					{
						if(rssi_level == 1)
							ratr_bitmap &= 0x000f0000;
						else if(rssi_level == 2)
							ratr_bitmap &= 0x000ff000;
						else
							ratr_bitmap &= 0x000ff005;
					}	
				}
				else
				{
					if (bCurTxBW40MHz)
					{
						if(rssi_level == 1)
							ratr_bitmap &= 0x0f8f0000;
						else if(rssi_level == 2)
							ratr_bitmap &= 0x0f8ff000;
						else
							ratr_bitmap &= 0x0f8ff015;
					}
					else
					{
						if(rssi_level == 1)
							ratr_bitmap &= 0x0f8f0000;
						else if(rssi_level == 2)
							ratr_bitmap &= 0x0f8ff000;
						else
							ratr_bitmap &= 0x0f8ff005;
					}
				}
			}
		}
		break;

		case WIRELESS_MODE_AC_5G:
		{
			ratr_index = RATR_INX_WIRELESS_AC_N;

			if (rf_mimo_mode == MIMO_1T1R)
				ratr_bitmap &= 0x003ff010;
			else
				ratr_bitmap &= 0xfffff010;
		}
		break;

		default:
			ratr_index = RATR_INX_WIRELESS_NGB;
			
			if(rf_mimo_mode == MIMO_1T1R)
				ratr_bitmap &= 0x000ff0ff;
			else
				ratr_bitmap &= 0x0f8ff0ff;
			break;
	}

	BW = Get_RA_BW(bCurTxBW80MHz, bCurTxBW40MHz);
	bShortGI = Get_RA_ShortGI(priv, pEntry, WirelessMode, BW);


	pstat->ratr_idx = MRateIdxToARFRId8812(priv, ratr_index, rf_mimo_mode) ;

	H2CCommand[0] = (pstat->aid);
	H2CCommand[1] =  (pstat->ratr_idx)| (bShortGI?0x80:0x00) ;	
	H2CCommand[2] = BW |Get_VHT_ENI(IOTAction, WirelessMode, ratr_bitmap);
	

	H2CCommand[2] |= BIT6;			// DisableTXPowerTraining

	H2CCommand[3] = (u1Byte)(ratr_bitmap & 0x000000ff);
	H2CCommand[4] = (u1Byte)((ratr_bitmap & 0x0000ff00) >>8);
	H2CCommand[5] = (u1Byte)((ratr_bitmap & 0x00ff0000) >> 16);
	H2CCommand[6] = (u1Byte)((ratr_bitmap & 0xff000000) >> 24);
	
	FillH2CCmd8812(priv, H2C_8812_RA_MASK, 7, H2CCommand);

	SetBcnCtrlReg_8812(priv, BIT3, 0);
/*
	panic_printk("UpdateHalRAMask8812E(): bitmap = %x ratr_index = %1x, MacID:%x, ShortGI:%x, MimoPs=%d\n", 
		ratr_bitmap, pstat->ratr_idx,  (pstat->aid), bShortGI, MimoPs);
	
	panic_printk("Cmd: %02x, %02x, %02x, %02x, %02x, %02x, %02x  \n",
		H2CCommand[0] ,H2CCommand[1], H2CCommand[2],
		H2CCommand[3] ,H2CCommand[4], H2CCommand[5], H2CCommand[6]		);
*/		
}
void
UpdateHalMSRRPT8812(
	struct rtl8192cd_priv *priv,
	unsigned short		aid,
	unsigned char		opmode
	)
{
	u1Byte		H2CCommand[3] ={ 0};
	
	H2CCommand[0] = opmode & 0x01;
	H2CCommand[1] = aid & 0xff;
	H2CCommand[2] = 0;
	FillH2CCmd8812(priv, H2C_8812_MSRRPT, 3, H2CCommand);
	
//	panic_printk("UpdateHalMSRRPT8812 Cmd: %02x, %02x, %02x  \n",
//		H2CCommand[0] ,H2CCommand[1], H2CCommand[2]);
}

void check_txrate_by_reg_8812(struct rtl8192cd_priv *priv, struct stat_info *pstat)
{
	unsigned char initial_rate = 0x7f;
	unsigned char legacyRA =0 ;

	if(!priv->pshare->rf_ft_var.update_rainfo)
		return;

	RTL_W8(0x8d, 0x01);
	RTL_W8(0x8f, 0x40);		
	
	if( should_restrict_Nrate(priv, pstat) && is_fixedMCSTxRate(priv))
		legacyRA = 1;
	
	if (pstat->aid && (pstat->aid < FW_NUM_STAT-1))
	{
		if (priv->pmib->dot11StationConfigEntry.autoRate || legacyRA)
		{
			RTL_W8(0x8c, REMAP_AID(pstat)&0x1f);
			initial_rate = RTL_R8(0x2f0);
			if ((initial_rate & 0x7f) == 0x7f)
				return;

			if ((initial_rate&0x3f) < 12) {
				pstat->current_tx_rate = dot11_rate_table[initial_rate&0x3f];
				pstat->ht_current_tx_info &= ~TX_USE_SHORT_GI;				
			} else {
				if((initial_rate&0x3f) >= 44)
					pstat->current_tx_rate = 0x90+((initial_rate&0x3f) -44);
				else
					pstat->current_tx_rate = 0x80+((initial_rate&0x3f) -12);
				
				if (initial_rate & BIT(7))
					pstat->ht_current_tx_info |= TX_USE_SHORT_GI;
				else
					pstat->ht_current_tx_info &= ~TX_USE_SHORT_GI;
			}

			priv->pshare->current_tx_rate    = pstat->current_tx_rate;
			priv->pshare->ht_current_tx_info = pstat->ht_current_tx_info;
		} 
	} 
}



#endif

#endif //CONFIG_RTL_8812_SUPPORT

