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
int forks[5];

/*Espera ate sendGo ser chamado*/
void waitForGo() {
	pthread_mutex_lock(&lock);
	while (cango == 0)
		pthread_cond_wait(&condgo,&lock);
	pthread_mutex_unlock(&lock);
}

/*Manda thread logica prosseguir pois conseguiu o recurso*/
void sendGo() {
	pthread_mutex_lock(&lock);
	cango = 1;
	pthread_cond_signal(&condgo);
	pthread_mutex_unlock(&lock);
}

/*Retorna valor do clock*/
int getClock() {
	int ret;
	pthread_mutex_lock(&lock);
	ret = lclock;
	pthread_mutex_unlock(&lock);
	return ret;
}

/*Incrementa e retorna valor anterior do clock atomicamente*/
int incClock() {
	pthread_mutex_lock(&lock);
	int ret = lclock++;
	pthread_mutex_unlock(&lock);
	return ret;
}

/*Atomicamente faz a operaçao clock = max(clock,fromclock)+1*/
void setMaxAndIncClock(int fromclock) {
	pthread_mutex_lock(&lock);
	if (fromclock > lclock)
		lclock = fromclock;
	lclock++;
	pthread_mutex_unlock(&lock);
}

/*Cria socket para receives*/
int createSocketListen(int numeroPorta) {
	int sock;
	struct sockaddr_in echoServAddr;

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


/*Cria socket para envios*/
int createSocketSend() {
	int sock;
	
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		perror ("socket() falhou");
	}
	return sock;

}

/*Manda uma mensagem para o socket do helper proprio*/
void sendSelf(int sock,char *msg, int tam) {
	struct sockaddr_in echoServAddr;
	int toid = id;
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	echoServAddr.sin_port   = htons(PORT+toid);
	sendto(sock, msg, tam, 0,
		(struct sockaddr *) &echoServAddr,
		sizeof(echoServAddr));
}

/*Pede para helper fazer um POP em fork, bloqueia até helper executar sendGo*/
void reqp(int sendsock,int fork) {
	char buf[ECHOMAX];
	sprintf(buf,"%d %d %c %d",id,incClock(),RPOP,fork);
	cango = 0;
	sendSelf(sendsock,buf, strlen(buf)+1);
	waitForGo();
}

/*Pede para helper fazer um VOP em fork*/
void reqv(int sendsock,int fork) {
	char buf[ECHOMAX];
	sprintf(buf,"%d %d %c %d",id,incClock(),RVOP,fork);
	sendSelf(sendsock,buf, strlen(buf)+1);
}

/*Estado do filosofo pensando*/
void thinking() {
	printf("(%d,%d) : THINKING\n",id,getClock());
	sleep(rand()%5+1);
}

/*Estado do filosofo comendo*/
void eating() {
	printf("(%d,%d) : EATING\n",id,getClock());
	sleep(rand()%5+1);
}

/*Estado do filosofo com fome, tenta obter os garfos, comer, e liberar os garfos*/
void hungry(int sendsock) {
	printf("(%d,%d) : HUNGRY\n",id,getClock());
	int fork1 = id-1;
	int fork2 = fork1+1;
	if (fork2 == 5) {
		fork1 = 0;
		fork2 = 4;
	}
	reqp(sendsock,fork1);
	reqp(sendsock,fork2);
	eating();
	reqv(sendsock,fork1);
	reqv(sendsock,fork2);
}


/*Envia uma mensagem para todos os outros processos, atravez de 5 sendto por uma socket udp, porta destino é PORT + id, ip destino é 127.0.0.1*/
void broadcast(int sock,char *msg, int tam) {
	struct sockaddr_in echoServAddr;
	int toid;
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
	int id,clock,param; /*id e relogio de lamport do processo que enviou a mensagem, parametro de qual semaforo esta sendo usado para VOPs e POPs*/
	char op; /*numero da operaçao sendo realizada*/
	int remove; /*estrutura interna a fila, usado para marcar mensagens*/
} message_t;


