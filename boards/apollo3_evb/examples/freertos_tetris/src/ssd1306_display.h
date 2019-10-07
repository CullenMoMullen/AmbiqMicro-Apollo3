//*****************************************************************************
//
//! @file ssd1306_display.h
//!
//! @brief Global includes for the freertos_tetris app.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2019, Ambiq Micro
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
// 
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision v2.2.0-7-g63f7c2ba1 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef SSD1306_DISPLAY_H
#define SSD1306_DISPLAY_H

#include "gfx/gfx.h"


#define AM_I2C_SSD1306_DISPLAY_ADDRESS_8_BIT (0x78)    //R/W bit is included but zero
#define AM_I2C_SSD1306_DISPLAY_ADDRESS_7_BIT (0x3C)    //R/W bit not included, indicated by xfer direction

#define SSD1306_LCD_WIDTH                    (128)
#define SSD1306_LCD_HEIGHT                   (64)

#define SSD1306_FB_SIZE                      (SSD1306_LCD_WIDTH * SSD1306_LCD_HEIGHT / 8)
#define SSD1306_MAXROW                       (8)
#define SSD1306_INIT_CONTRAST                (0xCF)
#define SSD1306_INIT_MUX_RATIO               (0x3F)
#define SSD1306_INIT_VERT_SHIFT              (0x00)
#define SSD1306_INIT_CLOCK_DIV               (0x80)
#define SSD1306_INIT_PRECHARGE               (0xF1)
#define SSD1306_INIT_COMPINS                 (0x12)
#define SSD1306_INIT_VCOMDESEL               (0x40)
#define SSD1306_INIT_CHARGEPUMP_ON           (0x14)

#define SSD1306_CTRL_BYTE_COMMAND            (0x00)
#define SSD1306_CTRL_BYTE_DATA               (0x40)

/*
 * LCD Commands
 */
#define SSD1306_SETCONTRAST                  (0x81)
#define SSD1306_DISPLAYALLON_RESUME          (0xA4)
#define SSD1306_DISPLAYALLON                 (0xA5)
#define SSD1306_NORMALDISPLAY                (0xA6)
#define SSD1306_INVERTDISPLAY                (0xA7)
#define SSD1306_DISPLAYOFF                   (0xAE)
#define SSD1306_DISPLAYON                    (0xAF)

#define SSD1306_CONT_HORIZ_SCROLL_RT         (0x26)
#define SSD1306_CONT_HORIZ_SCROLL_LF         (0x27)

#define SSD1306_SETDISPLAYOFFSET             (0xD3)
#define SSD1306_SETCOMPINS                   (0xDA)
#define SSD1306_SETVCOMDESEL                 (0xDB)
#define SSD1306_SETDISPLAYCLOCKDIV           (0xD5)
#define SSD1306_SETPRECHARGE                 (0xD9)
#define SSD1306_SETMULTIPLEX                 (0xA8)
#define SSD1306_SETLOWCOLUMN                 (0x00)
#define SSD1306_SETHIGHCOLUMN                (0x10)
#define SSD1306_SETSTARTLINE                 (0x40)
#define SSD1306_MEMORYMODE                   (0x20)
#define SSD1306_COLUMNADDR                   (0x21)
#define SSD1306_PAGEADDR                     (0x22)
#define SSD1306_SETPAGESTART_0               (0xB0)
#define SSD1306_SETPAGESTART_1               (0xB1)
#define SSD1306_SETPAGESTART_2               (0xB2)
#define SSD1306_SETPAGESTART_3               (0xB3)
#define SSD1306_SETPAGESTART_4               (0xB4)
#define SSD1306_SETPAGESTART_5               (0xB5)
#define SSD1306_SETPAGESTART_6               (0xB6)
#define SSD1306_SETPAGESTART_7               (0xB7)
#define SSD1306_COM_SCAN_INC                 (0xC0)
#define SSD1306_COM_SCAN_DEC                 (0xC8)
#define SSD1306_SEGREMAP_COL0_SEG0           (0xA0)
#define SSD1306_SEGREMAP_COL127_SEG0         (0xA1)
#define SSD1306_CHARGEPUMP                   (0x8D)
#define SSD1306_EXTERNALVCC                  (0x01)
#define SSD1306_SWITCHCAPVCC                 (0x02)

uint32_t ssd1306_platform_init(uint32_t ui32Module);
uint32_t ssd1306_display_init(void* iomHandle);
uint32_t ssd1306_clear_screen(bool updateDisplay);
uint32_t ssd1306_show_frame(gfx_Bitmap_t* srcBitmap);
uint32_t ssd1306_invert_display(bool invert);
gfx_Bitmap_t* ssd1306_get_frame_buffer(void);

#endif // SSD1306_DISPLAY_H


