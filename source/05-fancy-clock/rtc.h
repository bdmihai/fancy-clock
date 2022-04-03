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

#pragma once

/* clock control */
#define RTC_CLOCK_RUN  0x00
#define RTC_CLOCK_HALT 0x01

/* write protection */
#define RTC_24H_MODE   0x00
#define RTC_12H_MODE   0x01
#define RTC_AM         0x00
#define RTC_PM         0x01

/* write protection */
#define RTC_WP_OFF     0x00
#define RTC_WP_ON      0x01

/* trickle charger options */
#define RTC_TC_OFF     0b00000101
#define RTC_TC_ON      0b00001010
#define RTC_TC_1d      0x01
#define RTC_TC_2d      0x02
#define RTC_TC_2k      0x01
#define RTC_TC_4k      0x02
#define RTC_TC_8k      0x03

/* command templates */
#define RTC_CMD           0b10000000
#define RTC_CMD_WR        0b00000000
#define RTC_CMD_RD        0b00000001
#define RTC_CMD_CLOCK     0b00000000
#define RTC_CMD_RAM       0b01000000

/* clock addresses */
#define RTC_ADDR_SECONDS           0x00
#define RTC_ADDR_MINUTES           0x01
#define RTC_ADDR_HOURS             0x02
#define RTC_ADDR_DATE              0x03
#define RTC_ADDR_MONTH             0x04
#define RTC_ADDR_WEEKDAY           0x05
#define RTC_ADDR_YEAR              0x06
#define RTC_ADDR_WRITE_PROTECTION  0x07
#define RTC_ADDR_TRICKLE_CHARGE    0x08

/* RAM in the real time clock */
#define RTC_RAM_SIZE   31

const static char* rtc_week_days[] =
{
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT",
    "SUN"
};

typedef struct {
    uint8_t low:4;
    uint8_t high:3;
    uint8_t clock_halt:1;
} rtc_seconds_t;

typedef struct {
    uint8_t low:4;
    uint8_t high:3;
    uint8_t reserved:1;
} rtc_minutes_t;

typedef struct {
    uint8_t low:4;
    uint8_t high:2;
    uint8_t format:1;
} rtc_hours_24_t;

typedef struct {
    uint8_t low:4;
    uint8_t high:1;
    uint8_t am_pm:1;
    uint8_t format:1;
} rtc_hours_12_t;

typedef union {
    rtc_hours_12_t format_12;
    rtc_hours_24_t format_24;
} rtc_hours_t;

typedef struct {
    uint8_t low:4;
    uint8_t high:2;
    uint8_t reserved:2;
} rtc_date_t;

typedef struct {
    uint8_t low:4;
    uint8_t high:2;
    uint8_t reserved:2;
} rtc_month_t;

typedef struct {
    uint8_t low:3;
    uint8_t reserved:5;
} rtc_weekday_t;

typedef struct {
    uint8_t low:4;
    uint8_t high:4;
} rtc_year_t;

typedef struct {
    uint8_t reserved:7;
    uint8_t write:1;
} rtc_protection_t;

typedef struct {
    uint8_t rs:2;
    uint8_t ds:2;
    uint8_t tcs:4;
} rtc_trickel_charge_t;

typedef struct {
    uint8_t data[RTC_RAM_SIZE];
} rtc_ram_t;

typedef union {
    struct {
        rtc_seconds_t seconds;
        rtc_minutes_t minutes;
        rtc_hours_t hours;
        rtc_date_t date;
        rtc_month_t month;
        rtc_weekday_t weekday;
        rtc_year_t year;
        rtc_protection_t protection;
    };
    uint8_t data[8];
} rtc_clock_t;

typedef union {
    rtc_seconds_t seconds;
    rtc_minutes_t minutes;
    rtc_hours_t hours;
    rtc_date_t date;
    rtc_month_t month;
    rtc_weekday_t weekday;
    rtc_year_t year;
    rtc_protection_t protection;
    rtc_trickel_charge_t trickel_charge;
} rtc_register_t;

void rtc_init();

void rtc_write_register(rtc_register_t reg, uint8_t address);
rtc_register_t rtc_read_register(uint8_t address);
void rtc_write_clock(rtc_clock_t *clock);
void rtc_read_clock(rtc_clock_t *clock);

void rtc_write_ram_byte(uint8_t data, uint8_t address);
uint8_t rtc_read_ram_byte(uint8_t address);
void rtc_write_ram(rtc_ram_t *ram, uint8_t size);
void rtc_read_ram(rtc_ram_t *ram, uint8_t size);
