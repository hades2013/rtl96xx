/*
 * ntpclient.c - NTP client
 *
 * Copyright 1997, 1999, 2000, 2003  Larry Doolittle  <larry@doolittle.boa.org>
 * Last hack: July 5, 2003
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (Version 2,
 *  June 1991) as published by the Free Software Foundation.  At the
 *  time of writing, that license was published by the FSF with the URL
 *  http://www.gnu.org/copyleft/gpl.html, and is incorporated herein by
 *  reference.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  Possible future improvements:
 *      - Double check that the originate timestamp in the received packet
 *        corresponds to what we sent.
 *      - Verify that the return packet came from the host we think
 *        we're talking to.  Not necessarily useful since UDP packets
 *        are so easy to forge.
 *      - Write more documentation  :-(
 *
 *  Compile with -D_PRECISION_SIOCGSTAMP if your machine really has it.
 *  There are patches floating around to add this to Linux, but
 *  usually you only get an answer to the nearest jiffy.
 *  Hint for Linux hacker wannabes: look at the usage of get_fast_time()
 *  in net/core/dev.c, and its definition in kernel/time.c .
 *
 *  If the compile gives you any flak, check below in the section
 *  labelled "XXXX fixme - non-automatic build configuration".
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>     /* gethostbyname */
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

//add by zhouguanhua 2012/8/20
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<../shared/debug.h>

//end add by zhouguanhua 

#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>

//#include <cm_pub.h>
//#include <cm_oid.h>
//#include <mw_syslog.h>
//#include <mp_interface.h>

#ifdef _PRECISION_SIOCGSTAMP
#include <sys/ioctl.h>
#endif

#define ENABLE_DEBUG

extern char *optarg;

/* XXXX fixme - non-automatic build configuration */
#ifdef linux
#include <sys/utsname.h>
#include <sys/time.h>
typedef u_int32_t __u32;
#include <sys/timex.h>
#else
extern struct hostent *gethostbyname(const char *name);
extern int h_errno;
#define herror(hostname) \
	fprintf(stderr,"Error %d looking up hostname %s\n", h_errno,hostname)
typedef uint32_t __u32;
#endif

#define JAN_1970        0x83aa7e80      /* 2208988800 1970 - 1900 in seconds */
#define NTP_PORT (123)
#define CM_FILE_NTPINFO "/tmp/ntpclient.txt"
//#define CM_FILE_NTPINFO "/home/zhouguanhua/onu5/trunk/app/ntpclient/ntpclient.txt"
#define MEM_SIZE_64 64
#define MW_DBG_WARN DBG_PRINTF       //modify by zhouguanhua  2012-9-25 EPN104QID0060
#define MW_DBG_ERR DBG_PRINTF		//modify by zhouguanhua  2012-9-25 EPN104QID0060
#define MW_DBG_INFO DBG_PRINTF		//modify by zhouguanhua  2012-9-25 EPN104QID0060

/* How to multiply by 4294.967296 quickly (and not quite exactly)
 * without using floating point or greater than 32-bit integers.
 * If you want to fix the last 12 microseconds of error, add in
 * (2911*(x))>>28)
 */
#define NTPFRAC(x) ( 4294*(x) + ( (1981*(x))>>11 ) )

/* The reverse of the above, needed if we want to set our microsecond
 * clock (via settimeofday) based on the incoming time in NTP format.
 * Basically exact.
 */
#define USEC(x) ( ( (x) >> 12 ) - 759 * ( ( ( (x) >> 10 ) + 32768 ) >> 16 ) )

/* Converts NTP delay and dispersion, apparently in seconds scaled
 * by 65536, to microseconds.  RFC1305 states this time is in seconds,
 * doesn't mention the scaling.
 * Should somehow be the same as 1000000 * x / 65536
 */
#define sec2u(x) ( (x) * 15.2587890625 )

struct ntptime {
	unsigned int coarse;
	unsigned int fine;
};

/* prototype for function defined in phaselock.c */
int contemplate_data(unsigned int absolute, double skew, double errorbar, int freq);

/* prototypes for some local routines */
void send_packet(int usd);
int rfc1305print(uint32_t *data, struct ntptime *arrival, int tv);
//void udp_handle(int usd, char *data, int data_len, struct sockaddr *sa_source, int sa_len);


/* variables with file scope
 * (I know, bad form, but this is a short program) */
static uint32_t incoming_word[325];
#define incoming ((char *) incoming_word)
#define sizeof_incoming (sizeof(incoming_word)*sizeof(uint32_t))
//static struct timeval time_of_send;
static int live=0;
static int set_clock=0;   /* non-zero presumably needs root privs */

/* BEGIN: Modified, 2009/9/7 */
#define DEFAULT_TZ		    57    // china, HongKong 默认时区需要设置在合法时区内
#define MAX_TZ			    75
//#define DEFAULT_TZ		    68    // china, HongKong 默认时区需要设置在合法时区内
//#define MAX_TZ			    87
/* END: Modified, 2009/9/7 */
#define MAX_TZSERVER	    8
#define MAX_NAME_LENGTH     32

