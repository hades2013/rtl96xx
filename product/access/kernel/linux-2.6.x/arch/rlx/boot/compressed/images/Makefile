
#
# This dir holds all of the images for MIPS machines.
# Tom Rini     January 2001
# Pete Popov   2004

extra-y                := vmlinux.bin vmlinux.gz

vmlinux:
	#cp -rf ../../../../../vmlinux  vmlinux

OBJCOPYFLAGS_vmlinux.bin := -O binary
$(obj)/vmlinux.bin: vmlinux FORCE
	echo "building: " $@
#	$(OBJCOPY) -O binary -R .note -R .comment -S vmlinux  $@
	cp $(PRODUCTDIR)/release/$(ENV_PRO)/vmlinux.bin $@

#        $(call if_changed,objcopy)

#$(obj)/vmlinux.gz: $(obj)/vmlinux.bin FORCE
$(obj)/vmlinux.gz: vmlinux FORCE
	echo "building: " $@
	rm -f $@ $(obj)/vmlinux.prezip
	#cp $< $(obj)/vmlinux.prezip
	cp $(PRODUCTDIR)/release/$(ENV_PRO)/vmlinux.bin $(obj)/vmlinux.prezip	 
	gzip -9 $(obj)/vmlinux.prezip
	mv  $(obj)/vmlinux.prezip.gz $@
#       $(call if_changed,gzip)

# Files generated that shall be removed upon make clean
clean-files    := vmlinux* zImage* 

.PHONY: zImage

zImage: $(obj)/vmlinux.gz
