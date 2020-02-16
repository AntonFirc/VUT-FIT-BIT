/**
 * Zásobnik pre precedencnu analyzu
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack_token.h"
#include "parser.h"
#include "memory_stack.h"

void token_stack_init(Token_Stack *stack) {
    stack->top = -1;
    stack->token = malloc(sizeof(Token *));
    mem_store(stack->token);
}

void token_push(Token_Stack *stack, Token *token) {
    stack->top++;
    stack->token = realloc(stack->token, sizeof(Token *) * (stack->top + 1));
    stack->token[stack->top] = token;
}

Token *token_top(Token_Stack *stack) {
    return stack->token[stack->top];
}

Token *token_pop(Token_Stack *stack) {
    if (stack->top == -1) {
        return NULL;
    }
    
    Token *token = stack->token[stack->top];
    stack->token[stack->top--] = NULL;
    return token;
}

int token_is_empty(Token_Stack *stack) {
    return stack->top == -1;
}