static int clock_updated = 0;
static int tzone=0;
static int tzone_index = DEFAULT_TZ;
static int current_server_index = 0;
/*FIXME:same to webs:device_time_set.asp*/
#if 0
char *tz_cn[MAX_TZ] = {
   "GMT -12:00日界线西",  
   "GMT -11:00中途岛，萨摩亚群岛",
   "GMT -10:00夏威夷",
   "GMT -9:00阿拉斯加",
   "GMT -8:00太平洋时间（美国和加拿大）；蒂华纳",
   "GMT -7:00山地时间（美国和加拿大）",
   "GMT -7:00亚利桑那",
   "GMT -7:00奇瓦瓦，拉巴斯，马扎特兰",
   "GMT -6:00萨斯喀彻温",
   "GMT -6:00中部时间（美国和加拿大）",
   "GMT -6:00中美洲 ",
   "GMT -6:00瓜达拉哈拉，墨西哥城，蒙特雷",
   "GMT -5:00波哥大，利马，基多",
   "GMT -5:00东部时间（美国和加拿大）",
   "GMT -5:00印地安那州（东部）",
   "GMT -4:00大西洋时间（加拿大）",
   "GMT -4:00加拉加斯，拉巴斯",
   "GMT -4:00圣地亚哥",
   "GMT -3:30纽芬兰",
   "GMT -3:00巴西利亚",
   "GMT -3:00布宜诺斯艾利斯，乔治敦",
   "GMT -3:00格陵兰",
   "GMT -2:00中大西洋",
   "GMT -1:00佛得角群岛",
   "GMT -1:00亚速尔群岛",
   "GMT 格林威治标准时间：都柏林，爱丁堡，伦敦，里斯本",
   "GMT 卡撒布兰卡，蒙罗维亚",
   "GMT +1:00阿姆斯特丹，柏林，伯尔尼，罗马，斯德哥尔摩",
   "GMT +1:00贝尔格赖德，布拉迪斯拉发，布达佩斯",
   "GMT +1:00布鲁塞尔，哥本哈根，马德里，巴黎",
   "GMT +1:00萨拉热窝，斯科普里，华沙，萨格勒布",
   "GMT +1:00中非西部",
   "GMT +2:00布加勒斯特",
   "GMT +2:00哈拉雷，比萨陀利亚",
   "GMT +2:00赫尔辛基，基辅，里加，索非亚，塔林，维尔纽斯",
   "GMT +2:00开罗",
   "GMT +2:00雅典，贝鲁特，伊斯坦布尔，明斯克",
   "GMT +2:00耶路撒冷",
   "GMT +3:00巴格达",
   "GMT +3:00科威特，利雅得",
   "GMT +3:00莫斯科，圣彼得堡，伏尔加格勒",
   "GMT +3:00内罗毕",
   "GMT +3:30德黑兰",
   "GMT +4:00阿布扎比，马斯喀特",
   "GMT +4:00巴库，第比利斯，埃里温",
   "GMT +4:30喀布尔",
   "GMT +5:00叶卡捷琳堡",
   "GMT +5:00伊斯兰堡，卡拉奇，塔什干",
   "GMT +5:30马德拉斯，加尔各答，孟买，新德里",
   "GMT +5:45加德满都",
   "GMT +6:00阿拉木图，新西伯利亚",
   "GMT +6:00阿斯塔纳，达卡",
   "GMT +6:00斯里哈亚华登尼普拉",
   "GMT +6:30仰光",
   "GMT +7:00克拉斯诺亚尔斯克",
   "GMT +7:00曼谷，河内，雅加达",
   "GMT +8:00北京，重庆，香港特别行政区，乌鲁木齐",
   "GMT +8:00吉隆坡，新加坡",
   "GMT +8:00珀斯",
   "GMT +8:00台北",
   "GMT +8:00伊尔库茨克，乌兰巴图",
   "GMT +9:00大阪，札幌，东京",
   "GMT +9:00汉城",
   "GMT +9:00雅库茨克",
   "GMT +9:30阿德莱德",
   "GMT +9:30达尔文",
   "GMT +10:00布里斯班",
   "GMT +10:00符拉迪沃斯托克",
   "GMT +10:00关岛，莫尔兹比港",
   "GMT +10:00霍巴特",
   "GMT +10:00斯塔拉，墨尔本，悉尼",
   "GMT +11:00马加丹，索罗门群岛，新喀里多尼亚",
   "GMT +12:00奥克兰，惠灵顿",
   "GMT +12:00斐济，勘察加半岛，马绍尔群岛",
   "GMT +13:00努库阿洛法"       
};
#endif
/*char tzserver[MAX_TZSERVER][MAX_NAME_LENGTH] = {"192.168.0.4",
				"192.168.0.5",
				"192.168.0.80",
				"192.168.0.7",
				"192.168.0.8",
				"192.168.0.9",
				"192.168.0.10",
				"192.168.0.40"};
*/
char tzserver[MAX_TZSERVER][MAX_NAME_LENGTH] = {"time.windows.com",
				"time.twc.weather.com",
				"timeserver.cs.umb.edu",
				"ntp.alaska.edu",
				"clock.isc.org",
				"clock.redhat.com",
				"time.nist.gov",
				"time-b.nist.gov"};

