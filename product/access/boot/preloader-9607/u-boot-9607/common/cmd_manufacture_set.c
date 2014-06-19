#include <common.h>
#include <command.h>
#include <linux/ctype.h>
#include "../../../../include/lw_config.h"

#ifdef CONFIG_CMD_MACSET

#define size_MacId 6
#define size_SerNum 32
/*
 * getnext -- get the next token
 *
 * Parameters:
 *   src: pointer to the start of the source string
 *   separater: the symbol used to separate the token
 *   dest: destination of the next token to be placed
 *
 * Returns:
 *   length of token (-1 when failed)
 */
int getnext (char *src,int separator,char *dest)
{
    char *c;
    int len = 0;

    if ( (src == NULL) || (dest == NULL) ) {
        return -1;
    }

    c = strchr(src, separator);
    if (c == NULL) {
        strcpy(dest, src);
        return -1;
    }
    len = c - src;
    strncpy(dest, src, len);
    dest[len] = '\0';
    return len + 1;
}
int str_to_mac (unsigned char *mac,char *str)
{
    int     len;
    char    *ptr = str;
    char    buf[128], tmp[128];
    int     i;

    for (i = 0; i < 5; ++i)
    {
        if ((len = getnext(ptr, ':', buf)) == -1 &&
            (len = getnext(ptr, '-', buf)) == -1)
        {
            return -1; /* parse error */
        }

        if (len != 3) return -1;

        strcpy(tmp, "");
        strcpy(tmp, buf);

        if (NULL != strtok(tmp, "0123456789ABCDEFabcdef")) return -1;

        mac[i] = simple_strtoul(buf, NULL, 16);
        ptr += len;
    }

    if (strlen(ptr) != 2) return -1;
    strcpy(tmp, "");
    strcpy(tmp, ptr);

    if (NULL != strtok(tmp, "0123456789ABCDEFabcdef")) return -1;

    mac[5] = simple_strtoul(ptr, NULL, 16);

    return 0;
}
int Check_MAC(uchar *mac_addr)
{
    uchar MAC_buf[size_MacId];
    int valid_flag=1;
    
    memset(MAC_buf,0xff,size_MacId);//check all 0xff MAC
    if(!memcmp(mac_addr,MAC_buf,size_MacId))
    {
    	valid_flag=0;
    }
    memset(MAC_buf,0x00,size_MacId);//check all 0 MAC
    if(!memcmp(mac_addr,MAC_buf,size_MacId))
    {
    	valid_flag=0;
    }
    if((mac_addr[0])&0x01)//check multicast MAC
    {
    	valid_flag=0;		
    }
    
    return valid_flag;
}

int do_manufacture_set ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{    
    uchar mac[size_MacId] = {0};
    int ptr = 0;
    char  SerialNoGet[size_SerNum+1]={0};
    
    if (3 != argc)
    {
        printf("Usage: manufacture {mac|sn} {xx:xx:xx:xx:xx:xx|xxxxxx}.\n");
        return 0;
    }
    
    if (0 == strcmp("mac",argv[1]))
    {
        if (0 == str_to_mac(mac,argv[2]))
        {
            if (Check_MAC(mac))
            {
                setenv("ethaddr",argv[2]);
            }
            else
            {
                printf("broadcast multicast and all 0 MAC are not allowed!\n");
            }
        }
        else
        {
            printf("Input MAC address check error!\n");
        }
    }
    else if (0 == strcmp("sn",argv[1]))
    {
        if(strlen(argv[2]) != (strlen(CONFIG_PRODUCT_NAME) + 1 + 9))
        {
            printf("Invalid serial number lengths.\n");
            return 0;            
        }
        else
        {
            strcpy(SerialNoGet,argv[2]);
        }
        
        if(!strstr(SerialNoGet, CONFIG_PRODUCT_NAME) )
        {
            printf("Product model checked error in your serial number.\n");
            return 0;                                  
        }

        for(ptr = (strlen(CONFIG_PRODUCT_NAME) + 1); ptr < strlen(argv[2]); ptr++) //modify by dengjian 2012-07-23 for u-boot ST
        {
            if(!isdigit(SerialNoGet[ptr]))
            {
                printf("Serial number format error.\n");
                return 0;
            }
        }
        
        setenv("SN",argv[2]);
    }
    else
    {
        printf("Usage: manufacture {mac|sn} {xx:xx:xx:xx:xx:xx|xxxxxx}.\n");
    }
	return 0;
}


/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	manufacture,	3,	1,	do_manufacture_set,
	"set MAC address or SN.",
	NULL
);

#endif	/* CONFIG_CMD_MACSET */

