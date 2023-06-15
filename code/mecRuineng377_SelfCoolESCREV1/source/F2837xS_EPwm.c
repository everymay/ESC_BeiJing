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
	CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;                   //停止所有的TB时钟
//	ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV = 0;             //0:系统时钟200兆  ;默认1:2分频  100兆  EPWMCLK = PLLSYSCLK/EPWMCLKDIV; //在EPWM系统寄存器中,377 CPU时钟为200兆,系统默认为2分频,即为100兆系统时钟,epwm也为100兆时钟信号
	EDIS;
	//PWM3,PWM4,A相
    EPwm3Regs.TBCTL.bit.CLKDIV      = TB_DIV1;              //时钟分频系数//TBCLK = EPWMCLK / (HSPCLKDIV x CLKDIV)  //epwm时钟信号 = 时基时钟  = 100兆HZ
    EPwm3Regs.TBCTL.bit.HSPCLKDIV   = TB_DIV1;              //TBCLK = EPWMCLK / (HSPCLKDIV x CLKDIV)
    EPwm3Regs.TBCTL.bit.CTRMODE     = TB_COUNT_UPDOWN;      // 连续增减模式   // Period = 2*TBPRD
    EPwm3Regs.TBCTL.bit.SYNCOSEL    = TB_SYNC_DISABLE;           //产生4~9模块的phase-offset脉冲基准
    EPwm3Regs.TBPHS.bit.TBPHS       = 0;
    EPwm3Regs.TBCTR                 = 0;
    EPwm3Regs.TBPRD                 = T1PR;                 //设置计数周期  //40K频率
    EPwm3Regs.TBCTL.bit.PHSEN       = TB_DISABLE;           //主动模式，TBCTR不从TBPHS重载
    EPwm3Regs.TBCTL.bit.PRDLD       = TB_SHADOW;            //计数值为零时从影子寄存器取值
    EPwm3Regs.CMPCTL.bit.LOADAMODE  = CC_CTR_ZERO;          //时基计数器等于零,重装载 周期中断
    EPwm3Regs.CMPCTL.bit.SHDWAMODE  = CC_SHADOW;            //计数比较
    EPwm3Regs.CMPCTL.bit.LOADBMODE  = CC_CTR_ZERO;          //
    EPwm3Regs.CMPCTL.bit.SHDWBMODE  = CC_SHADOW;            //
    EPwm3Regs.DBCTL.bit.IN_MODE     = DBA_ALL;              // 输入源模式选择---
    EPwm3Regs.DBCTL.bit.OUT_MODE    = DB_DISABLE;           // 输出模式选择--- DB_DISABLE,输出B,A翻转
    EPwm3Regs.DBCTL.bit.POLSEL      = DB_ACTV_HI;           // 极性选择---都不翻转
    EPwm3Regs.DBFED.all             = 0;                    // 下降沿死区设置 dead time=DBFED*TBCLK
    EPwm3Regs.DBRED.all             = 0;                    // 上升沿死区设置
    EPwm3Regs.AQSFRC.bit.RLDCSF     = 3;

	EPwm4Regs.TBCTL.bit.CLKDIV      = TB_DIV1;              //
	EPwm4Regs.TBCTL.bit.HSPCLKDIV   = TB_DIV1;  	        //
	EPwm4Regs.TBCTL.bit.CTRMODE     = TB_COUNT_UPDOWN; 	    // 连续增减模式   // Period = 2*TBPRD
	EPwm4Regs.TBCTL.bit.SYNCOSEL    = TB_SYNC_DISABLE;           //
	EPwm4Regs.TBPHS.bit.TBPHS       = 0;
	EPwm4Regs.TBCTR                 = 0;                    //时基计数器等于零
	EPwm4Regs.TBPRD                 = T1PR;   		        //设置计数周期
	EPwm4Regs.TBCTL.bit.PHSEN       = TB_DISABLE;      	    //主动模式，TBCTR不从TBPHS重载
	EPwm4Regs.TBCTL.bit.PRDLD       = TB_SHADOW;            //计数值为零时从影子寄存器取值
	EPwm4Regs.CMPCTL.bit.LOADAMODE  = CC_CTR_ZERO;          //时基计数器等于零,重装载 周期中断
	EPwm4Regs.CMPCTL.bit.SHDWAMODE  = CC_SHADOW;            //
	EPwm4Regs.CMPCTL.bit.LOADBMODE  = CC_CTR_ZERO;          //
	EPwm4Regs.CMPCTL.bit.SHDWBMODE  = CC_SHADOW;            //
	EPwm4Regs.DBCTL.bit.IN_MODE     = DBA_ALL;              //
	EPwm4Regs.DBCTL.bit.OUT_MODE    = DB_DISABLE;           // DB_DISABLE,输出B,A翻转
	EPwm4Regs.DBCTL.bit.POLSEL      = DB_ACTV_HI;           // 都不翻转
	EPwm4Regs.DBFED.all             = 0;   		            // 上升沿死区设置 dead time=DBFED*TBCLK
	EPwm4Regs.DBRED.all             = 0;   		            // 下降沿死区设置
	EPwm4Regs.AQSFRC.bit.RLDCSF     = 3;

    //PWM5,PWM6,B相
	EPwm5Regs.TBCTL.bit.CLKDIV      = TB_DIV1;
	EPwm5Regs.TBCTL.bit.HSPCLKDIV   = TB_DIV1;
	EPwm5Regs.TBCTL.bit.CTRMODE     = TB_COUNT_UPDOWN;
	EPwm5Regs.TBCTL.bit.SYNCOSEL    = TB_SYNC_DISABLE;      	    //同步信号
	EPwm5Regs.TBPHS.bit.TBPHS       = 0;                    //错开
	EPwm5Regs.TBCTR                 = 0;
	EPwm5Regs.TBPRD                 = T1PR;
	EPwm5Regs.TBCTL.bit.PHSEN       = TB_DISABLE;
	EPwm5Regs.TBCTL.bit.PRDLD       = TB_SHADOW;
	EPwm5Regs.CMPCTL.bit.LOADAMODE  = CC_CTR_ZERO;
	EPwm5Regs.CMPCTL.bit.SHDWAMODE  = CC_SHADOW;
    EPwm5Regs.CMPCTL.bit.LOADBMODE  = CC_CTR_ZERO;      // 时基计数器等于零,重装载 周期中断
    EPwm5Regs.CMPCTL.bit.SHDWBMODE  = CC_SHADOW;         //公用一个时间基准
	EPwm5Regs.DBCTL.bit.IN_MODE     = DBA_ALL;
	EPwm5Regs.DBCTL.bit.OUT_MODE    = DB_DISABLE;
	EPwm5Regs.DBCTL.bit.POLSEL      = DB_ACTV_HI;
	EPwm5Regs.DBFED.all             = 0;
	EPwm5Regs.DBRED.all             = 0;
	EPwm5Regs.AQSFRC.bit.RLDCSF     = 3;

	EPwm6Regs.TBCTL.bit.CLKDIV      = TB_DIV1;
	EPwm6Regs.TBCTL.bit.HSPCLKDIV   = TB_DIV1;
	EPwm6Regs.TBCTL.bit.CTRMODE     = TB_COUNT_UPDOWN;
	EPwm6Regs.TBCTL.bit.SYNCOSEL    = TB_SYNC_DISABLE;           //同步信号
	EPwm6Regs.TBPHS.bit.TBPHS       = 0;
	EPwm6Regs.TBCTR                 = 0;
	EPwm6Regs.TBPRD                 = T1PR;
	EPwm6Regs.TBCTL.bit.PHSEN       = TB_DISABLE;            //从动模式
	EPwm6Regs.TBCTL.bit.PRDLD       = TB_SHADOW;
	EPwm6Regs.CMPCTL.bit.LOADAMODE  = CC_CTR_ZERO;
	EPwm6Regs.CMPCTL.bit.SHDWAMODE  = CC_SHADOW;
    EPwm6Regs.CMPCTL.bit.LOADBMODE  = CC_CTR_ZERO;      //
    EPwm6Regs.CMPCTL.bit.SHDWBMODE  = CC_SHADOW;         //
	EPwm6Regs.DBCTL.bit.IN_MODE     = DBA_ALL;
	EPwm6Regs.DBCTL.bit.OUT_MODE    = DB_DISABLE;
	EPwm6Regs.DBCTL.bit.POLSEL      = DB_ACTV_HI;
	EPwm6Regs.DBFED.all             = 0;
	EPwm6Regs.DBRED.all             = 0;
	EPwm6Regs.AQSFRC.bit.RLDCSF     = 3;

