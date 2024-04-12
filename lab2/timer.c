#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>


#include "i8254.h"

int tmr_tck_counter=0;
int hook_id = 0;


int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  
  uint8_t status; //default
  if(timer_get_conf(timer,&status)!=0)
    return 1;

  uint8_t last_bits = status & 0x0F; 

  if (freq < 19 || freq > TIMER_FREQ) 
    return 1;

  uint8_t control_word, timeraddr;
   
  switch (timer){
    case 0:
      timeraddr = TIMER_0;
      control_word = 0 | TIMER_LSB_MSB | last_bits; //building control word
      break;
    case 1:
      timeraddr = TIMER_1;
      control_word = TIMER_SEL1 | TIMER_LSB_MSB | last_bits; //building control word
      break;
    case 2:
      timeraddr = TIMER_2;
      control_word = TIMER_SEL2 | TIMER_LSB_MSB | last_bits; //building control word
      break;
    default:
      return 1;
      break;
  } 

  uint16_t div = TIMER_FREQ / freq;
  
  uint8_t lsb;
  util_get_LSB(div, &lsb);

  uint8_t msb;
  util_get_MSB(div, &msb);

  if(sys_outb(TIMER_CTRL, control_word))
    return 1; //send control word to timer_control

  if(sys_outb(timeraddr, lsb))
    return 1; //write the least significant byte to the corresponding counter register
  if(sys_outb(timeraddr, msb))
    return 1; //then, write the most significant byte
  
  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = (uint8_t) hook_id;
  hook_id = TIMER0_IRQ;
  if(sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id)!=OK)
    return 1;
  return 0;
  // timer_subscribe_int() must return, via its input argument,
  // the value that it has passed to the kernel in the third argument of sys_irqsetpolicy().

}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  if(sys_irqrmpolicy(&hook_id)!=OK)
    return 1;
  return 0;

}

void (timer_int_handler)() {
  /* To be implemented by the students */
  tmr_tck_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t rbc = 0, port = 0;

  if(timer > 2 || timer < 0){
    return 1;
  }

  rbc = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  sys_outb(TIMER_CTRL, rbc);

  switch (timer){
    case 0:
      port = TIMER_0;
      break;
    case 1:
      port = TIMER_1;
      break;
    case 2:
      port = TIMER_2;
      break;
    default:
      break;
  }

  if(util_sys_inb(port, st) != 0)
    return 1;

  //printf("%s is not yet implemented!\n", __func__);

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  
  union timer_status_field_val val;
  uint8_t aux = 0;

  // if(timer_get_conf(timer, &st) != 0)
  //   return 1;

  // tsf_all,     /*!< configuration/status */
  // tsf_initial, /*!< timer initialization mode */
  // tsf_mode,    /*!< timer counting mode */
  // tsf_base     /*!< timer counting base */

  switch(field) {
    case tsf_all :
      val.byte = st;
      break;

    case tsf_initial :
      val.in_mode = (st & BIT(5)) | (st & BIT(4)) >> 4;
      break;

    case tsf_mode :
      val.count_mode = (st & BIT(3)) | (st & BIT(2)) | (st & BIT(1)) >> 1;
      break;

    case tsf_base :
      aux = (st & BIT(0));
      if (aux == 1) {
        val.bcd = true;
        }
      else {
        val.bcd = false;
      }
      break;        

  }

  if(timer_print_config(timer, field, val) != 0)
        return 1;

  /* To be implemented by the students */
  // printf("%s is not yet implemented!\n", __func__);


  return 0;
}
