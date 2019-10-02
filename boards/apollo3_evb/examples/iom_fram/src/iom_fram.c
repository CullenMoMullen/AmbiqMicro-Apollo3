//*****************************************************************************
//
//! @file iom_fram.c
//!
//! @brief Example that demonstrates IOM, connecting to a SPI or I2C FRAM
//! Purpose: FRAM is initialized with a known pattern data using Blocking IOM Write.
//! This example starts a 1 second timer. At each 1 second period, it initiates
//! reading a fixed size block from the FRAM device using Non-Blocking IOM
//! Read, and comparing against the predefined pattern
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! Additional Information:
//! Define one of FRAM_DEVICE_ macros to select the FRAM device
//!
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

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#if FIREBALL_CARD
//
// The Fireball device card multiplexes various devices including each of an SPI
// and I2C FRAM. The Fireball device driver controls access to these devices.
// If the Fireball card is not used, FRAM devices can be connected directly
// to appropriate GPIO pins.
//
#include "am_devices_fireball.h"
#endif

//*****************************************************************************
// Customize the following for the test
//*****************************************************************************
// Control the example execution
#define FRAM_IOM_MODULE         0
#define PATTERN_BUF_SIZE        128
#define NUM_INERATIONS          16

// Select the FRAM Device
#define FRAM_DEVICE_MB85RS1MT     1     // SPI Fram (Fireball)
//#define FRAM_DEVICE_MB85RC64TA    1     // I2C Fram (Fireball)
//#define FRAM_DEVICE_MB85RS64V     1     // SPI Fram
//#define FRAM_DEVICE_MB85RC256V    1     // I2C Fram
//*****************************************************************************
//*****************************************************************************

#if (FRAM_DEVICE_MB85RS1MT == 1)
#include "am_devices_mb85rs1mt.h"
#define FRAM_DEVICE_ID          AM_DEVICES_MB85RS1MT_ID
#define FRAM_IOM_MODE           AM_HAL_IOM_SPI_MODE
#elif (FRAM_DEVICE_MB85RC256V == 1)
#include "am_devices_mb85rc256v.h"
#define FRAM_DEVICE_ID          AM_DEVICES_MB85RC256V_ID
#define FRAM_IOM_MODE           AM_HAL_IOM_I2C_MODE
#elif (FRAM_DEVICE_MB85RS64V == 1)
#include "am_devices_mb85rs64v.h"
#define FRAM_DEVICE_ID          AM_DEVICES_MB85RS64V_ID
#define FRAM_IOM_MODE           AM_HAL_IOM_SPI_MODE
#elif (FRAM_DEVICE_MB85RC64TA == 1)
#include "am_devices_mb85rc256v.h"
#define FRAM_DEVICE_ID          AM_DEVICES_MB85RC64TA_ID
#define FRAM_IOM_MODE           AM_HAL_IOM_I2C_MODE
#else
#error "Unknown FRAM Device"
#endif

#define IOM_INTERRUPT1(n)       AM_HAL_INTERRUPT_IOMASTER ## n
#define IOM_INTERRUPT(n)        IOM_INTERRUPT1(n)
#define FRAM_IOM_IRQn           ((IRQn_Type)(IOMSTR0_IRQn + FRAM_IOM_MODULE))

