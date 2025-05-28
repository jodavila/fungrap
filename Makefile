# General definitions
CXX        := g++
CXXFLAGS   := -std=c++11 -Wall -Wno-unused-function -g -I ./include
SRC        := src/main.cpp src/glad.c src/textrendering.cpp src/camera.cpp src/timer.cpp src/matrices.cpp src/glcontext.cpp src/geometrics.cpp
DEPS       := include/matrices.hpp include/utils.h include/dejavufont.h

# Linux
LIB_LINUX     := ./lib-linux/libglfw3.a -lrt -lm -ldl -lX11 -lpthread -lXrandr -lXinerama -lXxf86vm -lXcursor
BIN_DIR_LINUX := bin/Linux
TARGET_LINUX  := $(BIN_DIR_LINUX)/main

# Windows (MinGW)

LIB_WINDOWS     := ./lib-mingw-64/libglfw3.a -lgdi32 -lopengl32 -luser32 -lkernel32
BIN_DIR_WINDOWS := bin/Windows
TARGET_WINDOWS  := $(BIN_DIR_WINDOWS)/main.exe

# Detects the OS
ifeq ($(OS),Windows_NT)
  TARGET := $(TARGET_WINDOWS)
else
  TARGET := $(TARGET_LINUX)
endif

.PHONY: all linux windows clean run

all: clean $(TARGET) run

# Linux
$(TARGET_LINUX): $(SRC) $(DEPS)
	@echo ">>> Compilando para Linux em $(TARGET_LINUX)"
	@mkdir -p $(BIN_DIR_LINUX)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(LIB_LINUX)

# Windows
$(TARGET_WINDOWS): $(SRC) $(DEPS)
	@echo ">>> Compilando para Windows em $(TARGET_WINDOWS)"
	@mkdir -p $(BIN_DIR_WINDOWS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(LIB_WINDOWS)


linux: $(TARGET_LINUX)
windows: $(TARGET_WINDOWS)

# Run executables according to the OS (only Linux and Windows for now)
run: $(TARGET)
ifeq ($(OS),Windows_NT)
	@echo ">>> Executando $(TARGET_WINDOWS)"
	@cd $(BIN_DIR_WINDOWS) && ./main
else
	@echo ">>> Executando $(TARGET_LINUX)"
	@cd $(BIN_DIR_LINUX) && ./main
endif

clean:
	@echo ">>> Removendo bins"
	@rm -f $(TARGET_LINUX) $(TARGET_WINDOWS)
