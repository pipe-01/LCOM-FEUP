#include "i8042.h"
#include <lcom/timer.h>
int hook_id_timer = 0;
int counter = 0;

int(timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id_timer) != OK)
    return 1;

  return 0;
}

int(timer_subscribe_int)(uint8_t *bit_no) {
  hook_id_timer = IRQ0;

  if (sys_irqsetpolicy(IRQ0, IRQ_REENABLE, &hook_id_timer) != OK)
    return 1;

  return 0;
}

void(timer_int_handler)() {
  counter++;
}
