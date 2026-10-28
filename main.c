/* A4: AI-assisted instructor scaffolding. Mark your own assistance separately. */
#include <stdio.h>
#include <stdlib.h>
#include "readings.h"
#include "report.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s INPUT_PATH OUTPUT_PATH\n", argv[0]);
        return 1;
    }
    /* argv[1] is the input filename; argv[2] is the output filename.
       TODO: call the loader, handle failure, write the report, free the array,
       and return success/failure. Report errors to stderr. */
    fprintf(stderr, "TODO: implement HW4.\n");
    return 1;
}
