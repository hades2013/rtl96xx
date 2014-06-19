/*  
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.


    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <linux/reboot.h>

#include <vos.h>
//wfxu #include <opconn_lib.h>
#include "opconn_hw_reg.h"

#include "mtd.h"
#include "defs.h"

#define TRX_MAGIC       0x30524448      /* "HDR0" */
#define BUFSIZE 		(16 * 1024)
#define MAX_ARGS 		8

static char buf[BUFSIZE];
static int buflen;
int quiet = 1;

/*
Finished: return 0
*/
static int mtd_check(int fd)
{
	struct mtd_info_user mtdInfo;
	
	if(ioctl(fd, MEMGETINFO, &mtdInfo)) {
		fprintf(stderr, "Could not get MTD device info\r\n");
		return -1;
	}
	return 0;
}

static int
mtd_unlock(int fd)
{
	struct mtd_info_user mtdInfo;
	struct erase_info_user mtdLockInfo;

	if(ioctl(fd, MEMGETINFO, &mtdInfo)) {
		fprintf(stderr, "Could not get MTD device info \r\n");
		return -1;
	}

	mtdLockInfo.start = 0;
	mtdLockInfo.length = mtdInfo.size;
	if(ioctl(fd, MEMUNLOCK, &mtdLockInfo)) {
        fprintf(stderr, "Could not ioctl MEMUNLOCK \r\n");
		return -1;
	}
		
	return 0;
}

static int
mtd_open(const char *mtd, int flags)
{
	FILE *fp;
	char dev[PATH_MAX];
	int i;
	int ret;

	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, mtd)) {
				snprintf(dev, sizeof(dev), "/dev/mtd/%d", i);
				if ((ret=open(dev, flags))<0) {
					snprintf(dev, sizeof(dev), "/dev/mtd%d", i);
					ret=open(dev, flags);
				}
				fclose(fp);
				return ret;
			}
		}
		fclose(fp);
	}

	return open(mtd, flags);
}


static int
mtd_erase_partition(int fd)
{
	int ret;
	struct mtd_info_user mtdInfo;
	struct erase_info_user mtdEraseInfo;

	if(ioctl(fd, MEMGETINFO, &mtdInfo)) {
		fprintf(stderr, "Could not get MTD device info\r\n");
		return -1;
	}

	mtdEraseInfo.length = mtdInfo.erasesize;

	for (mtdEraseInfo.start = 0;
		 mtdEraseInfo.start < mtdInfo.size;
		 mtdEraseInfo.start += mtdInfo.erasesize) {
		
		ioctl(fd, MEMUNLOCK, &mtdEraseInfo);
		ret = ioctl(fd, MEMERASE, &mtdEraseInfo);
		if(ret) 
			fprintf(stderr, "Failed(%x) to erase block on %s at 0x%x\r\n", ret, mtdEraseInfo.start);
	}		
		 
	return 0;

}

static int mtd_write(int imagefd,FILE* fp,  int fd)
{
	int  i, result;
	size_t r, w, e;
	struct mtd_info_user mtdInfo;
	struct erase_info_user mtdEraseInfo;
	int ret = 0;

	if(ioctl(fd, MEMGETINFO, &mtdInfo)) {
		fprintf(stderr, "Could not get MTD device info from %x\r\n", fd);
		return -1;
	}
	
	r = w = e = 0;
	if (!quiet)
		fprintf(stderr, " [ ]");
	
        if (0!= fseek(fp, 0L, SEEK_END))
  	{
  	   printf( "error :  fseek un succce!\n "); 
  	}
	e = ftell(fp);

	if(e)
	{

	    mtdEraseInfo.start = 0;
//	       printf("write %d(0x%x) bytes to flash...\n",e,e);
	       e = ((e-1)|0xffff)+1;
	        while (e  > 0) { 
		        mtdEraseInfo.length = mtdInfo.erasesize;
  
		       if (!quiet)
				  fprintf(stderr, "\b\b\b[e]");

		        /* erase the chunk */
		       if (ioctl (fd,MEMERASE,&mtdEraseInfo) < 0) {
			       fprintf(stderr, "Erasing mtd failed: %s\n", fd);
			    exit(1);
		    
		       }

		    e -= mtdInfo.erasesize;
		    mtdEraseInfo.start += mtdInfo.erasesize;
	        }	

	}	

	for (;;) {
		/* buffer may contain data already (from trx check) */
		r = buflen;
		r += read(imagefd, buf + buflen, BUFSIZE - buflen);
		w += r;

		/* EOF */
		if (r <= 0) break;

#if 0
		/* need to erase the next block before writing data to it */
		while (w > e) {
			mtdEraseInfo.start = e;
			mtdEraseInfo.length = mtdInfo.erasesize;

			if (!quiet)
				fprintf(stderr, "\b\b\b[e]");
			/* erase the chunk */
			if (ioctl (fd,MEMERASE,&mtdEraseInfo) < 0) {
				fprintf(stderr, "Erasing mtd failed: %x\r\n", fd);
				return -1;
			}
			e += mtdInfo.erasesize;
		}
#endif		
		if (!quiet)
			fprintf(stderr, "\b\b\b[w]");

		if ((result = write(fd, buf, r)) < r) {
			if (result < 0) {
				fprintf(stderr, "Error writing image.\r\n");
				return -1;
			} else {
				fprintf(stderr, "Insufficient space.\r\n");
				return -1;
			}
		}

		buflen = 0;
	}
	if (!quiet)
		fprintf(stderr, "\b\b\b\b");

	return 0;
}