/*Define a ordem das mensagens na fila*/
int compareMessage(const void * pa, const void * pb) {
	message_t *a = (message_t*)pa;
	message_t *b = (message_t*)pb;
	if (a->clock == b->clock)
		return a->id - b->id;
	return a->clock - b->clock;
}

message_t queue[1000];
int queuesize = 0;

/*Adiciona a fila*/
void putInQueue(message_t msg) {
	msg.remove = 0;
	queue[queuesize++] = msg;
}


/*Ordena a fila de acordo com o tempo e id das mensagens*/
void sortQueue() {
	qsort(queue,queuesize,sizeof(message_t),compareMessage);
}


/*Extrai a estrutura mensagem de um array de chars*/
message_t readMessage(char* buffer) {
	int fromid, fromclock, param;
	char op;
	sscanf(buffer,"%d %d %c %d",&fromid,&fromclock,&op,&param);
	message_t ret;
	ret.id = fromid;
	ret.clock = fromclock;
	ret.op = op;
	ret.param = param;
	return ret;
}


/*Envia mensagem para todos os filosofos*/
void sendMessage(int sendsock,message_t msg) {
	char buf[ECHOMAX];
	sprintf(buf,"%d %d %c %d",msg.id,msg.clock,msg.op,msg.param);
	broadcast(sendsock,buf,strlen(buf)+1);
}

/*void printQueue(int clock) {
	if (id != 1 || queuesize == 0) return;
	printf("\n\n");
	int i;
	int printedclock = 0;
	for (i = 0; i < queuesize; i++) {
		if (!printedclock && clock <= queue[i].clock) {
			printf("<- %d\n",clock);
			printedclock = 1;
		}
		if (queue[i].op == POP)
			printf("POP ");
		else
			printf("VOP ");
		printf("%d %d\n",queue[i].id,queue[i].clock);
	}
	if (!printedclock)
		printf("<-\n");
}*/


/*Processa mensagens da fila com clock menor que clock*/
void processMessages(int clock) {
	sortQueue();
	//printQueue(clock);
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


/*Receive da socket da thread helper e processa ou enfila a mensagem*/
void hearing(int sock, int sendsock, int lastAck[]) {
	struct sockaddr_in echoClntAddr;
    unsigned int cliAddrLen;
    char echoBuffer[ECHOMAX];
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
			outmsg.clock = incClock();
			sendMessage(sendsock,outmsg);
			break;
		case POP:
		case VOP:
			putInQueue(msg);
			outmsg.id = id;
			outmsg.clock = incClock();
			outmsg.op = ACK;
			sendMessage(sendsock,outmsg);
			break;
		case ACK:
			lastAck[msg.id-1] = msg.clock;
			int oldestAck = msg.clock;
			int i;
			for (i = 0; i < 5; i++) {
				if (lastAck[i] < oldestAck)
					oldestAck = lastAck[i];
			}
			processMessages(oldestAck);
			break;
	}
}


/*Loop principal da thread helper*/
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

/*Cria os processos do filosofo, retorna a id do processo*/
int initializeProcess() {
	int id;
	for (id = 1; id < 5; id++){
		int cpid = fork();
		if (cpid != 0)
			break;
	}
	return id;
}


/*Inicializa Array dos Semaforos dos garfos */
void initializeForks() {
	int i;
	for (i = 0; i < 5; i++)
		forks[i] = 1;
}

int main(int argc, char *argv[]) {
	initializeForks();
	id = initializeProcess();
	srand(time(NULL)+id);
	pthread_t helperthread;
	pthread_mutex_init(&lock,NULL);
	pthread_create(&helperthread,NULL,helper,NULL);
	printf("Filosofo %d criado\n",id);
	int sendsock = createSocketSend();
	//broadcast(sendsock,"Hello!",strlen("Hello!")+1);
	while (1) {
		thinking();
		hungry(sendsock);
	}
	return 0;
}
