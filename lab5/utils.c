#include <lcom/lcf.h>

#include <stdint.h>

int cnt = 0;

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb == NULL)
    return 1;
  else
    *lsb = (uint8_t) val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb == NULL)
    return 1;
  else{
    val = val >> 8;
    *msb = (uint8_t) val;
  }

  return 0;
}

int(util_sys_inb)(int port, uint8_t *value) {
  uint32_t st = 0;

  if(sys_inb(port, &st) != 0)
    return 1;
  
#ifdef LAB3
  cnt++;
#endif

  *value = (uint8_t) st;

  return 0;
}
