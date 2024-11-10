.PHONY: all debug release

all: release

release:
	@$(MAKE) -sf build.mk TARGET=Release

debug:
	@$(MAKE) -sf build.mk TARGET=Debug

clean:
	@$(MAKE) -sf build.mk TARGET=Release clean
	@$(MAKE) -sf build.mk TARGET=Debug clean
