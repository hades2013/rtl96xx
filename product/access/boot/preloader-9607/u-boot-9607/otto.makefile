.PHONY: $(OTTO_SOC_H) $(OTTO_PBLR_H) .otto.v

$(OTTO_SOC_H) $(OTTO_PBLR_H):
	@if [ ! -e $(OTTO_RELEASE_DIR)/$(notdir $@) ]; then \
		if [ ! -e $@ ]; then \
			echo "EE: Missing $@" ; \
			exit 1; \
		fi \
	else \
		cmp $@ $(OTTO_RELEASE_DIR)/$(notdir $@) > /dev/null 2>&1 ; \
		if [ "$$?" -ne 0 ]; then \
			echo -n "[MISC] Generating $@... " ; \
			sed -e '/^#define USE_ON_FLASH_SECTION$$/d' -e '/^#define .\+SECTION.\+/d' $(OTTO_RELEASE_DIR)/$(notdir $@) > $@ ; \
			echo "done" ; \
		fi \
	fi

otto_distclean:
	@rm -f $(OTTO_SOC_H) $(OTTO_PBLR_H) .otto.E .otto.mk arch/otto/lib/plr_flash.c
	@rm -rf $(CPUDIR)/$(SOC)/preloader

# script to get version from SVN and GIT
SVN_VCODE := svn info | grep 'Revision' | sed -e 's|Revision: ||'
SVN_MCODE := svn status | grep '^M' > /dev/null 2>&1 && echo 'M' || echo ''
GIT_VCODE := git log | head -c 15 | sed -e 's|commit ||'
GIT_MCODE := git status | grep 'modified:' > /dev/null 2>&1 && echo 'M' || echo ''

# Version of U-Boot
ifeq ($(wildcard ./.svn),./.svn)
UB_VER := $(shell $(SVN_VCODE))
UB_VER := $(UB_VER)$(shell $(SVN_MCODE))
else ifeq ($(wildcard ./.git),./.git)
UB_VER := $(shell $(GIT_VCODE))
UB_VER := $(UB_VER)$(shell $(GIT_MCODE))
else
UB_VER := NA
endif

# Version of Preloader
ifeq ($(wildcard $(OTTO_RELEASE_DIR)/../.svn),$(OTTO_RELEASE_DIR)/../.svn)
PL_VER := $(shell cd $(OTTO_RELEASE_DIR)/.. && $(SVN_VCODE))
PL_VER := $(PL_VER)$(shell cd $(OTTO_RELEASE_DIR)/.. && $(SVN_MCODE))
else ifeq ($(wildcard $(OTTO_RELEASE_DIR)/../.git),$(OTTO_RELEASE_DIR)/../.git)
PL_VER := $(shell cd $(OTTO_RELEASE_DIR)/.. && $(GIT_VCODE))
PL_VER := $(PL_VER)$(shell cd $(OTTO_RELEASE_DIR)/.. && $(GIT_MCODE))
else
PL_VER := NA
endif

.otto.v:
	@echo -n "[MISC] Generating $@... "
	@echo UB_VER := $(UB_VER) >  $@
	@echo PL_VER := $(PL_VER) >> $@
	@echo "done"

OTTO_DATE  := $(shell date +%y%m%d%H%M)
OTTO_RLZ_FN:= otto_uboot.$(OTTO_DATE).u$(UB_VER).p$(PL_VER).tgz
release: clobber
	@echo -n "[MISC] Generating tarball $(OTTO_RLZ_FN)... "
	@cp toolkit_path.in.sample toolkit_path.bak
	@cp config.in config.in.bak
	@sed '/^OTTO_RELEASE_DIR.*/d' toolkit_path.bak > toolkit_path.in.sample
	@sed -e "s|bool 'Standalone U-Boot'|define_bool 'Standalone U-Boot'|" \
		-e "s|CONFIG_STANDALONE_UBOOT|CONFIG_STANDALONE_UBOOT y|" config.in.bak > config.in
	@cd ../ && tar czf $(TOPDIR)/release/$(OTTO_RLZ_FN) $(notdir $(TOPDIR)) \
		--exclude=*.git* --exclude=otto.makefile --exclude=toolkit_path.in --exclude=OTTO_README \
		--exclude=toolkit_path.bak --exclude=config.in.bak --exclude=release
	@mv toolkit_path.bak toolkit_path.in.sample
	@mv config.in.bak config.in
	@echo "done"
