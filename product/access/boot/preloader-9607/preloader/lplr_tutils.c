#include <preloader.h>

void *memcpy(u8_t *dest, const u8_t *src, u32_t n) {
	if (n == 0) return dest;
	do {
		*dest++ = *src++;
	} while (--n != 0);
	return dest;
}
