//###########################################################################
//
// FILE:   F2837xS_EPwm.c
//
// TITLE:  F2837xS EPwm Initialization & Support Functions.
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
#include "F2837xS_Device.h"     // DSP2833x Headerfile Include File
#include "F2837xS_Examples.h"   // DSP2833x Examples Include File
#include "MarcoDefine.h"
#include "VariblesRef.h"

void InitEPwm(void)
{
	EALLOW;
	CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0; //停止所有的PWM的时基时钟（Time Base Clock）。WY
	EDIS;

	/*配置EPWM3*/
	EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1; //分频系数。WY。默认情况下，EPWM模块的时钟 = 系统时钟 / 2。当系统时钟为200MHz时，EPWM模块的时钟即为100MHz。WY
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; //分频系数。WY
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; //增减模式。WY
	EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; //不产生同步信号。WY
	EPwm3Regs.TBPHS.bit.TBPHS = 0; //相位差为0。WY
	EPwm3Regs.TBCTR = 0; //时基计数器为0。WY
	EPwm3Regs.TBPRD = T1PR; //设置计数周期。频率40KHz。WY
	EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE; //失能时基寄存器的影子寄存器模式。WY
	EPwm3Regs.TBCTL.bit.PRDLD = TB_SHADOW; //当时基计数器为0时，从影子寄存器加载周期值。WY
	EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; //当时基计数器为0时，比较计数器CMPA从其影子寄存器中取值。WY
	EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; //使能比较计数器CMPA的影子寄存器模式。WY
	EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; //当时基计数器为0时，比较计数器CMPB从其影子寄存器中取值。WY
	EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW; //使能比较计数器CMPB的影子寄存器模式。WY
	EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL; //EPWM-3-A作为死区上升沿和下降沿的作用对象。WY
	EPwm3Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; //失能死区功能。WY
	EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HI; //极性选择。EPWM-3-A和EPWM-3-B均不作极性反转。WY
	EPwm3Regs.DBFED.all = 0; //下降沿死区设置为0。WY
	EPwm3Regs.DBRED.all = 0; //上升沿死区设置为0。WY
	EPwm3Regs.AQSFRC.bit.RLDCSF = 3; //失能动作限定模块（Action Qualifier）的影子寄存器模式。WY

	EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1; //分频系数。WY。默认情况下，EPWM模块的时钟 = 系统时钟 / 2。当系统时钟为200MHz时，EPWM模块的时钟即为100MHz。WY
	EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; //分频系数。WY
	EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; //增减模式。WY
	EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; //不产生同步信号。WY
	EPwm4Regs.TBPHS.bit.TBPHS = 0; //相位差为0。WY
	EPwm4Regs.TBCTR = 0; //时基计数器为0。WY
	EPwm4Regs.TBPRD = T1PR; //设置计数周期。频率40KHz。WY
	EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE; //失能时基寄存器的影子寄存器模式。WY
	EPwm4Regs.TBCTL.bit.PRDLD = TB_SHADOW; //当时基计数器为0时，从影子寄存器加载周期值。WY
	EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; //当时基计数器为0时，比较计数器CMPA从其影子寄存器中取值。WY
	EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; //当时基计数器为0时，比较计数器CMPA从其影子寄存器中取值。WY
	EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; //使能比较计数器CMPB的影子寄存器模式。WY
	EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW; //使能比较计数器CMPB的影子寄存器模式。WY
	EPwm4Regs.DBCTL.bit.IN_MODE = DBA_ALL; //EPWM-3-A作为死区上升沿和下降沿的作用对象。WY
	EPwm4Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; //失能死区功能。WY
	EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HI; //极性选择。EPWM-3-A和EPWM-3-B均不作极性反转。WY
	EPwm4Regs.DBFED.all = 0; //下降沿死区设置为0。WY
	EPwm4Regs.DBRED.all = 0; //上升沿死区设置为0。WY
	EPwm4Regs.AQSFRC.bit.RLDCSF = 3; //失能动作限定模块（Action Qualifier）的影子寄存器模式。WY

	EPwm5Regs.TBCTL.bit.CLKDIV = TB_DIV1; //分频系数。WY。默认情况下，EPWM模块的时钟 = 系统时钟 / 2。当系统时钟为200MHz时，EPWM模块的时钟即为100MHz。WY
	EPwm5Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; //分频系数。WY
	EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; //增减模式。WY
	EPwm5Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; //不产生同步信号。WY
	EPwm5Regs.TBPHS.bit.TBPHS = 0; //相位差为0。WY
	EPwm5Regs.TBCTR = 0; //时基计数器为0。WY
	EPwm5Regs.TBPRD = T1PR; //设置计数周期。频率40KHz。WY
	EPwm5Regs.TBCTL.bit.PHSEN = TB_DISABLE; //失能时基寄存器的影子寄存器模式。WY
	EPwm5Regs.TBCTL.bit.PRDLD = TB_SHADOW; //当时基计数器为0时，从影子寄存器加载周期值。WY
	EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; //当时基计数器为0时，比较计数器CMPA从其影子寄存器中取值。WY
	EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; //使能比较计数器CMPA的影子寄存器模式。WY
	EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; //当时基计数器为0时，比较计数器CMPB从其影子寄存器中取值。WY
	EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW; //使能比较计数器CMPB的影子寄存器模式。WY
	EPwm5Regs.DBCTL.bit.IN_MODE = DBA_ALL; //EPWM-3-A作为死区上升沿和下降沿的作用对象。WY
	EPwm5Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; //失能死区功能。WY
	EPwm5Regs.DBCTL.bit.POLSEL = DB_ACTV_HI; //极性选择。EPWM-3-A和EPWM-3-B均不作极性反转。WY
	EPwm5Regs.DBFED.all = 0; //下降沿死区设置为0。WY
	EPwm5Regs.DBRED.all = 0; //上升沿死区设置为0。WY
	EPwm5Regs.AQSFRC.bit.RLDCSF = 3; //失能动作限定模块（Action Qualifier）的影子寄存器模式。WY

	EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV1; //分频系数。WY。默认情况下，EPWM模块的时钟 = 系统时钟 / 2。当系统时钟为200MHz时，EPWM模块的时钟即为100MHz。WY
	EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; //分频系数。WY
	EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; //增减模式。WY
	EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; //不产生同步信号。WY
	EPwm6Regs.TBPHS.bit.TBPHS = 0; //相位差为0。WY
	EPwm6Regs.TBCTR = 0; //时基计数器为0。WY
	EPwm6Regs.TBPRD = T1PR; //设置计数周期。频率40KHz。WY
	EPwm6Regs.TBCTL.bit.PHSEN = TB_DISABLE; //失能时基寄存器的影子寄存器模式。WY
	EPwm6Regs.TBCTL.bit.PRDLD = TB_SHADOW; //当时基计数器为0时，从影子寄存器加载周期值。WY
	EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; //当时基计数器为0时，比较计数器CMPA从其影子寄存器中取值。WY
	EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; //使能比较计数器CMPA的影子寄存器模式。WY
	EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; //当时基计数器为0时，比较计数器CMPB从其影子寄存器中取值。WY
	EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW; //使能比较计数器CMPB的影子寄存器模式。WY
	EPwm6Regs.DBCTL.bit.IN_MODE = DBA_ALL; //EPWM-3-A作为死区上升沿和下降沿的作用对象。WY
	EPwm6Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; //失能死区功能。WY
	EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HI; //极性选择。EPWM-3-A和EPWM-3-B均不作极性反转。WY
	EPwm6Regs.DBFED.all = 0; //下降沿死区设置为0。WY
	EPwm6Regs.DBRED.all = 0; //上升沿死区设置为0。WY
	EPwm6Regs.AQSFRC.bit.RLDCSF = 3; //失能动作限定模块（Action Qualifier）的影子寄存器模式。WY

	EPwm7Regs.TBCTL.bit.CLKDIV = TB_DIV1; //分频系数。WY。默认情况下，EPWM模块的时钟 = 系统时钟 / 2。当系统时钟为200MHz时，EPWM模块的时钟即为100MHz。WY
	EPwm7Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; //分频系数。WY
	EPwm7Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; //增减模式。WY
	EPwm7Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; //不产生同步信号。WY
	EPwm7Regs.TBPHS.bit.TBPHS = 0; //相位差为0。WY
	EPwm7Regs.TBCTR = 0; //时基计数器为0。WY
	EPwm7Regs.TBPRD = T1PR; //设置计数周期。频率40KHz。WY
	EPwm7Regs.TBCTL.bit.PHSEN = TB_DISABLE; //失能时基寄存器的影子寄存器模式。WY
	EPwm7Regs.TBCTL.bit.PRDLD = TB_SHADOW; //当时基计数器为0时，从影子寄存器加载周期值。WY
	EPwm7Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; //当时基计数器为0时，比较计数器CMPA从其影子寄存器中取值。WY
	EPwm7Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; //使能比较计数器CMPA的影子寄存器模式。WY
	EPwm7Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; //当时基计数器为0时，比较计数器CMPB从其影子寄存器中取值。WY
	EPwm7Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW; //使能比较计数器CMPB的影子寄存器模式。WY
	EPwm7Regs.DBCTL.bit.IN_MODE = DBA_ALL; //EPWM-3-A作为死区上升沿和下降沿的作用对象。WY
	EPwm7Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; //失能死区功能。WY
	EPwm7Regs.DBCTL.bit.POLSEL = DB_ACTV_HI; //极性选择。EPWM-3-A和EPWM-3-B均不作极性反转。WY
	EPwm7Regs.DBFED.all = 0; //下降沿死区设置为0。WY
	EPwm7Regs.DBRED.all = 0; //上升沿死区设置为0。WY
	EPwm7Regs.AQSFRC.bit.RLDCSF = 3; //失能动作限定模块（Action Qualifier）的影子寄存器模式。WY

	EPwm8Regs.TBCTL.bit.CLKDIV = TB_DIV1; //分频系数。WY。默认情况下，EPWM模块的时钟 = 系统时钟 / 2。当系统时钟为200MHz时，EPWM模块的时钟即为100MHz。WY
	EPwm8Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; //分频系数。WY
	EPwm8Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; //增减模式。WY
	EPwm8Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; //不产生同步信号。WY
	EPwm8Regs.TBPHS.bit.TBPHS = 0; //相位差为0。WY
	EPwm8Regs.TBCTR = 0; //时基计数器为0。WY
	EPwm8Regs.TBPRD = T1PR; //设置计数周期。频率40KHz。WY
	EPwm8Regs.TBCTL.bit.PHSEN = TB_DISABLE; //失能时基寄存器的影子寄存器模式。WY
	EPwm8Regs.TBCTL.bit.PRDLD = TB_SHADOW; //当时基计数器为0时，从影子寄存器加载周期值。WY
	EPwm8Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; //当时基计数器为0时，比较计数器CMPA从其影子寄存器中取值。WY
	EPwm8Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; //使能比较计数器CMPA的影子寄存器模式。WY
	EPwm8Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; //当时基计数器为0时，比较计数器CMPB从其影子寄存器中取值。WY
	EPwm8Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW; //使能比较计数器CMPB的影子寄存器模式。WY
	EPwm8Regs.DBCTL.bit.IN_MODE = DBA_ALL; //EPWM-3-A作为死区上升沿和下降沿的作用对象。WY
	EPwm8Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; //失能死区功能。WY
	EPwm8Regs.DBCTL.bit.POLSEL = DB_ACTV_HI; //极性选择。EPWM-3-A和EPWM-3-B均不作极性反转。WY
	EPwm8Regs.DBFED.all = 0; //下降沿死区设置为0。WY
	EPwm8Regs.DBRED.all = 0; //上升沿死区设置为0。WY
	EPwm8Regs.AQSFRC.bit.RLDCSF = 3; //失能动作限定模块（Action Qualifier）的影子寄存器模式。WY

	EPwm3Regs.AQCTLA.bit.CAU = AQ_SET; //向上计数时，当时基计数器TBCTR = 比较计数器CMPA，A通道为高电平。WY
	EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR; //向下计数时，当时基计数器TBCTR = 比较计数器CMPA，A通道为低电平。WY
	EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR; //向上计数时，当时基计数器TBCTR =  比较计数器CMPB，B通道为低电平。WY
	EPwm3Regs.AQCTLB.bit.CBD = AQ_SET; //向下计数时，当时基计数器TBCTR =  比较计数器CMPB，B通道为高电平。WY

	EPwm4Regs.AQCTLA.bit.CAU = AQ_SET; //向上计数时，当时基计数器TBCTR = 比较计数器CMPA，A通道为高电平。WY
	EPwm4Regs.AQCTLA.bit.CAD = AQ_CLEAR; //向下计数时，当时基计数器TBCTR = 比较计数器CMPA，A通道为低电平。WY
	EPwm4Regs.AQCTLB.bit.CBU = AQ_CLEAR; //向上计数时，当时基计数器TBCTR =  比较计数器CMPB，B通道为低电平。WY
	EPwm4Regs.AQCTLB.bit.CBD = AQ_SET; //向下计数时，当时基计数器TBCTR =  比较计数器CMPB，B通道为高电平。WY

	EPwm5Regs.AQCTLA.bit.CAU = AQ_SET; //向上计数时，当时基计数器TBCTR = 比较计数器CMPA，A通道为高电平。WY
	EPwm5Regs.AQCTLA.bit.CAD = AQ_CLEAR; //向下计数时，当时基计数器TBCTR = 比较计数器CMPA，A通道为低电平。WY
	EPwm5Regs.AQCTLB.bit.CBU = AQ_CLEAR; //向上计数时，当时基计数器TBCTR =  比较计数器CMPB，B通道为低电平。WY
	EPwm5Regs.AQCTLB.bit.CBD = AQ_SET; //向下计数时，当时基计数器TBCTR =  比较计数器CMPB，B通道为高电平。WY

	EPwm6Regs.AQCTLA.bit.CAU = AQ_SET; //向上计数时，当时基计数器TBCTR = 比较计数器CMPA，A通道为高电平。WY
	EPwm6Regs.AQCTLA.bit.CAD = AQ_CLEAR; //向下计数时，当时基计数器TBCTR = 比较计数器CMPA，A通道为低电平。WY
	EPwm6Regs.AQCTLB.bit.CBU = AQ_CLEAR; //向上计数时，当时基计数器TBCTR =  比较计数器CMPB，B通道为低电平。WY
	EPwm6Regs.AQCTLB.bit.CBD = AQ_SET; //向下计数时，当时基计数器TBCTR =  比较计数器CMPB，B通道为高电平。WY

	EPwm7Regs.AQCTLA.bit.CAU = AQ_SET; //向上计数时，当时基计数器TBCTR = 比较计数器CMPA，A通道为高电平。WY
	EPwm7Regs.AQCTLA.bit.CAD = AQ_CLEAR; //向下计数时，当时基计数器TBCTR = 比较计数器CMPA，A通道为低电平。WY
	EPwm7Regs.AQCTLB.bit.CBU = AQ_CLEAR; //向上计数时，当时基计数器TBCTR =  比较计数器CMPB，B通道为低电平。WY
	EPwm7Regs.AQCTLB.bit.CBD = AQ_SET; //向下计数时，当时基计数器TBCTR =  比较计数器CMPB，B通道为高电平。WY

	EPwm8Regs.AQCTLA.bit.CAU = AQ_SET; //向上计数时，当时基计数器TBCTR = 比较计数器CMPA，A通道为高电平。WY
	EPwm8Regs.AQCTLA.bit.CAD = AQ_CLEAR; //向下计数时，当时基计数器TBCTR = 比较计数器CMPA，A通道为低电平。WY
	EPwm8Regs.AQCTLB.bit.CBU = AQ_CLEAR; //向上计数时，当时基计数器TBCTR =  比较计数器CMPB，B通道为低电平。WY
	EPwm8Regs.AQCTLB.bit.CBD = AQ_SET; //向下计数时，当时基计数器TBCTR =  比较计数器CMPB，B通道为高电平。WY

	EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1; //分频系数。默认情况下，EPWM模块的时钟 = 系统时钟 / 2。当系统时钟为200MHz时，EPWM模块的时钟即为100MHz。WY
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; //分频系数。WY
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; //增减模式。WY
	EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE; //不产生同步信号。WY
	EPwm2Regs.TBPHS.bit.TBPHS = 0; //相位差为0。WY
	EPwm2Regs.TBPRD = T1PR * 2; //设置计数周期。频率20KHz。WY
	EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE; //失能时基寄存器的影子寄存器模式。WY
	EPwm2Regs.TBCTL.bit.PRDLD = TB_SHADOW; //当时基计数器为零时，从影子寄存器加载周期值。WY
	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; //当时基计数器为0时，比较计数器CMPA从其影子寄存器中取值。WY
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; //使能比较计数器CMPA的影子寄存器模式。WY
	EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL; //通道A作为死区上升沿和下降沿的作用对象。WY
	EPwm2Regs.DBCTL.bit.OUT_MODE = DB_DISABLE; //失能死区功能。WY
	EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HI; //极性选择。通道A和通道B均不作极性反转。WY
	EPwm2Regs.DBFED.all = 0; //下降沿死区设置为0。WY
	EPwm2Regs.DBRED.all = 0; //上升沿死区设置为0。WY

	EPwm2Regs.ETSEL.bit.SOCAEN = 1; //PWM可以触发AD转换（SOC-A）。WY
	EPwm2Regs.ETSEL.bit.SOCASEL = ET_CTRU_CMPA; //计数器增加时，当时基计数器TBTCR = 比较计数器CMPA，产生触发AD转换的脉冲。WY
	EPwm2Regs.ETPS.bit.SOCAPRD = ET_1ST; //每次脉冲都可以触发AD转换（SOC-A）。WY
	EPwm2Regs.ETCLR.bit.SOCA = 1; //清除AD转换（SOC-A）的标志位。WY
	EPwm2Regs.CMPA.bit.CMPA = 112; //写入比较计数器的值。WY。

	/*
	 * 配置PWM1
	 * 功能说明：PWM-1-A用于风机调速
	 */
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
	EPwm1Regs.TBPHS.bit.TBPHS = 0;
	EPwm1Regs.TBCTR = 0;
	EPwm1Regs.TBPRD = FUNT1PR;
	EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;
	EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm1Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;
	EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HI;
	EPwm1Regs.DBFED.all = 0;
	EPwm1Regs.DBRED.all = 0;
	EPwm1Regs.AQSFRC.bit.RLDCSF = 3;

	EALLOW;
	CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;
}

