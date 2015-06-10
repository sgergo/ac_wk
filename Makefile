EXECUTABLE := $(shell basename `pwd`)
SRC = $(shell find . -name '[!.]*.c' -not -path "./contrib/*")

all: *.c *.h
	gcc -Wall -O4 $(SRC) -o $(EXECUTABLE) -lm

clean:
	rm -f $(EXECUTABLE)
