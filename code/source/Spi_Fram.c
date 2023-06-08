#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "VariblesRef.h"
#include "MarcoDefine.h"
#include "Spi_Frame.h"
#include "DSP28x_Project.h"
#include "mbcrc.h"
#include "DCL.h"
#include "DPLL.h"
#include <ti/sysbios/gates/GateHwi.h>
#include <ti/sysbios/knl/Semaphore.h>

//#pragma DATA_SECTION(Information_Structure,"EBSS1");
//#pragma DATA_SECTION(RemoteRefreshData,"EBSS1");
//#pragma DATA_SECTION(RecordData,"EBSS1");
extern GateHwi_Handle gateHwi0;
extern GateHwi_Handle gateHwi1;     //gateHwiSoeRemote;
extern Semaphore_Handle EEPROMResourceLock;

Stru_SOE SOE;
#pragma DATA_SECTION(IncidentRecord,"EBSS3");
Stru_IncidentRecord IncidentRecord[SOE_BUFF_GROUP];
Stru_RemoteRefresh_Structure RemoteRefreshData;
Stru_RemoteCtrl_Structure RemoteStateCtrl;
Stru_RemoteInstruct_Structure RemoteInstructCtrl;
Stru_RemoteMonitor_Structure RemoteStateMonitor;
Stru_Information_Structure Information_Structure={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12311,12312,12313,12314,12315,12316,12317,12318,12319,12320,12321,12322,12323,12324,12325,12326,12327,12328,12329,12330,12331,12332,12333,12334,12335,12336,12337,\
		12338,12339,12340,12341,12342,12343,12344,12345,12346,12347,12348,12349,12350,12351,12352,12353,12354,12355,12356,12357,12358,12359,12360,12361,12362,-12363,-12364,-12365,-12366,-12367,-12368,-12369,-12370,-12371,-12372,-12373,-12374,-12375,-12376,-12377,-12378,-12379,-12380};
Stru_Multiple_Parallel_Value Multiple_Parallel_Value;
Stru_Virtu_ZeroOffset VirtuZeroOffset={2076,2080,2080,2083,2081,2078,2053,2062,2056,0,0,0,0,0};
Stru_Virtu_ZeroOffSETVAL VirtuZeroOffSETVAL={2076,2080,2080,2083,2081,2078,2053,2062,2056,2057,2063,2075,0,0,0};
//#pragma	CODE_SECTION(SpiWrite ,"ram2func")
//#pragma	CODE_SECTION(FM_Write_Enable ,"ram2func")
//#pragma	CODE_SECTION(FM_Read_Byte ,"ram2func")
//#pragma	CODE_SECTION(FM_StructureDatWrite ,"ram2func")
//#pragma	CODE_SECTION(FM_DatRead ,"ram2func")
//#pragma	CODE_SECTION(FM_DatWordWrite ,"ram2func")
#pragma	CODE_SECTION(SPITXINTB_ISR ,"ram2func")
//#pragma	CODE_SECTION(FM_StructureDatWrite2 ,"ram2func")
//#pragma	CODE_SECTION(SoeRecData ,"ram2func")
#define  SPI_TX_OVER()  (SpibRegs.SPISTS.bit.INT_FLAG == 1)
int RecordData[8];
Uint16 *pSoe;
//void InitSpiGpio(void)
//{
//	EALLOW;
//
//	GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0;   // Enable pull-up on GPIO54 (SPISIMOA)
//	GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;   // Enable pull-up on GPIO55 (SPISOMIA)
//	GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;   // Enable pull-up on GPIO56 (SPICLKA)
//
//    GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3; // Asynch input GPIO54 (SPISIMOA)
//    GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3; // Asynch input GPIO55 (SPISOMIA)
//    GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3; // Asynch input GPIO56 (SPICLKA)
//
//    GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1; // Configure GPIO54 as SPISIMOA
//    GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1; // Configure GPIO55 as SPISOMIA
//    GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 1; // Configure GPIO56 as SPICLKA
//
//    //SPI wp,cs
//    GpioCtrlRegs.GPBPUD.bit.GPIO48 = 0;  // Enable pullup on GPIO48
//    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0; // GPIO48 = WP
//    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;  // GPIO48 = output
//    GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0;  // Enable pullup on GPIO57
//    GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0; // GPIO57 = SPICSn
//    GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;  // GPIO57 = output
//
//    EDIS;
//}


//void InitESpi(void)
//{
//   EALLOW;
//   SpiaRegs.SPICCR.all=0x0007;       //16-bit character, No Loopback mode
//   //SpiaRegs.SPICTL.all=0x0017;       //Interrupt enabled, Master/Slave XMIT enabled
//   SpiaRegs.SPICTL.all=0x0006;       //Interrupt disabled
//   //SpiaRegs.SPISTS.all=0x0000;
//   SpiaRegs.SPIBRR.all = 0x04;           // Baud rate  75Mhz /(4 + 1) = 15MHz
//   SpiaRegs.SPIPRI.all = 0x30;	// free run
//   SpiaRegs.SPICCR.all=0x0087;//使SPI退出复位状态
//   SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;
//   SpiaRegs.SPICTL.bit.CLK_PHASE   = 1;
//
//   EDIS;
////   FM_Write_Status(UNLOCK);
//   ConfigCpuTimer(&CpuTimer0, 150, 0.01);
//}


Uint16 SpiWrite(Uint16 data)
{
   while(StateFlag.EEPROMResourceLock&&(CntMs.IntTime<200));
   SpibRegs.SPIFFTX.bit.SPIRST = 0;					//缺此句会导致FIFO偶尔写不进去
   SpibRegs.SPIFFTX.bit.SPIRST = 1;					//缺此句会导致SPI发送失败
   Uint16 ret;
   SpibRegs.SPITXBUF = data;
   while(!SPI_TX_OVER());       //接收完成标志
   ret = SpibRegs.SPIRXBUF;
   return ret;
}


//写使能
void FM_Write_Enable(void)
{
	SET_SPICS(0);
	SpiWrite(WREN);
	SET_SPICS(1);
}

//写禁止
void FM_Write_Disable(void)
{
	SET_SPICS(0);
	SpiWrite(WRDI);
	SET_SPICS(1);
}



Uint16 FM_Read_Status(void)
{
	Uint16 data;
	SET_SPICS(0);
	SpiWrite(RDSR);
	data = SpiWrite(0x00);
	SET_SPICS(1);
	//判断SPI发送准备好
	return data;
}

void FM_Write_Status(Uint16 data)	//LOCK
{
	int i;
	FM_Write_Enable();
	for(i=0; i < 10; i++) 
	   asm( "  NOP ");
	SET_SPICS(0);
	SpiWrite(WRSR);
	SpiWrite(data);	
	SET_SPICS(1);
//	for(i = 0; i < 10; i++)
//	   asm( "  NOP ");
//	FM_Write_Disable();
	//判断SPI发送准备好
}

