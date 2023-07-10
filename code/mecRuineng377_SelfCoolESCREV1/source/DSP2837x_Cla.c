/*
 * DSP2837x_Cla.c
 *
 *  Created on: 2017年6月19日
 *      Author: Administrator
 */
#include "F2837xS_device.h"     // Headerfile Include File
#include "F2837xS_Examples.h"   // Examples Include File
#include "F2837xS_Cla_defines.h"
#include "F2837xS_epwm.h"
#pragma DATA_SECTION(mdebugen,"CLADataLS01");
#pragma DATA_SECTION(cladbg1,"CLADataLS01");
long mdebugen=0;
long cladbg1=0;
#pragma DATA_SECTION(CLATestValue,                       "CLADataLS01");

#pragma DATA_SECTION(Esc_PhaseA,                "CLADataLS01");
#pragma DATA_SECTION(Esc_PhaseB,                "CLADataLS01");
#pragma DATA_SECTION(Esc_PhaseC,                "CLADataLS01");
#pragma DATA_SECTION(Esc_VoltPhaseA,            "CLADataLS01");
#pragma DATA_SECTION(Esc_VoltPhaseB,            "CLADataLS01");
#pragma DATA_SECTION(Esc_VoltPhaseC,            "CLADataLS01");
#pragma DATA_SECTION(VirtulAD_loadCurrentA,     "CLADataLS01");
#pragma DATA_SECTION(VirtulAD_loadCurrentB,     "CLADataLS01");
#pragma DATA_SECTION(VirtulAD_loadCurrentC,     "CLADataLS01");
#pragma DATA_SECTION(CurrDirA,                  "CLADataLS01");
#pragma DATA_SECTION(CurrDirB,                  "CLADataLS01");
#pragma DATA_SECTION(CurrDirC,                  "CLADataLS01");
#pragma DATA_SECTION(T1PRmulESC_DutyDataA,      "CLADataLS01");
#pragma DATA_SECTION(T1PRmulESC_DutyDataB,      "CLADataLS01");
#pragma DATA_SECTION(T1PRmulESC_DutyDataC,      "CLADataLS01");
#pragma DATA_SECTION(T1PRmulESC_DutyDataAaddnegCurCompPerc,                  "CLADataLS01");
#pragma DATA_SECTION(T1PRmulESC_DutyDataBaddnegCurCompPerc,                  "CLADataLS01");
#pragma DATA_SECTION(T1PRmulESC_DutyDataCaddnegCurCompPerc,                  "CLADataLS01");
#pragma DATA_SECTION(T1PRmulESC_DutyDataAsubnegCurCompPerc,                  "CLADataLS01");
#pragma DATA_SECTION(T1PRmulESC_DutyDataBsubnegCurCompPerc,                  "CLADataLS01");
#pragma DATA_SECTION(T1PRmulESC_DutyDataCsubnegCurCompPerc,                  "CLADataLS01");
#pragma DATA_SECTION(EPwm8RegsCMPA,             "CLADataLS01");
#pragma DATA_SECTION(EPwm8RegsCMPB,             "CLADataLS01");
#pragma DATA_SECTION(EPwm7RegsCMPA,             "CLADataLS01");
#pragma DATA_SECTION(EPwm7RegsCMPB,             "CLADataLS01");
#pragma DATA_SECTION(EPwm6RegsCMPA,             "CLADataLS01");
#pragma DATA_SECTION(EPwm6RegsCMPB,             "CLADataLS01");
#pragma DATA_SECTION(EPwm5RegsCMPA,             "CLADataLS01");
#pragma DATA_SECTION(EPwm5RegsCMPB,             "CLADataLS01");
#pragma DATA_SECTION(EPwm4RegsCMPA,             "CLADataLS01");
#pragma DATA_SECTION(EPwm4RegsCMPB,             "CLADataLS01");
#pragma DATA_SECTION(EPwm3RegsCMPA,             "CLADataLS01");
#pragma DATA_SECTION(EPwm3RegsCMPB,             "CLADataLS01");
#pragma DATA_SECTION(T1PRmulESC_T1PR,           "CLADataLS01");

#pragma DATA_SECTION(EPwm8RegsAQCSFRC,           "CLADataLS01");
#pragma DATA_SECTION(EPwm7RegsAQCSFRC,           "CLADataLS01");
#pragma DATA_SECTION(EPwm6RegsAQCSFRC,           "CLADataLS01");
#pragma DATA_SECTION(EPwm5RegsAQCSFRC,           "CLADataLS01");
#pragma DATA_SECTION(EPwm4RegsAQCSFRC,           "CLADataLS01");
#pragma DATA_SECTION(EPwm3RegsAQCSFRC,           "CLADataLS01");

