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
 |  Author: Mihai Baneu                           Last modified: 11.Mar.2022  |
 |                                                                            |
 |___________________________________________________________________________*/

#include <stdint.h>
#include "defines.h"
#include "rtc.h"
#include "sd.h"
#include "utils.h"
#include "display.h"

#define DISPLAY_DURATION 1024

void display_off()
{
    sd_write_rt(SD_OFF, false);
    sd_enable(3);
    delay_us(100);
    sd_disable();
    delay_us(100);

    sd_write(10, false);
    sd_enable(2);
    delay_us(100);
    sd_disable();
    delay_us(100);

    sd_write_rt(SD_OFF, false);
    sd_enable(1);
    delay_us(100);
    sd_disable();
    delay_us(100);

    sd_write(10, false);
    sd_enable(0);
    delay_us(100);
    sd_disable();
    delay_us(100);
}

void display_time(uint8_t hour_high, uint8_t hour_low, uint8_t minute_high, uint8_t minute_low, const uint16_t luminosity)
{
    sd_write_rt(hour_high, false);
    sd_enable(3);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write(hour_low, true);
    sd_enable(2);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write_rt(minute_high, true);
    sd_enable(1);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write(minute_low, false);
    sd_enable(0);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);
}

void display_time(uint8_t hour_high, uint8_t hour_low, uint8_t minute_high, uint8_t minute_low, const uint16_t luminosity, bool blink)
{
    sd_write_rt(blink ? SD_OFF_P : hour_high, false);
    sd_enable(3);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write(blink ? SD_OFF_P : hour_low, true);
    sd_enable(2);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write_rt(blink ? SD_OFF_P : minute_high, true);
    sd_enable(1);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write(blink ? SD_OFF_P : minute_low, false);
    sd_enable(0);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);
}

void  display_time(uint8_t hour_high, uint8_t hour_low, uint8_t minute_high, uint8_t minute_low, const uint16_t luminosity, bool blink, uint8_t digit, bool enabled)
{
    sd_write_rt((blink && (digit == 0)) ? SD_OFF : hour_high, false);
    sd_enable(3);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write((blink && (digit == 1)) ? SD_OFF_P : hour_low, true);
    sd_enable(2);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write_rt((blink && (digit == 2)) ? SD_OFF_P : minute_high, true);
    sd_enable(1);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write((blink && (digit == 3)) ? SD_OFF_P : minute_low, enabled);
    sd_enable(0);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);
}

void display_date(uint8_t month_high, uint8_t month_low, uint8_t date_high, uint8_t date_low, const uint16_t luminosity)
{
    sd_write_rt(month_high, false);
    sd_enable(3);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write(month_low, true);
    sd_enable(2);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write_rt(date_high, false);
    sd_enable(1);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write(date_low, false);
    sd_enable(0);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);
}

void display_date(uint8_t month_high, uint8_t month_low, uint8_t date_high, uint8_t date_low, const uint16_t luminosity, bool blink, uint8_t digit)
{
    sd_write_rt((blink && (digit == 0)) ? SD_OFF : month_high, false);
    sd_enable(3);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write((blink && (digit == 1)) ? SD_OFF_P : month_low, true);
    sd_enable(2);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write_rt((blink && (digit == 2)) ? SD_OFF : date_high, false);
    sd_enable(1);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write((blink && (digit == 3)) ? SD_OFF : date_low, false);
    sd_enable(0);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);
}

void display_year(uint8_t year_high, uint8_t year_low, const uint16_t luminosity)
{
    sd_write_rt(2, false);
    sd_enable(3);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write(0, false);
    sd_enable(2);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write_rt(year_high, false);
    sd_enable(1);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write(year_low, false);
    sd_enable(0);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);
}

void display_year(uint8_t year_high, uint8_t year_low, const uint16_t luminosity, bool blink, uint8_t digit)
{
    sd_write_rt(2, false);
    sd_enable(3);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write(0, false);
    sd_enable(2);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write_rt((blink && (digit == 0)) ? SD_OFF : year_high, false);
    sd_enable(1);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write((blink && (digit == 1)) ? SD_OFF : year_low, false);
    sd_enable(0);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);
}

void display_value(uint16_t value, const uint16_t luminosity, bool with_dot)
{
    uint16_t digits[4];

    digits[3] = value % 10;
    value /= 10;
    digits[2] = (value == 0) ? SD_OFF : (value % 10);
    value /= 10;
    digits[1] = (value == 0) ? SD_OFF : (value % 10);
    value /= 10;
    digits[0] = (value == 0) ? SD_OFF : (value % 10);

    sd_write_rt(digits[0], false);
    sd_enable(3);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write(digits[1], with_dot);
    sd_enable(2);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write_rt(digits[2], false);
    sd_enable(1);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write(digits[3], false);
    sd_enable(0);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);
}

void display_temperature(int16_t value, const uint16_t luminosity)
{
    uint16_t digits[4];

    if (value > 0) {
        digits[0] = SD_OFF;
    } else {
        digits[0] = SD_MINUS;
        value = -value;
    }
    digits[3] = SD_DEG;

    digits[2] = value % 10;
    value /= 10;
    digits[1] = (value == 0) ? SD_OFF : (value % 10);
    
    sd_write_rt(digits[0], false);
    sd_enable(3);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write(digits[1], false);
    sd_enable(2);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write_rt(digits[2], false);
    sd_enable(1);
    delay_us(luminosity);
    sd_disable();
    sd_write_rt(SD_OFF, false);
    delay_us(DISPLAY_DURATION - luminosity);

    sd_write(digits[3], false);
    sd_enable(0);
    delay_us(luminosity);
    sd_disable();
    sd_write(10, false);
    delay_us(DISPLAY_DURATION - luminosity);
}



