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
 |  Author: Mihai Baneu                           Last modified: 03.Mar.2022  |
 |                                                                            |
 |___________________________________________________________________________*/

#include <stdint.h>
#include "defines.h"
#include "button.h"

#define BUTTON_1_MIN  (330-50)
#define BUTTON_1_MAX  (330+50)

#define BUTTON_2_MIN  (660-50)
#define BUTTON_2_MAX  (660+50)

#define BUTTON_12_MIN  (500-50)
#define BUTTON_12_MAX  (500+50)

static uint8_t counter[3];
static uint8_t state[3];

void button_init()
{
    counter[BUTTON_1] = 0;
    counter[BUTTON_2] = 0;
}

void button_measure(uint16_t v)
{
    if ((BUTTON_1_MIN < v) && (v < BUTTON_1_MAX)) {
        if (counter[BUTTON_1] < 50) {
            counter[BUTTON_1]++;
        }
    } else {
        counter[BUTTON_1] = 0;
    }

    if ((BUTTON_2_MIN < v) && (v < BUTTON_2_MAX)) {
        if (counter[BUTTON_2] < 50) {
            counter[BUTTON_2]++;
        }
    } else {
        counter[BUTTON_2] = 0;
    }

    if ((BUTTON_12_MIN < v) && (v < BUTTON_12_MAX)) {
        if (counter[BUTTON_12] < 50) {
            counter[BUTTON_12]++;
        }
    } else {
        counter[BUTTON_12] = 0;
    }

    if (counter[BUTTON_1] == 0) {
        if (state[BUTTON_1] & BUTTON_PRESSED) {
            state[BUTTON_1] = BUTTON_RELEASED;
        }
        else {
            state[BUTTON_1] = 0;
        }
    } 
    else if (counter[BUTTON_1] > 2) {
        state[BUTTON_1] SET BUTTON_PRESSED;
        if (counter[BUTTON_1] > 20) {
            state[BUTTON_1] SET BUTTON_HOLD;
        }
    }

    if (counter[BUTTON_2] == 0) {
        if (state[BUTTON_2] & BUTTON_PRESSED) {
            state[BUTTON_2] = BUTTON_RELEASED;
        }
        else {
            state[BUTTON_2] = 0;
        }
    } 
    else if (counter[BUTTON_2] > 2) {
        state[BUTTON_2] SET BUTTON_PRESSED;
        if (counter[BUTTON_2] > 20) {
            state[BUTTON_2] SET BUTTON_HOLD;
        }
    }

    if (counter[BUTTON_12] == 0) {
        if (state[BUTTON_12] & BUTTON_PRESSED) {
            state[BUTTON_12] = BUTTON_RELEASED;
        }
        else {
            state[BUTTON_12] = 0;
        }
    } 
    else if (counter[BUTTON_12] > 2) {
        state[BUTTON_12] SET BUTTON_PRESSED;
        if (counter[BUTTON_12] > 20) {
            state[BUTTON_12] SET BUTTON_HOLD;
        }
    }
}

uint8_t button_state(uint8_t button)
{
    return state[button];
}

