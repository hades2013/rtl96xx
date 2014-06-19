#include <preloader.h>
#include <cpu_utils.h>
#include "dram/plr_dram_gen2.h"
#include "swp_ddr_gen2_util.h"
#include "swp_util.h"

#define swp_info_t dram_gen2_info_t
#define PARA_SECTION __attribute__ ((section (".cli_ddr_para")))
#include "swp_access_soc.h"



#define DCR_DBUSWID_FD_S    (24)
#define DCR_DBUSWID_MASK    (0xF << DCR_DBUSWID_FD_S)
#define DTR0_CAS_FD_S       (28)
#define DTR0_CAS_MASK       (0xF << DTR0_CAS_FD_S)

u32_t swp_query_mem_clk(void);

// get ddr timing parameter utility
inline static unsigned int
get_ddr_timing_para(char *v)
{
	u32_t idx=0;
	u32_t factor=1;

    u32_t n=pblr_atoi(v);
	while(v[idx]!='\0') idx++;
    if (idx>=2) {
		/*  input dtr parameters, n ns
			dclk = n<ns>/(1/mhz.mem)<us>
			     = n<ns>/(1000/mhz.mem)<ns>
			     = (n*mhz.mem)/1000	    */
			     
        if (v[idx-1]=='s') {
            if (v[idx-2]=='n') {
            } else if (v[idx-2]=='u') {
            	factor=1000;
            } else if (v[idx-2]=='m') {
                factor=1000000;
            } else {
            	printf("unrecognized unit\n");
                return 0;
            }
            u32_t mem = swp_query_mem_clk();
            factor*=mem;
            u32_t num=factor*n;
            n=(num+999)/1000;	// do round up to over minumum requirement
            //printf("$=%ddclk\n", n);
        } 
    } 
    return n;
}

// virtual variable set/get
inline static unsigned int 
cli_dg2_memctl_get_DRAM_buswidth(const dram_gen2_info_t *info)  // original name _DCR_get_buswidth
{
    unsigned int buswidth;
    buswidth = ((info->dcr & DCR_DBUSWID_MASK) >> DCR_DBUSWID_FD_S);
    return 1<<(buswidth+3);
    /*
    switch (buswidth) {
        case 0:
            return (8);
        case 1:
            return (16);
        case 2:
            return (32);
        default:
            printf("#Error: error bus width setting (11)\n");
            return 0;
    }
    */
}

SECTION_ON_FLASH static int
cli_dg2_memctl_set_DRAM_buswidth(dram_gen2_info_t *info, unsigned int buswidth) {
    #if 0
    unsigned int t_cas, dmcr_value;
    /* get DCR value */
    #endif
    unsigned int dcr = info->dcr;

    switch (buswidth){
        case 8:
            dcr = (dcr & (~((unsigned int)DCR_DBUSWID_MASK)));
            break;

        case 16:
            dcr = (dcr & (~((unsigned int)DCR_DBUSWID_MASK))) | (unsigned int)(1<<DCR_DBUSWID_FD_S);
            break;
        default:
            return -1;
            //printf("%s, %d: Error buswidth value(%d)\n", __FUNCTION__, __LINE__, buswidth);
            //break;
    }
    info->dcr = dcr;
    return 0;
}
SECTION_ON_FLASH static unsigned int 
cli_dg2_memctl_get_DRAM_size(const dram_gen2_info_t *info) //cli_dg2_memctl_dram_size
{
    unsigned int dcr;
    int total_bit = 0;

    dcr = info->dcr;
    total_bit = ((dcr>>24)&0x3); //bus width
    total_bit += ((dcr>>20)&0x3)+11; //row count
    total_bit += ((dcr>>16)&0x7)+8 ; //col count
    total_bit += ((dcr>>28)&0x3)+1;  //bank count
    total_bit += (dcr>>15)&1;        //Dram Chip Select

    return ((1<<total_bit));
}

const unsigned int ddr1_8bit_size[] =  { 0x10120000/* 16MB */, 0x10220000/* 32MB */, 0x10230000/* 64MB */, 0x10330000/* 128MB */, 0x0};
const unsigned int ddr2_8bit_size[] =  { 0x10220000/* 32MB */, 0x10320000/* 64MB */, 0x20320000/* 128MB */, 0x20420000/* 256MB */, 0x20520000/* 512MB */};
const unsigned int ddr3_8bit_size[] =  { 0x20220000/* 64MB */, 0x20320000/* 128MB */, 0x20420000/* 256MB */, 0x20520000/* 512MB */, 0x20530000/* 1024MB */};
const unsigned int ddr1_16bit_size[] = { 0x11110000/* 16MB */, 0x11210000/* 32MB */, 0x11220000/* 64MB */, 0x11320000/* 128MB */, 0x0};
const unsigned int ddr2_16bit_size[] = { 0x11210000/* 32MB */, 0x11220000/* 64MB */, 0x21220000/* 128MB */, 0x21320000/* 256MB */, 0x21420000/* 512MB */};
const unsigned int ddr3_16bit_size[] = { 0x21210000/* 64MB */, 0x21220000/* 128MB */, 0x21320000/* 256MB */, 0x21420000/* 512MB */, 0x21520000/* 1024MB */};

