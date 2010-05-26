#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 1050
#define ECHOMAX 255

int id;
int lclock = 0;
pthread_mutex_t lock;
pthread_cond_t condgo;
int cango;
int s;

void waitForGo() {
	pthread_mutex_lock(&lock);
	while (cango == 0)
		pthread_cond_wait(&condgo,&lock);
	pthread_mutex_unlock(&lock);
}

void sendGo() {
	pthread_mutex_lock(&lock);
	cango = 1;
	pthread_cond_signal(&condgo);
	pthread_mutex_unlock(&lock);
}

int getClock() {
	int ret;
	pthread_mutex_lock(&lock);
	ret = lclock;
	pthread_mutex_unlock(&lock);
	return ret;
}

void incClock() {
	pthread_mutex_lock(&lock);
	lclock++;
	pthread_mutex_unlock(&lock);
}

void setMaxAndIncClock(int fromclock) {
	pthread_mutex_lock(&lock);
	if (fromclock > lclock)
		lclock = fromclock;
	lclock++;
	pthread_mutex_unlock(&lock);
}

int createSocketListen(int numeroPorta) {
	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in echoClntAddr;

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
	return sock;
}

int createSocketSend() {
	int sock;
    struct sockaddr_in echoServAddr;
	
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		perror ("socket() falhou");
	}
	return sock;

}

void thinking() {
	incClock()
	sleep(rand()%5+1);
}

void reqp() {
	cango = 0;
	char msg[ECHOMAX];
	sprintf(msg,"%d;%d;p",id,getClock());
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	echoServAddr.sin_port   = htons(PORT+id);
	sendto(sock, msg, strlen(msg)+1, 0,
		(struct sockaddr *) &echoServAddr,
		sizeof(echoServAddr));
	waitForGo();
}

void reqv() {
	char msg[ECHOMAX];
	sprintf(msg,"%d;%d;v",id,getClock());
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	echoServAddr.sin_port   = htons(PORT+id);
	sendto(sock, msg, strlen(msg)+1, 0,
		(struct sockaddr *) &echoServAddr,
		sizeof(echoServAddr));	
}



void eating() {
	incClock()
	sleep(rand()%5+1);
	reqp();
}

void hungry() {
	
}

void broadcast(int sock,char *msg, int tam) {
	struct sockaddr_in echoServAddr;
	int toid;
	int j;
	//for (j=0,toid =(id+3)% 5+1;  j<2;  j++,toid=id%5+1) {
	for (toid = 1; toid <= 5; toid++) {
		if (id == toid) continue;
		printf("%d %d\n",id,toid);
		memset(&echoServAddr, 0, sizeof(echoServAddr));
		echoServAddr.sin_family = AF_INET;
		echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		echoServAddr.sin_port   = htons(PORT+toid);
		sendto(sock, msg, tam, 0,
			(struct sockaddr *) &echoServAddr,
			sizeof(echoServAddr));
	}
}

void hearing(int sock) {
	struct sockaddr_in echoClntAddr;
    unsigned int cliAddrLen;
    char echoBuffer[ECHOMAX];
    unsigned short echoServPort;
    int recvMsgSize;
    
    cliAddrLen = sizeof(echoClntAddr);	
    if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
    	(struct sockaddr *) &echoClntAddr, &cliAddrLen)) <0){
			perror ("recvfrom () falhou");
	}
	int fromid,fromclock;
	char op;
	sscanf(echoBuffer,"%d;%d;%c",&fromid,&fromclock,&op);
	setMaxAndIncClock(fromclock);
}

void* helper(void* _) {
	int listensock = createSocketListen(PORT+id);
	int sendsock = createSocketSend();
	while (1) {
		hearing(listensock);
	}
}

int main(int argc, char *argv[]) {
	int left = 1;
	int right = 1;
	for (id = 1; id < 5; id++){
		int cpid = fork();
		if (cpid != 0)
			break;
	}
	pthread_t helperthread;
	pthread_mutex_init(&lock,NULL);
	pthread_create(&helperthread,NULL,helper,NULL);
	printf("Filosofo %d criado\n",id);
	int sendsock = createSocketSend();
	//broadcast(sendsock,"Hello!",strlen("Hello!")+1);
	while (1) {
		thinking();
		hungry();
	}
	return 0;
}
