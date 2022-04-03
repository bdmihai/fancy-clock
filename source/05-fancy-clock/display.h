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

#pragma once

void display_off();

void display_time(uint8_t hour_high, uint8_t hour_low, uint8_t minute_high, uint8_t minute_low, const uint16_t luminosity);
void display_time(uint8_t hour_high, uint8_t hour_low, uint8_t minute_high, uint8_t minute_low, const uint16_t luminosity, bool blink);
void display_time(uint8_t hour_high, uint8_t hour_low, uint8_t minute_high, uint8_t minute_low, const uint16_t luminosity, bool blink, uint8_t digit, bool enabled);
void display_date(uint8_t month_high, uint8_t month_low, uint8_t date_high, uint8_t date_low, const uint16_t luminosity);
void display_date(uint8_t month_high, uint8_t month_low, uint8_t date_high, uint8_t date_low, const uint16_t luminosity, bool blink, uint8_t digit);
void display_year(uint8_t year_high, uint8_t year_low, const uint16_t luminosity);
void display_year(uint8_t year_high, uint8_t year_low, const uint16_t luminosity, bool blink, uint8_t digit);
void display_value(uint16_t value, const uint16_t luminosity, bool with_dot);
void display_temperature(int16_t value, const uint16_t luminosity);