//
// Typedef - to encapsulate device driver functions
//
typedef struct
{
    uint8_t  devName[20];
    uint32_t (*fram_init)(uint32_t ui32Module, am_hal_iom_config_t *psIOMSettings, void **ppIomHandle);
    uint32_t (*fram_term)(uint32_t ui32Module);

    uint32_t (*fram_read_id)(uint32_t *pDeviceID);

    uint32_t (*fram_blocking_write)(uint8_t *ui8TxBuffer,
                             uint32_t ui32WriteAddress,
                             uint32_t ui32NumBytes);

    uint32_t (*fram_nonblocking_write)(uint8_t *ui8TxBuffer,
                                uint32_t ui32WriteAddress,
                                uint32_t ui32NumBytes,
                                am_hal_iom_callback_t pfnCallback,
                                void *pCallbackCtxt);

    uint32_t (*fram_blocking_read)(uint8_t *pui8RxBuffer,
                            uint32_t ui32ReadAddress,
                            uint32_t ui32NumBytes);

    uint32_t (*fram_nonblocking_read)(uint8_t *pui8RxBuffer,
                                                      uint32_t ui32ReadAddress,
                                                      uint32_t ui32NumBytes,
                                                      am_hal_iom_callback_t pfnCallback,
                                                      void *pCallbackCtxt);
    am_devices_fireball_control_e fram_fireball_control;
} fram_device_func_t;

//*****************************************************************************
//
// Global Variables
//
//*****************************************************************************
volatile bool                           g_bReadFram = false;
volatile bool                           g_bVerifyReadData = false;
void                                    *g_pIOMHandle;
am_hal_iom_buffer(PATTERN_BUF_SIZE)     gPatternBuf;
am_hal_iom_buffer(PATTERN_BUF_SIZE)     gRxBuf;

#if FIREBALL_CARD
// Determine the control value for I2C based on FRAM_IOM_MODULE
#define AM_DEVICES_FIREBALL_STATE_I2C_FRAM                          \
    AM_DEVICES_FIREBALL_STATE_I2C_FRAM1(FRAM_IOM_MODULE)
#define AM_DEVICES_FIREBALL_STATE_I2C_FRAM1(n)                      \
    AM_DEVICES_FIREBALL_STATE_I2C_FRAM2(n)
#define AM_DEVICES_FIREBALL_STATE_I2C_FRAM2(n)                      \
    AM_DEVICES_FIREBALL_STATE_I2C_IOM ## n
#endif

// Buffer for non-blocking transactions
uint32_t                                DMATCBBuffer[256];
fram_device_func_t device_func =
{
#if (FRAM_DEVICE_MB85RS1MT == 1)
    // Fireball installed SPI FRAM device
    .devName = "SPI FRAM MB85RS1MT",
    .fram_init = am_devices_mb85rs1mt_init,
    .fram_term = am_devices_mb85rs1mt_term,
    .fram_read_id = am_devices_mb85rs1mt_read_id,
    .fram_blocking_write = am_devices_mb85rs1mt_blocking_write,
    .fram_nonblocking_write = am_devices_mb85rs1mt_nonblocking_write,
    .fram_blocking_read = am_devices_mb85rs1mt_blocking_read,
    .fram_nonblocking_read = am_devices_mb85rs1mt_nonblocking_read,
#if FIREBALL_CARD
    .fram_fireball_control = AM_DEVICES_FIREBALL_STATE_SPI_FRAM,
#else
    .fram_fireball_control = 0,
#endif
#elif (FRAM_DEVICE_MB85RC256V == 1)
    .devName = "I2C FRAM MB85RC256V",
    .fram_init = am_devices_mb85rc256v_init,
    .fram_term = am_devices_mb85rc256v_term,
    .fram_read_id = am_devices_mb85rc256v_read_id,
    .fram_blocking_write = am_devices_mb85rc256v_blocking_write,
    .fram_nonblocking_write = am_devices_mb85rc256v_nonblocking_write,
    .fram_blocking_read = am_devices_mb85rc256v_blocking_read,
    .fram_nonblocking_read = am_devices_mb85rc256v_nonblocking_read,
    .fram_fireball_control = 0,
#elif (FRAM_DEVICE_MB85RS64V == 1)
    .devName = "SPI FRAM MB85RS64V",
    .fram_init = am_devices_mb85rs64v_init,
    .fram_term = am_devices_mb85rs64v_term,
    .fram_read_id = am_devices_mb85rs64v_read_id,
    .fram_blocking_write = am_devices_mb85rs64v_blocking_write,
    .fram_nonblocking_write = am_devices_mb85rs64v_nonblocking_write,
    .fram_blocking_read = am_devices_mb85rs64v_blocking_read,
    .fram_nonblocking_read = ,
    .fram_fireball_control = 0,
#elif (FRAM_DEVICE_MB85RC64TA == 1)
    // Fireball installed I2C FRAM device
    .devName = "I2C FRAM MB85RC64TA",
    .fram_init = am_devices_mb85rc256v_init,
    .fram_term = am_devices_mb85rc256v_term,
    .fram_read_id = am_devices_mb85rc256v_read_id,
    .fram_blocking_write = am_devices_mb85rc256v_blocking_write,
    .fram_nonblocking_write = am_devices_mb85rc256v_nonblocking_write,
    .fram_blocking_read = am_devices_mb85rc256v_blocking_read,
    .fram_nonblocking_read = am_devices_mb85rc256v_nonblocking_read,
#if FIREBALL_CARD
    .fram_fireball_control = AM_DEVICES_FIREBALL_STATE_I2C_FRAM,
#else
    .fram_fireball_control = 0,
#endif
#else
#error "Unknown FRAM Device"
#endif
};

