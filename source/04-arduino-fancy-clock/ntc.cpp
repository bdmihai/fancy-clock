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
#include "ntc.h"

// generated with https://www.sebulli.com/ntc/index.php

static const int16_t ntc_table[65] = {
  -62, -52, -42, -36, -31, -27, -23, -20, -17, 
  -15, -12, -10, -8, -6, -4, -2, -1, 1, 3, 
  5, 6, 8, 9, 11, 13, 14, 16, 17, 19, 20, 22, 
  24, 25, 27, 28, 30, 32, 33, 35, 37, 39, 41, 
  43, 45, 47, 49, 51, 54, 56, 59, 62, 65, 68, 
  72, 76, 80, 85, 91, 97, 106, 116, 130, 151, 
  193, 235
};

int16_t ntc_temperature(uint16_t adc_value)
{
  int16_t p1,p2;

  /* Estimate the interpolating point before and after the ADC value. */
  p1 = ntc_table[ (adc_value >> 4)  ];
  p2 = ntc_table[ (adc_value >> 4)+1];
 
  /* Interpolate between both points. */
  return p1 + ( (p2-p1) * (adc_value & 0x000F) ) / 16;
};