const uint32 crc_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

uint32 crc32(crc, buf, len)
    uint32 crc;
    const uint8 *buf;
    uint32 len;
{
    crc = crc ^ 0xffffffffL;
    while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }
    if (len) do {
      DO1(buf);
    } while (--len);
    return crc ^ 0xffffffffL;
}

/*
  check image file is invalid

  imagefile: image file to write

  return:
  0: done
  -1: fail
*/
int check_image(const char *imagefile)
{
	image_header_t headinfo;
	int fd;
	UINT32 chipid;
    char *pData=NULL;

	uint32 crc, checksum;
	int bytes_read;
	char *cp;

    oplRegRead(REG_GB_SOFTRST_ADN_DEVID, &chipid);

	fd = open(imagefile, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "Image check: can't open image file!\r\n");
		return -1;
	}

	if (vosSafeRead(fd, &headinfo, sizeof(image_header_t)) < 0)
	{
		close(fd);
		fprintf(stderr, "Image check: can't read image header!\r\n");
		return -1;
	}

    crc = 0;
	pData = (char *)vosAlloc(4097);
	if (pData == NULL)
	{
		close(fd);
		fprintf(stderr, "Image check: out of memory!\r\n");
		return -1;
	}

	/* check header checksum */
	checksum = ntohl(headinfo.ih_hcrc);
	headinfo.ih_hcrc = ntohl(0);
	cp = (uint8 *)&headinfo;

	crc = crc32(0, cp, sizeof(image_header_t));

    if(crc != checksum)
	{
		close(fd);
		vosFree(pData);
		fprintf(stderr, "Image check: header crc error %x(%x)!\r\n", crc, checksum);
		return -1;
	}

	/* check data checksum */
    crc = 0xffffffffL;
	while ((bytes_read = vosSafeRead(fd, pData, 4096)) > 0) {
		cp = (uint8 *) pData;
		while (bytes_read >= 8)
	    {
	      DO8(cp);
	      bytes_read -= 8;
	    }
		if (bytes_read) do {
	      DO1(cp);
	    } while (--bytes_read);
	}
    crc ^= 0xffffffffL;
	
	if (crc != ntohl(headinfo.ih_dcrc))
	{
		close(fd);
		vosFree(pData);
		fprintf(stderr, "Image check: data crc error %x(%x)!\r\n", crc, ntohl(headinfo.ih_dcrc));
		return -1;
	}

#ifdef OPULAN_IMAGE_HEADER
	/* for union version, we only check the product id */
	if ((utilSwap32(headinfo.ih_chipid)&0xFFFF0) != (chipid&0xFFFF0))
	{
		close(fd);
		vosFree(pData);
		fprintf(stderr, "Image check: invalid chip id %x(%x)!\r\n", chipid, utilSwap32(headinfo.ih_chipid));
		return -1;
	}
#endif

	vosFree(pData);
	close(fd);
	return 0;
}

/*
  write flash to MTD device

  imagefile: image file to write
  dev: mtd dev 

  return:
  0: done
  -1: fail
*/
int write_flash(const char *imagefile, const char * dev)
{
	int imagefd, devfd;
	int ret = 0;
	FILE* image_fp;

	/*open image file*/
	if ((imagefd = open(imagefile, O_RDONLY)) < 0) {
				fprintf(stderr, "Couldn't open image file: %s!\r\n", imagefile);
				return -1;
	}
	
        	if ((image_fp = fopen(imagefile, "r")) < 0) {
		fprintf(stderr, "Couldn't get fp of image file : %s!\n", imagefile);
		exit(1);
	}
	
	/*open mtd dev*/
	devfd = mtd_open(dev, O_RDWR | O_SYNC);
	if(devfd < 0) {
		fprintf(stderr, "5 !!Could not open mtd device: %x\r\n", devfd);
		ret = -1;
		goto done1;
	}

	ret = mtd_check(devfd);
	if(ret !=0){
		fprintf(stderr, "Can't mtd_check!\r\n");
		goto done;
	}

     sync();
#if 0
	ret = mtd_unlock(devfd);
	if(ret !=0){
		fprintf(stderr, "Can't mtd_unlock!\n");
		goto done;
	}
#endif

	ret = mtd_write(imagefd, image_fp,devfd) ;
	if(ret != 0){
		fprintf(stderr, "Can't mtd_write!\r\n");
		goto done;
	}

     sync();
		
done:
	close(devfd);
done1:
	close(imagefd);
	
	return ret;
}

/*
   Erase the whole partition

   return:
   0: done
*/

int erase_partition(const char * dev)
{
	int devfd;
	int ret = 0;

	/*open mtd dev*/
	devfd = mtd_open(dev, O_RDWR | O_SYNC);
	if(devfd < 0) {
		fprintf(stderr, "5 !!Could not open mtd device: %x\r\n", devfd);
		ret = -1;
		return ret;

	}

	ret = mtd_check(devfd);
	if(ret !=0){
		fprintf(stderr, "Can't mtd_check!\r\n");
		goto done;
	}

     sync();

     ret = mtd_erase_partition(devfd);

     sync();
		
done:
	close(devfd);
	
	return ret;
}
