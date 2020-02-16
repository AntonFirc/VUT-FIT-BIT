/**
 * Tokenizer 
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 **/

#include "tokens.h"

#include "errors.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "helpers.h"
#include "parser.h"
#include "definitions.h"
#include "memory_stack.h"

Token * token_init() {
    Token *token = malloc(sizeof(Token));
    mem_store(token);
    token->type = token_invalid;
    return token;
}

void token_data_tolower(Token* token){
    for (int prestiz = 0; prestiz < strlen(token->data); prestiz++) {
        token->data[prestiz] = tolower(token->data[prestiz]);
    }
}

void token_data(Token *token, char *data) {
    strcpy(token->data, data);
    strcat(token->data, "\0");
}

int *ln = NULL;

char _getchar() {
    char c = fgetc(stdin);
    //printf("%c\n", c);
    if (c == '\n') {
       // printf("Line number++%d\n\n\n", *ln);
        (*ln)++;
    }
    return c;
}

void _ungetchar(char c) {
    ungetc(c, stdin);
    if (c == '\n') {
        (*ln)--;
    }
}


Token * token_get(int *line_number_ptr) {
    ln = line_number_ptr;
    Token *token = token_init();
    State state = state_new;
    int line_number = *line_number_ptr;

    int alloc_size = 128;
    char *buffer = calloc(alloc_size, sizeof(char));
    mem_store(buffer);
    
    int buffer_length = 0;
    
    // Musíme pokračovat, dokud explicitně ve while nevrátíme nějaký token
    while (1) {
        // tolower - musíme změnit, protože např. v string literálu bychom neměli tolower používat
        if (alloc_size == buffer_length) {
            alloc_size += 128;
            buffer = realloc(buffer, alloc_size);
        }
        int c = _getchar();
        while ((c == ' ' || !isprint(c)) && state == state_new && c != '\n') {   //c == 13 -> CR znak
            c = _getchar();
        }
        switch (state) {
            case state_new:
                switch(c) {
                    case ';': token->type = token_semicolon; break;
                    case '+': token->type = token_plus; break;
                    case '-': token->type = token_minus; break;
                    case '*': token->type = token_multiply; break;
                    case '/':
                        // /' je začátek (víceřádkového) komentáře
                        if(1) {
                        }
                        int ch;
                        if ((ch = _getchar()) == '\'') {
                            while (1) {
                                //if ((ch = _getchar()) == '\n') {
                                //    ++(*line_number_ptr);
                                //    ++line_number;
                                //}
                                if ((ch = _getchar()) == '\'') {
                                    if ((ch = _getchar()) == '/') {
                                        state = state_new;
                                        break;
                                    }
                                }
                            }
                        } else {
                            // vrátíme pointer souboru zase o jeden zpět
                            _ungetchar(ch);
                            token->type = token_divide_double;
                        }
                    break;
                    case ',': token->type = token_comma; break;
                    case '(': token->type = token_bracket_left; break;
                    case ')': token->type = token_bracket_right; break;
                    case '\'':
                        // '/ je konec (víceřádkového) komentáře
                        if ((_getchar()) == '/') {
                            c = '/';
                            token->type = token_comment_end;
                            return token;
                        } else {
                            while((c = _getchar()) != '\n') {
                                
                            }
                            // Chceme zachovat nový řádek
                            token->type = token_eol;
                            //(*line_number_ptr)++;
                            return token;
                        }
                    break;
                    case '\\': token->type = token_divide; break;
                    case '=': token->type = token_equals; break;
                    case '<': 
                        if(1) {}
                        int ch1;
                        if ((ch1 =_getchar()) == '=') {
                            token->type = token_less_or_equal;
                            return token;
                        } else if (ch1 == '>') {
                            token->type = token_not_equal;
                        } else {
                            ungetc(ch1, stdin);
                            token->type = token_less;
                            return token;
                        }
                        break;
                    case '>':
                        if(1) {}
                        int ch2;
                        if ((ch2 = _getchar()) == '=') {
                            token->type = token_more_or_equal;
                            return token;
                        } else {
                            ungetc(ch2, stdin);
                            token->type = token_more;
                            return token;
                        }
                        break;    
                    case '\n': token->type = token_eol; /*(*line_number_ptr)++; */break;
                    case EOF: token->type = token_eof; break;
                    case '!':
                        c = _getchar();
                        if (c != '"') {
                            error_lexical("String literal must have the correct form: !\"STRING\"", line_number);
                        }
                        state = state_string_literal;
                    break;
                    default:
                        buffer[buffer_length++] = c;
                        if (isdigit(c)) {
                            state = state_digit;
                        } else {
                            state = state_keyword;
                        }
                        
                        continue;
                    break;
                }
                if (token->type != token_invalid) {
                    buffer[buffer_length++] = c;
                    token_data(token, buffer);
                    return token;
                }
            break;
            case state_expression:
                // Vynecháme všechny mezery
            break;
            case state_digit:
                if (isdigit(c)) {
                    buffer[buffer_length++] = c;
                } else if (c == 'e' || c == 'E') {
                    token->type = token_double;
                    buffer[buffer_length++] = c;
                    state = state_double_exp;
                    // exponent
                } else if (c == '.') {
                    state = state_double_dec;
                    token->type = token_double;
                    buffer[buffer_length++] = c;
                }
                else {
                    _ungetchar(c);
                    // jen v případě že jsme token typ ještě nezměnili (třeba na float)
                    if (token->type == token_invalid) {
                            token->type = token_int;
                    }
                    
                    token_data(token, buffer);
                    return token;
                }
            break;
            case state_double_exp:
                if (c == '+' || c == '-') {
                    buffer[buffer_length++] = c;
                    c = _getchar();
                }
                buffer[buffer_length++] = c;
                while (isdigit(c = _getchar())) {
                    buffer[buffer_length++] = c;
                }
                if (isalpha(c) || c == '\n' || c == 13) {     //13 -> CR znak
                    _ungetchar(c);                   
                    token_data(token, buffer);
                    return token;
                }
                else if (c == ' ') {

                }
                else {
                   // printf("%d, %c\n", c, c);
                    error_lexical("Wrong double  format!", line_number);
                }
            break;
            case state_double_dec:
                if (isdigit(c)) {
                    buffer[buffer_length++] = c;
                } else if (c == 'e' || c == 'E') {
                    buffer[buffer_length++] = c;
                    state = state_double_exp;
                    token->type = token_double;
                    // exponent
                }
                else if (c == ' ') {

                }
                else if (!isalpha(c) && c != '\n' && c != 13 && c != ';') {
                    //printf("Tu sme %d, %c\n", c, c);
                    error_lexical("Wrong double  format!", line_number);
                }
                else {
                    _ungetchar(c);
                    token_data(token, buffer);
                    return token;
                }
            break;
            case state_keyword:
                token->type = token_keyword;
               
                if (isalpha(c) || c == '_') {
                    buffer[buffer_length++] = tolower(c);
                } else if (isdigit(c)) {
                    buffer[buffer_length++] = c;
                } else {
                    /*if (c == '(') {
                        // function call
                        token->type = token_function_call;
                        
                        token_data(token, buffer);
                        return token;
                    }
                    */
                    _ungetchar(c);
                    token_data(token, buffer);
                    
                    if (check_var_type_exists(buffer)) {
                        token->type = token_var_type;
                    } else if (check_keyword_exists(buffer) == 1) {
                        token->type = token_keyword;
                        if (is_reserved_function(buffer)) {
                            token->type = token_identifier_function;
                            token_data_tolower(token);
                        }
                    } else if (function_get(buffer) != NULL) {
                        token->type = token_identifier_function;
                        token_data_tolower(token);
                    } else {
                        token->type = token_identifier;
                        token_data_tolower(token);
                    }
                    return token;
                }
                
            break;
            case state_string_literal:
                // ending string literal
                if (c == ' ') {
                    strcat(buffer, convert_char_to_sequence(32));
                    buffer_length += 4; //pridali sme 4 znaky \000 
                }
                else if (c == '\\') {
                    c = _getchar();
                    if (c == 'n') {
                        strcat(buffer, convert_char_to_sequence(10));
                        buffer_length += 4; //pridali sme 4 znaky \000 
                        break;
                    }
                    if (c == 't') {
                        strcat(buffer, convert_char_to_sequence(9));
                        buffer_length += 4; //pridali sme 4 znaky \000 
                        break;
                    }
                    if (c == '\\') {
                        strcat(buffer, convert_char_to_sequence(92));
                        buffer_length += 4; //pridali sme 4 znaky \000 
                        break;
                    }
                    char buff_escape[] = "\0\0\0\0";
                    int idx_escape = 0;
                    while (isdigit(c)) {
                        buff_escape[idx_escape++] = c;
                        c = _getchar();
                    }
                    if (idx_escape != 0) {
                        ungetc(c,stdin);
                        buffer[buffer_length++] = '\\';
                        strcat(buffer, buff_escape);
                        buffer_length += 3; //pridali sme 3 znaky miesto escape sekvence
                        break;
                    }
                    buffer[buffer_length++] = c;
                    break;
                }
                else if (c == '"') {
                    token->type = token_string_literal;
                    token_data(token, buffer);
                    return token;
                } else if (c == '\n'){
                    error_lexical("String literal must have the correct form: !\"STRING\"", line_number);
                } else {
                        buffer[buffer_length++] = c;
                }
            break;
            case state_var:
                if (isdigit(c)) {
                    token->type = token_int;
                    token_data(token, buffer);
                    
                    return token;
                } else if (c == '+') {
                    token->type = token_plus;
                    token_data(token, buffer);

                    return token;
                }
            break;
         
            default:
                error_internal("Lexical analyzer failure", line_number);             
            
        }
    }
}
