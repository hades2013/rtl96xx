#rules for making test
CFG_DIR = cfg

cfg-build:
	$(MAKE) -C $(CFG_DIR) all

cfg-install:
	$(MAKE) -C $(CFG_DIR) install

cfg-clean:
	$(MAKE) -C $(CFG_DIR) clean
