/**
 * Tabulka symbolu - Binarni strom
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 */

 #include "symtable.h"

 #include <ctype.h>
 #include "definitions.h"
 #include "parser.h" 
 #include "helpers.h"
 #include "tokens.h"
 #include "errors.h"
 #include "memory_stack.h"

extern Context_Stack *context_stack;

Binary_Tree_Functions BT_functions;

 /**
  * Vrati ID premennej pre binarny strom, podla nazvu a typu
  * @return int variable ID
  */
 int BST_Hash(char *var_name, char *var_owner) {
     int hash_key = 0;
     int str_length = strlen(var_name) + strlen(var_owner);
     char hash_name[str_length]; //= calloc(str_length, sizeof(char));
     //mem_store(hash_name);
     if (hash_name == NULL) {
         error_internal("Memory allocation failed",42);
     }
     strcpy(hash_name,var_name);
     strcat(hash_name,var_owner);
     strcat(hash_name, "\0");

     for(int i = 0; i < str_length; i++) {
         hash_key = 131 * hash_key + tolower(hash_name[i]); 
     }
     
     return hash_key;
 }

 void BST_Init(Binary_Tree *RootPtr) {
     *RootPtr = NULL;
 }
 
 void BST_Insert(Binary_Tree *RootPtr, Variable* var_in) {

    int var_ID = BST_Hash(var_in->name, var_in->owner);       //ziska ID premennej

    if(*RootPtr == NULL) {							//when BST is empty
		*RootPtr = malloc(sizeof (struct BS_uzel));
        mem_store(RootPtr);
		(*RootPtr)->RPtr = NULL;
		(*RootPtr)->LPtr = NULL;
		(*RootPtr)->Key = var_ID;
		(*RootPtr)->Var = var_in;
	}
	else {									//search to right
		if((*RootPtr)->Key < var_ID) {
			BST_Insert(&(*RootPtr)->RPtr,var_in);
		}	
		else if((*RootPtr)->Key > var_ID) {		//search to left
			BST_Insert(&(*RootPtr)->LPtr,var_in);
		}
		else {		// (*RootPtr)->Key == K); replace data
            if((*RootPtr)->Var->type == var_in->type) {
                (*RootPtr)->Var = var_in;
            }
            else {
                //volat err, redefinicia premennej
            }
		}
	}
 }

 Variable* BST_Search (Binary_Tree RootPtr, char* var_name, char* var_owner) {

    Variable *variable = NULL;

    if(RootPtr == NULL) {	//no nodes
           return variable;
    }
    else {
        int var_ID = BST_Hash(var_name, var_owner);
        if(var_ID == RootPtr->Key) {			//node found
            variable = RootPtr->Var;
            return variable;
        }
        else {
            if(RootPtr->Key < var_ID) {				//not found, continue to right
                if( (variable = BST_Search(RootPtr->RPtr,var_name, var_owner)) != NULL) {
                    return variable;
                }
                else {
                    return variable;
                }
            }
            else {							//not found, continue to left
                if( (variable = BST_Search(RootPtr->LPtr,var_name, var_owner)) != NULL) {
                    return variable;
                }
                else {
                    return variable;
                }
            }
        }
    }

} 

 void BST_Dispose (Binary_Tree *RootPtr) {
        
        if(*RootPtr != NULL) {					//if node exists
            BST_Dispose(&(*RootPtr)->LPtr);		//delete everything to left
            BST_Dispose(&(*RootPtr)->RPtr);		//delete everything to Right
            //free(*RootPtr);						//free node
            //*RootPtr = NULL;
        }

    }

int BST_Hash_Fnc(char* fnc_name) {
    int hash_key = 0;
    int str_length = strlen(fnc_name);

    for(int i = 0; i < str_length; i++) {
        hash_key = 131 * hash_key + tolower(fnc_name[i]); 
    }
     
    return hash_key;
}

void function_bst_init() {

    BT_functions = NULL;

}

/**
 * Inicializuje paměť pro funkci
 */
void function_init(Function *function) {
    function->arguments_cnt = 0;
    function->arguments = malloc(sizeof(Function_Arguments));
    mem_store(function->arguments);
    function->arguments->arr = malloc(sizeof(Function_Argument));
    mem_store(function->arguments->arr);
    function->name = malloc(sizeof(char *) * 128);
    mem_store(function->name);
    function->return_type = Void;
}

/**
 * Přidá funkci do seznamu funkcí (nerekurzivne)
 */
void function_add(Function *function) {

   int fnc_id = BST_Hash_Fnc(function->name);

   if (BT_functions == NULL) {
        BT_functions = malloc(sizeof(struct BS_uzel_fnc));
        mem_store(BT_functions);
        BT_functions->RPtr = NULL;
	    BT_functions->LPtr = NULL;
		BT_functions->Key = fnc_id;
        BT_functions->Fnc = function;
   }
   else {
       Binary_Tree_Functions tmp = BT_functions;
       while (tmp->Key != fnc_id) {
           if (tmp->Key < fnc_id) {
               if (tmp->RPtr != NULL) {
                   tmp = tmp->RPtr;
               }
               else {
                   break;
               }
           }
           else {
               if (tmp->LPtr != NULL) {
                   tmp = tmp->LPtr;
               }
               else {
                   break;
               }
           }
       }
       if (tmp->Key != fnc_id) {
           Binary_Tree_Functions new_fnc = malloc(sizeof(struct BS_uzel_fnc));
           mem_store(new_fnc);
           if (new_fnc == NULL) {
               error_internal("Memory allocation failed!",42);
           }
           new_fnc->Fnc = function;
           new_fnc->RPtr = NULL;
           new_fnc->LPtr = NULL;
           if (fnc_id > tmp->Key) {
               tmp->RPtr = new_fnc;
               tmp->RPtr->Key = fnc_id;
           }
           else {
               tmp->LPtr = new_fnc;
               tmp->LPtr->Key = fnc_id;
           }
           
       }
   }

}

/**
 * Vrátí ptr na funkci podle jejího jména
 */
Function *function_get(char *function_name) {
    
    if (BT_functions == NULL) {
        return NULL;
    }

    int fnc_id = BST_Hash_Fnc(function_name);

    if (fnc_id == BT_functions->Key) {
        return BT_functions->Fnc;
    }
    else {
        Binary_Tree_Functions tmp = BT_functions;
        while (tmp->Key != fnc_id) {
            if (tmp->Key < fnc_id) {
               if (tmp->RPtr != NULL) {
                   tmp = tmp->RPtr;
               }
               else {
                   break;
               }
           }
           else {
               if (tmp->LPtr != NULL) {
                   tmp = tmp->LPtr;
               }
               else {
                   break;
               }
           }
        }
        if (tmp->Key == fnc_id) {
            return tmp->Fnc;
        }
        else {
            return NULL;
        }
    }

    return NULL;
}


/**
  * Vrátí ptr na funkci podle jejího labelu
  */
Function *function_get_by_label(char *label_name) {
    
    //vyrobime z label_name, name
    int new_len = strlen(label_name) -9;
    char* name = malloc(sizeof(char) * 99);
    mem_store(name);
    int i;
    for (i = 0; i < new_len; i++) {
        name[i] = label_name[i];
    }
    name[i] = '\0' ;

    return function_get(name);
}