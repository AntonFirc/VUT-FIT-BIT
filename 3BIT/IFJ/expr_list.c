/**
 * Zoznam tokenov
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfirca00 (Anton Firc)
 */

#include "expr_list.h"
#include "errors.h"
#include <stdlib.h>

Token_List token_list;

void list_init() {
    token_list.Act = NULL;
    token_list.First = NULL;
    token_list.Last = NULL;
}

int list_act_n() {
    if (token_list.Act == token_list.Last) {
        return 1;
    }
    else if (token_list.Act == token_list.Last) {
        token_list.Act = token_list.First;
        return 0;
    }
    else {
        token_list.Act = token_list.Act->RPtr;
        return 0;
    }
}

Token* list_act() {
    if (token_list.Act == NULL) {
        error_internal("Token list fatal error",0);
    }
    else {
        return token_list.Act->token;
    }
    return NULL;
}

void list_insert(Token *token) {

    Token_List_Ptr new_item;

    if((new_item = malloc(sizeof(Token_List_Ptr)*3)) == NULL) {     //nepytajte sa ako som prisiel na to *3 (proste to bez toho hadze chyby, asi preto ze tam su 3 pointre ?)   
        error_internal("Memory allocation failed!", 0);
    }

    new_item->token = token;
    new_item->LPtr = NULL;
    new_item->RPtr = NULL;

    if(token_list.Last == NULL) {
        token_list.First = new_item;
        token_list.Last = token_list.First;
        token_list.Act = token_list.First;
    }
    else {
        token_list.Last->RPtr = new_item;
        new_item->LPtr = token_list.Last;
        token_list.Last = new_item;
    }
}

void list_insert_act(Token* token) {
    if (token_list.Act == NULL) {
        error_internal("Token list fatal error",0);
    }
    else {
        token_list.Act->token = token;
    }
}

Token* list_remove_pred() {
    if (token_list.Act == NULL) {
        error_internal("Token list fatal error",0);
    }
    if (token_list.Act == token_list.First) {
        return NULL;
    }
    else if (token_list.Act->LPtr != NULL) {
        Token_List_Ptr tmp = token_list.Act->LPtr;
        token_list.Act->LPtr = tmp->LPtr;
        if (tmp->LPtr != NULL) {
            tmp->LPtr->RPtr = token_list.Act;
        }
        return tmp->token;
    }
    return NULL;
}

void list_dispose() {
    free(token_list.First);
    list_init();
}
