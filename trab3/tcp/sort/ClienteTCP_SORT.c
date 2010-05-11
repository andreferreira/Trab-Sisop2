#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define ITERMAX 5000
#define ECHOMAX 255

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in echoServAddr;
    int echoBuffer[ECHOMAX];
    unsigned int echoStringLen;
    int bytesRcvd, totalBytesRcvd;

    char enderecoIPServidor[100];
    int numeroPorta;
    int tamanhoRecebido;
    int dataARRAY[250];
    int tamanhoMensagem;
    


    /* checagem de parametros */
    if (argc != 3){
	printf ("%s <endereco ip> <porta>\n", argv[0]);
	exit(1);
    }
    strcpy (enderecoIPServidor, argv[1]);
    numeroPorta = atoi(argv[2]);
    
    tamanhoMensagem = 250 * sizeof(int);

	int i;

	/* construcao do endereco de conexao */
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = inet_addr(enderecoIPServidor);
	echoServAddr.sin_port = htons(numeroPorta);

	clock_t start, end;

	int n;
	start = clock();
	for(n=0; n<ITERMAX; n++){

		/* Criacao do Socket TCP */		
		sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		/* Conectando ao servidor */
		if (connect(sock, (struct sockaddr *) &echoServAddr,
					sizeof(echoServAddr)) < 0){
		perror ("connect() falhou:");
		exit(1);
		}
		
		for(i = 0; i < 250; i++){
			dataARRAY[i]=htonl(i);
		}
	    
	    
		/* Envia o comando ao servidor */		
		if (send(sock, dataARRAY, 250*sizeof(int), 0) != tamanhoMensagem){
			perror ("send () falhou");
			exit(1);
		}

		/* Aguarda resposta do servidor */
		tamanhoRecebido = recv(sock, echoBuffer, 250*sizeof(int), 0);
		/*if (tamanhoRecebido == 0){
			close(sock);
		}*/
		
		for(i = 0; i < 250; i++){
			echoBuffer[i] = ntohl(echoBuffer[i]);
		}	
		close(sock);
	}
	end = clock();
    
    
   /* Print out the difference */
	printf ( "The interval was: %f seconds\n",
		(double)( end - start ) / (double)CLOCKS_PER_SEC);
}
