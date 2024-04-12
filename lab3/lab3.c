#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "keyboard.h"

extern int counter;
extern int scancode;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("./trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("./output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  int ipc_status;
  message msg;
  double r = 0;
  uint8_t hookid = 0;
  uint8_t irq_set = BIT(hookid);

  if (kbc_subscribe(&hookid) != 0){
    printf("Error subscribing keyboard interrupts\n");
    return 1;
  }

  while (scancode != 0x81 ) { /* You may want to use a different condition */
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d\n", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification *11: */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) {/* subscri14: ...  process it */
          // tck_counter++;
          kbc_ih();
          printSC();
          
          //kbd_print_scancode(true, 1, &scancode);
          //process_scancode()
          }
          break;

        default:
          break; /* no other notifications expected: do nothi19: */
           
      }
    }
    else { /* received a standard message, not a notification21: */
          /* no standard messages expected: do nothing */
           }
  }
  kbd_print_no_sysinb(count);
  

  if(kbc_unsubscribe() != 0)
    return 1;

  return 0;
}

int(kbd_test_poll)() {
  while(scancode != ESCAPE){
    kbc_read();
    kbc_print_scancode();
  }

  if(sys_outb(INPUT_BUFFER_COMMANDS, READ_COMMAND) != 0) return 1; // ler o read_command (0x20)
  
  kbc_read(); // ler command byte

  if(sys_outb(INPUT_BUFFER_COMMANDS, WRITE_COMMAND) != 0) return 1; // escrever o write_command (0x60)
  if(sys_outb(INPUT_BUFFER_ARGUMENTS, scancode | BIT(0)) != 0) return 1; // escrever o command byte

  kbd_print_no_sysinb(cnt);
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {  
  uint8_t bit_no_timer = IRQ0;
  uint32_t irq_set_kbd = BIT(IRQ1);
  uint32_t irq_set_timer = BIT(bit_no_timer);

  if(kbc_subscribe() != 0)
    return 1;

  if(timer_subscribe_int(&bit_no_timer) != 0)
    return 1;

  int ipc_status;
  message msg;
  double r = 0;

  while (scancode != ESCAPE) { //critério ESC
    if((r = driver_receive(ANY, &msg, &ipc_status)) != 0){
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if(is_ipc_notify(ipc_status)){ /* received notification */
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE:                             /* hardware interrupt notification */
          if(msg.m_notify.interrupts & irq_set_kbd){ /* subscribed interrupt */
            kbc_ih(); //calls interrupt handler
            kbc_print_scancode(); //prints scancode
            counter = 0;
          }
          if(msg.m_notify.interrupts & irq_set_timer){
            timer_int_handler();
            printf("Counter -> %d\n", counter);
            if(counter >= n * 60)
              scancode = ESCAPE;
          }
          break;
        default: break;
      }
    }
  }
  kbc_unsubscribe();
  timer_unsubscribe_int();

  kbd_print_no_sysinb(cnt); //prints thenumber of sys_inb used

  return 0;
}