//PWM7,PWM8,C相
	EPwm7Regs.TBCTL.bit.CLKDIV      = TB_DIV1;
	EPwm7Regs.TBCTL.bit.HSPCLKDIV   = TB_DIV1;
	EPwm7Regs.TBCTL.bit.CTRMODE     = TB_COUNT_UPDOWN;
	EPwm7Regs.TBCTL.bit.SYNCOSEL    = TB_SYNC_DISABLE;
	EPwm7Regs.TBPHS.bit.TBPHS       = 0;                  //实现超前于EPwm4Regs 1us产生AD采样,0.312us+0.7us(ADhold+CLA计算2相并输出),PWM时钟100M,
    EPwm7Regs.TBCTR                 = 0;
	EPwm7Regs.TBPRD                 = T1PR;
	EPwm7Regs.TBCTL.bit.PHSEN       = TB_DISABLE;
	EPwm7Regs.TBCTL.bit.PRDLD       = TB_SHADOW;
    EPwm7Regs.CMPCTL.bit.LOADAMODE  = CC_CTR_ZERO;      //改为零点更新
    EPwm7Regs.CMPCTL.bit.SHDWAMODE  = CC_SHADOW;         //立即更新
    EPwm7Regs.CMPCTL.bit.LOADBMODE  = CC_CTR_ZERO;
    EPwm7Regs.CMPCTL.bit.SHDWBMODE  = CC_SHADOW;
	EPwm7Regs.DBCTL.bit.IN_MODE     = DBA_ALL;
	EPwm7Regs.DBCTL.bit.OUT_MODE    = DB_DISABLE;
	EPwm7Regs.DBCTL.bit.POLSEL      = DB_ACTV_HI;
	EPwm7Regs.DBFED.all             = 0;
	EPwm7Regs.DBRED.all             = 0;
	EPwm7Regs.AQSFRC.bit.RLDCSF     = 3;

    EPwm8Regs.TBCTL.bit.CLKDIV      = TB_DIV1;
    EPwm8Regs.TBCTL.bit.HSPCLKDIV   = TB_DIV1;
    EPwm8Regs.TBCTL.bit.CTRMODE     = TB_COUNT_UPDOWN;
    EPwm8Regs.TBCTL.bit.SYNCOSEL    = TB_SYNC_DISABLE;
    EPwm8Regs.TBPHS.bit.TBPHS       = 0;                  //实现超前于EPwm5Regs 1us产生AD采样,0.312us+0.7us(ADhold+CLA计算2相并输出),PWM时钟100M,
    EPwm8Regs.TBCTR                 = 0;
    EPwm8Regs.TBPRD                 = T1PR;
    EPwm8Regs.TBCTL.bit.PHSEN       = TB_DISABLE;
    EPwm8Regs.TBCTL.bit.PRDLD       = TB_SHADOW;
    EPwm8Regs.CMPCTL.bit.LOADAMODE  = CC_CTR_ZERO;
    EPwm8Regs.CMPCTL.bit.SHDWAMODE  = CC_SHADOW;
    EPwm8Regs.CMPCTL.bit.LOADBMODE  = CC_CTR_ZERO;
    EPwm8Regs.CMPCTL.bit.SHDWBMODE  = CC_SHADOW;
    EPwm8Regs.DBCTL.bit.IN_MODE     = DBA_ALL;
    EPwm8Regs.DBCTL.bit.OUT_MODE    = DB_DISABLE;
    EPwm8Regs.DBCTL.bit.POLSEL      = DB_ACTV_HI;
    EPwm8Regs.DBFED.all             = 0;
    EPwm8Regs.DBRED.all             = 0;
    EPwm8Regs.AQSFRC.bit.RLDCSF     = 3;         //立即加载(活动寄存器由CPU直接访问，而不是从影子寄存器加载)。

    EPwm3Regs.AQCTLA.bit.CAU        = AQ_SET;                 //1管
    EPwm3Regs.AQCTLA.bit.CAD        = AQ_CLEAR;
    EPwm3Regs.AQCTLB.bit.CBU        = AQ_CLEAR;               //2管,
    EPwm3Regs.AQCTLB.bit.CBD        = AQ_SET;
    EPwm4Regs.AQCTLA.bit.CAU        = AQ_SET;                 //3管
    EPwm4Regs.AQCTLA.bit.CAD        = AQ_CLEAR;
    EPwm4Regs.AQCTLB.bit.CBU        = AQ_CLEAR;               //4管,
    EPwm4Regs.AQCTLB.bit.CBD        = AQ_SET;

    EPwm5Regs.AQCTLA.bit.CAU        = AQ_SET;
    EPwm5Regs.AQCTLA.bit.CAD        = AQ_CLEAR;
    EPwm5Regs.AQCTLB.bit.CBU        = AQ_CLEAR;
    EPwm5Regs.AQCTLB.bit.CBD        = AQ_SET;
    EPwm6Regs.AQCTLA.bit.CAU        = AQ_SET;
    EPwm6Regs.AQCTLA.bit.CAD        = AQ_CLEAR;
    EPwm6Regs.AQCTLB.bit.CBU        = AQ_CLEAR;
    EPwm6Regs.AQCTLB.bit.CBD        = AQ_SET;

    EPwm7Regs.AQCTLA.bit.CAU        = AQ_SET;
    EPwm7Regs.AQCTLA.bit.CAD        = AQ_CLEAR;
    EPwm7Regs.AQCTLB.bit.CBU        = AQ_CLEAR;
    EPwm7Regs.AQCTLB.bit.CBD        = AQ_SET;
    EPwm8Regs.AQCTLA.bit.CAU        = AQ_SET;
    EPwm8Regs.AQCTLA.bit.CAD        = AQ_CLEAR;
    EPwm8Regs.AQCTLB.bit.CBU        = AQ_CLEAR;
    EPwm8Regs.AQCTLB.bit.CBD        = AQ_SET;

