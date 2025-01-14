.PHONY: all pgo-instrument pgo-run pgo-build clean release debug dirs link

all: 
	@$(MAKE) -sf build.mk 

release:
	@$(MAKE) -sf build.mk TARGET=Release

debug:
	@$(MAKE) -sf build.mk TARGET=Debug

clean:
	@$(MAKE) -sf build.mk TARGET=Release clean
	@$(MAKE) -sf build.mk TARGET=Debug clean
