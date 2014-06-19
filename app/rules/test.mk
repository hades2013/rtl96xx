#rules for making test
TEST_DIR = test

test-build:
	$(MAKE) -C $(TEST_DIR) all

test-install:
	$(MAKE) -C $(TEST_DIR) install

test-clean:
	$(MAKE) -C $(TEST_DIR) clean