#pragma DATA_SECTION(CLAtest1,                  "CLADataLS01");
#pragma DATA_SECTION(CLAtest2,                  "CLADataLS01");
//#pragma DATA_SECTION(CLAtest3,                "CLADataLS01");
//#pragma DATA_SECTION(CLAtest4,                "CLADataLS01");


#pragma DATA_SECTION(AdcaResultRegsADCRESULT14,             "CLADataLS01");
#pragma DATA_SECTION(AdcaResultRegsADCRESULT15,             "CLADataLS01");
#pragma DATA_SECTION(AdcbResultRegsADCRESULT14,             "CLADataLS01");
#pragma DATA_SECTION(AdcbResultRegsADCRESULT15,             "CLADataLS01");
#pragma DATA_SECTION(AdccResultRegsADCRESULT14,             "CLADataLS01");
#pragma DATA_SECTION(AdccResultRegsADCRESULT15,             "CLADataLS01");

#pragma DATA_SECTION(AdccResultRegsADCRESULT12,             "CLADataLS01");
#pragma DATA_SECTION(AdccResultRegsADCRESULT13,             "CLADataLS01");
#pragma DATA_SECTION(AdccResultRegsADCRESULT10,             "CLADataLS01");
#pragma DATA_SECTION(AdccResultRegsADCRESULT8,              "CLADataLS01");
#pragma DATA_SECTION(AdcbResultRegsADCRESULT12,             "CLADataLS01");
#pragma DATA_SECTION(AdcbResultRegsADCRESULT13,             "CLADataLS01");
#pragma DATA_SECTION(AdcbResultRegsADCRESULT10,             "CLADataLS01");
#pragma DATA_SECTION(AdcbResultRegsADCRESULT8,              "CLADataLS01");
#pragma DATA_SECTION(AdcaResultRegsADCRESULT12,             "CLADataLS01");
#pragma DATA_SECTION(AdcaResultRegsADCRESULT13,             "CLADataLS01");
#pragma DATA_SECTION(AdcaResultRegsADCRESULT10,             "CLADataLS01");
#pragma DATA_SECTION(AdcaResultRegsADCRESULT8,              "CLADataLS01");

float Esc_PhaseA = 0,Esc_PhaseB = 0,Esc_PhaseC = 0;

float Esc_VoltPhaseA = 0; //A相相位。WY
float Esc_VoltPhaseB = 0; //B相相位。WY
float Esc_VoltPhaseC = 0; //C相相位。WY

float VirtulAD_loadCurrentA,VirtulAD_loadCurrentB,VirtulAD_loadCurrentC;
int CurrDirA,CurrDirB,CurrDirC;
float CLATestValue = 2.0f;
int32 T1PRmulESC_T1PR;

int32 T1PRmulESC_DutyDataA; //A相PWM周期值。WY
int32 T1PRmulESC_DutyDataB; //A相PWM周期值。WY
int32 T1PRmulESC_DutyDataC; //A相PWM周期值。WY

int32 T1PRmulESC_DutyDataAaddnegCurCompPerc,T1PRmulESC_DutyDataBaddnegCurCompPerc,T1PRmulESC_DutyDataCaddnegCurCompPerc;
int32 T1PRmulESC_DutyDataAsubnegCurCompPerc,T1PRmulESC_DutyDataBsubnegCurCompPerc,T1PRmulESC_DutyDataCsubnegCurCompPerc;

Uint32 *EPwm8RegsCMPA,*EPwm8RegsCMPB,*EPwm7RegsCMPA,*EPwm7RegsCMPB,*EPwm6RegsCMPA,*EPwm6RegsCMPB,*EPwm5RegsCMPA,*EPwm5RegsCMPB,*EPwm4RegsCMPA,*EPwm4RegsCMPB,*EPwm3RegsCMPA,*EPwm3RegsCMPB;
Uint32 *EPwm8RegsAQCSFRC,*EPwm7RegsAQCSFRC,*EPwm6RegsAQCSFRC,*EPwm5RegsAQCSFRC,*EPwm4RegsAQCSFRC,*EPwm3RegsAQCSFRC;