#if THREE_LEVEL_PWM == 1

/*
 * 功能：配置EPWM相关的引脚。WY
 */
void InitEPwmGpio(void)
{
	EALLOW;

	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1; //引脚复用为EPWM-1-A。WY

	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1; //引脚复用为EPWM-3-A。WY
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;//引脚复用为EPWM-3-B。WY

	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1; //引脚复用为EPWM-4-A。WY
	GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1; //引脚复用为EPWM-4-B。WY

	GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1; //引脚复用为EPWM-5-A。WY
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1; //引脚复用为EPWM-5-B。WY

	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1; //引脚复用为EPWM-6-A。WY
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1; //引脚复用为EPWM-6-B。WY

	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 1; //引脚复用为EPWM-7-A。WY
	GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 1; //引脚复用为EPWM-7-B。WY

	GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 1; //引脚复用为EPWM-8-A。WY
	GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 1; //引脚复用为EPWM-8-B。WY

	GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0; //上拉模式。WY

	GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0; //上拉模式。WY
	GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0; //上拉模式。WY

	GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0; //上拉模式。WY
	GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0; //上拉模式。WY

	GpioCtrlRegs.GPAPUD.bit.GPIO8 = 0; //上拉模式。WY
	GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0; //上拉模式。WY

	GpioCtrlRegs.GPAPUD.bit.GPIO10 = 0; //上拉模式。WY
	GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0; //上拉模式。WY

	GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0; //上拉模式。WY
	GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0; //上拉模式。WY

	GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0; //上拉模式。WY
	GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0; //上拉模式。WY

	EDIS;
}

