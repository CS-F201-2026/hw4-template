# HW4 — Sensor Log Report

**Scheduled release: Wednesday, November 4, 2026. Due Thursday, November 19, 2026.**

Work **individually or in a pair (two students maximum)**. Build a program that loads sensor readings from a real file,
stores them in a dynamically allocated array, and writes a summary report to a
second file. This assignment runs on your own computer/compiler, rather than
NetRun. It practices file I/O, structured data, pointers and ownership, dynamic
arrays, header/source files, Make, and Git. Sorting and recursion are not required.

## Get started

Create your own repository; the instructor does not create repositories or form
pairs for you. For a pair, choose one owner and create **one shared repository**.
Individual students follow exactly the same setup without adding a partner.

1. Open [hw4-template](https://github.com/CS-F201-2026/hw4-template).
2. Click **Use this template → Create a new repository**. Choose your **personal
   GitHub account** as owner, name it `cs201-hw4`, and select **Private**.
   Use the default branch only. Do not fork the template or create the solution
   repository in the course organization.
3. In your new repository, open **Settings → Collaborators → Add people**.
   Invite **`acmattson3`** so the instructor can grade your private work. For a
   pair, also invite your partner's GitHub account; the partner must accept.
   Check that the instructor can access the repo before the deadline.
4. Each student clones this same repository on their own computer. Replace OWNER
   below with the GitHub username of the repository's owner:

```sh
git clone https://github.com/OWNER/cs201-hw4.git
cd cs201-hw4
make
./sensor_report examples/readings.txt report.txt
make test
```

[GitHub template-copy instructions](https://docs.github.com/en/repositories/creating-and-managing-repositories/creating-a-repository-from-a-template)
and [collaborator instructions](https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/repository-access-and-collaboration/inviting-collaborators-to-a-personal-repository).

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
helpers. Shared types/declarations belong in `readings.h`, `analysis.h`, and
`report.h`, which have include guards. Keep the source responsibilities below.
Compile `.c` files separately and link them; never include a `.c`
file. Do not replace the dynamic array with a fixed-size array, variable-length
array, global buffer, or C++ container. Use `malloc`/`free` (or `calloc`/`free`).

| Source file | Required functions | Responsibility |
| --- | --- | --- |
| `validation.c` | `valid_reading`, `read_reading` | Field validation and reading one record |
| `readings.c` | `load_readings` | Named input file, dynamic allocation, cleanup, ownership transfer |
| `analysis.c` | `summarize_readings` | Pure summary calculations |
| `report.c` | `write_summary`, `write_freezing`, `write_report` | Stream output helpers and named report-file lifecycle |
| `main.c` | `main` | Connect the modules and release the loaded array |

### Part 0: Load and validate (15 points)

In `validation.c`, implement:

- `int valid_reading(struct Reading reading)`: return nonzero exactly when the
  ID, temperature, and unit satisfy the input rules. No I/O or mutation.
- `int read_reading(FILE *in, struct Reading *result)`: use `fscanf` to read one
  candidate and call `valid_reading`. Return **1** for a valid complete record,
  **-1** for clean EOF before any fields, or **0** for invalid/partial input or
  a read error. Set `*result` only on success; preserve it otherwise. Borrow the
  stream: do not close it. Preconditions: `in` is open/readable and `result`
  designates a writable record; numeric tokens fit in `int`.

In `readings.c`, implement `int load_readings(const char *path, struct Reading **items, int *length)`.
Open the named input file, read and validate its count and every record, and check
for unexpected trailing data. Call `read_reading` for each of the `N` records;
do not duplicate its parsing/validation. On complete success, return 1 and set both outputs.
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

### Part 2: Analyze (10 points)

In `analysis.c`, implement `struct Summary summarize_readings(const struct Reading items[], int length)`.
Return the count, temperature sum, number of readings **strictly below zero**,
minimum temperature, and maximum temperature. Zero is not freezing. For an empty
collection return five zeros; the report prints `N/A` for its extrema. Each call
starts fresh. Do not perform I/O or change the records. Negative-only and
positive-only collections must have correct extrema.

Preconditions: `0 <= length <= 10000`; for positive length, `items` designates
that many valid readings. For zero length it may be `NULL`. Sums fit in `int`.

### Part 3: Write the report (15 points)

In `report.c`, implement these three cooperating functions:

- `int write_summary(FILE *out, struct Summary summary)` writes the **six header
  lines** below, including `Freezing readings:`. Use `summary.count` to choose
  numeric extrema or `N/A`. It writes no record rows.
- `int write_freezing(FILE *out, const struct Reading items[], int length)`
  writes only the freezing record rows in input order, with no headers.
- `int write_report(const char *path, const struct Reading items[], int length)`
  opens the named output, calls `summarize_readings`, then `write_summary` and
  `write_freezing`, and closes the file. Call the helpers instead of duplicating
  their formatting/calculations.

The two stream helpers borrow `out`: it is already open/writable and must remain
open afterward, even on failure. They return 1 if every write succeeds or 0 on
a write failure; they do not open/close files or modify/free records. For
`write_summary`, the summary is a valid result of `summarize_readings`. For
`write_freezing`, the record-range preconditions are the same as for summary.

In `write_report`, open/create the output file with mode `"w"` and write
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

### Part 5: Build and student tests (10 points)

- **5 points:** preserve the multi-file layout and make `make`, `make clean`, and
  `make test` work from a fresh clone. The supplied Makefile is a starting point;
  update dependencies if you add files. Do not commit object files/executables.
- **5 points:** add at least **three original test cases per student** in
  `tests/student_cases.json`, using the documented format. This means at least
  three cases for individual work or six for a pair. Identify each case's author
  in its name, such as `alex: upper temperature boundary`, and explain coverage in
  `CONTRIBUTORS.md`. A test may expose a bug while you work; final cases should
  pass. Grading checks useful coverage, not just the number of cases.

### Part 6: Individual Git contributions (10 points per student)

**Every student must author and push their own substantive commits**, including
when working in a pair. The other 90 points assess the submitted project; these
10 points are assessed separately for each student:

- **5 points:** at least one personally authored commit implementing or
  substantially correcting required C functionality. Each partner must implement
  at least one required function, rather than leaving all programming to one person.
- **5 points:** at least one **additional**, personally authored substantive
  code or test commit, with a descriptive message. This must be a different commit
  from the one used for the first five points.

**No qualifying commits means 0/10 for this category**, even if your partner's
program is correct. A student with only one qualifying implementation commit can
earn 5/10. Creating the repo, importing the template, merging without substantive
authored changes, changing whitespace, or adding only your name does not count.
Do not split a trivial edit into artificial commits. There is no line-count or
equal-commit-count requirement; the instructor reviews what changed.

Use your own name and a GitHub-associated email for your commits. Do not make
your partner's work appear under your name. Co-author credit can document joint
work, but does not replace each student's own required commits. Complete
`CONTRIBUTORS.md` with names, GitHub usernames, responsibilities, test coverage,
and representative commit hashes. Preserve both authors' history; do not squash
all work into a single final commit.

### Planning work as a pair

The interfaces let you work on separate files without waiting for the whole
program. A suggested division is:

| Student | Initial responsibilities | Follow-up work |
| --- | --- | --- |
| A | `validation.c` and `readings.c`: validation, parsing, dynamic loading | Input/failure tests, resource cleanup, review integration |
| B | `analysis.c` and `report.c`: summary and report helpers | Summary/output tests, `main.c` integration, review ownership |

This is a suggestion, not a required assignment of roles. Agree on responsibilities
before coding and record them in `CONTRIBUTORS.md`. Keep the supplied interfaces
so work can be combined easily. Both students must understand the whole program,
review each other's work, and run the combined tests. Individuals implement all
parts. The required program and correctness/style standards are the same either way.

Use Git as you work; there is no required branch/PR workflow for this first Git
homework. For example, after finishing the loader:

```sh
git status
git add readings.c readings.h tests/student_cases.json
git commit -m "Load and validate dynamically allocated readings"
git push
```

For pairs using the same main branch, start each work session with
`git pull --ff-only` while your working tree is clean. Commit your changes, pull
your partner's latest work, and push. If Git reports diverging histories or a
conflict, stop and resolve it together (or ask for help); do not force-push over
your partner's commits. Each student must push their commits before submission.

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
git add main.c validation.c readings.c readings.h analysis.c analysis.h report.c report.h Makefile tests/student_cases.json CONTRIBUTORS.md
git commit -m "Finish report generation and boundary tests"
git push
git rev-parse HEAD
```

**Each student submits separately in Canvas**, including both members of a pair.
Submit the private repository URL, the full final commit hash printed above,
and your name/GitHub username plus your partner's name/GitHub username (if any).
Both partners submit the **same repository and final hash**. Ensure all commits
are pushed, `CONTRIBUTORS.md` is complete, and `acmattson3` has accepted access.
Only pushed work is available for grading. Canvas remains the official
gradebook. Follow the syllabus late-work rules; no additional late penalty is
introduced here. Do not publish your solution or open a PR against the template.

You may collaborate fully with your one declared partner. Outside that pair,
high-level discussion is allowed, but do not exchange implementations. Individual
submissions must represent your own work; pair submissions must represent the
declared pair's work. Follow the syllabus A0–A5 AI disclosure policy: mark AI-assisted
portions with the appropriate level and explain the assistance in a comment or
`AI_USAGE.md`. Unassisted human work needs no marking. The supplied instructor
scaffolding is marked A4; it is not a disclosure of your own implementation.

<!-- A4: Instructor assignment text and scaffolding generated with AI assistance;
     subject to instructor review before scheduled release. -->
