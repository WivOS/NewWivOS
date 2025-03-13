#include <tasking/thread.h>

void arch_thread_init_regs(thread_t *thread, thread_parameter_types_t dataType, void *data);
void arch_thread_set_stack(thread_t *thread, void *stack);