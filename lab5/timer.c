#include <lcom/lcf.h>
#include "utils.h"
#include "../lab2/i8254.h"

int hook_id_timer;
int counter;

int(timer_subscribe_int)(uint8_t *bit_no) {
  hook_id_timer = 0;
  //call sys_irq_setpolicy
  int policy = IRQ_REENABLE;
  int IRQ_line = 0;

  if (sys_irqsetpolicy(IRQ_line, policy, &hook_id_timer) != OK) {
    return 1;
  };
  //update bit_no

  return OK;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {

  if (timer != 0 && timer != 1 && timer != 2) { //Checking if the timer is valid

    printf("Error, invalid timer.\n");
    return 1;
  }

  uint8_t rbcmd;

  rbcmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer); // building read back command

  //printing the read-back command for debug:
  //printf("READ-BACK COMMAND: ");
  //printf("%d\n", rbcmd);

  if (sys_outb(TIMER_CTRL, rbcmd)!= 0) return 1; // writting the read-back command to the control register

  if (util_sys_inb(0x40 + timer, st) == 0)
    return 0; //read status from timer
  else {
    printf("Error, couldn't read status.\n");
    return 1;
  }
}


int(timer_unsubscribe_int)() {
  //call sys_irq_
  if (sys_irqrmpolicy(&hook_id_timer) != OK) {
    return 1;
  };
  return OK;
}

void(timer_int_handler)() {
  counter++;
}

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  //read the timer's current configs
  if (freq < 19 || freq > TIMER_FREQ) {
    return 1; //a frequencia tem de ser maior que 19 pois TIMER_FREQ/18 dá um numero não representável em 16 bits e div tem de ser no minimo 2, logo freq tem de ser menos que TIMER_FREQ
  }
  uint8_t st = 0; //st will have the old control word
  if (timer_get_conf(timer, &st) == 0) {
    printf("Read old configs succefully!\n");
  }
  else {
    printf("Error in reading the config!\n");
    return 1;
  }
  //get the bits of the old control word that we wanna keep (everything except the initialization //configs)
  uint8_t last_bits = st & 0x0F;

  uint8_t control_word = TIMER_RB_SEL(timer) | TIMER_LSB_MSB | last_bits; //building control word
  //printf("The control word is %d \n",control_word );

  uint16_t div = TIMER_FREQ / freq;
  uint16_t freq_rounded = TIMER_FREQ / div;
  //há a possibilidade de este div não ser represntável, logo pode haver necessidade de arrendondar
  if (freq != freq_rounded) {
    printf("The frequency %d Hz you inputed cannot be represented, so we will round it to the nearest number which is %d Hz\n", freq, freq_rounded);
  }
  else {
    printf("Set the timer frequency to %dHz\n", freq);
  }

  //printf("div value: %d\n",div);
  uint8_t lsb;
  util_get_LSB(div, &lsb);
  //printf("lsb: %d\n",lsb);
  uint8_t msb;
  util_get_MSB(div, &msb);
  //printf("msb: %d \n",msb);

  sys_outb(TIMER_CTRL, control_word); //send control word to timer_control

  sys_outb(TIMER_0 + timer, lsb); //write the least significant byte to the corresponding counter register
  sys_outb(TIMER_0 + timer, msb); //then, write the most significant byte
  return 0;
}
