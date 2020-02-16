/**
 * Tabulka symbolu - Binarni strom
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 */

#ifndef symtable_h
#define symtable_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helpers.h"

#define TRUE 1
#define FALSE 0

typedef struct BS_uzel {
    int Key;			                                                  
    Variable * Var;
	struct BS_uzel * LPtr;                                    
	struct BS_uzel * RPtr;                     
} *Binary_Tree;

typedef struct BS_uzel_fnc {
    int Key;			                                                  
    Function * Fnc;
	struct BS_uzel_fnc * LPtr;                                    
	struct BS_uzel_fnc * RPtr;                     
} *Binary_Tree_Functions;

extern Binary_Tree_Functions BT_functions;

#include "parser.h"


int BST_Hash(char *var_name, char *var_owner);
void BST_Init(Binary_Tree *RootPtr);
void BST_Insert(Binary_Tree *RootPtr, Variable* var_in);
Variable* BST_Search (Binary_Tree RootPtr, char* var_name, char* var_owner);
void BST_Dispose (Binary_Tree *RootPtr);

int BST_Hash_Fnc(char* fnc_name);
void function_init(Function *function);
void function_add(Function *function);
Function *function_get(char *function_name);
Function *function_get_by_label(char *label_name);

#endif
