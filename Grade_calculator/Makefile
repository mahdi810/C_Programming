CC = gcc

CFLAGS = -Wall -Wextra -std=c99
CFLAGS += -Iinclude
CFLAGS += -Ilibsodium-1.0.22-mingw/libsodium-win64/include
CFLAGS += -MMD -MP

LDFLAGS = -Llibsodium-1.0.22-mingw/libsodium-win64/lib
LDLIBS = -lsodium

TARGET = main

# Find all C source files automatically
SRC = $(wildcard *.c) $(wildcard include/*.c)
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)

run: all
	./$(TARGET)

clean:
	del /Q main.exe main *.o *.d include\*.o include\*.d 2>nul