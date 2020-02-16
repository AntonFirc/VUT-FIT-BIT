/**
 * Helpers
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 */

#include "helpers.h"

#include "definitions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "errors.h"
#include "tokens.h"
#include <ctype.h>
#include "helpers.h"
#include "parser.h"
#include "memory_stack.h"
#include "symtable.h"
#include "stack_token.h"
#include "expr_list.h"

extern Context_Stack *context_stack;

char *Variable_Type_String[] = {
   "None", "Int", "Double", "String", "Bool", "Void",
};


Function **functions;
int functions_cnt = 0;

Variable **variables;
int variables_count = 0;

int is_expr_part_cmp(Token* token) {
    return ((token->type == token_equals) || (token->type == token_not_equal) || (token->type == token_more) || 
        (token->type == token_more_or_equal) || (token->type == token_less) || (token->type == token_less_or_equal) ||
        (token->type == token_plus) || (token->type == token_minus) || (token->type == token_multiply) || 
        (token->type == token_divide) || (token->type == token_divide_double) || (token->type == token_bracket_left) ||
        (token->type == token_bracket_right) );
}

int is_cmp_char(Token* token) {
    return ((token->type == token_equals) || (token->type == token_not_equal) || (token->type == token_more) || 
        (token->type == token_more_or_equal) || (token->type == token_less) || (token->type == token_less_or_equal));
}

int is_expr_part(Token* token) {
    return ((token->type == token_plus) || (token->type == token_minus) || (token->type == token_multiply) || 
        (token->type == token_divide) || (token->type == token_divide_double) || (token->type == token_bracket_left) ||
        (token->type == token_bracket_right) );
}

/**
 * Přidá funkci do seznamu funkcí
 *//*
void function_add(Function *function) {
    int index = functions_cnt;
    if (functions_cnt == 0) {
        functions = malloc(sizeof(Function) * (++functions_cnt));
    } else {
        functions = realloc(functions, sizeof(Function) * (++functions_cnt));
    }

    functions[index] = function;
}
*/

/**
 * Inicializuje paměť pro funkci
 *//*
void function_init(Function *function) {
    function->arguments_cnt = 0;
    function->arguments = malloc(sizeof(Function_Arguments));
    function->arguments->arr = malloc(sizeof(Function_Argument));
    function->name = malloc(sizeof(char *) * 128);
    function->return_type = Void;
}
*/
int check_var_type_exists(char *type) {
    return str_match("bool", type) || str_match("integer", type) ||
        str_match("void", type) || str_match("string", type) || str_match("double", type);
}

