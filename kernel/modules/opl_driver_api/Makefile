CUR_DIR = $(shell /bin/pwd)
SRC = opl_modif.c
OBJS = opl_modif.o
TARGET = libopl_modif.a
CC = mips-linux-uclibc-gcc
AR = mips-linux-uclibc-ar
#CFLAGS = -I$(CUR_DIR)/include

.PHONY:all
.PHONY:symlink
all:symlink $(TARGET)
symlink:
	rm -rf ./include 
	ln -s ../include/ include
$(TARGET):$(OBJS)
	$(AR) vru $@ $(OBJS)
#$(TARGET):$(SRC)
#	$(CC) -fpic -shared -nostdlib $(SRC) -o $@
clean:
	rm -rf ./include
	rm -rf $(CUR_DIR)/*~
	rm -rf $(CUR_DIR)/$(TARGET)
	rm -rf $(CUR_DIR)/*.o
