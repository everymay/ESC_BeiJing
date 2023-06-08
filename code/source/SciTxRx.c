#include "Modbus.h"
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "VariblesRef.h"
#include "DSP28x_Project.h"
#include "DCL.h"
#include "Spi_Frame.h"
// DSP2833x_SysCtrl.c 已经配置由LSPCLK来计时
//#pragma CODE_SECTION(SciB_Run, "ramfuncs");
//#pragma CODE_SECTION(SciDeal_Run, "ramfuncs");
//#define APF_ID  0x10        // 单元地址修改,0x10 - 0x17 支持8个地址
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
	pStu->pReg->SCICCR.all =0x0007;       	// 7:0  1个结束位
										// 6:0
										// 5:0  不进行奇偶校验
										// 4:0  不采用内部SCI测试 内部TXD与RXD连接
										// 3:0  空闲线模式
										// 2-7  数据位个数为8位
	pStu->pReg->SCICTL1.all =0x0003;      	// 使能发送和接收功能
	pStu->pReg->SCICTL2.all =0x0000;
	if(baud == 1){
        pStu->pReg->SCIHBAUD.all = 0x0000;      //波特率设置为115200 00,D8
        pStu->pReg->SCILBAUD.all = 0x00D8;      //
	}else if(baud == 0){
        pStu->pReg->SCIHBAUD.all = 0x000A;      //波特率设置为9600 97,31
        pStu->pReg->SCILBAUD.all = 0x002B;      //
	}
 	pStu->pReg->SCIFFTX.all=0xE000;     	//使用FIFO
	pStu->pReg->SCIFFRX.all=0x2000;			//Sci1Regs.SCIFFRX.all=0x202F;
	pStu->pReg->SCIFFCT.all=0x00;
}


/*
 * 功能：配置SCI（串口）相关的GPIO。WY
 */
void InitSciGpio(void)
{
	EALLOW;

	/*
	 * SCI-A相关GPIO配置
	 * SCI-A可用作串口烧录。烧录程序完成后，用作GPIO输入模式
	 * WY
	 */
	GpioCtrlRegs.GPCPUD.bit.GPIO84 = 0; //引脚SCI-A-TX上拉模式。WY
	GpioCtrlRegs.GPCPUD.bit.GPIO85 = 0; //引脚SCI-A-RX上拉模式。WY
	GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 0; //引脚复用为SCI-A-TX。WY
	GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 0; //引脚复用为SCI-A-TX。WY
	GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 0; //引脚复用为SCI-A-RX。WY
	GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 0; //引脚复用为SCI-A-RX。WY

	/*
	 * SCI-D相关GPIO配置
	 * SCI-D用作蓝牙通信
	 * WY
	 */
	GpioCtrlRegs.GPDPUD.bit.GPIO106 = 0; //引脚SCI-D-TX上拉模式。WY
	GpioCtrlRegs.GPDPUD.bit.GPIO107 = 0; //引脚SCI-D-RX上拉模式。WY
	GpioCtrlRegs.GPDGMUX1.bit.GPIO106 = 1; //引脚复用SCI-D-TX。WY
	GpioCtrlRegs.GPDMUX1.bit.GPIO106 = 2; //引脚复用SCI-D-TX。WY
	GpioCtrlRegs.GPDGMUX1.bit.GPIO107 = 1; //引脚复用SCI-D-RX。WY
	GpioCtrlRegs.GPDMUX1.bit.GPIO107 = 2; //引脚复用SCI-D-RX。WY

	/*
	 * SCI-B相关GPIO配置
	 * SCI-B用作后台RS485
	 */
	GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0; //引脚SCI-B-TX上拉模式。WY
	GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0; //引脚SCI-B-RX上拉模式。WY
	GpioCtrlRegs.GPAPUD.bit.GPIO26 = 0; //SCI-B-RS485使能引脚上拉。WY
	GpioCtrlRegs.GPAGMUX2.bit.GPIO22 = 0; //引脚复用为SCI-B-TX。WY
	GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 3; //引脚复用为SCI-B-TX。WY
	GpioCtrlRegs.GPAGMUX2.bit.GPIO23 = 0; //引脚复用为SCI-B-RX。WY
	GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 3; //引脚复用为SCI-B-RX。WY
	GpioCtrlRegs.GPAGMUX2.bit.GPIO26 = 0; //失能引脚复用。WY
	GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0; //失能引脚复用。WY
	GpioCtrlRegs.GPADIR.bit.GPIO26 = 1; //输出模式。WY

	EDIS;
}

void ReadSpiPara(void)
{
	int err =0,cnt=0;
	do{
		err |= UserHarmnoicReading();  			//用户谐波
		err |= HarmnoicParamReading();  		//厂家谐波
		err |= ManufacturerParametersReading();	//工厂设置
		err |= UserPreferencesReading();		//用户设置
		err |= CapParamReading();				//电容投切
		err |= IDPreferencesReading();			//地址设置
		err |= VirtuPreferencesReading();		//零偏设置
		err |= FlashPreferencesReading();		//故障录波
	}while((cnt++ < 5) && err);

	if(err){
	    ESCFlagA.FAULTCONFIRFlag = 1;
	    ESCFlagB.FAULTCONFIRFlag = 1;
	    ESCFlagC.FAULTCONFIRFlag = 1;
		if(softwareFaultWord3.B.ESCTieDianReadErrFlag == 0){
            ESCFlagA.ESCCntMs.StartDelay = 0;
            ESCFlagB.ESCCntMs.StartDelay = 0;
            ESCFlagC.ESCCntMs.StartDelay = 0;
			softwareFaultWord3.B.ESCTieDianReadErrFlag = THREEPHASEFaultDetect(SOE_GP_FAULT+37,CNT_PARAM_STORAGE_EEPROM,0);
		}
	}else{		//可在此加入EEPROM错误后的处理函数
		SetFaultDelayCounter(CNT_PARAM_STORAGE_EEPROM,1);
		softwareFaultWord3.B.ESCTieDianReadErrFlag = 0;
	}
//	FlashPreferencesRefresh();              //故障录波设置
	UserPreferencesRefresh();				//更新用户设置(新)
	ManufacturerParametersRefresh();		//更新工厂设置(新)
	CapParamRefresh();						//更新电容设置
	IDParametersRefresh();					//更新地址设置
    VirtuPreferencesRefresh();				//更新零偏设置和采样参数
	if(StateFlag.isHarmCompensateMode)		HarmnoicCorrection();	//更新谐波设置
	InitCtrlParam(1);
}

//   -------------------  NO MORE -----------------------------//
