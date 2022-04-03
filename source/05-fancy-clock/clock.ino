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

// ----------------------------- alarm settings -------------------------------

typedef struct {
    uint8_t low:4;
    uint8_t high:3;
    uint8_t active:1;
} alarm_minutes_t;
static alarm_minutes_t alarm_minute = {0};

typedef struct {
    uint8_t low:4;
    uint8_t high:2;
} alarm_hours_type;
static alarm_hours_type alarm_hour = {0};

// -------- auxiliary variables used for the user actions and display ---------
static bool blink = false;
static uint8_t digit_index = 0;

// ----------------------------- adc measurements -----------------------------
static uint16_t v_photorezistor = 500, v_thermistor = 500, v_button = 0, version = 101;

// -------------------------------- the clock ---------------------------------
static rtc_clock_t clock = {0}, set_clock = {0};

// ------------------------ state transion definition -------------------------
typedef enum {
    loop_init = 0,
    loop_time,
    loop_set_time,
    loop_date,
    loop_set_date,
    loop_year,
    loop_set_year,
    loop_alarm,
    loop_set_alarm,
    loop_thermistor,
    loop_photoresistor,
    loop_button,
    loop_version
} loop_state_t;
static volatile loop_state_t loop_state = loop_init;

typedef bool (loop_transition_t)(void);
typedef void (loop_action_t)(void);

typedef struct {
    loop_state_t        from;    // the state from which we need to transition
    loop_state_t        to;      // what state we shall transition to
    loop_transition_t  *pfn_go;  // transition trigger function
    loop_action_t      *pfn_do;  // transition callback funtion
} loop_state_transition_t;

static bool button_1_released()
{
    button_event_t event = button_event();

    if (event.button == BUTTON_1 && event.state == button_released) {
        button_clear_event();
        return true;
    }

    return false;
}

static bool button_1_hold_released()
{
    button_event_t event = button_event();

    if (event.button == BUTTON_1 && event.state == button_hold_released) {
        button_clear_event();
        return true;
    }

    return false;
}

static bool button_2_released()
{
    button_event_t event = button_event();

    if (event.button == BUTTON_2 && event.state == button_released) {
        button_clear_event();
        return true;
    }

    return false;
}

static bool button_2_hold_released()
{
    button_event_t event = button_event();

    if (event.button == BUTTON_2 && event.state == button_hold_released) {
        button_clear_event();
        return true;
    }

    return false;
}

static bool button_12_hold_released()
{
    button_event_t event = button_event();

    if (event.button == BUTTON_12 && event.state == button_hold_released) {
        button_clear_event();
        return true;
    }

    return false;
}

static void alarm_init()
{
    digit_index = 0;
}

static void alarm_inc_digit()
{
    digit_index++;
    if (digit_index >= 4) {
        digit_index = 0;
    }
}

static void alarm_inc()
{
    switch (digit_index) {
    case 0:
        alarm_hour.high++;
        if (alarm_hour.high > 2) {
            alarm_hour.high = 0;
        }
        if (alarm_hour.high == 2 && alarm_hour.low > 3) {
            alarm_hour.high = 0;
        }
        break;
    case 1:
        alarm_hour.low++;
        if (alarm_hour.low > 9) {
            alarm_hour.low = 0;
        }
        if (alarm_hour.high == 2 && alarm_hour.low > 3) {
            alarm_hour.low = 0;
        }
        break;
    case 2:
        alarm_minute.high++;
        if (alarm_minute.high > 5) {
            alarm_minute.high = 0;
        }
        break;
    case 3:
        alarm_minute.low++;
        if (alarm_minute.low > 9) {
            alarm_minute.low = 0;
        }
        break;
    }
}

static void alarm_set()
{
    if (digit_index == 3) {
        alarm_minute.active = 1; // activate alarm only if last digit is selected
    }
    else {
        alarm_minute.active = 0;
    }

    // unlock real time clock
    rtc_register_t reg = {0};
    reg.protection.write = 0;
    rtc_write_register(reg, RTC_ADDR_WRITE_PROTECTION);

    rtc_write_ram_byte(*(uint8_t*)&alarm_minute, 0x00);
    rtc_write_ram_byte(*(uint8_t*)&alarm_hour, 0x01);

    // lock real time clock
    reg.protection.write = 1;
    rtc_write_register(reg, RTC_ADDR_WRITE_PROTECTION);
}

