//*****************************************************************************
//
//! @file mspi_flash_loader.c
//!
//! @brief MSPI External Flash Loading and Execution Example 
//!
//! Purpose: This example demonstrates loading a binary image from internal 
//! flash to MSPI external quad flash, then executing the program using 
//! XIP from the external flash.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//! Additional Information:
//! The binary must be linked to run from MSPI flash address range
//! (as specified by BIN_INSTALL_ADDR). The location and size of the binary
//! in internal flash are specified using BIN_ADDR_FLASH & BIN_SIZE
//!
//! This example has been enhanced to use the new 'binary patching' feature
//! This example will not build if proper startup/linker files are not used.
//!
//! Prepare the example as follows:
//!     1. Generate hello_world example to load and execute at MSPI Flash XIP location 0x04000000.
//!         i. In the /examples/hello_world/iar directory modify the FLASH region as follows:
//!             change "define region FLASH = mem:[from 0x0000C000 to 0x000FC000];"
//!             to "define region FLASH = mem:[from 0x04000000 to 0x040F0000];"
//!         ii. Execute "make" in the /examples/hello_world/iar directory to rebuild the project.
//!     2. Copy /examples/hello_world/iar/bin/hello_world.bin into /boards/common3/examples/mspi_flash_loader/
//!     3. Create the binary with mspi_flash_loader + external executable from Step #1.
//!         ./mspi_loader_binary_combiner.py --loaderbin iar/bin/mspi_flash_loader.bin --appbin hello_world.bin --install-address 0x04000000 --flags 0x2 --outbin loader_hello_world --loader-address 0x0000C000
//!     4. Open the J-Link SWO Viewer to the target board.
//!     5. Open the J-Flash Lite program.  Select the /examples/mspi_flash_loader/loader_hello_world.bin file and program at 0x0000C000 offset.
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
#include "am_devices_mspi_flash.h"
#include "am_util.h"

#if FIREBALL_CARD
//
// The Fireball device card multiplexes various devices including each of an SPI
// and I2C FRAM and MSPI flash devices.
// The Fireball device driver controls access to these devices.
// If the Fireball card is not used, FRAM devices can be connected directly
// to appropriate GPIO pins.
//
#include "am_devices_fireball.h"
#endif // FIREBALL_CARD


// Location of the MSPI Flash in address space
#define MSPI_XIP_BASE_ADDRESS   0x04000000

// Default Details of binary in internal flash
#define BIN_ADDR_FLASH          (64*1024)
#define BIN_SIZE                (32*1024)
#define BIN_INSTALL_ADDR        MSPI_XIP_BASE_ADDRESS
#define ENABLE_LOGGING

#define MSPI_TEST_MODULE        0

#define DEFAULT_TIMEOUT         10000
#define TEMP_BUFFER_SIZE        2048
#ifdef ENABLE_LOGGING
#define DEBUG_PRINT am_util_stdio_printf
#else
#define DEBUG_PRINT(...)
#endif

uint32_t        g_TempBuf[TEMP_BUFFER_SIZE / 4];
uint32_t        DMATCBBuffer[2560];

// Patchable section of binary
extern uint32_t __Patchable[];

const am_hal_mspi_dev_config_t      MSPI_Flash_Serial_CE0_MSPIConfig =
{
  .eSpiMode             = AM_HAL_MSPI_SPI_MODE_0,
  .eClockFreq           = AM_HAL_MSPI_CLK_24MHZ,
#if defined(MICRON_N25Q256A)
  .ui8TurnAround        = 3,
  .eAddrCfg             = AM_HAL_MSPI_ADDR_3_BYTE,
#elif defined (CYPRESS_S25FS064S)
  .ui8TurnAround        = 3,
  .eAddrCfg             = AM_HAL_MSPI_ADDR_3_BYTE,
#elif defined (MACRONIX_MX25U12835F)
  .ui8TurnAround        = 8,
  .eAddrCfg             = AM_HAL_MSPI_ADDR_3_BYTE,
#elif defined (ADESTO_ATXP032)
  .ui8TurnAround        = 8,
  .eAddrCfg             = AM_HAL_MSPI_ADDR_4_BYTE,
#endif
  .eInstrCfg            = AM_HAL_MSPI_INSTR_1_BYTE,
  .eDeviceConfig        = AM_HAL_MSPI_FLASH_SERIAL_CE0,
  .bSeparateIO          = true,
  .bSendInstr           = true,
  .bSendAddr            = true,
  .bTurnaround          = true,
  .ui8ReadInstr         = AM_DEVICES_MSPI_FLASH_FAST_READ,
  .ui8WriteInstr        = AM_DEVICES_MSPI_FLASH_PAGE_PROGRAM,
  .ui32TCBSize          = (sizeof(DMATCBBuffer) / sizeof(uint32_t)),
  .pTCB                 = DMATCBBuffer,
  .scramblingStartAddr  = 0,
  .scramblingEndAddr    = 0,
};

