/* Aluno: Leonardo Ribeiro Santiago
 * DRE: 120036072
 * 
 * Implementação do laboratório 10
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFF_LEN 13

sem_t escrita, leitura, mutex;
int escritores=0;
int leitoras=0;

int N_CONSUMIDORAS;
int N_PRODUTORAS;
int N_THREADS;

int buffer[BUFF_LEN];
int head = 0;

void * consumidor(void * arg){
  int *id = (int *) arg;
  while (1) {
    sem_wait(&leitura);
    sem_wait(&mutex);
    usleep(50000); // dorme por 50 milisegundos
    printf("[CONSUMIDOR %d] consumiu o valor %d\n", *id, buffer[head]);
    head++;
    if (head == BUFF_LEN) {
      printf("[CONSUMIDOR %d] consumiu o último elemento\n", *id);
      sem_post(&escrita);
    }
    sem_post(&mutex);
  }
  pthread_exit(NULL);
}

void * produtor(void * arg){
  int *id = (int *) arg;
  while( 1 ) {
    sem_wait(&escrita);
    printf("[PRODUTOR  %d] produziu: [", *id);
    for (int i=0; i < BUFF_LEN; i++){
      buffer[i] = rand() % 10;
      printf("%d, ", buffer[i]);
    }
    printf("]\n");
    head = 0;
    for (int i=0; i < BUFF_LEN; i++){
      sem_post(&leitura);
    }
  }
  pthread_exit(NULL);
}

int main(int argc, char** argv) {
  if (argc < 3){
    printf("Uso: %s <n_escritoras> <n_leitoras>\n", argv[0]);
    exit(1);
  }
  srand(time(0));
  N_CONSUMIDORAS = atoi(argv[1]);
  N_PRODUTORAS = atoi(argv[2]);
  N_THREADS = N_CONSUMIDORAS + N_PRODUTORAS;
  
  pthread_t threads[N_THREADS];
  int id[N_THREADS];
  //aloca espaco para os identificadores das threads
  for (int i=0; i < N_THREADS; i++)
    threads[i] = (pthread_t) malloc(sizeof(pthread_t));
 
  /* Cria as threads */
  sem_init(&escrita, 0, 1);
  sem_init(&leitura, 0, 0);
  sem_init(&mutex,   0, 1);
  
  for (int i = 0; i < N_CONSUMIDORAS; i++){
    id[i] = i;
    pthread_create(&threads[i], NULL, consumidor, (void *) &id[i]);
  }
  for (int i = N_PRODUTORAS; i < N_THREADS; i++){
    id[i] = i;
    pthread_create(&threads[i], NULL, produtor, (void *) &id[i]);
  }
  /* Espera todas as threads completarem */
  for (int i = 0; i < N_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  pthread_exit(NULL);
}
