import java.rmi.Naming; 
import java.rmi.RemoteException; 

 
public class NullClient { 
    public static void main(String[] args) throws Exception { 
    
		NullInterface c;

	/* encontrar o objeto remoto a partir do nome */
        c = (NullInterface)
             Naming.lookup
             ("rmi://127.0.0.1/NullService"); 

	long start = System.nanoTime();
	/* chamada de metodos do objeto remoto */
	for (int i = 0; i < 5000; i++) {
        c.doNothing(); 
	}
    long end = System.nanoTime();
    long timedone = end - start;
    System.out.printf("%d nanosegundos",timedone);
	}
} 