#define DDR_MIN_SIZE    (16)    //MB

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_DRAM_size(dram_gen2_info_t *info, unsigned int dram_size){
    const unsigned int *size_arry;
    unsigned int dcr_value, dram_bus_width, ddr_type, idx;

    dram_bus_width = cli_dg2_memctl_get_DRAM_buswidth(info);
    if(dram_bus_width>16){  // get failed
        dram_bus_width = 8;
    }

    ddr_type = GET_DDR_TYPE();
    
    if(dram_bus_width==8){
        size_arry = ((DDR_TYPE_DDR3==ddr_type)?(&ddr3_8bit_size[0]):\
                    ((DDR_TYPE_DDR2==ddr_type)?(&ddr2_8bit_size[0]):(&ddr1_8bit_size[0])));
    }else{
        size_arry = ((DDR_TYPE_DDR3==ddr_type)?(&ddr3_16bit_size[0]):\
                    ((DDR_TYPE_DDR2==ddr_type)?(&ddr2_16bit_size[0]):(&ddr1_16bit_size[0])));
    }

    idx = dram_size/(DDR_MIN_SIZE*(1<<ddr_type));
    
    switch (idx){
        case 2: /* DDR1:32MB, DDR2:64MB, DDR3:128MB*/
            dcr_value = size_arry[1];
            break;
        case 4: /* DDR1:64MB, DDR2:128MB, DDR3:256MB*/
            dcr_value = size_arry[2];
            break;
        case 8: /* DDR1:128MB, DDR2:256MB, DDR3:512MB*/
            dcr_value = size_arry[3];
            break;
        case 16: /* DDR1:???, DDR2:512MB, DDR3:1024MB*/
            dcr_value = size_arry[4];
            break;
        default: /* DDR1:16MB, DDR2:32MB, DDR3:64MB*/
            dcr_value = size_arry[0];
            break;
    }
    
    if(0 == dcr_value)  dcr_value = size_arry[0];
    info->dcr = dcr_value;
    return 0;
}

// DTR
SECTION_ON_FLASH static unsigned int 
cli_dg2_memctl_get_tcas(const dram_gen2_info_t *info)
{
    return (((info->dtr0>>28)&0xF)+1) ; 
}

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_tcas(dram_gen2_info_t *info, unsigned int val)
{
    u32_t ddr_type = GET_DDR_TYPE();

    // DDR1: 2~3, DDR2: CAS 2~6, DDR3: CAS 5~11
    if (((DDR_TYPE_DDR3==ddr_type) && (val>11||val<5))||
        ((DDR_TYPE_DDR2==ddr_type) && (val>6||val<2)) ||
        ((DDR_TYPE_DDR1==ddr_type) && (val>3||val<2)))
    {
        printf("<tCAS Range> DDR1:2~3, DDR2:2~6, DDR3:5~11 \n");
        return 0;        
    }
    
	info->dtr0 = (info->dtr0&0x0FFFFFFF) | ((val-1)<<28);

    if(DDR_TYPE_DDR3==ddr_type) {	// DDR3
    	// because cas<=11, ignore MR0 bit2
	    info->DDR3_mr0 = (info->DDR3_mr0 & 0xFFFFFF8F) | ((val-4)<<4);	
	} else {						// DDR 1/2
		info->DDR2_mr = (info->DDR2_mr & 0xFFFFFF8F) | (val<<4);	
	}
	return 0;
}

SECTION_ON_FLASH static char *
cli_dg2_memctl_get_twr(const dram_gen2_info_t *info, char *buf)
{
	return pblr_tostr(buf, (((info->dtr0>>24)&0xF)+1), 10, 0);
}

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_twr(dram_gen2_info_t *info, const char *v)
{
	u32_t val=get_ddr_timing_para((char *)v);
    u32_t ddr_type = GET_DDR_TYPE();

	if (DDR_TYPE_DDR1==ddr_type)
        return 0;
        
    // DDR2: 2~6. DDR3: 5~8
    if (((DDR_TYPE_DDR3==ddr_type) && (val>8||val<5))||
        ((DDR_TYPE_DDR2==ddr_type) && (val>6||val<2)))
    {
        printf("<tWR Range> DDR2: 2~6. DDR3: 5~8 \n");
        return 0;        
    }
        
	info->dtr0 = (info->dtr0&0xF0FFFFFF) | ((val-1)<<24);

    if(DDR_TYPE_DDR3==ddr_type) {	// DDR3
    	// focus on 5~8
	    info->DDR3_mr0 = (info->DDR3_mr0 & 0xFFFFF1FF) | ((val-4)<<9);	
	} else {						// DDR 1/2
		info->DDR2_mr = (info->DDR2_mr & 0xFFFFF1FF) | ((val-1)<<9);
	}
	return 0;
}

SECTION_ON_FLASH static char *
cli_dg2_memctl_get_tcwl(const dram_gen2_info_t *info, char *buf)
{
    return pblr_tostr(buf, (((info->dtr0>>20)&0xF)+1), 10, 0);
}

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_tcwl(dram_gen2_info_t *info, const char *v)
{
	u32_t val=get_ddr_timing_para((char *)v);
    u32_t ddr_type = GET_DDR_TYPE();
    // 1~8
    if (val>8||val<1) {
        printf("<tCWL Range> 1~8 \n");
        return 0;        
    }
        
	info->dtr0 = (info->dtr0&0xFF0FFFFF) | ((val-1)<<20);

	if(DDR_TYPE_DDR3==ddr_type) {	// DDR3
		info->DDR3_mr2 = (info->DDR3_mr2 & 0xFFFFFFC7) | ((val-5)<<3);
	}
	return 0;
}

SECTION_ON_FLASH static char *
cli_dg2_memctl_get_trtp(const dram_gen2_info_t *info, char *buf)
{
    return pblr_tostr(buf, (((info->dtr0>>16)&0xF)+1), 10, 0);
}

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_trtp(dram_gen2_info_t *info, const char *v)
{
	u32_t val=get_ddr_timing_para((char *)v);

    // 1~16
    if (val>16||val<1) {
        printf("<tRTP Range> 1~16 \n");
        return 0;        
    }
        
	info->dtr0 = (info->dtr0&0xFFF0FFFF) | ((val-1)<<16);
	return 0;
}

SECTION_ON_FLASH static char *
cli_dg2_memctl_get_twtr(const dram_gen2_info_t *info, char *buf)
{
    return pblr_tostr(buf, (((info->dtr0>>12)&0xF)+1), 10, 0);
}

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_twtr(dram_gen2_info_t *info, const char *v)
{
	u32_t val=get_ddr_timing_para((char *)v);

    // 1~16
    if (val>16||val<1) {
        printf("<tWTR Range> 1~16 \n");
        return 0;        
    }
        
	info->dtr0 = (info->dtr0&0xFFFF0FFF) | ((val-1)<<12);
	return 0;
}

