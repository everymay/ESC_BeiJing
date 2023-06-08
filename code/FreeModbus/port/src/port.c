#include "port.h"
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"
#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"
#include "VariblesRef.h"
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/Error.h>
UCHAR MusRegNum	=1;
UCHAR MasterReadSendEanble = 0;
UCHAR MasterWriteSendEanble = 0;
UCHAR MucSndAddr = 1;
UCHAR MusRegAddr = 0;
UCHAR MusNRegs = 0;
USHORT URegData[2];
USHORT * MpusDataBuffer = URegData;
Hwi_Handle hwiSCIRD,hwiSCITD,hwiSCIRB,hwiSCITB,hwiSCIRC,hwiSCITC;
Hwi_Params SCI_Params;
Error_Block eb;

void __assert(const char * x1, const char * x2, int x3)
{
	(void)x3;
}
//以下代码是关于全局中断使能和关闭
void EnterCriticalSection(  )//关闭全局中断
{
}

void ExitCriticalSection(  )//使能全局中断
{
}
void INTX_DISABLE(void)
{
}
void  INTX_ENABLE(void)
{
}

/*********************************************Main Control*****************************************/
Uint32 /*SCIDBUAD,*/SCICBUAD;
void InitModbusSCI(void){
	//注意:每个SCI口 主从模式的状态机只能进一个,例:SCIC做从机 虽然没有使能和初始化主机 但也绝对不允许进入主机的状态机
	UCHAR /*SlaveIDSCID,*/SlaveIDSCIB,SlaveIDSCIC;
//	Uint32 /*SCIDBUAD,*/SCICBUAD;

	Error_init(&eb);
    Hwi_Params_init(&SCI_Params);
    SCI_Params.maskSetting = Hwi_MaskingOption_SELF;
    SCI_Params.enableInt = TRUE;

    SCICMASTERENABLE = 0;  //蓝牙
    memcpy(&SCIC,&ModbusSciDefaults[0],sizeof(SCIC));
    hwiSCIRC = Hwi_create(92, serial_Rx_isr1, &SCI_Params, &eb);
    hwiSCITC = Hwi_create(93, serial_Tx_isr1, &SCI_Params, &eb);

//    SCIDMASTERENABLE = 0; //  后台 485-1
//    memcpy(&SCID,&ModbusSciDefaults[2],sizeof(SCID));
//    hwiSCIRD = Hwi_create(94, serial_Rx_isr3, &SCI_Params, &eb);
//    hwiSCITD = Hwi_create(95, serial_Tx_isr3, &SCI_Params, &eb);
                          //  液晶屏 485-2
	hwiSCIRB = Hwi_create(98, serial_Rx_isr2, &SCI_Params, &eb);
	hwiSCITB = Hwi_create(99, serial_Tx_isr2, &SCI_Params, &eb);

	if (Error_check(&eb)) {
        //       System_abort("Hwi create failed");
    }


    SlaveIDSCIC  = MuFaciID.APF_ID + 0x10;SCICBUAD = 115200;   //WIFI波特率为115200,蓝牙波特率为9600;
    SCIC.SendTrsformDelayTimCont = 4;

//    SlaveIDSCID = MuFaciID.APF_ID + 0x10;SCIDBUAD = 115200;
//    SCID.SendTrsformDelayTimCont = 2;

	SCIB.SendTrsformDelayTimCont = 2;
	SlaveIDSCIB =  MuFaciID.APF_ID + 0x10;

	InitEnableModbus_Slaver(&SCIC,SlaveIDSCIC,SCICBUAD);
//	InitEnableModbus_Slaver(&SCID,SlaveIDSCID,SCIDBUAD);
	InitEnableModbus_Slaver(&SCIB,SlaveIDSCIB,115200);

    Hwi_enableInterrupt(92);
    Hwi_enableInterrupt(93);
//    Hwi_enableInterrupt(94);
//    Hwi_enableInterrupt(95);
    Hwi_enableInterrupt(98);
    Hwi_enableInterrupt(99);
}

void InitEnableModbus_Master(struct ModbusSlaveStru *pStu,Uint32 Baund)
{
	eMBMasterInit(pStu, MB_RTU, 1, Baund, MB_PAR_NONE);
	eMBMasterEnable(pStu);
}

void InitEnableModbus_Slaver(struct ModbusSlaveStru *pStu,UCHAR SlaveID,Uint32 Baund)
{
	eMBInit(pStu,MB_RTU, SlaveID, 1, Baund, MB_PAR_NONE);
	eMBEnable(pStu);
}

void eMBModbus_Slaver_poll(struct ModbusSlaveStru *pStu)//主机(从机)轮询
{
	eMBPoll(pStu);    //从机(芯片)状态机
}

void eMBModbus_Master_poll(struct ModbusSlaveStru *pStu)//主机(从机)轮询
{
	if(MasterReadSendEanble == 1)//读取24组无功电容状态
	{
//	    MusRegAddr = 0x9040;MusNRegs = 24;
		eMBMasterReqReadHoldingRegister(pStu, MucSndAddr, 0x9040, CapGroup,3);//0x03
		MasterReadSendEanble = 0;
	}
	else if(MasterWriteSendEanble == 1)//电容器组投切
	{
		eMBMasterReqWriteMultipleHoldingRegister(pStu, MucSndAddr,MusRegAddr,MusRegNum, MpusDataBuffer, 3 );//0x10
		MasterWriteSendEanble = 0;
	}
	eMBMasterPoll(pStu);
}
