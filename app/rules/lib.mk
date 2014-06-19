# Rules for making library

lib-install:
	rm -fr $(TARGETDIR)/lib
	cp -r $(TOPDIR)/lib/targetlib $(TARGETDIR)/lib