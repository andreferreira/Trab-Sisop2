#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define ECHOMAX 255
#define ITERMAX 5000

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in fromAddr;
    unsigned int fromSize;
    char echoBuffer[ECHOMAX+1];
    int respStringLen;

    char nomeServidor[100];
    int numeroPorta;
    char mensagem[100];
    int tamanhoMensagem;

    if (argc != 3)
    {
        fprintf(stderr,"Usage: %s <IP Servidor> <Porta>\n", argv[0]);
        exit(1);
    }

    strcpy (nomeServidor, argv[1]);
    numeroPorta = atoi (argv[2]);
    strcpy (mensagem, "");
    tamanhoMensagem = strlen (mensagem);
    clock_t start, end;

	int n;
	start = clock();
	for(n=0; n<ITERMAX; n++){

		/* criar um socket UDP */
		if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
			perror ("socket() falhou");
		}

		/* construir a estrutura de endereco do servidor*/
		memset(&echoServAddr, 0, sizeof(echoServAddr));
		echoServAddr.sin_family = AF_INET;
		echoServAddr.sin_addr.s_addr = inet_addr(nomeServidor);
		echoServAddr.sin_port   = htons(numeroPorta);

		/* enviar o comando para o servidor */
		sendto(sock, mensagem, tamanhoMensagem+1, 0,
			(struct sockaddr *) &echoServAddr,
			sizeof(echoServAddr));
			
		/* receber uma resposta */
		fromSize = sizeof(fromAddr);
		respStringLen = recvfrom(sock, echoBuffer, 1, 0, 
			 (struct sockaddr *) &fromAddr, &fromSize);

		if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
		{
			fprintf(stderr,"Erro: recebemos um pacote de uma fonte desconhecida\n");
			exit(1);
		}
		close(sock);
	  }
      end = clock();
    
    
   /* Print out the difference */
  printf ( "The interval was: %f seconds\n",
    (double)( end - start ) / (double)CLOCKS_PER_SEC);
    
    exit(0);
}