#elif THREE_LEVEL_PWM == 0
void InitEPwmGpio(void)
{
	EALLOW;
	// 使用PWM口 PWM1A 2A 3A ===  GPIO0 GPIO2 GPIO4
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 =1;   // EPWM4A
	GpioCtrlRegs.GPAMUX1.bit.GPIO8 =1;   // EPWM5A
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 =1;   // EPWM6A

	GpioCtrlRegs.GPAPUD.bit.GPIO6 =0;
	GpioCtrlRegs.GPAPUD.bit.GPIO8 =0;
	GpioCtrlRegs.GPAPUD.bit.GPIO10 =0;
	EDIS;
}

#else
#error "Please select the THREE_LEVEL_PWM"
#endif
//
// InitEPwmGpio - Initialize all EPWM modules' GPIOs
//
//void InitEPwmGpio(void)
//{
//    InitEPwm1Gpio();
//    InitEPwm2Gpio();
//    InitEPwm3Gpio();
//    InitEPwm4Gpio();
//    InitEPwm5Gpio();
//    InitEPwm6Gpio();
//    InitEPwm7Gpio();
//    InitEPwm8Gpio();
//    InitEPwm9Gpio();
//    InitEPwm10Gpio();
//    InitEPwm11Gpio();
//    InitEPwm12Gpio();
//}