//读字节
Uint16 FM_Read_Byte(Uint16 address)
{
	Uint16 data;
	SET_SPICS(0);
	SpiWrite(READ);
    SpiWrite(address & 0xff00);
	SpiWrite(address<<8);
	data = SpiWrite(0x00);	
	SET_SPICS(1);
	return data;
}
/* 铁电存储 ： 调用的时候要注意调用函数的限定条件
 * 读： 10us
 * 写： 40us
 */
//写字节
void FM_Write_Byte(Uint16 address,Uint16 data)
{
	int i;
//	FM_Write_Status(UNLOCK);
	FM_Write_Enable();
    for(i=0; i < 10; i++)  
       asm( "  NOP ");
    SET_SPICS(0);
	SpiWrite(WRITE);
	SpiWrite(address & 0xff00);
	SpiWrite(address<<8);
	SpiWrite(data << 8);
	SET_SPICS(1);
	for(i = 0; i < 10; i++)  
	   asm( "  NOP ");
//	FM_Write_Disable();
//	FM_Write_Status(LOCK);
}


//连续写Byte
void FM_DatWrite(int addr, int length, int *ptr)
{
	int i;
//	FM_Write_Status(UNLOCK);
	FM_Write_Enable();
    for(i=0; i < 10; i++)  
       asm( "  NOP ");

    SET_SPICS(0);
	SpiWrite(WRITE);
	SpiWrite(addr & 0xff00);
	SpiWrite(addr<<8);
	for(i=0;i<length;i++)
	   SpiWrite((*ptr++)<< 8);
	SET_SPICS(1);

//	for(i = 0; i < 10; i++)
//	   asm( "  NOP ");
//	FM_Write_Disable();
//	FM_Write_Status(LOCK);
}

//连续读Byte
void FM_DatRead(int addr, int length, int *ptr)
{
    int i;
    
    SET_SPICS(0);
	SpiWrite(READ);
    SpiWrite(addr & 0xff00);
	SpiWrite(addr<<8);   
	for(i=0;i<length;i++)
	   *ptr++=SpiWrite(0x00);                                                
	SET_SPICS(1);
}

//连续读16Byte
void FM_DatRead16(int addr, int length, int *ptr)
{
    Uint16 i,j,k;
    SET_SPICS(0);
	SpiWrite(READ);
    SpiWrite(addr & 0xff00);
	SpiWrite(addr<<8);
	for(i=0;i<length;i++)
	{
	   j=SpiWrite(0x00);
	   k=SpiWrite(0x00);
	   *ptr++=(k & 0xff) + (j << 8);
	}
	SET_SPICS(1);
}

Uint16 FM_DatReadWord(Uint16 address)
{
    Uint16 j,k,data;
    SET_SPICS(0);
    SpiWrite(READ);
    SpiWrite(address & 0xff00);
    SpiWrite(address<<8);
    j=SpiWrite(0x00);
    k=SpiWrite(0x00);
    data=(k & 0xff) + (j << 8);
    SET_SPICS(1);
    return data;
}

void FM_DatRead32(int addr, int length, int32 *ptr)
{
    int i;
    SET_SPICS(0);
	SpiWrite(READ);
    SpiWrite(addr & 0xff00);
	SpiWrite(addr<<8);
	for(i=0;i<length;i++)
	   *ptr++=SpiWrite(0x00);
	SET_SPICS(1);
}

//写word
void FM_DatWordWrite(int addr,int data)
{
//	if((StateEventFlag == STATE_EVENT_FAULT)||(StateEventFlag == STATE_EVENT_STANDBY))
//	{
//		int i;
//		FM_Write_Status(UNLOCK);
		FM_Write_Enable();
		asm( "  NOP ");
		SET_SPICS(0);
		SpiWrite(WRITE);
		SpiWrite(addr & 0xff00);
		SpiWrite(addr << 8);
		SpiWrite(data & 0xff00);
		SpiWrite(data << 8);
		SET_SPICS(1);
//		for(i=0; i < 10; i++)
//		   asm( "  NOP ");
//		FM_Write_Disable();
//		FM_Write_Status(LOCK);
//	}
}

//EEPROM数据清空
void FM_StructureDatReset(int addr, int length)
{
	int i;
	FM_Write_Enable();
	asm( "  NOP ");
	SET_SPICS(0);
	SpiWrite(WRITE);
	SpiWrite(addr & 0xff00);
	SpiWrite(addr<<8);

	for(i=0;i<(length*2);i++)
		SpiWrite(0);
	SET_SPICS(1);
}

//查询连续写Uint16
//void FM_StructureDatWrite(int addr, int length, Uint16 *ptr)
//{
//	FM_Write_Enable();
//  asm( "  NOP ");
//  SPICS = 0;
//	SpiWrite(WRITE);
//	SpiWrite(addr & 0xff00);
//	SpiWrite(addr<<8);
//
//	pSoe = ptr;
//	for(i=0;i<length;i++)
//	{
//		SpiWrite((*pSoe) & 0xff00);
//		SpiWrite((*pSoe) << 8);
//		pSoe++;
//	}
//	SPICS = 1;
//	StateFlag.Unwritten = 1;
//}

//中断连续写Uint16
void FM_StructureDatWrite(int addr, int length, Uint16 *ptr)
{
	FM_Write_Enable();
	asm( "  NOP ");
	SET_SPICS(0);
	SpiWrite(WRITE);
	SpiWrite(addr & 0xff00);
	SpiWrite(addr<<8);
	SOE.WriteLength = length;
	pSoe = ptr;

	SpibRegs.SPIFFTX.bit.TXFIFO = 1;
	SpibRegs.SPIFFTX.bit.SPIRST = 1;
	SpibRegs.SPIFFTX.bit.SPIFFENA = 1;
	while((SOE.WriteLength) && (SpibRegs.SPIFFTX.bit.TXFFST <15)){
		SpibRegs.SPITXBUF = ((*pSoe) & 0xff00);
		SpibRegs.SPITXBUF = ((*pSoe)<< 8);
		pSoe++;
		SOE.WriteLength--;
	}
	CntMs.IntTime = 0;
    StateFlag.EEPROMResourceLock = EEPROMLOCK;          //查询写完地址之后才可加锁
	SpibRegs.SPIFFTX.bit.TXFFINTCLR = 1;				//清除中断标志
	SpibRegs.SPIFFTX.bit.TXFFIENA = 1;					//开中断
}

Uint16 FM_StructureDatWrite2(void)
{
	while((SOE.WriteLength) && (SpibRegs.SPIFFTX.bit.TXFFST <15)){
		SpibRegs.SPITXBUF = ((*pSoe) & 0xff00);
		SpibRegs.SPITXBUF = ((*pSoe)<< 8);
        pSoe++;
		SOE.WriteLength--;
	}

	if((SOE.WriteLength == 0)&&(SpibRegs.SPIFFTX.bit.TXFFST == 0)){						//发送完成
	    return TRUE;
	}else{
	    return FALSE;
	}
}