SECTION_ON_FLASH static char *
cli_dg2_memctl_get_trefi(const dram_gen2_info_t *info, char *buf)
{
    return pblr_tostr(buf, (((info->dtr0>>8)&0xF)+1)*32*(1<<((info->dtr0>>4)&0xF)), 10, 0);
    //return (((info->dtr0>>8)&0xF)+1);
}

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_trefi(dram_gen2_info_t *info, const char *v)
{
    u32_t val=get_ddr_timing_para((char *)v);
	u32_t u=0, unit=32, m;
    
    for (; u<=7; unit<<=1, ++u) {
        //m=(val+unit-1)/unit;	//round-up
        m=val/unit;
        if((m<=16)&&(m>=1)) {
            //printf("unit=%d, multiplier=%d\n", unit, m);
            info->dtr0 = (info->dtr0&0xFFFFF00F) | ((u)<<4) | ((m-1)<<8);
            return 0;
        } 
    }
    // error status    
    printf("EE: tREFI=%s is over range.\n", v);
	return 0;
}

#if 0
SECTION_ON_FLASH static unsigned int 
cli_dg2_memctl_get_trefi_unit(const dram_gen2_info_t *info)
{
    return 32*(1<<((info->dtr0>>4)&0xF));
}

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_trefi_unit(dram_gen2_info_t *info, unsigned int val)
{
	u32_t unit;
    for (unit=0; unit<=7; unit++) {
		if(val == 32*(1<<unit))
            break;
    }
    if (unit<=7)
		info->dtr0 = (info->dtr0&0xFFFFFF0F) | ((unit)<<4);
    else
        printf("<tREFI Unit> 32, 64, 128, ..., 4096");
    
	return 0;
}
#endif

SECTION_ON_FLASH static char *
cli_dg2_memctl_get_trp(const dram_gen2_info_t *info, char *buf)
{
    return pblr_tostr(buf, (((info->dtr1>>24)&0x1F)+1), 10, 0);
}

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_trp(dram_gen2_info_t *info, const char *v)
{
	u32_t val=get_ddr_timing_para((char *)v);

    // 1~32
    if (val>32||val<1) {
        printf("<tRP Range> 1~32 \n");
        return 0;        
    }
        
	info->dtr1 = (info->dtr1&0xE0FFFFFF) | ((val-1)<<24);
	return 0;
}

SECTION_ON_FLASH static char *
cli_dg2_memctl_get_trcd(const dram_gen2_info_t *info, char *buf)
{
    return pblr_tostr(buf, (((info->dtr1>>16)&0x1F)+1), 10, 0);
}

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_trcd(dram_gen2_info_t *info, const char *v)
{
	u32_t val=get_ddr_timing_para((char *)v);

    // 1~32
    if (val>32||val<1) {
        printf("<tRCD Range> 1~32 \n");
        return 0;        
    }
        
	info->dtr1 = (info->dtr1&0xFFE0FFFF) | ((val-1)<<16);
	return 0;
}

SECTION_ON_FLASH static char *
cli_dg2_memctl_get_trrd(const dram_gen2_info_t *info, char *buf)
{
    return pblr_tostr(buf, (((info->dtr1>>8)&0x1F)+1), 10, 0);
}

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_trrd(dram_gen2_info_t *info, const char *v)
{
	u32_t val=get_ddr_timing_para((char *)v);

    // 1~32
    if (val>32||val<1) {
        printf("<tRRD Range> 1~32 \n");
        return 0;        
    }
        
	info->dtr1 = (info->dtr1&0xFFFFE0FF) | ((val-1)<<8);
	return 0;
}

SECTION_ON_FLASH static char *
cli_dg2_memctl_get_tfawg(const dram_gen2_info_t *info, char *buf)
{
    return pblr_tostr(buf, ((info->dtr1&0x1F)+1), 10, 0);
}

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_tfawg(dram_gen2_info_t *info, const char *v)
{
	u32_t val=get_ddr_timing_para((char *)v);

    // 1~32
    if (val>32||val<1) {
        printf("<tFAWG Range> 1~32 \n");
        return 0;        
    }
        
	info->dtr1 = (info->dtr1&0xFFFFFFE0) | (val-1);
	return 0;
}

SECTION_ON_FLASH static char *
cli_dg2_memctl_get_trfc(const dram_gen2_info_t *info, char *buf)
{
    return pblr_tostr(buf, (((info->dtr2>>20)&0xFF)+1), 10, 0);
}

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_trfc(dram_gen2_info_t *info, const char *v)
{
	u32_t val=get_ddr_timing_para((char *)v);

    // 1~256
    if (val>256||val<1) {
        printf("<tRFC Range> 1~256 \n");
        return 0;        
    }
        
	info->dtr2 = (info->dtr2&0xF00FFFFF) | ((val-1)<<20);
	return 0;
}

SECTION_ON_FLASH static char *
cli_dg2_memctl_get_tras(const dram_gen2_info_t *info, char *buf)
{
    return pblr_tostr(buf, (((info->dtr2>>12)&0x3F)+1), 10, 0);
}

SECTION_ON_FLASH static int 
cli_dg2_memctl_set_tras(dram_gen2_info_t *info, const char *v)
{
	u32_t val=get_ddr_timing_para((char *)v);

    // 1~64
    if (val>64||val<1) {
        printf("<tRAS Range> 1~64 \n");
        return 0;        
    }
        
	info->dtr2 = (info->dtr2&0xFFFC0FFF) | ((val-1)<<12);
	return 0;
}

