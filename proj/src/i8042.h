#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Keyboard.
 */

/*waiting for KBC*/
#define WAIT_KBC 20000 

/**/
#define IRQ1 1
#define IRQ0 0
#define IRQ12 12


#define P_KEY_BREAKCODE 0x99
#define I_KEY_BREAKCODE 0x97
#define E_KEY_BREAKCODE 0x92


/*i8042 ports*/

#define STATUS_KBC 0x64 //read the KBC state

#define STATUS_REGISTER 0x64 //read the KBC state

#define INPUT_BUFFER_COMMANDS 0x64 //write commands to KBC access

#define INPUT_BUFFER_ARGUMENTS 0x60 //write arguments to KBC access

#define OUTPUT_BUFFER 0x60 //read sandcodes bothe make and break ans return values from KBC commands

/*status bits*/

#define OUT_BUFF BIT(0) /**< @brief Output buffer */

#define IN_BUFF BIT(1)  /**< @brief Input Buffer */

#define SYS_FLAG BIT(2) /**< @brief FLAG */

#define DATA_CMD BIT(3) /**< @brief Data Command 2 */

#define KBC_PAR_ERR BIT(7)  /**< @brief Keyboard parrity error */

#define KBC_TO_ERR BIT(6)   /**< @brief Keyboard error 2 */

/*Commands*/

#define READ_COMMAND 0x20   /**< @brief Read Command */

#define WRITE_COMMAND 0x60  /**< @brief Write command */

#define ISSUE_COMMAND_TO_THE_MOUSE 0xD4 /**< @brief COmmand to mouse */


/*ARGUMENTS MOUSE*/

#define ENABLE_MOUSE_DATA 0xF4  /**< @brief Enable mouse data */

#define DISABLE_MOUSE_DATA 0xF5 /**< @brief Disable mouse data */

#define SET_STREAM_MODE 0xEA    /**< @brief Set stream mode */

#define SET_REMOTE_MODE 0xF0    /**< @brief Set remote mode */

#define READ_DATA_MOUSE 0xEB    /**< @brief Read data mouse */

#define RESET_MOUSE 0xFF    /**< @brief Reset mouse */


/*KEYS*/

#define ESCAPE 0x81     /**< @brief Escape */
    


/*AKNOWLEDGMENTS*/

#define INVALID_BYTE 0xFE   /**< @brief Invalid Byte */

#define IS_ERROR 0xFC   /**< @brief Is Error */

#define IS_OK 0xFA  /**< @brief OK */

#define DELAY_US    20000   /**< @brief DELAY */

#endif
