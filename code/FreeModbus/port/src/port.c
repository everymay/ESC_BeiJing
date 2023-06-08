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
//���´����ǹ���ȫ���ж�ʹ�ܺ͹ر�
void EnterCriticalSection(  )//�ر�ȫ���ж�
{
}

void ExitCriticalSection(  )//ʹ��ȫ���ж�
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
	//ע��:ÿ��SCI�� ����ģʽ��״̬��ֻ�ܽ�һ��,��:SCIC���ӻ� ��Ȼû��ʹ�ܺͳ�ʼ������ ��Ҳ���Բ��������������״̬��
	UCHAR /*SlaveIDSCID,*/SlaveIDSCIB,SlaveIDSCIC;
//	Uint32 /*SCIDBUAD,*/SCICBUAD;

	Error_init(&eb);
    Hwi_Params_init(&SCI_Params);
    SCI_Params.maskSetting = Hwi_MaskingOption_SELF;
    SCI_Params.enableInt = TRUE;

    SCICMASTERENABLE = 0;  //����
    memcpy(&SCIC,&ModbusSciDefaults[0],sizeof(SCIC));
    hwiSCIRC = Hwi_create(92, serial_Rx_isr1, &SCI_Params, &eb);
    hwiSCITC = Hwi_create(93, serial_Tx_isr1, &SCI_Params, &eb);

//    SCIDMASTERENABLE = 0; //  ��̨ 485-1
//    memcpy(&SCID,&ModbusSciDefaults[2],sizeof(SCID));
//    hwiSCIRD = Hwi_create(94, serial_Rx_isr3, &SCI_Params, &eb);
//    hwiSCITD = Hwi_create(95, serial_Tx_isr3, &SCI_Params, &eb);
                          //  Һ���� 485-2
	hwiSCIRB = Hwi_create(98, serial_Rx_isr2, &SCI_Params, &eb);
	hwiSCITB = Hwi_create(99, serial_Tx_isr2, &SCI_Params, &eb);

	if (Error_check(&eb)) {
        //       System_abort("Hwi create failed");
    }


    SlaveIDSCIC  = MuFaciID.APF_ID + 0x10;SCICBUAD = 115200;   //WIFI������Ϊ115200,����������Ϊ9600;
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

void eMBModbus_Slaver_poll(struct ModbusSlaveStru *pStu)//����(�ӻ�)��ѯ
{
	eMBPoll(pStu);    //�ӻ�(оƬ)״̬��
}

void eMBModbus_Master_poll(struct ModbusSlaveStru *pStu)//����(�ӻ�)��ѯ
{
	if(MasterReadSendEanble == 1)//��ȡ24���޹�����״̬
	{
//	    MusRegAddr = 0x9040;MusNRegs = 24;
		eMBMasterReqReadHoldingRegister(pStu, MucSndAddr, 0x9040, CapGroup,3);//0x03
		MasterReadSendEanble = 0;
	}
	else if(MasterWriteSendEanble == 1)//��������Ͷ��
	{
		eMBMasterReqWriteMultipleHoldingRegister(pStu, MucSndAddr,MusRegAddr,MusRegNum, MpusDataBuffer, 3 );//0x10
		MasterWriteSendEanble = 0;
	}
	eMBMasterPoll(pStu);
}
