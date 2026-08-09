CXX = g++
CXXFLAGS = -Wall -O2 -Iinc $(shell pkg-config --cflags opencv4)
LIBS = $(shell pkg-config --libs opencv4)

TARGET = faceDetection
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean