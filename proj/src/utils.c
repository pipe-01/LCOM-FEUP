#include <lcom/lcf.h>

#include <stdint.h>

int count = 0;

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL){
    return 1;
  }
  else{
    *lsb = (uint8_t) val;
    return 0;
  }

}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL){
    return 1;
  }
  else{
    val = val >> 8;
    *msb = (uint8_t) val;
    return 0;
  }
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t byt32 = 0;

  int aux = sys_inb(port, &byt32);

  #ifdef LAB3
    count++;
  #endif

  if(aux != 0)
    return 1;

  *value = (uint8_t) byt32;
  
  return 0;
}

