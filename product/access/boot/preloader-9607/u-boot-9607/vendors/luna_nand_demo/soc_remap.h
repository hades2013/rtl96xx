/* RTL8686 remap register */
struct soc_reg_remap_t {
	unsigned int SOC_ID;
	unsigned int SOC_SUB_TYPE;
	unsigned int SOC_IO_MODE_EN; /* IO_MODE_EN : Enable interface IO */
	unsigned int SOC_I2C_IND_CMD; /*I2C master indirect command and address*/
	unsigned int SOC_I2C_IND_RD; /* I2C master indirect read data */
	unsigned int SOC_GPHY_IND_WD;/*GPHY indirect write data */
	unsigned int SOC_GPHY_IND_CMD; /*GPHY indirect command and address*/
	unsigned int SOC_GPHY_IND_RD;	/*GPHY indirect read data*/
	
	unsigned int SOC_GPIO_CTRL_0;
	unsigned int SOC_GPIO_CTRL_1;
	unsigned int SOC_GPIO_CTRL_2;
	unsigned int SOC_GPIO_CTRL_4;

	unsigned int SOC_IO_LED_EN;
	unsigned int SOC_LED_EN;
	unsigned int SOC_LED_BLINK_RATE_CFG;
	unsigned int SOC_LOW_RATE_BLINK_CFG;

	unsigned int SOC_SYS_PKT_BUF_CTRL;
	
} ;




