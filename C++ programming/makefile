CC = g++

FILE ?= .

compile : 
	$(CC) $(FILE).cpp -o $(FILE) 

run: 
	./$(FILE)


all: compile run

clean : 
	del /Q *.o *.exe
	
.PHONY: compile run all clean 