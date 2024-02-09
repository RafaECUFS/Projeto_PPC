/* File:  
 *    pth_pool.c
 *
 * Purpose:
 *    Implementação de um pool de threads
 *
 *
 * Compile:  gcc -g -Wall -o pth_pool pth_pool.c -lpthread -lrt
 * Usage:    ./pth_hello
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#define THREAD_NUM 6    // Tamanho do pool de threads
#define BUFFER_SIZE 20 // Númermo máximo de tarefas enfileiradas

typedef struct Clock{ //Define estrutura de relógio
   int T1, T2,T3;
   struct Clock* prox;
}RegVet;
typedef struct{ //Define fila de relógios
    RegVet* cabeca, *cauda;
}Fila_Clock;

void cria_fila(Fila_Clock* fila){//Define inicio da fila
    Fila_Clock->inicio=NULL;
}
void Consome_Relogio(Fila_Clock* fila){
    Fila_Clock* novo_clock = malloc(sizeof(RegVet));
    
    fila->cauda->prox=novo_clock;
    lista->cauda=novo_clock;//cauda aponta para novo elemento inserido

    printf("[%d, %d, %d]\n", fila->cabeca->T1, fila->cabeca->T2,fila->cabeca->T3);
    
}
