#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 1050
#define ECHOMAX 255

#define ACK 0
#define POP 1
#define VOP 2
#define RPOP 3
#define RVOP 4

int id;
int lclock = 0;
pthread_mutex_t lock;
pthread_cond_t condgo;
int cango;
int s;
int forks[10];

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

int incClock() {
	pthread_mutex_lock(&lock);
	int ret = lclock++;
	pthread_mutex_unlock(&lock);
	return ret;
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
	sleep(rand()%5+1);
}

void reqp() {

}

void reqv() {

}

void eating() {
	sleep(rand()%5+1);
}

void hungry() {
	
}

void broadcast(int sock,char *msg, int tam) {
	struct sockaddr_in echoServAddr;
	int toid;
	int j;
	//for (j=0,toid =(id+3)% 5+1;  j<2;  j++,toid=id%5+1) {
	for (toid = 1; toid <= 5; toid++) {
		memset(&echoServAddr, 0, sizeof(echoServAddr));
		echoServAddr.sin_family = AF_INET;
		echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		echoServAddr.sin_port   = htons(PORT+toid);
		sendto(sock, msg, tam, 0,
			(struct sockaddr *) &echoServAddr,
			sizeof(echoServAddr));
	}
}

typedef struct {
	int id,clock,param;
	char op;
	int remove;
} message_t;

int compareMessage(const void * pa, const void * pb) {
	message_t a = *(message_t*)pa;
	message_t b = *(message_t*)pb;
	if (a.clock == b.clock)
		return a.id - b.id;
	return a.clock - b.clock;
}

message_t queue[1000];
int queuesize = 0;

void putInQueue(message_t msg) {
	msg.remove = 0;
	queue[queuesize++] = msg;
}

void sortQueue() {
	qsort(queue,queuesize,sizeof(message_t),compareMessage);
}

message_t readMessage(char* buffer) {
	int fromid, fromclock, param;
	char op;
	sscanf(buffer,"%d %d %c %d",&fromid,&fromclock,&op,&param);
	message_t ret;
	ret.id = fromid;
	ret.clock = fromclock;
	ret.op = op;
	ret.param = param;
}

void sendMessage(int sendsock,message_t msg) {
	char buf[ECHOMAX];
	sprintf(buf,"%d %d %c %d",msg.id,msg.clock,msg.op,msg.param);
	broadcast(sendsock,buf,strlen(buf)+1);
}

void processMessages(int clock) {
	sortQueue();
	int i;
	for (i = 0; i < queuesize && queue[i].clock < clock; i++) {
		if (queue[i].op == VOP) {
			queue[i].remove = 1;
			forks[queue[i].param]++;
		}
	}
	for (i = 0; i < queuesize && queue[i].clock < clock; i++) {
		if (queue[i].op == POP && forks[queue[i].param] > 0) {
			queue[i].remove = 1;
			forks[queue[i].param]--;
			if (queue[i].id == id)
				sendGo();
		}
	}
	int j = 0;
	for (i = 0; i < queuesize; i++) {
		if (!queue[i].remove)
			queue[j++] = queue[i];
	}
	queuesize = j;
}

void hearing(int sock, int sendsock, int lastAck[]) {
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
	message_t msg = readMessage(echoBuffer);
	message_t outmsg;
	setMaxAndIncClock(msg.clock);
	switch (msg.op) {
		case RPOP:
		case RVOP:
			outmsg = msg;
			if (msg.op == RPOP) outmsg.op = POP;
			else outmsg.op = VOP;
			outmsg.clock = incClock()-1;
			sendMessage(sendsock,outmsg);
			break;
		case POP:
		case VOP:
			putInQueue(msg);
			outmsg.id = id;
			outmsg.clock = incClock()-1;
			outmsg.op = ACK;
			sendMessage(sendsock,outmsg);
			break;
		case ACK:
			if (msg.id == id) return;
			lastAck[msg.id-1] = msg.clock;
			int oldestAck = msg.clock;
			int i;
			for (i = 0; i < 5; i++) {
				if (i+1 == id) continue;
				if (lastAck[i] < oldestAck)
					oldestAck = lastAck[i];
			}
			processMessages(oldestAck);
			break;
	}
}

void* helper(void* _) {
	int listensock = createSocketListen(PORT+id);
	int sendsock = createSocketSend();
	int lastAck[5];
	int i;
	for (i = 0; i < 5; i++)
		lastAck[i] = 0;
	while (1) {
		hearing(listensock,sendsock,lastAck);
	}
}

int initializeProcess() {
	int id;
	for (id = 1; id < 5; id++){
		int cpid = fork();
		if (cpid != 0)
			break;
	}
	return id;
}

void initializeForks() {
	int i;
	for (i = 0; i < 10; i++)
		forks[i] = 1;
}

int main(int argc, char *argv[]) {
	initializeForks();
	id = initializeProcess();
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
