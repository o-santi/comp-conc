/* Aluno: Leonardo Ribeiro Santiago
   DRE: 120036072
*/

import java.util.Arrays;

//classe que estende Thread e implementa a tarefa de cada thread do programa 
class T implements Runnable{
    //identificador da thread
    int id;
    //objeto compartilhado com outras threads
    int[] vec;
    int soma;
    int nthreads;
    int size;
    //construtor
    public T(int tid, int[] vec, int nthreads, int size) { 
	this.id = tid;
	this.vec = vec;
	this.soma = 0;
	this.nthreads = nthreads;
	this.size = size;
    }

    //metodo main da thread
    @Override
    public void run() {
	for (int i=this.id; i<this.size; i += nthreads) {
	    this.soma += vec[i]; 
	}
    }

    public int getSoma() {
	return soma;
    }
}

//classe da aplicacao
class Lab7{
    static final int NTHREADS = 10;
    static final int ARRAY_SIZE = 1000;
    
    public static void main (String[] args) {
	//reserva espaço para um vetor de threads
	Thread[] threads = new Thread[NTHREADS];
	T[] objects = new T[NTHREADS];
	int vec_compartilhado[] = new int[ARRAY_SIZE];
	Arrays.setAll(vec_compartilhado, i -> i);

	//cria as threads da aplicacao
	for (int i=0; i<threads.length; i++) {
	    objects[i] = new T(i, vec_compartilhado, NTHREADS, ARRAY_SIZE);
	    threads[i] = new Thread(objects[i]);
	}

	//inicia as threads
	for (int i=0; i<threads.length; i++) {
	    threads[i].start();
	}

	//espera pelo termino de todas as threads
	int soma_total = 0;
	for (int i=0; i<threads.length; i++) {
	    try {
		threads[i].join();
		soma_total += objects[i].getSoma();
	    } catch (InterruptedException e) {
		System.out.println("Erro: Não conseguiu dar join na thread");
		return;
	    }
	}

	System.out.println("Soma do vetor = " + soma_total); 
    }
}
