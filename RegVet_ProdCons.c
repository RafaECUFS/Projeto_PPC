/* File:  
 *    RegVet_ProdCons.c
 *
 * Purpose:
 *    Implementação de um pool de threads
 *
 *           mpicc -Wall -o RegVet_ProdCons RegVet_ProdCons.c -lpthread -lrt
 * Compile:  mpicc RegVet_ProdCons.c -o RegVet_ProdCons -lpthread -lrt
 * Usage:    mpiexec -n 3 ./Produtor_Consumidor
 */
 
 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <unistd.h>
#include <semaphore.h>
#include <time.h>  
#include <mpi.h>
#define THREAD_NUM 3   // Tamanho do pool de threads
#define BUFFER_SIZE 20 // Númermo máximo de tarefas enfileiradas



typedef struct Clock{ //Define estrutura de relógio
   //int T1, T[2],T[3];
   int T[3];
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
    novo_clock->T[1]=rand()%100;
    novo_clock->T[2]=rand()%100;
    novo_clock->T[3]=rand()%100;
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

    printf("Relogio produzido: [%d, %d, %d]\n", fila->cauda->T[1], fila->cauda->T[2],fila->cauda->T[3]);
    
    
    

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condEmpty);
}

void Consome_Relogio(Fila_Clock *fila,int id){
    pthread_mutex_lock(&mutex);
   
    while (fila->tamanho_fila == 0){
       pthread_cond_wait(&condEmpty, &mutex);
    }
   
   
    printf("(Thread %d)Relogio consumido: [%d, %d, %d]\n", id, fila->cabeca->T[1], fila->cabeca->T[2],fila->cabeca->T[3]);
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

























void Event(int pid, RegVet *clock){
   clock->T[pid]++;   
}


void Send(int pid, RegVet *clock, int dest){
   // Incrementar o contador do processo que Eu envio
   clock->T[pid]++;
   // Eu envio o vetor do relógio junto com a mensagem
   MPI_Send(clock, 3, MPI_INT, dest, 0, MPI_COMM_WORLD);
}

void Receive(int pid, RegVet *clock, int source){
   RegVet temp;
   int i;
   // Receber o vetor do relógio junto com a mensagem
   MPI_Recv(&temp, 3, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   // Atualizar o vetor do relógio com o máximo entre os valores recebidos e os atuais
   for (i = 0; i < 3; i++){
      if (temp.T[i] > clock->T[i]){
         clock->T[i] = temp.T[i];
      }
   }
   // Incrementar o contador do processo que eu recebo
   clock->T[pid]++;
}

// Representa o processo de rank 0
void process0(){
   RegVet clock = {{0,0,0}};
   Event(0, &clock); // *Evento a (1,0,0)
   printf("Processo: %d, RegVet: (%d, %d, %d)\n", 0, clock.T[0], clock.T[1], clock.T[2]);
   
   // Eu envio uma mensagem para o processo 1
   Send(0, &clock, 1); // *Evento b (2,0,0)
   printf("Processo: %d, RegVet: (%d, %d, %d)\n", 0, clock.T[0], clock.T[1], clock.T[2]);
   
   // Eu recebo uma mensagem do processo 1
   Receive(0, &clock, 1); // *Evento c (3,1,0)
   printf("Processo: %d, RegVet: (%d, %d, %d)\n", 0, clock.T[0], clock.T[1], clock.T[2]);

   // Eu envio uma mensagem para o processo 2
   Send(0, &clock, 2); // *Evento d (4,1,0)
   printf("Processo: %d, RegVet: (%d, %d, %d)\n", 0, clock.T[0], clock.T[1], clock.T[2]);
   
   // Eu recebo uma mensagem do processo 2
   Receive(0, &clock, 2); // *Evento e (5,1,2)
   printf("Processo: %d, RegVet: (%d, %d, %d)\n", 0, clock.T[0], clock.T[1], clock.T[2]);
   
   // Eu envio uma mensagem para o processo 1
   Send(0, &clock, 1); // *Evento f (6,1,2)
   printf("Processo: %d, RegVet: (%d, %d, %d)\n", 0, clock.T[0], clock.T[1], clock.T[2]);
   
   Event(0, &clock); // *Evento g (7,1,2)
   printf("Processo: %d, RegVet: (%d, %d, %d)\n", 0, clock.T[0], clock.T[1], clock.T[2]);
   
}

// Representa o processo de rank 1
void process1(){
   RegVet clock = {{0,0,0}};
   Send(1 , &clock , 0); // *Evento h (0,1,0)
   printf("Processo: %d, RegVet: (%d, %d, %d)\n", 1, clock.T[0], clock.T[1], clock.T[2]);

   // Eu recebo uma mensagem do processo 0
   Receive(1, &clock, 0); // *Evento i (2,2,0)
   printf("Processo: %d, RegVet: (%d, %d, %d)\n", 1, clock.T[0], clock.T[1], clock.T[2]);

   // Eu recebo uma mensagem do processo 0
   Receive(1, &clock, 0); // *Evento j (6,3,2)
   printf("Processo: %d, RegVet: (%d, %d, %d)\n", 1, clock.T[0], clock.T[1], clock.T[2]);
}

// Representa o processo de rank 2
void process2(){
   RegVet clock = {{0,0,0}};
   Event(2, &clock); // *Evento k (0,0,1)
   printf("Processo: %d, RegVet: (%d, %d, %d)\n", 2, clock.T[0], clock.T[1], clock.T[2]);

   // Eu envio uma mensagem para o processo 0
   Send(2, &clock, 0); // *Evento l (0,0,2)
   printf("Processo: %d, RegVet: (%d, %d, %d)\n", 2, clock.T[0], clock.T[1], clock.T[2]);
   
   // Eu recebo uma mensagem do processo 0
   Receive(2, &clock, 0); // *Evento m (4,1,3)
   printf("Processo: %d, RegVet: (%d, %d, %d)\n", 2, clock.T[0], clock.T[1], clock.T[2]);
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

void *startRelogioThread(void* args){
    long id = (long) args; 
    while (1){ 
      int my_rank;               

      MPI_Init(NULL, NULL); 
      MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

      if (my_rank == 0) { 
        process0();
      } else if (my_rank == 1) {  
        process1();
      } else if (my_rank == 2) {  
        process2();
      }
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


int main(void) {
    
    pthread_mutex_init(&mutex, NULL);
   
    pthread_cond_init(&condEmpty, NULL);
    pthread_cond_init(&condFull, NULL);

    pthread_t produtorThread[THREAD_NUM];
    pthread_t relogioThread[THREAD_NUM];
    pthread_t consumidorThread[THREAD_NUM]; 
    long i;
    for (i = 0; i < THREAD_NUM; i++){  
        if (pthread_create(&produtorThread[i], NULL, &startProdutorThread, (void*) i) != 0)
        {
          perror("Failed to create the producer thread");
        }  
    }

    for (i = 0; i < THREAD_NUM; i++){  
        if (pthread_create(&relogioThread[i], NULL, &startRelogioThread, (void*) i) != 0)
        {
          perror("Failed to create the consumer thread");
        }  
    }

    for (i = 0; i < THREAD_NUM; i++){  
        if (pthread_create(&consumidorThread[i], NULL, &startConsumidorThread, (void*) i) != 0)
        {
          perror("Failed to create the consumer thread");
        }  
    }
    
    Inicia_Producao_Inicial(&fila_);
   
    for (i = 0; i < THREAD_NUM; i++){  
        if (pthread_join(produtorThread[i], NULL) != 0) {
            perror("Failed to join the producer thread");
        }  
    }

    for (i = 0; i < THREAD_NUM; i++){  
        if (pthread_join(consumidorThread[i], NULL) != 0) {
            perror("Failed to join the consumer thread");
        }  
    }
    
    MPI_Finalize(); 
   
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condEmpty);
    pthread_cond_destroy(&condFull);
   
   
    return 0;
} /* main */