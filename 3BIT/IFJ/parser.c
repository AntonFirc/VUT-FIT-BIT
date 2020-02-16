/**
 * Parser
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 **/


#include "parser.h"
#include "definitions.h"
#include "tokens.h"
#include "instructions.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "tokens.h"
#include "errors.h"
#include "helpers.h"
#include "symtable.h"
#include "exprs.h"
#include "memory_stack.h"
#include "string_stack.h"

extern Binary_Tree_Functions BT_functions;

Binary_Tree BT_vars;
Context_Stack *context_stack = NULL;

Token *token = NULL;
Token *token_buffer[128] = {0};
int buffer_length = 0;

/*
 * funkce pro praci s context_stack
 */
void context_stack_init(Context_Stack *stack) {
    stack->top = -1;
    stack->max = -1;
    stack->arr = malloc(sizeof(Context *));
    //mem_store(stack->arr);
}

void context_push(Context_Stack *stack, contexts context, char *label_name) {
    Context *ctx = malloc(sizeof(Context));
    mem_store(ctx);
    ctx->context = context;
    ctx->label_name = label_name;
    
    stack->top++;
    
    stack->arr = realloc(stack->arr, sizeof(Context *) * (stack->top + 1));
    stack->arr[stack->top] = ctx;
}

Context *context_top(Context_Stack *stack) {
    if (stack->top == -1) {
        return NULL;
    }
    return stack->arr[stack->top];
}

Context *context_pop(Context_Stack *stack) {
    if (stack->top == -1) {
        return NULL;
    }
    
    Context *ctx = stack->arr[stack->top];
    stack->arr[stack->top--] = NULL;
    // nechceme ho reallocovat na 0 prvků
    if (stack->top > 0) {
            stack->arr = realloc(stack->arr, sizeof(Context *) * (stack->top + 1));
    }
    
    return ctx;
}

void context_stack_dispose(Context_Stack* stack) {
    free(stack->arr);
}

/**
 * Funkce pro kontrolu počtu závorek. Levých musí být stejný počet, jako pravý,
 * jinak vrací 0
 * @return int
 */
int parse_check_brackets_cnt(Token *token_buffer[], int buffer_size) {
    int bracket_left = 0;
    int bracket_right = 0;

    for (int i = 0; i < buffer_size; i++) {
        if (token_buffer[i]->type == token_bracket_left) {
            bracket_left++;
        }    
        if (token_buffer[i]->type == token_bracket_right) {
            bracket_right++;
        }
    }
    return bracket_left == bracket_right;
}

/**
 * Pokusí se v bufferu najít token o daném typu. V případě nálezu vrátí index,
 * kde se nachzí, jinak vrací -1
 * @return int
 */
int parse_find_token(Token *token_buffer[], int buffer_size, Token_Type token_type) {
    for(int i = 0; i < buffer_size; i++) {
        if (token_buffer[i] == NULL) {
            error_internal("Fatal error, token_buffer[%d] is NULL", 0, i);
        }
        
        if (token_buffer[i]->type == token_type) {
            return i;
        }
    }
    return -1;
}
/**
 * Vyparsuje z token bufferu argumenty funkce a vrátí je d
 *
 */
Function_Arguments* parse_function_arguments(Token *token_buffer[], int buffer_length, int function_name_index) {
    Function_Arguments *arguments = malloc(sizeof(Function_Arguments) * 128);
    mem_store(arguments);
    
    arguments->arr = malloc(sizeof(Function_Argument));
    mem_store(arguments->arr);
    arguments->arguments_cnt = 0;
    //function_name_index has function name, + 1 is the first arg
    for(int i = function_name_index + 1; i < buffer_length; i++) {
       
        
        Token *actual = token_buffer[i];
        if (actual->type == token_comma) {
            continue;
        } else if (actual->type == token_identifier) {
            
            Variable *var_arg;
            char *owner;
            Context *ctx = context_top(context_stack);
            owner = ctx->label_name;
            
            var_arg = BST_Search(BT_vars,actual->data, owner);
            if(var_arg == NULL){
               error_semantic("Variable not defined!",-1); //line number ?
               return NULL;
            }
            

            int new_size = ++(arguments->arguments_cnt);
            //arguments = realloc(arguments, sizeof(Function_Arguments));
            
            arguments->arr = realloc(arguments->arr, sizeof(Function_Argument) * new_size);
            if (arguments->arr == NULL) {
                error_internal("Realloc failed in parse_function_arguments", 42);
            }
            Function_Argument *argument = &arguments->arr[new_size - 1];
            //printf("%p\n", argument);
            
            argument->var_name = actual->data;
            //argument->var_value =
            argument->var_type = var_arg->type;
        } else if (actual->type == token_bracket_right) {
            break;
        } else if (actual->type == token_bracket_left) {
            continue;
        } else if (actual->type == token_keyword) {
            // "as"
        } else if (actual->type == token_int || actual->type == token_double || actual->type == token_string_literal) {
            // argument is an int, a double or string literal
            
            int new_size = ++(arguments->arguments_cnt);
            arguments->arr = realloc(arguments->arr, sizeof(Function_Argument) * new_size);
            if (arguments->arr == NULL) {
                error_internal("Realloc failed in parse_function_arguments", 42);
            }
            Function_Argument *argument = &arguments->arr[new_size - 1];
       
            argument->value = actual->data;
            argument->var_name = NULL;
            switch(actual->type) {
                case token_int: argument->var_type = Int; break;
                case token_double: argument->var_type = Double; break;
                case token_string_literal: argument->var_type = String; break;
                default: break;
            }
            
        } else {
            //@todo line??
            printf("Error: Token is %s\n", actual->data);
            error_semantic("Expecting only identifiers and commas in argument list.", 0);
        }
        
    }
    return arguments;
}

extern int functions_cnt;
/*
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 * ZAKAZANA CAST - SEM NESAHAT AK PRESNE NEVIETE CO ROBITE !!!!!!!
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 */
/*
 * kontorla IS_RETURN
 */
int parse_is_return(int *line_number) {

    if (str_match(token->data, "return")) {
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);
        //return ID
        if (token->type == token_identifier) {
            token_buffer[buffer_length++] = token;
            token = token_get(line_number);
            if (token->type == token_eol) {
                parse_process(line_number);
                token = token_get(line_number);
                return 0;
            }
            else {
                error_syntactic("Expected EOL after RETURN EXPR", *line_number);
                return 1;
            }
        }
        //return EXPR
        else if ((token->type == token_int) || (token->type == token_double) || (token->type == token_string_literal) || 
                    is_expr_part(token) || (token->type == token_identifier)) {  
            token_buffer[buffer_length++] = token;
            token = token_get(line_number);
            while ((token->type == token_int) || (token->type == token_double) || (token->type == token_string_literal) || 
                    is_expr_part(token) || (token->type == token_identifier)) {       //EXPR
                token = token_get(line_number);
                token_buffer[buffer_length++] = token;
            }
            if (token->type == token_eol) {     //EXPR EOL
                parse_process(line_number);
                token = token_get(line_number);
                return 0;
            }
            else {      //neni EOL, pokracuje ID EXPR
                error_syntactic("Expected EOL after RETURN EXPR", *line_number);
                return 1;
            }
        }
        //return (INT || DOUBLE || STRING)
        else if ((token->type == token_string_literal) || (token->type == token_int) || (token->type == token_double)) {
            token_buffer[buffer_length++] = token;
            token = token_get(line_number);
            if (token->type != token_eol) {
                error_syntactic("Expected EOL after RETURN EXPR", *line_number);
                return 1;
            }
        }
        else {
            error_syntactic("Expected EXPRESSION or EOL after RETURN", *line_number);
            return 1;
        }
    }
    return 0;
}