/* BEGIN: Modified, 2009/9/7 */
const char TZCONST[MAX_TZ][5]={	//除以4就得时区了。
 			"-48","-44","-40","-36","-32","-28","-28","-28",
            "-24","-24","-24","-24","-20","-20","-20","-16",
            "-16","-16","-14","-12","-12","-12","-08","-04",
            "-04","0",  "+00","+04","+04","+04","+04","+04",
			"+08","+08","+08","+08","+08","+08","+12","+12",
			"+12","+12","+14","+16","+16","+18","+20","+20",
			"+22","+23","+24","+24","+24","+26","+28","+28",
			"+32","+32","+32","+32","+32","+36","+36","+36",
			"+38","+38","+40","+40","+40","+40","+40","+44",
			"+48","+48","+52"
		};
/*
const char OLD_TZCONST[MAX_TZ][5]={	
			"-24","-22","-22","-20","-18","-16","-16","-16",
			"-14","-14","-14","-12","-12","-12","-12","-10",
			"-10","-10","-10","-10","-08","-08","-08","-7",
			"-06","-02","-02","0  ","+00","+00","+00","+00",
			"+00","+02","+02","+02","+02","+02","+02","+02",
			"+02","+02","+02","+04","+04","+04","+04","+04",
			"+04","+04","+06","+06","+06","+06","+06","+07",
			"+08","+08","+10","+10","+10","+11","+11","+11",
			"+12","+14","+14","+16","+16","+16","+16","+16",
			"+18","+18","+18","+19","+19","+20","+20","+20",
			"+20","+20","+20","+22","+24","+24","+24" };
*/
char *hostserver=NULL;          /* must be set */

static int cycle_time=3;          /* request timeout in seconds */
static int probe_count=3;            /* default of 0 means loop forever */
static int server_count = MAX_TZSERVER;

#define TRY_INTERVAL_TIMER   5    /*Modified by huangmingjian 2013/9/06 Bug 517*/
#define CHECK_INTERVAL_TIMER (24*60*60)

//#define TRY_INTERVAL_TIMER   5
//#define CHECK_INTERVAL_TIMER (12)

/* when present, debug is a true global, shared with phaselock.c */
#ifdef ENABLE_DEBUG
int debug=0;
#define DEBUG_OPTION "d"
#else
#define debug 0
#define DEBUG_OPTION
#endif


int get_current_freq(void)
{
	/* OS dependent routine to get the current value of clock frequency.
	 */
#ifdef linux
	struct timex txc;
	txc.modes=0;
	if (__adjtimex(&txc) < 0) {
		MW_DBG_WARN("adjtimex"); return 0;
	}
	return txc.freq;
#else
	return 0;
#endif
}

int set_freq(int new_freq)
{
	/* OS dependent routine to set a new value of clock frequency.
	 */
#ifdef linux
	struct timex txc;
	txc.modes = ADJ_FREQUENCY;
	txc.freq = new_freq;
	if (__adjtimex(&txc) < 0) {
		MW_DBG_WARN("adjtimex"); return 0;
	}
	return txc.freq;//频率偏移 
#else
	new_freq = new_freq;
	return 0;
#endif
}

void send_packet(int usd)
{
	__u32 data[12];
	struct timeval now;

#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 4 
#define PREC -6

	if (debug) fprintf(stderr,"Sending ...\n");
	if (sizeof(data) != 48) {
		MW_DBG_ERR("size error\n");
		return;
	}
	bzero((char *) data,sizeof(data));
	data[0] = htonl (
		( LI << 30 ) | ( VN << 27 ) | ( MODE << 24 ) |
		( STRATUM << 16) | ( POLL << 8 ) | ( PREC & 0xff ) );
	data[1] = htonl(1<<16);  /* Root Delay (seconds) */
	data[2] = htonl(1<<16);  /* Root Dispersion (seconds) */
	gettimeofday(&now,NULL);
	data[10] = htonl(now.tv_sec + JAN_1970); /* Transmit Timestamp coarse */
	data[11] = htonl(NTPFRAC(now.tv_usec));  /* Transmit Timestamp fine   */
	send(usd,data,48,0);    
	//time_of_send=now;
}

void get_packet_timestamp(int usd, struct ntptime *udp_arrival_ntp)
{
	struct timeval udp_arrival;
#ifdef _PRECISION_SIOCGSTAMP
	if ( ioctl(usd, SIOCGSTAMP, &udp_arrival) < 0 ) {
		MW_DBG_WARN("ioctl-SIOCGSTAMP");
		gettimeofday(&udp_arrival,NULL);
	}
#else
    usd = usd;
	gettimeofday(&udp_arrival,NULL);
#endif
	udp_arrival_ntp->coarse = (unsigned long)udp_arrival.tv_sec + JAN_1970;
	udp_arrival_ntp->fine   = NTPFRAC((unsigned long)udp_arrival.tv_usec);
}

void check_source(int data_len, struct sockaddr *sa_source, int sa_len)
{
    sa_len = sa_len;
	/* This is where one could check that the source is the server we expect */
	if (debug) {
		struct sockaddr_in *sa_in=(struct sockaddr_in *)(long)sa_source;
		printf("packet of length %d received\n",data_len);
		if (sa_source->sa_family==AF_INET) {
			printf("Source: INET Port %d host %s\n",
				ntohs(sa_in->sin_port),inet_ntoa(sa_in->sin_addr));
		} else {
			printf("Source: Address family %d\n",sa_source->sa_family);
		}
	}
}

