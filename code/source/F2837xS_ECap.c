//###########################################################################
//
// FILE:   F2837xS_ECap.c
//
// TITLE:  F2837xS eCAP Initialization & Support Functions.
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

/*
 * 功能：配置脉冲捕获（ECAP）
 */
void InitECap(void)
{
	ECap1Regs.ECEINT.all = 0x0000; //关闭ECap-1的所有中断源。WY
	ECap1Regs.ECCLR.all = 0xFFFF; //清除ECap-1的所有中断标志位。WY
	ECap1Regs.ECCTL1.bit.CAPLDEN = 0; //失能时间戳记录。WY
	ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0; //停止计数器。WY

	ECap2Regs.ECEINT.all = 0x0000; //关闭ECap2的所有中断源。WY
	ECap2Regs.ECCLR.all = 0xFFFF; //清除ECap2的所有中断标志位。WY
	ECap2Regs.ECCTL1.bit.CAPLDEN = 0; //失能时间戳记录。WY
	ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0; //停止计数器。WY

	ECap3Regs.ECEINT.all = 0x0000; //关闭ECap3的所有中断源。WY
	ECap3Regs.ECCLR.all = 0xFFFF; //清除ECap3的所有中断标志位。WY
	ECap3Regs.ECCTL1.bit.CAPLDEN = 0; //失能时间戳记录。WY
	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0; //停止计数器。WY

	ECap1Regs.ECCTL1.bit.FREE_SOFT = 3; //计数器不受调试模式的影响。WY
	ECap1Regs.ECCTL1.bit.PRESCALE = 0; //预分频系数为1。WY
	ECap1Regs.ECCTL2.bit.CAP_APWM = 0; //脉冲捕获模式。WY
	ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0; //连续捕获脉冲。WY
	ECap1Regs.ECCTL2.bit.STOP_WRAP = 3; //在第四次捕获事件发生后，连续捕获脉冲。WY
	ECap1Regs.ECCTL1.bit.CAP1POL = 1; //捕获下降沿。WY
	ECap1Regs.ECCTL1.bit.CAP2POL = 1; //捕获下降沿。WY
	ECap1Regs.ECCTL1.bit.CAP3POL = 1; //捕获下降沿。WY
	ECap1Regs.ECCTL1.bit.CAP4POL = 1; //捕获下降沿。WY
	ECap1Regs.ECCTL1.bit.CTRRST1 = 1; //在第1次捕获事件发生后复位计数器。WY
	ECap1Regs.ECCTL1.bit.CTRRST2 = 1; //在第2次捕获事件发生后复位计数器。WY
	ECap1Regs.ECCTL1.bit.CTRRST3 = 1; //在第3次捕获事件发生后复位计数器。WY
	ECap1Regs.ECCTL1.bit.CTRRST4 = 1; //在第4次捕获事件发生后复位计数器。WY
	ECap1Regs.ECCTL2.bit.SYNCI_EN = 0; //失能输入同步信号。WY
	ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0; //将输入同步信号作为输出同步信号。WY
	ECap1Regs.ECCTL1.bit.CAPLDEN = 1; //使能时间戳记录。WY
	ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1; //启动计数器。WY
	ECap1Regs.ECCTL2.bit.REARM = 1; //复位ECAP-1。WY
	ECap1Regs.ECCTL1.bit.CAPLDEN = 1; //使能时间戳记录。WY
	ECap1Regs.ECEINT.bit.CEVT4 = 1; //将第4次捕获事件作为中断源。WY
	ECap1Regs.ECEINT.bit.CEVT3 = 1; //将第3次捕获事件作为中断源。WY
	ECap1Regs.ECEINT.bit.CEVT2 = 1; //将第2次捕获事件作为中断源。WY
	ECap1Regs.ECEINT.bit.CEVT1 = 1; //将第1次捕获事件作为中断源。WY

	ECap2Regs.ECCTL1.bit.FREE_SOFT = 3; //计数器不受调试模式的影响。WY
	ECap2Regs.ECCTL1.bit.PRESCALE = 0; //预分频系数为1。WY
	ECap2Regs.ECCTL2.bit.CAP_APWM = 0; //脉冲捕获模式。WY
	ECap2Regs.ECCTL2.bit.CONT_ONESHT = 0; //连续捕获脉冲。WY
	ECap2Regs.ECCTL2.bit.STOP_WRAP = 3; //在第四次捕获事件发生后，连续捕获脉冲。WY
	ECap2Regs.ECCTL1.bit.CAP1POL = 1; //捕获下降沿。WY
	ECap2Regs.ECCTL1.bit.CAP2POL = 1; //捕获下降沿。WY
	ECap2Regs.ECCTL1.bit.CAP3POL = 1; //捕获下降沿。WY
	ECap2Regs.ECCTL1.bit.CAP4POL = 1; //捕获下降沿。WY
	ECap2Regs.ECCTL1.bit.CTRRST1 = 1; //在第1次捕获事件发生后复位计数器。WY
	ECap2Regs.ECCTL1.bit.CTRRST2 = 1; //在第1次捕获事件发生后复位计数器。WY
	ECap2Regs.ECCTL1.bit.CTRRST3 = 1; //在第1次捕获事件发生后复位计数器。WY
	ECap2Regs.ECCTL1.bit.CTRRST4 = 1; //在第1次捕获事件发生后复位计数器。WY
	ECap2Regs.ECCTL2.bit.SYNCI_EN = 0; //失能输入同步信号。WY
	ECap2Regs.ECCTL2.bit.SYNCO_SEL = 0; //将输入同步信号作为输出同步信号。WY
	ECap2Regs.ECCTL1.bit.CAPLDEN = 1; //使能时间戳记录。WY
	ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1; //启动计数器。WY
	ECap2Regs.ECCTL2.bit.REARM = 1; //复位ECAP-2。WY
	ECap2Regs.ECCTL1.bit.CAPLDEN = 1; //使能时间戳记录。WY
	ECap2Regs.ECEINT.bit.CEVT4 = 1; //将第4次捕获事件作为中断源。WY
	ECap2Regs.ECEINT.bit.CEVT3 = 1; //将第3次捕获事件作为中断源。WY
	ECap2Regs.ECEINT.bit.CEVT2 = 1; //将第2次捕获事件作为中断源。WY
	ECap2Regs.ECEINT.bit.CEVT1 = 1; //将第1次捕获事件作为中断源。WY

	ECap3Regs.ECCTL1.bit.FREE_SOFT = 3; //计数器不受调试模式的影响。WY
	ECap3Regs.ECCTL1.bit.PRESCALE = 0; //预分频系数为1。WY
	ECap3Regs.ECCTL2.bit.CAP_APWM = 0; //脉冲捕获模式。WY
	ECap3Regs.ECCTL2.bit.CONT_ONESHT = 0; //连续捕获脉冲。WY
	ECap3Regs.ECCTL2.bit.STOP_WRAP = 3; //在第四次捕获事件发生后，连续捕获脉冲。WY
	ECap3Regs.ECCTL1.bit.CAP1POL = 1; //捕获下降沿。WY
	ECap3Regs.ECCTL1.bit.CAP2POL = 1; //捕获下降沿。WY
	ECap3Regs.ECCTL1.bit.CAP3POL = 1; //捕获下降沿。WY
	ECap3Regs.ECCTL1.bit.CAP4POL = 1; //捕获下降沿。WY
	ECap3Regs.ECCTL1.bit.CTRRST1 = 1; //在第1次捕获事件发生后复位计数器。WY
	ECap3Regs.ECCTL1.bit.CTRRST2 = 1; //在第2次捕获事件发生后复位计数器。WY
	ECap3Regs.ECCTL1.bit.CTRRST3 = 1; //在第3次捕获事件发生后复位计数器。WY
	ECap3Regs.ECCTL1.bit.CTRRST4 = 1; //在第4次捕获事件发生后复位计数器。WY
	ECap3Regs.ECCTL2.bit.SYNCI_EN = 0; //失能输入同步信号。WY
	ECap3Regs.ECCTL2.bit.SYNCO_SEL = 0; //将输入同步信号作为输出同步信号。WY
	ECap3Regs.ECCTL1.bit.CAPLDEN = 1; //使能时间戳记录。WY
	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1; //启动计数器。WY
	ECap3Regs.ECCTL2.bit.REARM = 1; //复位ECAP-3。WY
	ECap3Regs.ECCTL1.bit.CAPLDEN = 1; //使能时间戳记录。WY
	ECap3Regs.ECEINT.bit.CEVT4 = 1;//将第4次捕获事件作为中断源。WY
	ECap3Regs.ECEINT.bit.CEVT3 = 1;//将第3次捕获事件作为中断源。WY
	ECap3Regs.ECEINT.bit.CEVT2 = 1;//将第2次捕获事件作为中断源。WY
	ECap3Regs.ECEINT.bit.CEVT1 = 1;//将第1次捕获事件作为中断源。WY

	ECap1Regs.ECCLR.bit.CEVT1 = 1; //清除第1次捕获事件标志位。WY
	ECap1Regs.ECCLR.bit.CEVT2 = 1; //清除第2次捕获事件标志位。WY
	ECap1Regs.ECCLR.bit.CEVT3 = 1; //清除第3次捕获事件标志位。WY
	ECap1Regs.ECCLR.bit.CEVT4 = 1; //清除第4次捕获事件标志位。WY

	ECap2Regs.ECCLR.bit.CEVT1 = 1; //清除第1次捕获事件标志位。WY
	ECap2Regs.ECCLR.bit.CEVT2 = 1; //清除第2次捕获事件标志位。WY
	ECap2Regs.ECCLR.bit.CEVT3 = 1; //清除第3次捕获事件标志位。WY
	ECap2Regs.ECCLR.bit.CEVT4 = 1; //清除第4次捕获事件标志位。WY

	ECap3Regs.ECCLR.bit.CEVT1 = 1; //清除第1次捕获事件标志位。WY
	ECap3Regs.ECCLR.bit.CEVT2 = 1; //清除第2次捕获事件标志位。WY
	ECap3Regs.ECCLR.bit.CEVT3 = 1; //清除第3次捕获事件标志位。WY
	ECap3Regs.ECCLR.bit.CEVT4 = 1; //清除第4次捕获事件标志位。WY

	ECap5Regs.ECCTL2.bit.CAP_APWM = 1;
	ECap5Regs.ECCTL2.bit.APWMPOL = 1;
	ECap5Regs.CAP1 = 4000;
	ECap5Regs.CAP2 = 0;
	ECap5Regs.CTRPHS = 0x0;

	ECap5Regs.ECCTL2.bit.SYNCI_EN = 0;
	ECap5Regs.ECCTL2.bit.SYNCO_SEL = 0;
	ECap5Regs.ECCTL2.bit.TSCTRSTOP = 1;
}

