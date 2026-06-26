CC = gcc

CFLAGS = -Wall -Wextra -std=c99
CFLAGS += -Iinclude
CFLAGS += -Ilibsodium-1.0.22-mingw/libsodium-win64/include

LDFLAGS = -Llibsodium-1.0.22-mingw/libsodium-win64/lib
LDLIBS = -lsodium

TARGET = main

SRC = main.c include/user_t.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	del /Q main.exe main 2>nul
	del /Q main.o include\*.o 2>nul