#include <lcom/lcf.h>
#include "utils.h"

#define IRQ8 8
#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define REG_A 10
#define REG_C 12
#define REG_B 11
#define REG_YEAR 0x09
#define REG_MONTH 0x08
#define REG_DAY 0x07
#define REG_HOUR 0x04
#define REG_MINUTE 0x02
#define REG_SECOND 0x00

/** @defgroup RTC RTC
 * @{
 *
 * Functions for RTC usage
 */


/**
 * @brief Subscribes the rtc
 * 
 * @return 0 on success 
 */
int rtc_subscribe();

/**
 * @brief Unubscribes the rtc
 * 
 * @return 0 on success
 */
int rtc_unsubscribe();

/**
 * @brief convert bcd number to decimal
 * 
 * @param bcd 
 * @return number converted
 */
unsigned int convert_bcd_to_dec(uint8_t bcd);

/**
 * @brief fills the struct Date with the actual date
 * 
 * @return 0 on success
 */
int get_date();
