
/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, říjen 2017                                        */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického
** výrazu do postfixového tvaru. Pro převod využijte zásobník (tStack),
** který byl implementován v rámci příkladu c202. Bez správného vyřešení
** příkladu c202 se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix .... konverzní funkce pro převod infixového výrazu 
**                       na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**    
**    untilLeftPar .... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"

int solved;


/*
** Pomocná funkce untilLeftPar.
** Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka
** bude také odstraněna. Pokud je zásobník prázdný, provádění funkce se ukončí.
**
** Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
** znaků postExpr. Délka převedeného výrazu a též ukazatel na první volné
** místo, na které se má zapisovat, představuje parametr postLen.
**
** Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
** nadeklarovat a používat pomocnou proměnnou typu char.
*/
void untilLeftPar ( tStack* s, char* postExpr, unsigned* postLen ) {

  if(stackEmpty(s)) {   //if stack empty does nothing
    return;
  }
    
  char stack_top ;      //auxiliary variable that stores top of stack

  for(int i = s->top; i >= 0; i--){   //main cycle to cycle trough all elements stored in stack
    stackTop(s,&stack_top);
    if(stack_top == '(') {            //when first left bracket found, removes it from stack and stop
      stackPop(s);                    
      return;
    }
    postExpr[*postLen] = stack_top;  //moves top of stack to output string
    *postLen = *postLen + 1;
    stackPop(s);                    //removes "copied" element
  }
}

/*
** Pomocná funkce doOperation.
** Zpracuje operátor, který je předán parametrem c po načtení znaku ze
** vstupního pole znaků.
**
** Dle priority předaného operátoru a případně priority operátoru na
** vrcholu zásobníku rozhodneme o dalším postupu. Délka převedeného 
** výrazu a taktéž ukazatel na první volné místo, do kterého se má zapisovat, 
** představuje parametr postLen, výstupním polem znaků je opět postExpr.
*/
void doOperation ( tStack* s, char c, char* postExpr, unsigned* postLen ) {
  
  if(stackEmpty(s)) {     //if stack empty, stores operator immediately
    stackPush(s,c);
    return;
  }

  char stack_top;         //auxiliary variable to store top of the stack
  stackTop(s,&stack_top);

  if(stack_top == '('){ //if left bracket is on top of the stack, stores operator immediately
    stackPush(s,c);
    return;
  }

  if((c == '*') || (c == '/')) {
    if((stack_top == '+') || (stack_top == '-')) {    //stores operator when there is operator with lower priority on top of the stack
      stackPush(s,c);
      return;
    }
    while((stack_top == '*') || (stack_top == '/')) { //if found operator with same priority on top of the stack, cycles until this condition is resolved
      postExpr[*postLen] = stack_top;                 //copies operator from stack to output string
      *postLen = *postLen + 1;
      stackPop(s);                                  //removes copied element from the stack
      if(stackEmpty(s)){
        break;                        //if all elements were removed, stop
      }
      stackTop(s,&stack_top);       //load new stack top for while-cycle condition
    }
    stackPush(s,c);               //stores operator if conditions are met
  }
  else {
    while((stack_top == '*') || (stack_top == '/') || (stack_top == '+') || (stack_top == '-')) { //if found operator with same priority on top of the stack, cycles until this condition is resolved
      postExpr[*postLen] = stack_top;   //copies operator from stack to output string
      *postLen = *postLen + 1;
      stackPop(s);                      //removes copied element from the stack
      if(stackEmpty(s)){
        break;                        //if all elements were removed, stop
      }
      stackTop(s,&stack_top);         //load new stack top for while-cycle condition
    }     
    stackPush(s,c);                 //stores operator if conditions are met
  }   
}

/* 
** Konverzní funkce infix2postfix.
** Čte infixový výraz ze vstupního řetězce infExpr a generuje
** odpovídající postfixový výraz do výstupního řetězce (postup převodu
** hledejte v přednáškách nebo ve studijní opoře). Paměť pro výstupní řetězec
** (o velikosti MAX_LEN) je třeba alokovat. Volající funkce, tedy
** příjemce konvertovaného řetězce, zajistí korektní uvolnění zde alokované
** paměti.
**
** Tvar výrazu:
** 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
**    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
**    násobení má stejnou prioritu jako dělení. Priorita násobení je
**    větší než priorita sčítání. Všechny operátory jsou binární
**    (neuvažujte unární mínus).
**
** 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
**    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
**
** 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
**    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
**    chybné zadání výrazu).
**
** 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen 
**    ukončovacím znakem '='.
**
** 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
**
** Poznámky k implementaci
** -----------------------
** Jako zásobník použijte zásobník znaků tStack implementovaný v příkladu c202. 
** Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
**
** Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
**
** Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
** char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
**
** Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
** nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
** by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
** ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte namísto
** řetězce konstantu NULL.
*/
char* infix2postfix (const char* infExpr) {

  int i = 0;        //stores index to input string
  char input;       //auxiliary variable to store input string character
  tStack* char_stack; //stack
  char* postExpr;     //output expression
  unsigned postLen = 0; //output expression length

  postExpr = malloc(MAX_LEN * sizeof(char));    //allocates memory for output string
  char_stack = (tStack*) malloc(sizeof(tStack));  //allocates memory for stack
  if((postExpr == NULL) || (char_stack == NULL)) {
    return NULL;
  }
  stackInit(char_stack);            //initializes stack before use

  while((input = infExpr[i]) != '='){   //read input string till = character is found
    i++;    //increments index to read whole input string
    if((input == '*') || (input == '/') || (input == '+') || (input == '-')) {    //if operator found 
      doOperation(char_stack,input,postExpr,&postLen);    //calls function that determines what to do
    }
    else if(input == '(') {   //if left bracket found
      stackPush(char_stack,input);
    }
    else if(input == ')') {     //if right bracket found
      untilLeftPar(char_stack,postExpr,&postLen); 
    }
    else if ((input >= '0' && input <= '9') || (input >= 'A' && input <= 'Z') || (input >= 'a' && input <= 'z')) {       //if operand found
      postExpr[postLen] = input;    //copy directly into output string
      postLen++;
    }

  }
  //after = character was found
  while(!(stackEmpty(char_stack))) {        //while-cycle copies all remaining elements from stack to output string
    stackTop(char_stack,&(postExpr[postLen]));
    postLen++;
    stackPop(char_stack);
  }

  free(char_stack);     //frees memory allocated for stack
  postExpr[postLen] = '=';  //adds = character to the end of output string
  postLen++;
  postExpr[postLen] = '\0'; //adds string-ending character
  return postExpr;        //returns converted string

  //solved = 0;                        /* V případě řešení smažte tento řádek! */
  //return NULL;                /* V případě řešení můžete smazat tento řádek. */
}

/* Konec c204.c */
