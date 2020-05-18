breakout: breakout.o physics.o
	g++ -o breakout -lSDL -lSDL_image breakout.o physics.o

breakout.o: breakout.cpp
	g++ `sdl-config --cflags` -c breakout.cpp

physics.o: physics.cpp physics.h
	g++ -c physics.cpp