//
// InitECap - This function initializes the eCAP(s) to a known state.
//
//void InitECap(void)
//{
    // Initialize eCAP1/2/3/4/5/6

    //tbd...
//}

/*
 *	功能：配置脉冲捕获（ECAP）相关引脚。WY
 */
void InitECapGpio()
{
    InitECap1Gpio(94); //A相过流检测
    InitECap2Gpio(85); //B相过流检测
    InitECap3Gpio(97); //C相过流检测
}

/*
 * 功能：配置脉冲捕获（ECAP-1）的输入引脚。WY
 * 输入参数pin：脉冲输入引脚
 */
void InitECap1Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT7SELECT = pin; //将该引脚的脉冲输入ECAP-1
    EDIS;
}

/*
 * 功能：配置脉冲捕获（ECAP-2）的输入引脚。WY
 * 输入参数pin：脉冲输入引脚
 */
void InitECap2Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT8SELECT = pin; //将该引脚的脉冲输入ECAP-2
    EDIS;
}

/*
 * 功能：配置脉冲捕获（ECAP-3）的输入引脚。WY
 * 输入参数pin：脉冲输入引脚
 */
void InitECap3Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT9SELECT = pin; //将该引脚的脉冲输入ECAP-3
    EDIS;
}

//
// InitECap4Gpio - Initialize ECAP4 GPIOs
//
void InitECap4Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT10SELECT = pin;         // Set eCAP4 source to GPIO-pin
    EDIS;
}

