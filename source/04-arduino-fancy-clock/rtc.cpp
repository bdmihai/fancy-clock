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
 
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "defines.h"
#include "rtc.h"

#define DDR   DDRC
#define PORT  PORTC
#define PIN   PINC
#define SCLK  3
#define IO    4
#define CE    5

static void write_byte(uint8_t *byte)
{
    DDR SET BIT(IO);
    for (int i = 0; i < 8; i++) {
        if (bit_is_set(*byte, i)) {
            PORT SET BIT(IO);
        } 
        else {
            PORT RESET BIT(IO);
        }

        // pulse clock
        _delay_us(1);
        PORT SET BIT(SCLK);
        
        _delay_us(1);
        PORT RESET BIT(SCLK);
    }
}

static void read_byte(uint8_t *byte)
{
    DDR RESET BIT(IO);
    for (int i = 0; i < 8; i++) {
        
        
        if (bit_is_set(PIN, IO)) {
            *byte SET BIT(i);
        } else {
            *byte RESET BIT(i);
        }

        // pulse clock
        PORT SET BIT(SCLK);
        _delay_us(1);
        
        PORT RESET BIT(SCLK);
        _delay_us(1);
    }
}

void rtc_init()
{
    // configure the port B pins as output
    DDR SET (BIT(SCLK) | BIT(CE));
    DDR RESET BIT(IO);

    // set the clock and chip enable to low
    PORT RESET (BIT(SCLK) | BIT(CE));
}

void rtc_write_register(rtc_register_t reg, uint8_t address)
{
    uint8_t command = (RTC_CMD | RTC_CMD_WR | RTC_CMD_CLOCK | (address << 1));

    // enable the chip
    PORT SET BIT(CE);

    write_byte(&command);
    write_byte((uint8_t *)&reg);

    // disable the chip and set the IO pin to input
    DDR RESET BIT(IO);
    PORT RESET BIT(CE);
}

rtc_register_t rtc_read_register(uint8_t address)
{
    rtc_register_t reg;
    uint8_t command = (RTC_CMD | RTC_CMD_RD | RTC_CMD_CLOCK | (address << 1));

    // enable the chip
    PORT SET BIT(CE);

    write_byte(&command);
    read_byte((uint8_t *)&reg);

    // disable the chip and set the IO pin to input
    DDR RESET BIT(IO);
    PORT RESET BIT(CE);

    return reg;
}

void rtc_write_clock(rtc_clock_t *clock)
{
    uint8_t command = (RTC_CMD | RTC_CMD_WR | RTC_CMD_CLOCK | (0x1F << 1));
    uint8_t *pdata = clock->data;
    uint8_t size = sizeof(rtc_clock_t);

    // enable the chip
    PORT SET BIT(CE);

    write_byte(&command);
    while (size > 0) {
        write_byte(pdata++);
        size--;
    }
    
    // disable the chip and set the IO pin to input
    DDR RESET BIT(IO);
    PORT RESET BIT(CE);
}

void rtc_read_clock(rtc_clock_t *clock)
{
    uint8_t command = (RTC_CMD | RTC_CMD_RD | RTC_CMD_CLOCK | (0x1F << 1));
    uint8_t *pdata = clock->data;
    uint8_t size = sizeof(rtc_clock_t);

    // enable the chip
    PORT SET BIT(CE);

    write_byte(&command);
    while (size > 0) {
        read_byte(pdata++);
        size--;
    }
    
    // disable the chip and set the IO pin to input
    DDR RESET BIT(IO);
    PORT RESET BIT(CE);
}

void rtc_write_ram_byte(uint8_t data, uint8_t address)
{
    uint8_t command = (RTC_CMD | RTC_CMD_WR | RTC_CMD_RAM | (address << 1));

    // enable the chip
    PORT SET BIT(CE);

    write_byte(&command);
    write_byte(&data);

    // disable the chip and set the IO pin to input
    DDR RESET BIT(IO);
    PORT RESET BIT(CE);
}

uint8_t rtc_read_ram_byte(uint8_t address)
{
    uint8_t data;
    uint8_t command = (RTC_CMD | RTC_CMD_RD | RTC_CMD_RAM | (address << 1));

    // enable the chip
    PORT SET BIT(CE);

    write_byte(&command);
    read_byte(&data);

    // disable the chip and set the IO pin to input
    DDR RESET BIT(IO);
    PORT RESET BIT(CE);

    return data;
}

void rtc_write_ram(rtc_ram_t *ram, uint8_t size)
{
    uint8_t command = (RTC_CMD | RTC_CMD_WR | RTC_CMD_RAM | (0x1F << 1));
    uint8_t *pdata = ram->data;

    // enable the chip
    PORT SET BIT(CE);

    write_byte(&command);
    while (size > 0) {
        write_byte(pdata++);
        size--;
    }
    
    // disable the chip and set the IO pin to input
    DDR RESET BIT(IO);
    PORT RESET BIT(CE);
}

void rtc_read_ram(rtc_ram_t *ram, uint8_t size)
{
    uint8_t command = (RTC_CMD | RTC_CMD_RD | RTC_CMD_RAM | (0x1F << 1));
    uint8_t *pdata = ram->data;

    // enable the chip
    PORT SET BIT(CE);

    write_byte(&command);
    while (size > 0) {
        read_byte(pdata++);
        size--;
    }
    
    // disable the chip and set the IO pin to input
    DDR RESET BIT(IO);
    PORT RESET BIT(CE);
}
