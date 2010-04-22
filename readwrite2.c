#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int nr = 0;
int value = 5;

void enterReaderMonitor() {

}

void exitReaderMonitor() {

}

void enterWriterMonitor() {

}

void exitWriterMonitor() {

}

void* reader(void* arg) {
	int threadnumber = *(int*)arg;
	free(arg);
	while (1) {
		enterReaderMonitor();
		printf("Reader:%d Value:%d\n",threadnumber,value);
		exitReaderMonitor();
		sleep(rand()%4);
	}
}

void* writer(void* arg) {
	int threadnumber = *(int*)arg;
	free(arg);
	while (1) {
		enterWriterMonitor();
		value = rand()%100;
		printf("Writer:%d New value:%d\n",threadnumber,value);
		exitWriterMonitor();
		sleep(rand()%4);	
	}
}

int main(int argc, char *argv[]) {
	int nreaders, nwriters, i;
	pthread_t *readers, *writers;
	
	if (argc != 3) {
		printf("Usage:%s <number of readers> <number of writers>\n",argv[0]);
		return 1;
	}
	nreaders = atoi(argv[1]);
	nwriters = atoi(argv[2]);
	
	
	readers = malloc(nreaders*sizeof(pthread_t));
	writers = malloc(nwriters*sizeof(pthread_t));
	for (i = 0; i < nreaders; i++) {
		int *threadnumber = malloc(sizeof(int));
		*threadnumber = i;
		pthread_create(&readers[i],NULL,reader,threadnumber);
	}
	for (i = 0; i < nwriters; i++) {
		int *threadnumber = malloc(sizeof(int));
		*threadnumber = i;
		pthread_create(&writers[i],NULL,writer,threadnumber);
	}
	for (i = 0; i < nreaders; i++)
		pthread_join(readers[i], NULL);
	for (i = 0; i < nwriters; i++)
		pthread_join(writers[i], NULL);
	return 0;
}
