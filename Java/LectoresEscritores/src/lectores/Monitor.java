package lectores;

class Monitor {
    private int numLectores ;
    private int numEscritores ;

    public Monitor() {
        numLectores = 0;
        numEscritores= 0;
    }
    public synchronized void entrada_lectores() throws InterruptedException {
        while(numEscritores > 0) wait();
        numLectores++;
    }
    public synchronized void salida_lectores() {
        numLectores--;
        if(numEscritores>1) System.out.println(numEscritores);
        if(numLectores == 0) notifyAll();
    }
    public synchronized void entrada_escritores() throws InterruptedException {
        while(numLectores > 0 || numEscritores > 0) wait();
        numEscritores++;
    }
    public synchronized void salida_escritores() {
        numEscritores--;
        if(numLectores>1) System.out.println(numLectores);
        notifyAll();
    }
    public synchronized void quieroEscribir(int[] j) throws InterruptedException {
        entrada_escritores();
        j[0] = j[0] + 1;
        salida_escritores();
    }

}