double ntpdiff( struct ntptime *start, struct ntptime *stop)
{
	int a;
	unsigned int b;
	a = (int)stop->coarse - (int)start->coarse;
	if (stop->fine >= start->fine) {
		b = stop->fine - start->fine;
	} else {
		b = start->fine - stop->fine;
		b = ~b;
		a -= 1;
	}
	
	return a*1.e6 + b * (1.e6/4294967296.0);
}

/* Does more than print, so this name is bogus.
 * It also makes time adjustments, both sudden (-s)
 * and phase-locking (-l).  */
/* return value is number of microseconds uncertainty in answer */
int rfc1305print(uint32_t *data, struct ntptime *arrival, int tv)
{
/* straight out of RFC-1305 Appendix A */
	int li, vn, mode, stratum, poll, prec;
	int delay, disp, refid;
	struct ntptime reftime, orgtime, rectime, xmttime;
	double el_time,st_time,skew1,skew2;
	int freq;

#define Data(i) ntohl(((uint32_t *)data)[i])
	li      = Data(0) >> 30 & 0x03;
	vn      = Data(0) >> 27 & 0x07;
	mode    = Data(0) >> 24 & 0x07;
	stratum = Data(0) >> 16 & 0xff;
	poll    = Data(0) >>  8 & 0xff;
	prec    = Data(0)       & 0xff;
	if (prec & 0x80) prec|=0xffffff00;
	delay   = (int)Data(1);
	disp    = (int)Data(2);
	refid   = (int)Data(3);
	reftime.coarse = Data(4);
	reftime.fine   = Data(5);
	orgtime.coarse = Data(6);
	orgtime.fine   = Data(7);
	rectime.coarse = Data(8);
	rectime.fine   = Data(9);
	xmttime.coarse = Data(10);//向客户机答复分离服务器的时间,修正的时间
	xmttime.fine   = Data(11);//向客户机答复分离服务器的时间,修正的时间
#undef Data

	if (set_clock)
    {                           /* you'd better be root, or ntpclient will crash! */
		struct timeval tv_set;

        /* Modified for the time 2036 years*/
        /* error checking, see RFC-4330 section 5
           there more para to check, we do some para */
        if ((li == 3) || (vn < 3) || (mode != 4) || (stratum == 0)) 
        {
            MW_DBG_WARN("[ntpclient]: error para LI=%d,VN=%d,Mode=%d,Stratum=%d\n",
                   		li, vn, mode, stratum);
            return 0;
        }

        /* Server Originate Time should be equal to Client Transmit Time,
           here, do not check it. But check the Transmit Time*/
        if ((xmttime.coarse == 0) || (xmttime.fine == 0)
            || (xmttime.coarse < JAN_1970) ||(xmttime.coarse == 0xFFFFFFFF))
        {
            MW_DBG_WARN("[ntpclient]: error Transmit time %u.%.10u",
                        xmttime.coarse, xmttime.fine);
            return 0;            
        }
        
		/* it would be even better to subtract half the slop */
		//tv_set.tv_sec  = xmttime.coarse - JAN_1970 + tv*3600;
		/* Modified, 2009/9/7 */
		//tv_set.tv_sec  = xmttime.coarse - JAN_1970 + tv*1800;

        //tv_set.tv_sec  = (long)xmttime.coarse + tv*900 - JAN_1970;//秒,tv为时区
		tv_set.tv_sec  = (long)xmttime.coarse- JAN_1970;//秒,tv为时区

		/* divide xmttime.fine by 4294.967296 */
		tv_set.tv_usec = USEC(xmttime.fine);//微秒		
		if (settimeofday(&tv_set,NULL)<0) {//设置系统时间,必须是root用户才能设置时间
			MW_DBG_WARN("[ntpclient]: settimeofday\n");
			return 0;
		}
		if (debug) {
			printf("set time to %lu.%.6lu\n", tv_set.tv_sec, tv_set.tv_usec);
		}
        
		clock_updated = 1;
	}

	if (debug) {
	printf("LI=%d  VN=%d  Mode=%d  Stratum=%d  Poll=%d  Precision=%d\n",
		li, vn, mode, stratum, poll, prec);
	printf("Delay=%.1f  Dispersion=%.1f  Refid=%u.%u.%u.%u\n",
		sec2u(delay),sec2u(disp),
		refid>>24&0xff, refid>>16&0xff, refid>>8&0xff, refid&0xff);
	printf("Reference %u.%.10u\n", reftime.coarse, reftime.fine);//T4
	printf("Originate %u.%.10u\n", orgtime.coarse, orgtime.fine);//T1
	printf("Receive   %u.%.10u\n", rectime.coarse, rectime.fine);//T2
	printf("Transmit  %u.%.10u\n", xmttime.coarse, xmttime.fine);//T3
	printf("Our recv  %u.%.10u\n", arrival->coarse, arrival->fine);//接收到服务器应答时，客户的时间，从linux里读取
	}
	el_time=ntpdiff(&orgtime,arrival);   /* elapsed *///客户向服务器发送到接收的时间
	st_time=ntpdiff(&rectime,&xmttime);  /* stall *///T2-T3
	skew1=ntpdiff(&orgtime,&rectime);//T1-T4
	skew2=ntpdiff(&xmttime,arrival);//T3-arrival
	freq=get_current_freq();//linux系统时钟频率偏移 
	if (debug) {
	printf("Total elapsed: %9.2f\n"
	       "Server stall:  %9.2f\n"
	       "Slop:          %9.2f\n",
		el_time, st_time, el_time-st_time);
	printf("Skew:          %9.2f\n"
	       "Frequency:     %9d\n"
	       " day   second     elapsed    stall     skew  dispersion  freq\n",
		(skew1-skew2)/2, freq);// ((T2-T1)+(T3-T4))/2，时差
	}
	/* Not the ideal order for printing, but we want to be sure
	 * to do all the time-sensitive thinking (and time setting)
	 * before we start the output, especially fflush() (which
	 * could be slow).  Of course, if debug is turned on, speed
	 * has gone down the drain anyway. */
	if (live) {
		int new_freq;
		new_freq = contemplate_data(arrival->coarse, (skew1-skew2)/2,
			el_time+sec2u(disp), freq);
		if (!debug && new_freq != freq) set_freq(new_freq);
	}
    
    /*
	printf("%d %.5d.%.3d  %8.1f %8.1f  %8.1f %8.1f %9d\n",
		arrival->coarse/86400, arrival->coarse%86400,
		arrival->fine/4294967, el_time, st_time,
		(skew1-skew2)/2, sec2u(disp), freq);
	fflush(stdout);
    */

    //return (el_time-st_time);
	return 1;
}

