/**
 * Zásobnik string
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfirca00 (Anton Firc)
 */

#ifndef _STACK_S_
#define _STACK_S_

typedef struct {
    char **name;
    int top;
} String_Stack;

void str_stack_init();
void str_store(char* str_in);
char* str_top();
char* str_pop();
void str_free();

#endif