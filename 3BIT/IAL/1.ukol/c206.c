	
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, říjen 2017
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu, 
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem, 
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu, 
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/

    //sets all list pointers to NULL
    L->First = NULL;
    L->Act = NULL;
    L->Last = NULL;
    
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	while(L->First != NULL) {           //while list contains at least one element
        tDLElemPtr tmp = L->First;         //stores element to be removed
        free(L->First);                 //removes first element from list
        L->First = tmp->rptr;           //links back beginning of list
    }

    L->Act = NULL;      //deactivates list
    L->Last = NULL;    

	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    tDLElemPtr new_elem;        //auxiliary variable to store inserted element

    if((new_elem = malloc(sizeof(tDLElemPtr))) == NULL) {       
        DLError();                                          //if memory allocation fails call error func
    }

    new_elem->data = val;       //stores data into new element
    new_elem->rptr = NULL;
    new_elem->lptr = NULL;

    if(L->First == NULL) {      //if list empty
        L->First = new_elem;
        L->Last = new_elem;
    }
    else {                      //if list already contains at least one element
        L->First->lptr = new_elem;  //links element to original first one
        new_elem->rptr = L->First;  //links new element to its follower
        L->First = new_elem;        //sets new element as first
    }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
    tDLElemPtr new_elem;        //auxiliary variable to store inserted element

    if((new_elem = malloc(sizeof(tDLElemPtr))) == NULL) {
        DLError();                                          //if memory allocation fails call error func
    }

    new_elem->data = val;           //stores data into new element
    new_elem->rptr = NULL;
    new_elem->lptr = NULL;

    if(L->First == NULL) {          //if list empty
        L->First = new_elem;
        L->Last = new_elem;
    }       
    else {                          //if list already contains at least one element
        L->Last->rptr = new_elem;   //links element to original last one
        new_elem->lptr = L->Last;   //links new element to its ancestor
        L->Last = new_elem;         //sets new element as last
    }    
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->First;          //sets activity to the first element

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    
    L->Act = L->Last;       //sets activity to the last element
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

	if(L->First == NULL) {  
        DLError();          //calls error func if list empty
    }
    else {
        *val = L->First->data;      //returns value of first element
    }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if(L->First == NULL) {
        DLError();          //calls error func if list empty
    }
    else {
        *val = L->Last->data;   //returns value of first element
    }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	if(L->First == NULL) {
        return;                 //nothing to delete if list empty
    }
    else {
        if(L->Act == L->First) {    //if first element active, removes activity
            L->Act = NULL;          
        }
        if(L->First == L->Last) {
            free(L->First);
            L->First = NULL;
        }
        else {
            L->First = L->First->rptr;      //moves beginning of the list
            free(L->First->lptr);           //removes originally first element
            L->First->lptr = NULL;          //removes link to deleted element
        }
    }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
    if(L->First == NULL) {
        return;                 //nothing to delete if list empty
    }
    else {
        if(L->Act == L->Last) { //if last element active, removes activity
            L->Act = NULL;
        }
        if(L->Last == L->First) {
            free(L->Last);
            L->Last = NULL;
        }
        else {
            L->Last = L->Last->lptr;    //moves end of the list
            free(L->Last->rptr);        //removes originally last element
            L->Last->rptr = NULL;       //removes link to deleted element
        }
    }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
    
    if((L->Act == NULL) || (L->Act == L->Last)) { //if list not active or activity on last element do nothing
        return;
    }
    else {
        tDLElemPtr tmp;         //auxiliary variable to store removed element
        tmp = L->Act->rptr;  
        L->Act->rptr = tmp->rptr;   //links active element to follower of removed element
        
        if(tmp == L->Last) {        //if removed element was last
            L->Last = L->Act;
        }
        else {
            tmp->rptr->lptr = L->Act;   //links follower of removed element to active
        }
        
        free(tmp);      //frees allocated memory - removes element
    }

		
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
    
    if((L->Act == NULL) || (L->Act == L->First)) {  //if list not active or activity on first element do nothing
        return;
    }

    if(L->Act->lptr != NULL) {
        tDLElemPtr tmp;         //auxiliary variable to store removed element
        tmp = L->Act->lptr;
        L->Act->lptr = tmp->lptr;   //links active element to ancestor of removed element
        
        if(tmp == L->First) {        //if removed element was first
            L->First = L->Act;      
        }
        else{
            tmp->lptr->rptr = L->Act;       //links follower of removed element to active
        }
        
        free(tmp);          //frees allocated memory - removes element
    }
			
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    
    if(L->Act == NULL) {        //if list not active do nothing
        return;
    }
    else {
        tDLElemPtr new_elem;    //auxiliary variable to store new element

        if((new_elem = malloc(sizeof(tDLElemPtr))) == NULL) {   
            DLError();                                          //if memory allocation fails call error func
            return;
        }

        new_elem->data = val;           //stores value into new element
        new_elem->lptr = NULL;
        new_elem->rptr = NULL;

        if(L->Act == L->Last) {         //if active element is last
            L->Last->rptr = new_elem;   //links in new element
            new_elem->lptr = L->Last;   //links in new elements ancestor
            L->Last = new_elem;         //sets end of list to the new element
        }
        else {
            L->Act->rptr->lptr = new_elem; //links follower of active element with new element
            new_elem->rptr = L->Act->rptr;  //links new element with its ancestor
            L->Act->rptr = new_elem;        //links active element to new
            new_elem->lptr = L->Act;        //links new element with active element
        }

    }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    
    if(L->Act == NULL) {        //if list not active do nothing
        return;
    }
    else {
        tDLElemPtr new_elem;    //auxiliary variable to store new element
        

        if((new_elem = malloc(sizeof(tDLElemPtr))) == NULL) {
            DLError();                                          //if memory allocation fails call error func                     
            return;
        }

        new_elem->data = val;       //stores value into new element
        new_elem->lptr = NULL;
        new_elem->rptr = NULL;

        if(L->Act == L->First) {        //if active element is first
            L->First->lptr = new_elem;  //links in new element
            new_elem->rptr = L->First;  //links in new elements follower
            L->First = new_elem;        //sets beginning of list to the new element
        }
        else {
            L->Act->lptr->rptr = new_elem;  //links ancestor of active element with new element
            new_elem->lptr = L->Act->lptr;  //links new element with its follower
            L->Act->lptr = new_elem;        //links active element to new
            new_elem->rptr = L->Act;        //links new element with active element
        }

    }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
		
	if(L->Act == NULL) {    //if list not active call error func
        DLError();
        return;
    }
    else {
        *val = L->Act->data;    //copies data from active element 
    }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
    
    if(L->Act == NULL) {    //if list not active do nothing
        return;
    }
    else {
        L->Act->data = val; //sets new value for active element
    }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
    
    if(L->Act == NULL) {    //if list not active do nothing
        return;
    }
    else {
        if(L->Act == L->Last) {     //if last element is active, lose activity
            L->Act = NULL;
        }
        else {
            L->Act = L->Act->rptr;  //move activity to follower
        }
    }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
    
    if(L->Act == NULL) {    //if list not active do nothing
        return;
    }
    else {
        if(L->Act == L->First) {    //if first element is active, lose activity
            L->Act = NULL;
        }
        else {
            L->Act = L->Act->lptr;  //move activity to follower
        }
    }
	
 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	
    return (L->Act != NULL);    //if list active return non-zero, if inactive return 0

 //solved = FALSE;                   /* V případě řešení, smažte tento řádek! */
}

/* Konec c206.c*/
