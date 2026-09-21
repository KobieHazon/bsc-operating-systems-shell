# BSc Operating Systems - Shell

- Course: Operating Systems.
- My implementation is kept separately from supplied exercise files.

## Contents

Unix shell coursework implementing foreground/background execution, pipes, and signal handling against a supplied shell parser/driver.

## Files

Exercise/framework material:

- `assignment/shell.c`
- `assignment/hw4.pdf`

Implementation material:

- `src/myshell.c`

Tests:

- `scripts/test_shell.py` checks that foreground commands finish successfully.

## Tech Stack

- C.
- POSIX APIs where applicable.

## Validate

```bash
make check
```

This compiles the shell and verifies that a foreground command completes without an internal wait error.

## Notes

The 2016a shell handout is used as supplemental assignment evidence, and the submitted archive supplies the parser/driver split.
