//*****************************************************************************
//
//! @file ssd1306_display.c
//!
//! @brief Example of the app running under FreeRTOS.
//!
//! This example implements the display controller driver for the ssd1306.
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2019, Cullen Logan
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

//*****************************************************************************
//
// This application has a large number of common include files. For
// convenience, we'll collect them all together in a single header and include
// that everywhere.
//
//*****************************************************************************
#include "freertos_tetris.h"
#include "gfx/gfx.h"

static uint32_t ssd1306_platform_write(void* iomHandle, uint8_t reg, uint8_t *bufp, uint16_t len, bool command);

void* iom_handle = NULL;

#define SSD1306_DISPLAY_MAX_XFER_SIZE 129
uint32_t i2cTX[SSD1306_DISPLAY_MAX_XFER_SIZE];
uint32_t i2cRX[SSD1306_DISPLAY_MAX_XFER_SIZE];

uint8_t framebuffer[sizeof(gfx_Bitmap_t)+SSD1306_FB_SIZE] = {0};
gfx_Bitmap_t* pFrameBuffer;


am_hal_iom_transfer_t iomCmdXferDefault =
{
    .uPeerInfo.ui32I2CDevAddr = AM_I2C_SSD1306_DISPLAY_ADDRESS_7_BIT,
    .ui32InstrLen = 0,     // 8-bit transfers
    .ui32Instr = 0,     // Not using slave offset address here
    .ui32NumBytes = 1,
    .eDirection = AM_HAL_IOM_TX,
    .pui32TxBuffer = i2cTX,
    .pui32RxBuffer = i2cRX,
    .bContinue = false,
    .ui8RepeatCount = 0,
    .ui8Priority = 1,
    .ui32PauseCondition = 0,
    .ui32StatusSetClr = 0
};


//only call affter init!
gfx_Bitmap_t* ssd1306_get_frame_buffer(void){
    return pFrameBuffer;
}

