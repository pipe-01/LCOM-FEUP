#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"
#include "mouse.h"

struct packet packet1;
uint8_t read_packet;
bool is_sync = false;
int counter = 0;
int hook_id_mouse;
uint32_t total_packets = 0;
bool reading_error = false;




int(mouse_subscribe)(uint8_t *bit_no) {

  hook_id_mouse = 2;
  *bit_no = (uint8_t) hook_id_mouse;
  if (sys_irqsetpolicy(IRQ12, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse) != 0) return 1;
  return 0;
}

int(mouse_unsubscribe)() {

  if (sys_irqrmpolicy(&hook_id_mouse) != OK)
    return 1;
  return 0;
}

int(write_argument_to_mouse)(uint8_t argument){
  uint8_t acknow;
  uint8_t status;

  while(true){
    if(util_sys_inb(STATUS_REGISTER, &status)!= 0) return 1;
    if((status & BIT(1)) == 0) {
      if (sys_outb(INPUT_BUFFER_COMMANDS, ISSUE_COMMAND_TO_THE_MOUSE) != 0) return 1;
      break;
    }
  }

  while(true){
    if(util_sys_inb(STATUS_REGISTER, &status)!= 0) return 1;
    if((status & BIT(1)) == 0) { 
      if (sys_outb(INPUT_BUFFER_ARGUMENTS,argument)!= 0) return 1;  

      if (util_sys_inb(OUTPUT_BUFFER,&acknow) != 0) return 1;
      if(acknow == IS_OK) return 0;
    }
  }

}

int(enable_mouse_data)(){

  if (write_argument_to_mouse(SET_STREAM_MODE)) return 1;
  if (write_argument_to_mouse(ENABLE_MOUSE_DATA)) return 1;

  return 0;
}

int(disable_mouse_data)(){

  if (write_argument_to_mouse(DISABLE_MOUSE_DATA)!= 0) return 1;
  if (write_argument_to_mouse(SET_STREAM_MODE)!= 0) return 1;
  return 0;
}

int (kbc_read)(){
  uint8_t stat;

  while (true){
    if (util_sys_inb(STATUS_REGISTER, &stat) != OK) {
      printf("Couldn't read status from keyboard.\n");
      return 1;
    }

    if (stat & OUT_BUFF) {
      if (util_sys_inb(OUTPUT_BUFFER, &read_packet) != OK) {
        printf("Couldn't read scancode from buffer.\n");
        return 1;
      }
      if ((stat & (KBC_PAR_ERR | KBC_TO_ERR)) == 0) { 
        return 0;
      }
      else
        return 1;
    }
  }
}

void(mouse_ih)() {

  uint8_t stat;

  while(true) { //number of iterations that does not cause the program to timeout

    if (util_sys_inb(STATUS_REGISTER, &stat) != 0) { //reads status from the status register
      printf("Error! Couldn't read status from keyboard.\n");
      break;
    }

    if (stat & OUT_BUFF) { //checks if there is anything inside the output buffer

      if (util_sys_inb(OUTPUT_BUFFER, &read_packet) != 0) { //reads scancode from the output buffer
        printf("Error! Couldn't read scancode from buffer.\n");
        break;
      }

      if ((stat & (KBC_PAR_ERR | KBC_TO_ERR)) == 0) { //If there are no parity error (bit 7) or time out (bit 6) the scancode is valid and the function returns
        if (reading_error == true){
          counter ++;
        }
        return;
      }
      else {

        printf("Error! Parity error or Time Out recieved.\n");
        break;
      }
    }

    tickdelay(micros_to_ticks(WAIT_KBC)); //waits 20ms
  }
  reading_error = true;
  counter ++;
}

void(print_mouse_packet)() {
  
  if (is_sync == false) {

    if ((read_packet & 0x08) == 0) {
      return;
    }
    else
      is_sync = true;
  }

  if (counter == 0) {
    packet1.bytes[0] = read_packet;
    counter++;
    return;
  }

  if (counter == 1) {
    packet1.bytes[1] = read_packet;
    counter++;
    return;
  }

  if (counter == 2) {

    packet1.bytes[2] = read_packet;
    counter = 0;
    if((packet1.bytes[0] & 0x80)!= 0) packet1.y_ov = true;
    else packet1.y_ov = false;
    if((packet1.bytes[0] & 0x40)!= 0) packet1.x_ov = true;
    else packet1.x_ov = false;
    if ((packet1.bytes[0] & 0x20) != 0){
      packet1.delta_y = packet1.bytes[2] - 256;
    }
    else
      packet1.delta_y = packet1.bytes[2];
    if ((packet1.bytes[0] & 0x10) != 0){
      packet1.delta_x = packet1.bytes[1] - 256;
    }
    else
      packet1.delta_x = packet1.bytes[1];
    if ((packet1.bytes[0] & 0x04) != 0)
      packet1.mb = true;
    else
      packet1.mb = false;
    if ((packet1.bytes[0] & 0x02) != 0)
      packet1.rb = true;
    else
      packet1.rb = false;
    if ((packet1.bytes[0] & 0x01) != 0)
      packet1.lb = true;
    else
      packet1.lb = false;
    total_packets ++;
  }
}