//
// InitEPwm1Gpio - Initialize EPWM1 GPIOs
//
void InitEPwm1Gpio(void)
{
    EALLOW;

    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;    // Disable pull-up on GPIO0 (EPWM1A)
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;    // Disable pull-up on GPIO1 (EPWM1B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO145 = 1;    // Disable pull-up on GPIO145 (EPWM1A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO146 = 1;    // Disable pull-up on GPIO146 (EPWM1B)

    //
    // Configure EPWM-1 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM1 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;   // Configure GPIO0 as EPWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;   // Configure GPIO1 as EPWM1B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO145 = 1;   // Configure GPIO145 as EPWM1A
    // GpioCtrlRegs.GPEMUX2.bit.GPIO146 = 1;   // Configure GPIO0146 as EPWM1B

    EDIS;
}

//
// InitEPwm2Gpio - Initialize EPWM2 GPIOs
//
void InitEPwm2Gpio(void)
{
    EALLOW;

    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;    // Disable pull-up on GPIO2 (EPWM2A)
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;    // Disable pull-up on GPIO3 (EPWM2B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO147 = 1;    // Disable pull-up on GPIO147 (EPWM2A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO148 = 1;    // Disable pull-up on GPIO148 (EPWM2B)

    //
    // Configure EPwm-2 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM2 functional pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;   // Configure GPIO2 as EPWM2A
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;   // Configure GPIO3 as EPWM2B
   // GpioCtrlRegs.GPEMUX2.bit.GPIO147 = 1;   // Configure GPIO147 as EPWM2A
   // GpioCtrlRegs.GPEMUX2.bit.GPIO148 = 1;   // Configure GPIO148 as EPWM2B

    EDIS;
}