Uint32 *CLAtest1,*CLAtest2;//,*CLAtest3,*CLAtest4;
Uint32 *AdcaResultRegsADCRESULT14,*AdcaResultRegsADCRESULT15,*AdcbResultRegsADCRESULT14,*AdcbResultRegsADCRESULT15,*AdccResultRegsADCRESULT14,*AdccResultRegsADCRESULT15;
Uint32 *AdccResultRegsADCRESULT12,*AdccResultRegsADCRESULT13,*AdccResultRegsADCRESULT10,*AdccResultRegsADCRESULT8;
Uint32 *AdcbResultRegsADCRESULT12,*AdcbResultRegsADCRESULT13,*AdcbResultRegsADCRESULT10,*AdcbResultRegsADCRESULT8;
Uint32 *AdcaResultRegsADCRESULT12,*AdcaResultRegsADCRESULT13,*AdcaResultRegsADCRESULT10,*AdcaResultRegsADCRESULT8;
void CLA_configClaMemory(void)
{
	EALLOW;
	// Initialize and wait for CLA1ToCPUMsgRAM
	MemCfgRegs.MSGxINIT.bit.INIT_CLA1TOCPU = 1;
	while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CLA1TOCPU != 1){};

	// Initialize and wait for CPUToCLA1MsgRAM
	MemCfgRegs.MSGxINIT.bit.INIT_CPUTOCLA1 = 1;
	while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CPUTOCLA1 != 1){};

	// Select LS5RAM to be the programming space for the CLA
	// First configure the CLA to be the master for LS5 and then
	// set the space to be a program block
	MemCfgRegs.LSxMSEL.bit.MSEL_LS0 = 1;
	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS0 = 0;

	MemCfgRegs.LSxMSEL.bit.MSEL_LS1 = 1;            //00: Memory is dedicated to CPU.     01: Memory is shared between CPU and CLA1.
	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS1 = 1;        //0: CLA Data memory. 1: CLA Program memory.

	MemCfgRegs.LSxMSEL.bit.MSEL_LS2 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS2 = 1;


	//Next configure LS0RAM and LS1RAM as data spaces for the CLA
	// First configure the CLA to be the master for LS0(1) and then
    // set the spaces to be code blocks
    MemCfgRegs.LSxMSEL.bit.MSEL_LS3 = 0;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS3 = 0;


	MemCfgRegs.LSxMSEL.bit.MSEL_LS4 = 0;
	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS4 = 0;

	MemCfgRegs.LSxMSEL.bit.MSEL_LS5 = 0;
	MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS5 = 0;
	EDIS;
}