const unsigned int ZQ_prog_table[] = {
    0x7e, /* 48  */
    0x3e, /* 50  */
    0x7c, /* 60  */
    0x30, /* 80  */
    0x31, /* 100 */
    0x20, /* 120 */
    0x21, /* 150 */
    0x0a, /* 160 */
    0x19, /* 200 */
    0x11, /* 300 */
    0x48, /* 320 */
    0x49, /* 400 */
};

SECTION_ON_FLASH void 
cli_dg2_memctl_set_ODT_OCD(dram_gen2_info_t *info, unsigned int odt, unsigned int ocd)
{
    unsigned int memctl_odt, memctl_ocd, zq_setting;
    zq_setting = info->zq_setting;
    switch (odt){
        case 24:
            memctl_odt = ZQ_prog_table[0];
            break;
        case 25:
            memctl_odt = ZQ_prog_table[1];
            break;
        case 30:
            memctl_odt = ZQ_prog_table[2];
            break;
        case 40:
            memctl_odt = ZQ_prog_table[3];
            break;
        case 50:
            memctl_odt = ZQ_prog_table[4];
            break;
        case 60:
            memctl_odt = ZQ_prog_table[5];
            break;
        default:/* 75*/
        case 75:
            memctl_odt = ZQ_prog_table[6];
            break;
        case 80:
            memctl_odt = ZQ_prog_table[7];
            break;
        case 100:
            memctl_odt = ZQ_prog_table[8];
            break;
        case 150:
            memctl_odt = ZQ_prog_table[9];
            break;
        case 160:
            memctl_odt = ZQ_prog_table[10];
            break;
        case 200:
            memctl_odt = ZQ_prog_table[11];
            break;
    }
    switch (ocd){
        case 48:
            memctl_ocd = ZQ_prog_table[0];
            break;
        case 50:
            memctl_ocd = ZQ_prog_table[1];
            break;
        case 60:
            memctl_ocd = ZQ_prog_table[2];
            break;
        default: /* 80 */
        case 80:
            memctl_ocd = ZQ_prog_table[3];
            break;
        case 100:
            memctl_ocd = ZQ_prog_table[4];
            break;
        case 120:
            memctl_ocd = ZQ_prog_table[5];
            break;
        case 150:
            memctl_ocd = ZQ_prog_table[6];
            break;
        case 160:
            memctl_ocd = ZQ_prog_table[7];
            break;
        case 200:
            memctl_ocd = ZQ_prog_table[8];
            break;
        case 300:
            memctl_ocd = ZQ_prog_table[9];
            break;
        case 320:
            memctl_ocd = ZQ_prog_table[10];
            break;
        case 400:
            memctl_ocd = ZQ_prog_table[11];
            break;
    }
//  zq_setting =  zq_setting               | memctl_ocd | (memctl_odt << 7);
    zq_setting = (zq_setting & 0xFFFFC000) | memctl_ocd | (memctl_odt << 7);
    info->zq_setting = zq_setting;
}

// mrs_dll_enable
inline static int
cli_dg2_memctl_set_mrs_dll_enable(dram_gen2_info_t *info, u32_t dll_enable) {
    info->DDR3_mr1 = (info->DDR3_mr1 & 0xFFFFFFFE) | (dll_enable & 0x1);
    return 0;
}

inline static u32_t 
cli_dg2_memctl_get_mrs_dll_enable(const dram_gen2_info_t *info) {
    return (info->DDR3_mr1 & 0x1);
}



// mrs_odt
const unsigned int ddr3_odt[] = { 0x0 /*dis*/, 0x200 /*20ohm*/, 0x204/*30ohm*/, 0x44 /*40ohm*/, 0x4/*60ohm*/, 0x40 /*120ohm*/};
const unsigned int ddr2_odt[] = { 0x0 /*dis*/, 0x44 /*50ohm*/, 0x4/*75ohm*/, 0x40 /*150ohm */};
SECTION_ON_FLASH static int
cli_dg2_memctl_set_mrs_odt(dram_gen2_info_t *info, u32_t odt) {
	u32_t ddr = GET_DDR_TYPE();
	u32_t idx=0;
	if(DDR_TYPE_DDR3==ddr) {			//DDR3
		switch(odt) {
			case 0:
				idx = 0;
				break;
			case 20:
				idx = 1;
				break;
			case 30:
				idx = 2;
				break;
			case 40:
				idx = 3;
				break;
			case 60:
				idx = 4;
				break;
			case 120:
				idx = 5;
				break;
			default:
				idx = 4;
				break;
		}
		info->DDR3_mr1 = (info->DDR3_mr1 & 0xFFFFFDBB) | ddr3_odt[idx];
	} else if (DDR_TYPE_DDR2==ddr) {	//DDR2
		switch(odt) {
			case 0:
				idx = 0;
				break;
			case 50:
				idx = 1;
				break;
			case 75:
				idx = 2;
				break;
			case 150:
				idx = 3;
				break;
			default:
				idx = 2;
				break;
		}	
		info->DDR2_emr1 = (info->DDR2_emr1 & 0xFFFFFFBB) | ddr2_odt[idx];
	} else {
		//printf("DDR1 doesn't have ODT\n");
	}
	return 0;
}

SECTION_ON_FLASH static u32_t 
cli_dg2_memctl_get_mrs_odt(const dram_gen2_info_t *info) 
{
	u32_t ddr = GET_DDR_TYPE();
	u32_t mrs_odt = 0;
	if (DDR_TYPE_DDR3==ddr) {			//DDR3, FIXME, should be 2-bits 
		switch((info->DDR3_mr1 & 0x244)) {
			case 0x0:
				mrs_odt = 0;
				break;
			case 0x200:
				mrs_odt = 20;
				break;
			case 0x204:
				mrs_odt = 30;
				break;
			case 0x44:
				mrs_odt = 40;
				break;
			case 0x4:
				mrs_odt = 60;
				break;
			case 0x40:
				mrs_odt = 120;
				break;
			default:
				printf("Unknown!\n");
				break;
		}
	} else if (DDR_TYPE_DDR2==ddr) {	//DDR2
		switch((info->DDR2_emr1 & 0x44)) {
			case 0x0:
				mrs_odt = 0;
				break;
			case 0x44:
				mrs_odt = 50;
				break;
			case 0x4:
				mrs_odt = 75;
				break;
			case 0x40:
				mrs_odt = 150;
				break;
			default:
				printf("Unknown!\n");
				break;
		}		
	} else {
		//printf("DDR1 doesn't have ODT\n");
		return 0xffffffff;
	}
	return mrs_odt;
}

