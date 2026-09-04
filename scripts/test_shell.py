#!/usr/bin/env python3
from __future__ import annotations

import subprocess
import sys
from pathlib import Path


def main() -> None:
    if len(sys.argv) != 2:
        raise SystemExit("usage: test_shell.py PATH_TO_SHELL")

    executable = Path(sys.argv[1])
    result = subprocess.run(
        [str(executable)],
        input="/bin/echo portfolio-smoke\n",
        text=True,
        capture_output=True,
        timeout=5,
        check=False,
    )

    if result.returncode != 0:
        raise SystemExit(f"foreground smoke exited {result.returncode}: {result.stderr.strip()}")
    if result.stdout != "portfolio-smoke\n":
        raise SystemExit(f"unexpected foreground stdout: {result.stdout!r}")
    if result.stderr:
        raise SystemExit(f"unexpected foreground stderr: {result.stderr!r}")

    print("validated foreground shell execution")


if __name__ == "__main__":
    main()
