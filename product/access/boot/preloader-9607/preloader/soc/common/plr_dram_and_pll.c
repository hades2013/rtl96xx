#include <preloader.h>

#define DTR_REG   (*((volatile u32_t *)(0xB8001008)))
#define DCR_REG   (*((volatile u32_t *)(0xB8001004)))
#define EDTCR_REG (*((volatile u32_t *)(0xB800100C)))
#define DDCR_REG  (*((volatile u32_t *)(0xB8001050)))

/* Return DRAM size in byte. We need not this for now. */
__attribute__((__unused__))
static u32_t get_dram_size(void) {
	const u32_t BUSWID_val[] = {1, 2, 4, 0};
	const u32_t ROWCNT_val[] = {2048, 4096, 8192, 16384};
	const u32_t COLCNT_val[] = {256, 512, 1024, 2048, 4096, 0, 0, 0};
	const u32_t BANKCNT_val[] = {4, 8};

	const u32_t DCR = DCR_REG;
	const u32_t EDTCR = EDTCR_REG;

	const u32_t BUSWID_BYTE = BUSWID_val[(DCR >> 28) & 0x3];
	const u32_t ROWCNT = ROWCNT_val[(DCR >> 25) & 0x3];
	const u32_t COLCNT = COLCNT_val[(DCR >> 22) & 0x7];
	const u32_t BANKCNT = BANKCNT_val[(EDTCR >> 30) & 0x3];

	return (BUSWID_BYTE*ROWCNT*COLCNT*BANKCNT);
}

void dram_and_pll_setup(void) {
	printf("II: DRAM...");
	DTR_REG   = 0x6D0B40C0; //DTR
	DCR_REG   = 0x44880000; //DCR
	EDTCR_REG = 0x78000000; //EDTCR
	DDCR_REG  = 0xC0000000; //DDCR
	printf(" OK\n");

	/* A simple test */
#if (SELFTEST == 1)
	printf("DD: Simple DRAM test... ");
	*((volatile u32_t *)0x80000000) = 0x5AFE5AFE;
	if (*((volatile u32_t *)0x80000000) != 0x5AFE5AFE) {
		printf("$ failed... ");
		while(1);
	}

	*((volatile u32_t *)0xA0000000) = 0x05D650C0;
	if (*((volatile u32_t *)0xA0000000) != 0x05D650C0) {
		printf("Un$ failed.\n");
		while(1);
	} else {
		/* This could be kinda weird, but coming this far means both $ and un$ tests are passed. */
		printf("OK\n");
	}
#endif

	return;
}
