#ifndef MOUSE_H
#define MOUSE_H

#include <lcom/lcf.h>

/** @defgroup mouse mouse
 * @{
 *
 * Functions for using mouse
 */

/**
 * @brief Subscribes the mouse
 * 
 * @return 0 on success 
 */
int (mouse_subscribe)(uint8_t *hook_id_mouse);

/**
 * @brief Unubscribes the mouse
 * 
 * @return 0 on success 
 */
int (mouse_unsubscribe)();

/**
 * @brief Mouse interrupt handler
 * 
 */
void(mouse_ih)();

/**
 * @brief updates the packets
 * 
 */
void (print_mouse_packet)();

/**
 * @brief Writes the argument to the input_buffer after sending the 0xD4 and reads the acknolege from output_buffer
 * 
 * @return 0 on success 
 */
int(write_argument_to_mouse)(uint8_t argument);

/**
 * @brief Sets the stream mode and enables the mouse data
 * 
 * @return 0 on success 
 */
int(enable_mouse_data)();

/**
 * @brief Disables the mouse data and sets the stream mode
 * 
 * @return 0 on success 
 */
int(disable_mouse_data)();

#endif // MOUSE_H
