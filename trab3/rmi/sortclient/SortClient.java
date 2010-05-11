import java.rmi.Naming; 
import java.rmi.RemoteException; 

 
public class SortClient { 
    public static void main(String[] args) throws Exception { 
        
        SortInterface c;

	/* encontrar o objeto remoto a partir do nome */
        c = (SortInterface)
             Naming.lookup
             ("rmi://127.0.0.1/SortService");
        long start = System.nanoTime();
        for (int n = 0; n < 5000; n++) { 
			int[] vector = new int[250];
			for (int i = 0; i < 250; i++) {
				vector[i] = i;
			}
		/* chamada de metodos do objeto remoto */
		
			vector = c.sort(vector);
		}
		long end = System.nanoTime();
		System.out.printf("%f segundos",(end - start) * 0.000000001);
    } 
} 
