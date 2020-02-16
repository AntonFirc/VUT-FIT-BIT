/**
 * Precedencna syntakticka analyza
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack_token.h"
#include "tokens.h"
#include "parser.h"
#include "instructions.h"
#include "errors.h"
#include "memory_stack.h"
#include "exprs.h"
#include "expr_list.h"

#define SIZE 15
int if_count = 0;

char precedence_table[SIZE][SIZE] = {
/* token    +    -    *    \    /    (    )    =   <>    <    >   <=   >=    $    i
                                                                                          stack */
          {'>', '>', '<', '<', '<', '<', '>', '>', '>', '>', '>', '<', '>', '>', '<'}, /*   +   */ 
          {'>', '>', '<', '<', '<', '<', '>', '>', '>', '>', '>', '<', '>', '>', '<'}, /*   -   */ 
          {'>', '>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'}, /*   *   */ 
          {'>', '>', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'}, /*   \   */ 
          {'>', '>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'}, /*   /   */ 
          {'<', '<', '<', '<', '<', '<', '=', '<', '<', '<', '<', '<', '<', '>', '<'}, /*   (   */ 
          {'>', '>', '>', '>', '>', ' ', '>', '>', '>', '>', '>', '>', '>', ' ', ' '}, /*   )   */ 
          {'<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'}, /*   =   */ 
          {'<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'}, /*   <>  */ 
          {'<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'}, /*   <   */ 
          {'<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'}, /*   >   */ 
          {'<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'}, /*   <=  */ 
          {'<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<'}, /*   >=  */ 
          {'<', '<', '<', '<', '<', '<', ' ', '<', '<', '<', '<', '<', '<', ' ', '<'}, /*   $   */
          {'>', '>', '>', '>', '>', ' ', '>', '>', '>', '>', '>', '>', '>', '>', ' '}, /*   i  */  
};

Token * find_terminal(Token_Stack *stack) {
    Token_Stack *tmp_stack = malloc(sizeof(Token_Stack));
    token_stack_init(tmp_stack);

    Token *terminal = token_init();
    Token *top = token_init();
    int found = 0;
    
    while( !token_is_empty(stack) && found == 0 ) {
        top = token_top(stack);

        if( top->type == token_plus           ||
            top->type == token_minus          ||
            top->type == token_multiply       ||
            top->type == token_divide         ||
            top->type == token_divide_double  || 
            top->type == token_bracket_left   ||
            top->type == token_bracket_right  ||
            top->type == token_equals         ||
            top->type == token_not_equal      ||
            top->type == token_less           ||
            top->type == token_more           ||
            top->type == token_less           || 
            top->type == token_less_or_equal  ||
            top->type == token_more_or_equal  ||
            top->type == token_eof            ||
            top->type == token_identifier     || 
            top->type == token_int            ||
            top->type == token_double         ||
            top->type == token_string_literal  ) 
        {
            terminal = token_top(stack);
            found = 1;
        }

        else {
            token_push(tmp_stack, token_pop(stack));
        }
    }

    for(int i=0; i<(tmp_stack->top)+1; i++) {
        token_push(stack, token_pop(tmp_stack));
    }

    return terminal;
}

int token_type_2_row(Token *token) {
    if(token->type == token_plus) return 0;
    if(token->type == token_minus) return 1;
    if(token->type == token_multiply) return 2;
    if(token->type == token_divide) return 3;
    if(token->type == token_divide_double) return 4;
    if(token->type == token_bracket_left) return 5;
    if(token->type == token_bracket_right) return 6;
    if(token->type == token_equals) return 7;
    if(token->type == token_not_equal) return 8;
    if(token->type == token_less) return 9;
    if(token->type == token_more) return 10;
    if(token->type == token_less_or_equal) return 11;
    if(token->type == token_more_or_equal) return 12;
    if(token->type == token_eof) return 13;

    if(token->type == token_identifier     || 
       token->type == token_int            ||
       token->type == token_double         ||
       token->type == token_string_literal  ) 
    {
        return 14;
    } 
    return -1;
}

