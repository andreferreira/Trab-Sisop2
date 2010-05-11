import java.rmi.*;

public class NullServer {
   public NullServer(String ip) throws Exception {    

     /* criacao do objeto NullImpl */
     NullInterface nil = new NullImpl();

     /* associacao desse objeto a um nome */
     java.rmi.Naming.rebind
     	("rmi://"+ip+"/NullService",
     	nil);
   }

   public static void main(String args[]) throws Exception{
     new NullServer(args[0]);
   }
}
