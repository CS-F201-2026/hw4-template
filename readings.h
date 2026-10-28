/* A4: AI-assisted instructor scaffolding. */
#ifndef READINGS_H
#define READINGS_H

struct Reading {
    int id;
    int temp;
    char unit;
};

struct Summary {
    int count;
    int total;
    int freezing;
    int minimum;
    int maximum;
};

/* Preconditions: path is a valid filename; items/length are separate writable
   output objects, with no previous allocation to release. On success transfer
   ownership of an allocated array (NULL if empty) and its length, returning 1.
   On failure return 0 with both outputs unchanged; release temporary resources. */
int load_readings(const char *path, struct Reading **items, int *length);

/* Preconditions: 0 <= length <= 10000; items designates length valid readings,
   or may be NULL when length is zero. No mutation or I/O. Empty result is zero. */
struct Summary summarize_readings(const struct Reading items[], int length);

#endif
