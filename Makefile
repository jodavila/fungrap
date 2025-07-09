# Compilador e flags base
CXX := g++
INCLUDE := -I./include
SRC_DIR := src
OBJ_DIR := build/obj
BIN_DIR_LINUX := bin/Linux
BIN_DIR_WINDOWS := bin/Windows

SRC := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(SRC:.cpp=.o))
OBJ := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(OBJ:.c=.o))

# Bibliotecas
LIB_LINUX := ./lib-linux/libglfw3.a -lrt -lm -ldl -lX11 -lpthread -lXrandr -lXinerama -lXxf86vm -lXcursor
LIB_WINDOWS := ./lib-mingw-64/libglfw3.a -lgdi32 -lopengl32 -luser32 -lkernel32

# Detecção de sistema
ifeq ($(OS),Windows_NT)
    TARGET := $(BIN_DIR_WINDOWS)/main.exe
    LIBS := $(LIB_WINDOWS)
    BIN_DIR := $(BIN_DIR_WINDOWS)
else
    TARGET := $(BIN_DIR_LINUX)/main
    LIBS := $(LIB_LINUX)
    BIN_DIR := $(BIN_DIR_LINUX)
endif

# Flags padrões
CXXFLAGS := -std=c++11 -Wall -Wno-unused-function $(INCLUDE)

.PHONY: all clean run fast release

# Compilação incremental padrão (debug)
all: CXXFLAGS += -g -O0
all: $(TARGET)

# Compilação rápida (debug)
fast: CXXFLAGS += -O0 -g
fast: $(TARGET)

# Compilação otimizada (release)
release: CXXFLAGS += -O3 -march=native
release: $(TARGET)

# Compilação final
$(TARGET): $(OBJ)
	@echo ">>> Linkando para $(TARGET)"
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Compilação dos objetos (sem sujar src/)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo ">>> Compilando $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo ">>> Compilando $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Executar
run: $(TARGET)
	@echo ">>> Executando $(TARGET)"
	@cd $(BIN_DIR) && ./$(notdir $(TARGET))

# Limpeza
clean:
	@echo ">>> Limpando objetos e binários"
	@rm -rf $(OBJ_DIR) $(BIN_DIR_LINUX) $(BIN_DIR_WINDOWS)
