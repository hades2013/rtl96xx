#ifndef __TIME_ZONE_H__
#define __TIME_ZONE_H__

#include <time.h>

char *get_time_zone_string(int zone);
int get_time_zone_offset(int zone);
time_t local_time(time_t timep, int zone);
time_t utc_time(time_t time_local, int zone);
int time_zone_num(void);



#endif /*#ifndef __TIME_ZONE_H__*/