int stuff_net_addr(struct in_addr *p, char *hostname)
{
    //FILE *fd = NULL;
	struct hostent *ntpserver;

    res_init(); /* add*/

	ntpserver=gethostbyname(hostname);
	if (ntpserver == NULL) {
		DBG_PRINTF("NTP server gethostbyname is failed, hostname: %s.\n", hostname);
		return -1;
	}
	if (ntpserver->h_length != 4) {
		MW_DBG_ERR("oops %d\n",ntpserver->h_length);
		return -1;
	}
	memcpy(&(p->s_addr),ntpserver->h_addr_list[0],4);
    return 0;
}

void setup_receive(int usd, unsigned int interface, short port)
{
	struct sockaddr_in sa_rcvr;
	bzero((char *) &sa_rcvr, sizeof(sa_rcvr));
	sa_rcvr.sin_family=AF_INET;
	sa_rcvr.sin_addr.s_addr=htonl(interface);
	
	sa_rcvr.sin_port=htons((unsigned short)port);
	if(bind(usd,(struct sockaddr *) &sa_rcvr,sizeof(sa_rcvr)) == -1) {
		MW_DBG_ERR("could not bind to udp port %d\n",port);
		MW_DBG_ERR("bind");
		return;
	}
	listen(usd,3);
}

int setup_transmit(int usd, char *host, short port)
{
    int rtn_val;
	struct sockaddr_in sa_dest;
	bzero((char *) &sa_dest, sizeof(sa_dest));
	sa_dest.sin_family=AF_INET;
	rtn_val = stuff_net_addr(&(sa_dest.sin_addr),host);
    if (rtn_val != 0)
    {
        return -1;
    }

	DBG_PRINTF("NTP trying %s IP: %s\n", host, inet_ntoa(sa_dest.sin_addr));
	sa_dest.sin_port=htons((unsigned short)port);
	if (connect(usd,(struct sockaddr *)&sa_dest,sizeof(sa_dest))==-1)
	{
		if(debug)
			perror("connect"); //sc_yang
		return -1;
	} 
    return 0;
}

/* added by y04581 for dial on demand */
int ntp_stoped = 0;
int primary_loop(int usd, int num_probes, int interval, int goodness, int tv)
{
	fd_set fds;
	struct sockaddr sa_xmit;
	int i, pack_len, sa_xmit_len,error,probes_sent;
	struct timeval to;
	struct ntptime udp_arrival_ntp;
    goodness = goodness;
	if (debug) printf("Listening...\n");

    int synced_times= 0;
	probes_sent=0;
	sa_xmit_len=sizeof(sa_xmit);
	to.tv_sec=0;
	to.tv_usec=0;
	for (;!ntp_stoped;) {
		FD_ZERO(&fds);
		FD_SET((unsigned int)usd,&fds);
		i=select(usd+1,&fds,NULL,NULL,&to);  /* Wait on read or error */
		if (i <= 0) //i=0,表示超时
        {
			if (i < 0)
			{
                MW_DBG_ERR("Select error, error no = %d", i);
                if (errno == EINTR)
                    continue;
                else
                    break;
			}
            
            if ((probes_sent < num_probes) || (num_probes == 0))
            {
				send_packet(usd);//发送时间请求
                //MW_DBG_INFO("Send ntp probe, probes number = %d", probes_sent);              
				++probes_sent;
				to.tv_sec = interval;//select()超时阻塞时间
                /*Add by zhouguanhua 2013/8/22*/
                if(probes_sent==2) to.tv_sec = interval+3;//以不同的间隔同步
                if(probes_sent==3) to.tv_sec = interval+13;
                /*end Add by zhouguanhua 2013/8/22*/
				to.tv_usec = 0;
    			continue;
            }
            else
            {
                break;
            }
		}
/*		if ((i!=1)||(!FD_ISSET(usd,&fds))) {
			if (i==EINTR) continue;
			if (i<0) 
               MW_DBG_ERR("select");
			if (to.tv_sec == 0) {
				if (probes_sent >= num_probes &&
					num_probes != 0) break;
				send_packet(usd);
				++probes_sent;
				to.tv_sec=interval;
				to.tv_usec=0;
			}	
			continue;
		}
*/
        if (FD_ISSET((unsigned int)usd,&fds))
        {	
		    pack_len=recvfrom(usd,incoming,sizeof_incoming,0,
		                  &sa_xmit,(socklen_t *)(&sa_xmit_len));
		             
        	if (pack_len<0) {
                //perror("recvfrom");
        	} 
            else if (pack_len>0 && (unsigned)pack_len<sizeof_incoming){
        		get_packet_timestamp(usd, &udp_arrival_ntp);//获得客户系统时间
        		check_source(pack_len, &sa_xmit, sa_xmit_len);
        		error = rfc1305print(incoming_word, &udp_arrival_ntp, tv);//设置时间
                //if (goodness != 0 && error < goodness)
                if (error == 0)
                {
                    break;
                }
        		/* udp_handle(usd,incoming,pack_len,&sa_xmit,sa_xmit_len); */
        		return 0;
            } else {
        		MW_DBG_ERR("Ooops.  pack_len=%d\n",pack_len);
        	}
       }

       if (probes_sent >= num_probes && num_probes != 0) break;
	}
	return -1;
}

