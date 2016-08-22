
CXXFLAGS= -I.. -Wall -Wextra -Wfatal-errors -std=c++11 -Dcimg_use_vt100 -Dcimg_display=1
CXXFLAGS+= -g -o0
LDFLAGS=   -lm -lX11 -lpthread
OBJECTS = main.o gui.o util.o

all: $(OBJECTS) test
	g++ $(OBJECTS) -o main $(LDFLAGS)


test: test.o util.o
	g++ test.o util.o -o test $(LDFLAGS)

main.o: gui.h valuemap.h layer.h network.h util.h
gui.o: gui.h valuemap.h util.h

test.o: network.h valuemap.h layer.h util.h

clean:
	rm -f $(OBJECTS)
	rm -f main