//*****************************************************************************
//
// Timer configuration.
//
//*****************************************************************************
am_hal_ctimer_config_t g_sTimer0 =
{
    // Don't link timers.
    0,

    // Set up Timer0A.
    (AM_HAL_CTIMER_FN_REPEAT |
     AM_HAL_CTIMER_INT_ENABLE    |
     AM_HAL_CTIMER_LFRC_32HZ),

    // No configuration for Timer0B.
    0,
};

am_hal_iom_config_t     g_sIomCfg =
{
    .eInterfaceMode       = FRAM_IOM_MODE,
    .ui32ClockFreq        = AM_HAL_IOM_1MHZ,
};

//*****************************************************************************
//
// Function to initialize Timer A0 to interrupt every 1 second.
//
//*****************************************************************************
void
timerA0_init(void)
{
    uint32_t ui32Period;

    //
    // Enable the LFRC.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_LFRC_START, 0);

    //
    // Set up timer A0.
    //
    am_hal_ctimer_clear(0, AM_HAL_CTIMER_TIMERA);
    am_hal_ctimer_config(0, &g_sTimer0);

    //
    // Set up timerA0 to 32Hz from LFRC divided to 1 second period.
    //
    ui32Period = 32;
    am_hal_ctimer_period_set(0, AM_HAL_CTIMER_TIMERA, ui32Period,
                             (ui32Period >> 1));

    //
    // Clear the timer Interrupt
    //
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);
}

//*****************************************************************************
//
// Timer Interrupt Service Routine (ISR)
//
//*****************************************************************************
void
am_ctimer_isr(void)
{
    uint32_t ui32Status;

    ui32Status = am_hal_ctimer_int_status_get(true);

    if ( ui32Status )
    {
        am_hal_ctimer_int_clear(ui32Status);
        am_hal_ctimer_int_service(ui32Status);
    }
}

//
// Take over the interrupt handler for whichever IOM we're using.
//
#define fram_iom_isr                                                          \
    am_iom_isr1(FRAM_IOM_MODULE)
#define am_iom_isr1(n)                                                        \
    am_iom_isr(n)
#define am_iom_isr(n)                                                         \
    am_iomaster ## n ## _isr

//*****************************************************************************
//
// IOM ISRs.
//
//*****************************************************************************
//
//! Take over default ISR. (Queue mode service)
//
void fram_iom_isr(void)
{
    uint32_t ui32Status;

    if (!am_hal_iom_interrupt_status_get(g_pIOMHandle, true, &ui32Status))
    {
        if ( ui32Status )
        {
            am_hal_iom_interrupt_clear(g_pIOMHandle, ui32Status);
            am_hal_iom_interrupt_service(g_pIOMHandle, ui32Status);
        }
    }
}

