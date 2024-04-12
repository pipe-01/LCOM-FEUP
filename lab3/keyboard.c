#include "keyboard.h"
#include <i8042.h>
#include <i8254.h>

#include <stdio.h>

int hook_id_kbd = 0;
uint8_t scancode = 0x00;

int kbc_subscribe(uint8_t *bit_no){
    hook_id_kbd = *bit_no;

    if(sys_irqsetpolicy(IRQ1,IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_kbd) != OK)
        return 1;
 
    return 0;
}


int kbc_unsubscribe(){
    if(sys_irqrmpolicy(&hook_id_kbd) != OK)
        return 1;
    return 0;
}


/* Interrupt Handler */
void (kbc_ih)(){

  uint8_t st;
  util_sys_inb(0x64,&st);

  if(st & BIT(0)){
    util_sys_inb(0X60, &scancode);
    if((st  &  (BIT(7) | BIT(6)))==0){
      return ;
    }
    else{printf("Parrity or TIme out error\n");}
    }
}

int bytes = 1;
uint8_t scancodes[2];
bool makecode = false;

void printSC()  {

    if(scancode == 0xE0){
        scancodes[0] = scancode;
        bytes = 2;
        //printf("returning\n");
        return;
    }

    if((scancode & 0x80) == 0){
        makecode = true;
    }

    if(bytes == 2){
        scancodes[1] = scancode;}
    else{
        scancodes[0] = scancode;
    }

    kbd_print_scancode(makecode, bytes, scancodes);

    bytes = 1;
    scancodes[0] = 0;
    scancodes[1] = 0;
    makecode = false;
}

