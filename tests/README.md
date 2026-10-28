# Your C test cases

Run all practice tests with:

```sh
make test
```

The Makefile builds the C program and C test driver, then runs eight supplied
integration cases followed by your tests. A failed case makes `make test` fail.
`make clean` also removes the test executable. Python and JSON are not needed.

Each student adds at least **three original cases** inside `run_student_tests`
in `student_tests.c`: three for individual work or six for a pair. Use the
provided `check_program(name, input, expected_report)` function. Prefix each
case name with the author's name/GitHub username and record its coverage in
`CONTRIBUTORS.md`.

For example, calls inside that function can look like:

```c
check_program("alex: negative count", "-1\n", NULL);

check_program("alex: one positive reading", "1\n12 7 C\n",
              "Records: 1\nTotal temperature: 7\nFreezing: 0\n"
              "Minimum: 7\nMaximum: 7\nFreezing readings:\n");
```

These are ordinary C strings: `\n` represents a newline. Adjacent string literals
are joined by the C compiler, so a long expected report may span several source
lines as above. `NULL` as the expected report means input must be rejected with
a nonzero exit status and a diagnostic; no output file may be created or changed.
A successful report must match exactly, including its final newline. `NULL` as
the input tests a missing input file. Do not use the literal text `"NULL"`.

Choose your own reasoning-driven cases rather than copying these examples.
Boundaries, duplicate IDs, empty data, and malformed records are useful. The
harness prepares temporary files, runs `sensor_report`, compares output, reports
PASS/FAIL, and cleans up. It rejects crashes and times out each program run after
10 seconds. The provided POSIX process-management code in `test_runner.c` is
instructor infrastructure; students only need to add calls in `student_tests.c`.
Do not change the supplied public cases to make incorrect behavior pass.

Use `make sanitize` to rebuild and run the same C tests with memory/undefined
behavior checks on a compatible GCC/Clang Linux system. A green practice run is
not the entire grade: private tests, resource ownership, style, and each student's
Git contribution are reviewed separately.

<!-- A4: AI-assisted instructor scaffolding. -->
