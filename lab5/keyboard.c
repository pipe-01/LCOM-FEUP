#include <lcom/lcf.h>

#include "i8042.h"
#include "keyboard.h"
#include <stdint.h>

int hook_id_kbd = 0;
uint8_t scancode = 0;
uint8_t scancodes[2];

// sys_irqsetpolicy com IRQ_REENABLE | IRQ_EXCLUSIVE
int (kbc_subscribe)(){
  hook_id_kbd = IRQ1;

  if(sys_irqsetpolicy(IRQ1, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_kbd) != OK)
    return 1;
  
  return 0;
}

// irqrmpolicy
int (kbc_unsubscribe)(){
  if(sys_irqrmpolicy(&hook_id_kbd) != OK)
    return 1;

  return 0;
}

/* Interrupt Handler */
void (kbc_ih)(){
  uint8_t stat = 0;

  while(true){
    if(util_sys_inb(STATUS_REGISTER, &stat) != 0){
      printf("Error reading from stat buffer\n");
      return;
    }
    if(stat & OUT_BUFF){ // se o out_buff tiver lá alguma coisa, lê o scancode do output_buffer
      if(util_sys_inb(OUTPUT_BUFFER, &scancode) != 0){
        printf("Error reading from output buffer\n");
        return;
      }
      if((stat & (KBC_PAR_ERR | KBC_TO_ERR)) == 0)
        return;
      else{
        printf("Invalid scancode, Parity error or Timeout error\n");
        return;
      }
    }
    tickdelay(micros_to_ticks(WAIT_KBC));
  }

}

// calls kbd_print_scancode, deals with scancodes with 2 bytes long
void (kbc_print_scancode)(){
  if(scancode == 0xE0){
    scancodes[0] = scancode;
    return;
  }

  uint8_t n_bytes = 1;
  bool makecode = false;

  if((scancode & 0x80) == 0)
    makecode = true;

  if(scancodes[0] == 0xE0){
    scancodes[1] = scancode;
    n_bytes++;
  }
  else
    scancodes[0] = scancode;

  kbd_print_scancode(makecode, n_bytes, scancodes);

  makecode = false;
  scancodes[0] = 0;
}

// reads by polling instead of ih, it's the same pretty much
int (kbc_read)(){
  uint8_t stat = 0;

  while(true){
    if(util_sys_inb(STATUS_REGISTER, &stat) != 0){
      printf("Error reading from stat buffer\n");
      return 1;
    }
    if(stat & OUT_BUFF){ // se o out_buff tiver lá alguma coisa, lê o scancode do output_buffer
      if(util_sys_inb(OUTPUT_BUFFER, &scancode) != 0){
        printf("Error reading from output buffer\n");
        return 1;
      }
      if((stat & (KBC_PAR_ERR | KBC_TO_ERR)) == 0)
        return 0;
      else{
        printf("Invalid scancode, Parity error or Timeout error\n");
        return 1;
      }
    }
  }

  return 0;
}
