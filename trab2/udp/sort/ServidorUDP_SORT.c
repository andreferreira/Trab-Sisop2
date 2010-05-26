#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ECHOMAX 255
typedef int bool;
#define false 0;
#define true 1;

inline void troca(int* a, int* b)
{
  int aux = *a;
  *a = *b;
  *b = aux;
}

void bubbleSortInverso (int *primeiro, int *ultimo){
  bool naoTrocou;
  int *posAtual;
  for (; ultimo > primeiro; --ultimo)
  {
    naoTrocou = true;
    for (posAtual = primeiro; posAtual < ultimo; ++posAtual)
    {
      if ( *posAtual < *(posAtual+1))
      {
        troca (posAtual, posAtual+1);
        naoTrocou = false;
      }
    }
    if (naoTrocou)
		return;
  }
}


int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned int cliAddrLen;
    int echoBuffer[ECHOMAX];
    unsigned short echoServPort;
    int recvMsgSize;

    int numeroPorta;

    if (argc != 2){
        fprintf(stderr,"Usage:  %s <porta>\n", argv[0]);
        exit(1);
    }

    numeroPorta = atoi(argv[1]);

    /* criar socket UDP para receber ou enviar datagramas */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
	    perror ("sock() falhou");
    }

    /* construir a estrutura de endereco local */
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(numeroPorta);

    /* Bind ao endereco local */
    if (bind(sock, (struct sockaddr *) &echoServAddr,
			    sizeof(echoServAddr)) <0){
	    perror ("bind() falhou");
    }
  
	int dataARRAYint[250];
    while (1){
		/* esperar mensagem de alguem */
		cliAddrLen = sizeof(echoClntAddr);
		if ((recvMsgSize = recvfrom(sock, echoBuffer, 250*sizeof(int), 0,
			(struct sockaddr *) &echoClntAddr, &cliAddrLen)) <0){
			perror ("recvfrom () falhou");
		}
		int i;
		for(i = 0; i < 250; i++){
			echoBuffer[i] = ntohl(echoBuffer[i]);
		}
		/*printf ("Recebeu cliente %s\n",
				inet_ntoa (echoClntAddr.sin_addr));*/

		bubbleSortInverso(&echoBuffer[0],&echoBuffer[249]);
		
		/*for(i = 0; i < 250; i++){
			printf("%d ",echoBuffer[i]);
		}	
		printf("\n");*/

		/* enviar a array decrescente para o cliente */
		int tamanhoMensagem;
		tamanhoMensagem = 250 * sizeof(int);
		
		for(i = 0; i < 250; i++){
			echoBuffer[i] = htonl(echoBuffer[i]);
		}

		int bytesEnviados = sendto(sock, echoBuffer,
			tamanhoMensagem, 0, 
			(struct sockaddr *)&echoClntAddr,
			sizeof(echoClntAddr));

		/*printf ("enviando %d\n", bytesEnviados);*/
    }
}








