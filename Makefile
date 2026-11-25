CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2
SRC = src/main.c src/paciente.c src/fila.c src/atendimento.c src/menu.c
TEST_SRC = tests/teste_sistema.c src/paciente.c src/fila.c src/atendimento.c
TARGET = sistema_senhas
TEST_TARGET = teste_sistema

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

test: $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe $(TEST_TARGET) $(TEST_TARGET).exe *.o
