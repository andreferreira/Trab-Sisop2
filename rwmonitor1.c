#include <semaphore.h>
#include <stdlib.h>
#include "rwmonitor.h"

struct rwmonitor_t_struct {
	sem_t semr, semw;
	int nr;
};

void enter_reader_monitor(rwmonitor_t monitor) {
	struct rwmonitor_t_struct* m = (struct rwmonitor_t_struct*)monitor;
	sem_wait(&m->semr);
	m->nr++;
	if (m->nr == 1) //wait for writers to exit, block other writers from entering
		sem_wait(&m->semw);
	sem_post(&m->semr);
}

void exit_reader_monitor(rwmonitor_t monitor) {
	struct rwmonitor_t_struct* m = (struct rwmonitor_t_struct*)monitor;
	sem_wait(&m->semr);
	m->nr--;
	if (m->nr == 0) //no more readers, allow writers to enter their monitor
		sem_post(&m->semw);
	sem_post(&m->semr);
}

void enter_writer_monitor(rwmonitor_t monitor) {
	struct rwmonitor_t_struct* m = (struct rwmonitor_t_struct*)monitor;
	sem_wait(&m->semw);
}

void exit_writer_monitor(rwmonitor_t monitor) {
	struct rwmonitor_t_struct* m = (struct rwmonitor_t_struct*)monitor;
	sem_post(&m->semw);
}

void monitor_init(rwmonitor_t *monitor) {
	struct rwmonitor_t_struct *newmonitor = malloc(sizeof(struct rwmonitor_t_struct));
	sem_init(&newmonitor->semr,0,1);
	sem_init(&newmonitor->semw,0,1);
	newmonitor->nr = 0;
	void **temp = (void**)monitor;
	*temp = newmonitor;
}
