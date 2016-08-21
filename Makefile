
CXXFLAGS= -I.. -Wall -Wextra -Wfatal-errors -std=c++11 -Dcimg_use_vt100 -Dcimg_display=1
CXXFLAGS+= -g -o0
LDFLAGS=   -lm -lX11 -lpthread
OBJECTS = main.o gui.o util.o

all: $(OBJECTS)
	g++ $(OBJECTS) -o main $(LDFLAGS)



main.o: gui.h valuemap.h
gui.o: gui.h valuemap.h
