#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "MarcoDefine.h"
#include "VariblesRef.h"
#include "DSP28x_Project.h"
#include "CoreCtrl.h"
#include "stdio.h"
#include "VarMean.h"
#include "Flash2833x_API_Library.h"
#include "F2837xS_GlobalPrototypes.h"
#include <xdc/runtime/Log.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include "F021_F2837xS_C28x.h"
#include "DPLL.h"

#pragma	CODE_SECTION(EPWM7INT ,"ram2func")

/*
 * �ļ��汾��V2023.8.12
 * �޸����ڣ�2023��8��12��
 */

/*���Գ���WY*/

void main(void)
{
	InitSysCtrl(); //��ʼ��ϵͳ��WY

	/*�������Flash������RAM��WY*/
	MemCopy(&dclfuncsLoadStart, &dclfuncsLoadEnd, &dclfuncsRunStart);
	MemCopy(&mathTablesLoadStart, &mathTablesLoadEnd, &mathTablesRunStart);
	MemCopy(&Cla1funcsLoadStart, &Cla1funcsLoadEnd, &Cla1funcsRunStart);
	MemCopy(&Cla1ConstLoadStart, &Cla1ConstLoadEnd, &Cla1ConstRunStart);

    InitFlash_Bank0(); //��ʼ��Flash��WY
    CLA_configClaMemory(); //����CLA�洢�ռ䡣ע�⣺����Ŀ��δʹ��CLA��WY
    CLA_initCpu1Cla1(); //��ʼ��CLA��ע�⣺����Ŀ��δʹ��CLA��WY

    InitSciGpio(); //����SCI�����ڣ���WY
	InitCapGroupAct(); //�˺��������Ͷ����أ�ɾ������WY
    InitECap(); //����ECPA�����岶�񣩡�WY
    InitECapGpio(); //����ECAP�����岶��������š�WY

    SET_SPIWPn(1);
	InitSpiGpio(); //��ʼ��SPI������š�WY
    InitSpib(); //��ʼ��SPI-B��WY
    InitSpic(); //��ʼ��SPI-C��WY

	InitGpio_NewBoard(); //����GPIO��WY
    InitBeforeReadROM(); //��ȡ�������ǰ�ĳ�ʼ��������WY
	OutputGpioInit_NEWBOARD(); //��ʼ��GPIO��WY
	ReadSpiPara(); //����洢оƬ��WY

    initDPLL(&SPLL[0]); //����ϵ����ʼ����λ��һ��Ҫ���������ȡ����֮��.
    initDPLL(&SPLL[1]); //����alfbet���࣬���������
    initDPLL(&SPLL[2]);

    InitAfterReadROM(); //��ȡ�����ĳ�ʼ��������WY
	InitModbusSCI(); //��ʼ��Modbus��WY
	InitCtrlParam(0); //��ʼ��PI������������WY
	InitParameters();	//Initialization of key parameters
	DMAInitialize(); //����Ŀδ����DMA��WY
    InitAdc(); //����ADC��WY
    InitEPwmGpio(); //����EPWM��ص����š�WY
    InitEPwm(); //����EPWM��WY

	Log_info1("start bios====%d", 0); //��ӡ��־
    BIOS_start(); //����BIOS��WY
}

/* ί��ʵ����Խ��У�1��Ե 2���� 3���� 4���� 5���� 6�����ѹ�仯 7���Ƶ�ʱ仯ʵ�飨��ѡ��
 * 					8��г�������� 9����������� 10��Ĳ��� 11��Ӧʱ����� 12��������
 * 					13��ż���ʵ�� 14��Ƿ���ʵ�� 15�ṹ�����
 */

/*���ж�δʹ�á�WY*/
void  TINT0_ISR(void)
{
    StopCpuTimer0();
}

/*���ж�δʹ�á�WY*/
void EPWM7INT(void)
{
	EPwm7Regs.ETCLR.bit.INT = 1;		//epwm4�����жϸ�λ
}

/*���ж�δʹ�á�WY*/
interrupt void ECAP4_INT_ISR(void)
{
	ECap4Regs.ECCLR.bit.CEVT1 = 1;
	ECap4Regs.ECCLR.bit.INT = 1;
	ECap4Regs.ECCTL2.bit.REARM = 1;
}


void initCodeCopy(void)
{
    DisableDog();
    MemCopy(&knlLoadStart,&knlLoadEnd,&knlRunStart);
    MemCopy(&ram2funcLoadStart,&ram2funcLoadEnd,&ram2funcRunStart);
    MemCopy(&RamfuncsLoadStart,&RamfuncsLoadEnd,&RamfuncsRunStart);
}




