#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "i8042.h"

extern uint8_t scancode;
extern int bytes;
extern uint8_t scancodes[2];
extern bool makecode;

/** @defgroup keyboard keyboard
 * @{
 *
 * Functions for using keyboard
 */

/**
 * @brief Subscribes the keyboard
 * 
 * @param bit_no 
 * @return 0 on success 
 */
int kbc_subscribe(uint8_t *bit_no);

/**
 * @brief Unsubscribes the keyboard
 * 
 * @return 0 on success 
 */
int kbc_unsubscribe();

/**
 * @brief 
 * 
 * @return true 
 * @return false 
 */
bool printSC();

/**
 * @brief 
 * 
 * @param cmd 
 * @param port 
 * @return uint8_t 
 */
uint8_t writeCommand(uint8_t cmd, uint8_t port);

/**
 * @brief 
 * 
 * @return uint8_t 
 */
uint8_t readData();

/**
 * @brief 
 * 
 * @return uint8_t 
 */
uint8_t kbcEnableInterrutpt();

#endif
