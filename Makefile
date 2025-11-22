MAKEFLAGS += -j$(shell nproc)
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra \
    -I./include \
    -I./include/imgui \
    -I./include/rlImGui

LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = build/utility
OBJ_DIR = out

# Source files
SRCS = src/main.cpp \
       include/imgui/imgui.cpp \
       include/imgui/imgui_draw.cpp \
       include/imgui/imgui_tables.cpp \
       include/imgui/imgui_widgets.cpp \
       include/rlImGui/rlImGui.cpp

OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

# Default Linux build
all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Windows build
windows:
	$(MAKE) clean
	$(MAKE) CXX=x86_64-w64-mingw32-g++ \
	        CXXFLAGS="-std=c++17 -Wall -Wextra -I./include -I./include/imgui -I./include/rlImGui -I./include/raylibWin64/include" \
	        LDFLAGS="-L./include/raylibWin64/lib -lraylib -lopengl32 -lgdi32 -lwinmm" \
	        TARGET=build/utility.exe

clean:
	rm -rf $(OBJ_DIR) build

.PHONY: all windows clean