void
init_pattern(void)
{
    uint32_t i;
    for (i = 0; i < PATTERN_BUF_SIZE; i++)
    {
        gPatternBuf.bytes[i] = i & 0xFF;
    }
}

int
fram_init(void)
{
    uint32_t ui32Status;
    uint32_t ui32DeviceId;

    // Set up IOM
    // Initialize the Device
    g_sIomCfg.ui32NBTxnBufLength = sizeof(DMATCBBuffer) / 4;
    g_sIomCfg.pNBTxnBuf = &DMATCBBuffer[0];

#if FIREBALL_CARD
    uint32_t ui32Ret, ui32ID;

#if 1
    //
    // Get Fireball ID and Rev info.
    //
    ui32Ret = am_devices_fireball_control(AM_DEVICES_FIREBALL_STATE_ID_GET, &ui32ID);
    if ( ui32Ret != 0 )
    {
        am_util_stdio_printf("FAIL: am_devices_fireball_control(%d) returned 0x%X.\n",
                             AM_DEVICES_FIREBALL_STATE_ID_GET, ui32Ret);
        return -1;
    }
    else if ( ui32ID == FIREBALL_ID )
    {
        am_util_stdio_printf("Fireball found, ID is 0x%X.\n", ui32ID);
    }
    else
    {
        am_util_stdio_printf("Unknown device returned ID as 0x%X.\n", ui32ID);
    }

    ui32Ret = am_devices_fireball_control(AM_DEVICES_FIREBALL_STATE_VER_GET, &ui32ID);
    if ( ui32Ret != 0 )
    {
        am_util_stdio_printf("FAIL: am_devices_fireball_control(%d) returned 0x%X.\n",
                             AM_DEVICES_FIREBALL_STATE_VER_GET, ui32Ret);
        return -1;
    }
    else
    {
        am_util_stdio_printf("Fireball Version is 0x%X.\n", ui32ID);
    }
#endif

    if ( device_func.fram_fireball_control != 0 )
    {
        ui32Ret = am_devices_fireball_control(device_func.fram_fireball_control, 0);
        if ( ui32Ret != 0 )
        {
            am_util_stdio_printf("FAIL: am_devices_fireball_control(%d) returned 0x%X.\n",
                                 device_func.fram_fireball_control, ui32Ret);
            return -1;
        }
    }
#endif // FIREBALL_CARD

    ui32Status = device_func.fram_init(FRAM_IOM_MODULE, &g_sIomCfg, &g_pIOMHandle);
    if (0 == ui32Status)
    {
        ui32Status = device_func.fram_read_id(&ui32DeviceId);

        if ((ui32Status  != 0) || (ui32DeviceId != FRAM_DEVICE_ID))
        {
            return -1;
        }
        am_util_stdio_printf("%s Found\n", device_func.devName);
        // Set up a pattern data in FRAM memory
        am_util_stdio_printf("Setting up data pattern in FRAM using blocking write\n");
        return device_func.fram_blocking_write(&gPatternBuf.bytes[0], 0, PATTERN_BUF_SIZE);
    }
    else
    {
        return -1;
    }
}

void
read_complete(void *pCallbackCtxt, uint32_t transactionStatus)
{
    if (transactionStatus != AM_HAL_STATUS_SUCCESS)
    {
        am_util_stdio_printf("\nFRAM Read Failed 0x%x\n", transactionStatus);
    }
    else
    {
        am_util_stdio_printf(".");
        g_bVerifyReadData = true;
    }
}

void
initiate_fram_read(void)
{
    g_bReadFram = true;
}

int
verify_fram_data(void)
{
    uint32_t i;
    g_bVerifyReadData = false;
    // Verify Read FRAM data
    for (i = 0; i < PATTERN_BUF_SIZE; i++)
    {
        if (gPatternBuf.bytes[i] != gRxBuf.bytes[i])
        {
            am_util_stdio_printf("Receive Data Compare failed at offset %d - Expected = 0x%x, Received = 0x%x\n",
                i, gPatternBuf.bytes[i], gRxBuf.bytes[i]);
            return -1;
        }
    }
    return 0;
}

