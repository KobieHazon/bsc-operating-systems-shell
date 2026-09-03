check:
	python3 scripts/check_repository.py
	mkdir -p build
	cc -std=gnu11 -Wall -Wextra -Wno-unused-parameter -o build/myshell assignment/shell.c src/myshell.c

clean:
	rm -rf build
