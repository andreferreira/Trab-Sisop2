import java.rmi.Naming; 
import java.rmi.RemoteException; 

 
public class NullClient { 
    public static void main(String[] args) throws Exception { 
        
        NullInterface c;

	/* encontrar o objeto remoto a partir do nome */
        c = (NullInterface)
             Naming.lookup
             ("rmi://127.0.0.1/NullService"); 

	/* chamada de metodos do objeto remoto */
        c.doNothing();
    } 
} 
