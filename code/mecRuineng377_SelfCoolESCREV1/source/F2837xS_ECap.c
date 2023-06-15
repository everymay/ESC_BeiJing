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
//---------------------------------------------------------------------------
// InitECap:
//---------------------------------------------------------------------------
//CAP���ܣ�
// ���񵽿���DSP���͵�IO��ת�źţ����յ�������жϣ�����PWM������
// ʹ����Դ�� ECAP4��GPIO11
// δ���õļĴ��� ��CTRPHS
void InitECap(void)
{
////	InitECapGpio();  ֱ����ֲ��GPIO�ĳ�ʼ��������
//	ECap3Regs.ECEINT.all = 0x0000;             // ������в����ж�
//	ECap3Regs.ECCLR.all = 0xFFFF;              // ���жϱ�־λ
//	ECap3Regs.ECCTL1.bit.CAPLDEN = 0;          // ��ֹ CAP1-CAP4 register װ��
//	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;        //ֹͣ��ʱ��
//
//	// Configure peripheral registers
//	ECap3Regs.ECCTL2.bit.CONT_ONESHT = 1;       //One-shotģʽ
//	ECap3Regs.ECCTL2.bit.STOP_WRAP = 0;         //�¼�1������ֹͣ���񣬵ȴ��´��ж�
//	ECap3Regs.ECCTL1.bit.CAP1POL = 1;           //1�� Falling edge  0��rising edge
////	ECap4Regs.ECCTL1.bit.CAP2POL = 1;
////	ECap4Regs.ECCTL1.bit.CAP3POL = 0;
////	ECap4Regs.ECCTL1.bit.CAP4POL = 1;
//
//	ECap3Regs.ECCTL1.bit.CTRRST1 = 1;           //0������ʱ���ǩ���У� 1�����ģʽ�£�ÿ�β����λ������
////	ECap4Regs.ECCTL1.bit.CTRRST2 = 1;
////	ECap4Regs.ECCTL1.bit.CTRRST3 = 1;
////	ECap4Regs.ECCTL1.bit.CTRRST4 = 1;
//
//	ECap3Regs.ECCTL2.bit.SYNCI_EN = 0;//1;           //ͬ��
//	ECap3Regs.ECCTL2.bit.SYNCO_SEL = 2;//;          //��ͬ��
//	ECap3Regs.ECCTL1.bit.CAPLDEN = 1;            // Enable capture units
//
////	ECap4Regs.ECCTL1.bit.FREE_SOFT = 3;        // TSCTR�ļ�������������
//	ECap3Regs.ECCTL1.bit.PRESCALE = 0;         // ��Ԥװ��Ƶ��
//
//	// Configure peripheral registers
//	ECap3Regs.ECCTL2.bit.CAP_APWM = 0;  		//ʹ��CAPģʽ
//
//	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1;         // Start Counter
//	ECap3Regs.ECCTL2.bit.REARM = 1;             // Arm one-shot
//	ECap3Regs.ECCTL1.bit.CAPLDEN = 1;           // Enable CAP1-CAP4 register loads
//	ECap3Regs.ECEINT.bit.CEVT1 = 1;             // Enable 1 events interrupt



//	ECap3Regs.ECEINT.all = 0x0000;             // ������в����ж�
//	ECap3Regs.ECCLR.all = 0xFFFF;              // ���жϱ�־λ
////	ECap3Regs.ECCTL1.bit.CAPLDEN = EC_ENABLE;  //����CAP1-CAP4 register װ��       // ��ֹ CAP1-CAP4 register װ��
//	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;        //ֹͣ��ʱ��
//
//	// Configure peripheral registers
//	ECap3Regs.ECCTL2.bit.CONT_ONESHT = EC_CONTINUOUS; //Continuous mode//One-shotģʽ
//	ECap3Regs.ECCTL2.bit.STOP_WRAP = 0;         //�¼�1������ֹͣ���񣬵ȴ��´��ж�
//
//	ECap3Regs.ECCTL1.bit.CAP1POL = EC_FALLING;  //1�� Falling edge  0��rising edge
////	ECap4Regs.ECCTL1.bit.CAP2POL = 1;
////	ECap4Regs.ECCTL1.bit.CAP3POL = 0;
////	ECap4Regs.ECCTL1.bit.CAP4POL = 1;
//	ECap3Regs.ECCTL1.bit.CTRRST1 = EC_DELTA_MODE;//0������ʱ���ǩ���У� 1�����ģʽ�£�ÿ�β����λ������
////	ECap4Regs.ECCTL1.bit.CTRRST2 = 1;
////	ECap4Regs.ECCTL1.bit.CTRRST3 = 1;
////	ECap4Regs.ECCTL1.bit.CTRRST4 = 1;
//
//	ECap3Regs.ECCTL2.bit.SYNCI_EN = EC_DISABLE;		//ͬ��
//	ECap3Regs.ECCTL2.bit.SYNCO_SEL = EC_SYNCO_DIS;	//��ͬ��
//	ECap3Regs.ECCTL1.bit.CAPLDEN = EC_ENABLE;		// Enable capture units
//
////	ECap4Regs.ECCTL1.bit.FREE_SOFT = 3;        		// TSCTR�ļ�������������
//	ECap3Regs.ECCTL1.bit.PRESCALE = EC_DIV1;  		// ��Ԥװ��Ƶ��
//
//	// Configure peripheral registers
//	ECap3Regs.ECCTL2.bit.CAP_APWM = EC_CAP_MODE;	//ʹ��CAPģʽ
//
//	ECap3Regs.ECCTL2.bit.TSCTRSTOP = EC_RUN;	// Start Counter
//	ECap3Regs.ECCTL2.bit.REARM = 1;             // Arm one-shot
//	ECap3Regs.ECEINT.bit.CEVT1 = 1;             // Enable 1 events interrupt



// Initialize eCAP1/2/3
    ECap1Regs.ECEINT.all = 0x0000;             // Disable all capture interrupts
    ECap1Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
    ECap1Regs.ECCTL1.bit.CAPLDEN = 0;          // Disable CAP1-CAP4 register loads
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;        // Make sure the counter is stopped
    ECap2Regs.ECEINT.all = 0x0000;
    ECap2Regs.ECCLR.all = 0xFFFF;
    ECap2Regs.ECCTL1.bit.CAPLDEN = 0;
    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;
    ECap3Regs.ECEINT.all = 0x0000;
    ECap3Regs.ECCLR.all = 0xFFFF;
    ECap3Regs.ECCTL1.bit.CAPLDEN = 0;
    ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;

// Configure peripheral registers
    ECap1Regs.ECCTL1.bit.FREE_SOFT = 3;        //Run Free
    ECap1Regs.ECCTL1.bit.PRESCALE = 0;         //Ԥ��Ƶ
    ECap1Regs.ECCTL2.bit.CAP_APWM = 0;         //CAP mode
    ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0;      // Continuous mode
    ECap1Regs.ECCTL2.bit.STOP_WRAP = 3;        // Stop at 4 events
    ECap1Regs.ECCTL1.bit.CAP1POL = 1;          // falling edge
    ECap1Regs.ECCTL1.bit.CAP2POL = 1;          // falling edge
    ECap1Regs.ECCTL1.bit.CAP3POL = 1;          // falling edge
    ECap1Regs.ECCTL1.bit.CAP4POL = 1;          // falling edge
    ECap1Regs.ECCTL1.bit.CTRRST1 = 1;          // Difference operation
    ECap1Regs.ECCTL1.bit.CTRRST2 = 1;          // Difference operation
    ECap1Regs.ECCTL1.bit.CTRRST3 = 1;          // Difference operation
    ECap1Regs.ECCTL1.bit.CTRRST4 = 1;          // Difference operation
    ECap1Regs.ECCTL2.bit.SYNCI_EN = 0;         // Disable sync in
    ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0;        // Pass through
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable capture units
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
    ECap1Regs.ECCTL2.bit.REARM = 1;
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
    ECap1Regs.ECEINT.bit.CEVT4 = 1;            // 4 events = interrupt
    ECap1Regs.ECEINT.bit.CEVT3 = 1;
    ECap1Regs.ECEINT.bit.CEVT2 = 1;
    ECap1Regs.ECEINT.bit.CEVT1 = 1;

    ECap2Regs.ECCTL1.bit.FREE_SOFT = 3;        //Run Free
    ECap2Regs.ECCTL1.bit.PRESCALE = 0;         //Ԥ��Ƶ
    ECap2Regs.ECCTL2.bit.CAP_APWM = 0;         //CAP mode
    ECap2Regs.ECCTL2.bit.CONT_ONESHT = 0;      // Continuous mode
    ECap2Regs.ECCTL2.bit.STOP_WRAP = 3;        // Stop at 4 events
    ECap2Regs.ECCTL1.bit.CAP1POL = 1;          // falling edge
    ECap2Regs.ECCTL1.bit.CAP2POL = 1;          // falling edge
    ECap2Regs.ECCTL1.bit.CAP3POL = 1;          // falling edge
    ECap2Regs.ECCTL1.bit.CAP4POL = 1;          // falling edge
    ECap2Regs.ECCTL1.bit.CTRRST1 = 1;          // Difference operation
    ECap2Regs.ECCTL1.bit.CTRRST2 = 1;          // Difference operation
    ECap2Regs.ECCTL1.bit.CTRRST3 = 1;          // Difference operation
    ECap2Regs.ECCTL1.bit.CTRRST4 = 1;          // Difference operation
    ECap2Regs.ECCTL2.bit.SYNCI_EN = 0;         // Disable sync in
    ECap2Regs.ECCTL2.bit.SYNCO_SEL = 0;        // Pass through
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable capture units
    ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
    ECap2Regs.ECCTL2.bit.REARM = 1;
    ECap2Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
    ECap2Regs.ECEINT.bit.CEVT4 = 1;            // 4 events = interrupt
    ECap2Regs.ECEINT.bit.CEVT3 = 1;
    ECap2Regs.ECEINT.bit.CEVT2 = 1;
    ECap2Regs.ECEINT.bit.CEVT1 = 1;

    ECap3Regs.ECCTL1.bit.FREE_SOFT = 3;        //Run Free
    ECap3Regs.ECCTL1.bit.PRESCALE = 0;         //Ԥ��Ƶ
    ECap3Regs.ECCTL2.bit.CAP_APWM = 0;         //CAP mode
    ECap3Regs.ECCTL2.bit.CONT_ONESHT = 0;      // Continuous mode
    ECap3Regs.ECCTL2.bit.STOP_WRAP = 3;        // Stop at 4 events
    ECap3Regs.ECCTL1.bit.CAP1POL = 1;          // falling edge
    ECap3Regs.ECCTL1.bit.CAP2POL = 1;          // falling edge
    ECap3Regs.ECCTL1.bit.CAP3POL = 1;          // falling edge
    ECap3Regs.ECCTL1.bit.CAP4POL = 1;          // falling edge
    ECap3Regs.ECCTL1.bit.CTRRST1 = 1;          // Difference operation
    ECap3Regs.ECCTL1.bit.CTRRST2 = 1;          // Difference operation
    ECap3Regs.ECCTL1.bit.CTRRST3 = 1;          // Difference operation
    ECap3Regs.ECCTL1.bit.CTRRST4 = 1;          // Difference operation
    ECap3Regs.ECCTL2.bit.SYNCI_EN = 0;         // Disable sync in
    ECap3Regs.ECCTL2.bit.SYNCO_SEL = 0;        // Pass through
    ECap3Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable capture units
    ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
    ECap3Regs.ECCTL2.bit.REARM = 1;
    ECap3Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
    ECap3Regs.ECEINT.bit.CEVT4 = 1;            // 4 events = interrupt
    ECap3Regs.ECEINT.bit.CEVT3 = 1;
    ECap3Regs.ECEINT.bit.CEVT2 = 1;
    ECap3Regs.ECEINT.bit.CEVT1 = 1;

    ECap1Regs.ECCLR.bit.CEVT1 = 1;            // clear flag
    ECap1Regs.ECCLR.bit.CEVT2 = 1;
    ECap1Regs.ECCLR.bit.CEVT3 = 1;
    ECap1Regs.ECCLR.bit.CEVT4 = 1;
    ECap2Regs.ECCLR.bit.CEVT1 = 1;            // clear flag
    ECap2Regs.ECCLR.bit.CEVT2 = 1;
    ECap2Regs.ECCLR.bit.CEVT3 = 1;
    ECap2Regs.ECCLR.bit.CEVT4 = 1;
    ECap3Regs.ECCLR.bit.CEVT1 = 1;            // clear flag
    ECap3Regs.ECCLR.bit.CEVT2 = 1;
    ECap3Regs.ECCLR.bit.CEVT3 = 1;
    ECap3Regs.ECCLR.bit.CEVT4 = 1;

    ECap5Regs.ECCTL2.bit.CAP_APWM = 1; //APWM Mode ��ΪAPWM����ź�
    ECap5Regs.ECCTL2.bit.APWMPOL = 1;  //�͵�ƽ��Ч(1ռ�ձ�)
    ECap5Regs.CAP1 = 4000; // Set period value   50K
    ECap5Regs.CAP2 = 0; // Set Duty cyclei.e. compare value
    ECap5Regs.CTRPHS = 0x0; // make phasezero

    ECap5Regs.ECCTL2.bit.SYNCI_EN = 0; // Synchnot used
    ECap5Regs.ECCTL2.bit.SYNCO_SEL= 0; // Synchnot used
    ECap5Regs.ECCTL2.bit.TSCTRSTOP= 1; // AllowTSCTRto run

}

