/**
 * Zásobnik string
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfirca00 (Anton Firc)
 */

#include "string_stack.h"
#include <stdio.h>
#include "definitions.h"
#include <stdlib.h>
#include "errors.h"
#include "parser.h"

String_Stack str_stack;

void str_stack_init() {
    str_stack.top = -1;
    str_stack.name = malloc(sizeof(char *));
}

void str_store(char* str_in) {
    str_stack.top++;
    str_stack.name = realloc(str_stack.name, sizeof(char*) * (str_stack.top + 1));
    str_stack.name[str_stack.top] = str_in;
}

char* str_top() {
    return str_stack.name[str_stack.top];
}

char* str_pop() {
    return str_stack.name[str_stack.top--];
}

void str_free() {
    context_stack_dispose(context_stack);
    while (str_stack.top != -1) {
        free(str_stack.name[str_stack.top]);
        str_stack.top--;
    }
    free(str_stack.name);
}