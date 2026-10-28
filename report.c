/* A4: AI-assisted instructor scaffolding. Mark your own assistance separately. */
#include <stdio.h>
#include "report.h"

int write_summary(FILE *out, struct Summary summary) {
    /* TODO: write the six headers, with N/A extrema when count is zero. */
    (void)out;
    (void)summary;
    return 0;
}

int write_freezing(FILE *out, const struct Reading items[], int length) {
    /* TODO: write only the negative-temperature records; preserve input order. */
    (void)out;
    (void)items;
    (void)length;
    return 0;
}

int write_report(const char *path, const struct Reading items[], int length) {
    /* TODO: open, call summarize_readings, write_summary and write_freezing,
       and close.
       Check every output operation, including fclose. */
    (void)path;
    (void)items;
    (void)length;
    return 0;
}
