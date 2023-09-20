#include "Modbus.h"
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "VariblesRef.h"
#include "DSP28x_Project.h"
#include "DCL.h"
#include "Spi_Frame.h"
// DSP2833x_SysCtrl.c �Ѿ�������LSPCLK����ʱ
//#pragma CODE_SECTION(SciB_Run, "ramfuncs");
//#pragma CODE_SECTION(SciDeal_Run, "ramfuncs");
//#define APF_ID  0x10        // ��Ԫ��ַ�޸�,0x10 - 0x17 ֧��8����ַ
#define	RS232_RX_TIMEOUT		5	//(5*CTRLFREQUENCY/1000) = 5ms

#define EEPROM_HARM_ADDR 0x1060
#define EEPROM_GENH_ADDR (EEPROM_HARM_ADDR+6*(EEPROM_HARM_STEP)+6*(EEPROM_HARM_STEP)) //0x1108
#define EEPROM_HARM_STEP 14

#define Sci1Regs SciaRegs
#define Sci2Regs ScibRegs
#define Sci3Regs ScicRegs
//#define SCI3_TX_ENABLE  GpioDataRegs.GPBSET.bit.GPIO37
//#define SCI3_RX_ENABLE  GpioDataRegs.GPBCLEAR.bit.GPIO37
//#define SCI2_TX_ENABLE  GpioDataRegs.GPBSET.bit.GPIO53
//#define SCI2_RX_ENABLE  GpioDataRegs.GPBCLEAR.bit.GPIO53

//void SciDeal_Run(struct STRU_SCI * pC);
//void SCITtoR(struct STRU_SCI *pStu);
//void SCICommuStateTransition(struct STRU_SCI *pStu,Uint16 state);
//volatile Uint16 * getHarmParam2(volatile Uint16 *pSCIdealT,int step);

//#pragma CODE_SECTION(ModbusScicTxBuf, "ramfuncs");
//#pragma CODE_SECTION(SCICommuStateTransition, "ramfuncs");
//#pragma CODE_SECTION(SCITtoR, "ramfuncs");
//#pragma CODE_SECTION(Sci_Tx, "ramfuncs");
//#pragma CODE_SECTION(GetSciTxBuf, "ramfuncs");
//#pragma CODE_SECTION(SciTxFirstDeal, "ramfuncs");
//#pragma CODE_SECTION(Sci_Run, "ramfuncs");
//#pragma CODE_SECTION(SCITXINTA_ISR, "ramfuncs");
//#pragma CODE_SECTION(SCITXINTB_ISR, "ramfuncs");
//#pragma CODE_SECTION(SCITXINTC_ISR, "ramfuncs");

#define SET9600BAUD     "AT+BAUD4"
#define GET9600BAUDOK   "OK9600\0\0"
#define SET115200BAUD   "AT+BAUD8"
#define GET115200BAUDOK "OK115200"
const char WirelessBaudSet[4][9]={SET9600BAUD,GET9600BAUDOK,SET115200BAUD,GET115200BAUDOK};

#define DATA2BUF1(data)  *pSCIdealT++=(int16)(data)>>8;  *pSCIdealT++=(int16)(data)&0x00ff
#define DATA2BUF1U2(data) *pSCIdealT++=(Uint16)(data)>>8; *pSCIdealT++=(Uint16)(data)&0x00ff

typedef union{	float flt;
				Uint32 intig;
				}FDATA;
typedef union{	Uint32 flt;
				Uint32 intig;
				}DDATA;

/*
 * sci Asynchronous Baud = LSPCLK/((BRR + 1)*8)
 * Table 1-5. Asynchronous Baud Register Values for Common SCI Bit Rates
 * LSPCLK Clock Frequency, 37.5 MHz
 * Ideal Baud         BRR                       Actual Baud         % Error
 *     9600           2603.166667 HEX:(9731h)   9600.614439         6.4E-5
 *   115200           216.0138889 HEX:(D8h)     115207.37327        6.4E-5
 */
//
// Calculate BRR: 7-bit baud rate register value
// SPI CLK freq = 500 kHz
// LSPCLK freq  = CPU freq / 4  (by default)
// BRR          = (LSPCLK freq / SPI CLK freq) - 1
//
#if CPU_FRQ_200MHZ
#define SCI_BAUD_9600        ((200E6 / 1) / 9600) - 1
#define SCI_BAUD_115200        ((200E6 / 1) / 115200) - 1
#endif

#if CPU_FRQ_150MHZ
#define SPI_BRR        ((150E6 / 4) / 500E3) - 1
#endif

void InitSci(struct STRU_SCI *pStu,Uint16 baud)				//fifo
{
	pStu->pReg->SCICCR.all =0x0007;       	// 7:0  1������λ
										// 6:0
										// 5:0  ��������żУ��
										// 4:0  �������ڲ�SCI���� �ڲ�TXD��RXD����
										// 3:0  ������ģʽ
										// 2-7  ����λ����Ϊ8λ
	pStu->pReg->SCICTL1.all =0x0003;      	// ʹ�ܷ��ͺͽ��չ���
	pStu->pReg->SCICTL2.all =0x0000;
	if(baud == 1){
        pStu->pReg->SCIHBAUD.all = 0x0000;      //����������Ϊ115200 00,D8
        pStu->pReg->SCILBAUD.all = 0x00D8;      //
	}else if(baud == 0){
        pStu->pReg->SCIHBAUD.all = 0x000A;      //����������Ϊ9600 97,31
        pStu->pReg->SCILBAUD.all = 0x002B;      //
	}
 	pStu->pReg->SCIFFTX.all=0xE000;     	//ʹ��FIFO
	pStu->pReg->SCIFFRX.all=0x2000;			//Sci1Regs.SCIFFRX.all=0x202F;
	pStu->pReg->SCIFFCT.all=0x00;
}


