
#include "vxWorks.h"
#include "stdio.h"
#include "ioLib.h"
#include "stdlib.h"
#include "string.h"
#include "config.h"
#include <drv/multi/ppc860Siu.h>
#include <arch/ppc/vxPpcLib.h>



#undef  DEBUG_FPGA_DOWNLOAD
//#define FPGA_FILENAME	"/tffs0/fpga.bit"
#define FPGA_FILENAME	"/tffs0/act/msc/fpga.bit"
#define FPGA_IMAGE_SIZE	1600000

STATUS fpgaDownload(void);
STATUS dff(char *fname);
STATUS dfm(unsigned char *buffer,UINT32 size);

/*
// Description: download FPGA image to each FPGA chip
//
// Returns:
//    OK - success; ERROR - failure
*/



STATUS fpgaDownload()

{
	dff(FPGA_FILENAME);
	return  OK;
}


/***********************************************************************
* dff - downloading FPGA bit file to FPGA device by file name.         *
*                                                                      *
* Description:                                                         *
*                                                                      *
*Input:                                                                *
*       char *fname: file name of FPGA bit file.                       *
*                                                                      *
*Output:                                                               *
*       None                                                           *
*                                                                      *
*Return:                                                               *
*       Return OK if successful, otherwise return ERROR.               *
*                                                                      *
*Conditon for use:                                                     *
*                                                                      *
*Internal:                                                             *
*       None                                                           *
*                                                                      *
*Logic:                                                                *
*                                                                      *
*Notes:                                                                *
*                                                                      *
*                                                                      *
***********************************************************************/

STATUS dff(char *fname)
{
	UINT8   *pcFpga;
	STATUS	status = OK;
	UINT32	length = 0;
	FILE	*fd;


	printf("Download FPGA from file: %s\n",fname);

	/* Prepare the FPGA file.*/
	fd = fopen(fname,"r+b");
	if (fd==NULL){
		printf("FPGA download, can't open %s.\n", fname);
		return ERROR;
	}

	pcFpga= (UINT8 *)malloc(FPGA_IMAGE_SIZE);
	if (pcFpga==NULL){
		printf("FPGA download, no enough memory.\n");
		fclose(fd);
		return ERROR;
	}

	fseek(fd, 0, SEEK_SET);

	if ((length= fread(pcFpga, 1, FPGA_IMAGE_SIZE, fd))==0){
		printf("FPGA download, file size error.\n");
		fclose(fd);
		free(pcFpga);
		return ERROR;
	}
	
	printf("FPGA download, file size = %d.\n", length);

        status = dfm(pcFpga, length);
	
	free(pcFpga);
	fclose(fd);
	return status;
}

/***********************************************************************
* dfm - downloading FPGA bit file from memory buffer.                  *
*                                                                      *
*Input:                                                                *
*       buffer: memory buffer for FPGA bit data.                       *
*       size  : size of fpga bit data.                                 *
*                                                                      *
*Output:                                                               *
*       None                                                           *
*                                                                      *
*Return:                                                               *
*       Return OK if successful, otherwise return ERROR.               *
*                                                                      *
***********************************************************************/

