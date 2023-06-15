//###########################################################################
//
// FILE:   F2837xS_SysCtrl.c
//
// TITLE:  F2837xS Device System Control Initialization & Support Functions.
//
// DESCRIPTION:
//
//         Example initialization of system resources.
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
#include "CoreCtrl.h"
#ifdef __cplusplus
using std::memcpy;
#endif

#define STATUS_FAIL          0
#define STATUS_SUCCESS       1
#define TMR1SYSCLKCTR        0xF0000000
#define TMR2INPCLKCTR        0x800

//#pragma CODE_SECTION(Epwm1t2, "ram2func");
//#pragma CODE_SECTION(InitFlash_Bank1, "ramfuncs");

// ---------------------内存拷贝程序----------------------------------//
// Uint16 *SourceAddr        Pointer to the first word to be moved
//                           SourceAddr < SourceEndAddr
// Uint16* SourceEndAddr     Pointer to the last word to be moved
// Uint16* DestAddr          Pointer to the first destination word
void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr)
{
    while(SourceAddr < SourceEndAddr)
    {
       *DestAddr++ = *SourceAddr++;
    }
    return;
}

#define ADC_usDELAY  10000L
#define ADC_usDELAY2 40L
void InitAdc(void)
{
    extern void DSP28x_usDelay(Uint32 Count);

    // ADC校验，通过boot ROM自动调用ADC—cal来初始化ADCREFSEL和ADCOFFTRIM寄存器
    // 运行中校验自动完成，不需要操作。
    // 若禁止boot ROM，则需要手动配置这两个寄存器的初始化。
    EALLOW;
    CpuSysRegs.PCLKCR13.bit.ADC_A= 1;
    CpuSysRegs.PCLKCR13.bit.ADC_B= 1;
    CpuSysRegs.PCLKCR13.bit.ADC_C= 1;
    CpuSysRegs.PCLKCR13.bit.ADC_D= 1;
    //	ADC_Cal();
    EDIS;
    DELAY_US(ADC_usDELAY);
    EALLOW;
    /*┌───────────────────────────────────────────────┬──────────────────────────────────────────┬───────────────────────────────────────────────
     *│ADCA,ADCC,ADCB,的14,15通道,用做电流续流方向判断,│ 14通道                                                                 │ 15通道
     *│其触发源是                                                                   │ 11h BURSTTRIG17 - ePWM7, ADCSOCA          │ 12h BURSTTRIG18 - ePWM7, ADCSOCB
     *│配置为                                                                          │ AdcaRegs.ADCSOC14CTL.bit.TRIGSEL=0x11     │ AdcaRegs.ADCSOC15CTL.bit.TRIGSEL=0x12,
     *│转换完成后                                                                   │ EOC14 is trigger for ADCINT1              │ EOC15 is trigger for ADCINT2
     *│转换完成后                                                                   │ AdcaRegs.ADCINTSEL1N2.bit.INT1SEL=0xE     │ AdcaRegs.ADCINTSEL1N2.bit.INT2SEL=0xF
     *│触发CLA1,处理A相的增计数时的续流方向                      │ DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK1 = CLA_TRIG_ADCAINT1;
     *│触发CLA2,处理A相的减计数时的续流方向                      │                                           │ DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK2 = CLA_TRIG_ADCAINT2;
     *│其他                                                                             │                                           │
     *└───────────────────────────────────────────────┴──────────────────────────────────────────┴────────────────────────────────────────────
     */
    //ADCA,ADCC,ADCB,的其他通道
    /*************************************ADCA**************************************/
    AdcaRegs.ADCCTL2.bit.SIGNALMODE=0;
    AdcaRegs.ADCCTL2.bit.RESOLUTION=0;
    AdcaRegs.ADCCTL2.bit.PRESCALE = 0x6;
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ=1;

    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL=0x7;    //07h ADCTRIG7 - ePWM2, ADCSOCA   EPWM2触发AD中断源
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL=0x7;    //同上
    AdcaRegs.ADCSOC2CTL.bit.TRIGSEL=0x7;    //同上
//    AdcaRegs.ADCSOC3CTL.bit.TRIGSEL=0x7;    //同上
    AdcaRegs.ADCSOC4CTL.bit.TRIGSEL=0x7;    //同上
    AdcaRegs.ADCSOC5CTL.bit.TRIGSEL=0x7;    //同上

//    AdcaRegs.ADCSOC8CTL.bit.TRIGSEL =0x15;  //15h BURSTTRIG21 - ePWM9, ADCSOCA   //CLA
//    AdcaRegs.ADCSOC10CTL.bit.TRIGSEL=0x16;  //16h BURSTTRIG22 - ePWM9, ADCSOCB
//    AdcaRegs.ADCSOC13CTL.bit.TRIGSEL=0x15;  //15h BURSTTRIG21 - ePWM9, ADCSOCA
//    AdcaRegs.ADCSOC15CTL.bit.TRIGSEL=0x16;  //16h BURSTTRIG22 - ePWM9, ADCSOCB

    AdcaRegs.ADCSOC0CTL.bit.CHSEL=2;        //IRO,ESC A相主电抗器输出电流(ISA)
    AdcaRegs.ADCSOC0CTL.bit.ACQPS=0x18;
    AdcaRegs.ADCSOC1CTL.bit.CHSEL=5;        //IBO,ESC B相直流电容电压 (IOV)
    AdcaRegs.ADCSOC1CTL.bit.ACQPS=0x18;
    AdcaRegs.ADCSOC2CTL.bit.CHSEL=15;       //UC,A相高压侧电网电压AUH
    AdcaRegs.ADCSOC2CTL.bit.ACQPS=0x18;
//    AdcaRegs.ADCSOC3CTL.bit.CHSEL=1;        //LMT87DCK,DSP温度
//    AdcaRegs.ADCSOC3CTL.bit.ACQPS=0x18;
    AdcaRegs.ADCSOC4CTL.bit.CHSEL=4;        //IAS1,ESC C相低压侧电压CUL
    AdcaRegs.ADCSOC4CTL.bit.ACQPS=0x18;
    AdcaRegs.ADCSOC5CTL.bit.CHSEL=0;        //UU,ESC A相旁路电流 ISU
    AdcaRegs.ADCSOC5CTL.bit.ACQPS=0x18;

//    AdcaRegs.ADCSOC8CTL.bit.CHSEL =2;       //选择A2通道采A相主电抗电流,用来做预判算法
//    AdcaRegs.ADCSOC8CTL.bit.ACQPS =0x30;    //采样脉宽长度
//    AdcaRegs.ADCSOC10CTL.bit.CHSEL=2;       //ditto
//    AdcaRegs.ADCSOC10CTL.bit.ACQPS=0x30;
//    AdcaRegs.ADCSOC12CTL.bit.CHSEL=2;       //ditto
//    AdcaRegs.ADCSOC12CTL.bit.ACQPS=0x30;
//    AdcaRegs.ADCSOC14CTL.bit.CHSEL=2;       //ditto
//    AdcaRegs.ADCSOC14CTL.bit.ACQPS=0x30;
//    AdcaRegs.ADCSOC13CTL.bit.CHSEL=2;       //ditto
//    AdcaRegs.ADCSOC13CTL.bit.ACQPS=0x30;
//    AdcaRegs.ADCSOC15CTL.bit.CHSEL=2;       //ditto
//    AdcaRegs.ADCSOC15CTL.bit.ACQPS=0x30;

//    AdcaRegs.ADCINTSEL1N2.bit.INT1E=1;      //中断1使能
//    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL=0xD;  //EOC13 is trigger for ADCINT1
//    AdcaRegs.ADCINTSEL1N2.bit.INT1CONT = 1; //
//    AdcaRegs.ADCINTSEL1N2.bit.INT2E=1;      //中断2使能
//    AdcaRegs.ADCINTSEL1N2.bit.INT2SEL=0xF;  //EOC15 is trigger for ADCINT2
//    AdcaRegs.ADCINTSEL1N2.bit.INT2CONT = 1;
//    AdcaRegs.ADCINTFLGCLR.all = 0x000F;
//    AdcaRegs.ADCOFFTRIM.bit.OFFTRIM=0;

    /*************************************ADCB**************************************/

    AdccRegs.ADCCTL2.bit.SIGNALMODE=0;
    AdccRegs.ADCCTL2.bit.RESOLUTION=0;
    AdccRegs.ADCCTL2.bit.PRESCALE = 0x6;
    AdccRegs.ADCCTL1.bit.ADCPWDNZ=1;

    AdccRegs.ADCSOC0CTL.bit.TRIGSEL=0x7;    //07h ADCTRIG7 - ePWM2, ADCSOCA   EPWM2触发AD中断源
    AdccRegs.ADCSOC1CTL.bit.TRIGSEL=0x7;    //ditto
    AdccRegs.ADCSOC2CTL.bit.TRIGSEL=0x7;    //ditto
    AdccRegs.ADCSOC3CTL.bit.TRIGSEL=0x7;    //ditto

//    AdccRegs.ADCSOC4CTL.bit.TRIGSEL=0x13;  //13h BURSTTRIG19 - ePWM8, ADCSOCA
//    AdccRegs.ADCSOC6CTL.bit.TRIGSEL=0x13;  //ditto
//    AdccRegs.ADCSOC8CTL.bit.TRIGSEL=0x13;
//    AdccRegs.ADCSOC10CTL.bit.TRIGSEL=0x13;  //13h BURSTTRIG19 - ePWM8, ADCSOCA
//    AdccRegs.ADCSOC11CTL.bit.TRIGSEL=0x13;  //ditto

//    AdccRegs.ADCSOC8CTL.bit.TRIGSEL =0x17;  //17h BURSTTRIG23 - ePWM10, ADCSOCA
//    AdccRegs.ADCSOC10CTL.bit.TRIGSEL=0x18;  //18h BURSTTRIG24 - ePWM10, ADCSOCB
//    AdccRegs.ADCSOC13CTL.bit.TRIGSEL=0x17;  //17h BURSTTRIG23 - ePWM10, ADCSOCA
//    AdccRegs.ADCSOC15CTL.bit.TRIGSEL=0x18;  //18h BURSTTRIG24 - ePWM10, ADCSOCB

    AdccRegs.ADCSOC0CTL.bit.CHSEL=3;        //C3    ISO,ESC B相主电抗器输出电流(ISB)
    AdccRegs.ADCSOC0CTL.bit.ACQPS=0x18;
    AdccRegs.ADCSOC1CTL.bit.CHSEL=4;        //C4    ICO,ESC C相直流电容电压IOW
    AdccRegs.ADCSOC1CTL.bit.ACQPS=0x18;
    AdccRegs.ADCSOC2CTL.bit.CHSEL=5;        //C5    TEMP0,ESC测试温度T3
    AdccRegs.ADCSOC2CTL.bit.ACQPS=0x18;
    AdccRegs.ADCSOC3CTL.bit.CHSEL=2;        //C2    IBS1,ESC C相高压侧电压CUH
    AdccRegs.ADCSOC3CTL.bit.ACQPS=0x18;



//    AdccRegs.ADCSOC8CTL.bit.CHSEL =3;        //C3
//    AdccRegs.ADCSOC8CTL.bit.ACQPS =0x30;
//    AdccRegs.ADCSOC10CTL.bit.CHSEL=3;       //ditto
//    AdccRegs.ADCSOC10CTL.bit.ACQPS=0x30;
//    AdccRegs.ADCSOC12CTL.bit.CHSEL=3;       //ditto
//    AdccRegs.ADCSOC12CTL.bit.ACQPS=0x30;
//    AdccRegs.ADCSOC14CTL.bit.CHSEL=3;       //ditto
//    AdccRegs.ADCSOC14CTL.bit.ACQPS=0x30;
//    AdccRegs.ADCSOC13CTL.bit.CHSEL=3;       //ditto
//    AdccRegs.ADCSOC13CTL.bit.ACQPS=0x30;
//    AdccRegs.ADCSOC15CTL.bit.CHSEL=3;       //ditto
//    AdccRegs.ADCSOC15CTL.bit.ACQPS=0x30;

//    AdccRegs.ADCINTSEL1N2.bit.INT1E=1;      //中断1使能
//    AdccRegs.ADCINTSEL1N2.bit.INT1SEL=0xD;  //EOC13 is trigger for ADCINT1
//    AdccRegs.ADCINTSEL1N2.bit.INT1CONT = 1; //
//    AdccRegs.ADCINTSEL1N2.bit.INT2E=1;      //中断2使能
//    AdccRegs.ADCINTSEL1N2.bit.INT2SEL=0xF;  //EOC15 is trigger for ADCINT2
//    AdccRegs.ADCINTSEL1N2.bit.INT2CONT = 1;
//    AdccRegs.ADCINTFLGCLR.all = 0x000F;
//    AdccRegs.ADCOFFTRIM.bit.OFFTRIM=0;


//    AdccRegs.ADCCTL2.bit.SIGNALMODE=0;
//    AdccRegs.ADCCTL2.bit.RESOLUTION=0;
//    AdccRegs.ADCCTL2.bit.PRESCALE = 0x6;
//    AdccRegs.ADCCTL1.bit.ADCPWDNZ=1;
//
//    AdccRegs.ADCSOC0CTL.bit.TRIGSEL=0x5;    //05h BURSTTRIG5 - ePWM1, ADCSOCA
//    AdccRegs.ADCSOC1CTL.bit.TRIGSEL=0x5;    //同上
//    AdccRegs.ADCSOC2CTL.bit.TRIGSEL=0x5;    //同上
//    AdccRegs.ADCSOC3CTL.bit.TRIGSEL=0x5;    //同上
//
//    AdccRegs.ADCSOC14CTL.bit.TRIGSEL=0x13;  //13h BURSTTRIG19 - ePWM8, ADCSOCA
//    AdccRegs.ADCSOC15CTL.bit.TRIGSEL=0x14;  //14h BURSTTRIG20 - ePWM8, ADCSOCB
//
//    AdccRegs.ADCSOC0CTL.bit.CHSEL=2;        //C2    IBS1,电网电流B
//    AdccRegs.ADCSOC0CTL.bit.ACQPS=0x18;
//    AdccRegs.ADCSOC1CTL.bit.CHSEL=4;        //C4    ICO,输出电压C
//    AdccRegs.ADCSOC1CTL.bit.ACQPS=0x18;
//    AdccRegs.ADCSOC2CTL.bit.CHSEL=5;        //C5    TEMP1,散热片UV温度
//    AdccRegs.ADCSOC2CTL.bit.ACQPS=0x18;
//    AdccRegs.ADCSOC3CTL.bit.CHSEL=3;        //C3    ISO,输出电流B,不用
//    AdccRegs.ADCSOC3CTL.bit.ACQPS=0x18;
//
//    AdccRegs.ADCSOC14CTL.bit.CHSEL=2;       //C2    IBS1,电网电流B
//    AdccRegs.ADCSOC14CTL.bit.ACQPS=0x18;
//    AdccRegs.ADCSOC15CTL.bit.CHSEL=2;       //C2    IBS1,电网电流B
//    AdccRegs.ADCSOC15CTL.bit.ACQPS=0x18;
//
//    AdccRegs.ADCINTSEL1N2.bit.INT1E=1;      //中断1使能
//    AdccRegs.ADCINTSEL1N2.bit.INT1SEL=0xE;  //EOC14 is trigger for ADCINT1
//    AdccRegs.ADCINTSEL1N2.bit.INT1CONT = 1; //
//    AdccRegs.ADCINTSEL1N2.bit.INT2E=1;      //中断2使能
//    AdccRegs.ADCINTSEL1N2.bit.INT2SEL=0xF;  //EOC15 is trigger for ADCINT2
//    AdccRegs.ADCINTSEL1N2.bit.INT2CONT = 1;
//    AdccRegs.ADCINTFLGCLR.all = 0x000F;
//    AdccRegs.ADCOFFTRIM.bit.OFFTRIM=0;

    /*************************************ADCC**************************************/
    AdcbRegs.ADCCTL2.bit.SIGNALMODE=0;
    AdcbRegs.ADCCTL2.bit.RESOLUTION=0;
    AdcbRegs.ADCCTL2.bit.PRESCALE = 0x6;
    AdcbRegs.ADCCTL1.bit.ADCPWDNZ=1;

    AdcbRegs.ADCSOC0CTL.bit.TRIGSEL=0x7;    //07h ADCTRIG7 - ePWM2, ADCSOCA   EPWM2触发AD中断源
    AdcbRegs.ADCSOC1CTL.bit.TRIGSEL=0x7;    //同上
    AdcbRegs.ADCSOC2CTL.bit.TRIGSEL=0x7;    //同上
    AdcbRegs.ADCSOC3CTL.bit.TRIGSEL=0x7;    //同上

//    AdcbRegs.ADCSOC8CTL.bit.TRIGSEL =0x19;  //19h BURSTTRIG25 - ePWM11, ADCSOCA
//    AdcbRegs.ADCSOC10CTL.bit.TRIGSEL=0x1A;  //1Ah BURSTTRIG26 - ePWM11, ADCSOCB
//    AdcbRegs.ADCSOC12CTL.bit.TRIGSEL=0x19;  //19h BURSTTRIG25 - ePWM11, ADCSOCA
//    AdcbRegs.ADCSOC14CTL.bit.TRIGSEL=0x1A;  //1Ah BURSTTRIG26 - ePWM11, ADCSOCB


    AdcbRegs.ADCSOC0CTL.bit.CHSEL=5;        //ICS1,ESC B相低压侧电压BUL
    AdcbRegs.ADCSOC0CTL.bit.ACQPS=0x18;
    AdcbRegs.ADCSOC1CTL.bit.CHSEL=4;        //UA,ESC B相高压侧电压BUH
    AdcbRegs.ADCSOC1CTL.bit.ACQPS=0x18;
    AdcbRegs.ADCSOC2CTL.bit.CHSEL=2;        //UV,ESC B相旁路电流 ISV
    AdcbRegs.ADCSOC2CTL.bit.ACQPS=0x18;
    AdcbRegs.ADCSOC3CTL.bit.CHSEL=1;        //DCUP,ESC 温度测试T1
    AdcbRegs.ADCSOC3CTL.bit.ACQPS=0x18;
//  AdcbRegs.ADCSOC13CTL.bit.CHSEL=3;       //DCUN,不用
//  AdcbRegs.ADCSOC13CTL.bit.ACQPS=0x18;

//    AdcbRegs.ADCSOC8CTL.bit.CHSEL=2;       //
//    AdcbRegs.ADCSOC8CTL.bit.ACQPS=0x30;    //
//    AdcbRegs.ADCSOC10CTL.bit.CHSEL=2;       //ditto
//    AdcbRegs.ADCSOC10CTL.bit.ACQPS=0x30;
//    AdcbRegs.ADCSOC12CTL.bit.CHSEL=2;       //ditto
//    AdcbRegs.ADCSOC12CTL.bit.ACQPS=0x30;
//    AdcbRegs.ADCSOC14CTL.bit.CHSEL=2;       //ditto
//    AdcbRegs.ADCSOC14CTL.bit.ACQPS=0x30;
//    AdcbRegs.ADCSOC13CTL.bit.CHSEL=2;       //ditto
//    AdcbRegs.ADCSOC13CTL.bit.ACQPS=0x30;
//    AdcbRegs.ADCSOC15CTL.bit.CHSEL=2;       //ditto
//    AdcbRegs.ADCSOC15CTL.bit.ACQPS=0x30;

//    AdcbRegs.ADCINTSEL1N2.bit.INT1E=1;      //中断1使能
//    AdcbRegs.ADCINTSEL1N2.bit.INT1SEL=0xD;  //EOC13 is trigger for ADCINT1
//    AdcbRegs.ADCINTSEL1N2.bit.INT1CONT = 1; //
//    AdcbRegs.ADCINTSEL1N2.bit.INT2E=1;      //中断2使能
//    AdcbRegs.ADCINTSEL1N2.bit.INT2SEL=0xF;  //EOC15 is trigger for ADCINT2
//    AdcbRegs.ADCINTSEL1N2.bit.INT2CONT = 1;

    AdcbRegs.ADCINTFLGCLR.all = 0x000F;
    AdcbRegs.ADCOFFTRIM.bit.OFFTRIM=0;



    /*************************************ADCD**************************************/
    //ADCD用做普通AD采样产生EOC脉冲,触发ADCA1INT中断
    AdcdRegs.ADCCTL2.bit.SIGNALMODE=0;
    AdcdRegs.ADCCTL2.bit.RESOLUTION=0;
    AdcdRegs.ADCCTL2.bit.PRESCALE = 0x6;
    AdcdRegs.ADCCTL1.bit.ADCPWDNZ=1;

    AdcdRegs.ADCSOC0CTL.bit.TRIGSEL=0x7;    //07h ADCTRIG7 - ePWM2, ADCSOCA   EPWM2触发AD中断源
    AdcdRegs.ADCSOC1CTL.bit.TRIGSEL=0x7;    //同上
    AdcdRegs.ADCSOC2CTL.bit.TRIGSEL=0x7;    //同上
    AdcdRegs.ADCSOC3CTL.bit.TRIGSEL=0x7;    //同上
    AdcdRegs.ADCSOC4CTL.bit.TRIGSEL=0x7;    //同上

//    AdcdRegs.ADCSOC8CTL.bit.TRIGSEL =0x19;  //19h BURSTTRIG25 - ePWM11, ADCSOCA
//    AdcdRegs.ADCSOC10CTL.bit.TRIGSEL=0x1A;  //1Ah BURSTTRIG26 - ePWM11, ADCSOCB
//    AdcbRegs.ADCSOC13CTL.bit.TRIGSEL=0x19;  //19h BURSTTRIG25 - ePWM11, ADCSOCA
//    AdcbRegs.ADCSOC15CTL.bit.TRIGSEL=0x1A;  //1Ah BURSTTRIG26 - ePWM11, ADCSOCB

    AdcdRegs.ADCSOC0CTL.bit.CHSEL=5;        //D5    ITO,ESC C相主电抗器输出电流(ISC)
    AdcdRegs.ADCSOC0CTL.bit.ACQPS=0x18;
    AdcdRegs.ADCSOC1CTL.bit.CHSEL=1;        //D1    UB,ESC A相低压侧电压AUL
    AdcdRegs.ADCSOC1CTL.bit.ACQPS=0x18;
    AdcdRegs.ADCSOC2CTL.bit.CHSEL=2;        //D2    TEMP1,ESC测试温度T2
    AdcdRegs.ADCSOC2CTL.bit.ACQPS=0x18;
    AdcdRegs.ADCSOC3CTL.bit.CHSEL=3;        //D3    IAO,ESC A相直流电容电压 (IOU)
    AdcdRegs.ADCSOC3CTL.bit.ACQPS=0x18;
    AdcdRegs.ADCSOC4CTL.bit.CHSEL=4;        //D4    UW,ESC C相旁路电流
    AdcdRegs.ADCSOC4CTL.bit.ACQPS=0x18;

//    AdcdRegs.ADCSOC8CTL.bit.CHSEL =5;       //  D5
//    AdcdRegs.ADCSOC8CTL.bit.ACQPS =0x30;    //
//    AdcdRegs.ADCSOC10CTL.bit.CHSEL=5;       //ditto
//    AdcdRegs.ADCSOC10CTL.bit.ACQPS=0x30;

//    AdcdRegs.ADCINTSEL1N2.bit.INT1E=1;      //中断1使能
//    AdcdRegs.ADCINTSEL1N2.bit.INT1SEL=0xD;  //EOC13 is trigger for ADCINT1
//    AdcdRegs.ADCINTSEL1N2.bit.INT1CONT = 1; //
//    AdcdRegs.ADCINTSEL1N2.bit.INT2E=1;      //中断2使能
//    AdcdRegs.ADCINTSEL1N2.bit.INT2SEL=0xF;  //EOC15 is trigger for ADCINT2
//    AdcdRegs.ADCINTSEL1N2.bit.INT2CONT = 1;

    AdcdRegs.ADCINTSEL1N2.bit.INT1E=1;      //中断1使能
    AdcdRegs.ADCINTSEL1N2.bit.INT1SEL=0x0;  //EOC0 is trigger for ADCINT1
    AdcdRegs.ADCINTSEL1N2.bit.INT1CONT = 1; //
    AdcdRegs.ADCINTFLGCLR.all = 0x000F;
    AdcdRegs.ADCOFFTRIM.bit.OFFTRIM=0;
    EDIS;
}


