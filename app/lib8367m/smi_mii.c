#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <linux/mii.h>
#include <linux/sockios.h>


#include <rtk_api.h>
#include <rtk_api_ext.h>
#include <rtk_error.h>

#define MDC_MDIO_DUMMY_ID           0
#define MDC_MDIO_CTRL0_REG          31
#define MDC_MDIO_CTRL1_REG          21
#define MDC_MDIO_ADDRESS_REG        23
#define MDC_MDIO_DATA_WRITE_REG     24
#define MDC_MDIO_DATA_READ_REG      25
#define MDC_MDIO_PREAMBLE_LEN       32

#define MDC_MDIO_ADDR_OP           0x000E
#define MDC_MDIO_READ_OP           0x0001
#define MDC_MDIO_WRITE_OP          0x0003

#define	MII_INTERFACE	"eth1"

void MDC_MDIO_WRITE(int preamble, int dummy_id, uint32 reg, uint32 data)
{
	struct ifreq ifr;
	int sock, ret;
	struct mii_ioctl_data *mii_data;
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		printf("MII Write Operation fail: %s\n", strerror(errno));
		return;
	}
	
	bzero(&ifr, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, MII_INTERFACE, IFNAMSIZ);
	
	mii_data = (struct mii_ioctl_data *)&ifr.ifr_data;
	mii_data->phy_id = dummy_id;
	mii_data->reg_num = reg;
	mii_data->val_in = data & 0xffff;
	
	ret = ioctl(sock, SIOCSMIIREG, (caddr_t *)&ifr);
	if(ret < 0) {
		printf("MII Write Operation fail: %s\n", strerror(errno));
		close(sock);
		return;
	}
	close(sock);
}

void MDC_MDIO_READ(int preamble, int dummy_id, uint32 reg, uint32 *data)
{
	struct ifreq ifr;
	int sock, ret;
	struct mii_ioctl_data *mii_data;
	
	*data = 0;
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		printf("MII Read Operation fail: %s\n", strerror(errno));
		return;
	}
	
	bzero(&ifr, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, MII_INTERFACE, IFNAMSIZ);
	
	mii_data = (struct mii_ioctl_data *)&ifr.ifr_data;
	mii_data->phy_id = dummy_id;
	mii_data->reg_num = reg;
	
	ret = ioctl(sock, SIOCGMIIREG, (caddr_t *)&ifr);
	if(ret < 0) {
		printf("MII Read Operation fail: %s\n", strerror(errno));
		close(sock);
		return;
	}
	*data = mii_data->val_out;
	close(sock);
}


int32 smi_write(uint32 mAddrs, uint32 rData)
{
//	uint32 readback;

    /* Write address control code to register 31 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);

    /* Write address to register 23 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_ADDRESS_REG, mAddrs);

    /* Write data to register 24 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_DATA_WRITE_REG, rData);

    /* Write data control code to register 21 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_WRITE_OP);

//    smi_read(mAddrs, &readback);
//    printf("SMI WRITE: %04x %04x:%04x\n", mAddrs, rData, readback);

    return SUCCESS;
}

int32 smi_read(uint32 mAddrs, uint32 *rData)
{
    /* Write address control code to register 31 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL0_REG, MDC_MDIO_ADDR_OP);

    /* Write address to register 23 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_ADDRESS_REG, mAddrs);

    /* Write read control code to register 21 */
    MDC_MDIO_WRITE(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_CTRL1_REG, MDC_MDIO_READ_OP);

    /* Read data from register 25 */
    MDC_MDIO_READ(MDC_MDIO_PREAMBLE_LEN, MDC_MDIO_DUMMY_ID, MDC_MDIO_DATA_READ_REG, rData);

    return SUCCESS;
}