void
read_fram(void)
{
    uint32_t ui32Status;
    // Initiate read of a block of data from FRAM
    ui32Status = device_func.fram_nonblocking_read(&gRxBuf.bytes[0], 0, PATTERN_BUF_SIZE, read_complete, 0);
    if (ui32Status == 0)
    {
        g_bReadFram = false;
    }
}

void
fram_term(void)
{
    device_func.fram_term(FRAM_IOM_MODULE);
}

//*****************************************************************************
//
// Main function.
//
//*****************************************************************************
int
main(void)
{
    int iRet;
    uint32_t numRead = 0;
    //
    // Set the clock frequency.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    //
    // Configure the board for low power operation.
    //
    am_bsp_low_power_init();

    //
    // Enable the ITM print interface.
    //
    am_bsp_itm_printf_enable();

    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("IOM FRAM Example\n");

#if 0
    am_hal_gpio_out_bit_clear(42);
    am_hal_gpio_pin_config(42, AM_HAL_PIN_OUTPUT);
    am_hal_gpio_out_bit_clear(43);
    am_hal_gpio_pin_config(43, AM_HAL_PIN_OUTPUT);
    am_hal_gpio_out_bit_clear(49);
    am_hal_gpio_pin_config(49, AM_HAL_PIN_OUTPUT);
#endif

    //
    // TimerA0 init.
    //
    timerA0_init();

    //
    // Enable the timer Interrupt.
    //
    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA0);

    //
    // Enable the timer interrupt in the NVIC.
    //
    NVIC_EnableIRQ(CTIMER_IRQn);

    //
    // Enable the timer interrupt in the NVIC.
    //
    NVIC_EnableIRQ(FRAM_IOM_IRQn);

    am_hal_interrupt_master_enable();

    init_pattern();

    //
    // Initialize the FRAM Device
    //
    iRet = fram_init();
    if (iRet)
    {
        am_util_stdio_printf("Unable to initialize FRAM\n");
        while(1);
    }

    // Set up the periodic FRAM Read
    am_hal_ctimer_int_register(AM_HAL_CTIMER_INT_TIMERA0, initiate_fram_read);

    //
    // Start timer A0
    //
    am_hal_ctimer_start(0, AM_HAL_CTIMER_TIMERA);

    am_util_stdio_printf("Periodically Reading data from FRAM using non-blocking read - %d times\n", NUM_INERATIONS);
    //
    // Loop forever.
    //
    while(1)
    {
        //
        // Disable interrupt while we decide whether we're going to sleep.
        //
        uint32_t ui32IntStatus = am_hal_interrupt_master_disable();

        if (!g_bReadFram && !g_bVerifyReadData)
        {
            // Wait for Baud rate detection
            am_hal_sysctrl_sleep(true);
            //
            // Enable interrupts
            //
            am_hal_interrupt_master_set(ui32IntStatus);
        }
        else if (g_bReadFram)
        {
            //
            // Enable interrupts
            //
            am_hal_interrupt_master_set(ui32IntStatus);
            read_fram();
        }
        else if (g_bVerifyReadData)
        {
            //
            // Enable interrupts
            //
            am_hal_interrupt_master_set(ui32IntStatus);
            verify_fram_data();
            if (++numRead >= NUM_INERATIONS)
            {
                am_util_stdio_printf("\n%d Reads done\n", NUM_INERATIONS);
                break;
            }
        }
    }
    // Cleanup
    am_util_stdio_printf("\nEnd of FRAM Example\n");
    //
    // Disable the timer Interrupt.
    //
    am_hal_ctimer_int_disable(AM_HAL_CTIMER_INT_TIMERA0);

    //
    // disable the interrupts in the NVIC.
    //
    NVIC_DisableIRQ(CTIMER_IRQn);
    fram_term();
    while(1);
}
