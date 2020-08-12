breakout: breakout.o physics.o graphics.o
	g++ -o breakout -lSDL -lSDL_image breakout.o physics.o graphics.o

breakout.o: breakout.cpp
	g++ `sdl-config --cflags` -c breakout.cpp

physics.o: physics.cpp physics.h
	g++ -c physics.cpp

graphics.o: graphics.cpp graphics.h
	g++ -c graphics.cpp