STATUS dfm(unsigned char *buffer, UINT32 size)
{
	UINT32	                 i;
	UINT32                   br4, or4;
	UINT16                   pdat;
	UINT8                   *pcFpga;
	STATUS	                 status = OK;
	
	
	pcFpga = (UINT8 *)buffer;
	
#ifdef DEBUG_FPGA_DOWNLOAD
	printf("FPGA buffer download, size = %d.\n", size);*/
#endif	
	
	*(PCDIR(vxImmrGet())) &= (0xF67F); /* PC4,7,8 -- INPUT  */
	*(PCDIR(vxImmrGet())) |= (0x0260); /* PC5,6,9 -- OUTPUT 5-->10*/
	*(PCPAR(vxImmrGet())) &= (0xF03F); /* PC4-9   -- general purpose*/


    		/* setup CS4.*/

	br4 = * BR4(vxImmrGet());
	or4 = * OR4(vxImmrGet());
	
	*BR4(vxImmrGet()) = 0x06000401;
	*OR4(vxImmrGet()) = 0xFF000706;
	

    		/* DOWNLOAD BIN DATA.*/

        	/* pull down program signal, pin PC9. */
        *PCDAT(vxImmrGet()) &= 0xFFBF;
	
    		/*delay 300ns.*/
	for(i=0; i<1000; i++);
        
	pdat = *PCDAT(vxImmrGet());
		/* PC5,PC6, set DCLK,DATA0 for FPGA low  5-->10*/
	*PCDAT(vxImmrGet()) &= 0xFDDF;
	
		/* PC7, nSTATUS from FPGA should be low */
	if (!(pdat & 0x0100)) 
	{
	 	/* pull up program signal, pin C9 */
		*PCDAT(vxImmrGet()) |= 0x0040;
	}
	else 
	{
		printf("dfm()::FPGA not present.\n");
		return ERROR;
	}
	
	
	for(i=0; i<100000; i++) {
	
		/* check if init signal is high, pin D7. */
		pdat = *PCDAT(vxImmrGet());
	
		/* PC7, nSTATUS from FPGA should be high */
		if (pdat & 0x0100)
			break;
	
	}
	
	if (i==100000) {
		/* init signal is low.*/
		printf("FPGA init signal is low.\n");
		status = ERROR;
	}

	if (status==OK)
	{ 
		/* init signal is high. */
#ifdef DEBUG_FPGA_DOWNLOAD
		printf("FPGA init signal is high.\n");
#endif


{/****************************************************************/
 /* download to FPGA bit by bit, LSB first to PC6, DATA0         */
 /****************************************************************/
		FAST unsigned int nBit;

		for(i=0; i<size; i++)
		{
			for (nBit = 0; nBit < 8; nBit++)
			{
				if (pcFpga[i] & (0x01 << nBit))
					*PCDAT(vxImmrGet()) |= 0x0200;
				else
					*PCDAT(vxImmrGet()) &= 0xFDFF;

					/* data set up time before rising edge on DCLK, min 7 nm */
					/* PD6, generate Clock for FPGA, high time min 4nm 5-->10*/
				*PCDAT(vxImmrGet()) |= 0x0020;
				*PCDAT(vxImmrGet()) &= 0xFFDF;
			}/*one byte end */
		}
}/****************************************************************/
		/* check if init signal is high, pin D7==1.
		   if error occurs during writing, pin D7=0.
		   if init signal is high, continue to check done flag.
		*/
			/* PC7, nSTATUS from FPGA should be high(no error) */
		if ((*PCDAT(vxImmrGet()) & 0x0100) != 0) {

			/* check done flag, success if pin C8==1.*/
			for(i=0; i<1000000; i++){
				pdat = *PCDAT(vxImmrGet());
				if (pdat&0x0080) {
#ifdef DEBUG_FPGA_DOWNLOAD
					printf("FPGA done flag is high.\n");
#endif
					break;
				}
			}

			if (i>=1000000) {
				printf("FPGA done flag is low.\n");
				status = ERROR;
			}

		}
		else {
			printf("FPGA programming error,init signal is low.\n");
			status = ERROR;
		}
		
	} 



    /* restore CS4.*/
    /*improve the read/write timing based on Steve's comments:
      for read, let address come out much earlier than /CS.
      Data is latched into CPU at the rising edge of /CS which
      is very close to /OE in this case.
    */

	*OR4(vxImmrGet()) = or4; /*0xffe00f24; PONFPGA_OR4_VAL;*/
	*BR4(vxImmrGet()) = br4;

	if(status == OK)
    
		printf("FPGA programming completed.\n");
	else
		printf("FPGA programming failed.\n");

	return status;
}

