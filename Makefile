CC = gcc
CFLAGS = -Wall -g
SRC = src/cpu_simulator.c src/memoria.c src/alu.c src/cpu.c
TARGET = cpu_simulator

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

