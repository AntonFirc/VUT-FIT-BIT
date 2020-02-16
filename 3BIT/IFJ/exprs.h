/**
 * Precedencna syntakticka analyza
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 **/

#ifndef _EXPRS_H
#define _EXPRS_H
#include "stack_token.h"

#define SIZE 15

typedef enum {
    state_begin,
    state_end,
    state_push,
    state_bracket_right,
    state_bracket_left,
    state_operand,
    state_num_operator,
    state_cmp_operator,
} State_expr;

Token * find_terminal(Token_Stack *stack);
int token_type_2_row(Token *token);
char find_in_table(char table[SIZE][SIZE], int row, int col);
void reduce_expr(Token_Stack *stack);
int expr_eval(Token *tokens[], int index);
char* expr_instructions(Token** tokens, int index);

#endif