const am_hal_mspi_dev_config_t      MSPI_Flash_Quad_CE0_MSPIConfig =
{
  .eSpiMode             = AM_HAL_MSPI_SPI_MODE_0,
  .eClockFreq           = AM_HAL_MSPI_CLK_24MHZ,
#if defined(MICRON_N25Q256A)
  .ui8TurnAround        = 3,
  .eAddrCfg             = AM_HAL_MSPI_ADDR_3_BYTE,
#elif defined (CYPRESS_S25FS064S)
  .ui8TurnAround        = 3,
  .eAddrCfg             = AM_HAL_MSPI_ADDR_3_BYTE,
#elif defined (MACRONIX_MX25U12835F)
  .ui8TurnAround        = 8,
  .eAddrCfg             = AM_HAL_MSPI_ADDR_3_BYTE,
#elif defined (ADESTO_ATXP032)
  .ui8TurnAround        = 8,
  .eAddrCfg             = AM_HAL_MSPI_ADDR_4_BYTE,
#endif
  .eInstrCfg            = AM_HAL_MSPI_INSTR_1_BYTE,
  .eDeviceConfig        = AM_HAL_MSPI_FLASH_QUAD_CE0,
  .bSeparateIO          = false,
  .bSendInstr           = true,
  .bSendAddr            = true,
  .bTurnaround          = true,
#if (defined(MICRON_N25Q256A) || defined(MACRONIX_MX25U12835F) || defined(ADESTO_ATXP032))
  .ui8ReadInstr         = AM_DEVICES_MSPI_FLASH_FAST_READ,
#elif defined (CYPRESS_S25FS064S)
  .ui8ReadInstr         = AM_DEVICES_MSPI_FLASH_QUAD_IO_READ,
#endif          // TODO - Flag an error if another part.
  .ui8WriteInstr        = AM_DEVICES_MSPI_FLASH_PAGE_PROGRAM,
  .ui32TCBSize          = (sizeof(DMATCBBuffer) / sizeof(uint32_t)),
  .pTCB                 = DMATCBBuffer,
  .scramblingStartAddr  = 0,
  .scramblingEndAddr    = 0,
};

const am_hal_mspi_dev_config_t      MSPI_Flash_OctalMSPIConfig =
{
  .eSpiMode             = AM_HAL_MSPI_SPI_MODE_0,
  .eClockFreq           = AM_HAL_MSPI_CLK_24MHZ,
  .ui8TurnAround        = 8,
  .eAddrCfg             = AM_HAL_MSPI_ADDR_4_BYTE,
  .eInstrCfg            = AM_HAL_MSPI_INSTR_1_BYTE,
  .eDeviceConfig        = AM_HAL_MSPI_FLASH_OCTAL_CE0,
  .bSeparateIO          = false,
  .bSendInstr           = true,
  .bSendAddr            = true,
  .bTurnaround          = true,
  .ui8ReadInstr         = AM_DEVICES_MSPI_FLASH_FAST_READ,
  .ui8WriteInstr        = AM_DEVICES_MSPI_FLASH_PAGE_PROGRAM,
  .ui32TCBSize          = (sizeof(DMATCBBuffer) / sizeof(uint32_t)),
  .pTCB                 = DMATCBBuffer,
  .scramblingStartAddr  = 0,
  .scramblingEndAddr    = 0,
};

