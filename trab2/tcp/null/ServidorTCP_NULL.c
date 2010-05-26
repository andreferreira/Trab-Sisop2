#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>


#define MAXPENDING 5
#define RCVBUFSIZE 100

void DieWithError(char *errorMessage);
void HandleTCPClient(int clntSocket);

int main(int argc, char *argv[])
{
    int servSock;
    int clntSock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned int clntLen;

    int numeroPorta;
    char comando[RCVBUFSIZE];
    int tamanhoRecebido;
    
    /* checagem de parametros */
    if (argc != 2) {
        fprintf(stderr, "%s <porta>\n", argv[0]);
        exit(1);
    }
    numeroPorta = atoi (argv[1]);

    /* Criacao do socket TCP para receber conexoes */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
	    perror ("socket() falhou");
	    exit(1);
    }
    printf ("socket criado... %d\n", servSock);
      
    /* construcao do endereco de conexao */
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(numeroPorta);

    /* Bind ao endereco local */
    if (bind(servSock, (struct sockaddr *) &echoServAddr,
			    sizeof(echoServAddr)) < 0){
	    perror ("bind() falhou");
	    exit(1);
    }
    printf ("bind feito...\n");

    /* listen */
    if (listen(servSock, MAXPENDING) < 0){
	    perror ("listen() falhou");
	    exit(1);
    }
    printf ("listen feito...\n");

    while (1){
        /* esperando conexoes com accept */
        clntLen = sizeof(echoClntAddr);
        if ((clntSock = accept(servSock,
			(struct sockaddr *) &echoClntAddr, 
                               &clntLen)) < 0){
			perror ("accept() falhou");
			exit(1);
		}

        /* clntSock esta conectado a um cliente */
		if ((tamanhoRecebido = recv (clntSock, comando,
				RCVBUFSIZE, 0)) < 0){
			perror ("recv() falhou\n");
			exit(1);
		}
		int k = send (clntSock, "",1,0);
		close (clntSock);

    /* NOT REACHED */
	}
}