//    EPwmRegsAQCFRCset   = EPwm7Regs.CMPCTL.all & 0x0000FFFF; //立即更新CC_IMMEDIATE  0x1
//    EPwmRegsAQCFRCclear = EPwm7Regs.CMPCTL.all & 0x0000FFAF; //零点更新CC_SHADOW     0x0 ;bits 4,6清零

////PWM9 CLA中产生提前采样脉冲
//    EPwm9Regs.TBCTL.bit.CLKDIV      = TB_DIV1;
//    EPwm9Regs.TBCTL.bit.HSPCLKDIV   = TB_DIV1;
//    EPwm9Regs.TBCTL.bit.CTRMODE     = TB_COUNT_UPDOWN;
//    EPwm9Regs.TBCTL.bit.SYNCOSEL    = TB_SYNC_DISABLE;
//    EPwm9Regs.TBPHS.bit.TBPHS       = 0;                  //实现超前于EPwm6Regs 1us产生AD采样,0.312us+0.7us(ADhold+CLA计算2相并输出),PWM时钟100M,
//    EPwm9Regs.TBCTR                 = 0;
//    EPwm9Regs.TBPRD                 = T1PR;
//    EPwm9Regs.TBCTL.bit.PHSEN       = TB_DISABLE;
//    EPwm9Regs.TBCTL.bit.PRDLD       = TB_SHADOW;
//    EPwm9Regs.CMPCTL.bit.LOADAMODE  = CC_CTR_ZERO_PRD;
//    EPwm9Regs.CMPCTL.bit.SHDWAMODE  = CC_IMMEDIATE;
//    EPwm9Regs.CMPCTL.bit.LOADBMODE  = CC_CTR_ZERO_PRD;
//    EPwm9Regs.CMPCTL.bit.SHDWBMODE  = CC_IMMEDIATE;
//    EPwm9Regs.DBCTL.bit.IN_MODE     = DBA_ALL;
//    EPwm9Regs.DBCTL.bit.OUT_MODE    = DB_DISABLE;
//    EPwm9Regs.DBCTL.bit.POLSEL      = DB_ACTV_HI;
//    EPwm9Regs.DBFED.all             = 0;
//    EPwm9Regs.DBRED.all             = 0;
//
//    EPwm9Regs.ETSEL.bit.SOCAEN      = 1;
//    EPwm9Regs.ETSEL.bit.SOCASEL     = ET_CTRU_CMPA;         //上升沿触发AD
//    EPwm9Regs.ETPS.bit.SOCAPRD      = ET_1ST;
//    EPwm9Regs.ETCLR.bit.SOCA        = 1;
//    EPwm9Regs.ETSEL.bit.SOCBEN      = 1;
//    EPwm9Regs.ETSEL.bit.SOCBSEL     = ET_CTRD_CMPB;         //下降沿触发AD
//    EPwm9Regs.ETPS.bit.SOCBPRD      = ET_1ST;
//    EPwm9Regs.ETCLR.bit.SOCB        = 1;
//    EPwm9Regs.CMPA.bit.CMPA         = 0;                    //等待更新的与EPwm6Regs.CMPA.bit.CMPA一样,因为TBCTR超前,故采样脉冲也超前产生
//    EPwm9Regs.CMPB.bit.CMPB         = T1PR-1;               //CMPA只能产生一个上计数采样脉冲,故需要CMPB产生下计数采样脉冲
//
////    EPwm9Regs.ETSEL.bit.INTEN       = 1;
////    EPwm9Regs.ETSEL.bit.INTSEL      = ET_CTRU_CMPA;
////    EPwm9Regs.ETPS.bit.INTPRD       = ET_1ST;
////    EPwm9Regs.ETCLR.bit.INT         = 1;
////    EPwm9Regs.ETSEL.bit.INTEN       = 1;
////    EPwm9Regs.ETSEL.bit.INTSEL      = ET_CTRD_CMPB;
////    EPwm9Regs.ETPS.bit.INTPRD       = ET_1ST;
////    EPwm9Regs.ETCLR.bit.INT         = 1;
//
////PWM10
//    EPwm10Regs.TBCTL.bit.CLKDIV      = TB_DIV1;
//    EPwm10Regs.TBCTL.bit.HSPCLKDIV   = TB_DIV1;
//    EPwm10Regs.TBCTL.bit.CTRMODE     = TB_COUNT_UPDOWN;
//    EPwm10Regs.TBCTL.bit.SYNCOSEL    = TB_SYNC_DISABLE;
//    EPwm10Regs.TBPHS.bit.TBPHS       = 0;                  //实现超前于EPwm4Regs 1us产生AD采样,0.312us+0.7us(ADhold+CLA计算2相并输出),PWM时钟100M,
//    EPwm10Regs.TBCTR                 = 0;
//    EPwm10Regs.TBPRD                 = T1PR;
//    EPwm10Regs.TBCTL.bit.PHSEN       = TB_DISABLE;
//    EPwm10Regs.TBCTL.bit.PRDLD       = TB_SHADOW;
//    EPwm10Regs.CMPCTL.bit.LOADAMODE  = CC_CTR_ZERO_PRD;
//    EPwm10Regs.CMPCTL.bit.SHDWAMODE  = CC_IMMEDIATE;
//    EPwm10Regs.CMPCTL.bit.LOADBMODE  = CC_CTR_ZERO_PRD;
//    EPwm10Regs.CMPCTL.bit.SHDWBMODE  = CC_IMMEDIATE;
//    EPwm10Regs.DBCTL.bit.IN_MODE     = DBA_ALL;
//    EPwm10Regs.DBCTL.bit.OUT_MODE    = DB_DISABLE;
//    EPwm10Regs.DBCTL.bit.POLSEL      = DB_ACTV_HI;
//    EPwm10Regs.DBFED.all             = 0;
//    EPwm10Regs.DBRED.all             = 0;
//
//    EPwm10Regs.ETSEL.bit.SOCAEN      = 1;
//    EPwm10Regs.ETSEL.bit.SOCASEL     = ET_CTRU_CMPA;         //上升沿触发AD
//    EPwm10Regs.ETPS.bit.SOCAPRD      = ET_1ST;
//    EPwm10Regs.ETCLR.bit.SOCA        = 1;
//    EPwm10Regs.ETSEL.bit.SOCBEN      = 1;
//    EPwm10Regs.ETSEL.bit.SOCBSEL     = ET_CTRD_CMPB;         //下降沿触发AD
//    EPwm10Regs.ETPS.bit.SOCBPRD      = ET_1ST;
//    EPwm10Regs.ETCLR.bit.SOCB        = 1;
//    EPwm10Regs.CMPA.bit.CMPA         = 0;                    //等待更新的与EPwm4Regs.CMPA.bit.CMPA一样,因为TBCTR超前,故采样脉冲也超前产生
//    EPwm10Regs.CMPB.bit.CMPB         = T1PR-1;               //CMPA只能产生一个上计数采样脉冲,故需要CMPB产生下计数采样脉冲
//
////PWM11
//    EPwm11Regs.TBCTL.bit.CLKDIV      = TB_DIV1;
//    EPwm11Regs.TBCTL.bit.HSPCLKDIV   = TB_DIV1;
//    EPwm11Regs.TBCTL.bit.CTRMODE     = TB_COUNT_UPDOWN;
//    EPwm11Regs.TBCTL.bit.SYNCOSEL    = TB_SYNC_DISABLE;
//    EPwm11Regs.TBPHS.bit.TBPHS       = 0;                  //实现超前于EPwm5Regs 1us产生AD采样,0.312us+0.7us(ADhold+CLA计算2相并输出),PWM时钟100M,
//    EPwm11Regs.TBCTR                 = 0;
//    EPwm11Regs.TBPRD                 = T1PR;
//    EPwm11Regs.TBCTL.bit.PHSEN       = TB_DISABLE;
//    EPwm11Regs.TBCTL.bit.PRDLD       = TB_SHADOW;
//    EPwm11Regs.CMPCTL.bit.LOADAMODE  = CC_CTR_ZERO_PRD;
//    EPwm11Regs.CMPCTL.bit.SHDWAMODE  = CC_IMMEDIATE;
//    EPwm11Regs.CMPCTL.bit.LOADBMODE  = CC_CTR_ZERO_PRD;
//    EPwm11Regs.CMPCTL.bit.SHDWBMODE  = CC_IMMEDIATE;
//    EPwm11Regs.DBCTL.bit.IN_MODE     = DBA_ALL;
//    EPwm11Regs.DBCTL.bit.OUT_MODE    = DB_DISABLE;
//    EPwm11Regs.DBCTL.bit.POLSEL      = DB_ACTV_HI;
//    EPwm11Regs.DBFED.all             = 0;
//    EPwm11Regs.DBRED.all             = 0;
//
//    EPwm11Regs.ETSEL.bit.SOCAEN      = 1;
//    EPwm11Regs.ETSEL.bit.SOCASEL     = ET_CTRU_CMPA;         //上升沿触发AD
//    EPwm11Regs.ETPS.bit.SOCAPRD      = ET_1ST;
//    EPwm11Regs.ETCLR.bit.SOCA        = 1;
//    EPwm11Regs.ETSEL.bit.SOCBEN      = 1;
//    EPwm11Regs.ETSEL.bit.SOCBSEL     = ET_CTRD_CMPB;         //下降沿触发AD
//    EPwm11Regs.ETPS.bit.SOCBPRD      = ET_1ST;
//    EPwm11Regs.ETCLR.bit.SOCB        = 1;
//    EPwm11Regs.CMPA.bit.CMPA         = 0;                    //等待更新的与EPwm5Regs.CMPA.bit.CMPA一样,因为TBCTR超前,故采样脉冲也超前产生
//    EPwm11Regs.CMPB.bit.CMPB         = T1PR-1;               //CMPA只能产生一个上计数采样脉冲,故需要CMPB产生下计数采样脉冲

