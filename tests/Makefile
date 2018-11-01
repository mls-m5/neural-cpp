CORE_OBJECTS=${wildcard ../src/*.o}
TEST_CPP_FILES=${wildcard *-test.cpp}
TESTS=${TEST_CPP_FILES:.cpp=}
DEP=${TEST_CPP_FILES:.cpp=.d}
CXXFLAGS=-I../include -g


all: .depend ${TESTS}

-include .depend

.depend: ${DEP}
	@echo summarizing dependencies...
	@cat ${DEP} > $@

%.d: %.cpp Makefile
	@${CXX} ${CXXFLAGS} $< -MM -MT ${@:.d=} > $@ 
	
%-test: %-test.cpp ${CORE_OBJECTS}
	echo ${CORE_OBJECTS}
	echo ${LDFLAGS}
	echo $^
	g++ -o $@ ${CXXFLAGS} ${CORE_OBJECTS} $@.cpp ${LDFLAGS}
	
clean:
	rm -f ${TESTS}
	rm -f *.d
	rm -f .depend
