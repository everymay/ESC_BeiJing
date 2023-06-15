//###########################################################################
//
// FILE:   F2837xS_Spi.c
//
// TITLE:  F2837xS SPI Initialization & Support Functions.
//
//###########################################################################
// $TI Release: F2837xS Support Library v3.05.00.00 $
// $Release Date: Thu Oct 18 15:50:26 CDT 2018 $
// $Copyright:
// Copyright (C) 2014-2018 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

//
// Included Files
//
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
void InitSpibGpio(void);
void InitSpicGpio(void);
//
// Calculate BRR: 7-bit baud rate register value
// SPI CLK freq = 500 kHz
// LSPCLK freq  = CPU freq / 4  (by default)
// BRR          = (LSPCLK freq / SPI CLK freq) - 1
//
#if CPU_FRQ_200MHZ
#define SPI_BRR_B        ((200E6 / 1) / 15E6) - 1
#define SPI_BRR_C        ((200E6 / 1) / 15E6) - 1
#endif

#if CPU_FRQ_150MHZ
#define SPI_BRR        ((150E6 / 4) / 500E3) - 1
#endif

#if CPU_FRQ_120MHZ
#define SPI_BRR        ((120E6 / 4) / 500E3) - 1
#endif

//
// InitSPI - FM25CL64B,Parameter Storage, Fault Recording, SOE
//
void InitSpib(void)
{
//    // Initialize SPI-B
//
//    // Set reset low before configuration changes
//    // Clock polarity (0 == rising, 1 == falling)
//    // 16-bit character
//    // Enable loop-back
//    SpibRegs.SPICCR.bit.SPISWRESET = 0;
//    SpibRegs.SPICCR.bit.CLKPOLARITY = 0;
//    SpibRegs.SPICCR.bit.SPICHAR = (8-1);
//    SpibRegs.SPICCR.bit.SPILBK = 0;
//
//    // Enable master (0 == slave, 1 == master)
//    // Enable transmission (Talk)
//    // Clock phase (0 == normal, 1 == delayed)
//    // SPI interrupts are disabled
//    SpibRegs.SPICTL.bit.MASTER_SLAVE = 1;
//    SpibRegs.SPICTL.bit.TALK = 1;
//    SpibRegs.SPICTL.bit.CLK_PHASE = 1;
//    SpibRegs.SPICTL.bit.SPIINTENA = 1;
//
//    // Set the baud rate
//    SpibRegs.SPIBRR.bit.SPI_BIT_RATE = SPI_BRR_B;
//
//    // Set FREE bit
//    // Halting on a breakpoint will not halt the SPI
//    SpibRegs.SPIPRI.bit.FREE = 1;
//
//    // Release the SPI from reset
//    SpibRegs.SPICCR.bit.SPISWRESET = 1;
//    SpibRegs.SPICCR.bit.CLKPOLARITY = 0;
       EALLOW;
       SpibRegs.SPICCR.all=0x0007;       //16-bit character, No Loopback mode
       //SpiaRegs.SPICTL.all=0x0017;       //Interrupt enabled, Master/Slave XMIT enabled
       SpibRegs.SPICTL.all=0x0006;       //Interrupt disabled
       //SpiaRegs.SPISTS.all=0x0000;
       SpibRegs.SPIBRR.all = 0x0d;           // Baud rate  200Mhz /(4 + 1) = 20MHz
       SpibRegs.SPIPRI.all = 0x30;    // free run
       SpibRegs.SPICCR.all=0x0087;//Ê¹SPIÍË³ö¸´Î»×´Ì¬
       SpibRegs.SPICCR.bit.CLKPOLARITY = 0;
       SpibRegs.SPICTL.bit.CLK_PHASE   = 1;

       EDIS;
}

void InitSpic(void)
{
    // Initialize SPI-A

    // Set reset low before configuration changes
    // Clock polarity (0 == rising, 1 == falling)
    // 16-bit character
    // Enable loop-back
    SpicRegs.SPICCR.bit.SPISWRESET = 0;
    SpicRegs.SPICCR.bit.CLKPOLARITY = 0;
    SpicRegs.SPICCR.bit.SPICHAR = (8-1);
    SpicRegs.SPICCR.bit.SPILBK = 1;

    // Enable master (0 == slave, 1 == master)
    // Enable transmission (Talk)
    // Clock phase (0 == normal, 1 == delayed)
    // SPI interrupts are disabled
    SpicRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpicRegs.SPICTL.bit.TALK = 1;
    SpicRegs.SPICTL.bit.CLK_PHASE = 1;
    SpicRegs.SPICTL.bit.SPIINTENA = 0;

    // Set the baud rate
    SpicRegs.SPIBRR.bit.SPI_BIT_RATE = SPI_BRR_C;

    // Set FREE bit
    // Halting on a breakpoint will not halt the SPI
    SpicRegs.SPIPRI.bit.FREE = 1;

    // Release the SPI from reset
    SpicRegs.SPICCR.bit.SPISWRESET = 1;
}