////PWM12与PWM9联用
//    EPwm12Regs.TBCTL.bit.CLKDIV      = TB_DIV1;
//    EPwm12Regs.TBCTL.bit.HSPCLKDIV   = TB_DIV1;
//    EPwm12Regs.TBCTL.bit.CTRMODE     = TB_COUNT_UPDOWN;
//    EPwm12Regs.TBCTL.bit.SYNCOSEL    = TB_SYNC_DISABLE;
//    EPwm12Regs.TBPHS.bit.TBPHS       = 0;                  //实现超前于EPwm6Regs 1us产生AD采样,0.312us+0.7us(ADhold+CLA计算2相并输出),PWM时钟100M,
//    EPwm12Regs.TBCTR                 = 0;
//    EPwm12Regs.TBPRD                 = T1PR;
//    EPwm12Regs.TBCTL.bit.PHSEN       = TB_DISABLE;
//    EPwm12Regs.TBCTL.bit.PRDLD       = TB_SHADOW;
//    EPwm12Regs.CMPCTL.bit.LOADAMODE  = CC_CTR_ZERO_PRD;
//    EPwm12Regs.CMPCTL.bit.SHDWAMODE  = CC_IMMEDIATE;
//    EPwm12Regs.CMPCTL.bit.LOADBMODE  = CC_CTR_ZERO_PRD;
//    EPwm12Regs.CMPCTL.bit.SHDWBMODE  = CC_IMMEDIATE;
//    EPwm12Regs.DBCTL.bit.IN_MODE     = DBA_ALL;
//    EPwm12Regs.DBCTL.bit.OUT_MODE    = DB_DISABLE;
//    EPwm12Regs.DBCTL.bit.POLSEL      = DB_ACTV_HI;
//    EPwm12Regs.DBFED.all             = 0;
//    EPwm12Regs.DBRED.all             = 0;
//
//    EPwm12Regs.ETSEL.bit.SOCAEN      = 1;
//    EPwm12Regs.ETSEL.bit.SOCASEL     = ET_CTRU_CMPA;         //上升沿触发AD
//    EPwm12Regs.ETPS.bit.SOCAPRD      = ET_1ST;
//    EPwm12Regs.ETCLR.bit.SOCA        = 1;
//    EPwm12Regs.ETSEL.bit.SOCBEN      = 1;
//    EPwm12Regs.ETSEL.bit.SOCBSEL     = ET_CTRD_CMPB;         //下降沿触发AD
//    EPwm12Regs.ETPS.bit.SOCBPRD      = ET_1ST;
//    EPwm12Regs.ETCLR.bit.SOCB        = 1;
//    EPwm12Regs.CMPA.bit.CMPA         = 0;                    //等待更新的与EPwm6Regs.CMPA.bit.CMPA一样,因为TBCTR超前,故采样脉冲也超前产生
//    EPwm12Regs.CMPB.bit.CMPB         = T1PR-1;               //CMPA只能产生一个上计数采样脉冲,故需要CMPB产生下计数采样脉冲

