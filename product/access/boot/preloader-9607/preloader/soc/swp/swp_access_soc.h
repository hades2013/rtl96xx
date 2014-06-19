#ifndef SWP_ACCESS_SOC_H
#define SWP_ACCESS_SOC_H

//CHECK if swp_info_t was defined
#ifndef swp_info_t
    #error "swp_info_t should be defined before including this file"
    // the .h is like a template, swp_info_t should be defined before using it
#endif

#ifndef PARA_SECTION
    #define PARA_SECTION
#endif

#include <soc.h>
#include "swp_access_soc_type.h"


typedef u32_t (get_as_u32_para_t)(const swp_info_t *info);
typedef int (set_as_u32_para_t)(swp_info_t *info, u32_t v);
typedef char* (get_as_str_para_t)(const swp_info_t *info, char *buf);
typedef int (set_as_str_para_t)(swp_info_t *info, const char *buf);


typedef struct {
    const char *pname;
    u32_t type;
    union {
        get_as_u32_para_t *u32_get;
        get_as_str_para_t *str_get;
    } get;
    union {
        set_as_u32_para_t *u32_set;
        set_as_str_para_t *str_set;
    } set;
} as_para_t;

#define _sizeof_info_field(fname) sizeof(((swp_info_t *)0)-> fname)
#define AS_CONST const
//#define AS_CONST

#define _define_rv_para(para_prefix, name, _types)   \
    SECTION_ON_FLASH static u32_t _get_##para_prefix##_para_##name(const swp_info_t *info) \
        {return info->name;}\
    SECTION_ON_FLASH static int _set_##para_prefix##_para_##name(swp_info_t *info, u32_t v) \
        {info-> name=v; return 0;}\
    AS_CONST as_para_t __##para_prefix##_para_##name PARA_SECTION = { \
        .pname = #name, .type=DPT_FORM_U32|DPT_SIZE(_sizeof_info_field(name)*2)|(_types), \
        .get.u32_get=_get_##para_prefix##_para_##name, .set.u32_set=_set_##para_prefix##_para_##name }

#define _define_rv_para_tab(para_prefix, name, table, _types)   \
    SECTION_ON_FLASH static char * _get_##para_prefix##_para_##name(const swp_info_t *info, char *buf) \
        {return cli_string_lookup(table, info->name, buf);}\
    SECTION_ON_FLASH static int _set_##para_prefix##_para_##name(swp_info_t *info, const char *buf) \
        {u32_t v=info-> name; int r=cli_int_lookup(table, buf, &v); info-> name=v; return r;}\
    AS_CONST as_para_t __##para_prefix##_para_##name PARA_SECTION = { \
        .pname = #name, .type=DPT_FORM_STR|(_types), \
        .get.str_get=_get_##para_prefix##_para_##name, .set.str_set=_set_##para_prefix##_para_##name }

#define _define_vv_para(para_prefix, name, _get_func_, _set_func_, _types)   \
    SECTION_ON_FLASH static u32_t _get_##para_prefix##_para_##name(const swp_info_t *info) \
        {return _get_func_;}\
    SECTION_ON_FLASH static int _set_##para_prefix##_para_##name(swp_info_t *info, u32_t v) \
        {return _set_func_;}\
    AS_CONST as_para_t __##para_prefix##_para_##name PARA_SECTION = { \
        .pname = #name, .type=DPT_FORM_U32|DPT_INDENT(1)|DPT_VIRTUAL|_types, \
        .get.u32_get=_get_##para_prefix##_para_##name, .set.u32_set=_set_##para_prefix##_para_##name }

#define _define_vv_para_str(para_prefix, name, _get_func_, _set_func_, _types)   \
    SECTION_ON_FLASH static char * _get_##para_prefix##_para_##name(const swp_info_t *info, char *buf) \
        {return _get_func_;}\
    SECTION_ON_FLASH static int _set_##para_prefix##_para_##name(swp_info_t *info, const char *v) \
        {return _set_func_;}\
    AS_CONST as_para_t __##para_prefix##_para_##name PARA_SECTION = { \
        .pname = #name, .type=DPT_FORM_U32|DPT_INDENT(1)|DPT_VIRTUAL|_types, \
        .get.str_get=_get_##para_prefix##_para_##name, .set.str_set=_set_##para_prefix##_para_##name }

extern int swp_access_soc_get(const swp_info_t *info, const char *pname, char *buf,
    const as_para_t *begin, const as_para_t *end);
extern void swp_access_soc_dump(const swp_info_t *info, 
    const char *fmt, u32_t flags, u32_t flags_mask, const as_para_t *begin, const as_para_t *end, int display_order);
extern int swp_access_soc_set(swp_info_t *info, const char *pname, const char *value,
    const as_para_t *begin, const as_para_t *end);

#endif

