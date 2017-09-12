package filosofosmonitor;

import java.util.Random;

class SalaComida {
    final static private int PENSANDO = 0;
    final static private int HAMBRIENTO = 1;
    final static private int COMIENDO = 2;
    private int[] estado = { PENSANDO, PENSANDO, PENSANDO, PENSANDO, PENSANDO };

    private static int[] veces_comidas = new int[FilosofosMonitor.NUM_FILOSOFOS];
    private static int[] t_comiendo = new int[FilosofosMonitor.NUM_FILOSOFOS];
    private static int[] contador_filosofos = new int[FilosofosMonitor.NUM_FILOSOFOS];

    private int f_comiendo;
    private Random gen = new Random();

    private void comiendo(final int p) {
        int tiempo_dormir =  gen.nextInt(1501-500) + 500;
        new java.util.Timer().schedule(
        new java.util.TimerTask() {
            @Override
            public void run() {
                while(true)
                    contador_filosofos[p]++;
            }
        },
        (tiempo_dormir)
        );
        t_comiendo[p] += tiempo_dormir;
    }
    public void muestraResultado() {
        int j = 0;
        for (int i : t_comiendo) {
            System.out.println("Filosofo " + j + ":"
                               + "\tTiempo comiendo: " + i
                               + "\tVeces comidas: " + veces_comidas[j]
                               + "\tValor contador: " + contador_filosofos[j]);
            j++;
        }
    }
    public void comer(int p) {  // Note: public but not synchronized
        cogerPalillos(p);
        comiendo(p);
        soltarPalillos(p);
    }

    private synchronized void cogerPalillos(int p) {
        estado[p] = HAMBRIENTO;
        test(p);
        while (estado[p] != COMIENDO)
            try {
                wait();
            } catch (InterruptedException e) {  }

        f_comiendo++;
        System.out.println("El filosofo " + p + " está comiendo");
        if(f_comiendo>1)
            System.out.println("¡¡ESTÁN COMIENDO " + f_comiendo + " FILÓSOFOS!!" );
    }

    private synchronized void soltarPalillos(int p) {
        estado[p] = PENSANDO;
        veces_comidas[p]++;
        f_comiendo--;
        System.out.println("El filósofo " + p + " ha dejado de comer");
        test((p+4)%5);
        test((p+1)%5);
    }

    private void test(int p) {
        if (estado[p] == HAMBRIENTO
                && estado[(p+1)%5] != COMIENDO
                && estado[(p+4)%5] != COMIENDO
           ) {
            estado[p] = COMIENDO;
            notifyAll();
        }
    }

    private void pensando() {
        try {
            int piensa = FilosofosMonitor.generador.nextInt(1501-500) + 500;
            Thread.sleep(piensa);
        } catch(InterruptedException e) {
            /*System.out.println("error en pensando");*/
        }
    }
}