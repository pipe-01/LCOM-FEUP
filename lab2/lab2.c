#include <lcom/lab2.h>
#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

extern int tmr_tck_counter;

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

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t *status = malloc(sizeof(uint8_t) * 1);

  if (timer_get_conf(timer, status) != 0)
    return 1;
  if (timer_display_conf(timer, *status, field) != 0) {
    perror("Error on function timer_display_config");
    return 1;
  }
  free(status);
  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  if (timer_set_frequency(timer, freq) != 0)
    return 1;
  else
    return 0;
}

int(timer_test_int)(uint8_t time) {
  /* To be implemented by the students */
  // printf("%s is not yet implemented!\n", __func__);

  uint8_t bit_no = 0;

  if (timer_subscribe_int(&bit_no) != 0)
    return 1;

  int ipc_status;
  message msg;
  double r = 0;

  // frequency = 60Hz
  int frequency = 60;
  if (timer_set_frequency(0, frequency) != 0) {
    timer_unsubscribe_int();
    return 1;
  }

  while (tmr_tck_counter < time * frequency) { /* You may want to use a different condition */
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification *11: */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & IRQ_SETPOLICY) {/* subscri14: ...  process it */

            timer_int_handler(); //Increments the counter
            
            if (tmr_tck_counter % 60 == 0) //Every 60 ticks (freq = 60Hz) 1 sec passes
              timer_print_elapsed_time();
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

  if (timer_unsubscribe_int() != 0)
    return 2;

  return 0;
}
