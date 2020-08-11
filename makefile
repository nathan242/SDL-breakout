breakout: breakout.o physics.o render.o
	g++ -o breakout -lSDL -lSDL_image breakout.o physics.o render.o

breakout.o: breakout.cpp
	g++ `sdl-config --cflags` -c breakout.cpp

physics.o: physics.cpp physics.h
	g++ -c physics.cpp

render.o: render.cpp render.h
	g++ -c render.cpp