#if (TNMD150A200415REV1)
void InitGpio_NewBoard(void)
{
    EALLOW;
    //IO Input
    GpioCtrlRegs.GPCDIR.bit.GPIO77   = 0;//启动
    GpioCtrlRegs.GPCPUD.bit.GPIO77   = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO78   = 0;//停机
    GpioCtrlRegs.GPCPUD.bit.GPIO78   = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO79  = 0;//防雷
    GpioCtrlRegs.GPCPUD.bit.GPIO79  = 0;
//    GpioCtrlRegs.GPCDIR.bit.GPIO79   = 0;//24V控制电掉电检测
//    GpioCtrlRegs.GPCPUD.bit.GPIO79   = 0;
    GpioCtrlRegs.GPDDIR.bit.GPIO97   = 0;//IGBT故障反馈输入 ERRIN1
    GpioCtrlRegs.GPDPUD.bit.GPIO97   = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO94   = 0;//IGBT故障反馈输入 ERRIN2  未使用
    GpioCtrlRegs.GPCPUD.bit.GPIO94   = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO27   = 0;//直流硬件过压输入1,2
    GpioCtrlRegs.GPAPUD.bit.GPIO27   = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO72   = 0;//地址0
    GpioCtrlRegs.GPCPUD.bit.GPIO72   = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO84   = 0;//地址1
    GpioCtrlRegs.GPCPUD.bit.GPIO84   = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO85   = 0;//地址2
    GpioCtrlRegs.GPCPUD.bit.GPIO85   = 0;

    GpioCtrlRegs.GPEDIR.bit.GPIO150  = 0;
    GpioCtrlRegs.GPEPUD.bit.GPIO150  = 0;

    //IO Output
    GpioCtrlRegs.GPBDIR.bit.GPIO59  = 1;//板载运行灯
    GpioCtrlRegs.GPBPUD.bit.GPIO59  = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO60  = 1;//板载故障灯
    GpioCtrlRegs.GPBPUD.bit.GPIO60  = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO61  = 1;//板载同步灯
    GpioCtrlRegs.GPBPUD.bit.GPIO61  = 0;
//    GpioDataRegs.GPEDAT.bit.GPIO156  = 1;
    GpioCtrlRegs.GPEDIR.bit.GPIO156  = 1;//接触器控制
    GpioCtrlRegs.GPEPUD.bit.GPIO156  = 0;
//    GpioDataRegs.GPFDAT.bit.GPIO163  = 1;
    GpioCtrlRegs.GPFDIR.bit.GPIO163  = 1;//接触器输出 KMCON2
    GpioCtrlRegs.GPFPUD.bit.GPIO163  = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO95   = 1;//PWM使能信号端口 PWMEN1
    GpioCtrlRegs.GPCPUD.bit.GPIO95   = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO93   = 0;//PWM使能信号端口 PWMEN2 未使用
    GpioCtrlRegs.GPCPUD.bit.GPIO93   = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO42  = 1;//FAN1_ON-板载风机1
    GpioCtrlRegs.GPBPUD.bit.GPIO42  = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO43  = 1;//FNA2_ON-板载风机2
    GpioCtrlRegs.GPBPUD.bit.GPIO43  = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO153  = 1;//外部风机
    GpioCtrlRegs.GPEPUD.bit.GPIO153  = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO154  = 1;//外部故障灯
    GpioCtrlRegs.GPEPUD.bit.GPIO154  = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO155  = 1;//外部运行灯
    GpioCtrlRegs.GPEPUD.bit.GPIO155  = 0;
    GpioCtrlRegs.GPDDIR.bit.GPIO110  = 1;//PULSE,心跳脉冲输出
    GpioCtrlRegs.GPDPUD.bit.GPIO110  = 0;

    GpioCtrlRegs.GPADIR.bit.GPIO4   = 1;//PWM3A
    GpioCtrlRegs.GPAPUD.bit.GPIO4   = 0;
    GpioCtrlRegs.GPFDIR.bit.GPIO164  = 1;
    GpioCtrlRegs.GPFPUD.bit.GPIO164  = 0;

    GpioCtrlRegs.GPCDIR.bit.GPIO91   = 1;//调试引脚1
    GpioCtrlRegs.GPCPUD.bit.GPIO91   = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO92   = 1;//调试引脚2
    GpioCtrlRegs.GPCPUD.bit.GPIO92   = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO76   = 1;//调试引脚3
    GpioCtrlRegs.GPCPUD.bit.GPIO76   = 0;
#if TEST_RUNTIME
    GpioCtrlRegs.GPECSEL4.bit.GPIO157 = 1;
    GpioCtrlRegs.GPECSEL4.bit.GPIO158 = 1;
    GpioCtrlRegs.GPEDIR.bit.GPIO157   = 1;//CLA调试引脚4
    GpioCtrlRegs.GPEPUD.bit.GPIO157   = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO158   = 1;//CLA调试引脚5
    GpioCtrlRegs.GPEPUD.bit.GPIO158   = 0;
#endif
    GpioCtrlRegs.GPEDIR.bit.GPIO151  = 0;//开出 备用
    GpioCtrlRegs.GPEPUD.bit.GPIO151  = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO152  = 0;//开出 备用
    GpioCtrlRegs.GPEPUD.bit.GPIO152  = 0;
    //多机同步输出
    GpioCtrlRegs.GPADIR.bit.GPIO14   = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO14   = 0;
    //多机同步输入
    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;   // Enable pull-up on GPIO9 (CAP3)
    GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 0; // Synch to SYSCLKOUT GPIO9 (CAP3)
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;  // Configure GPIO15 as CAP3//28377没有cap功能
    EDIS;
}
#elif(NPC3CA10020210330REV1)
void InitGpio_NewBoard(void)
{
    EALLOW;
    //IO Input
    GpioCtrlRegs.GPCDIR.bit.GPIO91   = 0;   //防雷反馈
    GpioCtrlRegs.GPCPUD.bit.GPIO91   = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO1    = 0;   //15V控制电掉电检测
    GpioCtrlRegs.GPAPUD.bit.GPIO1    = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO157  = 0;   //急停
    GpioCtrlRegs.GPEPUD.bit.GPIO157  = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO92   = 0;   //旁路微断反馈
    GpioCtrlRegs.GPCPUD.bit.GPIO92   = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO94   = 0; //过流检测引脚,A
    GpioCtrlRegs.GPCPUD.bit.GPIO94   = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO85   = 0; //过流检测引脚,B
    GpioCtrlRegs.GPCPUD.bit.GPIO85   = 0;
    GpioCtrlRegs.GPDDIR.bit.GPIO97   = 0; //过流检测引脚,C
    GpioCtrlRegs.GPDPUD.bit.GPIO97   = 0;

//    GpioCtrlRegs.GPCDIR.bit.GPIO91   = 1;   //临时测试GPIO
//    GpioCtrlRegs.GPCPUD.bit.GPIO91   = 0;
//    GpioCtrlRegs.GPCDIR.bit.GPIO92   = 1;   //临时测试GPIO
//    GpioCtrlRegs.GPCPUD.bit.GPIO92   = 0;

    //IO Output
    GpioCtrlRegs.GPDDIR.bit.GPIO110  = 1;   //PULSE,心跳脉冲输出
    GpioCtrlRegs.GPDPUD.bit.GPIO110  = 0;
    GpioCtrlRegs.GPFDIR.bit.GPIO163  = 1;   //磁保持继电器控制 分
    GpioCtrlRegs.GPFPUD.bit.GPIO163  = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO79   = 1;   //A相高/低压磁保持继电器
    GpioCtrlRegs.GPCPUD.bit.GPIO79   = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO78   = 1;   //A相旁路磁保持继电器
    GpioCtrlRegs.GPCPUD.bit.GPIO78   = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO151  = 1;   //B相高/低压磁保持继电器
    GpioCtrlRegs.GPEPUD.bit.GPIO151  = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO150  = 1;   //B相旁路磁保持继电器
    GpioCtrlRegs.GPEPUD.bit.GPIO150  = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO77   = 1;   //C相高/低压磁保持继电器
    GpioCtrlRegs.GPCPUD.bit.GPIO77   = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO152  = 1;   //C相旁路磁保持继电器
    GpioCtrlRegs.GPEPUD.bit.GPIO152  = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO154  = 1;   //SCRA_EN
    GpioCtrlRegs.GPEPUD.bit.GPIO154  = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO153  = 1;   //SCRB_EN
    GpioCtrlRegs.GPEPUD.bit.GPIO153  = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO155  = 1;   //SCRC_EN
    GpioCtrlRegs.GPEPUD.bit.GPIO155  = 0;
//    GpioCtrlRegs.GPCDIR.bit.GPIO95   = 1;   //PWM使能信号端口  PWMEN1  //NPC3CA 10020 220711未用
//    GpioCtrlRegs.GPCPUD.bit.GPIO95   = 0;
    GpioCtrlRegs.GPEDIR.bit.GPIO156  = 1;   //A相PWM使能信号端口
    GpioCtrlRegs.GPEPUD.bit.GPIO156  = 0;
    GpioCtrlRegs.GPFDIR.bit.GPIO164  = 1;   //B相PWM使能信号端口
    GpioCtrlRegs.GPFPUD.bit.GPIO164  = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO93   = 1;   //C相PWM使能信号端口   PWMEN2
    GpioCtrlRegs.GPCPUD.bit.GPIO93   = 0;
    GpioCtrlRegs.GPDDIR.bit.GPIO105  = 1;   //A相小继电器控制
    GpioCtrlRegs.GPDPUD.bit.GPIO105  = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO76   = 1;   //B相小继电器控制
    GpioCtrlRegs.GPCPUD.bit.GPIO76   = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO43   = 1;   //C相小继电器控制
    GpioCtrlRegs.GPBPUD.bit.GPIO43   = 0;

    GpioCtrlRegs.GPEDIR.bit.GPIO158  = 1;   //外部运行灯
    GpioCtrlRegs.GPEPUD.bit.GPIO158  = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO42   = 1;   //故障指示灯
    GpioCtrlRegs.GPBPUD.bit.GPIO42   = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO59   = 1;    //板载运行灯
    GpioCtrlRegs.GPBPUD.bit.GPIO59   = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO60   = 1;    //板载故障灯
    GpioCtrlRegs.GPBPUD.bit.GPIO60   = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO61   = 1;    //板载同步灯
    GpioCtrlRegs.GPBPUD.bit.GPIO61   = 0;
    GpioCtrlRegs.GPDDIR.bit.GPIO104  = 1;    //控制15V电源供电引脚
    GpioCtrlRegs.GPDPUD.bit.GPIO104  = 0;

//    GpioCtrlRegs.GPCDIR.bit.GPIO91   = 1;   //防雷反馈
//    GpioCtrlRegs.GPCPUD.bit.GPIO91   = 0;


    GpioDataRegs.GPFSET.bit.GPIO163  = 1;    //(置高电平)
    GpioDataRegs.GPCSET.bit.GPIO79   = 1;
    GpioDataRegs.GPCSET.bit.GPIO78   = 1;
    GpioDataRegs.GPESET.bit.GPIO151  = 1;
    GpioDataRegs.GPESET.bit.GPIO150  = 1;
    GpioDataRegs.GPCSET.bit.GPIO77   = 1;
    GpioDataRegs.GPESET.bit.GPIO152  = 1;
    GpioDataRegs.GPDSET.bit.GPIO104  = 1;

    EDIS;
}