//*****************************************************************************
//
// Initialize the ssd1306 display module
//
//*****************************************************************************
uint32_t ssd1306_platform_init(uint32_t ui32IomModuleNum)
{
    int32_t retVal = AM_HAL_STATUS_SUCCESS;

    am_hal_gpio_pincfg_t const* pin_cfg_scl;
    am_hal_gpio_pincfg_t const* pin_cfg_sda;
    uint8_t pad_scl;
    uint8_t pad_sda;
    uint8_t init_success = pdFALSE;
    uint8_t wake_success = pdFALSE;

    am_hal_iom_config_t iom_i2c_config = {
        .eInterfaceMode = AM_HAL_IOM_I2C_MODE,
        .ui32ClockFreq = AM_HAL_IOM_100KHZ
    };

    retVal = am_hal_iom_initialize(ui32IomModuleNum, &iom_handle);
    if( AM_HAL_STATUS_SUCCESS != retVal )
    {
        am_util_debug_printf("ERROR! am_hal_iom_initialize returned: %d\r\n", retVal);
        goto iom_init_fail;
    }

    init_success = pdTRUE;

    retVal = am_hal_iom_power_ctrl(iom_handle, AM_HAL_SYSCTRL_WAKE, false);
    if( AM_HAL_STATUS_SUCCESS != retVal )
    {
        am_util_debug_printf("ERROR! am_hal_iom_power_ctrl returned: %d\r\n", retVal);
        goto iom_init_fail;
    }

    wake_success = pdTRUE;

    retVal = am_hal_iom_configure(iom_handle,&iom_i2c_config);
    if( AM_HAL_STATUS_SUCCESS != retVal )
    {
        am_util_debug_printf("ERROR! am_hal_iom_configure returned: %d\r\n", retVal);
        goto iom_init_fail;
    }

    retVal = am_hal_iom_enable(iom_handle);
    if( AM_HAL_STATUS_SUCCESS != retVal )
    {
        am_util_debug_printf("ERROR! am_hal_iom_enable returned: %d\r\n", retVal);
        goto iom_init_fail;
    }

    //we are initialized, powered, configured for i2c, and enabled
    //now we must setup out pinmux configuration

    switch(ui32IomModuleNum)
    {
    case 0:
        pad_scl = AM_BSP_GPIO_IOM0_SCL;
        pad_sda = AM_BSP_GPIO_IOM0_SDA;
        pin_cfg_scl = &g_AM_BSP_GPIO_IOM0_SCL;
        pin_cfg_sda = &g_AM_BSP_GPIO_IOM0_SDA;
        break;
    case 1:
        pad_scl = AM_BSP_GPIO_IOM1_SCL;
        pad_sda = AM_BSP_GPIO_IOM1_SDA;
        pin_cfg_scl = &g_AM_BSP_GPIO_IOM1_SCL;
        pin_cfg_sda = &g_AM_BSP_GPIO_IOM1_SDA;
        break;
    case 2:
        pad_scl = AM_BSP_GPIO_IOM2_SCL;
        pad_sda = AM_BSP_GPIO_IOM2_SDA;
        pin_cfg_scl = &g_AM_BSP_GPIO_IOM2_SCL;
        pin_cfg_sda = &g_AM_BSP_GPIO_IOM2_SDA;
        break;
    case 3:
        pad_scl = AM_BSP_GPIO_IOM3_SCL;
        pad_sda = AM_BSP_GPIO_IOM3_SDA;
        pin_cfg_scl = &g_AM_BSP_GPIO_IOM3_SCL;
        pin_cfg_sda = &g_AM_BSP_GPIO_IOM3_SDA;
        break;
    case 4:
        pad_scl = AM_BSP_GPIO_IOM4_SCL;
        pad_sda = AM_BSP_GPIO_IOM4_SDA;
        pin_cfg_scl = &g_AM_BSP_GPIO_IOM4_SCL;
        pin_cfg_sda = &g_AM_BSP_GPIO_IOM4_SDA;
        break;
    case 5:
        pad_scl = AM_BSP_GPIO_IOM5_SCL;
        pad_sda = AM_BSP_GPIO_IOM5_SDA;
        pin_cfg_scl = &g_AM_BSP_GPIO_IOM5_SCL;
        pin_cfg_sda = &g_AM_BSP_GPIO_IOM5_SDA;
        break;
    default:
        // should not occur as am_hal_iom_initialize should return this
        // error if ui32IomModuleNum >= AM_REG_IOM_NUM_MODULES
        am_util_debug_printf("ERROR! ui32IomModuleNum AM_HAL_STATUS_OUT_OF_RANGE: %d\r\n", ui32IomModuleNum);
        retVal = AM_HAL_STATUS_OUT_OF_RANGE;
        goto iom_init_fail;
        break;
    }

    // If we arrive here then:
    // 1. Our Pad variables are set properly
    // 2. Our Mux Function Selections are proper
    // 3. We have pointers to BSP versions of pin configurations
    retVal = am_hal_gpio_pinconfig(pad_scl, *pin_cfg_scl);
    if( AM_HAL_STATUS_SUCCESS != retVal )
    {
        am_util_debug_printf("ERROR! Failed to configure %d pad for SCL: %d\r\n", retVal);
        goto iom_init_fail;
    }

    retVal = am_hal_gpio_pinconfig(pad_sda, *pin_cfg_sda);
    if( AM_HAL_STATUS_SUCCESS != retVal )
    {
        am_util_debug_printf("ERROR! Failed to configure %d pad for SDA: %d\r\n", retVal);
        goto iom_init_fail;
    }

    goto iom_init_done;

iom_init_fail:

    if ( pdTRUE == wake_success )
    {
        //put module into normal sleep
        am_hal_iom_power_ctrl(iom_handle, AM_HAL_SYSCTRL_NORMALSLEEP, pdFALSE);
    }

    if( pdTRUE == init_success )
    {
        //Uninitialize IOM Module from handle
        am_hal_iom_uninitialize(iom_handle);
    }

iom_init_done:
    if( AM_HAL_STATUS_SUCCESS == retVal )
    {
        am_util_debug_printf("SUCCESS! Setup IOM %d for Display\r\n", ui32IomModuleNum);
        am_util_debug_printf("Initializing Display Controller\r\n");
        retVal = ssd1306_display_init(iom_handle);
    }

    return retVal;
}

