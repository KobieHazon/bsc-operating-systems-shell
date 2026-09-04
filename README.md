# BSc Operating Systems - Shell

A historical archive of my CS BSc coursework.

## Contents

Unix shell coursework implementing foreground/background execution, pipes, and signal handling against a supplied shell parser/driver.

## Provenance

- Era: CS BSc.
- Last recovered work: 2019-2020 archive copy.
- Supplied exercise material is identified separately below.
- My implementation is kept separately from supplied exercise files.
- The historical implementation remains recoverable from that solution commit. The maintained implementation adds only minimal host-compatibility fixes: the required signal declarations and acceptance of an already-reaped foreground child when the `SIGCHLD` handler wins the wait race.
- Submitted ZIP wrappers and Apple resource forks were omitted.

## Files

Exercise/framework material:

- `assignment/shell.c`
- `assignment/hw4.pdf`

Implementation material:

- `src/myshell.c`

Validation tools:

- `scripts/check_repository.py` checks source presence and privacy hygiene.
- `scripts/test_shell.py` runs a deterministic foreground-command regression smoke; it is validation infrastructure, not part of the historical submission.

## Tech Stack

- C.
- POSIX APIs where applicable.
- `pthread` for the parallel-find assignment.
- Linux kernel-module APIs for the message-slot assignment.

## Validate

```bash
make check
```

This performs the static/privacy check, compiles the recovered shell, and verifies that a foreground command completes without an internal wait error.

## Notes

The recovered 2016a shell handout is used as supplemental assignment evidence, and the submitted archive supplies the parser/driver split.