//
// InitSpiGpio - This function initializes GPIO pins to function as SPI pins.
//               Each GPIO pin can be configured as a GPIO pin or up to 3
//               different peripheral functional pins. By default all pins come
//               up as GPIO inputs after reset.
//
//               Caution:
//               For each SPI peripheral
//               Only one GPIO pin should be enabled for SPISOMO operation.
//               Only one GPIO pin should be enabled for SPISOMI operation.
//               Only one GPIO pin should be enabled for SPICLK  operation.
//               Only one GPIO pin should be enabled for SPISTE  operation.
//               Comment out other unwanted lines.
//
void InitSpiGpio()
{
    InitSpibGpio(); //
    InitSpicGpio(); //
}

//
// InitSpibGpio - Initialize SPIB GPIOs
//
void InitSpibGpio(void)
{
    EALLOW;

    //
    // Enable internal pull-up for the selected pins
    //
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;  // Enable pull-up on GPIO63 (SPISIMOB)
    GpioCtrlRegs.GPCPUD.bit.GPIO64 = 0;  // Enable pull-up on GPIO64 (SPISOMIB)
    GpioCtrlRegs.GPCPUD.bit.GPIO65 = 0;  // Enable pull-up on GPIO65 (SPICLKB)
    GpioCtrlRegs.GPCPUD.bit.GPIO66 = 0;  // Enable pull-up on GPIO66 (SPISTEB)

    //
    // Set qualification for selected pins to asynch only
    //
    // This will select asynch (no qualification) for the selected pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPBQSEL2.bit.GPIO63= 3; // Asynch input GPIO63 (SPISIMOB)
    GpioCtrlRegs.GPCQSEL1.bit.GPIO64 = 3; // Asynch input GPIO64 (SPISOMIB)
    GpioCtrlRegs.GPCQSEL1.bit.GPIO65 = 3; // Asynch input GPIO65 (SPICLKB)
    GpioCtrlRegs.GPCQSEL1.bit.GPIO66 = 3; // Asynch input GPIO66 (SPISTEB)

    //
    //Configure SPI-B pins using GPIO regs
    //
    // This specifies which of the possible GPIO pins will be SPI functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 3; // Configure GPIO63 as SPISIMOB
    GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 3; // Configure GPIO64 as SPISOMIB
    GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 3; // Configure GPIO65 as SPICLKB
    GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 0; // Configure GPIO66 as SPISTEB
    GpioCtrlRegs.GPCDIR.bit.GPIO66 = 1; // Configure GPIO66 as SPISTEB

    GpioCtrlRegs.GPBGMUX2.bit.GPIO63 = 3; // Configure GPIO63 as SPISIMOB
    GpioCtrlRegs.GPCGMUX1.bit.GPIO64 = 3; // Configure GPIO64 as SPISOMIB
    GpioCtrlRegs.GPCGMUX1.bit.GPIO65 = 3; // Configure GPIO65 as SPICLKB
    EDIS;

}

//
// InitSpicGpio - Initialize SPIC GPIOs
//
void InitSpicGpio(void)
{
    EALLOW;

    //
    // Enable internal pull-up for the selected pins
    //
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPCPUD.bit.GPIO69 = 0;  // Enable pull-up on GPIO69 (SPISIMOC)
    GpioCtrlRegs.GPCPUD.bit.GPIO70 = 0;  // Enable pull-up on GPIO70 (SPISOMIC)
    GpioCtrlRegs.GPCPUD.bit.GPIO71 = 0;  // Enable pull-up on GPIO71 (SPICLKC)
    GpioCtrlRegs.GPCPUD.bit.GPIO72 = 0;  // Enable pull-up on GPIO72 (SPISTEC)

    //
    // Set qualification for selected pins to asynch only
    //
    // This will select asynch (no qualification) for the selected pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPCQSEL1.bit.GPIO69 = 3; // Asynch input GPIO69 (SPISIMOC)
    GpioCtrlRegs.GPCQSEL1.bit.GPIO70 = 3; // Asynch input GPIO70 (SPISOMIC)
    GpioCtrlRegs.GPCQSEL1.bit.GPIO71 = 3; // Asynch input GPIO71 (SPICLKC)
    GpioCtrlRegs.GPCQSEL1.bit.GPIO72 = 3; // Asynch input GPIO72 (SPISTEC)

    //
    //Configure SPI-A pins using GPIO regs
    //
    // This specifies which of the possible GPIO pins will be SPI functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 2; // Configure GPIO69 as SPISIMOC
    GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 2; // Configure GPIO70 as SPISOMIC
    GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 2; // Configure GPIO71 as SPICLKC
    GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 2; // Configure GPIO72 as SPISTEC

    GpioCtrlRegs.GPCGMUX1.bit.GPIO69 = 2; // Configure GPIO69 as SPISIMOC
    GpioCtrlRegs.GPCGMUX1.bit.GPIO70 = 2; // Configure GPIO70 as SPISOMIC
    GpioCtrlRegs.GPCGMUX1.bit.GPIO71 = 2; // Configure GPIO71 as SPICLKC
    GpioCtrlRegs.GPCGMUX1.bit.GPIO72 = 2; // Configure GPIO72 as SPISTEC

    EDIS;
}
//
// End of file
//
