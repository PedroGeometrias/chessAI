// Autor : Pedro de oliveira haro
// Motivo : Uma stack é a estrutura de dados perfeita para representar a funcao de undu e redu 
// do meu jogo de xadrez, essa implementacao da stack precisa ser modular pois podem acontecer um 
// numero variado de possiveis jogadas
#include "stack.h"

// inicializando a stack na memoria
int init_stack(STACK* stack, int initial_size){
    stack->current_size = initial_size;
    stack->arr = malloc(sizeof(char* ) * stack->current_size);
    if(!stack->arr){
        printf("Erro alocando memoria para a stack, funcao init_stack\n");
        return error_true;
    }

    // estado inicial da stack nao possui elementos
    stack->top = -1; 

    // nenhum erro aconteceu
    return error_false;
}

// se o numero de elementos exceder o tamanho da stack, eu aumento o tamanho da stack 
int realloc_stack(STACK* stack){
    if(stack->top + 1 >= stack->current_size){
        char** new_arr = realloc(stack->arr, sizeof(char* ) * (stack->current_size + RESIZE));
        if(new_arr == NULL){
            printf("Erro alocando memoria para a stack, funcao realloc_stack\n");
            return error_true;
        }
        stack->arr =new_arr;
        stack->current_size += RESIZE;
    }

    return error_false;
}

// checo se a stack esta completa
int is_full(STACK* stack){
    return stack->top + 1 == stack->current_size;
}

// checo se ela esta vazia
int is_empty(STACK* stack){
    return stack->top == -1;
}

// adiciono uma string a stack 
int push(STACK* stack, char* element){
    // realocando memoria se necessario
    if (realloc_stack(stack)){
        printf("Erro ao redimensionar a stack\n");
        return error_true;
    }

    // aumentando o topo
    stack->top++;
    // tamanho da nova string 
    int len = strlen(element) + 1;

    // alocando memoria para a string a ser colocada no topo
    stack->arr[stack->top] = malloc(len);
    if (!stack->arr[stack->top]){
        printf("Erro alocando memoria, funcao push\n");
        stack->top--;
        return error_true;
    }

    // copiando o elemento ao topo 
    strcpy(stack->arr[stack->top], element);
    return error_false;
}

// detorno o ultimo valor retirando ele da stack
char* pop(STACK* stack){
    if (is_empty(stack)){
        printf("Stack esta vazia\n");
        return NULL; 
    }

    // alocando memoria para a copia
    int len = strlen(stack->arr[stack->top]);
    char* popped = (char*) malloc(len + 1); 
    if (!popped) {
        printf("Erro alocando memoria para a string\n");
        return NULL;
    }

    // copiando string
    strcpy(popped, stack->arr[stack->top]);

    // limpando memeoria
    free(stack->arr[stack->top]);

    // decrementando contador do topo
    stack->top--;

    // retornando uma copia do elemento destruido
    return popped;
}

// retorno o ultimo valor sem remover ele da stack
char* peek(STACK* stack){
    if(is_empty(stack)){
        printf("Stack esta vazia\n");
        return NULL;
    }
    // retornando elemento do topo
    return stack->arr[stack->top];
}

// limpo a memoria da stack depois de utilizar ela
void free_stack(STACK* stack){
    // limpo todas as strings
    for(int i = 0; i <= stack->top; ++i){
        free(stack->arr[i]);
    }

    // limpo a array de strings
    free(stack->arr);

    // evito ponteiro balancando
    stack->arr = NULL;
    stack->current_size = 0;
    stack->top = -1;
}