//
// InitECap5Gpio - Initialize ECAP5 GPIOs
//
void InitECap5Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT11SELECT = pin;         // Set eCAP5 source to GPIO-pin
    EDIS;
}

//
// InitECap6Gpio - Initialize ECAP6 GPIOs
//
void InitECap6Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT12SELECT = pin;         // Set eCAP6 source to GPIO-pin
    EDIS;
}

//
// InitAPwm1Gpio - Initialize EPWM1 GPIOs
//
//void InitAPwm1Gpio()
//{
//    EALLOW;
//    OutputXbarRegs.OUTPUT3MUX0TO15CFG.bit.MUX0 = 3; // Select ECAP1.OUT on Mux0
//    OutputXbarRegs.OUTPUT3MUXENABLE.bit.MUX0 = 1;  // Enable MUX0 for ECAP1.OUT
//    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 3;    // Select OUTPUTXBAR3 on GPIO5
//    EDIS;
//}
void InitAPwm5Gpio()
{
    EALLOW;
    OutputXbarRegs.OUTPUT4MUX0TO15CFG.bit.MUX8 = 3; // Select ECAP5.OUT on Mux8
    OutputXbarRegs.OUTPUT4MUXENABLE.bit.MUX8 = 1;   // Enable MUX8 for ECAP5.OUT
    GpioCtrlRegs.GPAGMUX1.bit.GPIO15 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 6;            // Select OUTPUTXBAR4 on GPIO15
    EDIS;
}

//
// End of file
//
