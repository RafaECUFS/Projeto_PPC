#include <stdio.h>
#include <string.h>  
#include <mpi.h>     


typedef struct { 
   int p[3];
} Clock;


void Event(int pid, Clock *clock){
   clock->p[pid]++;   
}


void Send(int pid, Clock *clock, int dest){
   // Incrementar o contador do processo que Eu envio
   Event(pid, clock);
   // Eu envio o vetor do relógio junto com a mensagem
   MPI_Send(clock, 3, MPI_INT, dest, 0, MPI_COMM_WORLD);
}

void Receive(int pid, Clock *clock, int source){
   Clock temp;
   int i;
   // Receber o vetor do relógio junto com a mensagem
   MPI_Recv(&temp, 3, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   // Atualizar o vetor do relógio com o máximo entre os valores recebidos e os atuais
   for (i = 0; i < 3; i++){
      if (temp.p[i] > clock->p[i]){
         clock->p[i] = temp.p[i];
      }
   }
   // Incrementar o contador do processo que eu recebo
   Event(pid, clock);
}

// Representa o processo de rank 0
void process0(){
   Clock clock = {{0,0,0}};
   Event(0, &clock); // *Evento a (1,0,0)
   printf("Processo: %d, Clock: (%d, %d, %d)\n", 0, clock.p[0], clock.p[1], clock.p[2]);
   
   // Eu envio uma mensagem para o processo 1
   Send(0, &clock, 1); // *Evento b (2,0,0)
   printf("Processo: %d, Clock: (%d, %d, %d)\n", 0, clock.p[0], clock.p[1], clock.p[2]);
   
   // Eu recebo uma mensagem do processo 1
   Receive(0, &clock, 1); // *Evento c (3,1,0)
   printf("Processo: %d, Clock: (%d, %d, %d)\n", 0, clock.p[0], clock.p[1], clock.p[2]);

   // Eu envio uma mensagem para o processo 2
   Send(0, &clock, 2); // *Evento d (4,1,0)
   printf("Processo: %d, Clock: (%d, %d, %d)\n", 0, clock.p[0], clock.p[1], clock.p[2]);
   
   // Eu recebo uma mensagem do processo 2
   Receive(0, &clock, 2); // *Evento e (5,1,2)
   printf("Processo: %d, Clock: (%d, %d, %d)\n", 0, clock.p[0], clock.p[1], clock.p[2]);
   
   // Eu envio uma mensagem para o processo 1
   Send(0, &clock, 1); // *Evento f (6,1,2)
   printf("Processo: %d, Clock: (%d, %d, %d)\n", 0, clock.p[0], clock.p[1], clock.p[2]);
   
   Event(0, &clock); // *Evento g (7,1,2)
   printf("Processo: %d, Clock: (%d, %d, %d)\n", 0, clock.p[0], clock.p[1], clock.p[2]);
   
}

// Representa o processo de rank 1
void process1(){
   Clock clock = {{0,0,0}};
   Send(1 , &clock , 0); // *Evento h (0,1,0)
   printf("Processo: %d, Clock: (%d, %d, %d)\n", 1, clock.p[0], clock.p[1], clock.p[2]);

   // Eu recebo uma mensagem do processo 0
   Receive(1, &clock, 0); // *Evento i (2,2,0)
   printf("Processo: %d, Clock: (%d, %d, %d)\n", 1, clock.p[0], clock.p[1], clock.p[2]);

   // Eu recebo uma mensagem do processo 0
   Receive(1, &clock, 0); // *Evento j (6,3,2)
   printf("Processo: %d, Clock: (%d, %d, %d)\n", 1, clock.p[0], clock.p[1], clock.p[2]);
}

// Representa o processo de rank 2
void process2(){
   Clock clock = {{0,0,0}};
   Event(2, &clock); // *Evento k (0,0,1)
   printf("Processo: %d, Clock: (%d, %d, %d)\n", 2, clock.p[0], clock.p[1], clock.p[2]);

   // Eu envio uma mensagem para o processo 0
   Send(2, &clock, 0); // *Evento l (0,0,2)
   printf("Processo: %d, Clock: (%d, %d, %d)\n", 2, clock.p[0], clock.p[1], clock.p[2]);
   
   // Eu recebo uma mensagem do processo 0
   Receive(2, &clock, 0); // *Evento m (4,1,3)
   printf("Processo: %d, Clock: (%d, %d, %d)\n", 2, clock.p[0], clock.p[1], clock.p[2]);
}

int main(void) {
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

   /* Finaliza MPI */
   MPI_Finalize(); 

   return 0;
} /* main */
