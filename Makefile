CFLAGS=-Wall -Werror -std=c99 -g -Og -DDEBUG

EXECUTABLE=bin/server
DEPS=src/server.c

all:$(EXECUTABLE)
	./$(EXECUTABLE)

build:$(EXECUTABLE)

$(EXECUTABLE):$(DEPS)
	@mkdir -p bin
	gcc -o $(EXECUTABLE) $(DEPS) $(CFLAGS)

clean:
	rm -rf bin

.PHONY: all clean build