// This function intializes the VTOR, SP and jumps the the Reset Vector of the image provided
#if defined(__GNUC_STDC_INLINE__)
__attribute__((naked))
static void
run_new_image(uint32_t *vtor)
{
    __asm
    (
        "   movw    r3, #0xED08\n\t"    // Store the vector table pointer of the new image into VTOR.
        "   movt    r3, #0xE000\n\t"
        "   str     r0, [r3, #0]\n\t"
        "   ldr     r3, [r0, #0]\n\t"   // Load the new stack pointer into R3 and the new reset vector into R2.
        "   ldr     r2, [r0, #4]\n\t"
        "   mov     sp, r3\n\t"         // Set the stack pointer for the new image.
        "   bx      r2\n\t"            // Jump to the new reset vector.
    );
}

#elif (defined (__ARMCC_VERSION)) && (__ARMCC_VERSION <  6000000)
static __asm void
run_new_image(uint32_t *vtor)
{
    //
    // Store the vector table pointer of the new image into VTOR.
    //
    movw    r3, #0xED08
    movt    r3, #0xE000
    str     r0, [r3, #0]

    //
    // Load the new stack pointer into R1 and the new reset vector into R2.
    //
    ldr     r3, [r0, #0]
    ldr     r2, [r0, #4]

    //
    // Set the stack pointer for the new image.
    //
    mov     sp, r3

    //
    // Jump to the new reset vector.
    //
    bx      r2
}
#elif defined(__IAR_SYSTEMS_ICC__)
__stackless static inline void
run_new_image(uint32_t *vtor)
{
    __asm volatile (
          "    movw    r3, #0xED08\n"    // Store the vector table pointer of the new image into VTOR.
          "    movt    r3, #0xE000\n"
          "    str     r0, [r3, #0]\n"
          "    ldr     r3, [r0, #0]\n"   // Load the new stack pointer into R1 and the new reset vector into R2.
          "    ldr     r2, [r0, #4]\n"
          "    mov     sp, r3\n"         // Set the stack pointer for the new image.
          "    bx      r2\n"            // Jump to the new reset vector.
          );
}
#else
#error "IDE not supported"
#endif

//*****************************************************************************
//
// MSPI Example Main.
//
//*****************************************************************************
int
main(void)
{
    uint32_t      ui32Status;
    uint32_t      u32InstallOffset;
    void          *pHandle;

    am_hal_mspi_dev_config_t MspiCfg = MSPI_Flash_Quad_CE0_MSPIConfig;
    bool          bScramble = false;
    bool          bRun = false;

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

#ifdef ENABLE_LOGGING
#if 0
    //
    // Initialize the printf interface for UART output.
    //
    am_bsp_uart_printf_enable();
#else
    //
    // Initialize the printf interface for ITM/SWO output.
    //
    am_bsp_itm_printf_enable();
#endif

    //
    // Print the banner.
    //
    am_util_stdio_terminal_clear();
    DEBUG_PRINT("Apollo3 MSPI Flash Loader Example\n\n");
#endif

#if FIREBALL_CARD
    //
    // Set the MUX for the Flash Device
    //
    uint32_t ui32Ret, ui32ID;

#if 1
    //
    // Get Fireball ID and Rev info.
    //
    ui32Ret = am_devices_fireball_control(AM_DEVICES_FIREBALL_STATE_ID_GET, &ui32ID);
    if ( ui32Ret != 0 )
    {
        DEBUG_PRINT("FAIL: am_devices_fireball_control(%d) returned 0x%X.\n",
                             AM_DEVICES_FIREBALL_STATE_ID_GET, ui32Ret);
        return -1;
    }
    else if ( ui32ID == FIREBALL_ID )
    {
        DEBUG_PRINT("Fireball found, ID is 0x%X.\n", ui32ID);
    }
    else
    {
        DEBUG_PRINT("Unknown device returned ID as 0x%X.\n", ui32ID);
    }

    ui32Ret = am_devices_fireball_control(AM_DEVICES_FIREBALL_STATE_VER_GET, &ui32ID);
    if ( ui32Ret != 0 )
    {
        DEBUG_PRINT("FAIL: am_devices_fireball_control(%d) returned 0x%X.\n",
                             AM_DEVICES_FIREBALL_STATE_VER_GET, ui32Ret);
        return -1;
    }
    else
    {
        DEBUG_PRINT("Fireball Version is 0x%X.\n", ui32ID);
    }
#endif

#if defined (ADESTO_ATXP032)
    ui32Ret = am_devices_fireball_control(AM_DEVICES_FIREBALL_STATE_OCTAL_FLASH_CE0, 0);
#else
    ui32Ret = am_devices_fireball_control(AM_DEVICES_FIREBALL_STATE_TWIN_QUAD_CE0_CE1, 0);
#endif
    if ( ui32Ret != 0 )
    {
        DEBUG_PRINT("FAIL: am_devices_fireball_control(%d) returned 0x%X.\n",
                             AM_DEVICES_FIREBALL_STATE_TWIN_QUAD_CE0_CE1, ui32Ret);
        return -1;
    }
#endif // FIREBALL_CARD

    uint32_t binAddr, binInstallAddr, binSize;

    // some of the parameters are controllable through binary patching
    if (__Patchable[0])
    {
        DEBUG_PRINT("Valid Patched information found\n");
        binAddr = __Patchable[0];
        binInstallAddr = __Patchable[1];
        binSize = __Patchable[2];
        u32InstallOffset = (binInstallAddr - MSPI_XIP_BASE_ADDRESS);
        if (__Patchable[3] & 0x1)
        {
            // Enable scrambling
            MspiCfg.scramblingStartAddr = u32InstallOffset;
            MspiCfg.scramblingEndAddr = u32InstallOffset + binSize - 1;
            bScramble = true;
        }
        if (__Patchable[3] & 0x2)
        {
            // Jump to program after installing
            bRun = true;
        }
    }
    else
    {
        binAddr = BIN_ADDR_FLASH;
        binInstallAddr = BIN_INSTALL_ADDR;
        u32InstallOffset = (BIN_INSTALL_ADDR - MSPI_XIP_BASE_ADDRESS);
        binSize = BIN_SIZE;
    }

    DEBUG_PRINT("Bin Address in internal flash = 0x%x\n", binAddr);
    DEBUG_PRINT("Bin Install Address in external flash = 0x%x\n", binInstallAddr);
    DEBUG_PRINT("Bin Size = 0x%x\n", binSize);
    DEBUG_PRINT("Scrambling is %s\n", bScramble ? "Enabled" : "Disabled");

    //
    // Configure the MSPI and Flash Device.
    //
    ui32Status = am_devices_mspi_flash_init(MSPI_TEST_MODULE, &MspiCfg, &pHandle);
    if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
    {
        DEBUG_PRINT("Failed to configure the MSPI and Flash Device correctly!\n");
        return -1;
    }

    //
    // Read the MSPI Device ID.
    //
#if !defined (ADESTO_ATXP032)
    ui32Status = am_devices_mspi_flash_id(MSPI_TEST_MODULE);
    if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
    {
        DEBUG_PRINT("Failed to read Flash Device ID!\n");
        return -1;
    }
#endif

#if !defined (ADESTO_ATXP032)
    // Mass Erase
    DEBUG_PRINT("Initiating mass erase Flash Device!\n");
    DEBUG_PRINT("This takes about 40 seconds!\n");
    ui32Status = am_devices_mspi_flash_mass_erase(MSPI_TEST_MODULE);
    if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
    {
        DEBUG_PRINT("Failed to mass erase Flash Device!\n");
        return -1;
    }
    DEBUG_PRINT("mass erase Flash Device Done!\n");
#else
    // Erase the sectors we need to program
    DEBUG_PRINT("Initiating erase of required sectors of Flash Device!\n");
    for (uint32_t address = u32InstallOffset; address < (u32InstallOffset + binSize); address += AM_DEVICES_MSPI_FLASH_SECTOR_SIZE)
    {
        uint32_t sector = address / AM_DEVICES_MSPI_FLASH_SECTOR_SIZE;
        //
        // Erase the target sector.
        //
        DEBUG_PRINT("Erasing Sector %d\n", sector);
        ui32Status = am_devices_mspi_flash_sector_erase(MSPI_TEST_MODULE, sector << 16);
        if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
        {
            DEBUG_PRINT("Failed to erase Flash Device sector!\n");
            return -1;
        }
    }
#endif
    DEBUG_PRINT("Erase Done!\n");

    //
    // Enable XIP mode.
    //
    ui32Status = am_devices_mspi_flash_enable(true, false);
    if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to enable XIP mode in the MSPI!\n");
    }

    //
    // Check to see that the sector is actually erased.
    //
    uint32_t    *pui32Address = (uint32_t *)(MSPI_XIP_BASE_ADDRESS + u32InstallOffset);
    for (uint32_t i = 0; i < binSize / 4; i++)
    {
        if (*pui32Address != 0xFFFFFFFF)
        {
            am_util_stdio_printf("Failed to erase Flash Device sector!\n");
            return -1;
        }
    }

    //
    // Make sure we aren't in XIP mode.
    //
    ui32Status = am_devices_mspi_flash_disable_xip(MSPI_TEST_MODULE);
    if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
    {
        DEBUG_PRINT("Failed to disable XIP mode in the MSPI!\n");
        return -1;
    }

    if (bScramble)
    {
        //
        // Turn on scrambling operation.
        //
        DEBUG_PRINT("Putting the MSPI into Scrambling mode\n");
        ui32Status = am_devices_mspi_flash_enable(false, bScramble);
        if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
        {
            DEBUG_PRINT("Failed to enable MSPI scrambling!\n");
        }
    }

    //
    // Write the executable binary into MSPI flash
    //
    DEBUG_PRINT("Writing image to External Flash Device!\n");
    ui32Status = am_devices_mspi_flash_write(MSPI_TEST_MODULE, (uint8_t *)binAddr, u32InstallOffset, binSize);
    if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
    {
        DEBUG_PRINT("Failed to write buffer to Flash Device!\n");
        return -1;
    }
    am_hal_cachectrl_control(AM_HAL_CACHECTRL_CONTROL_FLASH_CACHE_INVALIDATE, 0);
    // Confirm that the flash now has the correct data
    DEBUG_PRINT("Verifying image in External Flash Device!\n");
    for (uint32_t address = u32InstallOffset; address < (u32InstallOffset + binSize); address += TEMP_BUFFER_SIZE)
    {
        //
        // Read the data back into the RX buffer.
        //
        ui32Status = am_devices_mspi_flash_read((uint8_t *)g_TempBuf, address, TEMP_BUFFER_SIZE, true);
        if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
        {
            DEBUG_PRINT("Failed to read buffer to Flash Device!\n");
            return -1;
        }

        //
        // Compare the buffers
        //
        for (uint32_t i = 0; (i < TEMP_BUFFER_SIZE / 4) && ((address + i * 4) < binSize); i++)
        {
            if (g_TempBuf[i] != *((uint32_t *)(binAddr + address + i*4 - u32InstallOffset)))
            {
                DEBUG_PRINT("TX and RX buffers failed to compare at offset 0x%x - Expected 0x%x Actual 0x%x!\n",
                                address + i*4,
                                *((uint32_t *)(binAddr + address + i*4 - u32InstallOffset)),
                                g_TempBuf[i]);
                return -1;
            }
        }
    }

    if (bRun)
    {
        //
        // Set up for XIP operation.
        //
        DEBUG_PRINT("Putting the MSPI and External Flash into XIP mode\n");
        ui32Status = am_devices_mspi_flash_enable(true, bScramble);
        if (AM_DEVICES_MSPI_FLASH_STATUS_SUCCESS != ui32Status)
        {
            DEBUG_PRINT("Failed to put the MSPI into XIP mode!\n");
            return -1;
        }

        uint32_t vtor = binInstallAddr;

        DEBUG_PRINT("Jumping to relocated image in MSPI Flash\n");
        // Add delay
        am_util_delay_ms(100);
        // Run binary from MSPI
        run_new_image((uint32_t *)vtor);

        // Will not return!!
    }
    else
    {
        DEBUG_PRINT("MSPI Flash Loaded\n");
        while(1);
    }
}

