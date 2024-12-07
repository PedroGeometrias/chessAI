#ifndef STACK_H 
#define STACK_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// resize amount
#define RESIZE 10

// Já que a funcao de undu e redu não é necessaria, eu preciso coletar os erros em algum tipo de variavel, para
// caso eles acontecam eu possa apenas cancelar a habilidade do jogador utilizar essa funcao enquanto o jogo executa,
// assim o jogo nao quebra quando algum erro acontecer com a implementacao da stack
enum error{
    error_true = 1,
    error_false = 0,
};

// definicao de uma stack na memoria
typedef struct {
    // array de STRINGS
    char** arr;
    // tamanho atual
    int current_size;
    // index do topo
    int top;
}STACK;

// prototipos
int init_stack(STACK* stack, int initial_size);
int realloc_stack(STACK* stack);
int is_full(STACK* stack);
int is_empty(STACK* stack);
int push(STACK* stack, char* element);
char* pop(STACK* stack);
char* peek(STACK* stack);
void free_stack(STACK* stack);

#endif // COMMUNICATIONLAYER_H