void do_replay(void)
{
	char line[100];
	int n, day, freq, absolute;
	float sec, el_time, st_time, disp;
	double skew, errorbar;
	int simulated_freq = 0;
	unsigned int last_fake_time = 0;
	double fake_delta_time = 0.0;

	while (fgets(line,sizeof(line),stdin)) {//从stdin所指的文件内读入字符串到line内存中
		n=sscanf(line,"%d %f %f %f %lf %f %d",//将line字符串格式化到day等参数
			&day, &sec, &el_time, &st_time, &skew, &disp, &freq);
		if (n==7) {
			fputs(line,stdout);
			absolute=day*86400+(int)sec;
			errorbar=el_time+disp;
			if (debug) printf("contemplate %u %.1f %.1f %d\n",
				absolute,skew,errorbar,freq);
			if (last_fake_time==0) simulated_freq=freq;
			fake_delta_time += (absolute-last_fake_time)*((double)(freq-simulated_freq))/65536;
			if (debug) printf("fake %f %d \n", fake_delta_time, simulated_freq);
			skew += fake_delta_time;
			freq = simulated_freq;
			last_fake_time=(unsigned int)absolute;
			simulated_freq = contemplate_data((unsigned int)absolute, skew, errorbar, freq);
		} else {
			MW_DBG_ERR("Replay input error\n");
			return;
		}
	}
}

void renew_time_info(void)
{
    /*根据是否配置hostname来确定NTP server的参数，如果没有配置，采用系统默认*/
    if(hostserver != NULL)
    {
        if (strlen(hostserver) >= MAX_NAME_LENGTH)
        {
            MW_DBG_ERR("hostname is errorerror\n");
       		exit(0);
        }
        
        memset(tzserver, 0, sizeof(tzserver));
        strcpy(tzserver[0], hostserver);
        server_count = 1;
    }

    /*根据是否有命令配置，设置时区的值*/
    if ( tzone_index < 1 || tzone_index > MAX_TZ )
    {
        MW_DBG_ERR("timezone is error, use the default timezone\n");
        tzone_index = DEFAULT_TZ;
    }
    tzone = atoi(TZCONST[tzone_index -1]);

    return;
}

void ntp_stop(int sig)
{
    sig = sig;
    MW_DBG_INFO("Receive the SIGUSR2, and enter the ntp_stop.");
    server_count = 0;
    ntp_stoped = 1;
}

void ntp_retry(int sig)
{
    sig = sig;
    MW_DBG_INFO("Receive the SIGUSR1, and enter the ntp_retry.");
    server_count = MAX_TZSERVER;
	renew_time_info();
    clock_updated = 0;
    ntp_stoped = 0;
}

void ntp_quit_signal(int sig)
{
    sig = sig;
    /*MW_SYSLOG(LOG_INFO, "NTP client exited.");*/
	remove(CM_FILE_NTPINFO);
	exit(1);
}