static bool alarm_reached()
{
    if (alarm_minute.active                                    &&
            (clock.hours.format_24.high == alarm_hour.high)    &&
            (clock.hours.format_24.low  == alarm_hour.low)     &&
            (clock.minutes.high         == alarm_minute.high)  &&
            (clock.minutes.low          == alarm_minute.low)) {
        return true;
    }

    return false;
}

static bool alarm_finished()
{
    button_event_t event = button_event();

    if ((event.button == BUTTON_1 || event.button == BUTTON_2) && event.state == button_released) {
        button_clear_event();
        return true;
    }

    return !alarm_reached();
}

static void alarm_on()
{
    buzzer_on();
}

static void alarm_off()
{
    alarm_minute.active = 0;
    buzzer_off();
}

static void time_init()
{
    digit_index = 0;
    set_clock = clock;
}

static void time_inc()
{
    switch (digit_index) {
    case 0:
        set_clock.hours.format_24.high++;
        if (set_clock.hours.format_24.high > 2) {
            set_clock.hours.format_24.high = 0;
        }
        if (set_clock.hours.format_24.high == 2 && set_clock.hours.format_24.low > 3) {
            set_clock.hours.format_24.high = 0;
        }
        break;
    case 1:
        set_clock.hours.format_24.low++;
        if (set_clock.hours.format_24.low > 9) {
            set_clock.hours.format_24.low = 0;
        }
        if (set_clock.hours.format_24.high == 2 && set_clock.hours.format_24.low > 3) {
            set_clock.hours.format_24.low = 0;
        }
        break;
    case 2:
        set_clock.minutes.high++;
        if (set_clock.minutes.high > 5) {
            set_clock.minutes.high = 0;
        }
        break;
    case 3:
        set_clock.minutes.low++;
        if (set_clock.minutes.low > 9) {
            set_clock.minutes.low = 0;
        }
        break;
    }
}

static void time_inc_digit()
{
    digit_index++;
    if (digit_index >= 4) {
        digit_index = 0;
    }
}

static void time_set()
{
    // unlock the clock and ram
    rtc_register_t reg;
    reg.protection.write = 0;
    reg.protection.reserved = 0;
    rtc_write_register(reg, RTC_ADDR_WRITE_PROTECTION);

    // set the time
    reg.protection.write = 1;
    set_clock.seconds.clock_halt = 0;
    set_clock.seconds.high = 0;
    set_clock.seconds.low = 0;
    rtc_write_register(*(rtc_register_t *)&set_clock.seconds, RTC_ADDR_SECONDS);
    rtc_write_register(*(rtc_register_t *)&set_clock.minutes, RTC_ADDR_MINUTES);
    rtc_write_register(*(rtc_register_t *)&set_clock.hours, RTC_ADDR_HOURS);
    rtc_write_register(reg, RTC_ADDR_WRITE_PROTECTION);

    // read back the clock
    rtc_read_clock(&clock);
}

static void date_init()
{
    digit_index = 0;
    set_clock = clock;
}

static void date_inc()
{
    switch (digit_index) {
    case 0:
        set_clock.month.high++;  // todo: limit better
        if (set_clock.month.high > 1) {
            set_clock.month.high = 0;
        }
        break;
    case 1:
        set_clock.month.low++;
        if (set_clock.month.low > 9) {
            set_clock.month.low = 0;
        }
        break;
    case 2:
        set_clock.date.high++;
        if (set_clock.date.high > 3) {
            set_clock.date.high = 0;
        }
        break;
    case 3:
        set_clock.date.low++;
        if (set_clock.date.low > 9) {
            set_clock.date.low = 0;
        }
        break;
    }
}

