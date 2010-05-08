public class SortImpl
	extends java.rmi.server.UnicastRemoteObject 
	implements SortInterface { 

	public SortImpl() 
		throws java.rmi.RemoteException { 
		super(); 
	} 

	public int[] sort(int[] vector) {
		for (int n = 0; n < vector.length; n++) {
			for (int i = 0; i < vector.length - n - 1; i++) {
				if (vector[i] < vector[i+1]) {
					int temp = vector[i];
					vector[i] = vector[i+1];
					vector[i+1] = temp;
				}
			}
		}
		return vector;
	}
} 

