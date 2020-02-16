/**
 * Definice, rezervované slova, konstanty, ...
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 */

#ifndef definitions_h
#define definitions_h

#include <stdio.h>


extern const int ERROR_LEXICAL;
extern const int ERROR_SYNTACTIC;
extern const int ERROR_SEMANTIC;
extern const int ERROR_SEMANTIC_TYPE_COMPATIBILITY;
extern const int ERROR_SEMANTIC_OTHER;
extern const int ERROR_INTERNAL;
extern char *RESERVED_KEYWORDS_IDENTIFIERS[];
extern const int RESERVED_KEYWORDS_IDENTIFIERS_SIZE;

extern const char *RESERVED_KEYWORDS[];
extern const int RESERVED_KEYWORDS_SIZE;

extern const int _DEBUG;

extern char *frame_LF;
extern char *frame_GF;
extern char *frame_TF;

#endif /* definitions_h */
