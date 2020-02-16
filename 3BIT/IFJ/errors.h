/**
 * Výpis chybových hlášek
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 */


#ifndef errors_h
#define errors_h


void error_lexical(char *message, int line, ...);
void error_syntactic(char *message, int line, ...);
void error_semantic(char *message, int line, ...);
void error_incompatible_type(char *message, int line, ...);
void error_semantic_other(char *message, int line, ...);
void error_internal(char *message, int line, ...);

#endif /* errors_h */