#endif

//
// Functions that will be run from RAM need to be assigned to a different
// section.  This section will then be mapped to a load and run address using
// the linker cmd file.
//
//      *IMPORTANT*
//
//  IF RUNNING FROM FLASH, PLEASE COPY OVER THE SECTION ".TI.ramfunc" FROM
//  FLASH TO RAM PRIOR TO CALLING InitSysCtrl(). THIS PREVENTS THE MCU FROM
//  THROWING AN EXCEPTION WHEN A CALL TO DELAY_US() IS MADE.
//
#ifndef __cplusplus
    #ifdef __TI_COMPILER_VERSION__
        #if __TI_COMPILER_VERSION__ >= 15009000
            #pragma CODE_SECTION(InitFlash_Bank0, ".TI.ramfunc");
            #pragma CODE_SECTION(InitFlash_Bank1, ".TI.ramfunc");
            #pragma CODE_SECTION(FlashOff_Bank0, ".TI.ramfunc");
            #pragma CODE_SECTION(FlashOff_Bank1, ".TI.ramfunc");
        #else
            #pragma CODE_SECTION(InitFlash_Bank0, "ramfuncs");
            #pragma CODE_SECTION(InitFlash_Bank1, "ramfuncs");
            #pragma CODE_SECTION(FlashOff_Bank0, "ramfuncs");
            #pragma CODE_SECTION(FlashOff_Bank1, "ramfuncs");
        #endif
    #endif
