
CFLAGS	+= -I$(PDT_INC_DIR)

OBJS = mkuboot.o md5.o
all:mkuboot

mkuboot:$(OBJS)
	@gcc -o $@ $^
clean:
	@rm -rf *.o mkuboot .*.depend

%.o: %.c .%.depend
	@echo " [cm] CC $@"
	@gcc $(CFLAGS) -o $@ -c $<


.%.depend: %.c
	@gcc $(CFLAGS) -M $< > $@

-include $(OBJS:%.o=.%.depend)	
