import java.rmi.*;

public class SortServer {
   public SortServer() throws Exception {    
     /* criacao do objeto SortImpl */
     SortInterface sort = new SortImpl();

     /* associacao desse objeto a um nome */
     java.rmi.Naming.rebind
     	("rmi://127.0.0.1/SortService",
     	sort);
   }

   public static void main(String args[]) throws Exception{
     new SortServer();
   }
}
