/**
 * Zápis instrukcí
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 */
#include "instructions.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include "helpers.h"
#include "string.h"
#include "errors.h"
#include "memory_stack.h"

FILE *log_file = NULL;
int temp_expr_cnt = 0;

/**
 *
 * Dvojitý pointer je tu z důvodu, že musíme "ovlivnit" z této 
 * funkce i původní pointer
 */
void open_file(FILE **f) {
    *f = fopen("output.txt", "w");
    
    if (f == NULL) {
        fprintf(stderr, "Error opening file!\n");
    }
    log_file = *f;
}

void close_file(FILE *f) {
    fclose(f);
}

char *_instruction_convert_value(char *value) {
    char *ret = malloc(sizeof(char) * 128);
    mem_store(ret);
    int str_len = strlen(value);
    if (str_len == 0) {
        return "string@";
    }
    for (int i = 0; i < str_len; i++) {
        if (isdigit(value[i])) {
            continue;
        }
        else if ((value[i] == '.' || value[i] == 'E' || value[i] == 'e') && (i != 0)) {
            strcpy(ret, "float@");
            strcat(ret, value);
            return ret;
        }
        else {
            strcpy(ret, "string@");
            strcat(ret, value);
            return ret;
        }
    }
    strcpy(ret, "int@");
    strcat(ret, value);
    return ret;
}

