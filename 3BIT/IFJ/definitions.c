/**
 * Definice, rezervované slova, konstanty, ...
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska13 (Jakub Liška)
 */


#include "definitions.h"

#include <stdio.h>



const int ERROR_LEXICAL = 1;
const int ERROR_SYNTACTIC = 2;
const int ERROR_SEMANTIC = 3;
const int ERROR_SEMANTIC_TYPE_COMPATIBILITY = 4;
const int ERROR_SEMANTIC_OTHER = 6;
const int ERROR_INTERNAL = 99;
 char *RESERVED_KEYWORDS_IDENTIFIERS[] = {
    "as", "asc", "declare", "dim", "do", "double", "else", "end", "chr", "function",
    "if", "input", "integer", "length", "loop", "print", "return", "scope",
    "string", "substr", "then", "while"
};
const int RESERVED_KEYWORDS_IDENTIFIERS_SIZE = sizeof(RESERVED_KEYWORDS_IDENTIFIERS)/sizeof(RESERVED_KEYWORDS_IDENTIFIERS[0]);



const char *RESERVED_KEYWORDS[] = {
    "and", "boolean", "continue", "elseif", "exit", "false",
    "for", "next", "not", "or", "shared", "static", "true"
};
const int RESERVED_KEYWORDS_SIZE = sizeof(RESERVED_KEYWORDS)/sizeof(RESERVED_KEYWORDS[0]);

const int _DEBUG = 1;

char *frame_LF = "LF@";
char *frame_GF = "GF@";
char *frame_TF = "TF@";
