/* Laboratório 2

  Autor: Leonardo Santiago 
 */

#include "timer.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

float *saida_concorrente;
float *saida_sequencial;
float *matriz1;
float *matriz2;
pthread_t *tid; // identificador de thread

typedef struct {
  int id;
  int dim;
  int nthreads;
} tArgs;

void * tarefa(void *arg){
  tArgs *args = (tArgs*) arg;
  int dim = args->dim;
  int nthreads = args->nthreads;
  for (int i=args->id; i < dim; i += nthreads)
    for (int j=0; j< dim; j++)
      for (int k=0; k < dim; k++)
	saida_concorrente[i*dim + k] += matriz1[i * dim + k] * matriz2[k*dim + j]; 
  pthread_exit(NULL);
}

void multiplicacao_concorrente(int dim, int nthreads){
  tArgs *args = malloc(sizeof(tArgs) * nthreads);
  pthread_t *tid = malloc(sizeof(pthread_t) * nthreads);
   
  for (int id=0; id < nthreads; id++){
    (args+id)->id = id;
    (args+id)->dim = dim;
    (args+id)->nthreads = nthreads;

    if (pthread_create(tid+id,NULL, tarefa,  (void*) (args + id) )){
      printf("Erro ao criar thread"); exit(3);
    }
  }
  for(int i=0; i<nthreads; i++) {
    pthread_join(*(tid+i), NULL);
  }
  free(args);
  free(tid);
} 


void multiplicacao_sequencial(int dim){
  for (int i=0; i < dim; i++){
    for (int j=0; j < dim; j++)
      for (int k=0; k < dim; k++)
	saida_sequencial[i*dim + k] += matriz1[i * dim + k] * matriz2[k*dim + j]; 
  }
}

int checar_saida(int dim){
  int i, j;
  for (i=0; i < dim; i++){
    for (j=0; j < dim; j++){
      if (saida_concorrente[i*dim + j] != saida_sequencial[i*dim + j])
	break;
    }
  }
  if (i == dim && j == dim)
    return 0;
  else{
    printf("%d %d %d", i, j, dim);
    return 1;
  }
}

void inicializar_variaveis(int dim){
  srand(time(0));
  matriz1 = (float *) malloc(sizeof(float) * dim * dim);
  if(matriz1==NULL) {puts("ERRO--malloc"); exit(2);}

  matriz2 = (float*) malloc(sizeof(float) * dim * dim);
  if(matriz2==NULL) {puts("ERRO--malloc"); exit(2);}

  saida_concorrente = (float *) calloc(sizeof(float), dim * dim); // matriz de saida começa inteira com zero
  if(saida_concorrente==NULL) {puts("ERRO--malloc"); exit(2);}

  saida_sequencial = (float *) calloc(sizeof(float), dim * dim); // matriz de saida começa inteira com zero
  if(saida_sequencial ==NULL) {puts("ERRO--malloc"); exit(2);}
  
  for(int i=0; i<dim; i++) {
    for(int j=0; j<dim; j++){
      matriz1[i*dim+j] = (float) 255 * rand()/RAND_MAX; // números aleatórios entre 0 e 255 
      matriz2[i*dim+j] = (float) 255 * rand()/RAND_MAX;
    }
  }
}

void print_matrix(float * matrix, int dim){
  for (int i=0; i < dim; i++){
    for (int j=0; j < dim; j++)
      printf("%.1f ", matrix[i*dim + j]);
    printf("\n");
  }
}

int main(int argc, char* argv[]) {
  int dim, nthreads;
  double fim, inicio;

  if (argc < 3){
      printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
      return 1;
  }

  dim = atoi(argv[1]);
  nthreads = atoi(argv[2]);
  if (nthreads > dim) nthreads=dim; 
  inicializar_variaveis(dim);  
  
  GET_TIME(inicio);
  multiplicacao_concorrente(dim, nthreads);
  GET_TIME(fim);
  
  printf("CONCORRENTE: A solução concorrente levou %.4fs\n", fim - inicio);
  
  GET_TIME(inicio);
  multiplicacao_sequencial(dim);
  GET_TIME(fim);
  printf("SEQUENCIAL: A solução sequencial levou %.4fs\n", fim - inicio);

  if (checar_saida(dim))
    printf("Algo de errado ocorreu na multiplicação concorrente");
  else
    printf("A multiplicação ocorreu corretamente\n");

  //print_matrix(saida_sequencial, dim);
  //print_matrix(saida_concorrente, dim);
  
  free(matriz1);
  free(matriz2);
  free(saida_concorrente);
  free(saida_sequencial);
  free(tid);
  
  return 0;
}