//PWM2A,产生AD采样脉冲,及电流中点的采样中断
	EPwm2Regs.TBCTL.bit.CLKDIV      = TB_DIV1;
	EPwm2Regs.TBCTL.bit.HSPCLKDIV   = TB_DIV1;
	EPwm2Regs.TBCTL.bit.CTRMODE     = TB_COUNT_UPDOWN;
	EPwm2Regs.TBCTL.bit.SYNCOSEL    = TB_SYNC_DISABLE;
	EPwm2Regs.TBPHS.bit.TBPHS       = 0;
	EPwm2Regs.TBPRD                 = T1PR*2;          //20k频率
	EPwm2Regs.TBCTL.bit.PHSEN       = TB_DISABLE;
	EPwm2Regs.TBCTL.bit.PRDLD       = TB_SHADOW;
	EPwm2Regs.CMPCTL.bit.LOADAMODE  = CC_CTR_ZERO;
	EPwm2Regs.CMPCTL.bit.SHDWAMODE  = CC_SHADOW;
//	EPwm2Regs.AQCTLA.bit.CAU        = AQ_SET;
//	EPwm2Regs.AQCTLA.bit.CAD        = AQ_CLEAR;
	EPwm2Regs.DBCTL.bit.IN_MODE     = DBA_ALL;
	EPwm2Regs.DBCTL.bit.OUT_MODE    = DB_DISABLE;
	EPwm2Regs.DBCTL.bit.POLSEL      = DB_ACTV_HI;
	EPwm2Regs.DBFED.all             = 0;
	EPwm2Regs.DBRED.all             = 0;

	EPwm2Regs.ETSEL.bit.SOCAEN      = 1;
	EPwm2Regs.ETSEL.bit.SOCASEL     = ET_CTRU_CMPA;
	EPwm2Regs.ETPS.bit.SOCAPRD      = ET_1ST;
	EPwm2Regs.ETCLR.bit.SOCA        = 1;
    EPwm2Regs.CMPA.bit.CMPA         = 112;                  //产生电压,温度等采样脉冲   //调整电流中点AD采样值  矫正零点附近采样
