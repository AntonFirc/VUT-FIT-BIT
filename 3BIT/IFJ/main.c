/**
 * Hlavní soubor projektu
 * IFJ 2017 - 1. projekt
 * 
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 */
#include <stdio.h>
#include <string.h>
#include "definitions.h"
#include "tokens.h"
#include "helpers.h"
#include "parser.h"
#include "errors.h"
#include "instructions.h"
#include <ctype.h>
#include <stdlib.h>
#include "tokens.h"
#include "memory_stack.h"


typedef struct {
    int i;
} test;

int main(int argc, const char * argv[]) {
    /*char *test1 = malloc(sizeof(char) * 10);
    generate_string(test1, 10);
    printf("%s\n", test1);
    return 0;
    *//*char *test;
    test = malloc(sizeof(char *) * 100);
    test = "test";
    printf("%s %p", test, test);
    *//*int *test2;
    test = malloc(sizeof(int *) * 100);
    test = 100;
    printf("%s %p", test, test);
    */

    setbuf(stdout, NULL);
    context_stack = malloc(sizeof(Context_Stack));
    mem_store(context_stack);
    context_stack_init(context_stack);
    //FILE *file = fopen(argv[1], "rb");
    
    //////////////////// HACK PRO XCODE - stdin tam nejde pořádně pipovat
    if (argc > 1)
    {
        FILE * fp = freopen(argv[1], "r", stdin);
        if (fp == NULL)
        {
            perror(argv[1]);
            exit(1);
        }
    }
    parse_file();
    
    return 0;
}