//rtt_wr
const unsigned int ddr3_rtt_wr[] = { 0x0 /*dis*/, 0x200 /*60ohm*/, 0x400/*120ohm */};
SECTION_ON_FLASH static int 
cli_dg2_memctl_set_mrs_rtt_wr(dram_gen2_info_t *info, u32_t rtt_wr) {
	u32_t v;
	switch(rtt_wr) {
		case 0:
			v = 0x0;
			break;
		case 60:
			v = 0x200;
			break;
		case 120:
			v = 0x400;
			break;
		default:
			v = 0x400;
			break;
	}
	info->DDR3_mr2 = (info->DDR3_mr2 & 0xFFFFF9FF ) | v;
    return 0;
}

SECTION_ON_FLASH static u32_t 
cli_dg2_memctl_get_mrs_rtt_wr(const dram_gen2_info_t *info) {
	u32_t rtt_wr;
	switch((info->DDR3_mr2 & 0x600)) {
	    default:
		case 0x0:
			rtt_wr = 0;
			break;
		case 0x200:
			rtt_wr = 60;
			break;
		case 0x400:
			rtt_wr = 120;
			break;
	}
	return rtt_wr;
}

//mrs_drv_strength
SECTION_ON_FLASH static int 
cli_dg2_memctl_set_mrs_drv_strength(dram_gen2_info_t *info, u32_t drv_strength) {
    u32_t v;
    u32_t ddr = GET_DDR_TYPE();

    switch(drv_strength) {
        case 0:/* DDR1/2:full strength;    DDR3:RZQ/6 */
            v = 0x0;
            break;
        case 1:/* DDR1/2:reduced strength; DDR3:RZQ/7 */
            v = 0x2;
            break;
        default:
            v = 0x0;
            break;
    }
    if (DDR_TYPE_DDR3==ddr) {
        info->DDR3_mr1 = (info->DDR3_mr1 & 0xFFFFFFFD ) | v;
    } else if (DDR_TYPE_DDR2==ddr) {
        info->DDR2_emr1 = (info->DDR2_emr1 & 0xFFFFFFFD ) | v;
    } else if (DDR_TYPE_DDR1==ddr) {
        info->DDR1_emr = (info->DDR1_emr & 0xFFFFFFFD ) | v;
    } else {
        printf("Warning: DDR%d is not supported\n", ddr);
    }
    return 0;
}

SECTION_ON_FLASH static u32_t 
cli_dg2_memctl_get_mrs_drv_strength(const dram_gen2_info_t *info) {
    u32_t v;
    u32_t ddr = GET_DDR_TYPE();
    if (DDR_TYPE_DDR3==ddr) {
        v = info->DDR3_mr1;
    } else if (DDR_TYPE_DDR2==ddr) {
        v = info->DDR2_emr1;
    } else if (DDR_TYPE_DDR1==ddr) {
        v = info->DDR1_emr;
    } else {
        printf("Warning: DDR%d is not supported\n", ddr);
        v = 0;
    }
    v = ((v & 0x2) >> 1);
 
    return v;
}


//dq_wr_delay
SECTION_ON_FLASH static unsigned int 
cli_dg2_memctl_get_dq_wr_delay(const dram_gen2_info_t *info, unsigned int dq_num)
{
	const u32_t *src = &(info->static_cal_data_0);
	u32_t dq_delay;
	dq_delay = *(src+dq_num);
	return ((dq_delay >> 24) & 0x1F);
}
SECTION_ON_FLASH static int
cli_dg2_memctl_set_dq_wr_delay(dram_gen2_info_t *info, unsigned int dq_num, unsigned int wr_delay) {
    unsigned int *src;
    src = &(info->static_cal_data_0);
    *(src+dq_num) = (*(src+dq_num) & 0x00FFFFFF) | ((wr_delay&0x1F)<<24);
    return 0;
}
 

//dqm_delay
SECTION_ON_FLASH static unsigned int 
cli_dg2_memctl_get_dqm_delay(const dram_gen2_info_t *info, unsigned int dqm_num) {
	u32_t dqm_delay = info->static_cal_data_32;
	if(1==dqm_num)
		return ((dqm_delay >> 16) & 0x1F);
	else
		return ((dqm_delay >> 24) & 0x1F);
}
SECTION_ON_FLASH static int 
cli_dg2_memctl_set_dqm_delay(dram_gen2_info_t *info, unsigned int dqm_num, unsigned int dqm_delay) {
    if(1==dqm_num)
        info->static_cal_data_32 = (info->static_cal_data_32 & 0xFFE0FFFF) | ((dqm_delay & 0x1F) << 16);
    else
        info->static_cal_data_32 = (info->static_cal_data_32 & 0xE0FFFFFF) | ((dqm_delay & 0x1F) << 24);
    return 0;
}