uint32_t ssd1306_display_init(void* iomHandle)
{

    uint32_t retVal = 0;
    uint32_t len = 0;
    uint8_t *initPtr = NULL;
    uint8_t displayInitSeq[] = {
/*1*/    SSD1306_DISPLAYOFF,            //-- Display OFF
/*2*/    SSD1306_SETMULTIPLEX,          //-- Double Byte Command - Set Multiplex Ratio
            SSD1306_INIT_MUX_RATIO,     //---- Multiplex Ratio %**xxxxxx X[0x3F-0x0F] 0-14, are invalid values
/*2*/    SSD1306_SETDISPLAYOFFSET,      //-- Double Byte Command - Set vertical shift by COM from 0d~63d
            SSD1306_INIT_VERT_SHIFT,    //---- Vertical Shift [0x00-0x3F]
/*1*/    SSD1306_SETLOWCOLUMN,          //-- Set the lower nibble of the column start address register for Page Addressing Mode
/*1*/    SSD1306_SETHIGHCOLUMN,         //-- Set the higher nibble of the column start address register for Page Addressing Mode
/*1*/    SSD1306_SETSTARTLINE,          //-- Set display RAM display start line register from 0-63 using %01xxxxxx [0x40-0x7F]
/*1*/    SSD1306_SETPAGESTART_0,        //-- Set GDDRAM Page Start Address (PAGE0~PAGE7) for Page Addressing Mode %10110xxx [0xB0-0xB7]
/*1*/    SSD1306_SEGREMAP_COL127_SEG0,  //-- Set Segment Re-map %1010000x x=0:col0 mapped to SEG0, x=1:col127 mapped to SEG0
/*1*/    SSD1306_COM_SCAN_DEC,          //-- Set COM Output    Scan Direction 0xC0: Scan from COM0 to COM[N –1], 0xC8: Scan from COM[N-1] to COM0
/*2*/    SSD1306_SETCOMPINS,            //-- Double Byte Command - Set COM Pins Hardware Configuration
            SSD1306_INIT_COMPINS,       //---- [0x02-0x32] Bits 4 and 5 control pin (re)mapping
/*2*/    SSD1306_SETCONTRAST,           //-- Double byte command to select 1 out of 256    contrast steps
            SSD1306_INIT_CONTRAST,      //---- Contrast Between [0x00-0xFF]
/*1*/    SSD1306_NORMALDISPLAY,         //-- Set Inverse Display Mode A6: Normal, A7:Inverse
/*2*/    SSD1306_SETDISPLAYCLOCKDIV,    //-- Double Byte Command: Set Oscillator Freq. (Clock Divide)
            SSD1306_INIT_CLOCK_DIV,     //---- Osc. Freq + Divide Ratio X[7:4]: Osc. Freq., X[3:0]: Divide Ratio (X[3:0]+1)
/*2*/    SSD1306_CHARGEPUMP,            //-- Double perhaps Triple Byte Command - Charge Pump Setting
            SSD1306_INIT_CHARGEPUMP_ON, //---- Charge Pump ON
/*2*/    SSD1306_SETPRECHARGE,          //-- Double Byte Command - Set Pre-charge Period
            SSD1306_INIT_PRECHARGE,     //---- Precharge period X[7:4] Phase 2, X[3:0] Phase 1 - 0 is invalid for either bitfield
/*2*/    SSD1306_SETVCOMDESEL,          //-- Double Byte Command - Set VCOMH Deselect Level
            SSD1306_INIT_VCOMDESEL,     //---- VCOMH Deselect Level %0xxx0000 - X[6:4]: 000- .65xVCC, 010- .77xVCC, 011- .83xVCC
/*1*/    SSD1306_DISPLAYON,             //------ Display ON (required to set charge pump)
/*1*/    SSD1306_DISPLAYON              //-- Display ON
    };

    GFX_INIT_STATIC_BITMAP(pFrameBuffer, framebuffer, BITMAP_TYPE_1BPP_VERTICAL,SSD1306_LCD_WIDTH, SSD1306_LCD_HEIGHT);

    initPtr = displayInitSeq;

    len = 1;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 2;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 2;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 1;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 1;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 1;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 1;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 1;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 1;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 2;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 2;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 1;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 2;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 2;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 2;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 2;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    len = 1;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);
    initPtr += len;

    //SSD1306_DISPLAYON
    len = 1;
    retVal = ssd1306_platform_write(iomHandle, SSD1306_CTRL_BYTE_COMMAND, initPtr, len, true);

    return retVal;
}

