#include "keyboard.h"
#include <i8042.h>
#include <i8254.h>

#include <stdio.h>

int hook_id_kbd = 0;
uint8_t scancode = 0x00;

int kbc_subscribe(uint8_t *bit_no){
    hook_id_kbd = 1;
//   hook_id = KBD_IRQ;
    *bit_no = (uint8_t) hook_id_kbd;
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

  if(st & OUT_BUFF){
    util_sys_inb(0X60, &scancode);
    if((st  &  (KBC_PAR_ERR | KBC_TO_ERR) )==0){
      return ;
    }
    else{printf("Parrity or TIme out error\n");}
    }
}

int bytes = 1;
uint8_t scancodes[2];
bool makecode = false;

bool printSC()  {

    if(scancode == 0xE0){
        scancodes[0] = scancode;
        bytes = 2;
        //printf("returning\n");
        return false;
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
    return true;
}

uint8_t writeCommand(uint8_t cmd, uint8_t port)  {

    uint8_t stat;

    while( 1 ) {
    if(util_sys_inb(STATUS_KBC, &stat)) /* assuming it returns OK */
        return 1;       /* loop while 8042 input buffer is not empty */
    if( (stat & IN_BUFF) == 0 ) {
        if(sys_outb(port, cmd)) /* no args command */
            return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US)); // e.g. tickdelay()
    }

}


uint8_t readData()  {

    uint8_t stat;

    while( 1 ) {
        if(util_sys_inb(STATUS_KBC, &stat)) /* assuming it returns OK */
            return 0;                       /* loop while 8042 output buffer is empty */
        if( stat & OUT_BUFF ) {
            if(util_sys_inb(OUTPUT_BUFFER, &scancode))
                return 0;                        /* ass. it returns OK */
            if ( (stat &(KBC_PAR_ERR | KBC_TO_ERR)) == 0 )
                    return 1;
                else
                    return 0;
        }
    tickdelay(micros_to_ticks(DELAY_US)); // e.g. tickdelay()
    }
    // 0, false means error
}

uint8_t kbcEnableInterrutpt()  {

    // uint8_t  data;

    if(sys_outb( STATUS_KBC, READ_COMMAND))
        return 1;

    readData();

    if(sys_outb( STATUS_KBC, WRITE_COMMAND))
        return 1;
    if(sys_outb(INPUT_BUFFER_ARGUMENTS, (scancode | BIT(0))))
        return 1;

    return 0;
}

//Write command byte b4 exut

//CHeck IBF b4 writing

//KBC COMMAND BYTE - BYTE 0 to enable interruutp on OBF

//READ - command 0x20 to 0x64 & value read from 0x60

//Write- command 0x60 to 0x64 & value writen to 0x60

