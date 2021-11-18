/*
  Aluno: Leonardo Ribeiro Santiago
  DRE: 120036072

  eu uso _ pra maior parte das variáveis e nomes
  péssima ideia. eu sei, mas eu queria utilizar -
  igual em lisp. mas não pode. não querem fazer compiladores com - nos identificadores
  então me contento com _
 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS  2
#define VEC_SIZE 10000

typedef struct _t_args{
  int tamanho;
  int *inicio;
  
} t_args;

// função que eleva ao quadrado.
void * elevar_ao_quadrado(void * arg_struct){
  t_args* args = (t_args *) arg_struct;
  
  for (int i = 0; i < args->tamanho; i++){
    args->inicio[i] = args->inicio[i] * args->inicio[i]; 
  }
  free(arg_struct);
  pthread_exit(NULL);
}

int main(void){
  pthread_t tid_sistema[NTHREADS];
  t_args *arg;
  int i;

  int vetor_inicial[VEC_SIZE];
  for (i = 0; i< VEC_SIZE; i++)
    vetor_inicial[i] = i; // inicializa com valores genéricos
  
  /* a ideia principal deste programa é escolher partes diferentes do vetor
     para cada thread calcular, pois se duas threads mexerem na mesma posição
     de memória, coisas horripilantes acontecerão!!! */
  for (int thread = 0; thread < NTHREADS; thread++){
    arg = malloc(sizeof(t_args));
    arg->tamanho = VEC_SIZE / NTHREADS;
    /* dividimos o vetor em partes iguais e distribuimos cada uma a uma thread diferente
       se eu quisesse ser 100% correto, precisaria aqui levar em conta o módulo
       VEC_SIZE % NTHREADS e adicionar isso a alguma thread (a última provavelmente)
       mas acho que não precisa desse empenho todo já que 10000 % 2 = 0 (magia dos numeros pares) */
    arg->inicio = &(vetor_inicial[arg->tamanho * thread]);
    printf(" -- Criando a thread %d, que irá calcular da posição %d até %d\n", thread, arg->tamanho * thread, arg->tamanho * (thread+ 1) -1);

    if (pthread_create(&tid_sistema[thread], NULL, elevar_ao_quadrado, (void *) arg))
      printf(" -- ERRO: erro ao criar a thread\n");
  }

  for (int thread= 0; thread < NTHREADS; thread++){
    if (pthread_join(tid_sistema[thread], NULL))
      printf(" -- ERRO: erro ao dar join\n");
  }
  printf("Checando resultados: \n");
  for (i = 0; i < VEC_SIZE; i++){
    //printf("%d ", vetor_inicial[i]);
    if (vetor_inicial[i] != i*i){
      // sanity check
      break;
    }
  }
  if (i == VEC_SIZE)
    printf("Todos as posições foram calculadas corretamente");
  else
    printf("Errou seu bunda mole");
  return 0;
}
