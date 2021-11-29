/* Laboratório 3

  Autor: Leonardo Santiago 
 */

#include "timer.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>


typedef long long int vec_size;
// só pra não ter que ficar
// escrevendo long long toda vez.

float * vetor;
vec_size tamanho;
int nthreads;
float lim_inferior, lim_superior;


void * tarefa(void *args){
  long int id = (long int) args;
  vec_size bloco = tamanho / nthreads;
  vec_size start = id * bloco;
  vec_size *soma_local= malloc(sizeof(vec_size));
  *soma_local = 0;
  if (id == (nthreads -1))
    bloco += tamanho % nthreads;
  for (vec_size i=start; i < start + bloco; i++){
    if (vetor[i] > lim_inferior && vetor[i] < lim_superior)
      (*soma_local)++;
  }
  pthread_exit((void*) soma_local);
}

vec_size resolucao_sequencial(void){
  vec_size quantidade = 0;
  for (vec_size i = 0; i < tamanho; i++)
    if (vetor[i] > lim_inferior && vetor[i] < lim_superior)
      quantidade++;
  return quantidade;
}

vec_size resolucao_concorrente(void){
  vec_size soma_global = 0;
  vec_size *retorno;
  pthread_t *tid = malloc(sizeof(pthread_t) * nthreads);

  if(tid==NULL) {
    fprintf(stderr, "ERRO--erro ao criar o vetor tid\n");
    exit(1);
  }

  // usando long int só para não reclamar da conversão pra void pointer
  for (long int id= 0; id < nthreads; id++){
    if (pthread_create(tid+id,NULL, tarefa, (void*) id)){
      fprintf(stderr, "ERRO--erro ao criar a thread número %ld\n", id);
      exit(1);
    }
  }
  for (long int id=0; id < nthreads; id++){
    if(pthread_join(*(tid+id), (void**) &retorno)){
      fprintf(stderr, "ERRO--pthread_create\n");
      exit(1);
    }
    //soma global
    soma_global += *retorno;
  }
  free(tid);
  return soma_global;
}

int main(int argc, char* argv[]) {
  double inicio, fim;
  srand(time(0));
  if (argc < 3){
    printf("Digite: %s <tamanho do vetor> <numero de threads>\n", argv[0]);
    return 1;
  }
  tamanho = atoll(argv[1]); 
  nthreads = atoi(argv[2]);

  // inicializamos o vetor
  vetor = malloc(sizeof(float) * tamanho);
  if (vetor == NULL){
    fprintf(stderr, "ERRO -- não foi possivel inicializar o vetor com %lld posições\n", tamanho);
    exit(1);
  }

  for (vec_size i=0; i<tamanho; i++){
    vetor[i] = 255 * ((float) rand() / RAND_MAX) ; // números aleatórios entre 0 e 255
  }
  
  // inicializamos os limites
  printf("Insira o limite inferior:");
  scanf("%f", &lim_inferior);
  printf("Insira o limite superior:");
  scanf("%f", &lim_superior);
  printf("\n");

  // rodamos a resolução sequencial e guardamos o resultado numa variavel
  GET_TIME(inicio);
  vec_size quant_seq = resolucao_sequencial();
  GET_TIME(fim);
  printf("SEQUENCIAL: %.5fs\n", fim-inicio);

  // criamos as threads e escalonamos
  GET_TIME(inicio);
  vec_size quant_conc = resolucao_concorrente();
  GET_TIME(fim);
  if (quant_conc != quant_seq){
    printf("Ocorreu algo de errado ao executar a função concorrente.\n");
    return 2;
  }
  printf("CONCORRENTE: %.5fs\n", fim-inicio);

  free(vetor);
  
  return 0;
}
