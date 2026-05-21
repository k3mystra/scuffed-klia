CC = g++
CFLAGS = -g -Wall
LDFLAGS = -lGL -lGLEW -lglfw


# bin: main.cpp
# 	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


.DEFAULT_GOAL := main.exe
.PHONY: main.exe

BUILD_FOLDER := build

SOURCES := $(wildcard *.cpp)
# For each name in SOURCES (notdir removes the folder name, basename removes the file extension), append .o
OBJECTS := $(foreach x, $(basename $(notdir $(SOURCES))), $(BUILD_FOLDER)/$(x).o)

LIBS :=

$(BUILD_FOLDER)/%.o: %.cpp
	g++ $(CFLAGS) $< -c -o $@

main.exe: $(OBJECTS)
	g++ $(LDFLAGS) $(foreach x, $(LIBS), -I $x) $(OBJECTS) -o main.exe

# @ before a command to not print the command itself
debug:
	@echo $(LIBS)
	@echo $(SOURCES)
	@echo $(OBJECTS)

$(BUILD_FOLDER):
	mkdir $(BUILD_FOLDER)