//dqs0_dqs0_en_hclk
SECTION_ON_FLASH static int 
cli_dg2_set_dqs0_en_hclk(dram_gen2_info_t *info, u32_t en) {
    info->dider = (info->dider & 0x7FFFFFFF ) | (en<<31);
    return 0;
}
SECTION_ON_FLASH static u32_t 
cli_dg2_get_dqs0_en_hclk(const dram_gen2_info_t *info) {
    return (info->dider >>31) & 0x1;
}
//dqs0_en_tap
SECTION_ON_FLASH static int
cli_dg2_set_dqs0_en_tap(dram_gen2_info_t *info, u32_t tap) {
    info->dider = (info->dider & 0xE0FFFFFF ) | (tap<<24);
    return 0;
}
SECTION_ON_FLASH static u32_t 
cli_dg2_get_dqs0_en_tap(const dram_gen2_info_t *info) {
    return (info->dider >>24) & 0x1F;
}
//dqs1_en_hclk
SECTION_ON_FLASH static int
cli_dg2_set_dqs1_en_hclk(dram_gen2_info_t *info, u32_t en) {
    info->dider = (info->dider & 0xFF7FFFFF ) | (en<<23);
    return 0;
}
SECTION_ON_FLASH static u32_t 
cli_dg2_get_dqs1_en_hclk(const dram_gen2_info_t *info) {
    return (info->dider >>23) & 0x1;
}
//dqs1_en_tap
SECTION_ON_FLASH static int
cli_dg2_set_dqs1_en_tap(dram_gen2_info_t *info, u32_t tap) {
    info->dider = (info->dider & 0xFFE0FFFF ) | (tap<<16);
    return 0;
}
SECTION_ON_FLASH static u32_t 
cli_dg2_get_dqs1_en_tap(const dram_gen2_info_t *info) {
    return (info->dider >>16) & 0x1F;
}

//dqs0_group_tap
SECTION_ON_FLASH static int
cli_dg2_set_dqs0_group_tap(dram_gen2_info_t *info, u32_t val) {
    info->daccr= (info->daccr & 0xFFE0FFFF ) | (val<<16);
    return 0;
}
SECTION_ON_FLASH static u32_t 
cli_dg2_get_dqs0_group_tap(const dram_gen2_info_t *info) {
    return (info->daccr >>16) & 0x1F;
}

//dqs1_group_tap
SECTION_ON_FLASH static int
cli_dg2_set_dqs1_group_tap(dram_gen2_info_t *info, u32_t val) {
    info->daccr = (info->daccr & 0xFFFFE0FF ) | (val<<8);
    return 0;
}
SECTION_ON_FLASH static u32_t 
cli_dg2_get_dqs1_group_tap(const dram_gen2_info_t *info) {
    return (info->daccr >>8) & 0x1F;
}

// pattern generation
SECTION_ON_FLASH void
board_dram_write_pattern_generation(const dram_gen2_info_t *info,
    unsigned int test_times, unsigned int string_pattern) {
	volatile unsigned int *p_tar_addr;
	unsigned int test_size, i = 0, offset = cli_dg2_memctl_get_DRAM_size(info)/8;
	
	pblr_dc_flushall(); //_cache_flush();
	while(test_times){
		/* Write data */
		p_tar_addr = (volatile unsigned int *)(0xA0000000+ i*offset);
		test_size = 0x100000;
		while(test_size-=4){
			*p_tar_addr++ = string_pattern;
			test_times--;
			if(test_times == 0)
				break;
		}
		i = (i+1) % 8;
		pblr_puts("write \r/");
	}
}

SECTION_ON_FLASH void
board_dram_read_pattern_generation(const dram_gen2_info_t *info,unsigned int test_times) {
	volatile unsigned int *p_tar_addr, read_tmp;
	unsigned int test_size, i = 0, offset = cli_dg2_memctl_get_DRAM_size(info)/8;
	
	pblr_dc_flushall(); //_cache_flush();
	while(test_times){
		/* Read data */
		p_tar_addr = (volatile unsigned int *)(0xA0000000+ i*offset);
		test_size = 0x100000;
		while(test_size-=4){
			read_tmp = *p_tar_addr++;
			test_times--;
			if(test_times == 0)
				break;
		}
		i = (i+1) % 8;
		pblr_puts("read \r/");
	}	
}

#if 0
// tables
const str2int_entry_t _ddr_gen2_drv_strength_lookup[]={
    {"normal", 0},
    {"0", 0},
    {"reduced", 1},
    {"1", 1},
    {NULL, 0}
};
#endif

#define define_rv_para(name, _types) _define_rv_para(ddr, name, _types)
#define define_rv_para_tab(name, table, _types) _define_rv_para_tab(ddr, name, table, _types)
#define define_vv_para(name, _get_, _set_, _types) _define_vv_para(ddr, name, _get_, _set_, _types)
#define define_vv_para_str(name, _get_, _set_, _types) _define_vv_para_str(ddr, name, _get_, _set_, _types)


// declares of variables
define_rv_para(mcr, DPT_OMIT);
    define_vv_para(mrs_dll_enable, 
        cli_dg2_memctl_get_mrs_dll_enable(info),
        cli_dg2_memctl_set_mrs_dll_enable(info, v), DPT_DECIMAL);
    define_vv_para(mrs_odt, 
        cli_dg2_memctl_get_mrs_odt(info),
        cli_dg2_memctl_set_mrs_odt(info, v), DPT_DECIMAL);
    define_vv_para(mrs_rtt_wr,
        cli_dg2_memctl_get_mrs_rtt_wr(info),
        cli_dg2_memctl_set_mrs_rtt_wr(info, v), DPT_DECIMAL);
    define_vv_para(mrs_drv_strength,
        cli_dg2_memctl_get_mrs_drv_strength(info),
        cli_dg2_memctl_set_mrs_drv_strength(info, v), DPT_DECIMAL);
define_rv_para(dcr, DPT_OMIT);
    define_vv_para(size, 
        cli_dg2_memctl_get_DRAM_size(info)/1048576, 
        cli_dg2_memctl_set_DRAM_size(info, v), DPT_DECIMAL);
    define_vv_para(buswidth, 
        cli_dg2_memctl_get_DRAM_buswidth(info), 
        cli_dg2_memctl_set_DRAM_buswidth(info, v), DPT_DECIMAL);
