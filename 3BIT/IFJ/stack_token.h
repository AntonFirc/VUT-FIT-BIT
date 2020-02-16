/**
 * Zásobnik pre precedencnu analyzu
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 */

#ifndef _STACK_TOKEN_H_
#define _STACK_TOKEN_H_

#include "tokens.h"

typedef struct {
    Token **token;
    int top;
} Token_Stack;

void token_stack_init(Token_Stack *stack);
void token_push(Token_Stack *stack, Token *token);
Token *token_top(Token_Stack *stack);
Token *token_pop(Token_Stack *stack);
int token_is_empty(Token_Stack *stack);

#endif

