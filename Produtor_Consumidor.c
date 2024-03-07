/* File:  
 *    Produtor_Consumidor.c
 *
 * Purpose:
 *    Implementação de um pool de threads
 *
 *
 * Compile:  gcc -g -Wall -o Produtor_Consumidor Produtor_Consumidor.c -lpthread -lrt
 * Usage:    ./Produtor_Consumidor
 
Com base no exemplo pth_pool.c, implemente o modelo de comunicação entre threads 
Produtor/Consumidor usando uma fila intermediária controlada por variáveis de condição. 
Enquanto um grupo de 3 threads produzirá relógios vetoriais que serão colocados na fila, 
outro grupo de 3 threads consumirá esses relógios da fila e os imprimirá na saída padrão. 
Para verificar o uso correto das variáveis de condição na sua tarefa de sincronização 
entre produtores e consumidores, crie dois cenários de teste:
 
  Cenário onde a fila ficará cheia: Threads produtoras produzem relógios na fila em uma 
  taxa maior que as threads consumidoras. Por exemplo, em quanto cada thread produtora 
  produz 1 relógio na fila a cada segundo, cada thread consome 1 relógio da fila a cada 
  dois segundos. 

  Cenário onde a fila ficará vazia: Threads produtoras produzem relógios na fila em uma
  taxa menor que as threads consumidoras. Por exemplo, em quanto cada thread produtora 
  produz 1 relógio na fila a cada 2 segundos, cada thread consome 1 relógio da fila a
  cada 1 segundo. 
 
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
   int preenchido;
   struct Clock* prox;
}RegVet;

typedef struct{ //Define fila de relógios
    RegVet* cabeca, *cauda;
    int tamanho_fila; //Marcador do tamanho da fila
}Fila_Clock;

//Para o pthreads--------------------------------------
int index_buffer = 0;

pthread_mutex_t mutex;

pthread_cond_t condFull;
pthread_cond_t condEmpty;
//Para o pthreads--------------------------------------


Fila_Clock fila_ = {NULL,NULL,0};


void Produz_Relogio(Fila_Clock *fila){
    pthread_mutex_lock(&mutex);

    while (fila->tamanho_fila == BUFFER_SIZE){
       pthread_cond_wait(&condFull, &mutex);
    }
    
    
    
    //cria relogio e atribui valores
    RegVet* novo_clock=NULL;
    novo_clock = malloc(sizeof(RegVet));
    novo_clock->T1=rand()%100;
    novo_clock->T2=rand()%100;
    novo_clock->T3=rand()%100;
    novo_clock->preenchido = ++index_buffer; //se index_buffer==20: bloqueia produção
    fila->tamanho_fila++;

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
    
    
    

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condEmpty);
}

void Consome_Relogio(Fila_Clock *fila,int id){
    pthread_mutex_lock(&mutex);
   
    while (fila->tamanho_fila == 0){
       pthread_cond_wait(&condEmpty, &mutex);
    }
   
   
    printf("(Thread %d)Relogio consumido: [%d, %d, %d]\n", id, fila->cabeca->T1, fila->cabeca->T2,fila->cabeca->T3);
    RegVet* temp_clock;//relogio temporário pra guardr referencia
    
    temp_clock=fila->cabeca;
    fila->cabeca=fila->cabeca->prox;//cabeca aponta pro próximo valor da fila
    free(temp_clock); //libera espaço
    temp_clock=NULL;
    index_buffer--;
    fila->tamanho_fila--;
    
    
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condFull);
}





void *startProdutorThread(void* args){
   long id = (long) args; 
   while (1){ 
      Produz_Relogio(&fila_);
      sleep(rand()%2);
   }
   return NULL;
}

void *startConsumidorThread(void* args){
   long id = (long) args; 
   while (1){ 
      Consome_Relogio(&fila_, id);
      sleep(rand()%2);
   }
   return NULL;
}
void Inicia_Producao_Inicial(Fila_Clock *fila) {
    int i;
    for (i = 0; i < 50; i++) {
        Produz_Relogio(fila);
    }
}
/*--------------------------------------------------------------------*/
int main(int argc, char* args[]){
    pthread_mutex_init(&mutex, NULL);
   
    pthread_cond_init(&condEmpty, NULL);
    pthread_cond_init(&condFull, NULL);

    pthread_t produtorThread[THREAD_NUM / 2];
    pthread_t consumidorThread[THREAD_NUM / 2]; 
    long i;
    for (i = 0; i < THREAD_NUM / 2; i++){  
        if (pthread_create(&produtorThread[i], NULL, &startProdutorThread, (void*) i) != 0)
        {
          perror("Failed to create the producer thread");
        }  
    }

    for (i = 0; i < THREAD_NUM / 2; i++){  
        if (pthread_create(&consumidorThread[i], NULL, &startConsumidorThread, (void*) i) != 0)
        {
          perror("Failed to create the consumer thread");
        }  
    }
    
   Inicia_Producao_Inicial(&fila_);
   
   for (i = 0; i < THREAD_NUM / 2; i++){  
        if (pthread_join(produtorThread[i], NULL) != 0) {
            perror("Failed to join the producer thread");
        }  
    }

    for (i = 0; i < THREAD_NUM / 2; i++){  
        if (pthread_join(consumidorThread[i], NULL) != 0) {
            perror("Failed to join the consumer thread");
        }  
    }
   
   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&condEmpty);
   pthread_cond_destroy(&condFull);
   
    return 0;
}