#endif

//
// InitSysCtrl - Initialization of system resources.
//
void InitSysCtrl(void)
{
    //
    // Disable the watchdog
    //
    DisableDog();

#ifdef _FLASH
    //
    // Copy time critical code and Flash setup code to RAM. This includes the
    // following functions: InitFlash()
    //
    // The  RamfuncsLoadStart, RamfuncsLoadSize, and RamfuncsRunStart
    // symbols are created by the linker. Refer to the device .cmd file.
    //
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);

    //
    // Call Flash Initialization to setup flash waitstates. This function must
    // reside in RAM.
    //
    InitFlash_Bank0();
#endif

    //
    //      *IMPORTANT*
    //
    // The Device_cal function, which copies the ADC & oscillator calibration
    // values from TI reserved OTP into the appropriate trim registers, occurs
    // automatically in the Boot ROM. If the boot ROM code is bypassed during
    // the debug process, the following function MUST be called for the ADC and
    // oscillators to function according to specification. The clocks to the
    // ADC MUST be enabled before calling this function.
    //
    // See the device data manual and/or the ADC Reference Manual for more
    // information.
    //
    EALLOW;

    //
    // Enable pull-ups on unbonded IOs as soon as possible to reduce power
    // consumption.
    //
    //GPIO_EnableUnbondedIOPullups();

    CpuSysRegs.PCLKCR13.bit.ADC_A = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_B = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_C = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_D = 1;

    //
    // Check if device is trimmed
    //
    if(*((Uint16 *)0x5D1B6) == 0x0000){
        //
        // Device is not trimmed--apply static calibration values
        //
        AnalogSubsysRegs.ANAREFTRIMA.all = 31709;
        AnalogSubsysRegs.ANAREFTRIMB.all = 31709;
        AnalogSubsysRegs.ANAREFTRIMC.all = 31709;
        AnalogSubsysRegs.ANAREFTRIMD.all = 31709;
    }

    CpuSysRegs.PCLKCR13.bit.ADC_A = 0;
    CpuSysRegs.PCLKCR13.bit.ADC_B = 0;
    CpuSysRegs.PCLKCR13.bit.ADC_C = 0;
    CpuSysRegs.PCLKCR13.bit.ADC_D = 0;
    EDIS;

    //
    // Initialize the PLL control: SYSPLLMULT and SYSCLKDIVSEL.
    //
    // Defined options to be passed as arguments to this function are defined
    // in F2837xS_Examples.h.
    //
    // Note: The internal oscillator CANNOT be used as the PLL source if the
    // PLLSYSCLK is configured to frequencies above 194 MHz.
    //
    //  PLLSYSCLK = (XTAL_OSC) * (IMULT + FMULT) / (PLLSYSCLKDIV)
    //
