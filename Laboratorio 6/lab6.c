/* Aluno: Leonardo Ribeiro Santiago */
/* DRE: 120 036 072*/

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define N 10

//variaveis para sincronizacao
pthread_mutex_t mutex;
pthread_cond_t cond_leit, cond_escr;

int n_threads_lendo = 0; //contador de threads lendo
int n_threads_escrevendo = 0; // contador de threads escrevendo

int vetor_principal[N];

//entrada leitura
void comecar_leitura(int id) {
   pthread_mutex_lock(&mutex);
   while(n_threads_escrevendo > 0) {
     pthread_cond_wait(&cond_leit, &mutex);
   }
   n_threads_lendo++;
   printf("[READ] thread %d terminou\n", id);
   pthread_mutex_unlock(&mutex);
}

//saida leitura
void terminar_leitura (int id) {
   pthread_mutex_lock(&mutex);
   if(n_threads_lendo==N){
     n_threads_lendo = 0;
     pthread_cond_broadcast(&cond_escr);
   }
   printf("[READ] thread %d terminou\n", id);
   pthread_mutex_unlock(&mutex);
}

//entrada escrita
void comecar_escrita (int id) {
   pthread_mutex_lock(&mutex);
   while(n_threads_lendo>0) {
     pthread_cond_wait(&cond_escr, &mutex);
   }
   n_threads_escrevendo++;
   printf("[WRITE] thread %d começou\n", id);
   pthread_mutex_unlock(&mutex);
}

//saida escrita
void terminar_escrita (int id) {
   pthread_mutex_lock(&mutex);
   if(n_threads_escrevendo == N){
     n_threads_escrevendo=0;
     pthread_cond_broadcast(&cond_leit);
   }
   printf("[WRITE] thread %d terminou\n", id);
   pthread_mutex_unlock(&mutex);
}


//thread leitora
void * funcao_thread (void * arg) {
  int *id = (int *) arg;
  int local_counter = 0;
  for (int i=0; i< N; i++){
    comecar_leitura(*id);
    for (int j=0; j < N; j++){
      local_counter += vetor_principal[j];
    }
    terminar_leitura(*id);
    comecar_escrita(*id);
    vetor_principal[*id] = rand() % 10;
    terminar_escrita(*id);
  }
  
  return (void *) local_counter;
}

//funcao principal
int main(void) {
  //identificadores das threads
  pthread_t tid[N];
  int id[N];
  srand(time(0));

  for (int i=0; i< N; i++)
    vetor_principal[i] = rand() % 10;

  //inicializa as variaveis de sincronizacao
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_leit, NULL);
  pthread_cond_init(&cond_escr, NULL);

  //cria as threads leitoras
  for(int i=0; i<N; i++) {
    id[i] = i;
    if(pthread_create(&tid[i], NULL, funcao_thread, (void *) &id[i])) exit(-1);
  }

  int retvals[N];
  for(int i=0; i<N; i++) {
    pthread_join(tid[i], (void **) &retvals[i]);
  }

  for (int i=0; i<N; i++) {
    if (retvals[0] != retvals[0]){
      fprintf(stderr, "[ERRO] Thread %d não retornou o mesmo valor que a thread 0.", i);
      fprintf(stderr, "[NOTA] Esperava: %d; Obteve: %d", retvals[0], retvals[i]);
      exit(1);
    }
  }
  printf("[SUCESSO] Todos os valores foram retornados corretamente\n");
  
  pthread_exit(NULL);
  return 0;
}
 
