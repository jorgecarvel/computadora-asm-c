# ============================================================
#   Makefile Maestro - Proyecto Von Neumann (Integrado)
# ============================================================

CC = gcc
CFLAGS = -Wall -g
SRC_DIR = src
BUILD_DIR = build
EXAMPLES = ejemplos

CPU_SRCS = $(SRC_DIR)/cpu_simulator.c $(SRC_DIR)/memoria.c $(SRC_DIR)/alu.c $(SRC_DIR)/cpu.c
ASM_SRCS = $(SRC_DIR)/assembler.c
COMP_SRCS = $(SRC_DIR)/c_to_asm.c
MAIN_SRC = $(SRC_DIR)/main.c

CPU = $(BUILD_DIR)/cpu_simulator
ASM = $(BUILD_DIR)/assembler
COMP = $(BUILD_DIR)/c_to_asm
MAIN = $(BUILD_DIR)/main

FACTORIAL_C = $(EXAMPLES)/factorial.c
FACTORIAL_ASM = $(BUILD_DIR)/factorial.asm
FACTORIAL_MEM = $(BUILD_DIR)/factorial.mem

# ============================================================
#   Regla principal
# ============================================================
all: dirs $(CPU) $(ASM) $(COMP) $(MAIN)

dirs:
	mkdir -p $(BUILD_DIR)

# ============================================================
#   Compiladores independientes
# ============================================================
$(CPU): $(CPU_SRCS)
	$(CC) $(CFLAGS) -o $(CPU) $(CPU_SRCS)

$(ASM): $(ASM_SRCS)
	$(CC) $(CFLAGS) -o $(ASM) $(ASM_SRCS)

$(COMP): $(COMP_SRCS)
	$(CC) $(CFLAGS) -o $(COMP) $(COMP_SRCS)

$(MAIN): $(MAIN_SRC)
	$(CC) $(CFLAGS) -o $(MAIN) $(MAIN_SRC)

# ============================================================
#   Pipeline completo
# ============================================================

asm: $(COMP)
	$(COMP) $(FACTORIAL_C)
	mv factorial.asm $(FACTORIAL_ASM)

mem: asm $(ASM)
	$(ASM) $(FACTORIAL_ASM) $(FACTORIAL_MEM)

run: mem $(CPU)
	$(CPU) $(FACTORIAL_MEM)

# ============================================================
#   Limpieza
# ============================================================
clean:
	rm -rf $(BUILD_DIR)/*

<<<<<<< HEAD
=======


>>>>>>> 3f14a8867c6b4f7bdc29a45de37cf8e4e9dfae36
