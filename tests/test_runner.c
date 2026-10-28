/* A4: Instructor C practice-test harness. The POSIX process/temp-directory
   machinery is provided infrastructure, not an API students must implement.
   Run from the project root using make test. */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "test.h"

static int total, failures;
static char input_path[256], output_path[256], stdout_path[256], stderr_path[256];

static int write_file(const char *path, const char *text) {
    FILE *file = fopen(path, "w");
    if (file == NULL) return 0;
    int ok = fputs(text, file) >= 0;
    if (fclose(file) != 0) ok = 0;
    return ok;
}

static int matches(const char *path, const char *text) {
    FILE *file = fopen(path, "r");
    if (file == NULL) return 0;
    int ok = 1;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (fgetc(file) != (unsigned char)text[i]) {
            ok = 0;
            break;
        }
    }
    if (ok && fgetc(file) != EOF) ok = 0;
    if (ferror(file)) ok = 0;
    if (fclose(file) != 0) ok = 0;
    return ok;
}

static int exists(const char *path) {
    return access(path, F_OK) == 0;
}

/* Return -1 for a crash, timeout, or harness error; otherwise the program's
   actual exit code. No shell is used, so filenames do not become commands. */
static int run_program(void) {
    if (fflush(NULL) != 0) return -1;
    pid_t child = fork();
    if (child < 0) return -1;
    if (child == 0) {
        if (freopen(stdout_path, "w", stdout) == NULL ||
            freopen(stderr_path, "w", stderr) == NULL) _exit(126);
        alarm(10);
        execl("./sensor_report", "sensor_report", input_path, output_path, (char *)NULL);
        _exit(127);
    }
    int status;
    pid_t waited;
    do {
        waited = waitpid(child, &status, 0);
    } while (waited < 0 && errno == EINTR);
    if (waited != child || !WIFEXITED(status)) return -1;
    int result = WEXITSTATUS(status);
    return result;
}

static int diagnostic_ok(void) {
    FILE *file = fopen(stderr_path, "r");
    if (file == NULL) return 0;
    char line[1024];
    int any = 0, ok = 1;
    while (fgets(line, sizeof line, file) != NULL) {
        any = 1;
        if (strstr(line, "AddressSanitizer") != NULL ||
            strstr(line, "LeakSanitizer") != NULL ||
            strstr(line, "runtime error:") != NULL) ok = 0;
    }
    if (ferror(file)) ok = 0;
    if (fclose(file) != 0) ok = 0;
    return any && ok;
}

static void fail(const char *name, const char *reason) {
    failures++;
    printf("FAIL: %s (%s)\n", name, reason);
    FILE *file = fopen(stderr_path, "r");
    if (file != NULL) {
        char line[1024];
        while (fgets(line, sizeof line, file) != NULL) fputs(line, stderr);
        fclose(file);
    }
}

void check_program(const char *name, const char *input, const char *expected_report) {
    total++;
    remove(input_path);
    remove(output_path);
    remove(stdout_path);
    remove(stderr_path);
    if (input != NULL && !write_file(input_path, input)) {
        fail(name, "could not prepare input");
        return;
    }
    const char *preserved = "KEEP EXISTING OUTPUT\n";
    if (expected_report == NULL && !write_file(output_path, preserved)) {
        fail(name, "could not prepare output");
        return;
    }
    int status = run_program();
    if (status < 0) {
        fail(name, "program crashed, timed out, or could not run");
        return;
    }
    if (expected_report != NULL) {
        if (status != 0 || !matches(stdout_path, "") || !matches(stderr_path, "") ||
            !matches(output_path, expected_report)) {
            fail(name, "expected success, empty console output, and exact report");
            return;
        }
    } else {
        if (status == 0 || !diagnostic_ok() || !matches(output_path, preserved)) {
            fail(name, "invalid input accepted, missing diagnostic, or output changed");
            return;
        }
        if (remove(output_path) != 0) {
            fail(name, "could not remove output fixture");
            return;
        }
        status = run_program();
        if (status <= 0 || !diagnostic_ok() || exists(output_path)) {
            fail(name, "rejected input crashed or created an output file");
            return;
        }
    }
    printf("PASS: %s\n", name);
}

static void run_public_tests(void) {
    check_program("example", "5\n101 -5 C\n205 0 C\n101 8 C\n310 -2 C\n450 4 C\n",
                  "Records: 5\nTotal temperature: 5\nFreezing: 2\nMinimum: -5\n"
                  "Maximum: 8\nFreezing readings:\n101 -5 C\n310 -2 C\n");
    check_program("empty collection", "0",
                  "Records: 0\nTotal temperature: 0\nFreezing: 0\nMinimum: N/A\n"
                  "Maximum: N/A\nFreezing readings:\n");
    check_program("all negative, no final newline", "2\n1 -100 C\n2 -1 C",
                  "Records: 2\nTotal temperature: -101\nFreezing: 2\nMinimum: -100\n"
                  "Maximum: -1\nFreezing readings:\n1 -100 C\n2 -1 C\n");
    check_program("zero is not freezing", "1\n1 0 C\n",
                  "Records: 1\nTotal temperature: 0\nFreezing: 0\nMinimum: 0\n"
                  "Maximum: 0\nFreezing readings:\n");
    check_program("partial record", "1\n1 -5\n", NULL);
    check_program("extra record", "0\n1 1 C\n", NULL);
    check_program("invalid unit", "1\n1 5 F\n", NULL);
    check_program("missing file", NULL, NULL);
}

int main(void) {
    char directory[] = "/tmp/cs201-hw4-XXXXXX";
    if (mkdtemp(directory) == NULL) {
        perror("Could not create test directory");
        return 1;
    }
    snprintf(input_path, sizeof input_path, "%s/input data.txt", directory);
    snprintf(output_path, sizeof output_path, "%s/report.txt", directory);
    snprintf(stdout_path, sizeof stdout_path, "%s/stdout.txt", directory);
    snprintf(stderr_path, sizeof stderr_path, "%s/stderr.txt", directory);
    run_public_tests();
    int public_count = total;
    run_student_tests();
    printf("%d/%d cases passed; %d student cases (at least 3 per student required).\n",
           total - failures, total, total - public_count);
    remove(input_path);
    remove(output_path);
    remove(stdout_path);
    remove(stderr_path);
    if (rmdir(directory) != 0) {
        perror("Could not remove test directory");
        return 1;
    }
    return failures != 0;
}
