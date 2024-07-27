CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = simple_server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
