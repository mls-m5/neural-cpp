
CORE_CPP_FILES=${wildcard *.cpp}
CORE_OBJECTS=${CORE_CPP_FILES:.cpp=.o}
DEP=${CORE_CPP_FILES:.cpp=.d}
CXXFLAGS+=-I../include

all: .depend ${CORE_OBJECTS}

-include .depend

.depend: ${DEP}
	cat ${DEP} > $@

%.d: %.cpp Makefile
	@${CXX} ${CXXFLAGS} $< -MM -MT ${@:.d=.o} > $@ 
 
clean:
	rm -f *.o
	rm -f *.d
	rm -f .depend
	