static void date_inc_digit()
{
    digit_index++;
    if (digit_index >= 4) {
        digit_index = 0;
    }
}

static void date_set()
{
    // unlock the clock and ram
    rtc_register_t reg;
    reg.protection.write = 0;
    reg.protection.reserved = 0;
    rtc_write_register(reg, RTC_ADDR_WRITE_PROTECTION);

    // set the date
    reg.protection.write = 1;
    rtc_write_register(*(rtc_register_t *)&set_clock.date, RTC_ADDR_DATE);
    rtc_write_register(*(rtc_register_t *)&set_clock.month, RTC_ADDR_MONTH);
    rtc_write_register(*(rtc_register_t *)&set_clock.year, RTC_ADDR_YEAR);
    rtc_write_register(reg, RTC_ADDR_WRITE_PROTECTION);

    // read back the clock
    rtc_read_clock(&clock);
}

static void year_init()
{
    digit_index = 0;
    set_clock = clock;
}

static void year_inc()
{
    switch (digit_index) {
    case 0:
        set_clock.year.high++;
        if (set_clock.year.high > 9) {
            set_clock.year.high = 0;
        }
        break;
    case 1:
        set_clock.year.low++;
        if (set_clock.year.low > 9) {
            set_clock.year.low = 0;
        }
        break;
    }
}

static void year_inc_digit()
{
    digit_index++;
    if (digit_index >= 2) {
        digit_index = 0;
    }
}

static void year_set()
{
    // unlock the clock and ram
    rtc_register_t reg;
    reg.protection.write = 0;
    reg.protection.reserved = 0;
    rtc_write_register(reg, RTC_ADDR_WRITE_PROTECTION);

    // set the year
    reg.protection.write = 1;
    rtc_write_register(*(rtc_register_t *)&set_clock.year, RTC_ADDR_YEAR);
    rtc_write_register(reg, RTC_ADDR_WRITE_PROTECTION);

    // read back the clock
    rtc_read_clock(&clock);
}

static const loop_state_transition_t loop_state_transitions[] = {
    { loop_time,          loop_date,          button_1_released,        0                 },
    { loop_date,          loop_year,          button_1_released,        0                 },
    { loop_year,          loop_thermistor,    button_1_released,        0                 },
    { loop_thermistor,    loop_time,          button_1_released,        0                 },
    { loop_time,          loop_set_alarm,     button_2_hold_released,   alarm_init        },
    { loop_set_alarm,     loop_time,          button_2_hold_released,   alarm_set         },
    { loop_set_alarm,     loop_set_alarm,     button_2_released,        alarm_inc_digit   },
    { loop_set_alarm,     loop_set_alarm,     button_1_released,        alarm_inc         },
    { loop_time,          loop_alarm,         alarm_reached,            alarm_on          },
    { loop_alarm,         loop_time,          alarm_finished,           alarm_off         },
    { loop_time,          loop_set_time,      button_1_hold_released,   time_init         },
    { loop_set_time,      loop_time,          button_1_hold_released,   time_set          },
    { loop_set_time,      loop_set_time,      button_1_released,        time_inc          },
    { loop_set_time,      loop_set_time,      button_2_released,        time_inc_digit    },
    { loop_date,          loop_set_date,      button_1_hold_released,   date_init         },
    { loop_set_date,      loop_date,          button_1_hold_released,   date_set          },
    { loop_set_date,      loop_set_date,      button_1_released,        date_inc          },
    { loop_set_date,      loop_set_date,      button_2_released,        date_inc_digit    },
    { loop_year,          loop_set_year,      button_1_hold_released,   year_init         },
    { loop_set_year,      loop_year,          button_1_hold_released,   year_set          },
    { loop_set_year,      loop_set_year,      button_1_released,        year_inc          },
    { loop_set_year,      loop_set_year,      button_2_released,        year_inc_digit    },
    { loop_time,          loop_version,       button_12_hold_released,  0                 },
    { loop_version,       loop_time,          button_1_released,        0                 },
    { loop_version,       loop_time,          button_2_released,        0                 }
};

