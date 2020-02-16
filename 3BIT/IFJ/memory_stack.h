/**
 * Zásobnik
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfirca00 (Anton Firc)
 */

#ifndef _STACK_M_
#define _STACK_M_

typedef struct {
    void **mem_ptr;
    int top;
} Memory_Stack;

void mem_stack_init();
void mem_store(void* ptr_in);
void mem_free();

#endif