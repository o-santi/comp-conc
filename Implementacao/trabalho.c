/*Caio Gomes Monteiro - 120036373
Leonardo Santiago- 120036072*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include"timer.h"
#include<time.h>
#include<math.h>

int n_threads; //Quantidade de threads a serem utilizadas.
double comeco_intervalo, fim_intervalo; //Começo e fim do intervalo, respectivamente.
int grau_da_funcao; //Grau da função polinomial a ser utilizada pelo programa.
double *vetor_coeficientes; //Vetor que possui os coeficientes em ordem do polinômio a ser utilizado.
int n_divisoes;

//Função que calcula o valor retornado pelo polinômio no ponto x.
double polinomio(double x){
    double valor_retornado = 0;
    for(int i=0 ; i<grau_da_funcao+1 ; i++){
        valor_retornado += vetor_coeficientes[i]*(pow(x,i));
    }
    return valor_retornado;
}

/*Função a ser executada por cada thread.*/
void *funcaoThreads(void* arg) {
   long int identificador = (long int) arg; //Identificador local da thread.
   double delta_x = (fim_intervalo-comeco_intervalo)/n_divisoes; 
   
   double *soma_local; //Variável local a ser retornada pela thread.
   soma_local = (double*) malloc(sizeof(double));
   if(soma_local==NULL) {
      printf("ERRO--alocacao de memoria dentro das threads\n");
      exit(1);
   }
   
   //Divisão igual de tarefas para cada thread baseada no seu identificador.
   for(double x = comeco_intervalo + delta_x*identificador; x<fim_intervalo ; x+=n_threads*delta_x){
       *soma_local += (double) polinomio(x)*delta_x;
   }
   
   pthread_exit((void*) soma_local); 
}

//Thread principal (main)
int main(int argc, char *argv[]) {
   double tempo_inicio, tempo_fim; //Variáveis para tomada de tempo e avaliação de desempenho.
   pthread_t *idThreads; //Identificadores das threads no computador.
   double *valorRetorno; //Valor de retorno das threads.
   double soma_global = 0; //Soma final que representa o resultado da integração.

   //Recebimento e validação dos parâmetros de entrada.
   if(argc < 5) {
       fprintf(stderr, "Digite: %s <inicio do intervalo> <fim do intervalo> <numero de divisoes do intervalo> <numero de threads>\n", argv[0]);
       return 1; 
   }
   
   //Atribuição das variáveis presentes em argv[].
   comeco_intervalo = atof(argv[1]);
   fim_intervalo = atof(argv[2]);
   n_divisoes = atoi(argv[3]);
   n_threads = atoi(argv[4]);

   //Entrada e verificação do grau do polinômio.
   printf("Digite o grau do polinomio:\n");
   scanf("%d", &grau_da_funcao);
   if(grau_da_funcao<0){
       printf("O grau da funcao nao pode ser negativo.\n");
       exit(1);
   }

   //Alocação de memória para o vetor de coeficientes.
   vetor_coeficientes = (double*)malloc((grau_da_funcao+1)*sizeof(double));
   if(vetor_coeficientes==NULL) {
      printf("ERRO--alocacao de memoria vetor de coeficientes\n");
      exit(2);
   }

   //Entrada dos coeficientes do polinômio.
   printf("Digite os coeficientes do polinomio:\n");
   for(int i = 0 ; i<grau_da_funcao+1 ; i++){
       scanf(" %lf", vetor_coeficientes+i); 
   }
   
   //Cálculo da integral do polinômio no intervalo dado feito de forma concorrente.
   GET_TIME(tempo_inicio);
   
   //Alocação de memória para vetor de threads.
   idThreads = (pthread_t*) malloc(sizeof(pthread_t)*n_threads);
   if(idThreads==NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      return 3;
   }
   
   //Criação das threads.
   for(long int i=0; i<n_threads; i++) {
      if(pthread_create(idThreads+i, NULL, funcaoThreads, (void*) i)){
         fprintf(stderr, "ERRO--pthread_create\n");
         return 4;
      }
   }
   
   //Sincronização e finalização das threads.
   for(long int i=0; i<n_threads; i++) {
      if(pthread_join(*(idThreads+i), (void**) &valorRetorno)){
         fprintf(stderr, "ERRO--pthread_join\n");
         return 5;
      }
      //O valor retornado pelas threads é somado ao valor global.
      soma_global += *valorRetorno;
   }
   GET_TIME(tempo_fim);
   
   printf("Resultado obtido foi de %f com tempo de execucao de %lf segundos.\n", soma_global, tempo_fim-tempo_inicio);
   
   //Liberação de memória alocada.
   free(idThreads);
   return 0;
}