void SPITXINTB_ISR(void)     // SPI
{
	if(FM_StructureDatWrite2())
	{
		SpibRegs.SPIFFTX.bit.SPIFFENA = 0;
		SpibRegs.SPIFFTX.bit.TXFFIENA = 0;					//关中断
//		StartCpuTimer0();
		StateFlag.EEPROMResourceLock = EEPROMUNLOCK;
	    SET_SPICS(1);
	}
	SpibRegs.SPIFFTX.bit.TXFFINTCLR = 1;				//清除中断标志
//    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP6;
}

/*
 * 功能：生成操作日志。WY
 */
void SoeRecData(Uint16 faultNumber)
{
    Uint16 getkey;
	Uint16 soebuffer;
	SOE.PastFault = SOE.RecordFaultNumber;
	SOE.RecordFaultNumber = faultNumber;
	if(SOE.RecordFaultNumber != SOE.PastFault)
	{
	    getkey=GateHwi_enter(gateHwi0); //      DINT;
		soebuffer =  SOE.BufferStoragePointer;
		if(++ SOE.BufferStoragePointer>=SOE_BUFF_GROUP)		   SOE.BufferStoragePointer = 0;
        GateHwi_leave(gateHwi0,getkey); //      EINT;

		IncidentRecord[soebuffer].RecordTime.Year 		= Time.Year;
		IncidentRecord[soebuffer].RecordTime.Month 		= Time.Month;
		IncidentRecord[soebuffer].RecordTime.Day 		= Time.Day;
		IncidentRecord[soebuffer].RecordTime.Hour 		= Time.Hour;
		IncidentRecord[soebuffer].RecordTime.Minute 	= Time.Minute;
		IncidentRecord[soebuffer].RecordTime.Second 	= Time.Second;
		IncidentRecord[soebuffer].RecordTime.MicroSec 	= Time.MicroSec;
		IncidentRecord[soebuffer].FaultNumber 			= faultNumber;
		IncidentRecord[soebuffer].WaveRecordHandler 	= WaveRecordHandler;
		IncidentRecord[soebuffer].reserved1 		  = StateEventFlag_A;  //???
		IncidentRecord[soebuffer].reserved2 		  = 2;
		IncidentRecord[soebuffer].IOstate1 			  = \
				     shift(PWM_ins_indexA,0) 	            |shift(PWM_ins_indexB,1)			        |shift(PWM_ins_indexA,2)			    |shift(0,3)\
	                |shift(0,4)     |shift(StateFlag.onceTimeStateMachine,5)    |shift(StateFlag.MainContactEnanle,6)   |shift(0,7)\
	                |shift(GET_MAIN_CONTACT_ACTION_A,8)         |shift(GET_BYPASS_CONTACT_ACTION_A,9)              |shift(0,10)               |shift(0,11)  ;
		IncidentRecord[soebuffer].IOstate2 			  =\
				 shift(0,0)	|shift(0,1)				|shift(0,2)					    |shift(0,3)\
				|shift(0,4)	|shift(0,5)	|shift(0,6)	    |shift(0,7);
	//	IncidentRecord[soebuffer].IOstate1 			  = START_INSTRUCTION+STOP_INSTRUCTION*2+BYIOin1*4+FLback*8+BYIOin4*16+BYIOin5*32+GV_RMS_OVER_PROT_ON*64+\
	//		EXT_FAULT_LED*128+EXT_RUNNING_LED*256+EXT_FUN_CTR*512+EXT_FUN_CTR0*1024+EXT_FUN_CTR1*2048+RUNNING_LED*4096+FAULT_LED*8192+LED_SYNC*16384+MAIN_CONTACT_ON*32768;
	//	IncidentRecord[soebuffer].IOstate2 			  = SYNC_SINGLE+OUT_SYNC_SINGLE*2+TEMP1*4+TEMP2*8+COLD_FUN_ON*16+COLD_FUN_ON1*32+IGBT_EN1*64+\
	//		IGBT_FAULT_ACK1*128+DCBUS_OV1*256+DCBUS_OV2*512+DCBUS_OV12*1024+0*2048+0*4096+0*8192+0*16384+0*32768;
		IncidentRecord[soebuffer].GridVoltRms 			= GridVoltRms;
		IncidentRecord[soebuffer].LoadCurRms_F 			= LoadCurRms_F*10;
		IncidentRecord[soebuffer].ApfOutCurRms_F 		= ApfOutCurRms_F*10;
		IncidentRecord[soebuffer].dcVoltF 				= dcVoltF;
		IncidentRecord[soebuffer].EnvirTemperature 	    = WindCold.EnvirTemperature*10;
		IncidentRecord[soebuffer].HeatSinkTempterature 	= WindCold.HeatSinkTempterature*10;
		IncidentRecord[soebuffer].gridFreq 				= GridFreq*1000;
		IncidentRecord[soebuffer].GridActPower 			= GridActPower*10;
		IncidentRecord[soebuffer].GridReactPower 		= GridReactPower*10;
		IncidentRecord[soebuffer].GridCosFi 			= GridCosFi*1000;
		IncidentRecord[soebuffer].dcVoltDnF 			= dcVoltDnF;
		IncidentRecord[soebuffer].storageUsed 		  	= storageUsed*10;
		IncidentRecord[soebuffer].SOETest1 			  	= SOE.CurrentPointerPosition*10;
		IncidentRecord[soebuffer].gpVoltA_rms 			= VoltInA_rms;
		IncidentRecord[soebuffer].gpVoltB_rms 			= VoltInB_rms;
		IncidentRecord[soebuffer].gpVoltC_rms 			= VoltInC_rms;
		IncidentRecord[soebuffer].loadCurA_rms 			= loadCurA_rms*10;
		IncidentRecord[soebuffer].loadCurB_rms 			= loadCurB_rms*10;
		IncidentRecord[soebuffer].loadCurC_rms 			= loadCurC_rms*10;
		IncidentRecord[soebuffer].apfOutCurA_rms 		= VoltOutA_rms*10;
		IncidentRecord[soebuffer].apfOutCurB_rms 		= VoltOutB_rms*10;
		IncidentRecord[soebuffer].apfOutCurC_rms 		= VoltOutC_rms*10;
		IncidentRecord[soebuffer].gridCurA_rms 			= gridCurA_rms*10;
		IncidentRecord[soebuffer].gridCurB_rms 			= gridCurB_rms*10;
		IncidentRecord[soebuffer].gridCurC_rms 			= gridCurC_rms*10;
		IncidentRecord[soebuffer].GridVoltHvRms 	  = SPLL[0].GridPLLVoltD;
		IncidentRecord[soebuffer].gridCurHv_rms 	  = SPLL[0].GridPLLVoltQ*10;
		IncidentRecord[soebuffer].LoadCurHvRms_F 	  = SPLL[0].GridPLLVoltDn*10;
		IncidentRecord[soebuffer].ApfOutCurHvRms_F 	  = SPLL[0].GridPLLVoltQn*10;
		IncidentRecord[soebuffer].THDu_Grid 		  = SPLL[0].PllPiOutput*10;//(Uint16)(10*TotalHarmDistorionVolt);
		IncidentRecord[soebuffer].THDi_Grid 		  = (Uint16)(10*TotalHarmDistorionGridCurr);
		IncidentRecord[soebuffer].THDi_Load 		  = (Uint16)(10*TotalHarmDistorionLoadCurr);
        if(TransformRatioVal != 0){
            IncidentRecord[soebuffer].LoadCurRms_F    = LoadCurRms_F;
            IncidentRecord[soebuffer].GridActPower    = GridActPower;
            IncidentRecord[soebuffer].GridReactPower  = GridReactPower;
            IncidentRecord[soebuffer].loadCurA_rms          = loadCurA_rms;
            IncidentRecord[soebuffer].loadCurB_rms          = loadCurB_rms;
            IncidentRecord[soebuffer].loadCurC_rms          = loadCurC_rms;
            IncidentRecord[soebuffer].gridCurA_rms          = gridCurA_rms;
            IncidentRecord[soebuffer].gridCurB_rms          = gridCurB_rms;
            IncidentRecord[soebuffer].gridCurC_rms          = gridCurC_rms;
        }
		StateFlag.SoeFlag = TRUE;
	}
}
/*
void SoeRemote(Uint16 faultNumber)
{
	 int16 SoeIndex,i;
	 int16 *p =(int16 *)&RemoteRefreshData.SoeRemote[0].FaultNum;
	 int16 *q =(int16 *)&RemoteRefreshData.SoeRemote[1].FaultNum;
     Uint16 getkey=GateHwi_enter(gateHwi1); //      DINT;
	 SoeIndex = SoeRemoteBuf;
	 if(++SoeRemoteBuf>30){
		 SoeRemoteBuf = 30;SoeIndex = 29;
		 for(i=0;i<sizeof(RemoteRefreshData.SoeRemote[0])*29;i++)	*p++ = *q++;
	 }
	 GateHwi_leave(gateHwi1,getkey);        //      EINT;

	 RemoteRefreshData.SoeRemote[SoeIndex].FaultNum 			= faultNumber;
	 RemoteRefreshData.SoeRemote[SoeIndex].SoeTime.Year 		= Time.Year;;
	 RemoteRefreshData.SoeRemote[SoeIndex].SoeTime.Month 		= Time.Month;
	 RemoteRefreshData.SoeRemote[SoeIndex].SoeTime.Day 			= Time.Day;
	 RemoteRefreshData.SoeRemote[SoeIndex].SoeTime.Hour 		= Time.Hour;
	 RemoteRefreshData.SoeRemote[SoeIndex].SoeTime.Minute 		= Time.Minute;
	 RemoteRefreshData.SoeRemote[SoeIndex].SoeTime.Second 		= Time.Second;
	 RemoteRefreshData.SoeRemote[SoeIndex].SoeTime.MicroSec 	= Time.MicroSec*5;
	 if(((faultNumber>=101)&&(faultNumber<=107))||(faultNumber==132)){
		RemoteRefreshData.SoeRemote[SoeIndex].FaultValue1 = gpVoltA_rms;
		RemoteRefreshData.SoeRemote[SoeIndex].FaultValue2 = gpVoltB_rms;
		RemoteRefreshData.SoeRemote[SoeIndex].FaultValue3 = gpVoltC_rms;
	 }
	 if((faultNumber==108)||(faultNumber<=109)||(faultNumber==115)||(faultNumber==125)){
		RemoteRefreshData.SoeRemote[SoeIndex].FaultValue1 = dcVoltF;
		RemoteRefreshData.SoeRemote[SoeIndex].FaultValue2 = dcVoltDnF;
		RemoteRefreshData.SoeRemote[SoeIndex].FaultValue3 = 0;
	 }
	 if((faultNumber==110)||(faultNumber==111)||(faultNumber==112)){
		RemoteRefreshData.SoeRemote[SoeIndex].FaultValue1 = apfOutCurA_rms;
		RemoteRefreshData.SoeRemote[SoeIndex].FaultValue2 = apfOutCurB_rms;
		RemoteRefreshData.SoeRemote[SoeIndex].FaultValue3 = apfOutCurC_rms;
	 }
	 if((faultNumber==113)||(faultNumber==119)||(faultNumber==128)){
		RemoteRefreshData.SoeRemote[SoeIndex].FaultValue1 = WindCold.MotherBoardTempterature;
		RemoteRefreshData.SoeRemote[SoeIndex].FaultValue2 = WindCold.HeatSinkTempterature;
		RemoteRefreshData.SoeRemote[SoeIndex].FaultValue3 = 0;
	 }
}
*/
void ResetSoeData(int srcGroup)
{
	IncidentRecord[srcGroup].FaultNumber=0;			//清零故障号
}

