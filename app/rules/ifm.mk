#rules for making test
IFM_DIR = ifm

ifm-build:
	$(MAKE) -C $(IFM_DIR) all

ifm-install:
	$(MAKE) -C $(IFM_DIR) install

ifm-clean:
	$(MAKE) -C $(IFM_DIR) clean