//
// InitEPwm3Gpio - Initialize EPWM3 GPIOs
//
void InitEPwm3Gpio(void)
{
    EALLOW;

    //
    // Disable internal pull-up for the selected output pins
    //   for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;    // Disable pull-up on GPIO4 (EPWM3A)
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;    // Disable pull-up on GPIO5 (EPWM3B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO149 = 1;    // Disable pull-up on GPIO149 (EPWM3A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO150 = 1;    // Disable pull-up on GPIO150 (EPWM3B)

    //
    // Configure EPwm-3 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM3 functional pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;   // Configure GPIO4 as EPWM3A
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;   // Configure GPIO5 as EPWM3B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO149 = 1;   // Configure GPIO149 as EPWM3A
    // GpioCtrlRegs.GPEMUX2.bit.GPIO150 = 1;   // Configure GPIO150 as EPWM3B

    EDIS;
}

//
// InitEPwm4Gpio - Initialize EPWM4 GPIOs
//
void InitEPwm4Gpio(void)
{
    EALLOW;

    //
    // Disable internal pull-up for the selected output pins
    //   for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;    // Disable pull-up on GPIO6 (EPWM4A)
    GpioCtrlRegs.GPAPUD.bit.GPIO7 = 1;    // Disable pull-up on GPIO7 (EPWM4B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO151 = 1;    // Disable pull-up on GPIO151 (EPWM4A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO152 = 1;    // Disable pull-up on GPIO152 (EPWM4B)

     //
     // Configure EPWM-4 pins using GPIO regs
     // This specifies which of the possible GPIO pins will be EPWM4 functional
     // pins.
     // Comment out other unwanted lines.
     //
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;   // Configure GPIO6 as EPWM4A
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;   // Configure GPIO7 as EPWM4B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO151 = 1;   // Configure GPIO151 as EPWM4A
    // GpioCtrlRegs.GPEMUX2.bit.GPIO152 = 1;   // Configure GPIO152 as EPWM4B

    EDIS;
}

