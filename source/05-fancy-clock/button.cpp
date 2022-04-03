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

#define BUTTON_CNT     3

typedef struct {
    uint16_t min;
    uint16_t max;
} v_limits_t;

static uint8_t counter[BUTTON_CNT];
static button_state_t state[BUTTON_CNT], prev_state[BUTTON_CNT];
static button_event_t event;

static const v_limits_t v_limits[BUTTON_CNT] = {
    { BUTTON_1_MIN, BUTTON_1_MAX },
    { BUTTON_2_MIN, BUTTON_2_MAX },
    { BUTTON_12_MIN, BUTTON_12_MAX },
};

void button_init()
{
    for (uint8_t i = 0; i < BUTTON_CNT; i++) {
        counter[i]    = 0;
        state[i]      = button_released;
        prev_state[i] = button_released;
    }

    event.button = 0;
    event.state = button_released;
}

void button_measure(uint16_t v)
{
    event.button = 0xFF;
    event.state = button_none;
    for (uint8_t i = 0; i < BUTTON_CNT; i++) {
        if ((v_limits[i].min < v) && (v < v_limits[i].max)) {
            if (counter[i] < 50) {
                counter[i]++;
            }
        } else {
            counter[i] = 0;
        }

        if (counter[i] == 0) {
            state[i] = button_released;
        } 
        else if (counter[i] > 10) {
            state[i] = button_hold;
        }
        else if (counter[i] > 1) {
            state[i] = button_pressed;
        }

        if (prev_state[i] == button_released && state[i] == button_pressed) {
            event.button = i;
            event.state = button_pressed;
        }
        else if (prev_state[i] == button_pressed && state[i] == button_hold) {
            event.button = i;
            event.state = button_hold;
        }
        else if (prev_state[i] == button_pressed && state[i] == button_released) {
            event.button = i;
            event.state = button_released;
        }
        else if (prev_state[i] == button_hold && state[i] == button_released) {
            event.button = i;
            event.state = button_hold_released;
        }

        prev_state[i] = state[i];
    }
}

button_state_t button_state(uint8_t button)
{
    return state[button];
}

button_event_t button_event()
{
    return event;
}

void button_clear_event()
{
    event.button = 0xFF;
    event.state = button_none;
}
