CC = g++
CFLAGS = -g -Wall
LDFLAGS = -lGL -lGLEW -lglfw


bin: main.cpp
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
