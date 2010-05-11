#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ECHOMAX 255

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned int cliAddrLen;
    char echoBuffer[ECHOMAX];
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
  
    while (1){
        /* esperar mensagem de alguem */
        cliAddrLen = sizeof(echoClntAddr);
        if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
            (struct sockaddr *) &echoClntAddr, &cliAddrLen)) <0){
		perror ("recvfrom () falhou");
	}
	/*printf ("Recebeu cliente %s\n",
			inet_ntoa (echoClntAddr.sin_addr));*/
	int bytesEnviados = sendto(sock, "",
			1, 0, 
			(struct sockaddr *)&echoClntAddr,
			sizeof(echoClntAddr));
    }
}
