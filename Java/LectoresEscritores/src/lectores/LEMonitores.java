package lectores;

import java.util.Random;
public class LEMonitores extends Thread {

    private static final int NUM_THREADS = 2;
    private static final double MAX_ITERACIONES = 100000000;
    private static double max = MAX_ITERACIONES/NUM_THREADS;
    private static int count = 0;

    private static Monitor monitor = new Monitor();

    static long[] registro_lectura = new long[NUM_THREADS];
    static long[] registro_escritura = new long[NUM_THREADS];
    static long escrituras_totales = 0;

    Random n = new Random();
    private int tid;
    public LEMonitores(int id) {
        this.tid = id;
    }
    @Override
    public void run() {
        int nrand, op_lectura;
        double veces_escritas = 0;
        double veces_leidas = 0;
        double max_escrituras =  max * 0.1;
        double max_lecturas = max - max_escrituras;
        long start = 0, end = 0;
        try {
            for (int i=0; i < max; i++) {
                nrand = n.nextInt() % 2;
                if((nrand == 1 && veces_leidas < max_lecturas)) { //leer
                    start = System.nanoTime();
                    monitor.entrada_lectores();
                    end = System.nanoTime() - start;
                    registro_lectura[tid] += end;

                    op_lectura = count;
                    veces_leidas++;
                    monitor.salida_lectores();

                } else if(nrand == 0  && veces_escritas < max_escrituras) { //escribir
                    start = System.nanoTime();
                    monitor.entrada_escritores();
                    end = System.nanoTime() - start;
                    registro_escritura[tid] += end;

                    count++;
                    escrituras_totales++;
                    monitor.salida_escritores();
                    veces_escritas++;
                }
            }
        } catch(Exception e) {}
    }
    public static void main(String args[]) {
        Thread[] threads = new Thread[NUM_THREADS];
        long tespera_esc = 0, tespera_lec = 0;

        for (int i = 0; i < threads.length; i++) {
            threads[i] = new lectores.LEMonitores(i);
            threads[i].start();
        }
        for (int i = 0; i < threads.length; i++) {
            try {
                threads[i].join();
            } catch(Exception e) {}
        }
        System.out.println("Resultados tras "+ MAX_ITERACIONES + " iteraciones y " + NUM_THREADS + " hilos");

        for (int i=0; i<NUM_THREADS; i++) {
            tespera_esc += registro_escritura[i];
            tespera_lec += registro_lectura[i];
        }
        tespera_esc /= escrituras_totales;
        tespera_lec /= (MAX_ITERACIONES-escrituras_totales);

        System.out.println("Tiempo espera escritura: " + tespera_esc);
        System.out.println("Tiempo espera lectura:   " + tespera_lec);

    }
}
