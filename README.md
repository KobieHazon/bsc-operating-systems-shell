# BSc Operating Systems - Shell

- Course: Operating Systems.

My small Unix command shell, using the supplied input parser. It executes foreground commands, a two-command pipeline, and background commands ending in `&`.

## Requirements

macOS or Linux, a C compiler, and Make. On macOS, run `xcode-select --install` if needed; on Debian/Ubuntu, install `build-essential`. Tests also use Python 3 through `uv`.

## Build and run

From the repository root:

```sh
make build
make run
```

The shell waits for input **without displaying a prompt**. Type commands such as:

```text
echo hello
printf hello | tr a-z A-Z
sleep 2 &
echo still-running
```

The first command prints `hello`; the pipeline prints `HELLO` without a trailing newline. `sleep 2 &` runs in the background, so you can enter the next command immediately.

Press **Ctrl-D on an empty line** to exit. Ctrl-C interrupts a foreground command while leaving the shell available. Background commands are not automatically terminated when the shell exits.

You can also run the executable directly or supply commands on standard input:

```sh
./build/myshell
printf 'echo hello\nprintf hello | tr a-z A-Z\n' | ./build/myshell
```

This coursework shell is not Bash: the parser separates words on whitespace and does not implement quoting, variable expansion, redirection, multiple pipes, job control, or built-ins such as `cd` and `exit`. Put spaces around `|` and `&`. Commands run with your normal user permissions; use only commands you intend to execute.

## Tests

```sh
make test
```

The tests execute foreground commands, pipelines (including output larger than a pipe buffer), background commands, invalid commands/syntax, repeated commands, and foreground interruption. `make check` is an alias; `make clean` removes build output.

## Files

- `src/myshell.c`: process creation, pipes, waiting and signal handling.
- `assignment/shell.c`: supplied whitespace-based input parser, unchanged.
- `assignment/hw4.pdf`: supplemental course handout.
- `scripts/test_shell.py`: executable regression tests.