/*
 * kontorla VAR_DEF
 */
int parse_var_def(int *line_number) {

    if (token->type == token_equals) {  // =
       token_buffer[buffer_length++] = token;
       token = token_get(line_number);
       while ((token->type == token_string_literal) || (token->type == token_int) || 
          (token->type == token_double) || (token->type == token_identifier) || is_expr_part(token)) {  //EXPR
            token_buffer[buffer_length++] = token;
            token = token_get(line_number);
        }
    }
    return 0;
}

/*
 * kontorla ID_ASSIGN
 */
int parse_id_assign(int *line_number) {
    
    if (token->type == token_identifier || token->type == token_identifier_function) {  //ID
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);
        
        if (token->type == token_bracket_left) {    // (
            if (parse_check_arguments_call(line_number)) {
                return 1;
            }
            /*if (token->type != token_bracket_right) {
                error_syntactic("Expected \')\' after function arguments", *line_number);
                return 1;
            }
            token_buffer[buffer_length++] = token;
            token = token_get(line_number);*/
            if (token->type != token_eol) {
                error_syntactic("Unexpected sequence after function call", *line_number);
                return 1;
            }
            return 0;
        }
        else if ((token->type == token_string_literal) || (token->type == token_int) || 
            (token->type == token_double) || (token->type == token_identifier) || is_expr_part(token)) {
                token_buffer[buffer_length++] = token;
                token = token_get(line_number);
                while ((token->type == token_string_literal) || (token->type == token_int) || 
                       (token->type == token_double) || (token->type == token_identifier) || is_expr_part(token)) {
                    token_buffer[buffer_length++] = token;
                    token = token_get(line_number);
                }
            }
    }
    else if ((token->type == token_string_literal) || (token->type == token_int) || 
            (token->type == token_double) || is_expr_part(token)) {
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);
        while ((token->type == token_string_literal) || (token->type == token_int) || 
               (token->type == token_double) || (token->type == token_identifier) || is_expr_part(token)) {
            token_buffer[buffer_length++] = token;
            token = token_get(line_number);
        }
        if (token->type != token_eol) {
            error_syntactic("Unexpected sequence after ID =", *line_number);
            return 1;
        }
    }
    else {
        error_syntactic("Unexpected sequence.", *line_number);
        return 1;
    }
    return 0;
}

/*
 * kontrola WHAT_ID
 */
int parse_what_id(int *line_number) {

    token_buffer[buffer_length++] = token;
    token = token_get(line_number);
    token_buffer[buffer_length++] = token;
    if (token->type == token_bracket_left) {        // (
        if (parse_check_arguments_call(line_number)) {
            return 1;
        }
        /*if (token->type != token_bracket_right) {
            error_syntactic("Expected \')\' after function arguments", *line_number);
            return 1;
        }
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);*/
    }
    else if (token->type == token_equals) { // = 
        token = token_get(line_number);
        if (parse_id_assign(line_number)) {
        return 1;
        }
    }
    
    if (token->type != token_eol) {
        error_syntactic("Expected EOL", *line_number);
        return 1;
    }
    else {
        token_buffer[buffer_length++] = token;
        parse_process(line_number);
        token = token_get(line_number);
    }
    return 0;
}

/*
 * kontorla EXPRS_N
 */
int parse_exprs_n(int *line_number) {
    if (token->type != token_eol) {         //neni EOL, mame dalsie argumenty
        if (token->type == token_semicolon) {
            token_buffer[buffer_length++] = token;
            token = token_get(line_number);
            if ((token->type == token_identifier) || (token->type == token_string_literal) ||
                (token->type == token_int) || (token->type == token_double) || is_expr_part(token)) { //EXPR
                    while ((token->type == token_identifier) || (token->type == token_string_literal) ||
                            (token->type == token_int) || (token->type == token_double) || is_expr_part(token)) {
                        token_buffer[buffer_length++] = token;
                        token = token_get(line_number);
                    }
                    if (token->type != token_semicolon) {
                        error_syntactic("Expected semicolon after print expression", *line_number);
                        return 1;
                    }
                    if (parse_exprs_n(line_number)) {
                        return 1;
                    }
            }
        }
        else {
            error_syntactic("Expected semicolon", *line_number);
            return 1;
        }
    }
    return 0;    
}

/*
 * kontrola STATEMENT
 */
