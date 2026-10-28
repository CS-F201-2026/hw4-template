# HW4 — Sensor Log Report

**Scheduled release: Wednesday, November 4, 2026. Due Thursday, November 19, 2026.**

Work individually. Build a program that loads sensor readings from a real file,
stores them in a dynamically allocated array, and writes a summary report to a
second file. This assignment runs on your own computer/compiler, rather than
NetRun. It practices file I/O, structured data, pointers and ownership, dynamic
arrays, header/source files, Make, and Git. Sorting and recursion are not required.

## Get started

Your instructor will give you access to your own private repository. Accept the
repository invitation, then clone **your assigned repository**, replacing USERNAME:

```sh
git clone https://github.com/CS-F201-2026/hw4-USERNAME.git
cd hw4-USERNAME
make
./sensor_report examples/readings.txt report.txt
make test
```

Use a C11 compiler (`gcc` or compatible), GNU Make, Git, and Python 3 (for tests).
The Makefile supplies `-std=c11 -Wall -Wextra -Wpedantic -g`. On Debian/Ubuntu,
the packages are `build-essential git python3`. Other systems may use an
equivalent compiler/Make installation. Ask for help before setup blocks progress.
The command-line argument handling in `main.c` is provided; you do not need to
design a command-line parser. Initially the helpers are TODO stubs and tests fail.

Before your first commit, configure Git's author identity using your own name and
your GitHub-verified email (or your GitHub-provided private noreply address):

```sh
git config --global user.name "First Last"
git config --global user.email "YOUR_GITHUB_EMAIL"
```