//
// InitEPwm5Gpio - Initialize EPWM5 GPIOs
//
void InitEPwm5Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    //   for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 1;    // Disable pull-up on GPIO8 (EPWM5A)
    GpioCtrlRegs.GPAPUD.bit.GPIO9 = 1;    // Disable pull-up on GPIO9 (EPWM5B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO153 = 1;    // Disable pull-up on GPIO153 (EPWM5A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO154 = 1;    // Disable pull-up on GPIO154 (EPWM5B)

    //
    // Configure EPWM-5 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM5 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;   // Configure GPIO8 as EPWM5A
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;   // Configure GPIO9 as EPWM5B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO153 = 1;   // Configure GPIO153 as EPWM5A
    // GpioCtrlRegs.GPEMUX2.bit.GPIO154 = 1;   // Configure GPIO0154 as EPWM5B

    EDIS;
}

//
// InitEPwm6Gpio - Initialize EPWM6 GPIOs
//
void InitEPwm6Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO10 = 1;    // Disable pull-up on GPIO10 (EPWM6A)
    GpioCtrlRegs.GPAPUD.bit.GPIO11 = 1;    // Disable pull-up on GPIO11 (EPWM6B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO155 = 1;    // Disable pull-up on GPIO155 (EPWM6A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO156 = 1;    // Disable pull-up on GPIO156 (EPWM6B)

    //
    // Configure EPWM-6 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM6 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;   // Configure GPIO10 as EPWM6A
    GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;   // Configure GPIO11 as EPWM6B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO155 = 1;   // Configure GPIO155 as EPWM6A
    // GpioCtrlRegs.GPEMUX2.bit.GPIO156 = 1;   // Configure GPIO156 as EPWM6B

    EDIS;
}

