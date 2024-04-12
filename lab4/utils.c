#include <lcom/lcf.h>

#include <stdint.h>

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
  /* To be implemented by the students */
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
  /* To be implemented by the students */
  // printf("%s is not yet implemented!\n", __func__);

  uint32_t byt32 = 0;

  if(sys_inb(port, &byt32) != 0)
    return 1;

  *value = (uint8_t) byt32;
  


  return 0;
}