define_rv_para(dtr0, DPT_OMIT);   //T_CAS-31:28,T_WR-27:24,T_CWL-23:20,T_RTP-19:16,T_WTR-15:12,T_REFI-11:8,T_REFI_UNIT-7:4
	define_vv_para(tcas,
        cli_dg2_memctl_get_tcas(info),
        cli_dg2_memctl_set_tcas(info, v), DPT_DECIMAL);
	define_vv_para_str(twr,
        cli_dg2_memctl_get_twr(info, buf),
        cli_dg2_memctl_set_twr(info, v), DPT_FORM_STR);
   	define_vv_para_str(tcwl,
        cli_dg2_memctl_get_tcwl(info, buf),
        cli_dg2_memctl_set_tcwl(info, v), DPT_FORM_STR);
   	define_vv_para_str(trtp,
        cli_dg2_memctl_get_trtp(info, buf),
        cli_dg2_memctl_set_trtp(info, v), DPT_FORM_STR);
   	define_vv_para_str(twtr,
        cli_dg2_memctl_get_twtr(info, buf),
        cli_dg2_memctl_set_twtr(info, v), DPT_FORM_STR);
   	define_vv_para_str(trefi,
        cli_dg2_memctl_get_trefi(info, buf),
        cli_dg2_memctl_set_trefi(info, v), DPT_FORM_STR);
#if 0    
   	define_vv_para(trefi_unit,
        cli_dg2_memctl_get_trefi_unit(info),
        cli_dg2_memctl_set_trefi_unit(info, v), DPT_DECIMAL);
#endif
   	define_vv_para_str(trp,
        cli_dg2_memctl_get_trp(info, buf),
        cli_dg2_memctl_set_trp(info, v), DPT_FORM_STR);    
   	define_vv_para_str(trcd,
        cli_dg2_memctl_get_trcd(info, buf),
        cli_dg2_memctl_set_trcd(info, v), DPT_FORM_STR); 
   	define_vv_para_str(trrd,
        cli_dg2_memctl_get_trrd(info, buf),
        cli_dg2_memctl_set_trrd(info, v), DPT_FORM_STR);
   	define_vv_para_str(tfawg,
        cli_dg2_memctl_get_tfawg(info, buf),
        cli_dg2_memctl_set_tfawg(info, v), DPT_FORM_STR); 
   	define_vv_para_str(trfc,
        cli_dg2_memctl_get_trfc(info, buf),
        cli_dg2_memctl_set_trfc(info, v), DPT_FORM_STR); 
   	define_vv_para_str(tras,
        cli_dg2_memctl_get_tras(info, buf),
        cli_dg2_memctl_set_tras(info, v), DPT_FORM_STR);    

define_rv_para(dtr1, DPT_OMIT);   //T_RP-28:24,T_RCD-20:16,T_RRD-12:8,T_FAWG-4:0
define_rv_para(dtr2, DPT_OMIT);   //T_RFC-28:20,T_RAS-18:12
define_rv_para(mpmr0, DPT_OMIT);  //PM_MODE-29:28,T_CKE-27:24,T_RSD-21:12,T_XSREF-9:0
define_rv_para(mpmr1, DPT_OMIT);  //T_XARD-32:28,T_AXPD-27:24
define_rv_para(dider, DPT_OMIT);  //DQS0_EN_HCLK-31,DQS0_EN_TAP-28:24,DQS1_EN_HCLK-23,DQS1_EN_TAP-20:16
    define_vv_para(dqs0_en_hclk, 
        cli_dg2_get_dqs0_en_hclk(info), 
        cli_dg2_set_dqs0_en_hclk(info, v), DPT_DECIMAL);
    define_vv_para(dqs0_en_tap, 
        cli_dg2_get_dqs0_en_tap(info), 
        cli_dg2_set_dqs0_en_tap(info, v), DPT_DECIMAL);
    define_vv_para(dqs1_en_hclk, 
        cli_dg2_get_dqs1_en_hclk(info), 
        cli_dg2_set_dqs1_en_hclk(info, v), DPT_DECIMAL);
    define_vv_para(dqs1_en_tap, 
        cli_dg2_get_dqs1_en_tap(info), 
        cli_dg2_set_dqs1_en_tap(info, v), DPT_DECIMAL);
define_rv_para(d23oscr, DPT_OMIT);//ODT_ALWAYS_ON-31,TE_ALWAYS_ON-30
define_rv_para(daccr, DPT_OMIT);  //AC_MODE-31,DQS_SE-30,DQS0_GROUP_TAP-20:16,DQS1_GROUP_TAP-12:8,AC_DYN_BPTR_CLR_EN-5,AC_BPTR_CLEAR-4,AC_DEBUG_SEL-3:0
    define_vv_para(dqs0_group_tap, 
        cli_dg2_get_dqs0_group_tap(info), 
        cli_dg2_set_dqs0_group_tap(info, v), DPT_DECIMAL);
    define_vv_para(dqs1_group_tap, 
        cli_dg2_get_dqs1_group_tap(info), 
        cli_dg2_set_dqs1_group_tap(info, v), DPT_DECIMAL);
