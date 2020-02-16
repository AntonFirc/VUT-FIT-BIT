/**
 * Helpers
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn)
 */


#ifndef helpers_h
#define helpers_h

#include "tokens.h"

typedef enum {
    None,
    Int,
    Double,
    String,
    Bool,
    Void
} Variable_Type;

extern char *Variable_Type_String[];


typedef struct {
    Variable_Type type;
    char *name;
    char *value;
    char *owner;
} Variable;


typedef struct {
    Variable_Type var_type;
    char *var_name;
    char *value; // value, in case it isn't a variable (like function(3, 2, 5))
} Function_Argument;

typedef struct {
    int arguments_cnt;
    Function_Argument *arr;
} Function_Arguments;

typedef struct {
    Function_Arguments *arguments;
    int arguments_cnt;
    char *name;
    char *label_name;
    Variable_Type return_type;
} Function;

void var_init(Variable *var) ;


int is_expr_part_cmp(Token* token);
int is_cmp_char(Token* token);
int is_expr_part(Token* token);
int check_var_type_exists(char *type);
int parse_check_var_type(Variable *var, Token *token);

int token_is_int(char *token_string);
//void function_init(Function *function);
//void function_add(Function *function);
//Function *function_get(char *function_name);
//Function *function_get_by_label(char *label_name);
char* token_type_to_char(Token* token);
void function_argument_add(Function *function, char *var_name, char *var_type);
char* convert_double_to_int(char* dbl_in);
char* convert_value_to_var_type(char* value);
char* convert_char_to_sequence(int chr);
int check_keyword_exists(char *keyword);
int is_reserved_function(char* name);
int is_sign(char c);
int str_equal(char *str1, char *str2);
int str_match(char *str1, char *str2);
char *var_get_type(char *var_name);
int is_digit(char c);
int is_bracket(char c);
int is_dot(char c);
char *convert_variable_type_to_string(Variable_Type var_type);
char *replace_words (char *sentence, char *find, char *replace);
int is_comparsion_sign(char c);
char * evaluate_vars(char *str);
//do_math
void infix_to_postfix(Token **tokens, int length);
//END do_math
int evaluate_expression(char *expression);
Variable_Type convert_char_to_var_type(char *var_type);
char* convert_var_type_to_char(Variable_Type var_type);

Variable *var_set_int(char *var_name, int value);
Variable *var_get_int(char *var_name);
Variable *var_get_float(char *var_name) ;
Variable *var_get_string(char *var_name);
#endif