void InitSoeData(void)
{
	Uint16 i;
	for(i=0;i<SOE_BUFF_GROUP;i++)
		ResetSoeData(i);
}

void AddrWriteData(void)
{
	Uint16 soegroup;
	Uint16 *p = (Uint16 *)&IncidentRecord[SOE.BufferWritePointer];
	soegroup = SOE.CurrentPointerPosition;
    if(++soegroup>=RECORD_NUMBER)
    {
        soegroup = 0;
        StateFlag.SOEFullFlag = 1;                              //故障存满标记
    }
	FM_DatWordWrite(SOE_WRITE_ADDR_GROUP,soegroup);   		                       //故障组号保存
	FM_DatWordWrite(SOE_WRITE_ADDR_FAULTFLAG,	StateFlag.SOEFullFlag);		//故障内存标志位保存
	FM_StructureDatWrite(EEPROM_RECORD+sizeof(IncidentRecord[0])*SOE.CurrentPointerPosition*2,sizeof(IncidentRecord[0]),p);
	SOE.CurrentPointerPosition = soegroup;
}

void SaveDataEead(void)
{
	Uint16 ReadSoeGroup;
	SOE.CurrentPointerPosition = FM_Read_Byte(SOE_READ_ADDR_GROUP);
	StateFlag.SOEFullFlag = FM_Read_Byte(SOE_READ_ADDR_FAULTFLAG);
	if(SOE.CurrentPointerPosition==0)			ReadSoeGroup = RECORD_NUMBER-1;
	else 					                    ReadSoeGroup = SOE.CurrentPointerPosition-1;
	FM_DatRead(EEPROM_RECORD+sizeof(IncidentRecord[0])*ReadSoeGroup*2, sizeof(Time)*2, RecordData);
	Time.Year 	= RecordData[0] & 0xff;
	Time.Month 	= RecordData[1] & 0xff;
	Time.Day	= RecordData[2] & 0xff;
	Time.Hour 	= RecordData[3] & 0xff;
	Time.Minute = RecordData[4] & 0xff;
	Time.Second = RecordData[5] & 0xff;
	Time.MicroSec = ((RecordData[6]<<8)|(RecordData[7] & 0xff))/5;
}

