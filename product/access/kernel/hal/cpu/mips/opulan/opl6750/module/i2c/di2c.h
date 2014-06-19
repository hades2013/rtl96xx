#ifndef __DI2C__H
#define __DI2C__H

#define	I2C_MAJOR	60

				// The ioctl commads
#define OPL_I2C_START	1000
#define	OPL_I2C_STOP	1001
#define OPL_I2C_SEND	1002
#define OPL_I2C_READ	1003
#define OPL_I2C_ACK	1004

#define OPL_I2C_TEST	1011

#define OPL_I2C_STATUS	1010
#define OPL_ACK_SET	1
#define OPL_ACK_CLEAR	0

#define MAX_I2C_WORDS	100
typedef struct
{
	unsigned int  count;
	unsigned char words[MAX_I2C_WORDS];
} OPL_i2c_msg;
typedef struct
{
	unsigned char state;
	unsigned char enable;
	unsigned char control;
	unsigned char debug;
}OPL_i2c_status;

#endif