int parse_stat(int *line_number) {
    
    //PRINT exprs EOL
    if (str_match(token->data, "print")) {
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);
        if ((token->type == token_string_literal) || (token->type == token_expression) || (token->type == token_identifier) ||
            (token->type == token_int) || (token->type == token_double) || is_expr_part(token)) {  //print EXPR
            while((token->type == token_string_literal) || (token->type == token_expression) || (token->type == token_identifier) ||
                  (token->type == token_int) || (token->type == token_double) || is_expr_part(token)) {
                token_buffer[buffer_length++] = token;
                token = token_get(line_number);
            }
            if (parse_exprs_n(line_number)) {
                return 1;
            }
        }
        else {
            error_syntactic("Expected STRING or EXPRESSION after PRINT", *line_number);
            return 1;
        }
    }
    //DIM ID AS VAR_TYPE var_def EOL
    else if (str_match(token->data, "dim")) {
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);
        if (token->type == token_identifier) { //ID
            token_buffer[buffer_length++] = token;
            token = token_get(line_number);
            if (str_match(token->data, "as")) {  //AS
                token_buffer[buffer_length++] = token;
                token = token_get(line_number);
                if (token->type == token_var_type) { //VAR_TYPE
                    token_buffer[buffer_length++] = token;
                    token = token_get(line_number);
                    if (parse_var_def(line_number)) {   //var_def
                        return 1;
                    }
                    if (token->type != token_eol) {    //EOL
                        error_syntactic("Expected EOL", *line_number);
                        return 1;
                    }
                } else {
                    error_syntactic("Expected VAR_TYPE", *line_number);
                    return 1;   
                }
            } else {
                error_syntactic("Expected keyword AS", *line_number);
                return 1;
            }
        } else {
            if (token->type == token_identifier_function) {
                error_semantic("Variable name can't match function name", *line_number);
                return 1;
            }
            error_syntactic("Expected IDENTIFIER", *line_number);
            return 1;
        }
    }
    // what_id
    else if (token->type == token_identifier) {
        if (parse_what_id(line_number)) {
            return 1;
        }
    }
    // INPUT ID EOL
    else if (str_match(token->data, "input")) {
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);
        token_buffer[buffer_length++] = token;
        if (token->type != token_identifier) {
            error_syntactic("Expected IDENTIFIER", *line_number);
            return 1;
        }
        token = token_get(line_number);
        token_buffer[buffer_length++] = token;
        if (token->type != token_eol) {
            error_syntactic("Expected EOL", *line_number);
            return 1;
        }
    }
    // IF EXPR THEN EOL stat_list ELSE EOL stat_list END IF EOL
    else if (str_match(token->data, "if")) {
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);
        token_buffer[buffer_length++] = token;
        while ((token->type == token_int) || (token->type == token_double) || (token->type == token_string_literal) || 
                is_expr_part_cmp(token) || (token->type == token_identifier)) {       //EXPR
            token = token_get(line_number);
            token_buffer[buffer_length++] = token;
        }
        if (str_match(token->data, "then")) {    //THEN
            token = token_get(line_number);
            token_buffer[buffer_length++] = token;
            if (token->type == token_eol) {      //EOL
                parse_process(line_number);
                token = token_get(line_number);
                //token_buffer[buffer_length++] = token;
                if (parse_stat_list(line_number)) {   //STAT_LIST
                    return 1;
                }
                if (str_match(token->data, "else")) {    //ELSE
                    token_buffer[buffer_length++] = token;
                    token = token_get(line_number);
                    token_buffer[buffer_length++] = token;
                    if (token->type == token_eol) {     //EOL
                        parse_process(line_number);
                        token = token_get(line_number);
                        if (!str_match(token->data, "end")) {
                        token_buffer[buffer_length++] = token;}
                        if (parse_stat_list(line_number)) {   //STAT_LIST
                            return 1;
                        }
                        if (str_match(token->data, "end")) { //END
                            token_buffer[buffer_length++] = token;
                            token = token_get(line_number);
                            token_buffer[buffer_length++] = token;
                            if (str_match(token->data, "if")) {  //IF
                                token = token_get(line_number);
                                token_buffer[buffer_length++] = token;
                                if (token->type != token_eol) {     //EOL
                                    error_syntactic("Expected EOL", *line_number);
                                    return 1; 
                                }
                            }
                            else {
                                error_syntactic("Expected END IF", *line_number);
                                return 1;    
                            }
                        }
                        else {
                            error_syntactic("Expected END IF", *line_number);
                            return 1;
                        }
                    }
                    else {
                        error_syntactic("Expected EOL", *line_number);
                        return 1; 
                    }
                }
                else {
                    error_syntactic("Expected ELSE", *line_number);
                    return 1;
                }

            }
            else {
                error_syntactic("Expected EOL", *line_number);
                return 1;
            }
        }
        else {
            error_syntactic("Expected THEN", *line_number);
            return 1;
        }
        
    }
    // DO WHILE EXPR EOL stat_list LOOP EOL
    else if (str_match(token->data, "do")) { //DO
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);
        token_buffer[buffer_length++] = token;
        if (str_match(token->data, "while")) {   //WHILE
            token = token_get(line_number);
            token_buffer[buffer_length++] = token;
            while ((token->type == token_int) || (token->type == token_double) || (token->type == token_string_literal) || 
                    is_expr_part_cmp(token) || (token->type == token_identifier)) {       //EXPR
                token = token_get(line_number);
                token_buffer[buffer_length++] = token;
            }
            if (token->type == token_eol) {     //EOL
                
                token = token_get(line_number);
                token_buffer[buffer_length++] = token;
                parse_process(line_number);
                if (parse_stat_list(line_number)) {   //stat_list
                    return 1;
                }
                if(str_match(token->data, "loop")) {     //LOOP
                    token_buffer[buffer_length++] = token;
                    token = token_get(line_number);
                    token_buffer[buffer_length++] = token;
                    if (token->type != token_eol) {     //EOL
                        error_syntactic("Expected EOL", *line_number);
                        return 1; 
                    }
                } 
                else {
                    error_syntactic("Expected LOOP", *line_number);
                    return 1; 
                }
            }
            else {
                error_syntactic("Expected EOL", *line_number);
                return 1; 
            } 
        }
        else {
            error_syntactic("Expected WHILE", *line_number);
            return 1;
        }
    }
    else if (token->type == token_eol) {

    }
    else {
        error_syntactic("Unexpected sequence", *line_number);
        return 1;
    }

    //koniec riadku, nacitame zaciatok dalsieho riadku
    if (token->type == token_eol) {
        
        parse_process(line_number);
        token = token_get(line_number);
        return 0;
    }

    return 0;
}

/*
 * kontrola STAT_LIST
 */
int parse_stat_list(int *line_number) {
    
    if (str_match(token->data, "dim") ||
        str_match(token->data, "input") ||
        str_match(token->data, "print") ||
        str_match(token->data, "if") ||
        str_match(token->data, "do") ||
        token->type == token_identifier ||
        token->type == token_eol
    ) {
        if (parse_stat(line_number)) {
            return 1;
        }
        if (parse_stat_list(line_number)) {
            return 1;
        }
    }
    return  0;
}

/*
 * kontorla FUNCTION_ARGUMENTS_CALL_N
 */
int parse_check_arguments_call_next(int *line_number) {

    //token = token_get(line_number);
    //token_buffer[buffer_length++] = token;
    if (token->type == token_comma) {   // ','
        token = token_get(line_number);
        token_buffer[buffer_length++] = token;
        if (parse_check_argument_call(line_number)) {
            return 1;
        }
        // ak je za nou zatvorka => chyba, chyba argument
        if (parse_check_arguments_call_next(line_number)) {
            return 1;
        }
    }
    return 0;
}

/*
 * kontorla FUNCTION_ARGUMENT_CALL
 */
int parse_check_argument_call(int *line_number) {

    if ((token->type != token_identifier) && (token->type != token_int) && (token->type != token_double) && 
        (token->type != token_string_literal)) {  // ID
        error_syntactic("Expected EXPRESSION as function argument", *line_number);
        return 1;
    }
    else {
        while ((token->type == token_int) || (token->type == token_double) || (token->type == token_string_literal) || 
                is_expr_part(token) || (token->type == token_identifier)) {       //EXPR
            token = token_get(line_number);
            token_buffer[buffer_length++] = token;
            if (token->type == token_bracket_right) {
                token = token_get(line_number);
                token_buffer[buffer_length++] = token;
                if (token->type == token_eol) {
                    return 0;
                }
            }
            else if (token->type == token_eol) {
                error_syntactic("Missing ) after function arguments ?", *line_number);
                return 1;
            }
        }
    }
    return 0;

}

/*
 * kontorla FUNCTION_ARGUMENTS_CALL
 */
int parse_check_arguments_call(int *line_number) {

    token_buffer[buffer_length++] = token;
    token = token_get(line_number);
    token_buffer[buffer_length++] = token;
    // E
    if (token->type == token_bracket_right) {
        token = token_get(line_number);
        token_buffer[buffer_length++] = token;
        return 0;
    }

    if (parse_check_argument_call(line_number)) {
        return 1;
    }

    if (parse_check_arguments_call_next(line_number)) {
        return 1;
    }
    
    return 0;
}

/*
 * kontorla FUNCTION_ARGUMENTS_N
 */
int parse_check_arguments_next(int *line_number) {

    token = token_get(line_number);
    token_buffer[buffer_length++] = token;
    // ciarka oddelujuca argumenty najdena
    if (token->type == token_comma) {
        token = token_get(line_number);
        token_buffer[buffer_length++] = token;
        if (parse_check_argument(line_number)) {
            return 1;
        }

        if (parse_check_arguments_next(line_number)) {
            return 1;
        }
    }
    return 0;
}

/*
 * kontorla FUNCTION_ARGUMENT
 */
int parse_check_argument(int *line_number) {

    if (token->type != token_identifier) {  //ID
        error_syntactic("Expected IDENTIFIER as function argument", *line_number);
        return 1;
    }
    token = token_get(line_number);
    token_buffer[buffer_length++] = token;
    // keyword AS
    if (!str_match(token->data, "as")) { //AS
        error_syntactic("Expected keyword AS after IDENTIFIER", *line_number);
        return 1;
    }
    token = token_get(line_number);
    token_buffer[buffer_length++] = token;
    // datovy typ argumentu
    if (token->type != token_var_type) {    //VAR_TYPE
        error_syntactic("Expected VAR_TYPE after AS", *line_number);
        return 1;
    }
    return 0;
}

