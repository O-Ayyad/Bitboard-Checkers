CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

SRC = main.c game_board.c utils.c
OBJ = $(SRC:.c=.o)
TARGET = checkers

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c game_board.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)