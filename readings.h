/* A4: AI-assisted instructor scaffolding. */
#ifndef READINGS_H
#define READINGS_H

#include <stdio.h>

struct Reading {
    int id;
    int temp;
    char unit;
};

/* Preconditions: none. Return nonzero exactly for an in-range ID/temperature
   and unit C. Do not perform I/O or modify the value. */
int valid_reading(struct Reading reading);

/* Preconditions: in is an open readable stream; result is writable.
   Return 1 for a complete valid record, -1 for clean EOF before any field,
   or 0 for invalid/partial input or a read error. Leave *result unchanged on
   failure/end. Borrow in: do not close it. Numeric tokens fit in int. */
int read_reading(FILE *in, struct Reading *result);

/* Preconditions: path is a valid filename; items/length are separate writable
   output objects, with no previous allocation to release. On success transfer
   ownership of an allocated array (NULL if empty) and its length, returning 1.
   On failure return 0 with both outputs unchanged; release temporary resources. */
int load_readings(const char *path, struct Reading **items, int *length);

#endif
