breakout: breakout.o physics.o graphics.o
	g++ -g `sdl-config --cflags` -o breakout -L/usr/local/lib/ -lSDL -lSDL_image breakout.o physics.o graphics.o

breakout.o: breakout.cpp
	g++ -g `sdl-config --cflags` -c breakout.cpp

physics.o: physics.cpp physics.h
	g++ -g -c physics.cpp

graphics.o: graphics.cpp graphics.h
	g++ -g `sdl-config --cflags` -c graphics.cpp