// najde pravidlo v tabulke
char find_in_table(char table[SIZE][SIZE], int row, int col) {
    return table[row][col];
}

// zredukuje vyraz podla pravidla
void reduce_expr(Token_Stack *stack) {
    if (stack->top != 2 && stack->top != 0) {
        error_syntactic("Wrong expression syntax!",0);
    }
    Token *top = token_init();
    top = token_pop(stack);
    Token *non_terminal = malloc(sizeof(Token));
    non_terminal->type = token_non_terminal;
    non_terminal->length = 0;
    token_data(non_terminal, "E");

    State_expr state = state_begin;

    while(state != state_end) {
        switch(state) {
            // begin
            case state_begin:
                switch(top->type) {
                    // 'i'
                    case token_identifier:
                    case token_int:
                    case token_double:
                    case token_string_literal:
                    case token_non_terminal:
                        state = state_operand;
                        if( stack->top == -1 ) {
                            state = state_push;
                        }
                        else {
                            top = token_pop(stack);
                        }
                        break; 

                    // '('
                    case token_bracket_left:
                        state = state_bracket_left;
                        if( stack->top == -1 ) {
                            error_internal("Error in reduction of expression!", 0);
                        }
                        else {
                            top = token_pop(stack);
                        }
                        break;

                    // inak
                    default:
                        error_internal("Error in reduction of expression!", 0);
                }
                break; // skok na switch(state)

            // operand
            case state_operand:
                switch(top->type) {
                    // operator
                    case token_plus:
                    case token_minus:
                    case token_multiply:
                    case token_divide:
                    case token_divide_double:
                        state = state_num_operator;
                        top = token_pop(stack);
                        break;

                    // comparator
                    case token_equals:
                    case token_not_equal:
                    case token_less:
                    case token_less_or_equal:
                    case token_more:
                    case token_more_or_equal:
                        state = state_cmp_operator;
                        if_count++; // IF statement counter
                        top = token_pop(stack);
                        break;

                    // inak
                    default:
                        error_internal("Error in reduction of expression!", 0);
                }
                break; // skok na switch(state)

            // operator
            case state_num_operator:
            case state_cmp_operator:
                switch(top->type) {
                    // 'i'
                    case token_identifier:
                    case token_int:
                    case token_double:
                    case token_string_literal:
                    case token_non_terminal:
                        state = state_push;
                        break; 

                    // inak
                    default:
                        error_internal("Error in reduction of expression!", 0);
                }
                break; // skok na switch(state)

            // bracket left - '('
            case state_bracket_left:
                switch(top->type) {
                    // 'E'
                    case token_non_terminal:
                        state = state_bracket_right;
                        top = token_pop(stack);
                        break;
                    
                    // inak
                    default:
                        error_internal("Error in reduction of expression!", 0);
                }
                break; // skok na switch(state)
            
            // bracket right - ')'
            case state_bracket_right:
                switch(top->type) {
                    // ')'
                    case token_bracket_right:
                        state = state_push;
                        break;
                    
                    // inak
                    default:
                        error_internal("Error in reduction of expression!", 0);
                }
                break; // skok na switch(state)

            // push
            case state_push:
                if(token_is_empty(stack)) {
                    state = state_end;
                    token_push(stack, non_terminal); // push non_terminal to stack
                    break;
                }
                else {
                    error_internal("Error in reduction of expression!", 0);
                }
            break; // skok na switch(state)

            // end
            case state_end:
                break;
            break; // skok na switch(state)

        } // switch(state)
    } // while

    if(if_count > 1) {
        error_internal("More than one comparation in expression!", 0);
        if_count = 0;
    }
}

