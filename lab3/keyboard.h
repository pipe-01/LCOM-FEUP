#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "i8042.h"

extern uint8_t scancode;
extern int bytes;
extern uint8_t scancodes[2];
extern bool makecode;

int kbc_subscribe(uint8_t *bit_no);

int kbc_unsubscribe();

void printSC();

#endif
