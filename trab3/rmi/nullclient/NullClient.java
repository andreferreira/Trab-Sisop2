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
		for (int n = 0; n < 5000; n++) {
			c.doNothing(); 
		}
		long end = System.nanoTime();
		System.out.printf("%f segundos",(end - start) * 0.000000001);
	}
} 
