breakout: breakout.o physics.o graphics.o
	g++ -g -o breakout breakout.o physics.o graphics.o `sdl2-config --libs` -lSDL2_image

breakout.o: breakout.cpp
	g++ -g `sdl2-config --cflags` -c breakout.cpp

physics.o: physics.cpp physics.h
	g++ -g -c physics.cpp

graphics.o: graphics.cpp graphics.h
	g++ -g `sdl2-config --cflags` -c graphics.cpp

