
CXXFLAGS= -I.. -Wall -Wextra -Wfatal-errors -std=c++14 -Dcimg_use_vt100 -Dcimg_display=1
CXXFLAGS+= -g -o0
LDFLAGS=   -lm -lX11 -lpthread
OBJECTS = util.o layer.o
TARGETS = main test training mod1nn
OTHER = main.o training.o mod1nn.o $(TARGETS) #Objects that is to be removed when cleaning the project

all: $(TARGETS)

main: $(OBJECTS) main.o gui.o
	g++ $(OBJECTS) gui.o main.o -o main $(LDFLAGS)

training: $(OBJECTS) training.o test
	g++ $(OBJECTS) training.o -o training $(LDFLAGS)

mod1nn: $(OBJECTS) mod1nn.o gui.o
	g++ $(OBJECTS) mod1nn.o gui.o -o mod1nn $(LDFLAGS)


test: test.o util.o
	g++ test.o util.o -o test $(LDFLAGS)

main.o: gui.h valuemap.h layer.h network.h util.h
gui.o: gui.h valuemap.h util.h
layer.o: layer.h

common.dep: network.h valuemap.h layer.h util.h
	touch common.dep

test.o: common.dep
training.o: common.dep
mod1nn.o: common.dep

clean:
	rm -f $(OBJECTS)
	rm -f $(OTHER)