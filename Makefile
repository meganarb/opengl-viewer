CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Iinclude

LIBS := -lglfw -lGLEW -lGL

TARGET := main

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) 

rebuild: clean all

.PHONY: all clean rebuild