define_rv_para(dacspcr, DPT_OMIT);/* AC_SILEN_PERIOD_EN-31, AC_SILEN_TRIG-20, AC_SILEN_PERIOD_UNIT-19:16, AC_SILEN_PERIOD-15:8, AC_SILEN_LEN-7:0 */  
define_rv_para(dacspar, DPT_OMIT);/* AC_SPS_DQ15R-31, AC_SPS_DQ14R-30, AC_SPS_DQ13R-29, AC_SPS_DQ12R-28, AC_SPS_DQ11R-27, AC_SPS_DQ10R-26, AC_SPS_DQ9R-25, AC_SPS_DQ8R-24, AC_SPS_DQ7R-23, AC_SPS_DQ6R-22, AC_SPS_DQ5R-21, AC_SPS_DQ4R-20, AC_SPS_DQ3R-19, AC_SPS_DQ2R-18, AC_SPS_DQ1R-17, AC_SPS_DQ0R-16, AC_SPS_DQ15F-15, AC_SPS_DQ14F-14, AC_SPS_DQ13F-13, AC_SPS_DQ12F-12, AC_SPS_DQ11F-11, AC_SPS_DQ10F-10, AC_SPS_DQ9F-9, AC_SPS_DQ8F-8, AC_SPS_DQ7F-7, AC_SPS_DQ6F-6, AC_SPS_DQ5F-5, AC_SPS_DQ4F-4, AC_SPS_DQ3F-3, AC_SPS_DQ2F-2, AC_SPS_DQ1F-1, AC_SPS_DQ0F-0 */ 
define_rv_para(DDR1_mr, DPT_OMIT);
define_rv_para(DDR1_emr, DPT_OMIT);
define_rv_para(DDR2_mr, DPT_OMIT);
define_rv_para(DDR2_emr1, DPT_OMIT);
define_rv_para(DDR2_emr2, DPT_OMIT);
define_rv_para(DDR2_emr3, DPT_OMIT);
define_rv_para(DDR3_mr0, DPT_OMIT);
define_rv_para(DDR3_mr1, DPT_OMIT);
define_rv_para(DDR3_mr2, DPT_OMIT);
define_rv_para(DDR3_mr3, DPT_OMIT);
#define define_static_cal_data(n) \
    define_rv_para(static_cal_data_##n, DPT_OMIT); \
    define_vv_para(dq_wr_delay_##n, \
        cli_dg2_memctl_get_dq_wr_delay(info, n), \
        cli_dg2_memctl_set_dq_wr_delay(info, n, v), DPT_DECIMAL)
define_static_cal_data(0); //0xb8001510
define_static_cal_data(1); //0xb8001514
define_static_cal_data(2); //0xb8001518
define_static_cal_data(3); //0xb800151c
define_static_cal_data(4); //0xb8001520
define_static_cal_data(5); //0xb8001524
define_static_cal_data(6); //0xb8001528
define_static_cal_data(7); //0xb800152c
define_static_cal_data(8); //0xb8001530
define_static_cal_data(9); //0xb8001534
define_static_cal_data(10);//0xb8001538
define_static_cal_data(11);//0xb800153c
define_static_cal_data(12);//0xb8001540
define_static_cal_data(13);//0xb8001544
define_static_cal_data(14);//0xb8001548
define_static_cal_data(15);//0xb800154c
define_rv_para(static_cal_data_16, DPT_OMIT);//0xb8001550
define_rv_para(static_cal_data_17, DPT_OMIT);//0xb8001554
define_rv_para(static_cal_data_18, DPT_OMIT);//0xb8001558
define_rv_para(static_cal_data_19, DPT_OMIT);//0xb800155c
define_rv_para(static_cal_data_20, DPT_OMIT);//0xb8001560
define_rv_para(static_cal_data_21, DPT_OMIT);//0xb8001564
define_rv_para(static_cal_data_22, DPT_OMIT);//0xb8001568
define_rv_para(static_cal_data_23, DPT_OMIT);//0xb800156c
define_rv_para(static_cal_data_24, DPT_OMIT);//0xb8001570
define_rv_para(static_cal_data_25, DPT_OMIT);//0xb8001574
define_rv_para(static_cal_data_26, DPT_OMIT);//0xb8001578
define_rv_para(static_cal_data_27, DPT_OMIT);//0xb800157c
define_rv_para(static_cal_data_28, DPT_OMIT);//0xb8001580
define_rv_para(static_cal_data_29, DPT_OMIT);//0xb8001584
define_rv_para(static_cal_data_30, DPT_OMIT);//0xb8001588
define_rv_para(static_cal_data_31, DPT_OMIT);//0xb800158c
define_rv_para(static_cal_data_32, DPT_OMIT);//0xb8001590
    define_vv_para(dqm0, 
        cli_dg2_memctl_get_dqm_delay(info, 0), 
        cli_dg2_memctl_set_dqm_delay(info, 0, v), DPT_DECIMAL);
    define_vv_para(dqm1, 
        cli_dg2_memctl_get_dqm_delay(info, 1), 
        cli_dg2_memctl_set_dqm_delay(info, 1, v), DPT_DECIMAL);
define_rv_para(zq_setting, 0);    
define_rv_para(calibration_type, 0);  //0-static, 1-software(static_cal_data is not used)
define_rv_para(tx_clk_phs_delay, 0);
define_rv_para(clkm_delay, 0);
define_rv_para(clkm90_delay, 0);
define_rv_para(auto_calibration, 0);  //0-disable, 1-enable. Calibration window auto-sliding. 


extern as_para_t cli_ddr_para_begin, cli_ddr_para_end;
SECTION_ON_FLASH int
get_ddr_gen2_info(const dram_gen2_info_t *info, const char *pname, char *buf) {
    //return swp_access_soc_get(info, pname, buf, (&__ddr_para_begin)+1, &__ddr_para_end);
    return swp_access_soc_get(info, pname, buf, &cli_ddr_para_begin, &cli_ddr_para_end);
}
SECTION_ON_FLASH int
set_ddr_gen2_para(dram_gen2_info_t *info, const char *pname, const char *value) {
    //return swp_access_soc_set(info, pname, value, (&__ddr_para_begin)+1, &__ddr_para_end);
    return swp_access_soc_set(info, pname, value, &cli_ddr_para_begin, &cli_ddr_para_end);
}
SECTION_ON_FLASH void
dump_ddr_gen2_info(const dram_gen2_info_t *info, const char *fmt, u32_t flags, u32_t flags_mask) {
    //return swp_access_soc_dump(info, fmt, flags, flags_mask, (&__ddr_para_begin)+1, &__ddr_para_end);
    return swp_access_soc_dump(info, fmt, flags, flags_mask, &cli_ddr_para_begin, &cli_ddr_para_end, 
        ((u32_t)&__ddr_para_auto_calibration)-((u32_t)&__ddr_para_mcr));
}