/*
 *  Kontrola ARGUMENTOV
 */
int parse_check_arguments(int *line_number) {
    
    token = token_get(line_number);
    token_buffer[buffer_length++] = token;
    //E
    if (token->type == token_bracket_right) {
        return 0;
    }

    if (parse_check_argument(line_number)) {
        return 1;
    }

    if (parse_check_arguments_next(line_number)) {
        return 1;
    }

    return 0;
}

/*
 * kontorla predpisu funkcie (FUNCTION) ID ( ARGUMENTY ) RETURN VAR_TYPE EOL
 */
int parse_check_function(int *line_number) {
   
    token = token_get(line_number);
    token_buffer[buffer_length++] = token;
    // nasleduje ID funkcie
    if (token->type == token_identifier || token->type == token_identifier_function) {
        token = token_get(line_number);
        token_buffer[buffer_length++] = token;
        // nasleduje zavorka a argumenty
        if (token->type == token_bracket_left) {
            parse_check_arguments(line_number); //kontrola spravnosti argumentov
            if (token->type != token_bracket_right) {
                error_syntactic("Expected ')' after function arguments", *line_number);
                return 1;
            }
            token = token_get(line_number);
            token_buffer[buffer_length++] = token;
            if (str_match(token->data, "as")) {
                token = token_get(line_number);
                token_buffer[buffer_length++] = token;
                if (token->type == token_var_type) {
                    token = token_get(line_number);
                    token_buffer[buffer_length++] = token;
                    if (token->type != token_eol) {
                        error_syntactic("Expected end of line", *line_number);
                        return 1;
                    }// EOL
                    else {
                        parse_process(line_number);
                    }
                }
                else {
                    error_syntactic("Expected return type", *line_number);
                    return 1;    
                }// return_type
            }
            else {
                error_syntactic("Expected keyword AS", *line_number);
                return 1;     
            }// as return_type
        }
        else {
            error_syntactic("Expected left bracket (argument definition)", *line_number);
            return 1;     
        }//lava zavorka
    }
    else {
        error_syntactic("Expected identifier (function name)", *line_number);
        return 1; 
    }//id_funkce
    return 0;
}

/*
 * FUNCTION_DECLARATION
 */
int parse_function_declaration(int *line_number) {

    token_buffer[buffer_length++] = token;
    token = token_get(line_number);
    token_buffer[buffer_length++] = token;
    // ak deklarujeme prototyp funkcie
    if (str_match(token->data, "function")) {
        if (parse_check_function(line_number)) {
            return 1;
        }
        parse_process(line_number);
    }//keyword function
    else {
        error_syntactic("Expected function declaration", *line_number);
        return 1;
    }

    //EOL
    if (token->type == token_eol) {
        token_buffer[buffer_length++] = token;
        
        token = token_get(line_number);
    }
    else {
        error_syntactic("Unexpected sequence after END FUNCTION, EOL expected", *line_number);
        return 1;
    } 

    parse_process(line_number);

    return 0;
}

/*
 * FUNCTIONS_DECLARATION
 */
int parse_functions_declaration(int *line_number) {

    if (str_match(token->data, "declare")) {
    
        if (parse_function_declaration(line_number)) {
            return 1;
        }

        if (parse_functions_declaration(line_number)) {
            return 1;
        }
            
    } 
    return 0;
}

/*
 * FUNCTION_DEFINITION
 */
int parse_function_definition(int *line_number) {
    
    token_buffer[buffer_length++] = token;
    parse_check_function(line_number);
    token = token_get(line_number);
    parse_process(line_number);
    if (parse_stat_list(line_number)) {
        return 1;
    }

    //is_return
    if (parse_is_return(line_number)) {
        return 1;
    }

    //END function
    if (str_match(token->data, "end")) {
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);
    }
    else {
        error_syntactic("Expected END FUNCTION", *line_number);
        return 1;
    }

    //end FUNCTION
    if (str_match(token->data, "function")) {
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);
    }
    else {
        error_syntactic("Expected END FUNCTION", *line_number);
        return 1;
    }

    //EOL
    if (token->type == token_eol) {
        token_buffer[buffer_length++] = token;
        
        token = token_get(line_number);
    }
    else {
        error_syntactic("Unexpected sequence after END FUNCTION, EOL expected", *line_number);
        return 1;
    } 
    parse_process(line_number);
    if (!str_match(token->data,"scope")) {
        token = token_get(line_number);
    }
    return 0;

}

/*
 * FUNCTIONS_DEFINITION
 */
int parse_functions_definition(int *line_number) {
    
    if (str_match(token->data, "function")) {

        if (parse_function_definition(line_number)) {
            return 1;
        }

        if (parse_functions_definition(line_number)) {
            return 1;
        }
    }

    return 0;        
}

int parse_prog(int *line_number) {
    
    //nacitali sme prazdny riadok, ideme dalej
    if ((token = token_get(line_number))->type == token_eol) {
        
        return parse_prog(line_number);
    }
    
    //deklarace
    if (parse_functions_declaration(line_number)) {
        return 1;
    }

    // po deklaracich jump do mainu (prekok definici funkci)
    instruction_jump("\%SCOPE_MAIN\%");

    if (token->type == token_eol) {
        
        while ((token = token_get(line_number))->type == token_eol) {
            
        }
    }

    function_length();
    function_chr();
    function_asc();
    function_substr();

    //definice
    if (parse_functions_definition(line_number)) {
        return 1;
    }

    if (token->type == token_eol) {
        
        while ((token = token_get(line_number))->type == token_eol) {
            
        }
    }

    //SCOPE
    if (str_match(token->data, "scope")) {
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);
    }
    else {
        error_syntactic("Expected scope", *line_number);
        return 1;
    }

    //EOL
    if (token->type == token_eol) {
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);
    }
    else {
        printf("Toto tam je %d\n",token->data[0]);
        error_syntactic("Unexpected sequence after SCOPE, EOL expected", *line_number);
        return 1;
    }

    parse_process(line_number);

    //stat_list
    if (parse_stat_list(line_number)) {
        return 1;
    }
    //printf("token %s\n", token->data);
    //END scope
    if (str_match(token->data, "end")) {
        token_buffer[buffer_length++] = token;
        token = token_get(line_number);
    }
    else {
        error_syntactic("Expected END SCOPE", *line_number);
        return 1;
    }

    //end SCOPE
    if (str_match(token->data, "scope")) {
        token_buffer[buffer_length++] = token;
        parse_process(line_number);
        return 0;
    }
    else {
        error_syntactic("Expected END SCOPE", *line_number);
        return 1;
    }

    parse_process(line_number);

    return 0; //explicitny return, ale inak hadze warning
}
/*
 * @@@@@@@@@@@@@@@@@@@@@@
 * KONIEC ZAKAZANEJ CASTI
 * @@@@@@@@@@@@@@@@@@@@@@
 */

/**
 * Zparsuje se řádek, proběhne syntaktická analýza
 * @return int V případě chyby vrací 0, jinak 1 - může se pokračovat v dalším řádku
 */
