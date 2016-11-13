
CXXFLAGS= -I.. -Wall -Wextra -Wfatal-errors -std=c++14 -Dcimg_use_vt100 -Dcimg_display=1
CXXFLAGS+= -g -o0
LDFLAGS=   -lm -lX11 -lpthread
OBJECTS = util.o layer.o
OTHER = main.o training main training.o #Objects that is to be removed when cleaning the project

all: main test training

main: $(OBJECTS) main.o gui.o
	g++ $(OBJECTS) gui.o main.o -o main $(LDFLAGS)

training: $(OBJECTS) training.o test
	g++ $(OBJECTS) training.o -o training $(LDFLAGS)

test: test.o util.o
	g++ test.o util.o -o test $(LDFLAGS)

main.o: gui.h valuemap.h layer.h network.h util.h
gui.o: gui.h valuemap.h util.h
layer.o: layer.h

test.o: network.h valuemap.h layer.h util.h

training.o: network.h valuemap.h layer.h util.h

clean:
	rm -f $(OBJECTS)
	rm -f $(OTHER)