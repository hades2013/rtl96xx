#ifndef __MTD_UTILS_H__
#define __MTD_UTILS_H__


#define MTD_BOOT "/dev/mtd0"
#define MTD_KERNEL "/dev/mtd1"
#define MTD_ROOTFS "/dev/mtd2"

#define MTD_APP_0_KERNEL "/dev/mtd3"
#define MTD_APP_0_ROOTFS "/dev/mtd4"
#define MTD_APP_1_KERNEL "/dev/mtd5"
#define MTD_APP_1_ROOTFS "/dev/mtd6"

#define MTDBLOCK_BOOT "/dev/mtdblock0"
#define MTDBLOCK_KERNEL "/dev/mtdblock1"
#define MTDBLOCK_ROOTFS "/dev/mtdblock2"
#define MTDBLOCK_APP_0 "/dev/mtdblock3"
#define MTDBLOCK_APP_1 "/dev/mtdblock4"

typedef enum{
	MTD_OP_OK = 0,
	MTD_OP_NOMEM,
	MTD_OP_INVALID,
	MTD_OP_UNKNOWN,
	MTD_OP_IOERR,
	MTD_OP_VERIFY_LEN,
	MTD_OP_VERIFY_CRC
}
mtd_op_status_t;

typedef enum{
	MTD_ERASING = 0,
	MTD_WRITING,
	MTD_VERIFING,
	MTD_DONE
}
mtd_state_t;

typedef void (*mtd_on_state_change_t)(mtd_state_t state, int error, int process);


int mtd_write_start(const char *mtd_path, unsigned char *buffer, uint32_t size, mtd_on_state_change_t on_state_change);


#endif /*#ifndef __MTD_UTILS_H__*/

