#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "i8042.h"

int kbc_subscribe();

int kbc_unsubscribe();

void kbc_print_scancode();

int kbc_read();

#endif