void SOE_Write(void)
{
	AddrWriteData();									//SOE写
	ResetSoeData(SOE.BufferWritePointer);
	if(++SOE.BufferWritePointer>=SOE_BUFF_GROUP)						SOE.BufferWritePointer = 0;
	if(IncidentRecord[SOE.BufferWritePointer].FaultNumber == 0)		StateFlag.SoeFlag = false;
}

void SOE_Wave_Record(void)
{
    if(StorageTypeFlag)
    {
//       int *p = (int *)debugW1;
//      FM_StructureDatWrite(WAVE_RECORD_START_ADDR,sizeof(debugW1),p);
        RecordWaveFlag = 0;
    }
}

void Monitor_Message(void)
{
//    Uint16 isTransoform = TransformRatioVal;
	PowerReactStateRefresh();
	Uint16 *InfDataPoint = (Uint16 *)&Information_Structure.GridVoltRms;
	*InfDataPoint++  = GridVoltRms;
    if(StateFlag.VoltageModeFlag == 0){       //升压
        *InfDataPoint++  = GridCurRms;        //电网电流
        *InfDataPoint++  = LoadVoltRms;
        *InfDataPoint++  = GridCurBYRms;      //负载电流
    }else if(StateFlag.VoltageModeFlag == 1){
        *InfDataPoint++  = GridCurBYRms;      //电网电流
        *InfDataPoint++  = LoadVoltRms;
        *InfDataPoint++  = GridCurRms;        //负载电流
    }
    *InfDataPoint++  = VoltInA_rms;	          //A相电压有效值
	*InfDataPoint++  = VoltInB_rms;
	*InfDataPoint++  = VoltInC_rms;

	*InfDataPoint++  = VoltOutA_rms;          //A相负载电压有效值
	*InfDataPoint++  = VoltOutB_rms;
	*InfDataPoint++  = VoltOutC_rms;

	if(StateFlag.VoltageModeFlag == 0){
        *InfDataPoint++  = gridCurA_rms*10;                 //A相电网电流有效值
        *InfDataPoint++  = gridCurB_rms*10;                 //B相电网电流有效值
        *InfDataPoint++  = gridCurC_rms*10;                 //C相电网电流有效值

        *InfDataPoint++  = gridCurrBYAF_rms*10;                //A相负载电流有效值
        *InfDataPoint++  = gridCurrBYBF_rms*10;
        *InfDataPoint++  = gridCurrBYCF_rms*10;
	}else if(StateFlag.VoltageModeFlag == 1){
        *InfDataPoint++  = gridCurrBYAF_rms*10;                 //A相电网电流有效值
        *InfDataPoint++  = gridCurrBYBF_rms*10;                 //B相电网电流有效值
        *InfDataPoint++  = gridCurrBYCF_rms*10;                 //C相电网电流有效值

        *InfDataPoint++  = gridCurA_rms*10;                  //A相负载电流有效值
        *InfDataPoint++  = gridCurB_rms*10;                  //B相负载电流有效值
        *InfDataPoint++  = gridCurC_rms*10;                  //C相负载电流有效值
	}
	*InfDataPoint++	 = TempData[0]*10;    //散热器温度(出风口温度)
	*InfDataPoint++  = TempData[1]*10;    //单元机壳内部温度
	*InfDataPoint++  = GridFreq*100;

    //在协议中90AB-90DE未使用,为备用
	*InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
 	*InfDataPoint++  = 0;
	*InfDataPoint++	 = 0;
	*InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++	 = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++	 = 0;
	*InfDataPoint++	 = 0;
	*InfDataPoint++	 = 0;
	*InfDataPoint++	 = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++	 = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++	 = 0;
    *InfDataPoint++	 = 0;
    *InfDataPoint++	 = 0;
    *InfDataPoint++	 = 0;
    *InfDataPoint++	 = 0;
    *InfDataPoint++	 = 0;
    *InfDataPoint++	 = 0;
    *InfDataPoint++	 = 0;
    *InfDataPoint++	 = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
    *InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++  = 0;

	*InfDataPoint++	 = RecordFlash.RecordBlockNum;
	*InfDataPoint++	 = StateEventFlag1.all;
	*InfDataPoint++	 = StateFlag.startingMethod;	//启动方式
	*InfDataPoint++	 = StateFlag.VoltageMode<<8;
	*InfDataPoint++  = softwareFaultWord1.all;		//故障字1
	*InfDataPoint++	 = softwareFaultWord2.all;		//故障字2
	*InfDataPoint++  = softwareFaultWord3.all;		//故障字3
	*InfDataPoint++  = softwareFaultWord4.all;		//故障字4
	*InfDataPoint++  = softwareFaultWord5.all;		//故障字5
	*InfDataPoint++	 = Choose1;
	*InfDataPoint++	 = Choose2;
	*InfDataPoint++	 = Choose3;
	*InfDataPoint++	 = Choose4;
	*InfDataPoint++	 = Choose5;
	*InfDataPoint++  = DSPSOFT_VERSION_FREQC;       //从机版本:开关频率
	*InfDataPoint++  = DSPSOFT_VERSION_MAJOR;
	*InfDataPoint++  = DSPSOFT_VERSION_CHILD;
	*InfDataPoint++  = DSPSOFT_VERSION_YEAR;
	*InfDataPoint++  = DSPSOFT_VERSION_M_DAY;
	*InfDataPoint++  = COMMUNCICATION_VERSION;
	*InfDataPoint++  = GridCurRmsUnban*1000;
	*InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++  = 0;
	*InfDataPoint++  = gridCurA_rms*TransformCurrL2H *RATIO_UPLOAD10;
	*InfDataPoint++  = gridCurB_rms*TransformCurrL2H *RATIO_UPLOAD10;
	*InfDataPoint++  = gridCurC_rms*TransformCurrL2H *RATIO_UPLOAD10;
	*InfDataPoint++  = apfOutCurA_rms*MU_LCD_RATIO*TransformCurrL2H *RATIO_UPLOAD10;
	*InfDataPoint++  = apfOutCurB_rms*MU_LCD_RATIO*TransformCurrL2H *RATIO_UPLOAD10;
	*InfDataPoint++  = apfOutCurC_rms*MU_LCD_RATIO*TransformCurrL2H *RATIO_UPLOAD10;
	*InfDataPoint++  = loadCurA_rms*TransformCurrL2H *RATIO_UPLOAD10;
	*InfDataPoint++  = loadCurB_rms*TransformCurrL2H *RATIO_UPLOAD10;
	*InfDataPoint++  = loadCurC_rms*TransformCurrL2H *RATIO_UPLOAD10;
}
void RemoteParamerRefresh2(void)//0x02   遥信
{
    Uint16 *RemoteStatePoint = &RemoteStateMonitor.StateRunStop;
    *RemoteStatePoint++ = RemoteStateRunStop;
    *RemoteStatePoint++ = RemoteStateWait;
    *RemoteStatePoint++ = RemoteStateFault;
    *RemoteStatePoint++ = 0;//rsvd[1]
    *RemoteStatePoint++ = 0;
    *RemoteStatePoint++ = 0;
    *RemoteStatePoint++ = 0;
    *RemoteStatePoint++ = 0;
    *RemoteStatePoint++ = 0;
    *RemoteStatePoint++ = 0;
    *RemoteStatePoint++ = 0;
    *RemoteStatePoint++ = 0;
    *RemoteStatePoint++ = 0;
    *RemoteStatePoint++ = 0;
    *RemoteStatePoint++ = 0;
    *RemoteStatePoint++ = 0;//rsvd[13]
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCFastHardwareOverCurFlagA;            // 0位，ESC快速硬件过流故障A
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCFastHardwareOverCurFlagB;            // 1位，ESC快速硬件过流故障B
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCFastHardwareOverCurFlagC;            // 2位，ESC快速硬件过流故障C
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCInsOverCurFlagA;                     // 3位，ESC瞬时值过流故障A
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCInsOverCurFlagB;                     // 4位, ESC瞬时值过流故障B
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCInsOverCurFlagC;                     // 5位，ESC瞬时值过流故障C
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCDcCapVoltOverVoltFlagA;              // 6位，ESC直流电容电压过压故障A
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCDcCapVoltOverVoltFlagB;              // 7位，ESC直流电容电压过压故障B
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCDcCapVoltOverVoltFlagC;              // 8位，ESC直流电容电压过压故障C
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCGridRMSOverVoltFlagA;                // 9位，ESC电网电压有效值过压故障A
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCGridRMSOverVoltFlagB;                //10位，ESC电网电压有效值过压故障B
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCGridRMSOverVoltFlagC;                //11位，ESC电网电压有效值过压故障C
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCPowerFailDetectFlag;                 //12位，ESC掉电检测故障
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCGridRMSUnderVoltFlagA;               //13位，ESC电网电压有效值欠压故障A
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCGridRMSUnderVoltFlagB;               //14位，ESC电网电压有效值欠压故障B
    *RemoteStatePoint++ = softwareFaultWord1.B.ESCGridRMSUnderVoltFlagC;               //15位，ESC电网电压有效值欠压故障C
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCOverLoadFlagA;                       // 0位，ESC输出过载故障A
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCOverLoadFlagB;                       // 1位，ESC输出过载故障B
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCOverLoadFlagC;                       // 2位，ESC输出过载故障C
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCRmsOverCurrentFlagA;                 // 3位，ESC有效值输出过流故障A
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCRmsOverCurrentFlagB;                 // 4位，ESC有效值输出过流故障B
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCRmsOverCurrentFlagC;                 // 5位，ESC有效值输出过流故障C
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCOverTemperatureFlagA;                // 6位，ESC散热片温度过温故障A
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCOverTemperatureFlagB;                // 7位， ESC散热片温度过温故障B
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCOverTemperatureFlagC;                // 8位，ESC散热片温度过温故障C
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCFangLeiFaultFlag;                    // 9位，ESC防雷器故障
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCBypassContNoCloseFlagA;              //10位，ESC旁路合闸故障A
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCBypassContNoCloseFlagB;              //11位，ESC旁路合闸故障B
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCBypassContNoCloseFlagC;              //12位，ESC旁路合闸故障C
    *RemoteStatePoint++ = softwareFaultWord2.B.ESCChassisOverTempFlag;                 //13位，ESC单元过温(机壳)
    *RemoteStatePoint++ = 0;                                                           //14位   备用
    *RemoteStatePoint++ = 0;                                                           //15位   备用
    *RemoteStatePoint++ = softwareFaultWord3.B.ESCCalibrarionFailureFlag;              // 0位，ESC零偏校准过大故障
    *RemoteStatePoint++ = softwareFaultWord3.B.ESCResonanceFlautFlag;                  // 1位，ESC谐振故障
    *RemoteStatePoint++ = softwareFaultWord3.B.ESCGridOverFreqenceFlag;                // 2位，ESC电网频率过频故障
    *RemoteStatePoint++ = softwareFaultWord3.B.ESCGridUnderFreqenceFlag;               // 3位，ESC电网频率欠频故障
    *RemoteStatePoint++ = softwareFaultWord3.B.ESCTieDianReadErrFlag;                  // 4位，ESC铁电故障
    *RemoteStatePoint++ = softwareFaultWord3.B.ESCAutoStartMuchFaultFlag;              // 5位，ESC自启动频繁故障
    *RemoteStatePoint++ = 0;                                                           // 6位   备用
    *RemoteStatePoint++ = 0;                                                           // 7位   备用
    *RemoteStatePoint++ = 0;                                                           // 8位   备用
    *RemoteStatePoint++ = 0;                                                           // 9位   备用
    *RemoteStatePoint++ = 0;                                                           //10位   备用
    *RemoteStatePoint++ = 0;                                                           //11位   备用
    *RemoteStatePoint++ = 0;                                                           //12位   备用
    *RemoteStatePoint++ = 0;                                                           //13位   备用
    *RemoteStatePoint++ = 0;                                                           //14位   备用
    *RemoteStatePoint++ = 0;                                                           //15位   备用
}
void RemoteParamerRefresh(void)//0x03   遥测
{
	int16 *RemoteDataPoint = &RemoteRefreshData.VolttargetA;
	*RemoteDataPoint++ = VolttargetA;
	*RemoteDataPoint++ = VolttargetB;
	*RemoteDataPoint++ = VolttargetC;
	*RemoteDataPoint++ = VoltThreSholdA;
	*RemoteDataPoint++ = VoltThreSholdB;
	*RemoteDataPoint++ = VoltThreSholdC;
    *RemoteDataPoint++ = Backdiffvalue;
	*RemoteDataPoint++ = GridVoltRms;
	*RemoteDataPoint++ = GridCurRms;//LoadCurRms_F;
	*RemoteDataPoint++ = LoadVoltRms;//ApfOutCurRms_F*MU_LCD_RATIO;
	*RemoteDataPoint++ = TempData[0]*10;//dcVoltF*10;
	*RemoteDataPoint++ = TempData[1]*10;//WindCold.HeatSinkTempterature*10;
	*RemoteDataPoint++ = GridFreq*100;//WindCold.EnvirTemperature*10;
	*RemoteDataPoint++ = VoltInA_rms;//GridFreq*100;
	*RemoteDataPoint++ = VoltInB_rms;//GridActPower;
	*RemoteDataPoint++ = VoltInC_rms;//GridReactPower;
	*RemoteDataPoint++ = gridCurA_rms;//GridCosFi*1000;
	*RemoteDataPoint++ = gridCurB_rms;//dcVoltDnF*10;
	*RemoteDataPoint++ = gridCurC_rms;//storageUsed*10;
	*RemoteDataPoint++ = VoltOutA_rms;//GridApparentPower;
	*RemoteDataPoint++ = VoltOutB_rms;//VoltInA_rms;
	*RemoteDataPoint++ = VoltOutC_rms;//VoltInB_rms;
	*RemoteDataPoint++ = 0;
	*RemoteDataPoint++ = 0;//loadCurA_rms;
	*RemoteDataPoint++ = 0;//loadCurB_rms;
	*RemoteDataPoint++ = 0;//loadCurC_rms;
	*RemoteDataPoint++ = 0;//VoltInA_rms;
	*RemoteDataPoint++ = 0;//VoltInB_rms;
	*RemoteDataPoint++ = 0;//VoltInC_rms;
	*RemoteDataPoint++ = 0;//gridCurA_rms;
	*RemoteDataPoint++ = 0;//gridCurB_rms;
	*RemoteDataPoint++ = 0;//gridCurC_rms;
	*RemoteDataPoint++ = GridVoltRms_F * TransformVoltL2H;
	*RemoteDataPoint++ = GridCurRms_F * TransformCurrL2H;
	*RemoteDataPoint++ = LoadCurRms_F * TransformCurrL2H;
	*RemoteDataPoint++ = ApfOutCurRms_F*MU_LCD_RATIO*TransformCurrL2H*RATIO_UPLOAD10;
	*RemoteDataPoint++ = Information_Structure.THDu_Grid;
	*RemoteDataPoint++ = Information_Structure.THDi_Grid;
	*RemoteDataPoint++ = Information_Structure.THDi_Load;
	*RemoteDataPoint++ = Information_Structure.THDu_GridA;
	*RemoteDataPoint++ = Information_Structure.THDu_GridB;
	*RemoteDataPoint++ = Information_Structure.THDu_GridC;
	*RemoteDataPoint++ = Information_Structure.THDi_GridA;
	*RemoteDataPoint++ = Information_Structure.THDi_GridB;
	*RemoteDataPoint++ = Information_Structure.THDi_GridC;
	*RemoteDataPoint++ = Information_Structure.THDi_LoadA;
	*RemoteDataPoint++ = Information_Structure.THDi_LoadB;
	*RemoteDataPoint++ = Information_Structure.THDi_LoadC;
	*RemoteDataPoint++ = GridCurRms_F;
	*RemoteDataPoint++ = GridNeutralCurRms;
	*RemoteDataPoint++ = LoadFundaCurZRms;
	*RemoteDataPoint++ = GridActPowerPhA;
	*RemoteDataPoint++ = GridActPowerPhB;
	*RemoteDataPoint++ = GridActPowerPhC;
	*RemoteDataPoint++ = GridReactPowerPhA;
	*RemoteDataPoint++ = GridReactPowerPhB;
	*RemoteDataPoint++ = GridReactPowerPhC;
	*RemoteDataPoint++ = GridApparentPowerPhA;
	*RemoteDataPoint++ = GridApparentPowerPhB;
	*RemoteDataPoint++ = GridApparentPowerPhC;
	*RemoteDataPoint++ = GridCosFiPhA*1000;
	*RemoteDataPoint++ = GridCosFiPhB*1000;
	*RemoteDataPoint++ = GridCosFiPhC*1000;
	*RemoteDataPoint++ = LoadActPowerPhA;
	*RemoteDataPoint++ = LoadActPowerPhB;
	*RemoteDataPoint++ = LoadActPowerPhC;
	*RemoteDataPoint++ = LoadReactPowerPhA;
	*RemoteDataPoint++ = LoadReactPowerPhB;
	*RemoteDataPoint++ = LoadReactPowerPhC;
	*RemoteDataPoint++ = LoadApparentPowerPhA;
	*RemoteDataPoint++ = LoadApparentPowerPhB;
	*RemoteDataPoint++ = LoadApparentPowerPhC;
	*RemoteDataPoint++ = LoadCosFiPhA*1000;
	*RemoteDataPoint++ = LoadCosFiPhB*1000;
	*RemoteDataPoint++ = LoadCosFiPhC*1000;
	*RemoteDataPoint++ = Information_Structure.UbanRmsPerc*1000;
	*RemoteDataPoint++ = 0;
	*RemoteDataPoint++ = 0;
	*RemoteDataPoint++ = 0;
	*RemoteDataPoint++ = gridCurA_rms*TransformCurrL2H;
	*RemoteDataPoint++ = gridCurB_rms*TransformCurrL2H;
	*RemoteDataPoint++ = gridCurC_rms*TransformCurrL2H;
	*RemoteDataPoint++ = apfOutCurA_rms*MU_LCD_RATIO*TransformCurrL2H ;
	*RemoteDataPoint++ = apfOutCurB_rms*MU_LCD_RATIO*TransformCurrL2H ;
	*RemoteDataPoint++ = apfOutCurC_rms*MU_LCD_RATIO*TransformCurrL2H ;
	*RemoteDataPoint++ = loadCurA_rms*TransformCurrL2H;
	*RemoteDataPoint++ = loadCurA_rms*TransformCurrL2H;
	*RemoteDataPoint++ = loadCurA_rms*TransformCurrL2H;
	*RemoteDataPoint++ = Information_Structure.StateEventFlag;
	*RemoteDataPoint++ = Information_Structure.StartMode;
	*RemoteDataPoint++ = Information_Structure.modeChoosedFlag;
	*RemoteDataPoint++ = Information_Structure.FaultCharacter1;
	*RemoteDataPoint++ = Information_Structure.FaultCharacter2;
	*RemoteDataPoint++ = Information_Structure.FaultCharacter3;
	*RemoteDataPoint++ = Information_Structure.FaultCharacter4;
	*RemoteDataPoint++ = Information_Structure.FaultCharacter5;
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[0];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[1];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[2];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[3];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[4];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[5];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[6];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[7];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[8];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[9];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[10];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[11];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[12];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[13];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[14];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[15];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[16];
	*RemoteDataPoint++ = CapGroupAct.CapStateUp[17];

}
void Multiple_Parallel_Message(void)
{}

