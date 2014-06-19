ALL_SRCS += $(shell find ./src -name \*.c)
OBJS += $(ALL_SRCS:.c=.o)
CFLAGS	+= -I./h -I$(SOFTWAREDIR)/include -I./h -I$(PRODUCTDIR)/include
all:$(OBJS)
	$(AR) -r libmcast.a $(OBJS)
clean:
	rm -rf ./src/*.o 
	rm -rf libmcast.a