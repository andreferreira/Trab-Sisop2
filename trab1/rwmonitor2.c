#include <stdlib.h>
#include "rwmonitor.h"

struct rwmonitor_t_struct {
	pthread_cond_t condw, condr;
	pthread_mutex_t lock;
	int nr,nw;
};

void enter_reader_monitor(rwmonitor_t monitor) {
	struct rwmonitor_t_struct* m = (struct rwmonitor_t_struct*)monitor;
	pthread_mutex_lock(&m->lock);
	while (m->nw > 0) //if there is a writer writing, awaits
		pthread_cond_wait(&m->condr,&m->lock);
	m->nr++;
	pthread_mutex_unlock(&m->lock);
}

void exit_reader_monitor(rwmonitor_t monitor) {
	struct rwmonitor_t_struct* m = (struct rwmonitor_t_struct*)monitor;
	pthread_mutex_lock(&m->lock);
	m->nr--;
	if (m->nr == 0) //no more readers, awake a writer if there are any
		pthread_cond_signal(&m->condw);
	pthread_mutex_unlock(&m->lock);
}

void enter_writer_monitor(rwmonitor_t monitor) {
	struct rwmonitor_t_struct* m = (struct rwmonitor_t_struct*)monitor;
	pthread_mutex_lock(&m->lock);
	while (m->nw > 0 || m->nr > 0) //if there is a writer writing or a reader reading, awaits
		pthread_cond_wait(&m->condw,&m->lock);
	m->nw++;
	pthread_mutex_unlock(&m->lock);
}

void exit_writer_monitor(rwmonitor_t monitor) {
	struct rwmonitor_t_struct* m = (struct rwmonitor_t_struct*)monitor;
	pthread_mutex_lock(&m->lock);
	m->nw--;
	pthread_cond_signal(&m->condw);
	pthread_cond_broadcast(&m->condr);
	pthread_mutex_unlock(&m->lock);
}

void monitor_init(rwmonitor_t *monitor) {
	struct rwmonitor_t_struct *newmonitor = malloc(sizeof(struct rwmonitor_t_struct));
	pthread_mutex_init(&newmonitor->lock,NULL);
	pthread_cond_init(&newmonitor->condw,NULL);
	pthread_cond_init(&newmonitor->condr,NULL);
	newmonitor->nr = 0;
	newmonitor->nw = 0;
	void **temp = (void**)monitor;
	*temp = newmonitor;
}
