/* A4: AI-assisted instructor scaffolding. Mark your own assistance separately. */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "readings.h"

int load_readings(const char *path, struct Reading **items, int *length) {
    /* TODO: open, read the count, allocate, validate records and trailing input,
       close, then transfer ownership. Clean up without committing on failure. */
    (void)path;
    (void)items;
    (void)length;
    return 0;
}

struct Summary summarize_readings(const struct Reading items[], int length) {
    /* TODO: handle the empty case and calculate a fresh summary. */
    (void)items;
    (void)length;
    struct Summary result = {0, 0, 0, 0, 0};
    return result;
}
