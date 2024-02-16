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
//BUFFER_SIZE 20 // Númermo máximo de tarefas enfileiradas

typedef struct Clock{ //Define estrutura de relógio
   int T1, T2,T3;
   int preenchido;
   struct Clock* prox;
}RegVet;
typedef struct{ //Define fila de relógios
    RegVet* cabeca, *cauda;
}Fila_Clock;

void cria_fila(Fila_Clock* fila){//Define inicio da fila
    Fila_Clock->inicio=NULL;
}
void Produz_Relogio(Fila_Clock* fila, int index_buffer){
    //cria relogio e atribui valores
    RegVet* novo_clock=NULL;
    novo_clock = malloc(sizeof(RegVet));
    novo_clock->T1=rand()%100;
    novo_clock->T2=rand()%100;
    novo_clock->T3=rand()%100;
    novo_clock->preenchido = ++index_buffer; //se index_buffer==20: bloqueia produção

    //caso a fila esteja preenchida com mais de um elemento
    if(fila->cabeca!=NULL && fila->cabeca!=fila->cauda){
        fila->cauda->prox=novo_clock;
        fila->cauda=novo_clock;
    }
    //caso fila vazia
    else if(fila->cabeca==NULL){
        fila->cabeca=novo_clock;
        fila->cauda=novo_clock;
    }

    //caso cabeca==cauda (apenas 1 elemento)
    else{
        fila->cabeca->prox=novo_clock;
        fila->cauda=novo_clock;//cauda aponta para novo elemento inserido
        }

    printf("Relogio produzido: [%d, %d, %d]\n", fila->cauda->T1, fila->cauda->T2,fila->cauda->T3);
    
}
void Consome_Relogio(Fila_Clock* fila,int index_buffer){
    printf("Relogio consumido: [%d, %d, %d]\n", fila->cabeca->T1, fila->cabeca->T2,fila->cabeca->T3);
    Fila_Clock* temp_clock;//relogio temporário pra guardr referencia
    
    temp_clock=fila->cabeca;
    fila->cabeca=fila->cabeca->prox;//cabeca aponta pro próximo valor da fila
    free(temp_clock); //libera espaço
    temp_clock=NULL;
    index_buffer--;
}