static uint32_t ssd1306_platform_write(void* iomHandle, uint8_t reg, uint8_t *bufp, uint16_t len, bool command)
{
    uint32_t ui32Status = AM_HAL_STATUS_SUCCESS;
    uint8_t* dstPtr = (uint8_t *)i2cTX;
    am_hal_iom_transfer_t iomXfer = iomCmdXferDefault;

    if( len >= SSD1306_DISPLAY_MAX_XFER_SIZE ) {
        //len must be smaller than SSD1306_DISPLAY_MAX_XFER_SIZE since we add one control byte
        am_util_stdio_printf("ERROR!!! Transfer length too long\r\n");
        return 1;    //Error Length too great
    }
    if( NULL == bufp ){
        am_util_stdio_printf("ERROR!!! No TX buffer provided\r\n");
        return 1; // Error no data to transmit
    }

    //we must make a command with following I2C data
    //Slave Addr + 0x80 (cmd) + CMD + len of command (len-1)
    iomXfer.ui32InstrLen = 0;
    iomXfer.ui32Instr = 0;                // no sub-address
    iomXfer.ui32NumBytes = len + 1;        // Add 1 for the Control Byte
    iomXfer.uPeerInfo.ui32I2CDevAddr = AM_I2C_SSD1306_DISPLAY_ADDRESS_7_BIT;
    iomXfer.eDirection = AM_HAL_IOM_TX;
    iomXfer.pui32TxBuffer = i2cTX;
    iomXfer.pui32RxBuffer = i2cRX;
    iomXfer.bContinue = false;          // Release the bus after this transfer
    iomXfer.ui8RepeatCount = 0;         // ?
    iomXfer.ui8Priority = 1;            // ?
    iomXfer.ui32PauseCondition = 0;     // ?
    iomXfer.ui32StatusSetClr = 0;       // ?

    if ( SSD1306_CTRL_BYTE_COMMAND == reg )
    {
        *dstPtr = SSD1306_CTRL_BYTE_COMMAND;
    }
    else
    {
        *dstPtr = SSD1306_CTRL_BYTE_DATA;
    }

    dstPtr++;    //Increment the destination pointer

    //for length of 2 the below will be:
    //*(dstPtr + 1) = *(bufp + 1)
    //*(dstPtr + 0) = *(bufp + 0)
    while(len--)
    {   // Copy data to the i2c tx buffer
        *(dstPtr + len) = *(bufp + len);
    }

    ui32Status = am_hal_iom_blocking_transfer(iomHandle, &iomXfer);

    if ( AM_HAL_STATUS_SUCCESS != ui32Status ){
        am_util_stdio_printf("ERROR!!! iom_blocking_transfer: %u\r\n", ui32Status);
    }

    return ui32Status;
}

uint32_t ssd1306_clear_screen(bool updateDisplay)
{
    uint32_t retVal = 0;
    memset(pFrameBuffer->pData, 0, SSD1306_FB_SIZE);
    if( pdTRUE == updateDisplay )
    {
        ssd1306_show_frame(pFrameBuffer);
    }

    return retVal;
}

//Source bitmap must be exactly the size of one frame.
//It will not be bit-blitted
uint32_t ssd1306_show_frame(gfx_Bitmap_t* srcBitmap)
{
    uint32_t retVal = 0;
    uint16_t len;
    uint8_t m;
    uint8_t* pCmd;
    uint8_t pageStartCmd[] = {
            SSD1306_SETPAGESTART_0,
            SSD1306_SETLOWCOLUMN,
            SSD1306_SETHIGHCOLUMN
    };
    
    // If source is same as framebuffer do not copy
    if( srcBitmap->pData != pFrameBuffer->pData ){
        memcpy(pFrameBuffer->pData,srcBitmap->pData, ((srcBitmap->uHeight+7)/8) * srcBitmap->uWidth);
    }

    for(m=0;m<8;m++)
    {
        pCmd = pageStartCmd;
        pageStartCmd[0] = SSD1306_SETPAGESTART_0 + m;
        //select the page we want to address
        len = 1;
        retVal = ssd1306_platform_write(iom_handle, SSD1306_CTRL_BYTE_COMMAND, pCmd, len, true);
        pCmd += len;
        //low column start address
        len = 1;
        retVal = ssd1306_platform_write(iom_handle, SSD1306_CTRL_BYTE_COMMAND, pCmd, len, true);
        pCmd += len;
        //high column start address
        len = 1;
        retVal = ssd1306_platform_write(iom_handle, SSD1306_CTRL_BYTE_COMMAND, pCmd, len, true);
        pCmd += len;

        len = SSD1306_LCD_WIDTH;
        retVal = ssd1306_platform_write(iom_handle, SSD1306_CTRL_BYTE_DATA, pFrameBuffer->pData+(m*SSD1306_LCD_WIDTH), len, false);
    }

    return retVal;
}

uint32_t ssd1306_invert_display(bool invert)
{
    uint32_t retVal = 0;
    uint16_t len;
    uint8_t invertCmd = invert ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY;

    len = 1;
    retVal = ssd1306_platform_write(iom_handle, SSD1306_CTRL_BYTE_COMMAND, &invertCmd, len, true);
    return retVal;
}

/*void rotateDisplay(bool rot)
{
    if (rot)
    {
        _sendTWIcommand(SSD1306_SEGREMAP_COL0_SEG0);
        _sendTWIcommand(SSD1306_COM_SCAN_INC);
    }
    else
    {
        _sendTWIcommand(SSD1306_SEGREMAP_COL127_SEG0);
        _sendTWIcommand(SSD1306_COM_SCAN_DEC);
    }
}*/
