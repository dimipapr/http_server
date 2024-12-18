CFLAGS=-Wall -Wextra -std=c99 -g -Og -DDEBUG

all:run

run:build
	./bin/server

build:bin/server

bin/server:src/server.c
	mkdir -p $(dir $@)
	gcc -o $@ $< $(CFLAGS)

clean:
	rm -rf build bin

.PHONY: debug clean build run all