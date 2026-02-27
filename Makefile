CC = gcc
CFLAGS = -Wall -Isrc
SRC = src/lexer.c src/main.c
TEST_SRC = src/lexer.c tests/test_lexer.c
TARGET = lexer
TEST_TARGET = test_lexer

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

$(TEST_TARGET): $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET)

.PHONY: all test clean
