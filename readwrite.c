#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "rwmonitor.h"

rwmonitor_t rwmonitor;
int value = 5;


void* reader(void* arg) {
	int threadnumber = *(int*)arg;
	free(arg);
	while (1) {
		enter_reader_monitor(rwmonitor);
		printf("Reader:%d Value:%d\n",threadnumber,value);
		exit_reader_monitor(rwmonitor);
		sleep(1);
	}
}

void* writer(void* arg) {
	int threadnumber = *(int*)arg;
	free(arg);
	while (1) {
		enter_writer_monitor(rwmonitor);
		int newvalue = rand()%100;
		printf("Writer:%d Old value:%d New value:%d\n",threadnumber,value,newvalue);
		value = newvalue;
		exit_writer_monitor(rwmonitor);
		sleep(1);	
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
	
	monitor_init(&rwmonitor);
	
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
