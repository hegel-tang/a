#include <common.h>
extern void do_syscall(Context *c);
static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD:
    #ifdef CONFIG_STRACE
      Log("\nYield");
    #endif
    break;
    case EVENT_SYSCALL:
    #ifdef CONFIG_STRACE
      Log("\nSYSCALL");
    #endif
      do_syscall(c);break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