void ManufacturerParametersSetting(void)		//新厂家设置
{
    Uint16 *p = (Uint16 *)&FactorySet;
	FactorySet.CRC=usMBCRC16(p,sizeof(FactorySet)-1);
	FM_StructureDatWrite(EEPROM_FACTORY_PARAMETER,sizeof(FactorySet),p);
}

int16 ManufacturerParametersReading(void)
{
	int errCnt=5;
	Uint16 *p = (Uint16 *)&FactorySet;
	do{
		FM_DatRead16(EEPROM_FACTORY_PARAMETER,sizeof(FactorySet),(int16 *)p);
	}while((FactorySet.CRC != usMBCRC16(p,sizeof(FactorySet)-1))&&\
			--errCnt);
	if(errCnt>0)return false;
	else	return true;
}

void UserPreferencesSetting(void)	//新用户设置
{
	Uint16 *p = (Uint16 *)&UserSetting;
	UserSetting.CRC=usMBCRC16(p,sizeof(UserSetting)-1);
	FM_StructureDatWrite(EEPROM_USER_PREFERENCES,sizeof(UserSetting),p);
}

void IDPreferencesSetting(void)
{
	Uint16 *p = (Uint16 *)&MuFaciID;
	MuFaciID.CRC=usMBCRC16(p,sizeof(MuFaciID)-1);
	FM_StructureDatWrite(EEPROM_FACILITYID,sizeof(MuFaciID),p);
}

