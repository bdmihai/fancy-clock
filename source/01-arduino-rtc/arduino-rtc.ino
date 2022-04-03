/*_____________________________________________________________________________
 │                                                                            |
 │ COPYRIGHT (C) 2022 Mihai Baneu                                             |
 │                                                                            |
 | Permission is hereby  granted,  free of charge,  to any person obtaining a |
 | copy of this software and associated documentation files (the "Software"), |
 | to deal in the Software without restriction,  including without limitation |
 | the rights to  use, copy, modify, merge, publish, distribute,  sublicense, |
 | and/or sell copies  of  the Software, and to permit  persons to  whom  the |
 | Software is furnished to do so, subject to the following conditions:       |
 |                                                                            |
 | The above  copyright notice  and this permission notice  shall be included |
 | in all copies or substantial portions of the Software.                     |
 |                                                                            |
 | THE SOFTWARE IS PROVIDED  "AS IS",  WITHOUT WARRANTY OF ANY KIND,  EXPRESS |
 | OR   IMPLIED,   INCLUDING   BUT   NOT   LIMITED   TO   THE  WARRANTIES  OF |
 | MERCHANTABILITY,  FITNESS FOR  A  PARTICULAR  PURPOSE AND NONINFRINGEMENT. |
 | IN NO  EVENT SHALL  THE AUTHORS  OR  COPYRIGHT  HOLDERS  BE LIABLE FOR ANY |
 | CLAIM, DAMAGES OR OTHER LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT |
 | OR OTHERWISE, ARISING FROM,  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR  |
 | THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                 |
 |____________________________________________________________________________|
 |                                                                            |
 |  Author: Mihai Baneu                           Last modified: 23.Feb.2022  |
 |                                                                            |
 |___________________________________________________________________________*/

#include <Arduino.h>
#include "rtc.h"

void setup()
{
    Serial.begin(115200);
    rtc_init();

    // unlock the clock and ram
    rtc_register_t reg;
    reg.protection.write = 0;
    reg.protection.reserved = 0;
    rtc_write_register(reg, RTC_ADDR_WRITE_PROTECTION);

    // set the clock
    rtc_clock_t clock = {0};
    clock.minutes.high = 3;
    clock.minutes.low = 1;
    clock.hours.format_24.high = 1;
    clock.hours.format_24.low = 2;
    clock.date.high = 1;
    clock.date.low = 5;
    clock.month.low = 3;
    clock.weekday.low = 2;
    clock.year.high = 2;
    clock.year.low = 2;
    clock.protection.write = 0;
    rtc_write_clock(&clock);
}

void loop()
{
    rtc_register_t reg = rtc_read_register(RTC_ADDR_YEAR);
    Serial.print("Clock: 20");
    Serial.print(reg.year.high);
    Serial.print(reg.year.low);
    Serial.print('-');

    reg = rtc_read_register(RTC_ADDR_MONTH);
    Serial.print(reg.month.high);
    Serial.print(reg.month.low);
    Serial.print('-');

    reg = rtc_read_register(RTC_ADDR_DATE);
    Serial.print(reg.date.high);
    Serial.print(reg.date.low);
    Serial.print(' ');

    reg = rtc_read_register(RTC_ADDR_HOURS);
    Serial.print(reg.hours.format_24.high);
    Serial.print(reg.hours.format_24.low);
    Serial.print(':');

    reg = rtc_read_register(RTC_ADDR_MINUTES);
    Serial.print(reg.minutes.high);
    Serial.print(reg.minutes.low);
    Serial.print(':');

    reg = rtc_read_register(RTC_ADDR_SECONDS);
    Serial.print(reg.seconds.high);
    Serial.print(reg.seconds.low);
    Serial.print(' ');

    reg = rtc_read_register(RTC_ADDR_WEEKDAY);;
    Serial.println(rtc_week_days[reg.weekday.low-1]);

    reg = rtc_read_register(RTC_ADDR_WRITE_PROTECTION);
    Serial.print("Write protection: ");
    Serial.println(reg.protection.write);

    reg = rtc_read_register(RTC_ADDR_TRICKLE_CHARGE);
    Serial.print("Trickle charge: ");
    Serial.print(reg.trickel_charge.tcs, BIN);
    Serial.print(' ');
    Serial.print(reg.trickel_charge.ds, BIN);
    Serial.print(' ');
    Serial.println(reg.trickel_charge.rs, BIN);

    delay(1000);

    rtc_ram_t ram = {
        .data = {
            0x8A, 0x8B, 0x8C, 0x81, 0x81, 0x81, 0x81, 0x81, 
            0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 
            0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 
            0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xED
        } 
    };
    rtc_write_ram(&ram, RTC_RAM_SIZE);
    _delay_us(100);
    rtc_read_ram(&ram, RTC_RAM_SIZE);

    Serial.print("RAM: ");
    for (int i = 0; i < RTC_RAM_SIZE; i++) {
        Serial.print(ram.data[i], HEX);
    }
    Serial.println();
    delay(1000);
}

