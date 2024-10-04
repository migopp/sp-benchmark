CC = g++
CC_FLAGS = -std=c++23 -Wall
SRC_DIR = src
BUILD_DIR = build
TARGET = benchmark

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# .o -> exe
$(BUILD_DIR)/$(TARGET): $(OBJ_FILES) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# .cpp -> .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# build dir
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# clean up!
clean:
	rm -rf $(BUILD_DIR)