void FlashPreferencesSetting(void)
{
	Uint16 *p = (Uint16 *)&RecordFlash;
	RecordFlash.CRC=usMBCRC16(p,sizeof(RecordFlash)-1);
	FM_StructureDatWrite(EEPROM_RECORD_FLASH,sizeof(RecordFlash),p);
}

void VirtuPreferencesSetting(void)
{
	Uint16 *p = (Uint16 *)&VariZeroOffsetVAL;
	VariZeroOffsetVAL.CRC=usMBCRC16(p,sizeof(VariZeroOffsetVAL)-1);
	FM_StructureDatWrite(EEPROM_VIRTU_ZEROOFFSET,sizeof(VariZeroOffsetVAL),p);
}

int16 FlashPreferencesReading(void)
{
	int i,errCnt=5;
	Uint16 *p = (Uint16 *)&RecordFlash;
	Uint16 *q = (Uint16 *)&RecordFlash;
	do{
		FM_DatRead16(EEPROM_RECORD_FLASH,sizeof(RecordFlash),(int16 *)p);
	}while((RecordFlash.CRC != usMBCRC16(p,sizeof(RecordFlash)-1))&&\
			--errCnt);

	if(errCnt<=0){		//因为没有手动下发参数的操作,初次上电读的数据是错误的,所以要全部清零
		for(i=0;i<sizeof(RecordFlash);i++)	*q++ = 0;
	}
	if(errCnt>0)return false;
	else	return true;
}

