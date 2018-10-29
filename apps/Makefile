
CORE_OBJECTS=${wildcard ../src/*.o}
APPS_CPP_FILES=${wildcard *.cpp}
APPS_OBJECTS=${APPS_CPP_FILES:.cpp=.o}
APPS=${APPS_CPP_FILES:.cpp=-app}
DEP=${APPS_CPP_FILES:.cpp=.d}
CXXFLAGS+= -I../include -I../matgui/include/matgui


all: .depend ${APPS}

-include .depend

.depend: ${DEP}
	@echo summarizing dependencies...
	@cat ${DEP} > $@

%.d: %.cpp Makefile
	${CXX} ${CXXFLAGS} $< -MM -MT ${@:.d=.o} > $@ 
	
%-app: %.o ../matgui/matgui-sdl.a
	g++ -o $@ -Wl,--start-group ${CORE_OBJECTS} $^ `grep -oP '(?<=//LIBS=).*' ${@:-app=.cpp}` -Wl,--end-group ${LDFLAGS}
	

clean:
	rm -f ${APPS}
	rm -f *.o
	rm -f .depend
	rm -f *.d
	