//	EPwm2Regs.ETSEL.bit.INTEN       = 1;
//	EPwm2Regs.ETSEL.bit.INTSEL      = ET_CTR_PRDZERO;       //产生中断

//PWM1A,产生风机调速占空比
    EPwm1Regs.TBCTL.bit.CLKDIV      = TB_DIV1;
    EPwm1Regs.TBCTL.bit.HSPCLKDIV   = TB_DIV1;
    EPwm1Regs.TBCTL.bit.CTRMODE     = TB_COUNT_UPDOWN;
    EPwm1Regs.TBCTL.bit.SYNCOSEL    = TB_SYNC_DISABLE;
    EPwm1Regs.TBPHS.bit.TBPHS       = 0;
    EPwm1Regs.TBCTR                 = 0;
    EPwm1Regs.TBPRD                 = FUNT1PR;          //50k频率
    EPwm1Regs.TBCTL.bit.PHSEN       = TB_DISABLE;
    EPwm1Regs.TBCTL.bit.PRDLD       = TB_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE  = CC_CTR_ZERO;
    EPwm1Regs.CMPCTL.bit.SHDWAMODE  = CC_SHADOW;
    EPwm1Regs.AQCTLA.bit.CAU        = AQ_SET;
    EPwm1Regs.AQCTLA.bit.CAD        = AQ_CLEAR;
    EPwm1Regs.DBCTL.bit.IN_MODE     = DBA_ALL;
    EPwm1Regs.DBCTL.bit.OUT_MODE    = DB_DISABLE;
    EPwm1Regs.DBCTL.bit.POLSEL      = DB_ACTV_HI;
    EPwm1Regs.DBFED.all             = 0;
    EPwm1Regs.DBRED.all             = 0;
    EPwm1Regs.AQSFRC.bit.RLDCSF     = 3;         //立即加载(活动寄存器由CPU直接访问，而不是从影子寄存器加载)。

