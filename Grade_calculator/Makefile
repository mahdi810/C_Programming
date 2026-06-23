CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Ilibsodium-1.0.22-mingw/libsodium-win64/include
LDFLAGS = -Llibsodium-1.0.22-mingw/libsodium-win64/lib
LDLIBS = -lsodium

TARGET = grade_calculator
SRCS = grade_calculator.c

all: run

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean