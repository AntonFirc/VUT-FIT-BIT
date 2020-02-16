/**
 * Výpis chybových hlášek
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 */

#include <stdio.h>
#include <stdarg.h>

#include "definitions.h"
#include "memory_stack.h"
#include <stdlib.h>



// error_lexical("There was an lelxical error. We were expecting '%s', given '%s'.", line_number, expected, given);
void error_lexical(char *message, int line, ...) {
    va_list va_args;
    va_start(va_args, line);
    vfprintf(stderr, message, va_args);
    fprintf (stderr, " Error is on line %d\n", line);
    va_end(va_args);
    //mem_free();
    exit(ERROR_LEXICAL);
}

void error_syntactic(char *message, int line, ...) {
    va_list va_args;
    va_start(va_args, line);
    vfprintf(stderr, message, va_args);
    fprintf (stderr, " Error is on line %d\n", line);
    //mem_free();
    exit(ERROR_SYNTACTIC);
}


void error_semantic(char *message, int line, ...) {
    va_list va_args;
    va_start(va_args, line);
    vfprintf(stderr, message, va_args);
    fprintf (stderr, " Error is on line %d\n", line);
    va_end(va_args);
    //mem_free();
    exit(ERROR_SEMANTIC);
}

void error_incompatible_type(char *message, int line, ...) {
    va_list va_args;
    va_start(va_args, line);
    vfprintf(stderr, message, va_args);
    fprintf (stderr, " Error is on line %d\n", line);
    va_end(va_args);
    //mem_free();
    exit(ERROR_SEMANTIC_TYPE_COMPATIBILITY);
}

void error_semantic_other(char *message, int line, ...) {
    va_list va_args;
    va_start(va_args, line);
    vfprintf(stderr, message, va_args);
    fprintf (stderr, " Error is on line %d\n", line);
    va_end(va_args);
    //mem_free();
    exit(ERROR_SEMANTIC_OTHER);
}

void error_internal(char *message, int line, ...) {
    va_list va_args;
    va_start(va_args, line);
    vfprintf(stderr, message, va_args);
    fprintf (stderr, " Error is on line %d\n", line);
    va_end(va_args);
    //mem_free();
    exit(ERROR_INTERNAL);
}

