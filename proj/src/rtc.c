#include <lcom/lcf.h>
#include "rtc.h"
#include "structs.h"

Date date;

int rtc_hook_id = 3;



int rtc_subscribe(){
    if(sys_irqsetpolicy(IRQ8, IRQ_REENABLE, &rtc_hook_id) != OK)
        return 1;

    return 0;
}

int rtc_unsubscribe(){
    if(sys_irqrmpolicy(&rtc_hook_id) != OK)
        return 1;

    return 0;
}


unsigned int convert_bcd_to_dec(uint8_t bcd){
    return ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F);
}



int get_date(){
    uint8_t year=0, month=0, day=0, hour=0, minute=0, second=0;

    sys_outb(RTC_ADDR_REG, REG_YEAR);
    util_sys_inb(RTC_DATA_REG, &year);

    sys_outb(RTC_ADDR_REG, REG_MONTH);
    util_sys_inb(RTC_DATA_REG, &month);

    sys_outb(RTC_ADDR_REG, REG_DAY);
    util_sys_inb(RTC_DATA_REG, &day);

    sys_outb(RTC_ADDR_REG, REG_HOUR);
    util_sys_inb(RTC_DATA_REG, &hour);

    sys_outb(RTC_ADDR_REG, REG_MINUTE);
    util_sys_inb(RTC_DATA_REG, &minute);

    sys_outb(RTC_ADDR_REG, REG_SECOND);
    util_sys_inb(RTC_DATA_REG, &second);

    date.year = convert_bcd_to_dec(year);
    date.month = convert_bcd_to_dec(month);
    date.day = convert_bcd_to_dec(day);
    date.hour = convert_bcd_to_dec(hour);
    date.minute = convert_bcd_to_dec(minute);
    date.second = convert_bcd_to_dec(second);

    return 0;
}
