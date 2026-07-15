CC = g++
CFLAGS = -g -Wall -D GLM_ENABLE_EXPERIMENTAL -D GLM_FORCE_QUAT_DATA_WXYZ -D ENABLE_OPENGL_DEBUG
BUILD_FOLDER := build

ifeq ($(OS),Windows_NT)
    # Windows (MinGW / MSYS2) configuration
    LDFLAGS = -lglew32 -lglfw3 -lopengl32 -lgdi32
    MKDIR = if not exist $(BUILD_FOLDER) mkdir $(BUILD_FOLDER)
    RM_DIR = rmdir /s /q
    RM_FILE = del /q /f
    TARGET = main.exe
else
    # Linux / macOS configuration
    LDFLAGS = -lGL -lGLEW -lglfw
    MKDIR = mkdir -p $(BUILD_FOLDER)
    RM_DIR = rm -rf
    RM_FILE = rm -f
    TARGET = main
endif

.PHONY: debug clean
.DEFAULT_GOAL := $(TARGET)

SOURCES := $(wildcard *.cpp)
OBJECTS := $(foreach x, $(basename $(notdir $(SOURCES))), $(BUILD_FOLDER)/$(x).o)
LIBS :=
INCLUDE := include
HEADER_LIB := include/pch.h
COMPILED_HEADER_LIB := include/pch.h.gch

$(COMPILED_HEADER_LIB): $(HEADER_LIB)
	$(CC) $(CFLAGS) -x c++-header $^

# Order-only dependency ensures the build folder is safely created first
$(BUILD_FOLDER)/%.o: %.cpp | $(BUILD_FOLDER) $(COMPILED_HEADER_LIB)
	$(CC) $(CFLAGS) -include $(HEADER_LIB) -c $< -o $@

# Safe linker order: $(OBJECTS) comes BEFORE $(LDFLAGS)
$(TARGET): $(OBJECTS)
	$(CC) $(foreach x, $(LIBS), -I $x) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(BUILD_FOLDER):
	$(MKDIR)

debug:
	@echo "OS Detected: $(OS)"
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Flags: $(LDFLAGS)"

clean:
	$(RM_DIR) $(BUILD_FOLDER)
	$(RM_FILE) $(TARGET)
	$(RM_FILE) $(COMPILED_HEADER_LIB)
