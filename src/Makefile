CC = g++
CFLAGS = -g -Wall
BUILD_FOLDER := build
.DEFAULT_GOAL := main.exe
.PHONY: main.exe debug clean

# 1. DETECT OPERATING SYSTEM
ifeq ($(OS),Windows_NT)
    # Windows (MinGW / MSYS2) configuration
    LDFLAGS = -lglew32 -lglfw3 -lopengl32 -lgdi32
    MKDIR = if not exist $(BUILD_FOLDER) mkdir $(BUILD_FOLDER)
    RM_DIR = rmdir /s /q
    RM_FILE = del /q /f
else
    # Linux / macOS configuration
    LDFLAGS = -lGL -lGLEW -lglfw
    MKDIR = mkdir -p $(BUILD_FOLDER)
    RM_DIR = rm -rf
    RM_FILE = rm -f
endif

# 2. AUTOMATIC FILE DETECTION
SOURCES := $(wildcard *.cpp)
OBJECTS := $(foreach x, $(basename $(notdir $(SOURCES))), $(BUILD_FOLDER)/$(x).o)
LIBS :=

# 3. COMPILATION RULES
# Order-only dependency ensures the build folder is safely created first
$(BUILD_FOLDER)/%.o: %.cpp | $(BUILD_FOLDER)
	$(CC) $(CFLAGS) -c $< -o $@

# Safe linker order: $(OBJECTS) comes BEFORE $(LDFLAGS)
main.exe: $(OBJECTS)
	$(CC) $(foreach x, $(LIBS), -I $x) $(OBJECTS) -o main.exe $(LDFLAGS)

$(BUILD_FOLDER):
	$(MKDIR)

debug:
	@echo "OS Detected: $(OS)"
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Flags: $(LDFLAGS)"

clean:
	$(RM_DIR) $(BUILD_FOLDER)
	$(RM_FILE) main.exe