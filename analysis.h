/* A4: AI-assisted instructor scaffolding. */
#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "readings.h"

struct Summary {
    int count;
    int total;
    int freezing;
    int minimum;
    int maximum;
};

/* Preconditions: 0 <= length <= 10000; items designates length valid readings,
   or may be NULL when length is zero. No mutation or I/O. Empty result is zero. */
struct Summary summarize_readings(const struct Reading items[], int length);

#endif