// vyhodnoti, ci je vyraz syntakticky spravne
int expr_eval(Token *tokens[], int index) {
    Token_Stack *expr_stack = malloc(sizeof(Token_Stack));
    Token_Stack *temp_stack = malloc(sizeof(Token_Stack));
    mem_store(expr_stack);
    mem_store(temp_stack);

    token_stack_init(expr_stack);
    token_stack_init(temp_stack);

    Token dollar = {19, 0, "$"};
    Token handle = {7, 0, "<"};
    token_push(expr_stack, &dollar); 
    int cycle = 1;
    int i=0;

    while(cycle) {
        if(i >= index) {
            if( expr_stack->top == 0 ) {
                if(if_count == 0) {
                    return 1; // vyraz je v poriadku a NEobsahuje komparacne operatory
                }
                else {
                    return 2; // vyraz je v poriadku a OBsahuje komparacne operatory
                }
            }

            while( !(token_top(expr_stack) == &handle) && !(token_top(expr_stack) == &dollar) ) {
                    token_push(temp_stack, token_pop(expr_stack));
            }

            if(temp_stack->top != 2 && temp_stack->top != 0) {
                return 0; // syntakticka chyba -> vracia 0
            }
            
            token_pop(expr_stack); // pop '<'
            reduce_expr(temp_stack); 
            if(!token_is_empty(temp_stack)) {
                token_push(expr_stack, token_pop(temp_stack));
            }
        }

        else {
            Token *term = find_terminal(expr_stack);
            int row = token_type_2_row(term);
            int col = token_type_2_row(tokens[i]);
            char table_rule = find_in_table(precedence_table, row, col);
            
            // ak je v tabulke prazdne policko -> chyba
            if(table_rule == ' ') {
                error_internal("Invalid expression!", 0);
            }

            // ak je v tabulke '<' -> push('<') a push(aktualny_znak_na_vstupe)
            else if(table_rule == '<') {
                //vyberie neterminaly
                while( token_top(expr_stack) != term ) {
                    if(token_top(expr_stack)->type == token_non_terminal) {
                        token_push(temp_stack, token_pop(expr_stack));
                    }
                }
                //push handle
                token_push(expr_stack, &handle);
                //vrati spat
                while(!token_is_empty(temp_stack)) {
                    token_push(expr_stack, token_pop(temp_stack));
                }
                token_push(expr_stack, tokens[i]);
                i++;
            }

            // ak je v tabulke '>' -> redukuje po '<'
            else if(table_rule == '>') {
                while( !(token_top(expr_stack) == &handle) ) {
                    token_push(temp_stack, token_pop(expr_stack));
                }
                token_pop(expr_stack); // pop '<'
                reduce_expr(temp_stack); 
                if(!token_is_empty(temp_stack)) {
                    token_push(expr_stack, token_pop(temp_stack));
                }
            }

            // ak je v tabulke '=' -> push(aktualny_znak_na_vstupe)
            else if(table_rule == '=') {
                token_push(expr_stack, tokens[i]);
                i++;
                continue;
            }
        }

        // podmienky cyklu
        if( !(token_is_empty(expr_stack)) || i<index ) {
            cycle = 1;
        }  

        else {
            cycle = 0;
        }
    }
    
    return 0;
}

/*
 * Zapis vyrazu na instrukce
 */
char* expr_instructions(Token** tokens, int index) {
    infix_to_postfix(tokens, index);
    while (1) {
        Token* actual = list_act();
        if (actual->type == token_plus ||
            actual->type == token_minus ||
            actual->type == token_multiply ||
            actual->type == token_divide ||
            actual->type == token_divide_double
        ) {
                Token* right = list_remove_pred();
                Token* left = list_remove_pred();
                strcpy(actual->data, instruction_expr(left, right, actual)); //zavola zapis expression, a rovno ulozi nazov tmp premennej
                actual->type = token_identifier;
                list_insert_act(actual);
        }
        if (list_act_n()) {
            char* result_var = calloc(15, sizeof(char));
            mem_store(result_var);
            strcpy(result_var, list_act()->data);
            list_dispose();
            return result_var;
        }
    }
    
}
