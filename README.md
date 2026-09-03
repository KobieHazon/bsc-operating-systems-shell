# BSc Operating Systems - Shell

A historical archive of my CS BSc coursework.

## Contents

Unix shell coursework implementing foreground/background execution, pipes, and signal handling against a supplied shell parser/driver.

## Provenance

- Era: CS BSc.
- Last recovered work: 2019-2020 archive copy.
- Supplied exercise material is identified separately below.
- My implementation is kept separately from supplied exercise files.
- Submitted ZIP wrappers and Apple resource forks were omitted.

## Files

Exercise/framework material:

- `assignment/shell.c`
- `assignment/hw4.pdf`

Implementation material:

- `src/myshell.c`

## Tech Stack

- C.
- POSIX APIs where applicable.
- `pthread` for the parallel-find assignment.
- Linux kernel-module APIs for the message-slot assignment.

## Validate

```bash
make check
```

## Notes

The recovered 2016a shell handout is used as supplemental assignment evidence, and the submitted archive supplies the parser/driver split.