For HTTPS clone/push, use GitHub CLI or a Git credential manager to sign in;
your ordinary GitHub account password does not authenticate Git operations.
See [GitHub's authentication instructions](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/about-authentication-to-github#authenticating-with-the-command-line).
Never paste credentials into source files, commits, test cases, or Canvas.

## Input format

The first integer is the number of records, `N`, with `0 <= N <= 10000`. It is
followed by exactly `N` records, each containing three whitespace-separated fields:

```text
id temperature unit
```

IDs are integers from 1 through 1000000. Temperatures are integers from -100
through 100 inclusive. The unit must be the single character `C`. IDs may repeat:
each record is an observation, not a unique inventory entry. Whitespace separates
fields; physical lines are not significant. Blank lines and trailing whitespace
are allowed. A final newline is optional. An empty file is invalid; a file
containing `0` is a valid empty collection. After all records, any non-whitespace
character makes the input invalid. Do not skip malformed records or silently
ignore extra data. Numeric tokens supplied in grading fit in `int`; you do not
need to implement an overflow-safe text-to-integer parser. Comments are not part
of the input format.
Use the `fscanf` conversion behavior taught in class; enforcing additional
separator rules beyond those conversions is not required.

`examples/readings.txt` contains:

```text
5
101 -5 C
205 0 C
101 8 C
310 -2 C
450 4 C
```

## Required interfaces and ownership

Keep the supplied public structures and function signatures. You may add private
helpers. Put declarations and shared types in `readings.h` and `report.h` with
include guards; implement reading/analysis in `readings.c` and writing in
`report.c`. Compile `.c` files separately and link them; never include a `.c`
file. Do not replace the dynamic array with a fixed-size array, variable-length
array, global buffer, or C++ container. Use `malloc`/`free` (or `calloc`/`free`).

### Part 0: Load and validate (20 points)

Implement `int load_readings(const char *path, struct Reading **items, int *length)`.
Open the named input file, read and validate its count and every record, and check
for unexpected trailing data. On complete success, return 1 and set both outputs.
For zero records, set `*items = NULL` and `*length = 0`. On any failure, return 0
and leave **both caller outputs unchanged**. A failed load must not leave a partial
collection for the caller. Handle missing files, malformed/partial input, invalid
fields, premature EOF, excess data, read errors, and input-close errors. Check
`ferror` when distinguishing clean EOF from an input error.

Preconditions: `path` points to a valid, null-terminated filename; `items` and
`length` designate separate writable objects. The caller has no allocation that
this function needs to release. The grader may put sentinel values in the output
objects to check that failure preserves them; never dereference or free the old
`*items`. Use local temporary variables and commit outputs only after success.

### Part 1: Memory and resource ownership (15 points)

For positive `N`, allocate space for `N` readings on the heap. Check allocation
failure. Do not dereference a failed allocation, access outside `[0, N)`, leak an
allocation, free twice, or use data after freeing it. The loader owns its open
file and temporary allocation until success: close/free on every applicable
failure path. On success it transfers the array to the caller, which must free it
exactly once. `free(NULL)` is permitted for the empty collection. Each function
that opens a file must close it exactly once and check the close result. You may
add helpers, but do not change this ownership contract.

### Part 2: Analyze (15 points)

Implement `struct Summary summarize_readings(const struct Reading items[], int length)`.
Return the count, temperature sum, number of readings **strictly below zero**,
minimum temperature, and maximum temperature. Zero is not freezing. For an empty
collection return five zeros; the report prints `N/A` for its extrema. Each call
starts fresh. Do not perform I/O or change the records. Negative-only and
positive-only collections must have correct extrema.

Preconditions: `0 <= length <= 10000`; for positive length, `items` designates
that many valid readings. For zero length it may be `NULL`. Sums fit in `int`.

### Part 3: Write the report (15 points)

Implement `int write_report(const char *path, const struct Reading items[], int length)`.
Open/create the output file with mode `"w"`, call `summarize_readings`, and write
exactly the format below, including a final newline. After the six header lines,
write just the freezing records, in their original order, each as `id temp C`.
For no freezing records, the file ends immediately after `Freezing readings:`.
Return 1 only if opening, every write, and closing succeed; otherwise return 0.
Always close a file you successfully opened, including after a write failure.
Do not modify the records. An output error may leave a partial output file;
atomic replacement is not required. The range preconditions are the same as for
summary, and `path` is a valid filename.

For the example input, the entire report is:

```text
Records: 5
Total temperature: 5
Freezing: 2
Minimum: -5
Maximum: 8
Freezing readings:
101 -5 C
310 -2 C
```

For input `0`:

```text
Records: 0
Total temperature: 0
Freezing: 0
Minimum: N/A
Maximum: N/A
Freezing readings:
```

### Part 4: Integrate the program (5 points)

Keep the supplied two-path command interface:

```sh
./sensor_report INPUT_PATH OUTPUT_PATH
```

Complete the TODO in `main.c`: load, write, and release the array. Return 0 on
success and nonzero on failure. On failure print a useful diagnostic to stderr;
diagnostic wording is not graded. On success stdout/stderr should be empty (the
report goes in the output file). If loading fails, do not create or truncate the
output file. The input and output paths must refer to different files; tests
respect that precondition, so detecting aliases is not required. No menus or
interactive input are needed. The provided usage check already rejects the wrong
number of arguments.

### Part 5: Build and Git workflow (10 points)

- **5 points:** preserve the multi-file layout and make `make`, `make clean`, and
  `make test` work from a fresh clone. The supplied Makefile is a starting point;
  update dependencies if you add files. Do not commit object files/executables.
- **5 points:** develop in your assigned repository, commit at least two
  meaningful stages of your own implementation with descriptive messages, and
  push your final source, headers, Makefile, and tests. The initial template
  commit does not count as an implementation stage. Add at least **three** of
  your own test cases in `tests/student_cases.json`, using the documented format.
  A test may expose a bug while you work; the final cases should pass. A final
  submission needs the repository URL and full commit hash in Canvas.

Use Git as you work; there is no required branch/PR workflow for this first Git
homework. For example, after finishing the loader:

```sh
git status
git add readings.c readings.h tests/student_cases.json
git commit -m "Load and validate dynamically allocated readings"
git push
```

### Style (20 points)

Use meaningful names (4), consistent indentation/braces/spacing (4), focused
functions and a clear separation of I/O and computation (6), and accurate helper
contracts plus useful ownership/decision comments (6). Keep mutable state local.
No Preconditions comment is required for `main`. The headers already provide
contracts for the required helpers; keep them accurate if you document added
behavior. Document any additional functionality. Correct, documented extensions
are welcome, provided required behavior remains supported.

## Test and submit

`make test` runs public integration examples and your JSON cases. Each JSON entry
has a `name`, an `input` string, and either an exact `report` string for success or
`"valid": false` for a rejected input. See `tests/README.md`. Run
`make sanitize` for the same cases with AddressSanitizer and UndefinedBehaviorSanitizer
on a compatible GCC/Clang Linux system. A green practice check is **not** the
entire grade: grading also checks helpers, error paths, memory, your workflow,
and style. Do not edit the public tests to make incorrect behavior pass.

Before the deadline:

```sh
make clean
make test
git status
git add main.c readings.c readings.h report.c report.h Makefile tests/student_cases.json
git commit -m "Finish report generation and boundary tests"
git push
git rev-parse HEAD
```

Submit your private repository URL and the full hash printed by the last command
in Canvas. Only pushed work is available for grading. Canvas remains the official
gradebook. Follow the syllabus late-work rules; no additional late penalty is
introduced here. Do not publish your solution or open a PR against the template.

High-level discussion is allowed, but the implementation must represent your
individual work. Follow the syllabus A0–A5 AI disclosure policy: mark AI-assisted
portions with the appropriate level and explain the assistance in a comment or
`AI_USAGE.md`. Unassisted human work needs no marking. The supplied instructor
scaffolding is marked A4; it is not a disclosure of your own implementation.

<!-- A4: Instructor assignment text and scaffolding generated with AI assistance;
     subject to instructor review before scheduled release. -->