// ---------- main task for the application triggered by timer interupt -------
static void main_task()
{
    static uint16_t main_index = 0;

    if (main_index == 0) {
        // read the clock for the first time
        rtc_read_clock(&clock);

        // read the alarm settings
        *(uint8_t *)&alarm_minute = rtc_read_ram_byte(0x00);
        *(uint8_t *)&alarm_hour = rtc_read_ram_byte(0x01);

        // read adc
        adc_start(ADC_CHANNEL_THERMISTOR);
        v_thermistor = adc_read();

        adc_start(ADC_CHANNEL_PHOTORESISTOR);
        v_photorezistor = adc_read();

        adc_start(ADC_CHANNEL_BUTTON);
        v_button = adc_read();

        adc_start(ADC_CHANNEL_THERMISTOR);

        loop_state = loop_time;
    }

    // buzzer pulse generation
    if (main_index == 125) {
        buzzer_run(true);
    }
    if (main_index == 130) {
        buzzer_run(false);
    }

    if (main_index % 500 == 250) {
        // led_toggle();
        rtc_read_clock(&clock);
        blink = !blink;
    }

    if (main_index % 100 == 13) {
        v_thermistor = adc_read();
        adc_start(ADC_CHANNEL_PHOTORESISTOR);
    }

    if (main_index % 100 == 43) {
        v_photorezistor = adc_read();
        adc_start(ADC_CHANNEL_BUTTON);
    }

    if (main_index % 100 == 67) {
        v_button = adc_read();
        adc_start(ADC_CHANNEL_THERMISTOR);
        button_measure(v_button);
    }

    if (main_index % 100 == 89) {
        for (uint8_t i = 0; i < (sizeof(loop_state_transitions) / sizeof(loop_state_transition_t)); i++) {
            if (loop_state_transitions[i].from == loop_state) {
                if (loop_state_transitions[i].pfn_go()) {
                    if (loop_state_transitions[i].pfn_do) {
                        loop_state_transitions[i].pfn_do();
                    }
                    loop_state = loop_state_transitions[i].to;
                    break;
                }
            }
        }
    }

    main_index++;
    if (main_index > 1000) {
        main_index = 1;
    }
}

void setup() 
{
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
}

void loop()
{
    switch (loop_state) {
    case loop_init:
        display_off();
        break;
    case loop_time:
        display_time(clock.hours.format_24.high, clock.hours.format_24.low, clock.minutes.high, clock.minutes.low, v_photorezistor);
        break;
    case loop_set_time:
        display_time(set_clock.hours.format_24.high, set_clock.hours.format_24.low, set_clock.minutes.high, set_clock.minutes.low, v_photorezistor, blink, digit_index, false);
        break;
    case loop_date:
        display_date(clock.month.high, clock.month.low, clock.date.high, clock.date.low, v_photorezistor);
        break;
    case loop_set_date:
        display_date(set_clock.month.high, set_clock.month.low, set_clock.date.high, set_clock.date.low, v_photorezistor, blink, digit_index);
        break;
    case loop_year:
        display_year(clock.year.high, clock.year.low, v_photorezistor);
        break;
    case loop_set_year:
        display_year(set_clock.year.high, set_clock.year.low, v_photorezistor, blink, digit_index);
        break;
    case loop_alarm:
        display_time(clock.hours.format_24.high, clock.hours.format_24.low, clock.minutes.high, clock.minutes.low, v_photorezistor, blink);
        break;
    case loop_set_alarm:
        display_time(alarm_hour.high, alarm_hour.low, alarm_minute.high, alarm_minute.low, v_photorezistor, blink, digit_index, alarm_minute.active);
        break;
    case loop_photoresistor:
        display_value(v_photorezistor, v_photorezistor, false);
        break;
    case loop_thermistor:
        display_temperature(ntc_temperature(v_thermistor), v_photorezistor);
        break;
    case loop_button:
        display_value(v_button, v_photorezistor, false);
        break;
    case loop_version:
        display_value(version, v_photorezistor, true);
        break;
    
    default:
        break;
    }
    delay(4);
}
