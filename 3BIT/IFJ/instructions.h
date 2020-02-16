/**
 * Zápis instrukcí
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 */

#ifndef instructions_h
#define instructions_h
#include <stdio.h>
#include "helpers.h"
#include "parser.h"

extern int temp_expr_cnt;

void instruction(char *instrc, ...);
void _instruction_write_to_file(char *instruction);
void instruction_move(char *dest, char* frame, char *source);
void instruction_createframe();
void instruction_pushframe();
void instruction_popframe();
void instruction_header();
void instruction_defvar(char *var, char *frame);
void instruction_call(char *function_name, Function_Arguments *arguments, Context *ctx);
void instruction_add(char *var_result, char *frame1, char *var1, char *frame2, char *var2);
void instruction_sub(char *var_result, char *frame1, char *var1, char *frame2, char *var2);
void instruction_write(char *str, char* val);
void instruction_write_var(char *var_name);
void instruction_pushs(char *var_name, char *var_owner, char *val, char *frame);
void instruction_read(char *str, char *type);
void instruction_return(char *var_name, Context *ctx, char *value);
//char *instruction_label_condition();
void instruction_pops(char *var_name, char *frame);
void instruction_jump(char *label_name);
//void instruction_jump_elseif(Context *context);
//void instruction_jump_endif(Context *context);
void instruction_push(char *var_name);
void instruction_pop(char *var_name);
void instruction_arguments_function(Function *function);
char *instruction_label_function(char *function_name);
void instruction_make_label(char* name);
//char *instruction_label_while(Context *ctx);
//void instruction_jumpif_while_condition(Token *expression[], int tokens_length, Context * ctx, int jump_to_start);
void instruction_jump_endwhile(Context *ctx);
//void instruction_jumpif_condition(Token *expression[], int tokens_length, Context *ctx);
void function_length();
void function_chr();
void function_asc();
void function_substr();
void instruction_lt(char* var_result, char* frame1, char* var1, char* frame2, char* var2);
void instruction_gt(char* var_result, char* frame1, char* var1, char* frame2, char* var2);
void instruction_eq(char* var_result, char* frame1, char* var1, char* frame2, char* var2);
void instruction_or(char* var_result, char* frame1, char* var1, char* frame2, char* var2);
void instruction_int2char(char* var_result, char *var_int);
void instruction_stri2int(char* var_result, char* frame1, char* var1, char* frame2, char* var2);
void instruction_concat(char* var_result, char* frame1, char* var1, char* frame2, char* var2);
void instruction_strlen(char* var_result, char *var_str);
void instruction_getchar(char* var_result, char* frame1, char* var1, char* frame2, char* var2);
void instruction_jumpifeq(char* label, char* frame1, char* var1, char* frame2, char* var2);
void instruction_jumpifneq(char* label, char* frame1, char* var1, char* frame2, char* var2);
void generate_string(char *dest, size_t length);
void instruction_condition_begin(Token_Type cmp_type, char *id);
void instruction_condition_middle(char *id);
void instruction_condition_end(char *id);
void instruction_while_begin(char* L_name, char* R_name, Token_Type cmp_type, char *id);
void instruction_while_end(char *id);
void name_builder(char *name, char *str, char *id);
char *instruction_expr(Token *left, Token *right, Token *sign);
#endif /* instructions_h */