void try_ntp(void)
{
	int usd;  /* socket */
	short int udp_local_port=0;   /* default of 0 means kernel chooses */
	char *ntps;
	int goodness=0;

    /*网络校时*/
    if( current_server_index < server_count) {

		ntps = tzserver[current_server_index];

		/* Startup sequence */

        /*MW_DBG_INFO("Try to ntp server:%s", ntps);*/
		if ((usd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1) {
			MW_DBG_ERR("socket");
			return;
		}
        setup_receive(usd, INADDR_ANY, udp_local_port);//bind()

		if(setup_transmit(usd, ntps, NTP_PORT) == 0)
		{
    		primary_loop(usd, probe_count, cycle_time, goodness, tzone);//设置时间
        }

		close(usd);

		current_server_index ++;
		if (current_server_index >= server_count) 
            current_server_index = 0;
	}
    return;
}

void usage(char *argv0)
{
	fprintf(stderr,
//	"Usage: %s [-c count] [-d] [-g goodness] -h hostname [-i interval]\n"
	"Usage: %s [-c count] [-d] -h hostname [-i interval]\n"
	"\t[-l] [-p port] [-r] [-s] [-t timezone]\n",
	argv0);
}

void write_status( const char *sts)
{ 
	FILE *fp = NULL;
	fp = fopen(CM_FILE_NTPINFO,"w");
	if(NULL ==fp)
	{
		MW_DBG_ERR("%s,%d:open ntpinfo file error!",__FUNCTION__,__LINE__);
		return;
	}
	fprintf(fp,"%s",sts);
	fclose(fp);
}

//-------------------------------------------------
//手工设置系统时间//add by zhouguanhua 
#if 0
void set_time_by_user()
{
    int net_time[]={2012,6,15,12,16,0,6,-1};
    time_t t;
    struct tm real_time;
    real_time.tm_year = net_time[0]-1900;
    real_time.tm_mon  = net_time[1]-1;
    real_time.tm_mday = net_time[2];
    real_time.tm_hour = net_time[3];
    real_time.tm_min  = net_time[4];
    real_time.tm_sec  = net_time[5];
    real_time.tm_wdat  = net_time[6];//星期
    real_time.tm_isdst = net_time[7];//夏令标识，UTC时间

    t = mktime(&real_time);//将平常用的时间转成s
    stime(&t);//设置系统时间，

    //system("hwclock -w");     
}
#endif

//add by zhouguanhua to write pid to a file
#if 0 //add by zhouguanhua 2013/6/5
int  safe_write(int fd, const void *buf, size_t count)
{
	int n;

	do {
		n = write(fd, buf, count);
	} while (n < 0 );

	return n;
}

char* utoa_to_buf(unsigned n, char *buf, unsigned buflen)
{
	unsigned i, out, res;

	if (buflen) {
		out = 0;
		if (sizeof(n) == 4)
		// 2^32-1 = 4294967295
			i = 1000000000;
#if UINT_MAX > 4294967295 /* prevents warning about "const too large" */
		else
		if (sizeof(n) == 8)
		// 2^64-1 = 18446744073709551615
			i = 10000000000000000000;
#endif
		else
			BUG_sizeof();
		for (; i; i /= 10) {
			res = n / i;
			n = n % i;
			if (res || out || i == 1) {
				if (--buflen == 0)
					break;
				out++;
				*buf++ = '0' + res;
			}
		}
	}
	return buf;
}


int full_write(int fd, const void *buf, size_t len)
{
	int cc;
	int total;

	total = 0;

	while (len) {
		cc = safe_write(fd, buf, len);

		if (cc < 0) {
			if (total) {
				/* we already wrote some! */
				/* user can do another write to know the error code */
				return total;
			}
			return cc;	/* write() returns -1 on failure. */
		}

		total += cc;
		buf = ((const char *)buf) + cc;
		len -= cc;
	}

	return total;
}

signed char wrote_pidfile;

void  write_pidfile(const char *path)//把当前进程PID写入文件
{
	int pid_fd;
	char *end;
	char buf[sizeof(int)*3 + 2];
	struct stat sb;

	if (!path)
		return;
	/* we will overwrite stale pidfile */
	pid_fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (pid_fd < 0)
		return;

	/* path can be "/dev/null"! Test for such cases */
	wrote_pidfile = (fstat(pid_fd, &sb) == 0) && S_ISREG(sb.st_mode);

	if (wrote_pidfile) {
		/* few bytes larger, but doesn't use stdio */
		end = utoa_to_buf(getpid(), buf, sizeof(buf));
		*end = '\n';
		full_write(pid_fd, buf, end - buf + 1);
	}
	close(pid_fd);
}
#endif  //add by zhouguanhua 2013/6/5
#if 0
void write_numto_file(char num)
{
    int fd;
    char s[]="0";

    if(num==1)
    {
      *s="1" ; 
    }
    else
    *s="0" ; 
    
    fd = open("/tmp/ntpclient_ok", O_WRONLY|O_CREAT,S_IRWXU);
    wr(fd, s, sizeof(s));
    close(fd);
}

//end add by zhouguanhua
#endif


//-----------------------------------------------------

int main(int argc, char *argv[]) {
	int c = EOF;
	/* These parameters are settable from the command line
	   the initializations here provide default behavior */
	short int udp_local_port=0;   /* default of 0 means kernel chooses */
    /* int debug=0; is a global above */
	int replay=0;                 /* replay mode overrides everything */
    char log_ch[100] = {0}, log_en[100] = {0};
    int fd;//add by zhouguanhua
    
	for (;;) {//命令解析，optarg存有命令行双引号后面的参数
		c = getopt( argc, argv, "c:" DEBUG_OPTION "g:h:i:lp:t:rs");
		if (c == EOF) break;
		switch (c) {
			case 'c':// 向时间服务器发送请求次数 (default 0 means go forever)
				probe_count = atoi(optarg);
				break;
#ifdef ENABLE_DEBUG
			case 'd'://显示调试信息
				++debug;
				break;
#endif
			case 'h'://连接时间服务器的地址
				hostserver = optarg;
				break;
			case 'i':// check time every interval seconds (default 600)
				cycle_time = atoi(optarg);//多少秒发送一次请求
				break;
			case 'l'://attempt to lock local clock to server using adjtimex(2)
				live++;
				break;
			case 'p'://local NTP client UDP port (default 0 means "any available")
				udp_local_port = (short)atoi(optarg);
				break;
			case 't'://时区
				tzone_index = atoi(optarg);
				break;
			case 'r':// replay analysis code based on stdin
				replay++;
				break;
			case 's'://simple clock set (implies -c 1)
				set_clock++;//没有实际的意义，set_clock非零时为超级用户
				break;
			default:
				usage(argv[0]);
				exit(1);
		}
	}
	    
	if (replay) {
		do_replay();
		exit(0);
	}

	//syslog_init();
	renew_time_info();
#if defined(CONFIG_SYSLOG_CHINESE)
    /* Modified , 2009/9/2 */
    sprintf(log_ch, "NTP尝试获取系统时间。");
    //sprintf(log_ch, "NTP以%d秒为周期尝试获取时间。", time_value);
#else
    /* Modified , 2009/9/2*/
    sprintf(log_en, "NTP try to get system's time.");
    //sprintf(log_en, "NTP try to get time vevery %d seconds.", time_value);
#endif
	
    //MW_SYSLOG_OP(LOG_INFO, SYSLOG_TYPE_DEVICE, SYSLOG_LOGID_COMMON_NULL, log_ch, log_en);
    
    if (debug) {
        printf("Configuration:\n"
		"  -c probe_count %d\n"
		"  -d (debug)     %d\n"
		"  -h hostname    %s\n"
		"  -i interval    %d\n"
		"  -l live        %d\n"
		"  -p local_port  %d\n"
		"  -s set_clock   %d\n",
		probe_count, debug, hostserver, cycle_time,
		live, udp_local_port, set_clock );
	}
	
//注册信号函数
	signal(SIGUSR1, ntp_retry); /* mean to get the ntp time immediately*/
	signal(SIGUSR2, ntp_stop);  /* mean to stop to get the ntp time*/
   	signal(SIGINT, ntp_quit_signal);//CTRL+C退出
	signal(SIGTERM, ntp_quit_signal);//
	signal(SIGQUIT, ntp_quit_signal);
//add by zhouguanhua 
    //write_pidfile("/tmp/ntpclient.pid");//把当前进程pid写入文件
    write_status("0");//写一个同步失败标识到指定文件，add by zhouguanhua 
//end add by zhouguanhua 
	while(1) {
		try_ntp();          
		
		if (clock_updated)
	    { 
			/*
            req_hdr_t       ntp_req_hdr;
            ntp_payload_t   ntp_payload;
            ret_hdr_t       ntp_rethdr;

            strncpy(ntp_req_hdr.client_name, "ntpclient", sizeof(ntp_req_hdr.client_name)-1);
            ntp_req_hdr.client_pid = getpid();
            ntp_req_hdr.event_id = SERV_NTP;
            ntp_req_hdr.payload_len = sizeof(ntp_payload);

            ntp_payload.msg_type = NTP_TIME_GET;*/
			{
                time_t tm;
				struct tm tm_time;
				char timestr[MEM_SIZE_64] = {0};
                time(&tm);//获得系统时间--s,localtime()转换成当地时间
				memcpy(&tm_time, localtime((time_t *)&tm), sizeof(tm_time));
				sprintf(timestr,"%d-%02d-%02d %02d:%02d:%02d",tm_time.tm_year+1900, 
				tm_time.tm_mon+1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);

                MW_DBG_INFO("NTP time is obtained: %s  UTC\n",timestr);//modify by zhouguanhua  2012-9-25 EPN104QID0060

#if 0                
				#if defined(CONFIG_SYSLOG_CHINESE)
				/*2009-09-30*/
				memset(log_ch,0, sizeof(log_ch));
				sprintf(log_ch, "获取网络时间成功，设备时间已更新为%s，时区为：%s。",timestr,tz_cn[tzone_index-1]);
				#else
				memset(log_en,0, sizeof(log_en));
				sprintf(log_en, "NTP time is obtained, adjust the system time to %s.",timestr);
				#endif
#endif				
			}
            /*MW_SYSLOG(LOG_INFO, "NTP time is obtained, adjust the system time.");*/
			//MW_SYSLOG_OP(LOG_INFO,SYSLOG_TYPE_DEVICE,SYSLOG_LOGID_COMMON_NULL, log_ch, log_en);
           /* reqmp_pr_event(&ntp_req_hdr, (char *)&ntp_payload, &ntp_rethdr, NULL);
            MW_DBG_INFO("Send the message of NTP_TIME_GET to MP.");*/           
            write_status("1");//写同步成功标识到文件，其他进程会用到,add by zhouguanhua 
	        sleep(CHECK_INTERVAL_TIMER);//24小时
	       
            /*MW_SYSLOG(LOG_INFO, "NTP try to update the time.");*/
            #if defined(CONFIG_SYSLOG_CHINESE)
            sprintf(log_ch, "NTP尝试更新时间。");
            #else
            sprintf(log_en, "NTP client try to update the time.");
            #endif
            //MW_SYSLOG_OP(LOG_INFO,SYSLOG_TYPE_DEVICE,SYSLOG_LOGID_COMMON_NULL, log_ch, log_en);
            current_server_index = 0;
            clock_updated = 0;
 		}
        else
        {
           
            if (server_count != 0)
            {
                //MW_SYSLOG(LOG_INFO, "NTP time isn't obtained, try again...");
                DBG_PRINTF("NTP time isn't obtained, try again...\n");
            }
            
            write_status("0");//写一个同步失败标识到指定文件，add by zhouguanhua 
			sleep(TRY_INTERVAL_TIMER);
        }
    }
}



