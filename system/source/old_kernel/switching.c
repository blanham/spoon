#include <process.h>
#include <threads.h>

struct process *g_switchingProcess = NULL;
struct thread  *g_switchingThread = NULL;

