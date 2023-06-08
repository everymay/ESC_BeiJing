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
 * ���ܣ��������岶��ECAP��
 */
void InitECap(void)
{
	ECap1Regs.ECEINT.all = 0x0000; //�ر�ECap-1�������ж�Դ��WY
	ECap1Regs.ECCLR.all = 0xFFFF; //���ECap-1�������жϱ�־λ��WY
	ECap1Regs.ECCTL1.bit.CAPLDEN = 0; //ʧ��ʱ�����¼��WY
	ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0; //ֹͣ��������WY

	ECap2Regs.ECEINT.all = 0x0000; //�ر�ECap2�������ж�Դ��WY
	ECap2Regs.ECCLR.all = 0xFFFF; //���ECap2�������жϱ�־λ��WY
	ECap2Regs.ECCTL1.bit.CAPLDEN = 0; //ʧ��ʱ�����¼��WY
	ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0; //ֹͣ��������WY

	ECap3Regs.ECEINT.all = 0x0000; //�ر�ECap3�������ж�Դ��WY
	ECap3Regs.ECCLR.all = 0xFFFF; //���ECap3�������жϱ�־λ��WY
	ECap3Regs.ECCTL1.bit.CAPLDEN = 0; //ʧ��ʱ�����¼��WY
	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0; //ֹͣ��������WY

	ECap1Regs.ECCTL1.bit.FREE_SOFT = 3; //���������ܵ���ģʽ��Ӱ�졣WY
	ECap1Regs.ECCTL1.bit.PRESCALE = 0; //Ԥ��Ƶϵ��Ϊ1��WY
	ECap1Regs.ECCTL2.bit.CAP_APWM = 0; //���岶��ģʽ��WY
	ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0; //�����������塣WY
	ECap1Regs.ECCTL2.bit.STOP_WRAP = 3; //�ڵ��Ĵβ����¼������������������塣WY
	ECap1Regs.ECCTL1.bit.CAP1POL = 1; //�����½��ء�WY
	ECap1Regs.ECCTL1.bit.CAP2POL = 1; //�����½��ء�WY
	ECap1Regs.ECCTL1.bit.CAP3POL = 1; //�����½��ء�WY
	ECap1Regs.ECCTL1.bit.CAP4POL = 1; //�����½��ء�WY
	ECap1Regs.ECCTL1.bit.CTRRST1 = 1; //�ڵ�1�β����¼�������λ��������WY
	ECap1Regs.ECCTL1.bit.CTRRST2 = 1; //�ڵ�2�β����¼�������λ��������WY
	ECap1Regs.ECCTL1.bit.CTRRST3 = 1; //�ڵ�3�β����¼�������λ��������WY
	ECap1Regs.ECCTL1.bit.CTRRST4 = 1; //�ڵ�4�β����¼�������λ��������WY
	ECap1Regs.ECCTL2.bit.SYNCI_EN = 0; //ʧ������ͬ���źš�WY
	ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0; //������ͬ���ź���Ϊ���ͬ���źš�WY
	ECap1Regs.ECCTL1.bit.CAPLDEN = 1; //ʹ��ʱ�����¼��WY
	ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1; //������������WY
	ECap1Regs.ECCTL2.bit.REARM = 1; //��λECAP-1��WY
	ECap1Regs.ECCTL1.bit.CAPLDEN = 1; //ʹ��ʱ�����¼��WY
	ECap1Regs.ECEINT.bit.CEVT4 = 1; //����4�β����¼���Ϊ�ж�Դ��WY
	ECap1Regs.ECEINT.bit.CEVT3 = 1; //����3�β����¼���Ϊ�ж�Դ��WY
	ECap1Regs.ECEINT.bit.CEVT2 = 1; //����2�β����¼���Ϊ�ж�Դ��WY
	ECap1Regs.ECEINT.bit.CEVT1 = 1; //����1�β����¼���Ϊ�ж�Դ��WY

	ECap2Regs.ECCTL1.bit.FREE_SOFT = 3; //���������ܵ���ģʽ��Ӱ�졣WY
	ECap2Regs.ECCTL1.bit.PRESCALE = 0; //Ԥ��Ƶϵ��Ϊ1��WY
	ECap2Regs.ECCTL2.bit.CAP_APWM = 0; //���岶��ģʽ��WY
	ECap2Regs.ECCTL2.bit.CONT_ONESHT = 0; //�����������塣WY
	ECap2Regs.ECCTL2.bit.STOP_WRAP = 3; //�ڵ��Ĵβ����¼������������������塣WY
	ECap2Regs.ECCTL1.bit.CAP1POL = 1; //�����½��ء�WY
	ECap2Regs.ECCTL1.bit.CAP2POL = 1; //�����½��ء�WY
	ECap2Regs.ECCTL1.bit.CAP3POL = 1; //�����½��ء�WY
	ECap2Regs.ECCTL1.bit.CAP4POL = 1; //�����½��ء�WY
	ECap2Regs.ECCTL1.bit.CTRRST1 = 1; //�ڵ�1�β����¼�������λ��������WY
	ECap2Regs.ECCTL1.bit.CTRRST2 = 1; //�ڵ�1�β����¼�������λ��������WY
	ECap2Regs.ECCTL1.bit.CTRRST3 = 1; //�ڵ�1�β����¼�������λ��������WY
	ECap2Regs.ECCTL1.bit.CTRRST4 = 1; //�ڵ�1�β����¼�������λ��������WY
	ECap2Regs.ECCTL2.bit.SYNCI_EN = 0; //ʧ������ͬ���źš�WY
	ECap2Regs.ECCTL2.bit.SYNCO_SEL = 0; //������ͬ���ź���Ϊ���ͬ���źš�WY
	ECap2Regs.ECCTL1.bit.CAPLDEN = 1; //ʹ��ʱ�����¼��WY
	ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1; //������������WY
	ECap2Regs.ECCTL2.bit.REARM = 1; //��λECAP-2��WY
	ECap2Regs.ECCTL1.bit.CAPLDEN = 1; //ʹ��ʱ�����¼��WY
	ECap2Regs.ECEINT.bit.CEVT4 = 1; //����4�β����¼���Ϊ�ж�Դ��WY
	ECap2Regs.ECEINT.bit.CEVT3 = 1; //����3�β����¼���Ϊ�ж�Դ��WY
	ECap2Regs.ECEINT.bit.CEVT2 = 1; //����2�β����¼���Ϊ�ж�Դ��WY
	ECap2Regs.ECEINT.bit.CEVT1 = 1; //����1�β����¼���Ϊ�ж�Դ��WY

	ECap3Regs.ECCTL1.bit.FREE_SOFT = 3; //���������ܵ���ģʽ��Ӱ�졣WY
	ECap3Regs.ECCTL1.bit.PRESCALE = 0; //Ԥ��Ƶϵ��Ϊ1��WY
	ECap3Regs.ECCTL2.bit.CAP_APWM = 0; //���岶��ģʽ��WY
	ECap3Regs.ECCTL2.bit.CONT_ONESHT = 0; //�����������塣WY
	ECap3Regs.ECCTL2.bit.STOP_WRAP = 3; //�ڵ��Ĵβ����¼������������������塣WY
	ECap3Regs.ECCTL1.bit.CAP1POL = 1; //�����½��ء�WY
	ECap3Regs.ECCTL1.bit.CAP2POL = 1; //�����½��ء�WY
	ECap3Regs.ECCTL1.bit.CAP3POL = 1; //�����½��ء�WY
	ECap3Regs.ECCTL1.bit.CAP4POL = 1; //�����½��ء�WY
	ECap3Regs.ECCTL1.bit.CTRRST1 = 1; //�ڵ�1�β����¼�������λ��������WY
	ECap3Regs.ECCTL1.bit.CTRRST2 = 1; //�ڵ�2�β����¼�������λ��������WY
	ECap3Regs.ECCTL1.bit.CTRRST3 = 1; //�ڵ�3�β����¼�������λ��������WY
	ECap3Regs.ECCTL1.bit.CTRRST4 = 1; //�ڵ�4�β����¼�������λ��������WY
	ECap3Regs.ECCTL2.bit.SYNCI_EN = 0; //ʧ������ͬ���źš�WY
	ECap3Regs.ECCTL2.bit.SYNCO_SEL = 0; //������ͬ���ź���Ϊ���ͬ���źš�WY
	ECap3Regs.ECCTL1.bit.CAPLDEN = 1; //ʹ��ʱ�����¼��WY
	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1; //������������WY
	ECap3Regs.ECCTL2.bit.REARM = 1; //��λECAP-3��WY
	ECap3Regs.ECCTL1.bit.CAPLDEN = 1; //ʹ��ʱ�����¼��WY
	ECap3Regs.ECEINT.bit.CEVT4 = 1;//����4�β����¼���Ϊ�ж�Դ��WY
	ECap3Regs.ECEINT.bit.CEVT3 = 1;//����3�β����¼���Ϊ�ж�Դ��WY
	ECap3Regs.ECEINT.bit.CEVT2 = 1;//����2�β����¼���Ϊ�ж�Դ��WY
	ECap3Regs.ECEINT.bit.CEVT1 = 1;//����1�β����¼���Ϊ�ж�Դ��WY

	ECap1Regs.ECCLR.bit.CEVT1 = 1; //�����1�β����¼���־λ��WY
	ECap1Regs.ECCLR.bit.CEVT2 = 1; //�����2�β����¼���־λ��WY
	ECap1Regs.ECCLR.bit.CEVT3 = 1; //�����3�β����¼���־λ��WY
	ECap1Regs.ECCLR.bit.CEVT4 = 1; //�����4�β����¼���־λ��WY

	ECap2Regs.ECCLR.bit.CEVT1 = 1; //�����1�β����¼���־λ��WY
	ECap2Regs.ECCLR.bit.CEVT2 = 1; //�����2�β����¼���־λ��WY
	ECap2Regs.ECCLR.bit.CEVT3 = 1; //�����3�β����¼���־λ��WY
	ECap2Regs.ECCLR.bit.CEVT4 = 1; //�����4�β����¼���־λ��WY

	ECap3Regs.ECCLR.bit.CEVT1 = 1; //�����1�β����¼���־λ��WY
	ECap3Regs.ECCLR.bit.CEVT2 = 1; //�����2�β����¼���־λ��WY
	ECap3Regs.ECCLR.bit.CEVT3 = 1; //�����3�β����¼���־λ��WY
	ECap3Regs.ECCLR.bit.CEVT4 = 1; //�����4�β����¼���־λ��WY

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
 *	���ܣ��������岶��ECAP��������š�WY
 */
void InitECapGpio()
{
    InitECap1Gpio(94); //A��������
    InitECap2Gpio(85); //B��������
    InitECap3Gpio(97); //C��������
}

/*
 * ���ܣ��������岶��ECAP-1�����������š�WY
 * �������pin��������������
 */
void InitECap1Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT7SELECT = pin; //�������ŵ���������ECAP-1
    EDIS;
}

/*
 * ���ܣ��������岶��ECAP-2�����������š�WY
 * �������pin��������������
 */
void InitECap2Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT8SELECT = pin; //�������ŵ���������ECAP-2
    EDIS;
}

/*
 * ���ܣ��������岶��ECAP-3�����������š�WY
 * �������pin��������������
 */
void InitECap3Gpio(Uint16 pin)
{
    EALLOW;
    InputXbarRegs.INPUT9SELECT = pin; //�������ŵ���������ECAP-3
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
