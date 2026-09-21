CC ?= cc
CFLAGS ?= -std=gnu11 -Wall -Wextra -Wno-unused-parameter

.PHONY: all build run test check clean
all: build
build: build/myshell

build/myshell: assignment/shell.c src/myshell.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS)  -o $@ assignment/shell.c src/myshell.c

run: build
	./build/myshell

test: build
	uv run --no-project python scripts/test_shell.py build/myshell

check: test

clean:
	rm -rf build
