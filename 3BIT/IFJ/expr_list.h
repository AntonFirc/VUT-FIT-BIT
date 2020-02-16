/**
 * Zoznam tokenov
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfirca00 (Anton Firc)
 */

#ifndef _LIST_T_
#define _LIST_T_

#include "tokens.h"

typedef struct Token_elem {                 
        Token* token;                                            
        struct Token_elem *LPtr;         
        struct Token_elem *RPtr;        
} *Token_List_Ptr;

typedef struct {                                 
    Token_List_Ptr First;                     
    Token_List_Ptr Act;                    
    Token_List_Ptr Last;              
} Token_List;

extern Token_List token_list;

void list_init();
int list_act_n();
Token* list_act();
void list_insert(Token* token);
void list_insert_act(Token* token);
Token* list_remove_pred();
void list_dispose();

#endif