//*****************************************************************************
//
//! @file startup_iar.c
//!
//! @brief Definitions for interrupt handlers, the vector table, and the stack.
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

#include <stdint.h>

//*****************************************************************************
//
// Enable the IAR extensions for this source file.
//
//*****************************************************************************
#pragma language = extended

//*****************************************************************************
//
// Weak function links.
//
//*****************************************************************************
#pragma weak am_mpufault_isr        = am_fault_isr
#pragma weak am_busfault_isr        = am_fault_isr
#pragma weak am_usagefault_isr      = am_fault_isr
#pragma weak am_svcall_isr          = am_default_isr
#pragma weak am_debugmon_isr        = am_default_isr
#pragma weak am_pendsv_isr          = am_default_isr
#pragma weak am_systick_isr         = am_default_isr
#pragma weak am_brownout_isr        = am_default_isr
#pragma weak am_adc_isr             = am_default_isr
#pragma weak am_watchdog_isr        = am_default_isr
#pragma weak am_clkgen_isr          = am_default_isr
#pragma weak am_vcomp_isr           = am_default_isr
#pragma weak am_ioslave_ios_isr     = am_default_isr
#pragma weak am_ioslave_acc_isr     = am_default_isr
#pragma weak am_iomaster0_isr       = am_default_isr
#pragma weak am_iomaster1_isr       = am_default_isr
#pragma weak am_gpio_isr            = am_default_isr
#pragma weak am_ctimer_isr          = am_default_isr
#pragma weak am_uart_isr            = am_default_isr

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
extern __stackless void am_reset_isr(void);
extern __weak void am_nmi_isr(void);
extern __weak void am_fault_isr(void);
extern void am_mpufault_isr(void);
extern void am_busfault_isr(void);
extern void am_usagefault_isr(void);
extern void am_svcall_isr(void);
extern void am_debugmon_isr(void);
extern void am_pendsv_isr(void);
extern void am_systick_isr(void);
extern void am_brownout_isr(void);
extern void am_adc_isr(void);
extern void am_watchdog_isr(void);
extern void am_clkgen_isr(void);
extern void am_vcomp_isr(void);
extern void am_ioslave_ios_isr(void);
extern void am_ioslave_acc_isr(void);
extern void am_iomaster0_isr(void);
extern void am_iomaster1_isr(void);
extern void am_gpio_isr(void);
extern void am_ctimer_isr(void);
extern void am_uart_isr(void);

extern void am_default_isr(void);

//*****************************************************************************
//
// The entry point for the application startup code.
//
//*****************************************************************************
extern void __iar_program_start(void);

//*****************************************************************************
//
// Reserve space for the system stack.
//
//*****************************************************************************
static uint32_t pui32Stack[1024] @ ".noinit";

//*****************************************************************************
//
// A union that describes the entries of the vector table.  The union is needed
// since the first entry is the stack pointer and the remainder are function
// pointers.
//
//*****************************************************************************
typedef union
{
    void (*pfnHandler)(void);
    uint32_t ui32Ptr;
}
uVectorEntry;

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
// Note: Aliasing and weakly exporting am_mpufault_isr, am_busfault_isr, and
// am_usagefault_isr does not work if am_fault_isr is defined externally.
// Therefore, we'll explicitly use am_fault_isr in the table for those vectors.
//
//*****************************************************************************
__root const uVectorEntry __vector_table[] @ ".intvec" =
{
    { .ui32Ptr = (uint32_t)pui32Stack + sizeof(pui32Stack) },
                                                // The initial stack pointer
    am_reset_isr,                               // The reset handler
    am_nmi_isr,                                 // The NMI handler
    am_fault_isr,                               // The hard fault handler
    am_fault_isr,                               // The MPU fault handler
    am_fault_isr,                               // The bus fault handler
    am_fault_isr,                               // The usage fault handler
    0,                                          // Reserved
    0,                                          // Reserved
    0,                                          // Reserved
    0,                                          // Reserved
    am_svcall_isr,                              // SVCall handle
    am_debugmon_isr,                            // Debug monitor handler
    0,                                          // Reserved
    am_pendsv_isr,                              // The PendSV handler
    am_systick_isr,                             // The SysTick handler

    //
    // Peripheral Interrupts
    //
    am_brownout_isr,                            //  0: Brownout
    am_watchdog_isr,                            //  1: Watchdog
    am_clkgen_isr,                              //  2: CLKGEN
    am_vcomp_isr,                               //  3: Voltage Comparator
    am_ioslave_ios_isr,                         //  4: I/O Slave general
    am_ioslave_acc_isr,                         //  5: I/O Slave access
    am_iomaster0_isr,                           //  6: I/O Master 0
    am_iomaster1_isr,                           //  7: I/O Master 1
    am_adc_isr,                                 //  8: ADC
    am_gpio_isr,                                //  9: GPIO
    am_ctimer_isr,                              // 10: CTIMER
    am_uart_isr                                 // 11: UART
};

//*****************************************************************************
//
// Note - The template for this function is originally found in IAR's module,
//        low_level_init.c. As supplied by IAR, it is an empty function.
//
// This module contains the function `__low_level_init', a function
// that is called before the `main' function of the program.  Normally
// low-level initializations - such as setting the prefered interrupt
// level or setting the watchdog - can be performed here.
//
// Note that this function is called before the data segments are
// initialized, this means that this function cannot rely on the
// values of global or static variables.
//
// When this function returns zero, the startup code will inhibit the
// initialization of the data segments. The result is faster startup,
// the drawback is that neither global nor static data will be
// initialized.
//
// Copyright 1999-2017 IAR Systems AB.
//
// $Revision: 112610 $
//
//
//
//
//*****************************************************************************
#define AM_REGVAL(x)               (*((volatile uint32_t *)(x)))
#define VTOR_ADDR                   0xE000ED08

__interwork int __low_level_init(void)
{

    AM_REGVAL(VTOR_ADDR) = (uint32_t)&__vector_table;

    /*==================================*/
    /* Choose if segment initialization */
    /* should be done or not.           */
    /* Return: 0 to omit seg_init       */
    /*         1 to run seg_init        */
    /*==================================*/
    return 1;
}

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.
//
//*****************************************************************************
void
am_reset_isr(void)
{
    //
    // Call the application's entry point.
    //
    __iar_program_start();
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
__weak void
am_nmi_isr(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
__weak void
am_fault_isr(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
am_default_isr(void)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}