//
// InitEPwm7Gpio - Initialize EPWM7 GPIOs
//
void InitEPwm7Gpio(void)
{
    EALLOW;

    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 1;    // Disable pull-up on GPIO12 (EPWM7A)
    GpioCtrlRegs.GPAPUD.bit.GPIO13 = 1;    // Disable pull-up on GPIO13 (EPWM7B)
    // GpioCtrlRegs.GPEPUD.bit.GPIO157 = 1;    // Disable pull-up on GPIO157 (EPWM7A)
    // GpioCtrlRegs.GPEPUD.bit.GPIO158 = 1;    // Disable pull-up on GPIO158 (EPWM7B)

    //
    // Configure EPWM-6 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM6 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 1;   // Configure GPIO12 as EPWM7A
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 1;   // Configure GPIO13 as EPWM7B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO157 = 1;   // Configure GPIO157 as EPWM7A
    // GpioCtrlRegs.GPEMUX2.bit.GPIO158 = 1;   // Configure GPIO158 as EPWM7B

    EDIS;
}

//
// InitEPwm8Gpio - Initialize EPWM8 GPIOs
//
void InitEPwm8Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 1;    // Disable pull-up on GPIO14 (EPWM8A)
    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 1;    // Disable pull-up on GPIO15 (EPWM8B)
