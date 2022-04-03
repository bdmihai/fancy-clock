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
#include "adc.h"

void adc_init()
{
    // AVCC with external capacitor at AREF pin
    ADMUX SET BIT(REFS0);

    // set prescaller to 128 and enable
    ADCSRA SET (BIT(ADPS2) | BIT(ADPS1) | BIT(ADPS0) | BIT(ADEN));
}

void adc_start(uint8_t channel)
{
    // select ADC channel
    ADMUX RESET (0b00001111);
    ADMUX SET (channel);

    // start conversion
    ADCSRA SET BIT(ADSC);
}

uint16_t adc_read()
{
    // wait for conversion to finish
    loop_until_bit_is_clear(ADCSRA, ADSC);
    return ADC;
}
