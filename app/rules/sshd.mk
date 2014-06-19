# Rules for making loopd
sshd-config:
	if [ ! -f sshd/Makefile ];then \
	cd sshd;	echo $(shell pwd);./config.sh;cd ..;fi
sshd-build:sshd-config
	@$(MAKE) -C sshd \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		DESTDIR="$(TARGETDIR)"\
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		all
		
sshd-install:
	@$(MAKE) -C sshd \
		CROSS_COMPILE="$(CROSS_COMPILE)" \
		KBUILD_HAVE_NLS=no \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		DESTDIR="$(TARGETDIR)" \
		install
#	@cp loopd/loopd /tftpboot

sshd-clean:
	 if [ -f sshd/Makefile ] ;then $(MAKE) -C sshd clean ;fi
