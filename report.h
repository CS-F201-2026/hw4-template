/* A4: AI-assisted instructor scaffolding. */
#ifndef REPORT_H
#define REPORT_H

#include "analysis.h"

/* Preconditions: out is an open writable stream; summary is a valid result of
   summarize_readings. Write the six report header lines, including
   "Freezing readings:". Return 1 on success, 0 on any write failure.
   Borrow out: do not close it. */
int write_summary(FILE *out, struct Summary summary);

/* Preconditions: out is open/writable; items designates length valid records,
   0 <= length <= 10000, or may be NULL when empty. Write only records below
   zero, in input order. Return 1 on success, 0 on any write failure.
   Do not close out or modify/free the records. No headers here. */
int write_freezing(FILE *out, const struct Reading items[], int length);

/* Preconditions: path is a valid filename; 0 <= length <= 10000; items designates
   length valid readings, or may be NULL when empty. Open/write/close the report.
   Return 1 on full success, otherwise 0. Do not modify or free caller's array. */
int write_report(const char *path, const struct Reading items[], int length);

#endif