#ifdef _LAUNCHXL_F28377S
    InitSysPll(XTAL_OSC,IMULT_40,FMULT_0,PLLCLK_BY_2);
#else
    InitSysPll(XTAL_OSC,IMULT_20,FMULT_0,PLLCLK_BY_2);
#endif

    //
    // Turn on all peripherals
    //
    InitPeripheralClocks();
}

//
// InitPeripheralClocks - Initializes the clocks for the peripherals.
//
// Note: In order to reduce power consumption, turn off the clocks to any
// peripheral that is not specified for your part-number or is not used in the
// application
//
void InitPeripheralClocks(void)
{
//    000,LSPCLK = SYSCLK / 1
//    001,LSPCLK = SYSCLK / 2
//    010,LSPCLK = SYSCLK / 4 (default on reset)
    EALLOW;
    ClkCfgRegs.LOSPCP.bit.LSPCLKDIV=0x0;

    CpuSysRegs.PCLKCR0.bit.CLA1 = 1;
    CpuSysRegs.PCLKCR0.bit.DMA = 1;
    CpuSysRegs.PCLKCR0.bit.CPUTIMER0 = 1;
    CpuSysRegs.PCLKCR0.bit.CPUTIMER1 = 1;
    CpuSysRegs.PCLKCR0.bit.CPUTIMER2 = 1;
    CpuSysRegs.PCLKCR0.bit.HRPWM = 1;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;

    CpuSysRegs.PCLKCR1.bit.EMIF1 = 1;
    CpuSysRegs.PCLKCR1.bit.EMIF2 = 1;

    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM2 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM3 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM4 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM5 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM6 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM7 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM8 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM9 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM10 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM11 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM12 = 1;

    CpuSysRegs.PCLKCR3.bit.ECAP1 = 1;
    CpuSysRegs.PCLKCR3.bit.ECAP2 = 1;
    CpuSysRegs.PCLKCR3.bit.ECAP3 = 1;
    CpuSysRegs.PCLKCR3.bit.ECAP4 = 1;
    CpuSysRegs.PCLKCR3.bit.ECAP5 = 1;
    CpuSysRegs.PCLKCR3.bit.ECAP6 = 1;

    CpuSysRegs.PCLKCR4.bit.EQEP1 = 1;
    CpuSysRegs.PCLKCR4.bit.EQEP2 = 1;
    CpuSysRegs.PCLKCR4.bit.EQEP3 = 1;

    CpuSysRegs.PCLKCR6.bit.SD1 = 1;
    CpuSysRegs.PCLKCR6.bit.SD2 = 1;

    CpuSysRegs.PCLKCR7.bit.SCI_A = 1;
    CpuSysRegs.PCLKCR7.bit.SCI_B = 1;
    CpuSysRegs.PCLKCR7.bit.SCI_C = 1;
    CpuSysRegs.PCLKCR7.bit.SCI_D = 1;

    CpuSysRegs.PCLKCR8.bit.SPI_A = 1;
    CpuSysRegs.PCLKCR8.bit.SPI_B = 1;
    CpuSysRegs.PCLKCR8.bit.SPI_C = 1;

    CpuSysRegs.PCLKCR9.bit.I2C_A = 1;
    CpuSysRegs.PCLKCR9.bit.I2C_B = 1;

    CpuSysRegs.PCLKCR10.bit.CAN_A = 1;
    CpuSysRegs.PCLKCR10.bit.CAN_B = 1;

    CpuSysRegs.PCLKCR11.bit.McBSP_A = 1;
    CpuSysRegs.PCLKCR11.bit.McBSP_B = 1;
    CpuSysRegs.PCLKCR11.bit.USB_A = 1;

    CpuSysRegs.PCLKCR12.bit.uPP_A = 1;

    CpuSysRegs.PCLKCR13.bit.ADC_A = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_B = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_C = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_D = 1;

    CpuSysRegs.PCLKCR14.bit.CMPSS1 = 1;
    CpuSysRegs.PCLKCR14.bit.CMPSS2 = 1;
    CpuSysRegs.PCLKCR14.bit.CMPSS3 = 1;
    CpuSysRegs.PCLKCR14.bit.CMPSS4 = 1;
    CpuSysRegs.PCLKCR14.bit.CMPSS5 = 1;
    CpuSysRegs.PCLKCR14.bit.CMPSS6 = 1;
    CpuSysRegs.PCLKCR14.bit.CMPSS7 = 1;
    CpuSysRegs.PCLKCR14.bit.CMPSS8 = 1;

    CpuSysRegs.PCLKCR16.bit.DAC_A = 1;
    CpuSysRegs.PCLKCR16.bit.DAC_B = 1;
    CpuSysRegs.PCLKCR16.bit.DAC_C = 1;

    EDIS;
}

//
// DisablePeripheralClocks - Gates-off all peripheral clocks.
//
void DisablePeripheralClocks(void)
{
    EALLOW;

    CpuSysRegs.PCLKCR0.all = 0;
    CpuSysRegs.PCLKCR1.all = 0;
    CpuSysRegs.PCLKCR2.all = 0;
    CpuSysRegs.PCLKCR3.all = 0;
    CpuSysRegs.PCLKCR4.all = 0;
    CpuSysRegs.PCLKCR6.all = 0;
    CpuSysRegs.PCLKCR7.all = 0;
    CpuSysRegs.PCLKCR8.all = 0;
    CpuSysRegs.PCLKCR9.all = 0;
    CpuSysRegs.PCLKCR10.all = 0;
    CpuSysRegs.PCLKCR11.all = 0;
    CpuSysRegs.PCLKCR12.all = 0;
    CpuSysRegs.PCLKCR13.all = 0;
    CpuSysRegs.PCLKCR14.all = 0;
    CpuSysRegs.PCLKCR16.all = 0;

    EDIS;
}

//
// InitFlash_Bank0 - This function initializes the Flash Control registers for
//                   Bank 0.
//
//      *CAUTION*
// This function MUST be executed out of RAM. Executing it out of OTP/Flash
// will yield unpredictable results.
//
#ifdef __cplusplus
    #ifdef __TI_COMPILER_VERSION__
        #if __TI_COMPILER_VERSION__ >= 15009000
            #pragma CODE_SECTION(".TI.ramfunc");
        #else
            #pragma CODE_SECTION("ramfuncs");
        #endif
    #endif
#endif
void InitFlash_Bank0(void)
{
    EALLOW;

    //
    // The default value of VREADST is good enough for the flash to power up
    // properly at the INTOSC frequency. Below VREADST configuration covers up
    // to the max frequency possible for this device. This is required for
    // proper flash wake up at the higher frequencies if users put it to sleep
    // for power saving reason.
    //
    Flash0CtrlRegs.FBAC.bit.VREADST = 0x14;

    //
    // At reset bank and pump are in sleep. A Flash access will power up the
    // bank and pump automatically.
    //
    // After a Flash access, bank and pump go to low power mode (configurable
    // in FBFALLBACK/FPAC1 registers) if there is no further access to flash.
    //
    // Power up Flash bank and pump. This also sets the fall back mode of
    // flash and pump as active.
    //
    Flash0CtrlRegs.FPAC1.bit.PMPPWR = 0x1;
    Flash0CtrlRegs.FBFALLBACK.bit.BNKPWR0 = 0x3;

    //
    // Disable Cache and prefetch mechanism before changing wait states
    //
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 0;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 0;

    //
    // Set waitstates according to frequency
    //
    //      *CAUTION*
    // Minimum waitstates required for the flash operating at a given CPU rate
    // must be characterized by TI. Refer to the datasheet for the latest
    // information.
    //
    #if CPU_FRQ_200MHZ
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x3;
    #endif

    #if CPU_FRQ_150MHZ
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x2;
    #endif

    #if CPU_FRQ_120MHZ
    Flash0CtrlRegs.FRDCNTL.bit.RWAIT = 0x2;
    #endif

    //
    // Enable Cache and prefetch mechanism to improve performance of code
    // executed from Flash.
    //
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 1;
    Flash0CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 1;

    //
    // At reset, ECC is enabled. If it is disabled by application software and
    // if application again wants to enable ECC.
    //
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;

    EDIS;

    //
    // Force a pipeline flush to ensure that the write to the last register
    // configured occurs before returning.
    //
    __asm(" RPT #7 || NOP");
}

//
// InitFlash - This function initializes the Flash Control registers for
//             Bank 1.
//
//      *CAUTION*
// This function MUST be executed out of RAM. Executing it out of OTP/Flash
// will yield unpredictable results.
//
#ifdef __cplusplus
    #ifdef __TI_COMPILER_VERSION__
        #if __TI_COMPILER_VERSION__ >= 15009000
            #pragma CODE_SECTION(".TI.ramfunc");
        #else
            #pragma CODE_SECTION("ramfuncs");
        #endif
    #endif
