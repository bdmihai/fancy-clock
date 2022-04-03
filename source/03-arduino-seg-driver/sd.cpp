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
 |  Author: Mihai Baneu                           Last modified: 24.Feb.2022  |
 |                                                                            |
 |___________________________________________________________________________*/

#include <stdint.h>
#include <avr/io.h>
#include "defines.h"
#include "sd.h"

static const uint8_t segments[] = {
    0b10001000, // 0
    0b11101011, // 1
    0b01001100, // 2
    0b01001001, // 3
    0b00101011, // 4
    0b00011001, // 5, 
    0b00011000, // 6
    0b11001011, // 7
    0b00001000, // 8
    0b00001001, // 9
    0b11111111  // null
};

void sd_init()
{
    DDRD SET 0xFF;
    DDRB SET 0x0F;
}

void sd_enable(uint8_t number) {
    PORTB SET 0x0F;
    PORTB RESET BIT(number);
}

void sd_disable() {
    PORTB SET 0x0F;
}

void sd_write(uint8_t digit, bool point) {
  uint8_t mask = segments[digit] | (point ? 0b00001000 : 0b00000000);
  PORTD = mask;
}
