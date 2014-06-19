#include <stdio.h>
#if 1
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#endif

#include "../../include/lw_config.h"

#if 1
/*FMW_HEADLEN must be 4 aligned*/

#define SWAP_32(data)        ((((unsigned int)(data) & 0x000000FF) << 24) | \
                             (((unsigned int)(data) & 0x0000FF00) << 8)  | \
                             (((unsigned int)(data) & 0x00FF0000) >> 8)  | \
                             (((unsigned int)(data) & 0xFF000000) >> 24))

#define	min(a, b)	((a) < (b) ? (a) : (b))

#include "app_head.h"

#define ER5200_MODEL (0x01)
#define ER5100_MODEL (0x02)
#define ER3260_MODEL (0x03)
#define ER3200_MODEL (0x04)
#define ER3100_MODEL (0x05)

#define ICG1000_MODEL (0x06)
#define ICG1800_MODEL (0x07)
//#define ER3280G_MODEL (0x08)

#define NEW_MODEL  (0xaa)

#define TOTAL_FMW_HEADLEN sizeof(struct fmw_hdr_t)
/*
输入参数举例：

mkfw ER5200V100R001D001(L00).bin -k kernel_file -f fs_file -s kernel_size

*/


#endif

int main(int argc, char **argv)
{
	
#if 1	
	FILE *ifp_k=NULL, *ifp_f=NULL, *ofp=NULL;
	unsigned char buf;	
	unsigned int i, count;
	unsigned int checksum;
	unsigned int checksum_tmp;
	unsigned int checksum_tmp_i;
	unsigned int ibuf;
	unsigned int ifp_filelen;
	unsigned int fbuf[256], *ipBuf;
	char *       kernel_ptr = 0;
	unsigned int kernel_size = 0;
	
	time_t this_time;
	//struct tm * local_tp;
		
        struct fmw_hdr_t fmw_hdr;
        struct addi_hdr_t addi_hdr;
        unsigned int addi_hdr_index;
        
	char source[128];
	char product_name[PRODUCT_VER_STR_LEN_MAX];
	char version_str[PRODUCT_VER_STR_LEN_MAX];
	char *cpVersion = NULL;
	char *cpVersion_2 = NULL;
#endif

        //printf("hello.\n");
	 //printf("world!\n");
        
        
	if (( argc != 6)&&( argc != 8) )
	{
	    printf("Usage: make a bin used for software update. \n");
	    printf("  mkfw bin_file_name [-k kernel_file ] [-f fs_file ] [-s kernel_size]\n");
		
	    return -1;
	}
	
#if 1
	if ( (argv[2][0] != '-') || ( (argv[2][1] != 'k') && (argv[2][1] != 'f') )) 
	{
		printf("Invalid system type\n");
	       return -1;
	}

        if ( argc == 8)
        {
	        if ( (argv[4][0] != '-') || (argv[4][1] == argv[2][1]) || ( (argv[4][1] != 'k') && (argv[4][1] != 'f') )) 
	        {
 	        	printf("Invalid system type\n");
	              return -1;
	        }

	        kernel_ptr = argv[7];
	    }
        else
        {
            kernel_ptr = argv[5];
        }

        /*获取内核的分块长度*/
        kernel_size = 0;
        for(i=0; i<strlen(kernel_ptr); i++)
        {
            kernel_size = (kernel_size * 10) + (kernel_ptr[i] - '0');
        }
        printf("kernel_size=%d\n",kernel_size);
	
        memset(&fmw_hdr,0xff,sizeof(fmw_hdr));  	
	fmw_hdr.kernellen = SWAP_32(0);
	fmw_hdr.fslen = SWAP_32(0);
	fmw_hdr.kernel_type = NULL_TYPE;
	fmw_hdr.fs_type = NULL_TYPE;
	
	
      	    strcpy(source, argv[1]);
      	    //strcpy(source, "ER5200V100D002P01.bin");
      	    cpVersion_2 = source;
      	    cpVersion = strrchr(cpVersion_2, '.');
#if 0
	    if (NULL != cpVersion)
	        *cpVersion = 0;
		
           if (PRODUCT_VER_STR_LEN_MAX > strlen(cpVersion_2))
	        strcpy((char *)fmw_hdr.product_version_str, cpVersion_2);
	    else
	    {
	        printf("version string length err. \n");
	        return -1;
	    }
		
      	    cpVersion = source;
      	    cpVersion = strrchr(cpVersion, 'V'); 
	    if ((NULL == cpVersion) ||(source == cpVersion))
	    {
	        printf("version V absence err or position err. \n");
	        return -1;
	    }

	    cpVersion_2 = cpVersion;		
      	    cpVersion_2 = strrchr(cpVersion_2, 'R'); 
	    if (NULL == cpVersion_2)
	    {
	        printf("version R absence err or position err. \n");
	        return -1;
	    }

	    if (4 != (cpVersion_2 - cpVersion))
	    {
	        printf("version VxxxDxxx format err. \n");
	        return -1;
	    }

           if (4 < strlen(cpVersion_2))
           {
               if (('P' != *(cpVersion_2+4)) && ('L' != *(cpVersion_2+4)))
               {
	            printf("version DxxxP(L)xx format err. \n");
	            return -1;                   
               }
           }
           else if (4 > strlen(cpVersion_2))
           {
	        printf("version Dxxx format err. \n");
	        return -1;
           }
		
      	    strcpy(version_str, cpVersion);

#endif
      	    *cpVersion = 0;
      	    strcpy(product_name, source);

			memset(fmw_hdr.product_version_str, 0, PRODUCT_VER_STR_LEN_MAX);
			memcpy(fmw_hdr.product_version_str, CONFIG_PRODUCT_NAME, strlen(CONFIG_PRODUCT_NAME));
      	
      	    cpVersion++;				
      	    if ((('2' <= *cpVersion) && ('9' >= *cpVersion))||('%' == *cpVersion))
      	    {
	        fmw_hdr._model = NEW_MODEL;
      	    }
	    else
	    {
		if (strcmp(product_name, "ER5200") == 0)
		{
			fmw_hdr._model = ER5200_MODEL;				
		}
		else if (strcmp(product_name, "ER5100") == 0)
		{
			fmw_hdr._model = ER5100_MODEL;
		}
		else if (strcmp(product_name, "ER3260") == 0)
		{
			fmw_hdr._model = ER3260_MODEL;
		}
		else if (strcmp(product_name, "ER3200") == 0)
		{
			fmw_hdr._model = ER3200_MODEL;
		}
		else if (strcmp(product_name, "ER3100") == 0)
		{
			fmw_hdr._model = ER3100_MODEL;
		}
		else if (strcmp(product_name, "ICG1000") == 0)
		{
			fmw_hdr._model = ICG1000_MODEL;
		       fmw_hdr._patch = 0;  /*not return to D001 for icg1000. */
		}
		else if (strcmp(product_name, "ICG1800") == 0)
		{
			fmw_hdr._model = ICG1800_MODEL;
		}
		//else if (strcmp(product_name, "ER3280G") == 0)
		//{
		//	fmw_hdr._model = ER3280G_MODEL;
		//}		
		else
		{
	           /*default process*/
		    fmw_hdr._model = NEW_MODEL;
		}
	    }

		   


	    #if 0                   	
        memset(&addi_hdr,0,sizeof(addi_hdr));  
        addi_hdr.str_mark[0] = 'H';
        addi_hdr.str_mark[1] = '3';
        addi_hdr.str_mark[2] = 'C';
        addi_hdr.str_mark[3] = '_';
        addi_hdr_index = 4;
	    #endif
        
	time(&this_time);
	fmw_hdr.timestamp = SWAP_32(this_time);
	//local_tp = localtime(&time);
	
	
	fmw_hdr.headlen = SWAP_32(FMW_HEADLEN);		
	
	
	/*kernel or fs version */
	for (i = 2 ; i < (unsigned int)argc ; i += 2)
	{
		if (argv[i][1]=='k')
		{
			ifp_k = fopen(argv[i+1], "rb");
			if ( !ifp_k ) {
				printf("Can not open destination kernel file \n");
				if (ifp_f)  fclose(ifp_f);
	                     return -1;
			}
			
			fseek(ifp_k, 0, SEEK_END);
			ifp_filelen = ftell(ifp_k);
		    /*动态的内核分块长度*/
			if(0 == kernel_size)
			{
			    printf("DYNAMIC PARTITION kernel.\n");
    			fmw_hdr.kernellen = SWAP_32(((ifp_filelen + TOTAL_FMW_HEADLEN + 64*1024 - 1)&(~(64*1024 - 1))) - TOTAL_FMW_HEADLEN);  /*64K 字节对齐*/
			}
			/*固定的内核分块长度*/
			else
			{
			    printf("STATIC PARTITION kernel pad.\n");
    			fmw_hdr.kernellen = SWAP_32(((kernel_size + 64*1024 - 1)&(~(64*1024 - 1))) - TOTAL_FMW_HEADLEN);  /*64K 字节对齐*/
			}
			printf("kernel file len: 0x%x\n", SWAP_32(fmw_hdr.kernellen));
			fseek(ifp_k, 0, SEEK_SET);
			
            #if 0
			addi_hdr.str_mark[addi_hdr_index] = '+';
			addi_hdr_index++;
			addi_hdr.str_mark[addi_hdr_index] = 'k';
			addi_hdr_index++;
			#endif
            
			fmw_hdr.kernel_type = KERNEL_TYPE;
		}
		else if (argv[i][1]=='f')
		{
			ifp_f = fopen(argv[i+1], "rb");
			if ( !ifp_f ) {
				printf("Can not open destination fs file \n");
				if (ifp_k)  fclose(ifp_k);
	                     return -1;
			}
			
			fseek(ifp_f, 0, SEEK_END);
			ifp_filelen = ftell(ifp_f);
			fmw_hdr.fslen = SWAP_32((ifp_filelen + 3)&(~3));  /*4字节对齐*/
			printf("fs file len: 0x%x\n", SWAP_32(fmw_hdr.fslen));
			fseek(ifp_f, 0, SEEK_SET);

            #if 0
			addi_hdr.str_mark[addi_hdr_index] = '+';
			addi_hdr_index++;
			addi_hdr.str_mark[addi_hdr_index] = 'f';
			addi_hdr_index++;
			#endif
            
			fmw_hdr.fs_type = FS_TYPE;
		}
		else if(argv[i][1]=='s')
		{
		    continue;
		}
		else 
		{
			/*default process*/
			if (ifp_f)  fclose(ifp_f);
			if (ifp_k)  fclose(ifp_k);
		}
		
	}
	
	
	fmw_hdr.imgsize = SWAP_32((SWAP_32(fmw_hdr.headlen) 
	       + SWAP_32(fmw_hdr.kernellen) + SWAP_32(fmw_hdr.fslen) + 3)&(~3));
	
	fmw_hdr.checksum = SWAP_32(0);

	ofp = fopen(argv[1], "wb");
	if ( !ofp ) {
		printf("Can not open destination file\n");
	        return -1;
	}
    
    #if 0
	count = 0;
	addi_hdr.imgsize = SWAP_32(fmw_hdr.imgsize); /*need big endian*/
	if ((i = fwrite(&addi_hdr, 1, sizeof(addi_hdr), ofp))!= ADDI_HEADLEN)
	{
		printf("i = %d\n, sizeof(addi_hdr)= %d\n", i, (int)sizeof(addi_hdr));
		printf("write addition header error.\n");
		goto endd;
	}
	count +=i;
	#endif
    
	count = 0;
	if ((i = fwrite(&fmw_hdr, 1, sizeof(fmw_hdr), ofp))!= FMW_HEADLEN)
	{
		printf("i = %d\n, sizeof(fmw_hdr)= %d\n", i, (int)sizeof(fmw_hdr));
		printf("write file header error.\n");
		goto endd;
	}
	count +=i;

	count = TOTAL_FMW_HEADLEN;
	checksum_tmp = 0;
	checksum = 0;
	checksum_tmp_i = 0;
    
	if (ifp_k)
	{
		fseek(ifp_k, 0, SEEK_SET);
		while(!feof(ifp_k)) {
			if (fread(&buf, 1, 1, ifp_k)!= 1)
			{
				//printf("fread num = 0x%x\n",fread(&buf, 1, 1, ifp_k));
				if (!feof(ifp_k))  printf("kernel file read error.\n");   /*这个条件，判断文件是否正常读完，为什么*/
				break;
			}
			
			checksum_tmp = (checksum_tmp<<8)|((unsigned int)buf);  /*little endian mode*/
			checksum_tmp_i++;
			if (checksum_tmp_i >=4)
			{
    		    checksum += checksum_tmp;
    		    checksum_tmp_i = 0;
			}
			
			
			if(fwrite(&buf, 1, 1, ofp)!=1)
			{
				printf("target file write kernel data error.\n");
				break;
			}
			count++;
			
		}
	}

       //printf("deddd-1\n");
	while(count % 4 != 0)
	{
		buf = 0;
		
		checksum_tmp = (checksum_tmp<<8)|((unsigned int)buf);  /*little endian mode*/
		checksum_tmp_i++;
		if (checksum_tmp_i >=4)
		{
    		checksum += checksum_tmp;
    		checksum_tmp_i = 0;
	    }
			
		if(fwrite(&buf, 1, 1, ofp)!=1)
		{
			printf("target file write kernel pad error.\n");
			break;
		}
		count++;		
	}

    /*动态的内核分块长度*/
	if(0 == kernel_size)
	{
	    printf("DYNAMIC PARTITION kernel pad.\n");
    	while(count % (64*1024) != 0)
    	{
    		buf = 0;
    		
    		checksum_tmp = (checksum_tmp<<8)|((unsigned int)buf);  /*little endian mode*/
    		checksum_tmp_i++;
    		if (checksum_tmp_i >=4)
    		{
        		checksum += checksum_tmp;
        		checksum_tmp_i = 0;
    	    }
    			
    		if(fwrite(&buf, 1, 1, ofp)!=1)
    		{
    			printf("target file write kernel pad error.\n");
    			break;
    		}
    		count++;		
    	}
	}
	/*固定的内核分块长度*/
	else
	{
	    printf("STATIC PARTITION kernel pad.\n");
    	printf("count = 0x%x, \nkernel_size = 0x%x\n", count,  kernel_size);
    	while(count < kernel_size)
    	{
    		buf = 0;
    		
    		checksum_tmp = (checksum_tmp<<8)|((unsigned int)buf);  /*little endian mode*/
    		checksum_tmp_i++;
    		if (checksum_tmp_i >=4)
    		{
        		checksum += checksum_tmp;
        		checksum_tmp_i = 0;
    	    }
    			
    		if(fwrite(&buf, 1, 1, ofp)!=1)
    		{
    			printf("target file write kernel pad error.\n");
    			break;
    		}
    		count++;		
    	}
	}
	
	
    //printf("deddd-2\n");
    //count -= ADDI_HEADLEN;
	checksum_tmp = 0;
	checksum_tmp_i = 0;
	
	if (ifp_f)
	{
       	//printf("deddd-33\n");
		fseek(ifp_f, 0, SEEK_SET);
		while(!feof(ifp_f)) {
			if (fread(&buf, 1, 1, ifp_f)!= 1)
			{
				if (!feof(ifp_f))  printf("fs file read error.\n");
				break;
			}
			
			checksum_tmp = (checksum_tmp<<8)|((unsigned int)buf);  /*little endian mode*/
			checksum_tmp_i++;
			if (checksum_tmp_i >=4)
			{
    		    checksum += checksum_tmp;
    		    checksum_tmp_i = 0;
			}
			
			if(fwrite(&buf, 1, 1, ofp)!=1)
			{
				printf("target file write fs data error.\n");
				break;
			}
			count++;
			
		}
	}
	
       //printf("deddd-3\n");
	while(count % 4 != 0)
	{
		buf = 0;
		
		checksum_tmp = (checksum_tmp<<8)|((unsigned int)buf);  /*little endian mode*/
		checksum_tmp_i++;
		if (checksum_tmp_i >=4)
		{
    		checksum += checksum_tmp;
    		checksum_tmp_i = 0;
	    }
		
		if(fwrite(&buf, 1, 1, ofp)!=1)
		{
			printf("target file write fs pad error.\n");
			break;
		}
		count++;		
	}

       //printf("deddd-4\n");
	printf("count = 0x%x, \nfmw_hdr.imgsize = 0x%x\n", count,  SWAP_32(fmw_hdr.imgsize));
	if (count != SWAP_32(fmw_hdr.imgsize))
	{
		printf("target file length can't match with header.\n");
		goto endd;
	}
	
	
	
	fseek(ofp, 0, SEEK_END);
	ifp_filelen = ftell(ofp);
	printf("target file len: 0x%x\n", ifp_filelen);
	fseek(ofp, 0, SEEK_SET);
		
#if 0	
	fseek(ofp, 0, SEEK_SET);
	checksum = 0;

    memset(&fbuf, 0, sizeof(fbuf));
#endif

       ipBuf=(unsigned int *)&fmw_hdr;
       i = FMW_HEADLEN;
       while(i)
       {
       	checksum += SWAP_32(*ipBuf);
       	ipBuf++;
       	i -=4;
       }

#if 0
       i = 0;
	if (ifp_k)
	{
		fseek(ifp_k, 0, SEEK_SET);
		while(!feof(ifp_k)) {
			if (fread(&fbuf[i], 1, 4, ifp_k)!= 4) 	 /*fread(&fbuf[i], 4, 1, ifp_k) 	就出错，为什么*/
			{
				//printf("fread num = 0x%x\n",fread(&buf, 1, 1, ifp_k));
				//printf("buf = 0x%x\n", buf);
				//printf("i = 0x%x\n", i);
				if (!feof(ifp_k))  
				{
					printf("kernel file read error for checksum.\n");   
					goto endd;
				} 
				break;
			}
			checksum += SWAP_32(fbuf[i]);
			i++;
			i &= (256-1);			
		}
	}

	
       i = 0;
	if (ifp_f)
	{
		fseek(ifp_f, 0, SEEK_SET);
		while(!feof(ifp_f)) {
			if (fread(&fbuf[i], 1, 4, ifp_f)!= 4)
			{
				//printf("fread num = 0x%x\n",fread(&buf, 4, 1, ifp_k));
				if (!feof(ifp_f))  
				{
					printf("fs file read error for checksum.\n");   
					goto endd;
				}
				break;
			}
			checksum += SWAP_32(fbuf[i]);
			i++;
			i &= (256-1);			
		}
	}
	
#endif
		
	checksum = SWAP_32(~checksum);
	
	//fseek(ofp, (unsigned long)(&fmw_hdr.checksum)-(unsigned long)(&fmw_hdr)+ADDI_HEADLEN, SEEK_SET);
	fseek(ofp, (unsigned long)(&fmw_hdr.checksum)-(unsigned long)(&fmw_hdr), SEEK_SET);
	if (fwrite(&checksum, 1, 4, ofp)!= 4)		/*(fwrite(&checksum, 4, 1, ofp)    就出错，为什么*/
	{
		printf("target file write checksum error.\n");
		goto endd;
	}
	
	printf("Firmware convertion done!\n");

endd:
	
	fclose(ofp);
	if (ifp_f) fclose(ifp_f);
	if (ifp_k) fclose(ifp_k);

	return 0;
	
#endif

}
