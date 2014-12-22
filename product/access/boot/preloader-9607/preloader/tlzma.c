#include <preloader.h>
#include <lzma/LzmaDec.h>

void *heap_base = 0;

void *SzAlloc(void *p __attribute__((__unused__)),
              size_t size) {
	void *cur = heap_base;

	heap_base += size;
	
	return cur;
}

void SzFree(void *p       __attribute__((__unused__)),
            void *address __attribute__((__unused__))) {
	return;
}

/************************************************
  LZMA compressed file format (lzma.txt)
  ---------------------------
  Offset Size Description
  0      1   Special LZMA properties (lc,lp, pb in encoded form)
  1      4   Dictionary size (little endian)
  5      8   Uncompressed size (little endian). -1 means unknown size
  13     -   Compressed data
************************************************/
//#define LZ_PROP    0
//#define LZ_DICT_SZ 1
#define LZ_UCOMP_SZ 5
#define LZ_COMP_DAT 13

#define LAYOUTI parameters.soc.layout

s32_t decompress(u8_t *src, u8_t *dest, void *heap) {
	s32_t ret;

	u32_t srcLen = ((uimage_header_t *)(LAYOUTI.bootloader1_addr + FLASH_BASE))->ih_size;
	/* "Uncompressed data size" is little endian in lzma header. */
	u32_t desLen = endian_swap(src+LZ_UCOMP_SZ);
	ELzmaStatus status;
	ISzAlloc myalloc = {SzAlloc, SzFree};

	heap_base = heap;

	ret = LzmaDecode(dest, &desLen,
	                 src+LZ_COMP_DAT, &srcLen,
	                 src, 5,
	                 LZMA_FINISH_ANY, &status, &myalloc);

	return ret;
}
