import java.rmi.*;

public class SortServer {
   public SortServer(String ip) throws Exception {    
     /* criacao do objeto SortImpl */
     SortInterface sort = new SortImpl();

     /* associacao desse objeto a um nome */
     java.rmi.Naming.rebind
     	("rmi://"+ip+"/SortService",
     	sort);
   }

   public static void main(String args[]) throws Exception{
     new SortServer(args[0]);
   }
}
