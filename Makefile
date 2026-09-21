.PHONY: check clean

check:
	mkdir -p build
	cc -std=gnu11 -Wall -Wextra -Wno-unused-parameter -o build/myshell assignment/shell.c src/myshell.c
	uv run --no-project python scripts/test_shell.py build/myshell

clean:
	rm -rf build
