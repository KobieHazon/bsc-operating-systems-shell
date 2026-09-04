check:
	python3 scripts/check_repository.py
	mkdir -p build
	cc -std=gnu11 -Wall -Wextra -Wno-unused-parameter -o build/myshell assignment/shell.c src/myshell.c
	python3 scripts/test_shell.py build/myshell

clean:
	rm -rf build