int parse_process(int *line_number) {
    int tmp_buff_length = buffer_length;    
    buffer_length = 0;
    int keyword_index = 0;

    // Any keyword
    if ((keyword_index = parse_find_token(token_buffer, tmp_buff_length, token_keyword)) != -1) {
        char *keyword = token_buffer[keyword_index]->data;
        
        /*
         * To co som tu vymazal uz neni treba, ak sa dostaneme sem tak je vsetko syntakticky v poriadku
         */
        if (str_match(keyword, "end")) {
            
            Token *end_what = token_buffer[keyword_index + 1];
            //printf("Context: %d Label: %s Address: %p\n", ctx->context, ctx->label_name, ctx);
            if (str_match(end_what->data, "scope")) {
                context_pop(context_stack);
                instruction_popframe();
            } else if (str_match(end_what->data, "function")) {
                Context *ctx = context_pop(context_stack);
                contexts context_actual = ctx->context;
                //ak sme este nemali return
                if (context_actual == context_return) {
                    context_pop(context_stack);
                    Function* function = function_get_by_label(ctx->label_name);
                    if (function == NULL) {
                        error_internal("Internal search error.",0);
                    }
                    if (function->return_type == Int) {
                        instruction_pushs(NULL, NULL, "0", NULL);
                    }
                    if (function->return_type == Double) {
                        instruction_pushs(NULL, NULL, "0.0", NULL);
                    }
                    if (function->return_type == String) {
                        instruction_pushs(NULL, NULL, "", NULL);
                    }
                    instruction_return(NULL, ctx, NULL);
                }
                return 1;
            } else if (str_match(end_what->data, "if")) {
                instruction_condition_end(str_pop());
                return 1;
                
            }
        }
        
        Token **buffer_expression_condition = NULL;
        Token **buffer_expression_right = NULL;
        int size_condition = 0;
        int size_right = 0;
        

        if (str_match(keyword, "if")) {
            //nacita token za IF -> IF >>>EXPR<<< THEN EOL
            Token* actual = token_buffer[size_condition]; //ked tu je null tak podmienka na dalsom while hodi segfault
            Token* cmp_sign = NULL;
            buffer_expression_condition = calloc(128, sizeof(Token *));
            buffer_expression_right = calloc(128, sizeof(Token *));
            int is_cmp = 0;
            char* name = calloc(6, sizeof(char));
            generate_string(name, 5);
            str_store(name);
            char* L_val = calloc(6, sizeof(char));
            mem_store(L_val);
            strcpy(L_val,"L_");
            strcat(L_val,name);
            instruction_defvar(L_val, "LF");
            char* R_val = calloc(6, sizeof(char));
            mem_store(R_val);
            strcpy(R_val,"R_");
            strcat(R_val,name);
            instruction_defvar(R_val, "LF");
            //context_push(context_stack, context_condition, name);

            while (!str_match(actual->data,"then")) {
                actual = token_buffer[size_condition]; //size_condition+1, pretoze prvy z token_buffer je pre nas 0-ty
                buffer_expression_condition[size_condition++] = actual;
                if (is_cmp_char(actual)) {
                    cmp_sign = actual;
                    char* result = expr_instructions(buffer_expression_condition, size_condition);
                    if (BST_Search(BT_vars, result, context_top(context_stack)->label_name) != NULL) {
                        instruction_move(L_val, "LF", result);
                    }
                    else {
                        instruction_move(L_val, convert_value_to_var_type(result), result);
                    }
                    is_cmp = 1;
                    continue;
                }
                if (is_cmp) {
                    buffer_expression_right[size_right++] = actual;
                    buffer_expression_condition[size_condition] = actual;
                }
            }
            if (is_cmp) {
                char* result = expr_instructions(buffer_expression_right, size_right);
                if (BST_Search(BT_vars, result, context_top(context_stack)->label_name) != NULL) {
                    instruction_move(R_val, "LF", result);
                }
                else {
                    instruction_move(R_val, convert_value_to_var_type(result), result);
                }
                instruction_condition_begin(cmp_sign->type, name);
            }
            else {
                char* result = expr_instructions(buffer_expression_condition, size_condition);
                if (BST_Search(BT_vars, result, context_top(context_stack)->label_name) != NULL) {
                        instruction_move(L_val, "LF", result);
                }
                else {
                    instruction_move(L_val, convert_value_to_var_type(result), result);
                }
                instruction_move(R_val, "int", "0");
                instruction_condition_begin(token_not_equal, name);
            }
            return 1;
        } else if (str_match(keyword, "else")) {
            instruction_condition_middle(str_top());
        }

        if (str_match(keyword, "do" )) {

            Token **buffer_while_condition = NULL;
            Token **buffer_expression_right = NULL;
            int size_while = 0;

            //nacita token za WHILE -> DO WHILE >>>EXPR<<< EOL
            Token* actual = token_buffer[0] ;
            Token* cmp_sign = NULL;
            buffer_while_condition = calloc(128, sizeof(Token *));
            buffer_expression_right = calloc(128, sizeof(Token *));
            int is_cmp = 0;
            char* name = calloc(6, sizeof(char));
            generate_string(name, 5);
            str_store(name);
            char* L_val = calloc(6, sizeof(char));
            mem_store(L_val);
            strcpy(L_val,"L_");
            strcat(L_val,name);
            instruction_defvar(L_val, "LF");
            char* R_val = calloc(6, sizeof(char));
            mem_store(R_val);
            strcpy(R_val,"R_");
            strcat(R_val,name);
            instruction_defvar(R_val, "LF");
            char* L_var = NULL;
            char* R_var = NULL;
            //context_push(context_stack, context_condition, name);

            while (actual->type != token_eol) {
                actual = token_buffer[size_while +2]; //size_while+1, pretoze prvy z token_buffer je pre nas 0-ty
                buffer_while_condition[size_while++] = actual;
                if (is_cmp_char(actual)) {
                    cmp_sign = actual;
                    char* result = expr_instructions(buffer_while_condition, size_while);
                    if (BST_Search(BT_vars, result, context_top(context_stack)->label_name) != NULL) {
                        L_var = calloc(15, sizeof(char));
                        strcpy(L_var,result);
                    }
                    else {
                        instruction_move(L_val, convert_value_to_var_type(result), result);
                    }
                    is_cmp = 1;
                    continue;
                }
                if (is_cmp) {
                    buffer_expression_right[size_right++] = actual;
                    buffer_while_condition[size_while] = actual;
                }
            }
                if (is_cmp) {
                char* result = expr_instructions(buffer_expression_right, size_right);
                if (BST_Search(BT_vars, result, context_top(context_stack)->label_name) != NULL) {
                    R_var = calloc(15, sizeof(char));
                    strcpy(R_var,result);
                }
                else {
                    instruction_move(R_val, convert_value_to_var_type(result), result);
                }
                instruction_while_begin(L_var, R_var, cmp_sign->type, name);
                }
                else {
                    char* result = expr_instructions(buffer_while_condition, size_while);
                    if (BST_Search(BT_vars, result, context_top(context_stack)->label_name) != NULL) {
                        L_var = calloc(15, sizeof(char));
                        strcpy(L_val, result);
                    }
                    else {
                        instruction_move(L_val, convert_value_to_var_type(result), result);
                    }
                    instruction_move(R_val, "int", "0");
                    instruction_while_begin(L_var, R_var, token_not_equal, name);
                }
        } else if (str_match(keyword, "loop")) {         
            instruction_while_end(str_pop());
        }
        
        if (str_match(keyword, "scope")) {
            context_push(context_stack, context_scope, "scope");
            instruction_make_label("\%SCOPE_MAIN\%");
            instruction_createframe();
            instruction_pushframe();
        }//context_stack
        
        if (str_match(keyword, "input")) {
            instruction_write(NULL, "?\\032");
            // Input var_name
            char *var_name = token_buffer[keyword_index + 1]->data;
            char *owner;
            Context *ctx = context_top(context_stack);
            if (ctx == NULL) {
                owner = "main";
            } else {
                owner = ctx->label_name;
            }
            Variable *var = BST_Search(BT_vars,var_name, owner);
            if (var == NULL) {
                error_semantic("Undefined variable!", *line_number);
            }
            
            
            // var_type musíme zjistit
            char *var_type = var_get_type(var_name);
            instruction_read(var_name, var_type);
            return 1;
        }
        
       
        
        // print !"test"; !"test2"; !"test3"
        // WRITE testtest2test3
        if (str_match(keyword, "print")) {
            for (int i = 1; i < tmp_buff_length; i++) {
                Token *actual = token_buffer[i];
                // print !"test" ; !"test2"
                if (actual->type == token_semicolon) {
                    continue;
                }
                if (((actual->type == token_int) || (actual->type == token_double) || (actual->type == token_identifier)) && tmp_buff_length > 3 && is_expr_part(token_buffer[i+1])) {
                    Token **expr_buffer = NULL;
                    expr_buffer = calloc(128, sizeof(Token *));
                    int cnt_expr = 0;
                    expr_buffer[cnt_expr++] = token_buffer[i-1];
                    while ((actual->type == token_string_literal) || (actual->type == token_int) || 
                           (actual->type == token_double) || is_expr_part(actual) || actual->type == token_identifier) {
                        expr_buffer[cnt_expr++] = token_buffer[i++];
                        if (i == tmp_buff_length) break;
                        actual = token_buffer[i];
                    }
                    //expr_eval(tokens, index); @@TODO@@ hadze chyby
                    char* result = expr_instructions(expr_buffer, cnt_expr);
                    if (result[0] == '_' && result[1] == '_') { // ak je result[0-3] %_%_
                        instruction_write(result, NULL);
                    }
                    else {
                        instruction_write(NULL, result);
                    }
                }
                if ((actual->type == token_string_literal) || (actual->type == token_int) || (actual->type == token_double) ) {
                    instruction_write(NULL, actual->data);
                }
                //printf("Actual:::: %d (%s), var_name: %d\n", actual->type, actual->data, token_var_name);
                if (actual->type == token_identifier) {
                    if (BST_Search(BT_vars, actual->data, context_top(context_stack)->label_name) == NULL) {
                        error_semantic("Trying to print undeclared variable!", *line_number);
                    }
                    instruction_write_var(actual->data);
                }
            }
            return 1;
        }
        
        // Declare Function ID (ID As VAR_TYPE) As VAR_TYPE
        if (str_match(keyword, "declare")) {

            //kontrola ci uz funkcia neni ulozena
            if (function_get(token_buffer[2]->data) != NULL) {
                error_semantic("Redeclaration of function !", *line_number); 
            }

            Function *function = malloc(sizeof(Function));
            mem_store(function);
            function_init(function);    //inicializace pameti pro funkci
            State state = state_new;
            Token* previous = NULL;

            for (int i = 0; i < tmp_buff_length; i++) {
                Token *actual = token_buffer[i];
                switch (actual->type){
                    case token_identifier:  //meno funkce
                        if (str_match(previous->data, "function")) {                            
                            function->name = actual->data;
                        } 
                        else if (state == state_function_arguments) {   //ID -> argument funkce
                            if (actual->type == token_comma) {
                                continue;
                            }
                            // Jsme v závorkách, očekáváme argumenty. V aktuálním je název proměnné
                            char *var_name = actual->data;
                            char *var_type = token_buffer[i + 2]->data; // i + 1 je Keyword "as" a i + 2 je typ proměnné
                            function_argument_add(function, var_name, var_type);
                            i += 2; // Posuneme se o dvě -> preskocime ciarku a citame ID dalsieho argumentu
                        } 
                        break;
                    case token_var_type:
                        if (state == state_function_return_type) {      //var_type return
                            
                            char *var_type_char = actual->data;          
                            Variable_Type var_type = convert_char_to_var_type(var_type_char);
                            function->return_type = var_type;
                            function_add(function);     //ulozi funkciu do vyhladavacej struktury
                        }
                        break;
                    case token_bracket_left:
                        state = state_function_arguments;
                        break;
                    case token_bracket_right:
                        state = state_function_return_type;
                        break;
                    default:
                        
                        break;
                }
                previous = actual;
            }
            return 0;
        }
        
        // Function factorial (n As Integer) As Integer
        if (str_match(keyword, "function")) {
            
            /*
             [0] = Declare (keyword)
             [1] = Function (keyword)
             [2] = factorial (identifier)
             [3] = ( (bracket_left)
             [4] = n (identifier)
             [5] = As (keyword)
             [6] = Integer (var_type)
             [7] = ) (bracket_right)
             [8] = As (keyword)
             [9] = Integer (var_type)
             */
            Token *previous = NULL;
            Function *function = NULL;
            State state = state_new;
            int is_new = 0;

            //zisti ci uz funckia existuje alebo ju musime ulozit
            if ( (function = function_get(token_buffer[1]->data)) == NULL) {
                function = malloc(sizeof(Function));
                mem_store(function);
                function_init(function);    //inicializace funkce
                is_new = 1;
            }        
            
            int j = 0;

            for (int i = 0; i < tmp_buff_length; i++) {
                Token *actual = token_buffer[i];
                switch (actual->type){
                    case token_identifier:
                        if (state == state_function_arguments) {
                            if (actual->type == token_comma) {
                                continue;
                            }
                            
                            // Jsme v závorkách, očekáváme argumenty. V aktuálním je název proměnné
                            char *var_name = actual->data;
                            char *var_type = token_buffer[i + 2]->data;
                            if (!is_new) {  //jestli uz mame prototyp funkce
                                if ( (j >= function->arguments_cnt) ||
                                    (convert_char_to_var_type(var_type) != function->arguments->arr[j].var_type)) {
                                    error_semantic("Function definition (arguments) does not match its prototype !", *line_number);
                                }
                                ++j;    //inkrementace aby kontrolovalo dalsi ulozeny argument
                            }
                            else {
                                function_argument_add(function, var_name, var_type);
                            }
                            Variable *arg_var = malloc(sizeof(Variable));   //musime alokovat pamat pre kazdu premennu, aby boli pristupne stale
                            mem_store(arg_var);
                            Context *ctx = context_top(context_stack);
                            if (BST_Search(BT_vars, var_name, ctx->label_name) != NULL) {
                                error_semantic("Redefiniton of variable in function definition", *line_number);
                            }
                            arg_var->name = var_name;
                            arg_var->owner = ctx->label_name;
                            arg_var->type = convert_char_to_var_type(var_type);
                            BST_Insert(&BT_vars, arg_var);  //ulozime premennu
                            i += 2; // Posuneme se o dvě, protože už je zpracováme tu
                        }
                        else if (str_match(previous->data, "function")) {      //meno funkce                       
                            char *label_name = instruction_label_function(actual->data);
                            function->name = actual->data;
                            function->label_name = label_name;
                            context_push(context_stack, context_function, label_name);
                            context_push(context_stack, context_return, label_name);  //push return
                        
                        }
                        break;
                    case token_identifier_function:
                        if (str_match(previous->data, "function")) {      //meno funkce                       
                            char *label_name = instruction_label_function(actual->data);
                            function->name = actual->data;
                            function->label_name = label_name;
                            context_push(context_stack, context_function, label_name);
                            context_push(context_stack, context_return, label_name);  //push return
                        
                        }
                        break;
                    
                    case token_var_type:
                        if (state == state_function_return_type) {
                                
                            char *var_type_char = actual->data;
                            Variable_Type var_type = convert_char_to_var_type(var_type_char); 
                            if (!is_new) {
                                if (j != function->arguments_cnt) { //
                                    error_semantic("Function definition (arguments) does not match its prototype (%d vs %d arguments)!", *line_number, j, function->arguments_cnt);
                                }
                                if (var_type != function->return_type) {
                                    error_semantic("Function definition (return type) does not match its prototype !", *line_number);
                                }
                            }
                            else {  //ak funkce nebola deklarovana ulozime ju, inak sme len doplnili info
                                function->return_type = var_type; 
                                function_add(function);
                            }
                        }
                        break;

                    case token_bracket_left:
                        state = state_function_arguments;
                        break;
                    case token_bracket_right:
                        state = state_function_return_type;
                        break;
                    default:
                        
                        break;
                }
                previous = actual;
            }
            if (_DEBUG) {
                //printf("Deklarujeme funkci. Má název %s, return type je %d a počet argumentů %d\n", function->name, function->return_type, function->arguments_cnt);
            }
            return 1;
        }
        
        // return result
        if (str_match(keyword, "return")) {
          
            Context *ctx = context_pop(context_stack);
        
            //nacitame to co je za RETURN
            Token* actual = token_buffer[1];
            //ak mame 3 znaky (RETURN ID EOL) a vraciame premennu ID
            if (actual->type == token_identifier) {

                Variable *var_return;
                char *owner;
                owner = ctx->label_name;
                var_return = BST_Search(BT_vars, actual->data, owner);
                Function *function = function_get_by_label(ctx->label_name);
                if (function == NULL) {
                    error_semantic("Function not found", *line_number);
                }
                
                if (var_return != NULL) {
                    if (var_return->type != function->return_type) {
                        if (var_return->type == Int && function->return_type == Double) {
                            //typova konverze, len prepiseme var_type pred @hodnota
                            //chyyybaju mi na to instrukce, wheere are you ? :(
                        }
                        if (var_return->type == Double && function->return_type == Int) {
                            char* return_conv = convert_double_to_int(var_return->name);
                            instruction_return(NULL, ctx, return_conv);
                            return 1;
                        }
                        error_incompatible_type("Return type does not match with return!", *line_number);
                        return 0;
                    }
                    else {
                        instruction_return(token_buffer[keyword_index + 1]->data, ctx, NULL);
                    }    
                }
                else {
                    error_semantic("Trying to return undeclared variable '%s'", *line_number, actual->data);
                    return 0;
                }
            }
            //RETURN (INTEGER || DOUBLE || STRING) EOL
            else if ((actual->type == token_int) || (actual->type == token_double) || (actual->type == token_string_literal)) {
                Function *function = function_get_by_label(ctx->label_name);
                if (function == NULL) {
                    error_semantic("Function not found", *line_number);
                }
                if ((actual->type == token_int) && (function->return_type != Int)) {    //Integer
                    if (function->return_type != Double) {
                        error_incompatible_type("Return type does not match with return!", *line_number);
                        return 0;
                    }
                    // TODO, potrebujem ale hotove instrukcie :( ASAP
                    //typova konverze - len prepiseme var_type pred @hodnota
                    instruction_return(actual->data, ctx, NULL);
                    return 1;
                }
                if ((actual->type == token_double) && (function->return_type != Double)) {  //Double
                    if (function->return_type != Int) {
                        error_incompatible_type("Return type does not match with return!", *line_number);
                        return 0;
                    }
                    char* return_conv = convert_double_to_int(actual->data);
                    instruction_return(NULL, ctx, return_conv);
                    return 1;
                }
                if ((actual->type == token_string_literal) && (function->return_type != String)) {  //String
                    error_incompatible_type("Return type does not match with return!", *line_number);
                    return 0;
                }
                instruction_return(NULL, ctx, actual->data);
                
            }
            else if (actual->type == token_expression) {
                //sem zatial neviem co dat, musime spravit to vyhodnocovanie vyrazov
            }
        }

        //dim, variable declaration
        if(str_match(keyword, "dim")) {
            Variable *var = malloc(sizeof(Variable));
            mem_store(var);
            var_init(var);
            Token *actual = NULL;
            int i;

            for (i = 1; i < tmp_buff_length; i++) {
                actual = token_buffer[i];
                //printf("Var_type je %d. My teď máme %d: %s\n", token_var_type, actual->type, actual->data);
                if (actual->type == token_identifier) {
                    if (var->name == NULL) {    
                        var->name = actual->data;
                    }
                    else {
                        // Dim i as int = <<<other_identifier>>>
                        if (str_match(actual->data, convert_var_type_to_char(var->type))) {
                            Context *ctx = context_top(context_stack);
                            var->owner = ctx->label_name;
                            //Variable* var_src = BST_Search(BT_vars, var->name, var->owner);
                            //instruction_move(var->name, var_src->value);
                        }
                    }
                } else if (actual->type == token_var_type) {
                    if (str_match(actual->data, "bool")){
                        var->type = Bool;
                    }
                    else if (str_match(actual->data, "integer")) {
                        var->type = Int;
                    }
                    else if (str_match(actual->data, "void")) {
                        var->type = Void;
                    }
                    else if (str_match(actual->data, "float")) {
                        var->type = Double;
                    }
                    else if (str_match(actual->data, "string")) {
                        var->type = String;
                    }
                    else if (str_match(actual->data, "double")) {
                        var->type = Double;
                    }
                } else if (actual->type == token_equals) {
                    if (var->type == String) {
                        if (token_buffer[i+1]->type != token_string_literal) {
                            error_incompatible_type("This is not a STRING", *line_number);
                        }
                    } 
                    if (parse_check_var_type(var,token_buffer[i+1]) ) {
                        error_incompatible_type("Type mismatch in variable declaration !", *line_number);
                        return 0;
                    }
                }
            }
            Context *ctx = context_top(context_stack);
            var->owner = ctx->label_name;
            if (_DEBUG) {
                //printf("Diming variable %s with type %d ", var->name, var->type);
                //printf("%s\n",var->owner);
            }
            if (BST_Search(BT_vars, var->name, var->owner) != NULL) {
                error_semantic("Redefinition of variable", *line_number);
                return 0;
            }
            BST_Insert(&BT_vars, var);
            
        
            if (ctx->context == context_function ||
                ctx->context == context_return) {
                instruction_defvar(var->name, "LF");
            } else {
          
                instruction_defvar(var->name, "LF");
            }
            if (tmp_buff_length > 5) {  // DIM ID AS INT = <- = je 5 token, ak je ich viac je tam aj = hodnota
               /* Token **tokens = malloc(sizeof(Token) * tmp_buff_length);
                mem_store(*tokens);
                int index = 0;
                int assigment_index = parse_find_token(token_buffer, tmp_buff_length, token_equals);
                for(int i = assigment_index + 1; i < tmp_buff_length; i++) {
                    tokens[index] = token_buffer[i];
                    index++;
                }
                
                char* result = expr_instructions(tokens, index);
                if (BST_Search(BT_vars, result, context_top(context_stack)->label_name) != NULL) {
                    char *L_var = calloc(15, sizeof(char));
                    strcpy(L_var, result);
                    
                    instruction_move(var->name, "LF", result);
                }
                else {
                    instruction_move(var->name, convert_value_to_var_type(result), result);
                }

                return 0;*/

                
            }else {
                if (var->type == Int) instruction_move(var->name, "int", "0");
                if (var->type == Double) instruction_move(var->name, "float", "0.0");
                if (var->type == String) instruction_move(var->name, "string", "");
                return 0;
            }
        }

    }
    
    // any function call
    if ((keyword_index = parse_find_token(token_buffer, tmp_buff_length, token_identifier_function)) != -1) {
       
        //if ((tmp_buff_length > keyword_index + 1) && token_buffer[keyword_index + 1]->type == token_bracket_left) {
        // function call
        char *function_name = token_buffer[keyword_index]->data;
        Function *function = malloc(sizeof(Function));
        mem_store(function);
        
        function = function_get(function_name);
        if (function == NULL) {
            error_semantic("Unknown function called!", *line_number);
        }
        else {
            for(int i = 0; i < tmp_buff_length; i++) {
                 //printf("'%d': %s, %d\n", i, token_buffer[i]->data, token_buffer[i]->type);
            }
            //printf("function name is '%s' on index %d. length is %d\n", function_name, keyword_index, tmp_buff_length);
            
            Function_Arguments *arguments = parse_function_arguments(token_buffer, tmp_buff_length, keyword_index);
            //function_call(function_name, arguments);
            if (arguments == NULL) {
                return 1;
            }
            
            
            if(function->arguments_cnt != arguments->arguments_cnt) {       //check ci sedi pocet argumentov
                error_incompatible_type("Bad arg count!", *line_number);
            } else {
                
                for (int i = 0; i < arguments->arguments_cnt; i++) {
                    if (arguments->arr[i].var_type != function->arguments->arr[i].var_type) {
                        //printf("%p %p\n", arguments->arr[i], function->arguments->arr[i]);
                        //printf("ARGS_cmp %s\n",arguments->arr[i].value);
                        if (arguments->arr[i].var_type == Int && function->arguments->arr[i].var_type == Double) {
                            continue;
                        } else if (arguments->arr[i].var_type == Double && function->arguments->arr[i].var_type == Int) {
                            arguments->arr[i].value = convert_double_to_int(arguments->arr[i].value);
                            continue;
                        }

                        Variable_Type arg_type_func = function->arguments->arr[i].var_type;
                        Variable_Type arg_type_given = arguments->arr[i].var_type;
                        error_incompatible_type("Argument type in function %s do not match (argument index %d) (%s vs %s)", *line_number, function->name, i, convert_variable_type_to_string(arg_type_func), convert_variable_type_to_string(arg_type_given));
                    }
                }

                Context *ctx = context_top(context_stack);
                instruction_call(function_name, arguments, ctx);                 //vsetko OK zapiseme volanie
                
                
            }
        }
           // free(function);
        //}
    }
    
    
    int assigment_index = -1;
    if ((assigment_index = parse_find_token(token_buffer, tmp_buff_length, token_equals)) != -1) {
        
        if (!parse_check_brackets_cnt(token_buffer, tmp_buff_length)) {
            error_syntactic("Bracket count doesn't match on line %d", *line_number);
        }
        
        // dim i as >>int<<
        char *result_var = token_buffer[assigment_index - 1]->data;
        if (check_var_type_exists(result_var) && tmp_buff_length < 7) {
            // dim >>i<< as int
           result_var = token_buffer[assigment_index - 3]->data;
           instruction_move(result_var, token_type_to_char(token_buffer[assigment_index + 1]) ,token_buffer[assigment_index + 1]->data); 
        }
        else {
            if ((token_buffer[assigment_index + 1]->type == token_identifier_function ||  
                token_buffer[assigment_index + 1]->type == token_identifier) &&
                token_buffer[assigment_index + 2]->type == token_bracket_left) {
                Function* function = function_get(token_buffer[assigment_index + 1]->data);
                if (function == NULL) {
                    error_semantic("Function not declared", *line_number);
                }/*
                char* var_char = token_type_to_char(token_buffer[assigment_index - 1]);
                if (str_match(var_char, "int")) {
                    var_char = calloc(8, sizeof(char));
                    strcpy(var_char,"integer");
                }
                Variable_Type var_type = convert_char_to_var_type(var_char);
                if (function->return_type != var_type) {
                    if (function->return_type == Int && var_type == Double) {
                        //@@@@TODO@@@@
                    }
                    else if (function->return_type == Double && var_type == Int) {
                        //@@@@TODO@@@@
                    }
                    else {
                        error_incompatible_type("Type mismatch in id assign!", *line_number);
                    }
                }*/
                instruction_pops(result_var, "LF");
            } else if ((token_buffer[assigment_index + 1]->type == token_string_literal || 
                       token_buffer[assigment_index + 1]->type == token_int || 
                       token_buffer[assigment_index + 1]->type == token_double || 
                       token_buffer[assigment_index + 1]->type == token_identifier) && tmp_buff_length < 5) {
                Variable* var;
                if ((var = BST_Search(BT_vars, token_buffer[assigment_index - 1]->data, context_top(context_stack)->label_name)) == NULL) {
                    error_semantic("Variable not declared", *line_number);
                }
                char* type_char = token_type_to_char(token_buffer[assigment_index + 1]);
                if (str_match(type_char, "int")) {
                    type_char = calloc(8, sizeof(char));
                    strcpy(type_char,"integer");
                }
                Variable_Type var_type = convert_char_to_var_type(type_char); //token_type to var_type
                if (var->type != var_type) {
                    if (var->type == Int && var_type == Double) {
                        instruction_move(token_buffer[assigment_index - 1]->data, "int" ,convert_double_to_int(token_buffer[assigment_index + 1]->data));
                    }
                    else if (var->type == Double && var_type == Int) {
                        instruction_move(token_buffer[assigment_index - 1]->data, "float" ,token_buffer[assigment_index + 1]->data);
                    }
                    else {
                        error_incompatible_type("Type mismatch in id assign!", *line_number);
                    }
                }     
                instruction_move(token_buffer[assigment_index - 1]->data, token_type_to_char(token_buffer[assigment_index + 1]) ,token_buffer[assigment_index + 1]->data);
            } else {
                Token **tokens = malloc(sizeof(Token) * tmp_buff_length);
                mem_store(*tokens);
                int index = 0;
                Token* actual = NULL;
                for(int i = assigment_index + 1; i < tmp_buff_length; i++) {
                    actual =  token_buffer[i];
                    if (actual->type == token_identifier || actual->type == token_int || actual->type == token_double || is_expr_part(actual)) {
                        tokens[index++] = token_buffer[i];
                    }
                }
                //printf("\nResult:\n");
                expr_eval(tokens, index);
                char* result = expr_instructions(tokens, index);
                //infix_to_postfix(tokens, index);
                if (check_var_type_exists(result_var)) {
                    // dim >>i<< as int
                    result_var = token_buffer[assigment_index - 3]->data;
                }
                instruction_move(result_var, "LF", result);
 
            }
            /*char *result = "42"; //do_math(tokens, index);
            printf("Result: %s\n", result);
            instruction_move(result_var, result, "");*/
        }
    }
    
    if (tmp_buff_length && token_buffer[tmp_buff_length - 1]->type == token_eof) {
        return 0;
    } else {
        return 1;
    }

}

void parse_file() {
    /* if (file == NULL) {
        printf("Error opening source file!\n");
        return;
    }*/
    instruction_header();
    int *line_number = malloc(sizeof(int));
    mem_store(line_number);
    *line_number = 1;
    BST_Init(&BT_vars);
    context_stack = malloc(sizeof(Context_Stack) * 5);
    mem_store(context_stack);
    context_stack->max = 5;
    context_stack_init(context_stack);
    parse_prog(line_number);
    free(line_number);
    BST_Dispose(&BT_vars);
}
