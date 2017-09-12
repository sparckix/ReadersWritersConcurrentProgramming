package filosofosmonitor;

import java.util.Random;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class FilosofosMonitor extends Thread {
    static final int NUM_FILOSOFOS = 5;
    private static final ScheduledExecutorService worker =
        Executors.newSingleThreadScheduledExecutor();
    static Random generador = new Random();

    private static volatile boolean continuar = true;
    private int tid;
    private static SalaComida comedor = new SalaComida();

    public FilosofosMonitor(int id) {
        this.tid = id;
    }
    public void pensando() {
        try {
            int piensa = generador.nextInt(1501-500) + 500;
            Thread.sleep(piensa);
        } catch(InterruptedException e) { }
    }
    @Override
    public void run() {
        while (continuar) {
            pensando();
            comedor.comer(tid);
        }
    }

    public static void main(String[] args) {

        Thread[] threads = new Thread[NUM_FILOSOFOS];
        int i;
        for(i = 0; i < threads.length; i++) {
            threads[i] = new filosofosmonitor.FilosofosMonitor(i);
            threads[i].start();
        }

        Runnable task = new Runnable() {
            @Override
            public void run() {
                continuar = false;
            }
        };
        worker.schedule(task, 30, TimeUnit.SECONDS);
        for(i = 0; i < threads.length; i++) {
            try {
                threads[i].join();
            } catch(Exception e) {}
        }
        comedor.muestraResultado();
        System.exit(1);
    }
}