//    EPwm1Regs.ETSEL.bit.SOCAEN      = 1;
//    EPwm1Regs.ETSEL.bit.SOCASEL     = ET_CTRU_CMPA;
//    EPwm1Regs.ETPS.bit.SOCAPRD      = ET_1ST;
//    EPwm1Regs.ETCLR.bit.SOCA        = 1;
//    EPwm1Regs.CMPA.bit.CMPA         = 112;                  //产生电压,温度等采样脉冲   //调整电流中点AD采样值  矫正零点附近采样
//    EPwm1Regs.ETSEL.bit.INTEN       = 1;
//    EPwm1Regs.ETSEL.bit.INTSEL      = ET_CTR_PRDZERO;       //产生中断

//EPwm2 CLA的AD触发
//	EPwm2Regs.TBCTL.bit.CLKDIV      = TB_DIV1;
//	EPwm2Regs.TBCTL.bit.HSPCLKDIV   = TB_DIV1;
//	EPwm2Regs.TBCTL.bit.CTRMODE     = TB_COUNT_UP;
//	EPwm2Regs.TBCTL.bit.SYNCOSEL    = TB_SYNC_IN;           //同步信号
//	EPwm2Regs.TBPHS.bit.TBPHS       =  0;
//	EPwm2Regs.TBPRD                 = T1PR;                 //16k*2(3125) 12.8k*2(3906) 1.28M(78.125)
//	EPwm2Regs.TBCTL.bit.PHSEN       = TB_ENABLE;            //从动模式
//	EPwm2Regs.TBCTL.bit.PRDLD       = TB_SHADOW;
//	EPwm2Regs.CMPCTL.bit.LOADAMODE  = CC_CTR_ZERO;
//	EPwm2Regs.CMPCTL.bit.SHDWAMODE  = CC_IMMEDIATE;
//	EPwm2Regs.AQCTLA.bit.CAU        = AQ_SET;               // 输出取反了，要反电平
//	EPwm2Regs.AQCTLA.bit.CAD        = AQ_CLEAR;             // 输出取反了，要反电平
//	EPwm2Regs.DBCTL.bit.IN_MODE     = DBA_ALL;
//	EPwm2Regs.DBCTL.bit.OUT_MODE    = DB_DISABLE;
//	EPwm2Regs.DBCTL.bit.POLSEL      = DB_ACTV_HI;
//	EPwm2Regs.DBFED.all             = 1;
//	EPwm2Regs.DBRED.all             = 1;
//
//	EPwm2Regs.ETSEL.bit.SOCAEN      = 1;
//	EPwm2Regs.ETSEL.bit.SOCASEL     = 4;                    //Enable event time-base counter equal to CMPA when the timer	is incrementing
//	EPwm2Regs.ETPS.bit.SOCAPRD      = ET_1ST;
//	EPwm2Regs.ETCLR.bit.SOCA        = 1;

	EALLOW;
	CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;
}

