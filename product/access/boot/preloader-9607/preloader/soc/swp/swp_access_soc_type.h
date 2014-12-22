#ifndef SWP_ACCESS_SOC_TYPE_H
#define SWP_ACCESS_SOC_TYPE_H
#define DPT_FORM_MASK       1
#define DPT_FORM_U32        0
#define DPT_FORM_STR        1
#define DPT_IS_U32(t)       (((t)&DPT_FORM_MASK)==DPT_FORM_U32)
#define DPT_IS_STR(t)       (((t)&DPT_FORM_MASK)==DPT_FORM_STR)

#define DPT_OMIT_OFF        2
#define DPT_OMIT_MASK       (1<<DPT_OMIT_OFF)
#define DPT_OMIT            (1<<DPT_OMIT_OFF)
#define DPT_IS_OMIT(t)      (((t)&DPT_OMIT_MASK)!=0)

#define DPT_VIRTUAL_OFF     3
#define DPT_VIRTUAL_MASK    (1<<DPT_VIRTUAL_OFF)
#define DPT_VIRTUAL         (1<<DPT_VIRTUAL_OFF)
#define DPT_IS_VIRTUAL(t)   (((t)&DPT_VIRTUAL_MASK)!=0)

#define DPT_INDENT_OFF      4
#define DPT_INDENT_MASK     ((0x0f)<<DPT_INDENT_OFF)
#define DPT_INDENT(v)       ((((v)<<DPT_INDENT_OFF))&DPT_INDENT_MASK)
#define DPT_GET_INDENT(t)   (((t)&DPT_INDENT_MASK)>>DPT_INDENT_OFF)

#define DPT_SIZE_OFF        8
#define DPT_SIZE_MASK       ((0x0f)<<DPT_SIZE_OFF)
#define DPT_SIZE(v)         ((((v)<<DPT_SIZE_OFF))&DPT_SIZE_MASK)
#define DPT_GET_SIZE(t)     (((t)&DPT_SIZE_MASK)>>DPT_SIZE_OFF)

#define DPT_DECIMAL_OFF     12
#define DPT_DECIMAL_MASK    (1<<DPT_DECIMAL_OFF)
#define DPT_DECIMAL         (1<<DPT_DECIMAL_OFF)
#define DPT_IS_DECIMAL(t)   (((t)&DPT_DECIMAL_MASK)!=0)

#endif //SWP_ACCESS_SOC_TYPE_H

