public class NullImpl
	extends java.rmi.server.UnicastRemoteObject 
	implements NullInterface { 

	public NullImpl() 
		throws java.rmi.RemoteException { 
		super(); 
	} 

	public void doNothing() {
	}
} 