void CLA_initCpu1Cla1(void)
{
	//  Compute all CLA task vectors
	// On Type-1 CLAs the MVECT registers accept full 16-bit task addresses as
	// opposed to offsets used on older Type-0 CLAs
	EALLOW;
	Cla1Regs.MVECT1 = (uint16_t)(&Cla1Task1);
	Cla1Regs.MVECT2 = (uint16_t)(&Cla1Task2);
	Cla1Regs.MVECT3 = (uint16_t)(&Cla1Task3);
	Cla1Regs.MVECT4 = (uint16_t)(&Cla1Task4);
	Cla1Regs.MVECT5 = (uint16_t)(&Cla1Task5);
	Cla1Regs.MVECT6 = (uint16_t)(&Cla1Task6);
//	Cla1Regs.MVECT7 = (uint16_t)(&Cla1Task7);
//	Cla1Regs.MVECT8 = (uint16_t)(&Cla1Task8);

	// Enable IACK instruction to start a task on CLA in software
	// for all  8 CLA tasks
	asm("   RPT #3 || NOP");
    Cla1Regs.MCTL.bit.IACKE = 1;
    Cla1Regs.MIER.all = 0x3F;

    // Configure the vectors for the end-of-task interrupt for all
    // 8 tasks
/*
	PieVectTable.CLA1_1_INT   = &cla1Isr1;
	PieVectTable.CLA1_2_INT   = &cla1Isr2;
	PieVectTable.CLA1_3_INT   = &cla1Isr3;
	PieVectTable.CLA1_4_INT   = &cla1Isr4;
	PieVectTable.CLA1_5_INT   = &cla1Isr5;
	PieVectTable.CLA1_6_INT   = &cla1Isr6;
	PieVectTable.CLA1_7_INT   = &cla1Isr7;
	PieVectTable.CLA1_8_INT   = &cla1Isr8;
*/
    //
    // Set the EPWM1INT as the trigger for task 1
    //
    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK1 = CLA_TRIG_ADCAINT1;     //A相处理
    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK2 = CLA_TRIG_ADCAINT2;
    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK3 = CLA_TRIG_ADCCINT1;     //B相处理
    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK4 = CLA_TRIG_ADCCINT2;
    DmaClaSrcSelRegs.CLA1TASKSRCSEL2.bit.TASK5 = CLA_TRIG_ADCBINT1;     //C相处理
    DmaClaSrcSelRegs.CLA1TASKSRCSEL2.bit.TASK6 = CLA_TRIG_ADCBINT2;

	// Enable CLA interrupts at the group and subgroup levels
	PieCtrlRegs.PIEIER11.all  = 0xFFFF;
//	IER |= (M_INT11 );      //JCLDBG
	EDIS;

	EPwm8RegsCMPA  = (Uint32 *)(&EPwm8Regs.CMPA.all);       //offsetof()
	EPwm8RegsCMPB  = (Uint32 *)(&EPwm8Regs.CMPB.all);       //CLA地址均为32位,所以结构体在CLA中编译时候,遇到int型,地址偏移为16位导致取地址不对
	EPwm7RegsCMPA  = (Uint32 *)(&EPwm7Regs.CMPA.all);       //所以在CPU中编译时候,先取出地址
	EPwm7RegsCMPB  = (Uint32 *)(&EPwm7Regs.CMPB.all);
	EPwm6RegsCMPA  = (Uint32 *)(&EPwm6Regs.CMPA.all);
	EPwm6RegsCMPB  = (Uint32 *)(&EPwm6Regs.CMPB.all);
    EPwm5RegsCMPA  = (Uint32 *)(&EPwm5Regs.CMPA.all);       //offsetof()
    EPwm5RegsCMPB  = (Uint32 *)(&EPwm5Regs.CMPB.all);       //CLA地址均为32位,所以结构体在CLA中编译时候,遇到int型,地址偏移为16位导致取地址不对
    EPwm4RegsCMPA  = (Uint32 *)(&EPwm4Regs.CMPA.all);       //所以在CPU中编译时候,先取出地址
    EPwm4RegsCMPB  = (Uint32 *)(&EPwm4Regs.CMPB.all);
    EPwm3RegsCMPA  = (Uint32 *)(&EPwm3Regs.CMPA.all);
    EPwm3RegsCMPB  = (Uint32 *)(&EPwm3Regs.CMPB.all);

    EPwm8RegsAQCSFRC = (Uint32 *)(&EPwm8Regs.AQCSFRC.all);
    EPwm7RegsAQCSFRC = (Uint32 *)(&EPwm7Regs.AQCSFRC.all);
    EPwm6RegsAQCSFRC = (Uint32 *)(&EPwm6Regs.AQCSFRC.all);
    EPwm5RegsAQCSFRC = (Uint32 *)(&EPwm5Regs.AQCSFRC.all);
    EPwm4RegsAQCSFRC = (Uint32 *)(&EPwm4Regs.AQCSFRC.all);
    EPwm3RegsAQCSFRC = (Uint32 *)(&EPwm3Regs.AQCSFRC.all);

	CLAtest1        = (Uint32 *)(&EPwm5Regs.TBCTR);
	CLAtest2        = (Uint32 *)(&EPwm5Regs.TBSTS.all);
//	CLAtest3
//	CLAtest4
	AdcaResultRegsADCRESULT14 = (Uint32 *)(&AdcaResultRegs.ADCRESULT14);
	AdcaResultRegsADCRESULT15 = (Uint32 *)(&AdcaResultRegs.ADCRESULT15);
    AdcbResultRegsADCRESULT14 = (Uint32 *)(&AdcbResultRegs.ADCRESULT14);
    AdcbResultRegsADCRESULT15 = (Uint32 *)(&AdcbResultRegs.ADCRESULT15);
    AdccResultRegsADCRESULT14 = (Uint32 *)(&AdccResultRegs.ADCRESULT14);
    AdccResultRegsADCRESULT15 = (Uint32 *)(&AdccResultRegs.ADCRESULT15);

    AdccResultRegsADCRESULT12 = (Uint32 *)(&AdccResultRegs.ADCRESULT12);
    AdccResultRegsADCRESULT13 = (Uint32 *)(&AdccResultRegs.ADCRESULT13);
    AdccResultRegsADCRESULT10 = (Uint32 *)(&AdccResultRegs.ADCRESULT10);
    AdccResultRegsADCRESULT8  = (Uint32 *)(&AdccResultRegs.ADCRESULT8);

    AdcbResultRegsADCRESULT12 = (Uint32 *)(&AdcbResultRegs.ADCRESULT12);
    AdcbResultRegsADCRESULT13 = (Uint32 *)(&AdcbResultRegs.ADCRESULT13);
    AdcbResultRegsADCRESULT10 = (Uint32 *)(&AdcbResultRegs.ADCRESULT10);
    AdcbResultRegsADCRESULT8  = (Uint32 *)(&AdcbResultRegs.ADCRESULT8);

    AdcaResultRegsADCRESULT12 = (Uint32 *)(&AdcaResultRegs.ADCRESULT12);
    AdcaResultRegsADCRESULT13 = (Uint32 *)(&AdcaResultRegs.ADCRESULT13);
    AdcaResultRegsADCRESULT10 = (Uint32 *)(&AdcaResultRegs.ADCRESULT10);
    AdcaResultRegsADCRESULT8  = (Uint32 *)(&AdcaResultRegs.ADCRESULT8);



}