//
// InitECap - This function initializes the eCAP(s) to a known state.
//
//void InitECap(void)
//{
    // Initialize eCAP1/2/3/4/5/6

    //tbd...
//}

//
// InitECapGpio - This function initializes GPIO pins to function as ECAP pins
//                Each GPIO pin can be configured as a GPIO pin or up to 3
//                different peripheral functional pins. By default all pins
//                come up as GPIO inputs after reset.
//                Caution:
//                For each eCAP peripheral
//                Only one GPIO pin should be enabled for ECAP operation.
//                Comment out other unwanted lines.
//
void InitECapGpio()
{
//    InitAPwm5Gpio();                                      //cap5
//    GPIO_SetupPinOptions(14, GPIO_OUTPUT, GPIO_OPENDRAIN);       //gpio14
    InitECap1Gpio(94);  //CAP��������Ϊ�����ź�.����GPIO�ڵı仯,�Ӷ�����һЩ����(������ж�)   A��
    InitECap2Gpio(85);  //CAP��������Ϊ�����ź�.����GPIO�ڵı仯,�Ӷ�����һЩ����(������ж�)   B��
    InitECap3Gpio(97);  //CAP��������Ϊ�����ź�.����GPIO�ڵı仯,�Ӷ�����һЩ����(������ж�)   C��
}

//
// InitECap1Gpio - Initialize ECAP1 GPIOs
//
void InitECap1Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT7SELECT = pin;         // Set eCAP1 source to GPIO-pin
    EDIS;
}

//
// InitECap2Gpio - Initialize ECAP2 GPIOs
//
void InitECap2Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT8SELECT = pin;         // Set eCAP2 source to GPIO-pin
    EDIS;
}

//
// InitECap3Gpio - Initialize ECAP3 GPIOs
//
void InitECap3Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT9SELECT = pin;         // Set eCAP3 source to GPIO-pin
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
