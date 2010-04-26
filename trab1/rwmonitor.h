typedef void* rwmonitor_t;

void enter_reader_monitor(rwmonitor_t monitor);
void exit_reader_monitor(rwmonitor_t monitor);
void enter_writer_monitor(rwmonitor_t monitor);
void exit_writer_monitor(rwmonitor_t monitor);
void monitor_init(rwmonitor_t *monitor);
