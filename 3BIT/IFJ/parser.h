/**
 * Parser
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 **/

#ifndef parser_h
#define parser_h

#include <stdio.h>
#include "tokens.h"
#include "symtable.h"

typedef enum {
    context_function,
    context_return,
    context_condition,
    context_condition_else,
    context_while,
    context_scope,
    context_none
} contexts;

typedef struct {
    contexts context;
    char *label_name;
    
} Context;

typedef struct {
    Context **arr;
    int top;
    int max;
} Context_Stack;

typedef enum {
    label_function,
    label_condition
    
} label_type;

extern Binary_Tree BT_vars;
extern Context_Stack *context_stack;

int parse_is_return(int *line_number);
int parse_var_def(int *line_number);
int parse_id_assign(int *line_number);
int parse_what_id(int *line_number);
int parse_exprs_n(int *line_number);
int parse_stat(int *line_number);
int parse_stat_list(int *line_number);
int parse_check_arguments_call_next(int *line_number);
int parse_check_argument_call(int *line_number);
int parse_check_arguments_call(int *line_number);
int parse_check_arguments_next(int *line_number);
int parse_check_argument(int *line_number);
int parse_check_arguments(int *line_number);
int parse_check_function(int *line_number);
int parse_function_declaration(int *line_number);
int parse_functions_declaration(int *line_number);
int parse_function_definition(int *line_number);
int parse_functions_definition(int *line_number);
int parse_commentary(int *line_number, Token* token);
int parse_prog(int *line_number);

void parse_file();
int parse_find_token(Token *token_buffer[], int buffer_size, Token_Type token_type);
int parse_check_bracket(Token *token_buffer[], int buffer_size);
int parse_process(int *line_number);

void context_stack_init(Context_Stack *stack);
void context_push(Context_Stack *stack, contexts context, char *label_name);
Context *context_top(Context_Stack *stack);
Context *context_pop(Context_Stack *stack);
void context_stack_dispose(Context_Stack* stack);


#endif /* parser_h */
