/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Leitores e escritores usando monitores em Java */
/* -------------------------------------------------------------------*/

import java.util.Random;

// Monitor que implementa a logica do padrao leitores/escritores
class LE {
    private int leit, escr, dado;  
  
  // Construtor
  LE() { 
     this.leit = 0; //leitores lendo (0 ou mais)
     this.escr = 0; //escritor escrevendo (0 ou 1)
     this.dado = 0; // variavel compartilhada
  } 
  
  // Entrada para leitores
  public synchronized void EntraLeitor (int id) {
    try { 
      while (this.escr > 0) {
         wait();  //bloqueia pela condicao logica da aplicacao 
      }
      this.leit++;  //registra que ha mais um leitor lendo
    } catch (InterruptedException e) { }
  }
  
  // Saida para leitores
  public synchronized void SaiLeitor (int id) {
     this.leit--; //registra que um leitor saiu
     if (this.leit == 0) 
           this.notify(); //libera escritor (caso exista escritor bloqueado)
  }
  
  // Entrada para escritores
  public synchronized void EntraEscritor (int id) {
    try { 
      while ((this.leit > 0) || (this.escr > 0)) {
         wait();  //bloqueia pela condicao logica da aplicacao 
      }
      this.escr++; //registra que ha um escritor escrevendo
    } catch (InterruptedException e) { }
  }
  
  // Saida para escritores
  public synchronized void SaiEscritor (int id) {
     this.escr--; //registra que o escritor saiu
     notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
  }

    public int getDado(){
	return this.dado;
    }

    public void setDado(int novo){
	this.dado = novo;
    }
}



//Aplicacao de exemplo--------------------------------------------------------
// Leitor
class Leitor extends Thread {
  int id; //identificador da thread
  int delay; //atraso bobo
  LE monitor;//objeto monitor para coordenar a lógica de execução das threads

  // Construtor
  Leitor (int id, LE m) {
    this.id = id;
    this.monitor = m;
  }

  // Método executado pela thread
  public void run () {
      this.monitor.EntraLeitor(this.id);
      int dado = this.monitor.getDado();
      if (dado % 2 == 0){ System.out.println("Par");}
      else {System.out.println("Ímpar");}
      this.monitor.SaiLeitor(this.id);
  }
}

//--------------------------------------------------------
// Escritor
class Escritor extends Thread {
  int id; //identificador da thread
  int delay; //atraso bobo...
  LE monitor; //objeto monitor para coordenar a lógica de execução das threads

  // Construtor
  Escritor (int id, LE m) {
    this.id = id;
    this.monitor = m;
  }

  // Método executado pela thread
    public void run () {
	this.monitor.EntraEscritor(this.id); 
	this.monitor.setDado(this.id);
	this.monitor.SaiEscritor(this.id); 
  }
}

class LeitorEscritor extends Thread {
    int id; //identificador da thread
    LE monitor; //objeto monitor para coordenar a lógica de execução das threads
    
    // Construtor
    LeitorEscritor (int id, LE m) {
	this.id = id;
	this.monitor = m;
    }

    public void run () {
	try {
	    Random rand = new Random();
	    this.monitor.EntraLeitor(this.id); 
	    int dado = this.monitor.getDado();
	    System.out.println("[THREAD " + this.id + "] Li o valor: " + dado);
	    this.monitor.SaiLeitor(this.id);
	    int delay = rand.nextInt((int) 3.15 * 1000000); 
	    System.out.println("[THREAD " + this.id + "] Soninho... vou dormir. Ate daqui a " + delay / 86400 + " dias");
	    sleep( ((long) delay) * 1000); // dorme um tempo aleatório de segundos
	    // de 0 segundos até 1 ano
	    // processamento bobo emoji malucao emoji malucao
	    this.monitor.EntraEscritor(this.id);
	    this.monitor.setDado(this.id + 1);
	    this.monitor.SaiEscritor(this.id); 
	} catch (InterruptedException e) {}
  }

}


//--------------------------------------------------------
// Classe principal
class Lab8 {
    static final int L = 4;
    static final int E = 3;
    static final int LE = 12;

    public static void main (String[] args) {
	int i;
	LE monitor = new LE();            // Monitor (objeto compartilhado entre leitores e escritores)
	Leitor[] l = new Leitor[L];       // Threads leitores
	Escritor[] e = new Escritor[E];   // Threads escritores
	LeitorEscritor[] le = new LeitorEscritor[LE];
    
	for (i=0; i<L; i++) {
	    l[i] = new Leitor(i+1, monitor);
	    l[i].start(); 
	}
	for (i=0; i<E; i++) {
	    e[i] = new Escritor(i+1, monitor);
	    e[i].start(); 
	}
	for (i=0; i<LE; i++) {
	    le[i] = new LeitorEscritor(i+1, monitor);
	    le[i].start(); 
	}
    }
}
