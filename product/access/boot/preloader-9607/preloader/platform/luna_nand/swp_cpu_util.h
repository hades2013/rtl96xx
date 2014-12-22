#ifndef SWP_CPU_UTIL_H
#define SWP_CPU_UTIL_H

int get_cpu_info(const char *pname, char *buf);
int set_cpu_para(const char *pname, const char *value);
void dump_cpu_info(const char *fmt, u32_t flags, u32_t flags_mask);

#endif //SWP_CPU_UTIL_H