//  GpioCtrlRegs.GPEPUD.bit.GPIO159 = 1;    // Disable pull-up on GPIO159 (EPWM8A)
//  GpioCtrlRegs.GPFPUD.bit.GPIO160 = 1;    // Disable pull-up on GPIO160 (EPWM8B)

     //
     // Configure EPWM-6 pins using GPIO regs
     // This specifies which of the possible GPIO pins will be EPWM6 functional
     // pins.
     // Comment out other unwanted lines.
     //
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 1;   // Configure GPIO14 as EPWM8A
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 1;   // Configure GPIO15 as EPWM8B
    // GpioCtrlRegs.GPEMUX2.bit.GPIO159 = 1;   // Configure GPIO159 as EPWM8A
    // GpioCtrlRegs.GPFMUX1.bit.GPIO160 = 1;   // Configure GPIO160 as EPWM8B

    EDIS;
}

//
// InitEPwm9Gpio - Initialize EPWM9 GPIOs
//
void InitEPwm9Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPFPUD.bit.GPIO161 = 1;    // Disable pull-up on GPIO161 (EPWM9A)
    GpioCtrlRegs.GPFPUD.bit.GPIO162 = 1;    // Disable pull-up on GPIO162 (EPWM9B)

    //
    // Configure EPWM-6 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM6 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPFMUX1.bit.GPIO161 = 1;   // Configure GPIO161 as EPWM9A
    GpioCtrlRegs.GPFMUX1.bit.GPIO162 = 1;   // Configure GPIO162 as EPWM9B

    EDIS;
}

//
// InitEPwm10Gpio - Initialize EPWM10 GPIOs
//
void InitEPwm10Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPFPUD.bit.GPIO163 = 1;    // Disable pull-up on GPIO163 (EPWM10A)
    GpioCtrlRegs.GPFPUD.bit.GPIO164 = 1;    // Disable pull-up on GPIO164 (EPWM10B)

    //
    // Configure EPWM-6 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM6 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPFMUX1.bit.GPIO163 = 1;   // Configure GPIO163 as EPWM10A
    GpioCtrlRegs.GPFMUX1.bit.GPIO164 = 1;   // Configure GPIO164 as EPWM10B

    EDIS;
}

//
// InitEPwm11Gpio - Initialize EPWM11 GPIOs
//
void InitEPwm11Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPFPUD.bit.GPIO165 = 1;    // Disable pull-up on GPIO165 (EPWM11A)
    GpioCtrlRegs.GPFPUD.bit.GPIO166 = 1;    // Disable pull-up on GPIO166 (EPWM11B)

    //
    // Configure EPWM-6 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM6 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPFMUX1.bit.GPIO165 = 1;   // Configure GPIO165 as EPWM11A
    GpioCtrlRegs.GPFMUX1.bit.GPIO166 = 1;   // Configure GPIO166 as EPWM11B

    EDIS;
}

//
// InitEPwm12Gpio - Initialize EPWM12 GPIOs
//
void InitEPwm12Gpio(void)
{
    EALLOW;
    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPFPUD.bit.GPIO167 = 1;    // Disable pull-up on GPIO167 (EPWM12A)
    GpioCtrlRegs.GPFPUD.bit.GPIO168 = 1;    // Disable pull-up on GPIO168 (EPWM12B)

    //
    // Configure EPWM-6 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM6 functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPFMUX1.bit.GPIO167 = 1;   // Configure GPIO167 as EPWM12A
    GpioCtrlRegs.GPFMUX1.bit.GPIO168 = 1;   // Configure GPIO168 as EPWM12B

    EDIS;
}
