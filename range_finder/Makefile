CC = gcc 
CFLAGS = -Wall -Wextra -Wpedantic -Wshadow -std=c11 -g -Iinclude 
LDFLAGS = -lm
NAME = range

# defining the target 
TARGET = $(NAME).exe 

# finding the sources 
MAIN_SRC = $(NAME).c
SRC_INCLUDE = $(wildcard include/*.c)
SRCS = $(MAIN_SRC) $(SRC_INCLUDE)


# getting the object file 
OBJS = $(SRCS:.c=.o)

.PHONY : all run clean 

all : $(TARGET)

# compiling the srouces files both in the include and in the main folder 
$(TARGET) : $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

# linking the object files to the source files 
# %.o means any file with the extension .o 
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

run : $(TARGET)
	./$(TARGET)

clean : 
	del /Q $(OBJS) $(TARGET)
# 	rm -rf $(OBJS) $(TARGET) #THIS IS USED FOR THE LINUX PROGRAMMING IN C