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

#include "defines.h"
#include "sd.h"
#include "adc.h"
#include "rtc.h"
#include "timer.h"
#include "led.h"
#include "ntc.h"
#include "buzzer.h"
#include "display.h"
#include "button.h"

typedef enum {
  loop_init = 0,
  loop_clock,
  loop_photoresistor,
  loop_thermistor,
  loop_button
} loop_state_t;

static volatile loop_state_t loop_state = loop_init;

uint16_t time_on = 0, time_off = 0;
uint16_t v_photorezistor = 500, v_thermistor = 500, v_button;
bool blink = false;

rtc_clock_t clock = {0};

uint8_t prev_button_1_state = 0, button_1_state = 0;
uint8_t prev_button_2_state = 0, button_2_state = 0;
uint8_t prev_button_12_state = 0, button_12_state = 0;

static void main_task()
{
  static uint16_t main_index = 0;

  if (main_index == 0) {
    // read the clock for the first time
    rtc_read_clock(&clock);

    // read adc
    adc_start(ADC_CHANNEL_THERMISTOR);
    v_thermistor = adc_read();
    
    adc_start(ADC_CHANNEL_PHOTORESISTOR);
    v_photorezistor = adc_read();

    adc_start(ADC_CHANNEL_BUTTON);
    v_button = adc_read();

    adc_start(ADC_CHANNEL_THERMISTOR);

    loop_state = loop_clock;
  }

  if (main_index == 125) {
    buzzer_on();
  }
  if (main_index == 130) {
    buzzer_off();
  }

  if (main_index % 250 == 125) {
    led_toggle();
    rtc_read_clock(&clock);
  } 

  if (main_index % 100 == 1) {
    v_thermistor = adc_read();
    adc_start(ADC_CHANNEL_PHOTORESISTOR);
  }

  if (main_index % 100 == 33) {
    v_photorezistor = adc_read();
    adc_start(ADC_CHANNEL_BUTTON);
  }

  if (main_index % 100 == 66) {
    v_button = adc_read();
    button_measure(v_button);
    adc_start(ADC_CHANNEL_THERMISTOR);
  }

  button_1_state = button_state(BUTTON_1);
  button_2_state = button_state(BUTTON_2);
  button_12_state = button_state(BUTTON_12);
  if ((button_1_state & BUTTON_RELEASED) && (prev_button_1_state & BUTTON_PRESSED)) {
    if (loop_state == loop_clock) {
      loop_state = loop_photoresistor;
    }

    else if (loop_state == loop_photoresistor) {
      loop_state = loop_thermistor;
    }

    else if (loop_state == loop_thermistor) {
      loop_state = loop_button;
    }

    else if (loop_state == loop_button) {
      loop_state = loop_clock;
    }
  }
  prev_button_1_state = button_1_state;
  prev_button_2_state = button_2_state;
  prev_button_12_state = button_12_state;
  
  main_index++;
  if (main_index > 1000) {
    main_index = 1;
  }
}

void setup() {
  //Serial.begin(115200);
  loop_state = loop_init;

  // initialize the led module
  led_init();

  // initialize the buzzer module
  buzzer_init();

  // initialize the segmented display
  sd_init();

  // initialize the adc ports
  adc_init();

  // initialize the rtc clock
  rtc_init();

  button_init();

  // initialize the timer task
  timer_init(main_task);

//    // unlock the clock and ram
//    rtc_register_t reg;
//    reg.protection.write = 0;
//    reg.protection.reserved = 0;
//    rtc_write_register(reg, RTC_ADDR_WRITE_PROTECTION);
//
//    // set the clock
//    rtc_clock_t clock = {0};
//
//    clock.seconds.high = 0;
//    clock.seconds.low = 0;
//
//    clock.minutes.high = 4;
//    clock.minutes.low = 2;
//
//    clock.hours.format_24.high = 1;
//    clock.hours.format_24.low = 0;
//    
//    clock.date.low = 6;
//    clock.month.low = 3;
//    clock.weekday.low = 7;
//    clock.year.high = 2;
//    clock.year.low = 2;
//    clock.protection.write = 0;
//    rtc_write_clock(&clock);
}

void loop() {
  uint16_t value;
  time_on = v_photorezistor;
  time_off = 1024 - v_photorezistor;

  switch (loop_state) {
    case loop_init:
        display_off();
      break;

    case loop_clock:
        display_clock(&clock, v_photorezistor);
      break;

      case loop_photoresistor:
        display_value(v_photorezistor, v_photorezistor);
      break;

      case loop_thermistor:
        display_temperature(ntc_temperature(v_thermistor), v_photorezistor);
      break;

      case loop_button:
        display_value(v_button, v_photorezistor);
      break;

    default:
      break;
  }

  delay(5);
}

