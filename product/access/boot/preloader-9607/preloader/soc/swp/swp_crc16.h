#ifndef SWP_CRC16_H
#define SWP_CRC16_H

#include <soc.h>
extern const u16_t crc16_coff[];
#define CRC16_CCITT_ADD(crc, data) ((crc >> 8) ^ crc16_coff[(crc ^ (data)) & 0xff])
static inline u16_t crc16_ccitt_add_byte(u16_t pre_crc, u8_t cur_data) {return CRC16_CCITT_ADD(pre_crc, cur_data);}
u16_t crc16_ccitt_add_buffer(u16_t crc, const u8_t *data, int length);

#endif //SWP_CRC16_H
