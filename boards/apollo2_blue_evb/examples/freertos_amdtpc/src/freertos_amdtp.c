//*****************************************************************************
//
//! @file freertos_amdtp.c
//!
//! @brief AMDTP example.
//!
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

//*****************************************************************************
//
// This application has a large number of common include files. For
// convenience, we'll collect them all together in a single header and include
// that everywhere.
//
//*****************************************************************************
#include "freertos_amdtp.h"
#include "ble_menu.h"
#include "rtos.h"

#ifdef BLE_MENU
//*****************************************************************************
//
// UART configuration settings.
//
//*****************************************************************************
am_hal_uart_config_t g_sUartConfig =
{
    .ui32BaudRate = 115200,
    .ui32DataBits = AM_HAL_UART_DATA_BITS_8,
    .bTwoStopBits = false,
    .ui32Parity   = AM_HAL_UART_PARITY_NONE,
    .ui32FlowCtrl = AM_HAL_UART_FLOW_CTRL_NONE,
};

void
setup_serial(int32_t i32Module)
{
    //
    // Make sure the UART RX and TX pins are enabled.
    //
    am_bsp_pin_enable(COM_UART_TX);
    am_bsp_pin_enable(COM_UART_RX);

    //
    // Power on the selected UART
    //
    am_hal_uart_pwrctrl_enable(i32Module);

    //
    // Start the UART interface, apply the desired configuration settings, and
    // enable the FIFOs.
    //
    am_hal_uart_clock_enable(i32Module);

    //
    // Disable the UART before configuring it.
    //
    am_hal_uart_disable(i32Module);

    //
    // Configure the UART.
    //
    am_hal_uart_config(i32Module, &g_sUartConfig);

    //
    // Enable the UART FIFO.
    //
   am_hal_uart_fifo_config(i32Module,
        AM_HAL_UART_TX_FIFO_3_4 | AM_HAL_UART_RX_FIFO_3_4);

    //
    // Enable the UART.
    //
    am_hal_uart_enable(i32Module);

#if defined(AM_PART_APOLLO)
    am_hal_interrupt_priority_set(AM_HAL_INTERRUPT_UART, configMAX_SYSCALL_INTERRUPT_PRIORITY);
#else
    am_hal_interrupt_priority_set(AM_HAL_INTERRUPT_UART0, configMAX_SYSCALL_INTERRUPT_PRIORITY);
#endif
    am_hal_uart_int_clear(i32Module, AM_HAL_UART_INT_RX | AM_HAL_UART_INT_RX_TMOUT);
    am_hal_uart_int_enable(i32Module, AM_HAL_UART_INT_RX | AM_HAL_UART_INT_RX_TMOUT);

    am_hal_interrupt_enable(AM_HAL_INTERRUPT_UART);
}

// buffer for printf
static char g_prfbuf[AM_PRINTF_BUFSIZE];

uint32_t
am_menu_printf(const char *pcFmt, ...)
{
    uint32_t ui32NumChars;

    //
    // Convert to the desired string.
    //
    va_list pArgs;
    va_start(pArgs, pcFmt);
    ui32NumChars = am_util_stdio_vsprintf(g_prfbuf, pcFmt, pArgs);
    va_end(pArgs);

    //
    // This is where we print the buffer to the configured interface.
    //
    am_hal_uart_string_transmit_polled(AM_BSP_UART_PRINT_INST, g_prfbuf);

    //
    // return the number of characters printed.
    //
    return ui32NumChars;
}

#else

uint32_t
am_menu_printf(const char *pcFmt, ...)
{
    return 0;
}

#endif

//*****************************************************************************
//
// Enable printing to the console.
//
//*****************************************************************************
void
enable_print_interface(void)
{
    //
    // Initialize a debug printing interface.
    //
    am_hal_itm_enable();
    am_bsp_debug_printf_enable();
    am_util_debug_printf_init(am_hal_itm_print);
    am_util_stdio_terminal_clear();
}

//*****************************************************************************
//
// Main Function
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the clock frequency
    //
    am_hal_clkgen_sysclk_select(AM_HAL_CLKGEN_SYSCLK_MAX);

    //
    // Set the default cache configuration
    //
    am_hal_cachectrl_enable(&am_hal_cachectrl_defaults);

#ifndef NOFPU
    //
    // Enable the floating point module, and configure the core for lazy
    // stacking.
    //
    am_hal_sysctrl_fpu_enable();
    am_hal_sysctrl_fpu_stacking_enable(true);
#else
    am_hal_sysctrl_fpu_disable();
#endif

    //
    // Configure the board for low power.
    //
    am_bsp_low_power_init();

    // Turn off unused Flash & SRAM

#ifdef AM_PART_APOLLO
    //
    // SRAM bank power setting.
    // Need to match up with actual SRAM usage for the program
    // Current usage is between 32K and 40K - so disabling upper 3 banks
    //
    am_hal_mcuctrl_sram_power_set(AM_HAL_MCUCTRL_SRAM_POWER_DOWN_5 |
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_6 |
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_7,
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_5 |
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_6 |
                                  AM_HAL_MCUCTRL_SRAM_POWER_DOWN_7);

#if 0 // Not turning off the Flash as it may be needed to download the image
    //
    // Flash bank power set.
    //
    am_hal_mcuctrl_flash_power_set(AM_HAL_MCUCTRL_FLASH_POWER_DOWN_1);
#endif
#endif // AM_PART_APOLLO
#ifdef AM_PART_APOLLO2
#if 0 // Not turning off the Flash as it may be needed to download the image
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEMEN_FLASH512K);
#endif
    am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEMEN_SRAM256K);
#endif

    //
    // Enable printing to the console.
    //
#ifdef AM_DEBUG_PRINTF
    enable_print_interface();
#endif

    //
    // Initialize plotting interface.
    //
    am_util_debug_printf("FreeRTOS AMDTP Example\n");

#ifdef BLE_MENU
    setup_serial(0);
#endif

    //
    // Run the application.
    //
    run_tasks();

    //
    // We shouldn't ever get here.
    //
    while (1)
    {
    }

}

