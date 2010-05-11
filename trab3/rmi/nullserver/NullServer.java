import java.rmi.*;

public class NullServer {
   public NullServer() throws Exception {    

     /* criacao do objeto NullImpl */
     NullInterface nil = new NullImpl();

     /* associacao desse objeto a um nome */
     java.rmi.Naming.rebind
     	("rmi://127.0.0.1/NullService",
     	nil);
   }

   public static void main(String args[]) throws Exception{
     new NullServer();
   }
}