char* convert_char_to_sequence(int chr) {
    char *sequence = calloc(sizeof(char), 4);
    sequence[0] = '\\';
    char buff[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    sequence[3] = buff[chr % 10];
    chr = chr / 10;
    sequence[2] = buff[chr % 10];
    chr = chr / 10;
    sequence[1] = buff[chr % 10];
    return sequence; 
}

/*
 * kontrola ci je spravne zapisany integer a double
 */
int parse_check_var_type(Variable *var, Token *token) {

    Variable_Type token_type = convert_char_to_var_type(token_type_to_char(token));

    if (var->type != token_type) {
        if (var->type == Int && token_type == Double) {
            token_type = token_int;
            strcpy(token->data, convert_double_to_int(token->data));
            return 0;
        }
        else if (var->type == Double && token_type == Int) {
            return 0;
        }
        return 1;
    }
    return 0;

    
}

/**
 * Přidá argument do funkce
 */
void function_argument_add(Function *function, char *var_name, char *var_type_char) {
    Variable_Type var_type = convert_char_to_var_type(var_type_char);
    for (int i = 0; i < function->arguments_cnt; i++) {
        if (str_match(function->arguments->arr[i].var_name, var_name)) {
            return;
        }
    }
    Function_Argument *argument = malloc(sizeof(Function_Argument));
    mem_store(argument);
    argument->var_name = var_name;
    argument->var_type = var_type;
    int new_size = ++function->arguments_cnt;
    //printf("zvetšíme argument length na %d\n", function->arguments_cnt);
    
    // Zvětšíme paměť, aby se nám tam argument vešel
    if (_DEBUG) {
        //printf("new_size = %d\n",new_size);
    }
    function->arguments->arr = realloc(function->arguments->arr, sizeof(Function_Argument) * new_size);
    function->arguments->arr[new_size - 1] = *argument;
    
    
}

char* convert_value_to_var_type(char* value) {
    for (int i = 0; i < strlen(value); i++) {
        if (isdigit(value[i])) {
            continue;
        }
        else if ((value[i] == '.' || value[i] == 'E' || value[i] == 'e') && (i != 0)) {
            return "float";
        }
        else {
            return "string";
        }
    }
    return "int";
}

char *convert_variable_type_to_string(Variable_Type var_type) {
    return Variable_Type_String[var_type];
}

char* convert_double_to_int(char* dbl_in) {

    int i = 0;
    char* int_out = calloc(11, sizeof(char)); //musi stacit pre rozsah integer
    mem_store(int_out);
    //opakuj kym nenajdes oddelovac celej casti alebo nenacitas cely retazec (to by tam nemuselo byt inak by to bol int ?)
    while((i < strlen(dbl_in)) && (dbl_in[i] != '.' && dbl_in[i] != 'E' && dbl_in[i] != 'e') ) {
        int_out[i] = dbl_in[i];
        ++i;
        if (i == 11) break;
    }
    return int_out;
}


/**
 * Zkonvertuje typ z řetězce do Variable_Type
 * @return Variable_Type
 */
Variable_Type convert_char_to_var_type (char* var_type) {
    if (str_match(var_type, "bool")) {
        return Bool;
    }
    if (str_match(var_type, "double")) {
        return Double;
    }
    if (str_match(var_type, "float")) {
        return Double;
    }
    if (str_match(var_type, "string")) {
        return String;
    }
    if (str_match(var_type, "integer") || str_match(var_type, "int")) {
        return Int;
    }
    return None;
}

char* token_type_to_char(Token* token) {
    if (token->type == token_int) {
        return "int";
    }
    if (token->type == token_double) {
        return "float";
    }
    if (token->type == token_string_literal) {
        return "string";
    }
    
    return NULL;
}

char* convert_var_type_to_char(Variable_Type var_type) {
    if (var_type == Bool) {
        return "bool";
    }
    if (var_type == Double) {
        return "float";
    }
    if (var_type == String) {
        return "string";
    }
    if (var_type == Int) {
        return "int";
    }
    return NULL;
}

void function_call(char *function_name, Function_Argument *arguments) {
    
}



/**
 * Hezčí verze funkce strcmp
 */
int str_equal(char *str1, char *str2) {
    return strcasecmp(str1, str2) == 0;
}

int str_match(char *str1, char *str2) {
    return str_equal(str1, str2);
}

int is_bracket(char c) {
    return c == '(' || c == ')';
}

int is_digit(char c) {
    return isdigit(c);
}

int is_dot(char c) {
    return c == '.';
}

void var_init(Variable *var) {
    var->name = NULL;
    var->owner = NULL;
    var->type = None;
    var->value = NULL;
}

/**
 * @author Tono
 * @todo for Tono
 */
char *var_get_type(char *var_name) {
    Context* ctx = context_top(context_stack);
    char* owner = ctx->label_name;
    Variable* var = BST_Search(BT_vars, var_name, owner);
    char *type = calloc(sizeof(char), 7); //7 pretoze String je najdlhsi -> 6znakov + \0
    mem_store(type);
    strcpy(type, convert_var_type_to_char(var->type));
    return type;
}

Variable *var_find(char *var_name) {
    for(int i = 0; i < variables_count; i++) {
        if (variables[i]->name == var_name) {
            return variables[i];
        }
    }
    return NULL;
}

Variable *var_set_int(char *var_name, int value) {
    return NULL;
}

Variable *var_get_int(char *var_name) {
   // Variable *var = var_find(var_name);
    return NULL;
}

Variable *var_get_float(char *var_name) {
    return NULL;
}

Variable *var_get_string(char *var_name) {
    
    return NULL;
}



/**
 * Zjistí, zda daný keyword existuje (pouze identifiery)
 * @return int
 */
int check_keyword_exists(char *keyword) {
    if (_DEBUG) {
        //printf("Checking if keyword %s exists\n", keyword);
    }
    //sizeof(RESERVED_KEYWORDS_IDENTIFIERS)
    int size = RESERVED_KEYWORDS_IDENTIFIERS_SIZE;
    for(int i = 0; i < size; i++) {
        if (str_match(RESERVED_KEYWORDS_IDENTIFIERS[i], keyword)) {
            return 1;
        }
    }
    return 0;
}

int is_reserved_function(char* name) {
    return (str_match(name, "asc") || str_match(name, "chr") || str_match(name, "length") || str_match(name, "substr"));
}

int is_sign(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

int evaluate_expression(char *expression) {
    return 1;
}

//----------|
// DO_MATH  |
//----------|
int prec(Token *sign) {
    switch (sign->type) {
        case token_plus:
        case token_minus:
            return 1;
        
        case token_divide:
            return 2;

        case token_multiply:
        case token_divide_double:
            return 3;
        
        default:
            return -1;
    }
    return -1;
}

void infix_to_postfix(Token **tokens, int index) {
    int i;
    Token_Stack *stack = malloc(sizeof(Token_Stack));
    mem_store(stack);
    token_stack_init(stack);

    for(i=0; i < index; ++i) {
        // ak je na vstupe OPERAND
        if (tokens[i]->type == token_identifier ||
            tokens[i]->type == token_int        ||
            tokens[i]->type == token_double      )
        {
            list_insert(tokens[i]);
        }
        // ak je na vstupe lava zatvorka '('
        else if (tokens[i]->type == token_bracket_left) {
            token_push(stack, tokens[i]);
        }
        // ak je na vstupe prava zatvorka ')'
        else if (tokens[i]->type == token_bracket_right) {
            while ( (!token_is_empty(stack)) && (token_top(stack)->type != token_bracket_left) ) {
                //postfix[++k] = *token_pop(stack);
                list_insert(token_pop(stack));
            }
            token_pop(stack);
        }
        // ak je na vstupe OPERATOR
        else if (tokens[i]->type == token_plus          ||
                 tokens[i]->type == token_minus         ||
                 tokens[i]->type == token_multiply      ||
                 tokens[i]->type == token_divide        ||
                 tokens[i]->type == token_divide_double  )
        {
            while ( (!token_is_empty(stack)) && (prec(tokens[i]) <= prec(token_top(stack))) ) {
                //postfix[++k] = *token_pop(stack);
                list_insert(token_pop(stack));
            }
            token_push(stack, tokens[i]);
        }
    }
    while( !token_is_empty(stack) ) {
        //postfix[++k] = *token_pop(stack);
        list_insert(token_pop(stack));
    }

}
//--------------|
// END DO_MATH  |
//--------------|

/**
 * Najde všechny proměnné ve stringu a dosadí za něj hodnotu
 * Vstup: a < 10, Výstup: 5 < 10
 * Vstup: a*b*c/d, Výstup: 1*2*3/4
 *
char * evaluate_vars(char *str) {
    char var_name[128];
    int var_name_length = 0;
    char var_value[128];
    
    for(int i = 0; i < strlen(str); i++) {
        char c = str[i];
        if (c == ' ') {
            continue;
        }
        
        // Pokračujem dokud dostáváme písmenka
        if (isalpha(c)) {
            var_name[var_name_length++] = c;
        } else {
            // Nějakou proměnnou jsme pravděpodobně našli
            if (var_name_length) {
                if (check_keyword_exists(var_name)) {
                    // @todo co s tim řádkem?
                    error_semantic("Keyword can't be used in an expression", 0);
                }
            } else {
                continue;
            }
        }
        replace_words(str, var_name, var_value);
    }
    return str;
}*/

/**
 * Přepisuje celé slova na udanou hodnotu
 * https://stackoverflow.com/questions/27160073/replacing-words-inside-string-in-c
 *
char *replace_words (char *sentence, char *find, char *replace)
{
    char *dest = malloc (strlen(sentence)-strlen(find)+strlen(replace)+1);
    mem_store(dest);
    char *destptr = dest;
    
    *dest = 0;
    
    while (*sentence)
    {
        if (!strncmp (sentence, find, strlen(find)) &&
            (sentence[strlen(find)] == 0 || sentence[strlen(find)] == ' '))
        {
            strcat (destptr, replace);
            sentence += strlen(find);
            destptr += strlen(replace);
        } else
        {
            while (*sentence && *sentence != ' ')
            {
                *destptr = *sentence;
                destptr++;
                sentence++;
            }
            while (*sentence == ' ')
            {
                *destptr = *sentence;
                destptr++;
                sentence++;
            }
        }
    }
    *destptr = 0;
    return dest;
}


int evaluate_condition(Token *token) {
    char left[128];
    char right[128];
    char middle[128];
    int left_index = 0;
    int right_index = 0;
    int middle_index = 0;
    // Jdeme písmenko po písmenku
    for(int i = 0; i < token->length; i++) {
        char c = token->data[i];
        // mezery přeskakujeme
        if (c == ' ') {
            continue;
        }
        // Pokud nemáme
        if (!left_index) {
            if (is_comparsion_sign(c)) {
                // Toto je případ, kdy dostanem třeba If > 5 Then
                // to je špatně a vracíme chybu
                error_syntactic("Comparsion sign must be between two values", 0);
            }
        }
        
        // pokud máme porovnávací znaménka, zapíšeme je do middle
        // pokud již máme porovnávací znaménka, zapíšeme do right
        // jinak zapisujem do left
        if (is_comparsion_sign(c)) {
            middle[middle_index++] = c;
        } else if (middle_index) {
            right[right_index++] = c;
        } else {
            left[left_index++] = c;
        }
        
        //if (left
        
        // Nyní máme: left = "abc", middle = "<", right = "333"
        
        
    }
    return 0;
}*/

int is_comparsion_sign(char c) {
    return c == '<' || c == '>' || c == '=';
}
