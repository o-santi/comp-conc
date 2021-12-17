/* Aluno: Leonardo Ribeiro Santiago
 * DRE: 120036072
 * 
 * Implementação do laboratório 5
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  5
#define TCOUNT  20

int condicao;

pthread_mutex_t p_mutex;
pthread_cond_t p_cond;


typedef struct _t_arg {
    int prioridade;
    char* frase;
} t_arg;


void * print_string(void* args){
  t_arg* arg = (t_arg *) args;
    
  pthread_mutex_lock(&p_mutex);
  while (arg->prioridade > condicao){
    pthread_cond_wait(&p_cond, &p_mutex);
  }
  condicao++;
  printf("%s\n", arg->frase);
  pthread_cond_broadcast(&p_cond);
  pthread_mutex_unlock(&p_mutex);
  pthread_exit(NULL);
}


/* Funcao principal */
int main(void) {
  pthread_t threads[NTHREADS];
  //aloca espaco para os identificadores das threads
  for (int i=0; i < NTHREADS; i++)
    threads[i] = (pthread_t) malloc(sizeof(pthread_t));
  
  /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&p_mutex, NULL);
  pthread_cond_init (&p_cond, NULL);
 
  /* Cria as threads */
  t_arg *args = malloc(sizeof(t_arg) * NTHREADS);
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
  /* Desaloca variaveis e termina */
  pthread_mutex_destroy(&p_mutex);
  pthread_cond_destroy(&p_cond);
}
