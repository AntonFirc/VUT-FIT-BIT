/**
 * Zásobnik
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfirca00 (Anton Firc)
 */

#include "memory_stack.h"
#include <stdio.h>
#include "definitions.h"
#include <stdlib.h>
#include "errors.h"
#include "parser.h"

Memory_Stack mem_stack;

void mem_stack_init() {
    mem_stack.top = -1;
    mem_stack.mem_ptr = malloc(sizeof(void *));
}

void mem_store(void* ptr_in) {
    mem_stack.top++;
    mem_stack.mem_ptr = realloc(mem_stack.mem_ptr, sizeof(void*) * (mem_stack.top + 1));
    mem_stack.mem_ptr[mem_stack.top] = ptr_in;
}

void mem_free() {
    context_stack_dispose(context_stack);
    while (mem_stack.top != -1) {
        free(mem_stack.mem_ptr[mem_stack.top]);
        mem_stack.top--;
    }
    free(mem_stack.mem_ptr);
}