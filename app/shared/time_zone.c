
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "time_zone.h"


typedef struct{
	int id;
	char *hour_offset;
	char *district;
}
time_zone_t;


const time_zone_t time_zones[] = {
{0,"-12:00","Enewetak, Kwajalein"},
{1,"-11:00","Midway Island, Samoa"},
{2,"-10:00","Hawaii"},
{3,"-09:00","Alaska"},
{4,"-08:00","Pacific Time (US & Canada);Tijuana"},
{5,"-07:00","Arizona"},
{6,"-07:00","Mountain Time (US & Canada)"},
{7,"-06:00","Central Time (US & Canada)"},
{8,"-06:00","Mexico City, Tegucigalpa"},
{9,"-06:00","Saskatchewan"},
{10,"-05:00","Bogota, Lima, Quito"},
{11,"-05:00","Eastern Time (US & Canada)"},
{12,"-05:00","Indiana (East)"},
{13,"-04:00","Atlantic Time (Canada)"},
{14,"-04:00","Caracas, La Paz"},
{15,"-04:00","Caracas, La Paz"},
{16,"-03:00","Newfoundland"},
{17,"-03:00","Brasilia"},
{18,"-03:00","Buenos Aires, Georgetown"},
{19,"-02:00","Mid-Atlantic"},
{20,"-01:00","Azores, Cape Verde Is."},
{21,"-01:00","Casablanca, Monrovia"},
{22,"+00:00","Greenwich Mean Time: Dublin, Edinburgh"},
{23,"-01:00","Greenwich Mean Time: Lisbon, London"},
{24,"+01:00","Amsterdam, Berlin, Bern, Rome"},
{25,"+01:00","Stockholm, Vienna, Belgrade"},
{26,"+01:00","Bratislava, Budapest, Ljubljana"},
{27,"+01:00","Prague,Brussels, Copenhagen, Madrid"},
{28,"+01:00","Paris, Vilnius, Sarajevo, Skopje"},
{29,"+01:00","Sofija, Warsaw, Zagreb"},
{30,"+02:00","Athens, Istanbul, Minsk"},
{31,"+02:00","Bucharest"},
{32,"+02:00","Cairo"},
{33,"+02:00","Harare, Pretoria"},
{34,"+02:00","Helsinki, Riga, Tallinn"},
{35,"+02:00","Helsinki, Riga, Tallinn"},
{36,"+03:00","Baghdad, Kuwait, Nairobi, Riyadh"},
{37,"+03:00","Moscow, St. Petersburg"},
{38,"+03:00","Tehran"},
{39,"+04:00","Abu Dhabi, Muscat, Tbilisi, Kazan"},
{40,"+04:00","Volgograd, Kabul"},
{41,"+05:00","Islamabad, Karachi, Ekaterinburg"},
{42,"+06:00","Almaty, Dhaka"},
{43,"+07:00","Bangkok, Jakarta, Hanoi"},
{44,"+08:00","Beijing, Chongqing, Urumqi"},
{45,"+08:00","Hong Kong, Perth, Singapore, Taipei"},
{46,"+09:00","Toyko, Osaka, Sapporo, Yakutsk"},
{47,"+10:00","Brisbane"},
{48,"+10:00","Canberra, Melbourne, Sydney"},
{49,"+10:00","Guam, Port Moresby, Vladivostok"},
{50,"+10:00","Hobart"},
{51,"+11:00","Magadan, Solamon, New Caledonia"},
{52,"+12:00","Fiji, Kamchatka, Marshall Is."},
{53,"+12:00","Wellington, Auckland"},
{0,NULL,NULL}};


int time_zone_num(void)
{
	return sizeof(time_zones)/ sizeof(time_zones[0]) - 1;
}


char *get_time_zone_string(int zone)
{
	int i = 0;
	do {
		if (time_zones[i].id == zone) return time_zones[i].district;
		i ++;
	}while(time_zones[i].id > 0);
	return NULL;
}

int get_time_zone_offset(int zone)
{
	int i = 0;
	do {
		if (time_zones[i].id == zone){
			return atoi(time_zones[i].hour_offset);
		} 
		i ++;
	}while(time_zones[i].id > 0);
	return 0;	
}




time_t local_time(time_t timep, int zone)
{
	time_t t;
	int offs = get_time_zone_offset(zone);
	t = abs(offs) * 60 * 60;
//	fprintf(stderr, "timep:%lu, zone:%d, offs:%d, t:%lu\n", timep, zone, offs, t);
	if (offs > 0){
		return timep + t;
	}else if (timep > t){
		return timep - t;
	}else {
		return timep;
	}
}


time_t utc_time(time_t time_local, int zone)
{
	time_t t;
	int offs = get_time_zone_offset(zone);
	t = abs(offs) * 60 * 60;
//	fprintf(stderr, "timep:%lu, zone:%d, offs:%d, t:%lu\n", timep, zone, offs, t);
	if (offs < 0){
		return time_local + t;
	}else if (time_local > t){
		return time_local - t;
	}else {
		return time_local;
	}
}