#endif
void InitFlash_Bank1(void)
{
    EALLOW;

    //
    // Set VREADST to the proper value for the flash banks to power up
    // properly. This sets the bank power up delay.
    //
    Flash1CtrlRegs.FBAC.bit.VREADST = 0x14;

    //
    // At reset bank and pump are in sleep. A Flash access will power up the
    // bank and pump automatically.
    //
    // After a Flash access, bank and pump go to low power mode (configurable
    // in FBFALLBACK/FPAC1 registers) if there is no further access to flash.
    //
    // Power up Flash bank and pump. This also sets the fall back mode of
    // flash and pump as active.
    //
    Flash1CtrlRegs.FPAC1.bit.PMPPWR = 0x1;
    Flash1CtrlRegs.FBFALLBACK.bit.BNKPWR0 = 0x3;

    //
    // Disable Cache and prefetch mechanism before changing wait states
    //
    Flash1CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 0;
    Flash1CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 0;

    //
    // Set waitstates according to frequency
    //
    //      *CAUTION*
    // Minimum waitstates required for the flash operating at a given CPU rate
    // must be characterized by TI. Refer to the datasheet for the latest
    // information.
    //
    #if CPU_FRQ_200MHZ
    Flash1CtrlRegs.FRDCNTL.bit.RWAIT = 0x3;
    #endif

    #if CPU_FRQ_150MHZ
    Flash1CtrlRegs.FRDCNTL.bit.RWAIT = 0x2;
    #endif

    #if CPU_FRQ_120MHZ
    Flash1CtrlRegs.FRDCNTL.bit.RWAIT = 0x2;
    #endif

    //
    // Enable Cache and prefetch mechanism to improve performance of code
    // executed from Flash.
    //
    Flash1CtrlRegs.FRD_INTF_CTRL.bit.DATA_CACHE_EN = 1;
    Flash1CtrlRegs.FRD_INTF_CTRL.bit.PREFETCH_EN = 1;

    //
    // At reset, ECC is enabled. If it is disabled by application software and
    // if application again wants to enable ECC.
    //
    Flash1EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;

    EDIS;

    //
    // Force a pipeline flush to ensure that the write to the last register
    // configured occurs before returning.
    //
    __asm(" RPT #7 || NOP");
}

//
// FlashOff_Bank0 - This function powers down the flash
//
//      *CAUTION*
// This function MUST be executed out of RAM. Executing it out of OTP/Flash
// will yield unpredictable results. Also you must seize the flash pump in
// order to power it down.
//
#ifdef __cplusplus
    #ifdef __TI_COMPILER_VERSION__
        #if __TI_COMPILER_VERSION__ >= 15009000
            #pragma CODE_SECTION(".TI.ramfunc");
        #else
            #pragma CODE_SECTION("ramfuncs");
        #endif
    #endif
#endif
void FlashOff_Bank0(void)
{
    EALLOW;

    //
    // Set VREADST to the proper value for the flash banks to power up properly
    //
    Flash0CtrlRegs.FBAC.bit.VREADST = 0x14;

    //
    // Power down bank
    //
    Flash0CtrlRegs.FBFALLBACK.bit.BNKPWR0 = 0;

    //
    // Power down pump
    //
    Flash0CtrlRegs.FPAC1.bit.PMPPWR = 0;

    EDIS;
}

//
// FlashOff_Bank1 - This function powers down the flash
//
//      *CAUTION*
// This function MUST be executed out of RAM. Executing it out of OTP/Flash
// will yield unpredictable results. Also you must seize the flash pump in
// order to power it down.
//
#ifdef __cplusplus
    #ifdef __TI_COMPILER_VERSION__
        #if __TI_COMPILER_VERSION__ >= 15009000
            #pragma CODE_SECTION(".TI.ramfunc");
        #else
            #pragma CODE_SECTION("ramfuncs");
        #endif
    #endif
#endif
void FlashOff_Bank1(void)
{
    EALLOW;

    //
    // Set VREADST to the proper value for the flash banks to power up properly
    //
    Flash1CtrlRegs.FBAC.bit.VREADST = 0x14;

    //
    // Power down bank
    //
    Flash1CtrlRegs.FBFALLBACK.bit.BNKPWR0 = 0;

    //
    // Power down pump
    //
    Flash1CtrlRegs.FPAC1.bit.PMPPWR = 0;

    EDIS;
}

//
// SeizeFlashPump_Bank0 - Wait until the flash pump for Bank 0 is available.
//                        Then take control of it using the flash pump
//                        Semaphore.
//
void SeizeFlashPump_Bank0(void)
{
    EALLOW;

    while (FlashPumpSemaphoreRegs.PUMPREQUEST.bit.PUMP_OWNERSHIP != 0x2)
    {
        FlashPumpSemaphoreRegs.PUMPREQUEST.all = IPC_PUMP_KEY | 0x2;
    }

    EDIS;
}

//
// SeizeFlashPump_Bank1 - Wait until the flash pump for Bank 1 is available.
//                        Then take control of it using the flash pump
//                        Semaphore.
//
void SeizeFlashPump_Bank1(void)
{
    EALLOW;

    while(FlashPumpSemaphoreRegs.PUMPREQUEST.bit.PUMP_OWNERSHIP != 0x1)
    {
        FlashPumpSemaphoreRegs.PUMPREQUEST.all = IPC_PUMP_KEY | 0x1;
    }

    EDIS;
}

//
// ReleaseFlashPump - Release control of the flash pump using the flash pump
//                    semaphore.
//
void ReleaseFlashPump(void)
{
    EALLOW;
    FlashPumpSemaphoreRegs.PUMPREQUEST.all = IPC_PUMP_KEY | 0x0;
    EDIS;
}

//
// ServiceDog - This function resets the watchdog timer.
//
// Enable this function for using ServiceDog in the application.
//
void ServiceDog(void)
{
    EALLOW;
    WdRegs.WDKEY.bit.WDKEY = 0x0055;
    WdRegs.WDKEY.bit.WDKEY = 0x00AA;
    EDIS;
}

//
// DisableDog - This function disables the watchdog timer.
//
void DisableDog(void)
{
    volatile Uint16 temp;

    //
    // Grab the clock config first so we don't clobber it
    //
    EALLOW;
    temp = WdRegs.WDCR.all & 0x0007;
    WdRegs.WDCR.all = 0x0068 | temp;
    EDIS;
}