int16 VirtuPreferencesReading(void)
{
	int errCnt=5;
	Uint16 *p = (Uint16 *)&VariZeroOffsetVAL;
	do{
		FM_DatRead16(EEPROM_VIRTU_ZEROOFFSET,sizeof(VariZeroOffsetVAL),(int16 *)p);
	}while((VariZeroOffsetVAL.CRC != usMBCRC16(p,sizeof(VariZeroOffsetVAL)-1))&&\
			--errCnt);
	if(errCnt>0)return false;
	else	return true;
}

int16 UserPreferencesReading(void)
{
	int errCnt=5;
	Uint16 *p = (Uint16 *)&UserSetting;
	do{
		FM_DatRead16(EEPROM_USER_PREFERENCES,sizeof(UserSetting),(int16 *)p);
	}while((UserSetting.CRC != usMBCRC16(p,sizeof(UserSetting)-1))&&\
			--errCnt);
	if(errCnt>0)return false;
	else	return true;
}

int16 IDPreferencesReading(void)
{
	int errCnt=5;
	Uint16 *p = (Uint16 *)&MuFaciID;
	do{
		FM_DatRead16(EEPROM_FACILITYID,sizeof(MuFaciID),(int16 *)p);
	}while((MuFaciID.CRC != usMBCRC16(p,sizeof(MuFaciID)-1))&&\
			--errCnt);
	if(errCnt>0)return false;
	else	return true;
}

void CapParamSetting(void)		//电容设置
{
	Uint16 *p;
	p = (Uint16 *)&CapGroupAct.Capacity[0];
	CapGroupAct.CRC = usMBCRC16(p,sizeof(CapGroupAct)-25);
	FM_StructureDatWrite(EEPROM_CAPPARAM,(sizeof(CapGroupAct)-24),p);
}

int16 CapParamReading(void)		//电容设置
{
	int k,errCnt=5;
	Uint16 *p = (Uint16 *)&CapGroupAct.Capacity[0];
	do{
		FM_DatRead16(EEPROM_CAPPARAM,(sizeof(CapGroupAct)-24),(int16 *)p);
	}while((CapGroupAct.CRC != usMBCRC16(p,sizeof(CapGroupAct)-25))&&\
			--errCnt);
	for(k=0;k<24;k++)   CapGroupAct.CapStateUp[k]=0;
	if(errCnt>0)return false;
	else	return true;
}

void UserHarmnoicSetting(void)		//用户谐波设置
{
	Uint16 *p;
	p = (Uint16 *)&harmCompPercParameter;
	harmCompPercParameter.CRC=usMBCRC16(p,sizeof(harmCompPercParameter)-1);
	FM_StructureDatWrite(EEPROM_HARMNOIC_USER,sizeof(harmCompPercParameter),p);
}

int16 UserHarmnoicReading(void)
{
	int errCnt=5;
	Uint16 *p = (Uint16 *)&harmCompPercParameter;
	do{
		FM_DatRead16(EEPROM_HARMNOIC_USER,sizeof(harmCompPercParameter),(int16 *)p);
	}while((harmCompPercParameter.CRC != usMBCRC16(p,sizeof(harmCompPercParameter)-1))&&\
			--errCnt);
	if(errCnt>0)return false;
	else	return true;
}

void HarmnoicParamSetting(void)		//工厂谐波设置
{
	Uint16 *p;
	p = (Uint16 *)&Harmonic;
	Harmonic.CRC=usMBCRC16(p,sizeof(Harmonic)-1);
	FM_StructureDatWrite(EEPROM_HARMNOIC_FACTORY,sizeof(Harmonic),p);
}

int16 HarmnoicParamReading(void)
{
	int errCnt=5;
	Uint16 *p = (Uint16 *)&Harmonic;
	do{
		FM_DatRead16(EEPROM_HARMNOIC_FACTORY,sizeof(Harmonic),(int16 *)p);
	}while((Harmonic.CRC != usMBCRC16(p,sizeof(Harmonic)-1))&&\
			--errCnt);
	if(errCnt>0)return false;
	else	return true;
}
//===========================================================================
// End of file.
//===========================================================================
