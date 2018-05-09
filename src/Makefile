
CXXFLAGS= -I.. -Wall -Wextra -Wfatal-errors -std=c++14 -Dcimg_use_vt100 -Dcimg_display=1
CXXFLAGS+= -g -o0 -fmax-errors=10
LDFLAGS=   -lm -lX11 -lpthread 
OBJECTS = util.o layer.o
TARGETS = ../main ../test ../training ../mod1nn ../mod2svm ../neural_main ../mnist1
OTHER = main.o training.o mod1nn.o test.o neural_main.o $(TARGETS) #Objects that is to be removed when cleaning the project

all: $(TARGETS)

../main: $(OBJECTS) main.o gui.o
	g++ $(OBJECTS) gui.o main.o -o ../main $(LDFLAGS)

../training: $(OBJECTS) training.o
	g++ $(OBJECTS) training.o -o ../training $(LDFLAGS)

../mod1nn: $(OBJECTS) mod1nn.o gui.o
	g++ $(OBJECTS) mod1nn.o gui.o -o ../mod1nn $(LDFLAGS)

../mod2svm: $(OBJECTS) mod2svm.o gui.o
	g++ $(OBJECTS) mod2svm.o gui.o -o ../mod2svm $(LDFLAGS)

../neural_main: $(OBJECTS) mod2svm.o gui.o
	g++ $(OBJECTS) gui.o neural_main.o -o ../neural_main $(LDFLAGS)

../test: test.o util.o
	g++ test.o util.o -o ../test $(LDFLAGS)
	
../mnist1:  $(OBJECTS) mnist1.o gui.o
	g++ $(OBJECTS) gui.o mnist1.o -o ../mnist1 $(LDFLAGS)
	

main.o: gui.h valuemap.h layer.h network.h util.h
gui.o: gui.h valuemap.h util.h
layer.o: layer.h

common.dep: network.h valuemap.h layer.h util.h
	touch common.dep

test.o: common.dep test.cpp
training.o: common.dep training.cpp
mod1nn.o: common.dep mod1nn.cpp
mod2svm.o: common.dep mod2svm.cpp
neural_main.o: common.dep neural_main.cpp
minst1.o: common.dep mnist1.cpp

clean:
	rm -f $(OBJECTS)
	rm -f $(OTHER)