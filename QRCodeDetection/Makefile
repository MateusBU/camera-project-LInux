# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -O2 -Iinc $(shell pkg-config --cflags opencv4)
LIBS = $(shell pkg-config --libs opencv4)

# Output binary name and source files
TARGET = qrDetection
SRC = main.cpp

# Default rule (runs when calling just 'make')
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

# Rule to clean generated files
clean:
	rm -f $(TARGET)

.PHONY: all clean