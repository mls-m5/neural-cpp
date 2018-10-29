
CXXFLAGS= -I.. -Wall -Wfatal-errors -std=c++14 -Dcimg_use_vt100 -Dcimg_display=1
CXXFLAGS+= -o0 -fmax-errors=10
LDFLAGS=   -lm -lX11 -lpthread

export LDFLAGS CXXFLAGS

all: text-header
	make -C matgui/
	make -C src/
	make -C apps/
	make -C tests/

debug: CXXFLAGS += -g
debug: all

clean:
	make -C matgui/ clean
	make -C src/ clean
	make -C apps/ clean
	make -C tests/ clean
	
text-header:
	@echo
	@echo
	@echo
	@echo
	@echo
	@echo
	@echo ============================================================================================
	@echo
	@echo "                                   Building project"
	@echo
	@echo ============================================================================================
	@echo
	
	

	