#if THREE_LEVEL_PWM == 1
void InitEPwmGpio(void)
{
	EALLOW;
	// 使用PWM口 PWM4A 5A 6A ===  GPIO6 GPIO8 GPIO10

	GpioCtrlRegs.GPAMUX1.bit.GPIO0 =1;    // EPWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 =1;    // EPWM3A
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 =1;    // EPWM3B
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 =1;    // EPWM4A
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 =1;    // EPWM4B
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 =1;    // EPWM5A
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 =1;    // EPWM5B
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 =1;   // EPWM6A
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 =1;   // EPWM6B
	GpioCtrlRegs.GPAMUX1.bit.GPIO12 =1;   // EPWM7A
	GpioCtrlRegs.GPAMUX1.bit.GPIO13 =1;   // EPWM7B
	GpioCtrlRegs.GPAMUX1.bit.GPIO14 =1;   // EPWM8A
	GpioCtrlRegs.GPAMUX1.bit.GPIO15 =1;   // EPWM8B

//    GpioCtrlRegs.GPAGMUX1.bit.GPIO4  = 0;    // EPWM3A
//    GpioCtrlRegs.GPAGMUX1.bit.GPIO5  = 0;    // EPWM3B
//    GpioCtrlRegs.GPAGMUX1.bit.GPIO6  = 0;   // EPWM4A
//    GpioCtrlRegs.GPAGMUX1.bit.GPIO7  = 0;   // EPWM4B
//    GpioCtrlRegs.GPAGMUX1.bit.GPIO8  = 0;   // EPWM5A
//    GpioCtrlRegs.GPAGMUX1.bit.GPIO9  = 0;   // EPWM5B
//    GpioCtrlRegs.GPAGMUX1.bit.GPIO10 = 0;   // EPWM6A
//    GpioCtrlRegs.GPAGMUX1.bit.GPIO11 = 0;   // EPWM6B
//    GpioCtrlRegs.GPAGMUX1.bit.GPIO12 = 0;   // EPWM7A
//    GpioCtrlRegs.GPAGMUX1.bit.GPIO13 = 0;   // EPWM7B
//    GpioCtrlRegs.GPAGMUX1.bit.GPIO14 = 0;   // EPWM8A
//    GpioCtrlRegs.GPAGMUX1.bit.GPIO15 = 0;   // EPWM8B

	GpioCtrlRegs.GPAPUD.bit.GPIO0 =0;
	GpioCtrlRegs.GPAPUD.bit.GPIO4 =0;
	GpioCtrlRegs.GPAPUD.bit.GPIO5 =0;
	GpioCtrlRegs.GPAPUD.bit.GPIO6 =0;
	GpioCtrlRegs.GPAPUD.bit.GPIO7 =0;
	GpioCtrlRegs.GPAPUD.bit.GPIO8 =0;
	GpioCtrlRegs.GPAPUD.bit.GPIO9 =0;
    GpioCtrlRegs.GPAPUD.bit.GPIO10 =0;
    GpioCtrlRegs.GPAPUD.bit.GPIO11 =0;
    GpioCtrlRegs.GPAPUD.bit.GPIO12 =0;
    GpioCtrlRegs.GPAPUD.bit.GPIO13 =0;
    GpioCtrlRegs.GPAPUD.bit.GPIO14 =0;
    GpioCtrlRegs.GPAPUD.bit.GPIO15 =0;

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
