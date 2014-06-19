#define swp_info_t void
#include "swp_access_soc.h"

#include <preloader.h>
#include <cpu_utils.h>
#include "swp_util.h"

SECTION_ON_FLASH int
swp_access_soc_get(const swp_info_t *info, 
    const char *pname, char *buf,
    const as_para_t *begin, const as_para_t *end) {
    const as_para_t *idx=begin;
    
    while (idx!=end) {
        if (pblr_strcmp(idx->pname, pname)==0) {
            u32_t t=idx->type;
            if (DPT_IS_STR(t)) {
                idx->get.str_get(info, buf);
            } else {
                u32_t v=idx->get.u32_get(info);
                if (DPT_IS_DECIMAL(t)) {
                    pblr_tostr(buf, v, 10, 0);
                } else {
                    int nd=DPT_GET_SIZE(t);
                    pblr_tostr(buf, v, 16, nd);
                }
            }
            return 0;
        }
        ++idx;
    }
    return -1;
}
SECTION_ON_FLASH void
swp_access_soc_dump(const swp_info_t *info, 
    const char *fmt, u32_t flags, u32_t flags_mask,
    const as_para_t *begin, const as_para_t *end, 
    int display_order) {
    char buf[32];
    
    int step=1;  
    if (display_order<0) {
        const as_para_t *t=begin;
        step=-1;
        begin=end-1;
        end=t-1;
    } 
    
    int with_indent=1;
    if (*fmt=='^') {
        with_indent=0;
        ++fmt;
    }

    const as_para_t *idx=begin;
    while (idx!=end) {
        u32_t t=idx->type;
        if ((t&flags_mask)==flags) {
            if (with_indent) {
                u32_t d=DPT_GET_INDENT(t);
                while ((d--)>0) pblr_puts("    ");
            }
            if (DPT_IS_STR(t)) {
                idx->get.str_get(info, buf);
            } else {
                u32_t v=idx->get.u32_get(info);
                if (DPT_IS_DECIMAL(t)) {
                    pblr_tostr(buf, v, 10, 0);
                } else {
                    int nd=DPT_GET_SIZE(t);
                    pblr_tostr(buf, v, 16, nd);
                }
            }
            printf(fmt, idx->pname, buf);
        }
        idx+=step;
    }
}
SECTION_ON_FLASH int
swp_access_soc_set(swp_info_t *info, 
    const char *pname, const char *value,
    const as_para_t *begin, const as_para_t *end) {
    const as_para_t *idx=begin;
    
    while (idx!=end) {
        if (pblr_strcmp(idx->pname, pname)==0) {
            if (DPT_IS_STR(idx->type)) {
                idx->set.str_set(info, value);
            } else {
                u32_t v=pblr_atoi(value);
                idx->set.u32_set(info, v);
            }
            return 0;
        }
        ++idx;
    }
    return -1;
}

