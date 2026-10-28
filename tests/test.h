/* A4: Instructor test support. Students add calls in student_tests.c. */
#ifndef TEST_H
#define TEST_H

/* Run the complete program with input text and compare its report exactly.
   expected_report == NULL means the input must be rejected without changing
   an existing output file or creating a new one. input == NULL tests a missing
   input file. name must describe the case (include your name for student tests).
   The harness handles files, process execution, cleanup, and PASS/FAIL output. */
void check_program(const char *name, const char *input, const char *expected_report);

/* Students implement this function by adding calls to check_program. */
void run_student_tests(void);

#endif