//
// InitSysPll()
// This function initializes the PLL registers.
// Note:
//  - The internal oscillator CANNOT be used as the PLL source if the
//    PLLSYSCLK is configured to frequencies above 194 MHz.
//
//  - This function uses the Watchdog as a monitor for the PLL. The user
//  watchdog settings will be modified and restored upon completion.  Function
//  allows for a minimum re lock attempt for 5 tries.  Re lock attempt is carried
//  out if either SLIP condition occurs or SYSCLK to Input Clock ratio is off by 10%
//
//  - This function uses the following resources to support PLL initialization:
//          o Watchdog
//          o CPU Timer 1
//          o CPU Timer 2
//
void InitSysPll(Uint16 clock_source, Uint16 imult, Uint16 fmult, Uint16 divsel)
{
    Uint16 SCSR, WDCR, WDWCR, intStatus,  t1TCR, t1TPR, t1TPRH;
    Uint16 t2TCR, t2TPR, t2TPRH, t2SRC, t2Prescale;
    Uint32 t1PRD, t2PRD, ctr1;
    float sysclkToInClkError, mult, div;
    bool sysclkInvalidFreq=true;

    if((clock_source == ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL)    &&
       (imult        == ClkCfgRegs.SYSPLLMULT.bit.IMULT)           &&
       (fmult        == ClkCfgRegs.SYSPLLMULT.bit.FMULT)           &&
       (divsel       == ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV))
    {
        //
        // Everything is set as required, so just return
        //
        return;
    }

    if(clock_source != ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL)
    {
        switch (clock_source)
        {
            case INT_OSC1:
                SysIntOsc1Sel();
                break;

            case INT_OSC2:
                SysIntOsc2Sel();
                break;

            case XTAL_OSC:
                SysXtalOscSel();
                break;
        }
    }

    EALLOW;
    if(imult != ClkCfgRegs.SYSPLLMULT.bit.IMULT ||
       fmult != ClkCfgRegs.SYSPLLMULT.bit.FMULT)
    {
        Uint16 i;

        //
        // This bit is reset only by POR
        //
        if(DevCfgRegs.SYSDBGCTL.bit.BIT_0 == 1)
        {
            //
            // The user can optionally insert handler code here. This will only
            // be executed if a watchdog reset occurred after a failed system
            // PLL initialization. See your device user's guide for more
            // information.
            //
            // If the application has a watchdog reset handler, this bit should
            // be checked to determine if the watchdog reset occurred because
            // of the PLL.
            //
            // No action here will continue with retrying the PLL as normal.
            //
            // Failed PLL initialization is due to any of the following:
            //      - No PLL clock
            //      - SLIP condition
            //      - Wrong Frequency
            //
        }

        //
        // Bypass PLL and set dividers to /1
        //
        ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0;
        asm(" RPT #20 || NOP");
        ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = 0;

        //
        // Lock the PLL five times. This helps ensure a successful start.
        // Five is the minimum recommended number. The user can increase this
        // number according to allotted system initialization time.
        //
        for(i = 0; i < 5; i++)
        {
            //
            // Turn off PLL
            //
            ClkCfgRegs.SYSPLLCTL1.bit.PLLEN = 0;
            asm(" RPT #20 || NOP");

            //
            // Write multiplier, which automatically turns on the PLL
            //
            ClkCfgRegs.SYSPLLMULT.all = ((fmult << 8U) | imult);

            //
            // Wait for the SYSPLL lock counter
            //
            while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1)
            {
                //
                // Uncomment to service the watchdog
                //
                // ServiceDog();
            }
        }
    }

    //
    // Set divider to produce slower output frequency to limit current increase
    //
    if(divsel != PLLCLK_BY_126)
    {
         ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = divsel + 1;
    }
    else
    {
         ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = divsel;
    }

    //
    //      *CAUTION*
    // It is recommended to use the following watchdog code to monitor the PLL
    // startup sequence. If your application has already cleared the watchdog
    // SCRS[WDOVERRIDE] bit this cannot be done. It is recommended not to clear
    // this bit until after the PLL has been initiated.
    //

    //
    // Backup User Watchdog
    //
    SCSR = WdRegs.SCSR.all;
    WDCR = WdRegs.WDCR.all;
    WDWCR = WdRegs.WDWCR.all;

    //
    // Disable windowed functionality, reset counter
    //
    EALLOW;
    WdRegs.WDWCR.all = 0x0;
    WdRegs.WDKEY.bit.WDKEY = 0x55;
    WdRegs.WDKEY.bit.WDKEY = 0xAA;

    //
    // Disable global interrupts
    //
    intStatus = __disable_interrupts();

    //
    // Configure for watchdog reset and to run at max frequency
    //
    WdRegs.SCSR.all = 0x0;
    WdRegs.WDCR.all = 0x28;

    //
    // This bit is reset only by power-on-reset (POR) and will not be cleared
    // by a WD reset
    //
    DevCfgRegs.SYSDBGCTL.bit.BIT_0 = 1;

    //
    // Enable PLLSYSCLK is fed from system PLL clock
    //
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1;

    //
    // Delay to ensure system is clocking from PLL prior to clearing status bit
    //
    asm(" RPT #20 || NOP");

    //
    // Service watchdog
    //
    ServiceDog();

    //
    // Slip Bit Monitor and SYSCLK Frequency Check using timers
    // Re-lock routine for SLIP condition or if SYSCLK and CLKSRC timer counts
    // are off by +/- 10%.
    // At a minimum, SYSCLK check is performed.  Re lock attempt is carried out
    // if SLIPS bit is set. This while loop is monitored by watchdog.
    // In the event that the PLL does not successfully lock, the loop will be
    // aborted by watchdog reset.
    //
    EALLOW;
    while(sysclkInvalidFreq == true)
    {
        if(ClkCfgRegs.SYSPLLSTS.bit.SLIPS == 1)
        {
            //
            // Bypass PLL
            //
            ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0;
            asm(" RPT #20 || NOP");

            //
            // Turn off PLL
            //
            ClkCfgRegs.SYSPLLCTL1.bit.PLLEN = 0;
            asm(" RPT #20 || NOP");

            //
            // Write multipliers, which automatically turns on the PLL
            //
            ClkCfgRegs.SYSPLLMULT.all = ((fmult << 8U) | imult);

            //
            // Wait for the SYSPLL lock counter to expire
            //
            while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS != 1);

            //
            // Enable PLLSYSCLK is fed from system PLL clock
            //
            ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 1;

            //
            // Delay to ensure system is clocking from PLL
            //
            asm(" RPT #20 || NOP");
        }

        //
        // Backup timer1 and timer2 settings
        //
        t1TCR = CpuTimer1Regs.TCR.all;
        t1PRD = CpuTimer1Regs.PRD.all;
        t1TPR = CpuTimer1Regs.TPR.all;
        t1TPRH = CpuTimer1Regs.TPRH.all;
        t2SRC = CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKSRCSEL;
        t2Prescale = CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKPRESCALE;
        t2TCR = CpuTimer2Regs.TCR.all;
        t2PRD = CpuTimer2Regs.PRD.all;
        t2TPR = CpuTimer2Regs.TPR.all;
        t2TPRH = CpuTimer2Regs.TPRH.all;

        //
        // Set up timers 1 and 2
        // Configure timer1 to count SYSCLK cycles
        //
        CpuTimer1Regs.TCR.bit.TSS = 1;                                    // stop timer1
        CpuTimer1Regs.PRD.all = TMR1SYSCLKCTR;                            // seed timer1 counter
        CpuTimer1Regs.TPR.bit.TDDR = 0x0;                                 // sysclock divider
        CpuTimer1Regs.TCR.bit.TRB = 1;                                    // reload timer with value in PRD
        CpuTimer1Regs.TCR.bit.TIF = 1;                                    // clear interrupt flag
        CpuTimer1Regs.TCR.bit.TIE = 1;                                    // enable interrupt

        //
        // Configure timer2 to count Input clock cycles
        //
        switch(clock_source)
        {
            case INT_OSC1:
                // Clk Src = INT_OSC1
                CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKSRCSEL = 0x1;
                break;
            case INT_OSC2:
                // Clk Src = INT_OSC2
                CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKSRCSEL = 0x2;
                break;
            case XTAL_OSC:
                // Clk Src = XTAL
                CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKSRCSEL = 0x3;
                break;

        }
        CpuTimer2Regs.TCR.bit.TIF = 1;                                    // clear interrupt flag
        CpuTimer2Regs.TCR.bit.TIE = 1;                                    // enable interrupt
        CpuTimer2Regs.TCR.bit.TSS = 1;                                    // stop timer2
        CpuTimer2Regs.PRD.all = TMR2INPCLKCTR;                            // seed timer2 counter
        CpuTimer2Regs.TPR.bit.TDDR = 0x0;                                 // sysclock divider
        CpuTimer2Regs.TCR.bit.TRB = 1;                                    // reload timer with value in PRD

        //
        // Stop/Start timer counters
        //
        CpuTimer1Regs.TCR.bit.TSS = 1;                                    // stop timer1
        CpuTimer2Regs.TCR.bit.TSS = 1;                                    // stop timer2
        CpuTimer1Regs.TCR.bit.TRB = 1;                                    // reload timer1 with value in PRD
        CpuTimer2Regs.TCR.bit.TRB = 1;                                    // reload timer2 with value in PRD
        CpuTimer2Regs.TCR.bit.TIF = 1;                                    // clear timer2 interrupt flag
        CpuTimer2Regs.TCR.bit.TSS = 0;                                    // start timer2
        CpuTimer1Regs.TCR.bit.TSS = 0;                                    // start timer1

        //
        // Stop timers if either timer1 or timer2 counters overflow
        //
        while((CpuTimer2Regs.TCR.bit.TIF == 0) && (CpuTimer1Regs.TCR.bit.TIF == 0));

        CpuTimer1Regs.TCR.bit.TSS = 1;                                    // stop timer1
        CpuTimer2Regs.TCR.bit.TSS = 1;                                    // stop timer2

        //
        // Calculate elapsed counts on timer1
        //
        ctr1 = TMR1SYSCLKCTR - CpuTimer1Regs.TIM.all;

        //
        // Restore timer settings
        //
        CpuTimer1Regs.TCR.all = t1TCR;
        CpuTimer1Regs.PRD.all = t1PRD;
        CpuTimer1Regs.TPR.all = t1TPR;
        CpuTimer1Regs.TPRH.all = t1TPRH;
        CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKSRCSEL = t2SRC;
        CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKPRESCALE = t2Prescale;
        CpuTimer2Regs.TCR.all = t2TCR;
        CpuTimer2Regs.PRD.all = t2PRD;
        CpuTimer2Regs.TPR.all = t2TPR;
        CpuTimer2Regs.TPRH.all = t2TPRH;

        //
        // Calculate Clock Error:
        // Error = (mult/div) - (timer1 count/timer2 count)
        //
        mult = (float)(imult) + (float)(fmult)/4;
        div = (float)((!ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV) ? 1 : (ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV << 1));

        sysclkToInClkError = (mult/div) - ((float)ctr1/(float)TMR2INPCLKCTR);

        //
        // sysclkInvalidFreq will be set to true if sysclkToInClkError is off by 10%
        //
        sysclkInvalidFreq = ((sysclkToInClkError > 0.10) || (sysclkToInClkError < -0.10));
    }

    //
    // Clear bit
    //
    DevCfgRegs.SYSDBGCTL.bit.BIT_0 = 0;

    //
    // Restore user watchdog, first resetting counter
    //
    WdRegs.WDKEY.bit.WDKEY = 0x55;
    WdRegs.WDKEY.bit.WDKEY = 0xAA;

    WDCR |= 0x28;                     // Setup WD key--KEY bits always read 0
    WdRegs.WDCR.all = WDCR;
    WdRegs.WDWCR.all = WDWCR;
    WdRegs.SCSR.all = SCSR & 0xFFFE;  // Mask write to bit 0 (W1toClr)

    //
    // Restore state of ST1[INTM]. This was set by the __disable_interrupts()
    // intrinsic previously.
    //
    if(!(intStatus & 0x1))
    {
        EINT;
    }

    //
    // Restore state of ST1[DBGM]. This was set by the __disable_interrupts()
    // intrinsic previously.
    //
    if(!(intStatus & 0x2))
    {
        asm(" CLRC DBGM");
    }

    //
    // 200 PLLSYSCLK delay to allow voltage regulator to stabilize prior
    // to increasing entire system clock frequency.
    //
    asm(" RPT #200 || NOP");

    //
    // Set the divider to user value
    //
    ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV = divsel;

    EDIS;
}