void _instruction_write_to_file(char *instruction) {
    if (log_file == NULL) {
        open_file(&log_file);
    }
    fprintf(log_file, "%s\n", instruction);
    printf("%s\n", instruction);
}
void instruction(char *instrc, ...) {
    char *instruction_final;
    va_list va_args;
    va_start(va_args, instrc);
    vasprintf(&instruction_final, instrc, va_args);
    _instruction_write_to_file(instruction_final);
    va_end(va_args);
}
void generate_string(char *dest, size_t length) {
    char charset[] = "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}
void name_builder(char *name, char *str, char *id) {
    mem_store(name);
    strcpy(name, str);
    strcat(name, id);
}
//---------------------------------------------------------------------------//

// MOVE
void instruction_move(char *dest, char* frame, char *source) {
    instruction("MOVE LF@%s %s@%s", dest, frame , source);
}

// HEADER    
void instruction_header() {
    _instruction_write_to_file(".IFJcode17");
}

// CREATEFRAME
void instruction_createframe() {
    _instruction_write_to_file("CREATEFRAME");
}

// PUSHFRAME
void instruction_pushframe() {
    _instruction_write_to_file("PUSHFRAME");
}

// POPFRAME
void instruction_popframe() {
    _instruction_write_to_file("POPFRAME");
}

// DEFVAR
void instruction_defvar(char *var, char *frame) {
    instruction("DEFVAR %s@%s", frame, var);
}

// CALL
void instruction_call(char *function_name, Function_Arguments *arguments, Context *ctx) {
    for(int i = arguments->arguments_cnt - 1; i >= 0; i--) {
        // V případě, že argument je třeba string literal (a ne proměnná)
        if (arguments->arr[i].var_name == NULL) {
            instruction_pushs(NULL, ctx->label_name, arguments->arr[i].value, convert_var_type_to_char(arguments->arr[i].var_type));
        } else {
            instruction_pushs(arguments->arr[i].var_name, ctx->label_name, NULL, "LF");
        }
    }

    Function *function = function_get(function_name);
    Function_Argument *arguments_defined = function->arguments->arr;

    instruction_createframe();
    for(int i = 0; i < function->arguments_cnt; i++) {
        instruction_defvar(arguments_defined[i].var_name, "TF");
        instruction_pops(arguments_defined[i].var_name, "TF");
    }
    instruction_pushframe();

    instruction("CALL %s", function->label_name);

    instruction_popframe();
    
}

// RETURN
void instruction_return(char *var_name, Context *ctx, char *value) {

    if (var_name == NULL && value == NULL) {
        _instruction_write_to_file("RETURN");
        return;
    }
    else {
        if (var_name) {
            instruction_pushs(var_name, ctx->label_name, NULL, "LF");
        } else {
            instruction_pushs(NULL, ctx->label_name, value, NULL);
        }
        _instruction_write_to_file("RETURN");
    }
}

/**
 * Akceptuje název proměnné, vyvolává instrukci, která vypíše její hodnotu
 */
void instruction_write_var(char *var_name) {
    instruction("WRITE LF@%s", var_name);
}

// PUSHS
void instruction_pushs(char *var_name, char *var_owner, char *val, char *frame) {
    char *value = malloc(sizeof(char) * 128);
    mem_store(value);
    if (var_name == NULL) {
        value = _instruction_convert_value(val);
    } else {
        strcpy(value, "\0");
        strcat(value, frame);
        strcat(value, "@");
        strcat(value, var_name);
    }
    instruction("PUSHS %s", value);
}

// POPS
void instruction_pops(char *var_name, char *frame) {
    instruction("POPS %s@%s", frame, var_name);
}

// CLEARS
void instruction_clear(){
    _instruction_write_to_file("CLEARS");
}

// LABEL  
void instruction_make_label(char* name) {
    instruction("LABEL %s", name);
}
/*
char * instruction_label_condition(Context *ctx) {
    char *name = malloc(sizeof(char) * 128);
    mem_store(name);

    // creating new if
    if (ctx == NULL) {
        strcpy(name, "if_");
        strcat(name, "random_here");
    } else if (ctx->context == context_condition) {
        strcpy(name, "elseif_");
        strcat(name, ctx->label_name);
    } else if (ctx->context == context_condition_else) {
        strcpy(name, "endif_");
        strcat(name, ctx->label_name);
    }
    
    instruction("LABEL %s", name);
    return name;
}


char *instruction_label_while(Context *ctx) {
    char *name = calloc(128, sizeof(char));
    
    if (ctx == NULL) {
        strcpy(name, "while_");
        strcat(name, "random_here");

    } else {
        strcpy(name, "endwhile_");
        strcat(name, ctx->label_name);
    }
    char *id = calloc(5,sizeof(char));
    mem_store(id);
    generate_string(id, 5);
    strcat(name, id);
    instruction("LABEL %s", name);
    return name;
}
*/

// ADD
void instruction_add(char *var_result, char *frame1, char *var1, char *frame2, char *var2) {
    instruction("ADD LF@%s %s@%s %s@%s", var_result, frame1, var1, frame2, var2);
}

// SUB
void instruction_sub(char *var_result, char *frame1, char *var1, char *frame2, char *var2) {
    instruction("SUB LF@%s %s@%s %s@%s", var_result, frame1, var1, frame2, var2);
}

// MUL
void instruction_mul(char *var_result, char *frame1, char *var1, char *frame2, char *var2) {
    instruction("MUL LF@%s %s@%s %s@%s", var_result, frame1, var1, frame2, var2);
}
/*  
void instruction_jumpif_while_condition(Token *expression[], int tokens_length, Context * ctx, int jump_to_start) {
    if (jump_to_start) {
        instruction("JUMPIFQIQWIOEBJ NECO %s", ctx->label_name);
    } else {
        instruction("JUMPIFNEBOEQUAL NECO end%s", ctx->label_name);
    }
    
}

void instruction_jumpif_condition(Token *expression[], int tokens_length, Context *ctx) {
    instruction("JUMPIFNEBOEQUAL NECO else%s", ctx->label_name);
}
*/

// DIV
void instruction_div(char *var_result, char *frame1, char *var1, char *frame2, char *var2) {
    instruction("DIV LF@%s %s@%s %s@%s", var_result, frame1, var1, frame2, var2);
}

// ADDS
void instruction_adds() {
    _instruction_write_to_file("ADDS");
}

// SUBS
void instruction_subs() {
    _instruction_write_to_file("SUBS");
}

// MULS
void instruction_muls() {
    _instruction_write_to_file("MULS");
}

// DIVS
void instruction_divs() {
    _instruction_write_to_file("DIVS");
}

// LT
void instruction_lt(char* var_result, char* frame1, char* var1, char* frame2, char* var2) {
    instruction("LT LF@%s %s@%s %s@%s", var_result, frame1, var1, frame2, var2);
}
// GT
void instruction_gt(char* var_result, char* frame1, char* var1, char* frame2, char* var2) {
    instruction("GT LF@%s %s@%s %s@%s", var_result, frame1, var1, frame2, var2);
}
// EQ
void instruction_eq(char* var_result, char* frame1, char* var1, char* frame2, char* var2) {
    instruction("EQ LF@%s %s@%s %s@%s", var_result, frame1, var1, frame2, var2);
}
// LTS

// GTS

// EQS

// AND

// OR
void instruction_or(char* var_result, char* frame1, char* var1, char* frame2, char* var2) {
    instruction("OR LF@%s %s@%s %s@%s", var_result, frame1, var1, frame2, var2);
}
// NOT

// ANDS

// ORS

// NOTS

// INT2FLOAT

// FLOAT2INT

// FLOAT2R2EINT

// FLOAT2R2OINT

// INT2CHAR
void instruction_int2char(char* var_result, char *var_int) {
    instruction("INT2CHAR LF@%s LF@%s",var_result, var_int);
}

// STRI2INT
void instruction_stri2int(char* var_result, char* frame1, char* var1, char* frame2, char* var2) {
    instruction("STRI2INT LF@%s %s@%s %s@%s", var_result, frame1, var1, frame2, var2);
}

// INT2FLOATS

// FLOAT2INTS
void instruction_float2ints() {
    instruction("FLOAT2INTS");
}

// FLOAT2R2EINTS

// FLOAT2R2OINTS

// INT2CHARS

// STRI2INTS

// READ
void instruction_read(char *str, char *type) {
    /***** TODO ****/
    instruction("READ LF@%s %s", str, type);
}

// WRITE
void instruction_write(char *str, char* val) {
    if (str == NULL) {
        char* neco = _instruction_convert_value(val);
        instruction("WRITE %s", neco);
    } else {
        instruction("WRITE LF@%s", str);
    }
}

// CONCAT
void instruction_concat(char* var_result, char* frame1, char* var1, char* frame2, char* var2) {
    instruction("CONCAT LF@%s %s@%s %s@%s", var_result, frame1, var1, frame2, var2);
}
// STRLEN
void instruction_strlen(char* var_result, char *var_str) {
    instruction("STRLEN LF@%s LF@%s",var_result, var_str);
}

// GETCHAR
void instruction_getchar(char* var_result, char* frame1, char* var1, char* frame2, char* var2) {
    instruction("GETCHAR LF@%s %s@%s %s@%s", var_result, frame1, var1, frame2, var2);
}

// SETCHAR

// TYPE

// JUMP
/**
 * Instrukce pro skok na daný label name
 */
void instruction_jump(char *label_name) {
    instruction("JUMP %s", label_name);
}

// JUMPIFEQ
void instruction_jumpifeq(char* label, char* frame1, char* var1, char* frame2, char* var2) {
    instruction("JUMPIFEQ %s %s@%s %s@%s", label, frame1, var1, frame2, var2);
}

// JUMPIFNEQ
void instruction_jumpifneq(char* label, char* frame1, char* var1, char* frame2, char* var2) {
    instruction("JUMPIFNEQ %s %s@%s %s@%s", label, frame1, var1, frame2, var2);
}
// JUMPIFEQS

// JUMPIFNEQS

// BREAK

// DPRINT

/*
 * Instrukce pro jump ZA konec podmínky
 */
/*
void instruction_jump_endif(Context *context) {
    char *label_name = calloc(128, sizeof(128));
    strcpy(label_name, "endif_");
    strcat(label_name, context->label_name);
    instruction_jump(label_name);
}
*/
void instruction_arguments_function(Function *function) {
    for(int i = 0; i < function->arguments_cnt; i++) {
        //instruction_pop(function->arguments->arr[i].var_name);
    }
}

char *instruction_label_function(char *function_name) {
    char *name = malloc( (strlen(function_name)+9) * sizeof(char) );
    mem_store(name);
    if (name == NULL) {
        error_internal("Memory allocation failed !", 0);
    }
    strcpy(name, function_name);
    strcat(name, "_function\0");
    instruction("LABEL %s", name);
    return name;
}


/**
 * Instrukce pro jump ZA konec podmínky
 */
/*
void instruction_jump_elseif(Context *context) {
    char *label_name = context->label_name;
    strcpy(label_name, "_endif");
    instruction_jump(label_name);
}
*/
void function_length() {
    //musime ulozit funkciu a zapisat label funkce
    char* label_name = instruction_label_function("length");
    Function* function = malloc(sizeof(Function));
    mem_store(function);
    function_init(function);
    //function->arguments_cnt = 1;
    function->return_type = Int;
    function->label_name = label_name;
    function->name = "length";
    function_argument_add(function, "s", "string");
    function_add(function);

    instruction_defvar("length", "LF");
    instruction_strlen("length", "s");
    instruction_pushs("length\0", NULL, NULL, "LF\0");
    instruction_return(NULL, NULL, NULL);
}

void function_chr(){
    char* label_name = instruction_label_function("chr");
    Function* function = malloc(sizeof(Function));
    mem_store(function);
    function_init(function);

    function->return_type = String;
    function->label_name = label_name;
    function->name = "chr";
    function_argument_add(function, "i", "integer");
    function_add(function);

    instruction_defvar("char", "LF");
    instruction_int2char("char", "i");
    instruction_pushs("char\0", NULL, NULL, "LF\0");
    instruction_return(NULL, NULL, NULL);
}

void function_asc(){
    char* label_name = instruction_label_function("asc");
    Function* function = malloc(sizeof(Function));
    mem_store(function);
    function_init(function);

    function->return_type = Int;
    function->label_name = label_name;
    function->name = "asc";
    function_argument_add(function, "s", "string");
    function_argument_add(function, "i", "integer");
    function_add(function);

    instruction_defvar("length", "LF");
    instruction_strlen("length", "s");
    instruction_defvar("bad_val", "LF");
    instruction_lt("bad_val", "LF", "i", "int", "0");
    instruction_jumpifeq("bad_input", "LF", "bad_val", "bool", "true");
    instruction_gt("bad_val", "LF", "i", "LF", "length");
    instruction_jumpifeq("bad_input", "LF", "bad_val", "bool", "true");
    instruction_defvar("result", "LF");
    instruction_sub("i", "LF", "i", "int", "1");
    instruction_stri2int("result", "LF", "s", "LF", "i");
    instruction_pushs("result\0", NULL, NULL, "LF\0");
    instruction_return(NULL, NULL, NULL);
    instruction_pushs("0\0", NULL, NULL, "int");
    instruction_return(NULL, NULL, NULL);    
}

void function_substr() {
    char* label_name = instruction_label_function("substr");
    Function* function = malloc(sizeof(Function));
    mem_store(function);
    function_init(function);

    function->return_type = String;
    function->label_name = label_name;
    function->name = "substr";
    function_argument_add(function, "s", "string");
    function_argument_add(function, "i", "integer");
    function_argument_add(function, "n", "integer");
    function_add(function);

    instruction_defvar("length", "LF");
    instruction_strlen("length", "s");
    instruction_jumpifeq("nothing" ,"LF", "length", "int", "0");
    instruction_defvar("i_is_less", "LF");
    instruction_lt("i_is_less", "LF", "i", "int", "1");
    instruction_jumpifeq("nothing", "LF", "i_is_less", "bool", "true");
    instruction_sub("i", "LF", "i", "int", "1");
    instruction_defvar("n_is_less", "LF");
    instruction_lt("n_is_less", "LF", "n", "int", "0");
    instruction_jumpifeq("all_ok", "LF", "n_is_less", "bool", "false");
    instruction_move("n", "LF", "length");
    instruction_make_label("all_ok");
    instruction_defvar("substring", "LF");
    instruction_move("substring", "string", "\000");
    instruction_defvar("char", "LF");
    instruction_defvar("index", "LF");
    instruction_defvar("bounds", "LF");
    instruction_defvar("n_done", "LF");
    instruction_defvar("finish", "LF");
    instruction_move("index", "int", "0");
    instruction_make_label("can_copy");
    instruction_jumpifeq("copy", "LF", "i", "LF", "index");
    instruction_make_label("read");
    instruction_getchar("char", "LF", "s", "LF", "index");
    instruction_add("index", "LF", "index", "int", "1");
    instruction_jump("can_copy");
    instruction_make_label("copy");
    instruction_getchar("char", "LF", "s", "LF", "index");
    instruction_add("index", "LF", "index", "int", "1");
    instruction_concat("substring", "LF", "substring", "LF", "char");
    instruction_sub("n", "LF", "n", "int", "1");
    instruction_eq("bounds", "LF", "index", "LF", "length");
    instruction_eq("n_done", "LF", "n", "int", "0");
    instruction_or("finish", "LF", "bounds", "LF", "n_done");
    instruction_jumpifeq("copy", "LF", "finish", "bool", "false");
    instruction_make_label("finish");
    instruction_pushs("substring", NULL, NULL, "LF");
    instruction_return(NULL, NULL, NULL);
    instruction_make_label("nothing");
    instruction_pushs("\000", NULL, NULL, "string");
    instruction_return(NULL, NULL,NULL);
}

void instruction_int2floats() {
    _instruction_write_to_file("INT2FLOATS");
}

void instruction_condition_begin(Token_Type cmp_type, char *id) {
    char *condition_name = malloc (sizeof(char) * 16 );
    char *then_name = malloc(sizeof(char) * 11 );
    char *L_name = malloc (sizeof(char) * 8 );
    char *R_name = malloc (sizeof(char) * 8 );
    char *else_name = malloc (sizeof(char) * 11 );
    name_builder(condition_name, "condition_", id);
    name_builder(L_name, "L_", id);
    name_builder(R_name, "R_", id);
    name_builder(else_name, "else_", id);
 
    switch(cmp_type){
        case token_equals :
            instruction_jumpifneq(else_name, "LF", L_name, "LF", R_name);
            break;
        case token_not_equal :
            instruction_jumpifeq(else_name, "LF", L_name, "LF", R_name);
            break;
        case token_less :
            instruction_defvar(condition_name, "LF");
            instruction_lt(condition_name, "LF", L_name, "LF", R_name);
            instruction_jumpifeq(else_name, "LF", condition_name, "bool", "false");
            break;
        case token_more :
            instruction_defvar(condition_name, "LF");
            instruction_gt(condition_name, "LF", L_name, "LF", R_name);
            instruction_jumpifeq(else_name, "LF", condition_name, "bool", "false");
            break;
        case token_less_or_equal :
            name_builder(then_name, "then_", id);

            instruction_jumpifeq(then_name, "LF", L_name, "LF", R_name);
            instruction_defvar(condition_name, "LF");
            instruction_lt(condition_name, "LF", L_name, "LF", R_name);
            instruction_jumpifeq(else_name, "LF", condition_name, "bool", "false");
            instruction_make_label(then_name);
            break;
        case token_more_or_equal :
            name_builder(then_name, "then_", id);

            instruction_jumpifeq(then_name, "LF", L_name, "LF", R_name);
            instruction_defvar(condition_name, "LF");
            instruction_gt(condition_name, "LF", L_name, "LF", R_name);
            instruction_jumpifeq(else_name, "LF", condition_name, "bool", "false");
            instruction_make_label(then_name);
            break;
        default :
            break;
    }

}
void instruction_condition_middle(char *id) {
    char *end_if_name = malloc (sizeof(char) * 13 );
    char *else_name = malloc (sizeof(char) * 11 );
    name_builder(end_if_name, "end_if_", id);
    name_builder(else_name, "else_", id);

    instruction_jump(end_if_name);
    instruction_make_label(else_name);
}
void instruction_condition_end(char *id) {
    char *end_if_name = malloc (sizeof(char) * 13 );
    name_builder(end_if_name, "end_if_", id);

    instruction_make_label(end_if_name);
}
void instruction_while_begin(char* L_name, char* R_name, Token_Type cmp_type, char *id) {
    char *condition_name = malloc (sizeof(char) * 16 );
    char *end_while_name = malloc (sizeof(char) * 13 );
    char *start_while_name = malloc(sizeof(char) * 18);
    char *then_name = malloc(sizeof(char) * 11 );
    if (L_name == NULL) {
        L_name = malloc (sizeof(char) * 8 );
        name_builder(L_name, "L_", id);
    }
    if (R_name == NULL) {
        R_name = malloc (sizeof(char) * 8 );
        name_builder(R_name, "R_", id);
    }
    
    name_builder(condition_name, "condition_", id);
    name_builder(end_while_name, "end_while_", id);
    name_builder(start_while_name, "start_while_", id);

    instruction_defvar(condition_name, "LF");
    instruction_make_label(start_while_name);

    switch(cmp_type){
    case token_equals :
        instruction_jumpifneq(end_while_name, "LF", L_name, "LF", R_name);
        break;
    case token_not_equal :
        instruction_jumpifeq(end_while_name, "LF", L_name, "LF", R_name);
        break;
    case token_less :
        instruction_lt(condition_name, "LF", L_name, "LF", R_name);
        instruction_jumpifeq(end_while_name, "LF", condition_name, "bool", "false");
        break;
    case token_more :
        instruction_gt(condition_name, "LF", L_name, "LF", R_name);
        instruction_jumpifeq(end_while_name, "LF", condition_name, "bool", "false");
        break;
    case token_less_or_equal :
        name_builder(then_name, "then_", id);

        instruction_jumpifeq(then_name, "LF", L_name, "LF", R_name);
        instruction_lt(condition_name, "LF", L_name, "LF", R_name);
        instruction_jumpifeq(end_while_name, "LF", condition_name, "bool", "false");
        instruction_make_label(then_name);
        break;
    case token_more_or_equal :
        name_builder(then_name, "then_", id);

        instruction_jumpifeq(then_name, "LF", L_name, "LF", R_name);
        instruction_gt(condition_name, "LF", L_name, "LF", R_name);
        instruction_jumpifeq(end_while_name, "LF", condition_name, "bool", "false");
        instruction_make_label(then_name);
        break;
    default :
        break;
    }
    
}
void instruction_while_end(char *id) {
    char *end_while_name = malloc (sizeof(char) * 13 );
    char *start_while_name = malloc(sizeof(char) * 18);
    name_builder(end_while_name, "end_while_", id);
    name_builder(start_while_name, "start_while_", id);

    instruction_jump(start_while_name);
    instruction_make_label(end_while_name);
}    
char *instruction_expr(Token *left, Token *right, Token *sign) {
    char *temp_res = calloc(15, sizeof(char));
    char *frame_1 = calloc(7, sizeof(char));
    char *frame_2 = calloc(7, sizeof(char));
    sprintf(temp_res, "__TEMP_VAR_%d", temp_expr_cnt);
    instruction_defvar(temp_res, "LF");

    switch(sign->type) {
        case token_plus:
            if (left->type == token_identifier) {strcpy(frame_1, "LF");} else {strcpy(frame_1,convert_value_to_var_type(left->data));}
            if (right->type == token_identifier) {strcpy(frame_2, "LF");} else {strcpy(frame_2,convert_value_to_var_type(right->data));}
            instruction_add(temp_res, frame_1, left->data, frame_2, right->data);
            break;
        case token_minus:
            if (left->type == token_identifier) {strcpy(frame_1, "LF");} else {strcpy(frame_1,convert_value_to_var_type(left->data));}
            if (right->type == token_identifier) {strcpy(frame_2, "LF");} else {strcpy(frame_2,convert_value_to_var_type(right->data));}
            instruction_sub(temp_res, frame_1, left->data, frame_2, right->data);
            break;
        case token_multiply:
            if (left->type == token_identifier) {strcpy(frame_1, "LF");} else {strcpy(frame_1,convert_value_to_var_type(left->data));}
            if (right->type == token_identifier) {strcpy(frame_2, "LF");} else {strcpy(frame_2,convert_value_to_var_type(right->data));}
            instruction_mul(temp_res, frame_1, left->data, frame_2, right->data);
            break;
        case token_divide:
            if (left->type == token_identifier) {
                if(left->type == token_int) {
                    instruction_pushs(left->data, NULL, NULL, "LF");
                    instruction_int2floats();
                    instruction_pops(left->data, "LF");
                }
                strcpy(frame_1, "LF");
            }
            else {
                strcpy(frame_1,"float");
            }

            if (right->type == token_identifier) {
                if(right->type == token_int) {
                    instruction_pushs(right->data, NULL, NULL, "LF");
                    instruction_int2floats();
                    instruction_pops(right->data, "LF");
                }
                strcpy(frame_2, "LF");
            }
            else {
                strcpy(frame_2,"float");
            }
            instruction_div(temp_res, frame_1, left->data, frame_2, right->data);
            instruction_pushs(temp_res, NULL, NULL, "LF");
            instruction_float2ints();
            instruction_pops(temp_res, "LF");
            break;
        case token_divide_double:
            if (left->type == token_identifier) {
                if(left->type == token_int) {
                    instruction_pushs(left->data, NULL, NULL, "LF");
                    instruction_int2floats();
                    instruction_pops(left->data, "LF");
                }
                strcpy(frame_1, "LF");
            } 
            else {
                strcpy(frame_1,"float");
            } 

            if (right->type == token_identifier) {
                if(right->type == token_int) {
                    instruction_pushs(right->data, NULL, NULL, "LF");
                    instruction_int2floats();
                    instruction_pops(right->data, "LF");
                }
                strcpy(frame_2, "LF");
            } 
            else {
                strcpy(frame_2,"float");
            }
            instruction_div(temp_res, frame_1, left->data, frame_2, right->data);
            break;
        default:
            break;
    }
    temp_expr_cnt++;
    return temp_res;
}