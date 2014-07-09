# toolchain configuration

#ifndef CONFIG_PRODUCT_EPN104N
#CFLAGS = -Os -Wall -pipe -mips32 -mtune=mips32 -msoft-float -funit-at-a-time  -fpic
#CXXFLAGS = -Os -Wall -pipe -mips32 -mtune=mips32 -funit-at-a-time  -msoft-float -fpic
#CPPFLAGS += -I$(TOOLCHAIN_DIR)/usr/include \
#	-I$(TOOLCHAIN_DIR)/include -I$(SOFTWAREDIR)/include -I$(PDT_INC_DIR)
#LDFLAGS += -L$(TOOLCHAIN_DIR)/usr/lib \
# 	-L$(TOOLCHAIN_DIR)/lib \
# 	-L$(TOOLCHAIN_DIR)/usr/lib
##else
CFLAGS = -Os -Wall -pipe -mtune=mips32 -msoft-float -funit-at-a-time -fpic -g
CXXFLAGS = -Os -Wall -pipe -mtune=mips32 -funit-at-a-time -msoft-float -fpic
CPPFLAGS += -I$(TOOLCHAIN_DIR)/usr/include  -I$(TOOLCHAIN_DIR)/include -I$(SOFTWAREDIR)/include -I$(PDT_INC_DIR)
LDFLAGS += -L$(TOOLCHAIN_DIR)/usr/lib \
 	-L$(TOOLCHAIN_DIR)/lib \
 	-L$(TOOLCHAIN_DIR)/usr/lib\
 	-T$(SOFTWAREDIR)/rules/shared.lds   -lifm -L$(PRODUCTDIR)/lwlib -lshared -L$(SOFTWAREDIR)/shared  -lcfg -L$(PRODUCTDIR)/lwlib -lm
##endif
export CFLAGS CXXFLAGS CPPFLAGS LDFLAGS
