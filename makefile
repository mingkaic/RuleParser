CC = g++
DEBUG = -g
CFLAG = -std=c++11

all:
	$(CC) $(CFLAG) *.cpp -o test

clean:
	rm test