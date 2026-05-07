# ==========================================
# Rutas de SystemC
# ==========================================
SYSTEMC_HOME = /usr/local/systemc-3.0.2
SYS_INCDIR = -I$(SYSTEMC_HOME)/include
SYS_LIBDIR = -L$(SYSTEMC_HOME)/lib-linux64

# ==========================================
# Carpetas del Proyecto
# ==========================================
INC_DIR = include
SRC_DIR = src
TEST_DIR = test
BIN_DIR = bin
TRACES_DIR = traces

# ==========================================
# Configuración del Compilador
# ==========================================
CXX = g++
# Añadimos -I$(INC_DIR) para que g++ encuentre tus .h locales
CXXFLAGS = -std=c++17 -O2 -Wall $(SYS_INCDIR) -I$(INC_DIR)
LIBS = -lsystemc -lm

# ==========================================
# Archivos y Objetivos
# ==========================================

# Aplicaciones principales en src/ (ej: main_visualizer.cpp -> bin/asm_visualizer)
MAIN_SRCS = $(wildcard $(SRC_DIR)/main_*.cpp)
MAIN_APPS = $(patsubst $(SRC_DIR)/main_%.cpp,$(BIN_DIR)/asm_%,$(MAIN_SRCS))

# Testbenches en test/ (ej: tb_adder.cpp -> bin/test_adder)
TEST_SRCS = $(wildcard $(TEST_DIR)/tb_*.cpp)
TEST_BINS = $(patsubst $(TEST_DIR)/tb_%.cpp,$(BIN_DIR)/test_%,$(TEST_SRCS))

# ==========================================
# Reglas
# ==========================================
all: directories $(MAIN_APPS)

# Crea las carpetas si no existen
directories:
	@mkdir -p $(BIN_DIR) $(TRACES_DIR)

# Compila las aplicaciones (src/)
$(BIN_DIR)/asm_%: $(SRC_DIR)/main_%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(SYS_LIBDIR) $(LIBS)

# Compila los testbenches (test/)
$(BIN_DIR)/test_%: $(TEST_DIR)/tb_%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(SYS_LIBDIR) $(LIBS)

# Limpieza total
clean:
	rm -rf $(BIN_DIR)/* $(TRACES_DIR)/* *.o *.vcd

.PHONY: all clean directories
