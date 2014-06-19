/**********************************************************
**  Copyright (C) 2012-7-11, LOSOTECH Co. Ltd.
**  All rights reserved.
**  
**  FileName:        app_watchdog.c
**  Description:     app_watchdog  C source file
**  Author:          shipeng
**  Date:            2012/7/11
**  Others:      App feed watchdog
**********************************************************/

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/watchdog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIFO "/tmp/myfifo"
#define BUFSIZE_16 	16

int main(int argc, char **argv)
{
	int ret;
	int fifo_fd;
	int wtd_dev_fd;
	int nread;
	char flag = 0;
	unsigned char num = 0;
	unsigned char buf_fd[BUFSIZE_16];
	
	memset(buf_fd, '\0', BUFSIZE_16);

	if((wtd_dev_fd = open("/dev/mw_wtd_dev", O_RDWR)) < 0)/*open wtd */
	{
		perror("/dev/mw_wtd_dev");
		exit(1);
	}
	ioctl(wtd_dev_fd, WDIOC_SETOPTIONS);/*enable wtd */

	while(1)
	{			
		if(flag == 0)
		{
			if((fifo_fd = open(FIFO, O_RDONLY, 0)) != -1)/*open fifo_server */
			{
				flag = 1;
				num = 0;
				if((nread = read(fifo_fd, buf_fd, BUFSIZE_16)) != -1)/*read the id of master from fifo_server */
				{
					close(fifo_fd);
					unlink(FIFO);/*delete the fd of fifo_server */
				}
			}
		}
		else
		{
			if((ret = access(buf_fd, 0)) < 0)/*Check if the master exist */
			{
				printf("the master process isn't exist \n");
				num++;
				if(num == 2)
				{
					ioctl(wtd_dev_fd, WDIOC_GETTIMEOUT);/*watchdog hw reboot cpu. */
				}
			}
			ioctl(wtd_dev_fd, WDIOC_KEEPALIVE);/*app feed wtd */
		}

		sleep(3);
	    if(!(get_pid_by_name("eponoamd") > 0))
	    {	
			system("eponoamd &");         
	    }	
	}
}
