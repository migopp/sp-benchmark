CC = g++
CC_FLAGS = -std=c++20 -Wall
SRC_DIR = src
BUILD_DIR = build
RES_DIR = res
LOGS_DIR = logs
TARGET = benchmark

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# .o -> exe
$(BUILD_DIR)/$(TARGET): $(OBJ_FILES) | $(BUILD_DIR)
	$(CC) $(CC_FLAGS) -o $@ $^

# .cpp -> .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CC_FLAGS) -c $< -o $@

# make dirs
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR) $(RES_DIR) $(LOGS_DIR)

# clean up!
clean:
	rm -rf $(BUILD_DIR) $(RES_DIR) $(LOGS_DIR)
