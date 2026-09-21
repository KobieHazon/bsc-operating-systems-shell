"""Behavioral tests for the supplied parser and the shell process implementation."""

import os
import select
import signal
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

PROGRAM = Path(sys.argv.pop(1)).resolve()


class ShellTests(unittest.TestCase):
    def run_commands(self, commands):
        return subprocess.run(
            [str(PROGRAM)], input=commands, text=True, capture_output=True, timeout=20
        )

    def assert_success(self, result, output):
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stdout, output)
        self.assertEqual(result.stderr, "")

    def test_foreground_and_blank_lines(self):
        self.assert_success(
            self.run_commands("\necho hello\necho world\n"), "hello\nworld\n"
        )

    def test_pipeline(self):
        self.assert_success(
            self.run_commands("printf hello | tr a-z A-Z\necho done\n"), "HELLOdone\n"
        )

    def test_pipeline_larger_than_pipe_buffer(self):
        with tempfile.TemporaryDirectory(prefix="shell-input-") as directory:
            path = Path(directory) / "input.txt"
            path.write_bytes(b"x" * 300000)
            result = self.run_commands(f"cat {path} | wc -c\n")
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stdout.strip(), "300000")
            self.assertEqual(result.stderr, "")

    def test_repeated_commands_do_not_leak_descriptors(self):
        self.assert_success(self.run_commands("echo ok\n" * 300), "ok\n" * 300)

    def test_invalid_command_does_not_end_session(self):
        result = self.run_commands("missing-course-command-12345\necho alive\n")
        self.assertEqual(result.returncode, 0)
        self.assertEqual(result.stdout, "alive\n")
        self.assertIn("missing-course-command-12345", result.stderr)

    def test_invalid_syntax_does_not_end_session(self):
        for command in ("&", "| echo x", "echo x |", "echo x | cat | cat", "echo & x"):
            with self.subTest(command=command):
                result = self.run_commands(command + "\necho alive\n")
                self.assertEqual(result.returncode, 0)
                self.assertEqual(result.stdout, "alive\n")
                self.assertIn("Syntax:", result.stderr)

    def interactive_process(self):
        process = subprocess.Popen(
            [str(PROGRAM)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            start_new_session=True,
        )

        def cleanup():
            try:
                os.killpg(process.pid, signal.SIGKILL)
            except ProcessLookupError:
                pass
            process.communicate(timeout=5)

        self.addCleanup(cleanup)
        return process

    def test_background_command_does_not_block_input(self):
        process = self.interactive_process()
        process.stdin.write("sleep 2 &\necho available\n")
        process.stdin.flush()
        self.assertTrue(select.select([process.stdout], [], [], 1)[0])
        self.assertEqual(process.stdout.readline(), "available\n")
        output, errors = process.communicate(timeout=5)
        self.assertEqual((process.returncode, output, errors), (0, "", ""))

    def test_interrupt_foreground_keeps_shell_alive(self):
        process = self.interactive_process()
        # Allow the foreground pipeline to start, then interrupt its process group.
        process.stdin.write("echo ready | sleep 10\n")
        process.stdin.flush()
        import time

        time.sleep(0.2)
        os.killpg(process.pid, signal.SIGINT)
        process.stdin.write("echo alive\n")
        process.stdin.flush()
        output, errors = process.communicate(timeout=5)
        self.assertEqual((process.returncode, output, errors), (0, "alive\n", ""))


if __name__ == "__main__":
    unittest.main()
