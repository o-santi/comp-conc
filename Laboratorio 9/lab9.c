/* Aluno: Leonardo Ribeiro Santiago
 * DRE: 120036072
 * 
 * Implementação do laboratório 5
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define NTHREADS  5
#define TCOUNT  20

typedef struct _t_arg {
  int prioridade;
  char* frase;
} t_arg;

sem_t semaforo;
int condicao = 0;


void * print_string(void* args){
  t_arg* arg = (t_arg *) args;
  while (arg->prioridade > condicao){
    sem_wait(&semaforo);
    sem_post(&semaforo);
  } // precisamos dar o post para evitar deadlock
  sem_wait(&semaforo); // aqui limitamos novamente, para que apenas uma thread print por vez.
  condicao++;
  printf("%s\n", arg->frase);
  sem_post(&semaforo);
  pthread_exit(NULL);
}


/* Funcao principal */
int main(void) {
  pthread_t threads[NTHREADS];
  //aloca espaco para os identificadores das threads
  for (int i=0; i < NTHREADS; i++)
    threads[i] = (pthread_t) malloc(sizeof(pthread_t));
 
  /* Cria as threads */
  sem_init(&semaforo, 0, 1);
  
  t_arg args[NTHREADS]; 
  args[0].frase = "Volte sempre!";
  args[1].frase = "Fique a vontade.";
  args[2].frase = "Sente-se por favor.";
  args[3].frase = "Aceita um copo d'agua?.";
  args[4].frase = "Seja bem-vindo!";
  args[0].prioridade = 4;
  args[1].prioridade = 1;
  args[2].prioridade = 1;
  args[3].prioridade = 1;
  args[4].prioridade = 0;
  for (int i = 0; i < NTHREADS; i++)
    pthread_create(&threads[i], NULL, print_string, (void *) (&args[i]));
 
  /* Espera todas as threads completarem */
  for (int i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  pthread_exit(NULL);
}
