# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude
LDFLAGS = -lSDL2

# Source files and target
SRC_DIR = src
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = nes-emulator

# Main rule
all: $(TARGET)

# Link the program
$(TARGET): $(OBJECTS)
    $(CXX) -o $@ $^ $(LDFLAGS)

# Compile sources
%.o: %.cpp
    $(CXX) -c $(CXXFLAGS) -o $@ $<

# Clean build files
clean:
    rm -f $(OBJECTS) $(TARGET)

# Run the emulator
run: $(TARGET)
    ./$(TARGET) roms/game.nes

.PHONY: all clean run