//---------------------------����GPIO��ʼ������----------------------//
void InitSciGpio(void)
{
	EALLOW;
//--------------------��д��ɺ��ݲ�ʹ��,����Ϊ����IO------------------//
    GpioCtrlRegs.GPCPUD.bit.GPIO84      = 0;    // ʹ����������
    GpioCtrlRegs.GPCPUD.bit.GPIO85      = 0;
    GpioCtrlRegs.GPCMUX2.bit.GPIO84     = 0;    // ��дSCITXDA
    GpioCtrlRegs.GPCMUX2.bit.GPIO84     = 0;    // ��дSCITXDA
    GpioCtrlRegs.GPCMUX2.bit.GPIO85     = 0;    // ��дSCIRXDA
    GpioCtrlRegs.GPCMUX2.bit.GPIO85     = 0;    // ��дSCIRXDA

//--------------------����-------------------------------//
	GpioCtrlRegs.GPDPUD.bit.GPIO106     = 0;
	GpioCtrlRegs.GPDPUD.bit.GPIO107     = 0;
    GpioCtrlRegs.GPDGMUX1.bit.GPIO106   = 1;    // ����SCITXDC
    GpioCtrlRegs.GPDMUX1.bit.GPIO106    = 2;    // ����SCITXDC
    GpioCtrlRegs.GPDGMUX1.bit.GPIO107   = 1;    // ����SCIRXDC
    GpioCtrlRegs.GPDMUX1.bit.GPIO107    = 2;    // ����SCIRXDC

//--------------------��̨485-1-------------------------//
	GpioCtrlRegs.GPAPUD.bit.GPIO22      = 0;
	GpioCtrlRegs.GPAPUD.bit.GPIO23      = 0;
	GpioCtrlRegs.GPAPUD.bit.GPIO26      = 0;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO22    = 0;    // SCITXDB
    GpioCtrlRegs.GPAMUX2.bit.GPIO22     = 3;    // SCITXDB
    GpioCtrlRegs.GPAGMUX2.bit.GPIO23    = 0;    // SCIRXDB
    GpioCtrlRegs.GPAMUX2.bit.GPIO23     = 3;    // SCIRXDB
    GpioCtrlRegs.GPAGMUX2.bit.GPIO26    = 0;    // ͨ�ÿ�
    GpioCtrlRegs.GPAMUX2.bit.GPIO26     = 0;    // ͨ�ÿ�
	GpioCtrlRegs.GPADIR.bit.GPIO26      = 1;    // �������

//--------------------Һ����485-2-------------------------//
//	GpioCtrlRegs.GPDPUD.bit.GPIO104     = 0;
//	GpioCtrlRegs.GPDPUD.bit.GPIO105     = 0;
//	GpioCtrlRegs.GPDPUD.bit.GPIO103     = 0;
//    GpioCtrlRegs.GPDGMUX1.bit.GPIO104   = 1;    // SCITXDD
//    GpioCtrlRegs.GPDMUX1.bit.GPIO104    = 2;    // SCITXDD
//    GpioCtrlRegs.GPDGMUX1.bit.GPIO105   = 1;    // SCIRXDD
//    GpioCtrlRegs.GPDMUX1.bit.GPIO105    = 2;    // SCIRXDD
//    GpioCtrlRegs.GPDGMUX1.bit.GPIO103   = 0;    // ͨ�ÿ�
//    GpioCtrlRegs.GPDMUX1.bit.GPIO103    = 0;    // ͨ�ÿ�
//	GpioCtrlRegs.GPDDIR.bit.GPIO103     = 1;    // �������
	EDIS;
}

void ReadSpiPara(void)
{
	int err = 0, cnt = 0;
	do
	{
		err |= UserHarmnoicReading();  			//�û�г��
		err |= HarmnoicParamReading();  		//����г��
		err |= ManufacturerParametersReading();	//��������
		err |= UserPreferencesReading();		//�û�����
		err |= CapParamReading();				//����Ͷ��
		err |= IDPreferencesReading();			//��ַ����
		err |= VirtuPreferencesReading();		//��ƫ����
		err |= FlashPreferencesReading();		//����¼��
	}
	while((cnt ++ < 5) && err);

	if(err)
	{
		ESCFlagA.FAULTCONFIRFlag = 1;
		ESCFlagB.FAULTCONFIRFlag = 1;
		ESCFlagC.FAULTCONFIRFlag = 1;
		if(softwareFaultWord3.B.ESCTieDianReadErrFlag == 0)
		{
			ESCFlagA.ESCCntMs.StartDelay = 0;
			ESCFlagB.ESCCntMs.StartDelay = 0;
			ESCFlagC.ESCCntMs.StartDelay = 0;
			softwareFaultWord3.B.ESCTieDianReadErrFlag = THREEPHASEFaultDetect(SOE_GP_FAULT + 37, CNT_PARAM_STORAGE_EEPROM, 0);
		}
	}
	else
	{		//���ڴ˼���EEPROM�����Ĵ�����
		SetFaultDelayCounter(CNT_PARAM_STORAGE_EEPROM, 1);
		softwareFaultWord3.B.ESCTieDianReadErrFlag = 0;
	}
//	FlashPreferencesRefresh();              //����¼������
	UserPreferencesRefresh();				//�����û�����(��)
	ManufacturerParametersRefresh();		//���¹�������(��)
	CapParamRefresh();						//���µ�������
	IDParametersRefresh();					//���µ�ַ����
	VirtuPreferencesRefresh();				//������ƫ���úͲ�������
	if(StateFlag.isHarmCompensateMode)
		HarmnoicCorrection();	//����г������
	InitCtrlParam(1);
}

//   -------------------  NO MORE -----------------------------//
