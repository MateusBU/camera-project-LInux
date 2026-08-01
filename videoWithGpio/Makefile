CXX := g++

INC_DIR := inc
SRC_DIR := src
BUILD_DIR := build

CXXFLAGS := -Wall -Wextra -std=c++17 -I$(INC_DIR) $(shell pkg-config --cflags opencv4)
LDFLAGS := $(shell pkg-config --libs opencv4) -lgpiod

TARGET := videoWithGpio

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(wildcard $(INC_DIR)/*.hpp)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)