//
// InitAuxPll - This function initializes the AUXPLL registers.
//
// Note: For this function to properly detect PLL startup,
// SYSCLK >= 2*AUXPLLCLK after the AUXPLL is selected as the clocking source.
//
// This function will use CPU Timer 2 to monitor a successful lock of the
// AUXPLL.
//
void InitAuxPll(Uint16 clock_source, Uint16 imult, Uint16 fmult, Uint16 divsel)
{
    Uint16 i;
    Uint16 counter = 0;
    Uint16 started = 0;
    Uint16 t2TCR, t2TPR, t2TPRH, t2SRC, t2Prescale, attempts;
    Uint32 t2PRD;

    if((clock_source == ClkCfgRegs.CLKSRCCTL2.bit.AUXOSCCLKSRCSEL) &&
       (imult        == ClkCfgRegs.AUXPLLMULT.bit.IMULT)           &&
       (fmult        == ClkCfgRegs.AUXPLLMULT.bit.FMULT)           &&
       (divsel       == ClkCfgRegs.AUXCLKDIVSEL.bit.AUXPLLDIV))
    {
        //
        // Everything is set as required, so just return
        //
        return;
    }

    switch (clock_source)
    {
        case INT_OSC2:
            AuxIntOsc2Sel();
            break;

        case XTAL_OSC:
            AuxXtalOscSel();
            break;

        case AUXCLKIN:
            AuxAuxClkSel();
            break;
    }

    //
    // Backup Timer 2 settings
    //
    t2SRC = CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKSRCSEL;
    t2Prescale = CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKPRESCALE;
    t2TCR = CpuTimer2Regs.TCR.all;
    t2PRD = CpuTimer2Regs.PRD.all;
    t2TPR = CpuTimer2Regs.TPR.all;
    t2TPRH = CpuTimer2Regs.TPRH.all;

    //
    // Configure Timer 2 for AUXPLL as source in known configuration
    //
    EALLOW;
    CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKSRCSEL = 0x6;
    CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKPRESCALE = 0x0;    // Divide by 1

    CpuTimer2Regs.TCR.bit.TSS = 1;      // Stop timer
    CpuTimer2Regs.PRD.all = 10;         // Small PRD value to detect overflow
    CpuTimer2Regs.TPR.all = 0;
    CpuTimer2Regs.TPRH.all = 0;
    CpuTimer2Regs.TCR.bit.TIE = 0;      // Disable timer interrupts

    //
    // Set AUX Divide by 8 to ensure that AUXPLLCLK <= SYSCLK/2 while using
    // Timer 2
    //
    ClkCfgRegs.AUXCLKDIVSEL.bit.AUXPLLDIV = 0x3;
    EDIS;

    while((counter < 5) && (started == 0))
    {
        EALLOW;
        ClkCfgRegs.AUXPLLCTL1.bit.PLLEN = 0;    // Turn off AUXPLL
        asm(" RPT #20 || NOP");                 // Small delay for power down

        //
        // Set integer and fractional multiplier, which automatically turns on
        // the PLL
        //
        ClkCfgRegs.AUXPLLMULT.all = ((fmult << 8U) | imult);

        //
        // Enable AUXPLL
        //
        ClkCfgRegs.AUXPLLCTL1.bit.PLLEN = 1;
        EDIS;

        //
        // Wait for the AUXPLL lock counter
        //
        while(ClkCfgRegs.AUXPLLSTS.bit.LOCKS != 1)
        {
            //
            // Uncomment to service the watchdog
            //
            // ServiceDog();
        }

        //
        // Enable AUXPLLCLK to be fed from AUX PLL
        //
        EALLOW;
        ClkCfgRegs.AUXPLLCTL1.bit.PLLCLKEN = 1;
        asm(" RPT #20 || NOP");

        //
        // CPU Timer 2 will now be setup to be clocked from AUXPLLCLK. This is
        // used to test that the PLL has successfully started.
        //
        CpuTimer2Regs.TCR.bit.TRB = 1;      // Reload period value
        CpuTimer2Regs.TCR.bit.TSS = 0;      // Start Timer

        //
        // Check to see timer is counting properly
        //
        for(i = 0; i < 1000; i++)
        {
            //
            // Check overflow flag
            //
            if(CpuTimer2Regs.TCR.bit.TIF)
            {
                //
                // Clear overflow flag
                //
                CpuTimer2Regs.TCR.bit.TIF = 1;

                //
                // Set flag to indicate PLL started and break out of for-loop
                //
                started = 1;
                break;
            }
        }

        //
        // Stop timer
        //
        CpuTimer2Regs.TCR.bit.TSS = 1;
        counter++;
        EDIS;
    }

    if(started == 0)
    {
        //
        // AUX PLL may not have started. Reset multiplier to 0 (bypass PLL).
        //
        EALLOW;
        ClkCfgRegs.AUXPLLMULT.all = 0;
        EDIS;

        //
        // The user should put some handler code here based on how this
        // condition should be handled in their application.
        //
        asm(" ESTOP0");
    }

    //
    // Slip Bit Monitor
    // Re-lock routine for SLIP condition
    //
    attempts = 0;
    while(ClkCfgRegs.AUXPLLSTS.bit.SLIPS && (attempts < 10))
    {
        EALLOW;
        //
        // Bypass AUXPLL
        //
        ClkCfgRegs.AUXPLLCTL1.bit.PLLCLKEN = 0;
        asm(" RPT #20 || NOP");

        //
        // Turn off AUXPLL
        //
        ClkCfgRegs.AUXPLLCTL1.bit.PLLEN = 0;
        asm(" RPT #20 || NOP");

        //
        // Set integer and fractional multiplier, which automatically turns
        // on the PLL
        //
        ClkCfgRegs.AUXPLLMULT.all = ((fmult << 8U) | imult);

        //
        // Wait for the AUXPLL lock counter
        //
        while(ClkCfgRegs.AUXPLLSTS.bit.LOCKS != 1);

        //
        // Enable AUXPLLCLK to be fed from AUXPLL
        //
        ClkCfgRegs.AUXPLLCTL1.bit.PLLCLKEN = 1;
        asm(" RPT #20 || NOP");

        attempts++;
        EDIS;
    }

    //
    // Set divider to desired value
    //
    EALLOW;
    ClkCfgRegs.AUXCLKDIVSEL.bit.AUXPLLDIV = divsel;

    //
    // Restore Timer 2 configuration
    //
    CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKSRCSEL = t2SRC;
    CpuSysRegs.TMR2CLKCTL.bit.TMR2CLKPRESCALE = t2Prescale;
    CpuTimer2Regs.TCR.all = t2TCR;
    CpuTimer2Regs.PRD.all = t2PRD;
    CpuTimer2Regs.TPR.all = t2TPR;
    CpuTimer2Regs.TPRH.all = t2TPRH;

    //
    // Reload period value
    //
    CpuTimer2Regs.TCR.bit.TRB = 1;
    EDIS;
}

//
// CsmUnlock - This function unlocks the CSM. User must replace 0xFFFF's with
//             current password for the DSP. Returns 1 if unlock is successful.
//
Uint16 CsmUnlock(void)
{
    volatile Uint16 temp;

    //
    // Load the key registers with the current password. The 0xFFFF's are dummy
    // passwords.  User should replace them with the correct password for the
    // DSP.
    //
    EALLOW;
    DcsmZ1Regs.Z1_CSMKEY0 = 0xFFFFFFFF;
    DcsmZ1Regs.Z1_CSMKEY1 = 0xFFFFFFFF;
    DcsmZ1Regs.Z1_CSMKEY2 = 0xFFFFFFFF;
    DcsmZ1Regs.Z1_CSMKEY3 = 0xFFFFFFFF;

    DcsmZ2Regs.Z2_CSMKEY0 = 0xFFFFFFFF;
    DcsmZ2Regs.Z2_CSMKEY1 = 0xFFFFFFFF;
    DcsmZ2Regs.Z2_CSMKEY2 = 0xFFFFFFFF;
    DcsmZ2Regs.Z2_CSMKEY3 = 0xFFFFFFFF;
    EDIS;

    return(0);
}

//
// SysIntOsc1Sel - This function switches to Internal Oscillator 1.
//
void SysIntOsc1Sel(void)
{
    EALLOW;
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 2;     // Clk Src = INTOSC1
    ClkCfgRegs.CLKSRCCTL1.bit.XTALOFF=1;            // Turn off XTALOSC
    EDIS;
}

//
// SysIntOsc2Sel - This function switches to Internal oscillator 2.
//
void SysIntOsc2Sel(void)
{
    EALLOW;
    ClkCfgRegs.CLKSRCCTL1.bit.INTOSC2OFF=0;         // Turn on INTOSC2
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 0;     // Clk Src = INTOSC2
    ClkCfgRegs.CLKSRCCTL1.bit.XTALOFF=1;            // Turn off XTALOSC
    EDIS;
}

//
// SysXtalOscSel - This function switches to External CRYSTAL oscillator.
//
void SysXtalOscSel(void)
{
    EALLOW;
    ClkCfgRegs.CLKSRCCTL1.bit.XTALOFF=0;            // Turn on XTALOSC
    ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL = 1;     // Clk Src = XTAL
    EDIS;
}

//
// AuxIntOsc2Sel - This function switches to Internal oscillator 2.
//
void AuxIntOsc2Sel(void)
{
    EALLOW;
    ClkCfgRegs.CLKSRCCTL1.bit.INTOSC2OFF=0;         // Turn on INTOSC2
    ClkCfgRegs.CLKSRCCTL2.bit.AUXOSCCLKSRCSEL = 0;  // Clk Src = INTOSC2
    EDIS;
}

//
// AuxXtalOscSel - This function switches to External CRYSTAL oscillator.
//
void AuxXtalOscSel(void)
{
    EALLOW;
    ClkCfgRegs.CLKSRCCTL1.bit.XTALOFF=0;            // Turn on XTALOSC
    ClkCfgRegs.CLKSRCCTL2.bit.AUXOSCCLKSRCSEL = 1;  // Clk Src = XTAL
    EDIS;
}

//
// AuxAUXCLKOscSel - This function switches to AUXCLKIN (from a GPIO).
//
void AuxAuxClkSel(void)
{
    EALLOW;
    ClkCfgRegs.CLKSRCCTL2.bit.AUXOSCCLKSRCSEL = 2; // Clk Src = XTAL
    EDIS;
}

//
// IDLE - Enter IDLE mode (single CPU).
//
void IDLE(void)
{
    EALLOW;
    CpuSysRegs.LPMCR.bit.LPM = LPM_IDLE;
    EDIS;
    asm(" IDLE");
}

//
// STANDBY - Enter STANDBY mode (single CPU).
//
void STANDBY(void)
{
    EALLOW;
    CpuSysRegs.LPMCR.bit.LPM = LPM_STANDBY;
    EDIS;
    asm(" IDLE");
}

//
// HALT - Enter HALT mode.
//
void HALT(void)
{
    EALLOW;
    CpuSysRegs.LPMCR.bit.LPM = LPM_HALT;
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0;
    ClkCfgRegs.SYSPLLCTL1.bit.PLLEN = 0;
    EDIS;
    asm(" IDLE");
}

//
// HIB - Enter HIB mode.
//
void HIB(void)
{
    EALLOW;
    CpuSysRegs.LPMCR.bit.LPM = LPM_HIB;
    EDIS;
    DisablePeripheralClocks();
    EALLOW;
    ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN = 0;
    ClkCfgRegs.SYSPLLCTL1.bit.PLLEN = 0;
    EDIS;
    asm(" IDLE");
}
