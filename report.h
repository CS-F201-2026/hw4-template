/* A4: AI-assisted instructor scaffolding. */
#ifndef REPORT_H
#define REPORT_H

#include "readings.h"

/* Preconditions: path is a valid filename; 0 <= length <= 10000; items designates
   length valid readings, or may be NULL when empty. Open/write/close the report.
   Return 1 on full success, otherwise 0. Do not modify or free caller's array. */
int write_report(const char *path, const struct Reading items[], int length);

#endif
