/*
 * ApfFunc.c
 *
 *  Created on: 2018年7月30日
 *      Author: XuYuLin
 */
/***************************APF includes***************************************/
#include "MarcoDefine.h"
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "VariblesRef.h"
#include "DSP28x_Project.h"
#include "DCL.h"
#include "math.h"
#include "Spi_Frame.h"

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"
/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"
#include "FlashApiAuthorFuc.h"
#include <ti/sysbios/knl/Semaphore.h>

void ElseWrite(Uint16 ADDR,int16 DATA);
eMBErrorCode OffWaveReadWriteRegister(UCHAR *pucRegBuffer,USHORT usAddress,USHORT usNRegs,eMBRegisterMode eMode);//0x69
eMBErrorCode WaveReadWriteRegister(UCHAR *pucRegBuffer,USHORT usAddress,USHORT usNRegs,eMBRegisterMode eMode);//0x30,0x31
eMBErrorCode SoeReadWriteRegister(UCHAR *pucRegBuffer,USHORT usAddress,USHORT usNRegs,eMBRegisterMode eMode);//0x32,0x68
eMBErrorCode DefineReadWriteRegister(UCHAR *pucRegBuffer,USHORT usAddress,USHORT usNRegs,eMBRegisterMode eMode);//0x66,0x67
//UCHAR * getHarmParam(UCHAR *pucRegBuffer,int step);
/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_FUNC_READ_ADDR_OFF               ( MB_PDU_DATA_OFF + 0)
#define MB_PDU_FUNC_READ_REGCNT_OFF             ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE                   ( 4 )
#define MB_PDU_FUNC_READ_REGCNT_MAX             ( 0x007D )

#define MB_PDU_FUNC_WRITE_MUL_ADDR_OFF          ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF        ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF       ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_WRITE_MUL_VALUES_OFF        ( MB_PDU_DATA_OFF + 5 )
#define MB_PDU_FUNC_WRITE_MUL_SIZE_MIN          ( 5 )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX        ( 0x0078 )

#define DATA2BUF(data)  *pucRegBuffer++=(int16)(data)>>8;  *pucRegBuffer++=(int16)(data)&0x00ff
#define DATA2BUFU(data) *pucRegBuffer++=(Uint16)(data)>>8; *pucRegBuffer++=(Uint16)(data)&0x00ff

typedef union{	float flt;
				Uint32 intig;
				}FDATA;
typedef union{	Uint32 flt;
				Uint32 intig;
				}DDATA;

Stru_SOERead SOERead;
Stru_harmCompPercParameter harmCompPercParameter;
//Stru_HarmonicWaveParameter HarmonicWaveParameter[48];
Stru_Harmonic Harmonic;
Uint32 iRecord;
//Event_Id_00 :SCI_A received
//Event_Id_01 :SCI_B received
//Event_Id_02 :SCI_C received
//Event_Id_03 :SCI_A has been sent
//Event_Id_04 :SCI_B has been sent
//Event_Id_05 :SCI_C has been sent

//int ESCWIFIFlag = 1;
//int ESCWIFIFlag1 = 1;
//int ESCFICTRLFlag = 1;
//int ESCWIFIDelayFlag1 = 1,ESCWIFIDelayFlag2 = 1;
void UartAllRun(void)
{
    UInt events;
    while(1)
    {
        // Wait for ANY of the ISR events to be posted *
      events = Event_pend(Event_SCI_Reveived, Event_Id_NONE,Event_Id_00 + Event_Id_01 + Event_Id_02,BIOS_WAIT_FOREVER);
        // Process all the events that have occurred
      if(events & Event_Id_00){
        if(CntMs.StartDelay > CNT_MS(4000)){    //Wait for the UART Bluetooth startup to succeed
//            if(CntMs.StartDelay > CNT_MS(10000)){
//                if((ESCFICTRLFlag == 1)&&(ESCWIFIFlag1 == 0)){
//                    xMBPortEventPost( &SCIC,EV_FRAME_SENT0 );
//                    ESCFICTRLFlag = 0;
//                    CntMs.WIFIDelay = 0;
//                }
//            }
//            if(CntMs.WIFIDelay > CNT_MS(100)){
                eMBModbus_Slaver_poll(&SCIC);//Bluetooth,Onboard uart;蓝牙,板载uart
//                ESCFICTRLFlag = 1;
//            }
        }
      }
      if(events & Event_Id_01){
         eMBModbus_Slaver_poll(&SCIB);//液晶屏口,对应单元4851A,4851B
      }
//      if(CntMs.StartDelay > CNT_MS(7000)){   //使用AT指令对wifi模块下发数据
//          if((CntMs.StartDelay > CNT_MS(8000))&&(ESCWIFIDelayFlag1 == 1))            ESCWIFIFlag = 2;
//          if((CntMs.StartDelay > CNT_MS(9000))&&(ESCWIFIDelayFlag2 == 1))            ESCWIFIFlag = 3;
//
//          if(ESCWIFIFlag == 1){
//              xMBPortEventPost( &SCIC,EV_FRAME_SENT1 );
//              ESCWIFIFlag = 0;
//          }
//          if(ESCWIFIFlag == 2){
//              xMBPortEventPost( &SCIC,EV_FRAME_SENT2 );
//              ESCWIFIFlag = 0;
//              ESCWIFIDelayFlag1 = 0;
//          }
//          if(ESCWIFIFlag == 3){
//              xMBPortEventPost( &SCIC,EV_FRAME_SENT3 );
//              ESCWIFIFlag = 0;
//              ESCWIFIDelayFlag2 = 0;
//              ESCWIFIFlag1 = 0;
//          }
//      }

//      if (events & Event_Id_02) {
//            if(FactorySet.INIT_SCIC_BAUD.B.SciType==5)
//                eMBModbus_Master_poll(&SCID);  //备用 .4852A,4852B
//            else
//                eMBModbus_Slaver_poll(&SCID);  //备用 .4852A,4852B
//        }
    }
}

//0x68
eMBException
APFWaveReadFunction( UCHAR * pucFrame, USHORT * usLen )//指针第一个数据是功能码
{
	USHORT          usRegAddress;
	USHORT          usRegCount;
	UCHAR          *pucFrameCur;

	eMBException    eStatus = MB_EX_NONE;
	eMBErrorCode    eRegStatus;

	if( *usLen == ( MB_PDU_FUNC_READ_SIZE + MB_PDU_SIZE_MIN ) )
	{
		usRegAddress = ( USHORT )( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF] << 8 );
		usRegAddress |= ( USHORT )( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF + 1] );
//        usRegAddress++;

		usRegCount = ( USHORT )( pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF] << 8 );
		usRegCount |= ( USHORT )( pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF + 1] );

		/* Check if the number of registers to read is valid. If not
		 * return Modbus illegal data value exception.
		 */
		if( ( usRegCount >= 1 ) && ( usRegCount <= MB_PDU_FUNC_READ_REGCNT_MAX ) )
		{
			/* Set the current PDU data pointer to the beginning. */
			pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
			*usLen = MB_PDU_FUNC_OFF;

			/* First byte contains the function code. */
			*pucFrameCur++ = MB_FUNC_APF_SCENEREAD;
			*usLen += 1;

			/* Second byte in the response contain the number of bytes. */
			*pucFrameCur++ = ( UCHAR ) ( usRegCount * 2 );
			*usLen += 1;

			/* Make callback to fill the buffer. */
			eRegStatus = WaveReadWriteRegister(pucFrameCur,usRegAddress,usRegCount,MB_REG_READ);
			/* If an error occured convert it into a Modbus exception. */
			  if( eRegStatus != MB_ENOERR )
			  {
				  eStatus = prveMBError2Exception( eRegStatus );
			  }
			  else
			  {
				  *usLen += usRegCount * 2;
			  }
		  }
		  else
		  {
			  eStatus = MB_EX_ILLEGAL_DATA_VALUE;
		  }
	  }
	  else
	  {
		  /* Can't be a valid request because the length is incorrect. */
		  eStatus = MB_EX_ILLEGAL_DATA_VALUE;
	  }
	  return eStatus;
}

//0x69
eMBException
APFOffWaveReadFunction( UCHAR * pucFrame, USHORT * usLen )//指针第一个数据是功能码
{
	USHORT          usRegAddress;
	USHORT          usRegCount;
	UCHAR          *pucFrameCur;

	eMBException    eStatus = MB_EX_NONE;
	eMBErrorCode    eRegStatus;

	if( *usLen == ( MB_PDU_FUNC_READ_SIZE +  MB_PDU_SIZE_MIN ) )
	{
		usRegAddress = ( USHORT )( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF] << 8 );
		usRegAddress |= ( USHORT )( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF + 1] );
//        usRegAddress++;

		usRegCount = ( USHORT )( pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF] << 8 );
		usRegCount |= ( USHORT )( pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF + 1] );

		/* Check if the number of registers to read is valid. If not
		 * return Modbus illegal data value exception.
		 */
		if( ( usRegCount >= 1 ) && ( usRegCount <= MB_PDU_FUNC_READ_REGCNT_MAX ) )
		{
			/* Set the current PDU data pointer to the beginning. */
			pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
			*usLen = MB_PDU_FUNC_OFF;

			/* First byte contains the function code. */
			*pucFrameCur++ = MB_FUNC_APF_SOEDEFINED;
			*usLen += 1;

			/* Second byte in the response contain the number of bytes. */
			*pucFrameCur++ = ( UCHAR ) ( usRegCount * 2 );
			*usLen += 1;

			/* Make callback to fill the buffer. */
			eRegStatus = OffWaveReadWriteRegister(pucFrameCur,usRegAddress,usRegCount,MB_REG_READ);
			/* If an error occured convert it into a Modbus exception. */
			  if( eRegStatus != MB_ENOERR )
			  {
				  eStatus = prveMBError2Exception( eRegStatus );
			  }
			  else
			  {
				  *usLen += usRegCount * 2;
			  }
		  }
		  else
		  {
			  eStatus = MB_EX_ILLEGAL_DATA_VALUE;
		  }
	  }
	  else
	  {
		  /* Can't be a valid request because the length is incorrect. */
		  eStatus = MB_EX_ILLEGAL_DATA_VALUE;
	  }
	  return eStatus;
}
//0x6A
eMBException
APFSoeReadFunction( UCHAR * pucFrame, USHORT * usLen )//指针第一个数据是功能码
{
	USHORT          usRegAddress;
	USHORT          usRegCount;
	UCHAR           *pucFrameCur;
	eMBException    eStatus = MB_EX_NONE;
	eMBErrorCode    eRegStatus;

	if( *usLen == ( MB_PDU_FUNC_READ_SIZE + MB_PDU_SIZE_MIN ) )
	{
		usRegAddress = ( USHORT )( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF] << 8 );
		usRegAddress |= ( USHORT )( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF + 1] );
//      usRegAddress++;

		usRegCount = ( USHORT )( pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF] << 8 );
		usRegCount |= ( USHORT )( pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF + 1] );

		/* Check if the number of registers to read is valid. If not
		 * return Modbus illegal data value exception.
		 */
		if( ( usRegCount >= 1 ) && ( usRegCount <= MB_PDU_FUNC_READ_REGCNT_MAX ) )
		{
			/* Set the current PDU data pointer to the beginning. */
			pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
			*usLen = MB_PDU_FUNC_OFF;

			/* First byte contains the function code. */
			*pucFrameCur++ = MB_FUNC_APF_SOEDATAREAD;
			*usLen += 1;

			/* Second byte in the response contain the number of bytes. */
			*pucFrameCur++ = ( UCHAR ) ( usRegCount * 2 );
			*usLen += 1;

			/* Make callback to fill the buffer. */
			eRegStatus = SoeReadWriteRegister(pucFrameCur,usRegAddress,usRegCount,MB_REG_READ);
			/* If an error occured convert it into a Modbus exception. */
			  if( eRegStatus != MB_ENOERR )
			  {
				  eStatus = prveMBError2Exception( eRegStatus );
			  }
			  else
			  {
				  *usLen += usRegCount * 2;
			  }
		  }
		  else
		  {
			  eStatus = MB_EX_ILLEGAL_DATA_VALUE;
		  }
	  }
	  else
	  {
		  /* Can't be a valid request because the length is incorrect. */
		  eStatus = MB_EX_ILLEGAL_DATA_VALUE;
	  }
	  return eStatus;
}

//0x66
eMBException
APFUserDefinedReadFunction( UCHAR * pucFrame, USHORT * usLen )//指针第一个数据是功能码
{
	USHORT          usRegAddress;
	USHORT          usRegCount;
	UCHAR          *pucFrameCur;

	eMBException    eStatus = MB_EX_NONE;
	eMBErrorCode    eRegStatus;

	if( *usLen == ( MB_PDU_FUNC_READ_SIZE + MB_PDU_SIZE_MIN ) )
	{
		usRegAddress = ( USHORT )( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF] << 8 );
		usRegAddress |= ( USHORT )( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF + 1] );
//        usRegAddress++;

		usRegCount = ( USHORT )( pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF] << 8 );
		usRegCount |= ( USHORT )( pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF + 1] );

		/* Check if the number of registers to read is valid. If not
		 * return Modbus illegal data value exception.
		 */
		if( ( usRegCount >= 1 ) && ( usRegCount <= MB_PDU_FUNC_READ_REGCNT_MAX ) )
		{
			/* Set the current PDU data pointer to the beginning. */
			pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
			*usLen = MB_PDU_FUNC_OFF;

			/* First byte contains the function code. */
			*pucFrameCur++ = MB_FUNC_APF_USERDEFINEREAD;
			*usLen += 1;

			/* Second byte in the response contain the number of bytes. */
			*pucFrameCur++ = ( UCHAR ) ( usRegCount * 2 );
			*usLen += 1;

			/* Make callback to fill the buffer. */
			eRegStatus = DefineReadWriteRegister( pucFrameCur, usRegAddress, usRegCount, MB_REG_READ );
			/* If an error occured convert it into a Modbus exception. */
			  if( eRegStatus != MB_ENOERR )
			  {
				  eStatus = prveMBError2Exception( eRegStatus );
			  }
			  else
			  {
				  *usLen += usRegCount * 2;
			  }
		  }
		  else
		  {
			  eStatus = MB_EX_ILLEGAL_DATA_VALUE;
		  }
	  }
	  else
	  {
		  /* Can't be a valid request because the length is incorrect. */
		  eStatus = MB_EX_ILLEGAL_DATA_VALUE;
	  }
	  return eStatus;
}

//0x67
eMBException
APFUserDefinedWriteFunction( UCHAR * pucFrame, USHORT * usLen )//指针第一个数据是功能码
{
	   USHORT          usRegAddress;
	    USHORT          usRegCount;
	    UCHAR           ucRegByteCount;

	    eMBException    eStatus = MB_EX_NONE;
	    eMBErrorCode    eRegStatus;

	    if( *usLen >= ( MB_PDU_FUNC_WRITE_MUL_SIZE_MIN + MB_PDU_SIZE_MIN ) )
	    {
	        usRegAddress = ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_MUL_ADDR_OFF] << 8 );
	        usRegAddress |= ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_MUL_ADDR_OFF + 1] );
	//        usRegAddress++;

	        usRegCount = ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF] << 8 );
	        usRegCount |= ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF + 1] );

	        ucRegByteCount = pucFrame[MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF];

	        if( ( usRegCount >= 1 ) &&
	            ( usRegCount <= MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX ) &&
	            ( ucRegByteCount == ( UCHAR ) ( 2 * usRegCount ) ) )
	        {
	            /* Make callback to update the register values. */
	 		eRegStatus = DefineReadWriteRegister(&pucFrame[MB_PDU_FUNC_WRITE_MUL_VALUES_OFF],usRegAddress,usRegCount,MB_REG_WRITE);
            /* If an error occured convert it into a Modbus exception. */
            if( eRegStatus != MB_ENOERR )
            {
                eStatus = prveMBError2Exception( eRegStatus );
            }
            else
            {
                /* The response contains the function code, the starting
                 * address and the quantity of registers. We reuse the
                 * old values in the buffer because they are still valid.
                 */
                *usLen = MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF;
            }
        }
        else
        {
            eStatus = MB_EX_ILLEGAL_DATA_VALUE;
        }
    }
    else
    {
        /* Can't be a valid request because the length is incorrect. */
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}

/**********************************************从机应用层接口 自定义功能码*************************/
//0x69
eMBErrorCode OffWaveReadWriteRegister(UCHAR *pucRegBuffer,USHORT usAddress,USHORT usNRegs,eMBRegisterMode eMode)
{
    eMBErrorCode    eStatus = MB_ENOERR;
	Uint16 tmp;
	volatile float32 *TxBuffPos;
	FDATA  SciDATASee;
	if(usAddress>= 30000)//第16条故障的起始地址
	    RecordFlash.FlashState.B.AutoRecordReadFlag = 0;
	switch (eMode)
	{
	/* read current register values from the protocol stack. */
	case MB_REG_READ:
	    usAddress=usAddress %1000+1000*(USHORT)(usAddress/2000);//28377临时用

		TxBuffPos		=&debugW[0][0]+usAddress;	//得到16bit地址,源数据为float32型
		tmp=usNRegs>>1;
		while(tmp--) {
			SciDATASee.flt = *TxBuffPos++;
			*pucRegBuffer++ =  SciDATASee.intig&0xff;
			*pucRegBuffer++ = (SciDATASee.intig>>8)&0xff;
			*pucRegBuffer++ = (SciDATASee.intig>>16)&0xff;
			*pucRegBuffer++ = (SciDATASee.intig>>24)&0xff;
			}

		break;
	/* write current register values with new values from the protocol stack. */
	case MB_REG_WRITE:
		break;
	}
    return eStatus;
//    eMBErrorCode    eStatus = MB_ENOERR;
//    Uint16 *TxBuffPos;
//    Uint16 rx34;
//	switch (eMode)
//	{
//		/* read current register values from the protocol stack. */
//		case MB_REG_READ:
//			TxBuffPos		=(Uint16 *)(&debugW[0][0]+usAddress);	//得到16bit地址,源数据为float32型
//			while(usNRegs--) {
//				rx34= *TxBuffPos++;
//				*pucRegBuffer++ = (rx34>>8)&0xff;
//				*pucRegBuffer++ =  rx34&0xff;
//				}
//			break;
//
//		/* write current register values with new values from the protocol stack. */
//		case MB_REG_WRITE:
//			break;
//	}
//    return eStatus;
}
//0x68
eMBErrorCode WaveReadWriteRegister(UCHAR *pucRegBuffer,USHORT usAddress,USHORT usNRegs,eMBRegisterMode eMode)
{
    eMBErrorCode    eStatus = MB_ENOERR;
	Uint16 tmp;
	volatile float32 *TxBuffPos;
	FDATA  SciDATASee;
	switch (eMode)
	{
	/* read current register values from the protocol stack. */
	case MB_REG_READ:
        usAddress=usAddress %1000+1000*(USHORT)(usAddress/2000);//28377临时用

        TxBuffPos		=&debugW[0][0]+usAddress;	//得到16bit地址,源数据为float32型
		tmp=usNRegs>>1;
		while(tmp--){
			SciDATASee.flt = *TxBuffPos++;
			*pucRegBuffer++ =  SciDATASee.intig&0xff;
			*pucRegBuffer++ = (SciDATASee.intig>>8)&0xff;
			*pucRegBuffer++ = (SciDATASee.intig>>16)&0xff;
			*pucRegBuffer++ = (SciDATASee.intig>>24)&0xff;
		}

		break;
	/* write current register values with new values from the protocol stack. */
	case MB_REG_WRITE:
		break;
	}
    return eStatus;
}
//0x6A
eMBErrorCode SoeReadWriteRegister(UCHAR *pucRegBuffer,USHORT usAddress,USHORT usNRegs,eMBRegisterMode eMode)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int *pRecord,RecordData1[84];
	Uint16 tmp,Cs,Ys,FF;
	switch (eMode)
	{
	/* read current register values from the protocol stack. */
	case MB_REG_READ:
		if(StateFlag.SOEFullFlag)	FF = SOE.CurrentPointerPosition;//最早的记录
		else                        FF = 0;
		Cs = usAddress/sizeof(IncidentRecord[0]);
		Ys = fmod(usAddress,sizeof(IncidentRecord[0]))*2;
		FF += Cs;
          while(FF>=RECORD_NUMBER)
		{
           FF -= RECORD_NUMBER;
		}
		FM_DatRead(EEPROM_RECORD+sizeof(IncidentRecord[0])*FF*2,sizeof(IncidentRecord[0])*2,RecordData1);
		pRecord = &RecordData1[Ys];
		tmp = usNRegs*2;
		while(tmp--){
			*pucRegBuffer++ = *pRecord++;
		}
		break;

	/* write current register values with new values from the protocol stack. */
	case MB_REG_WRITE:
		break;
	}
    return eStatus;
}

//自定义0x66 0x67
eMBErrorCode DefineReadWriteRegister(UCHAR *pucRegBuffer,USHORT usAddress,USHORT usNRegs,eMBRegisterMode eMode)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int rx34,*pZdy;
        switch (eMode)
        {
        /* read current register values from the protocol stack. */
        case MB_REG_READ:
			if((usAddress>=0x81FC)&&(usAddress<=0x81FE)){   //地址自动匹配
//				srand(Seed);
			    MuFaciID.RandIDH = RandIDH  = rand();
			    MuFaciID.RandIDL = RandIDL  = rand();
			    pZdy = usAddress-0x81FC+((int*)&MuFaciID);
			    RandTim = rand()%50;RandTimCount = 0;
			    while(RandTimCount<=RandTim);
			}
			if(usAddress>=0x81FF) pZdy = usAddress-0x81FF+((int*)&MuFaciID);//单元总数

			if((usAddress>=0x8200)&&(usAddress<=0x83DF))
				pZdy = usAddress-0x8200+((int*)&FactorySet);//厂家

			if((usAddress>=0x83E0)&&(usAddress<=0x83FF))//零偏预置参数
				pZdy=(int16*)&VariZeroOffsetVAL;

			if((usAddress>=0x8400)&&(usAddress<=0x8FFF))
				pZdy = usAddress-0x8400+((int*)&UserSetting);//用户

			if((usAddress>=0x9000)&&(usAddress<=0x906C))
				pZdy = usAddress-0x9000+((int*)&CapGroupAct.PhactGroup[0]);//无功电容

			if((usAddress>=0x9075)&&(usAddress<=0x93FF))
				pZdy = usAddress-0x9075+((int*)&Information_Structure);//多机并联值

			if((usAddress>=0x9400)&&(usAddress<=0x9FFF))
				pZdy = usAddress-0x9400+((int*)&VirtuZeroOffSETVAL);//零偏刷新显示

			if((usAddress>=0xA000)&&(usAddress<=0xA02F))
				pZdy = usAddress-0xA000+((int*)&harmCompPercParameter);//谐波用户设置

			if((usAddress>=0xA030)&&(usAddress<=0xA1B7))
				pZdy = usAddress-0xA030+((int*)&Harmonic);//谐波厂家设置

			if((usAddress>=0xF9F8)&&(usAddress<=0xF9F9)){
				pZdy = usAddress-0xF9F8+((int*)&SOERead);			 //SOE配置
				SOERead.ChannelPoints = sizeof(IncidentRecord[0]);
				if(StateFlag.SOEFullFlag)	SOERead.ChannelNumber = RECORD_NUMBER;
				else		                SOERead.ChannelNumber = SOE.CurrentPointerPosition;
			}

			if((usAddress>=0xFA00)&&(usAddress<=0xFFFF)){
//                if(!RecordFlash.UiBlockRead)	RecordFlash.FlashBlockRead = RecordFlash.RecordBlockNum-1;
//                else							RecordFlash.FlashBlockRead = RecordFlash.UiBlockRead-1;
//                FlashDataRead();
				pZdy = usAddress - 0xFA00 + (int *)&WaveRecCfgHalf;     //故障录播配置文件
			}

			while(usNRegs--) {
				rx34= *pZdy++;
				*pucRegBuffer++ = (rx34>>8)&0xff;
				*pucRegBuffer++ =  rx34&0xff;
			}
         break;

        /* write current register values with new values from the protocol stack. */
        case MB_REG_WRITE:
    		if((usAddress>=0x81FC)&&(usAddress<=0x81FF)){//地址自动匹配
				pZdy=(int16*)&MuFaciID;
				pZdy = pZdy+usAddress-0x81FC;
			}

    		if((usAddress>=0x8200)&&(usAddress<=0x83DF)){//厂家设置
				pZdy=(int16*)&FactorySet;
				pZdy = pZdy+usAddress-0x8200;
				StateFlag.ManufacturerParameters = true;
			}

    		if((usAddress>=0x83E0)&&(usAddress<=0x83FF)){//零偏校准下发
				pZdy=(int16*)&VariZeroOffsetVAL;
				pZdy = pZdy+usAddress-0x83E0;
				StateFlag.VitruZeroParameters = true;
			}

			if((usAddress>=0x8400)&&(usAddress<=0x8FFF)){//用户设置
				pZdy=(int16*)&UserSetting;
				pZdy = pZdy+usAddress-0x8400;
				StateFlag.UserPreferences = true;
			}

        	if((usAddress>=0x9000)&&(usAddress<=0x9017)){//无功电容
				pZdy=(int16*)&CapGroupAct.PhactGroup[0];
				pZdy = pZdy+usAddress-0x9000;
        		CapFlagDown = usAddress-0x9000;
        	}
			if((usAddress>=0x9018)&&(usAddress<=0x9037)){//无功电容
				pZdy=(int16*)&CapGroupAct.Capacity[0];
				pZdy = pZdy+usAddress-0x9018;
				StateFlag.CapDataParamWrite = true;
			}
			if((usAddress>=0x9038)&&(usAddress<=0x906C)){//无功电容
				pZdy=(int16*)&CapGroupAct.CapComuFaultFlag;
				pZdy = pZdy+usAddress-0x9038;
			}

			if((usAddress==0x9070)&&(usAddress<=0x9073)){//广播对时
				pZdy=(int16*)&Time;
				pZdy = pZdy+usAddress-0x9070;
			}

			if((usAddress>=0xA000)&&(usAddress<=0xA02F)){//谐波用户设置
				pZdy=(int16*)&harmCompPercParameter;
				pZdy = pZdy+usAddress-0xA000;
				StateFlag.harmCompPercParamRefresh = true;
			}

			if((usAddress>=0xA030)&&(usAddress<=0xA1B7)){//谐波厂家设置
				pZdy=(int16*)&Harmonic;
				pZdy = pZdy+usAddress-0xA030;
   		   		StateFlag.HarmonicWaveParamRefresh = true;
			}

			if((usAddress>=0x8000)&&(usAddress<=0x81FB)){//遥控区
				rx34 = *pucRegBuffer++ << 8;
				rx34 |= *pucRegBuffer;
				ElseWrite(usAddress,rx34);
			}
			else if((usAddress>=0x81FC)&&(usAddress<=0xA1B7)){
				while(usNRegs--){
					*pZdy = *pucRegBuffer++ << 8;
					*pZdy |= *pucRegBuffer++;
					 pZdy++;
				}
				if((usAddress>=0x81FC)&&(usAddress<=0x81FF)){
					if((RandIDH==MuFaciID.RandIDH)&&(RandIDL==MuFaciID.RandIDL))
						StateFlag.IDPreferences = true;
					else{
						RandTimCount = 0;RandTim = 20;
						while(RandTimCount<=RandTim);
					}
				}
			}
        break;
        }
    return eStatus;
}

void ElseWrite(Uint16 ADDR,int16 DATA){
	switch(ADDR)
	{
	  case 0x8000:								//界面搜索指令 无操作
		   break;
	   case 0x8001:
		   if(DATA == 1){
			   if((!ESCFlagA.startFlag)&&(!ESCFlagB.startFlag)&&(!ESCFlagC.startFlag))			SoeRecData(SOE_GP_EVENT);
#if ESC_SINGLEPHASE
               ESCFlagA.startFlag = 1;
               ESCFlagB.startFlag = 0;
               ESCFlagC.startFlag = 0;
#elif ESC_THREEPHASE
               ESCFlagA.startFlag = 1;
               ESCFlagB.startFlag = 1;
               ESCFlagC.startFlag = 1;
#elif ESC_ONEINTWOPHASE
               ESCFlagA.startFlag = 1;
               ESCFlagB.startFlag = 1;
               ESCFlagC.startFlag = 0;
#elif ESC_ONEINTHREEPHASE
               ESCFlagA.startFlag = 1;
               ESCFlagB.startFlag = 1;
               ESCFlagC.startFlag = 1;
#endif
			   if(     (StateEventFlag_A == STATE_EVENT_STANDBY_A)||\
			           (StateEventFlag_B == STATE_EVENT_STANDBY_B)||\
			           (StateEventFlag_C == STATE_EVENT_STANDBY_C)     ){
	               SET_POWER_CTRL(1);
	               Delayus(TIME_WRITE_15VOLT_REDAY);
			   }
               ESCFlagA.autoStFlag = ORIGINAL_STATE_A;
               ESCFlagB.autoStFlag = ORIGINAL_STATE_B;
               ESCFlagC.autoStFlag = ORIGINAL_STATE_C;
               SET_RUNNING_LED(0);
               if(  (StateEventFlag_A != STATE_EVENT_FAULT_A)&&\
                    (StateEventFlag_B != STATE_EVENT_FAULT_B)&&\
                    (StateEventFlag_C != STATE_EVENT_FAULT_C)  )
               {
                   SET_FAULT_LED(0);
               }
               ESCFlagA.HWPowerFAULTFlag = 0;
               ESCFlagB.HWPowerFAULTFlag = 0;
               ESCFlagC.HWPowerFAULTFlag = 0;
               ESCFlagA.HWPowerSTOPFlag = 0;
               ESCFlagB.HWPowerSTOPFlag = 0;
               ESCFlagC.HWPowerSTOPFlag = 0;
		   }
		   break;
	   case 0x8002:
		   if(DATA == 1){
				if((!ESCFlagA.stopFlag)&&(!ESCFlagB.stopFlag)&&(!ESCFlagC.stopFlag))		SoeRecData(SOE_GP_EVENT+1);
		   		ESCFlagA.stopFlag = 1;
		   		ESCFlagB.stopFlag = 1;
		   		ESCFlagC.stopFlag = 1;
		   		ESCFlagA.autoStFlag = ORIGINAL_STATE_A;
		   		ESCFlagB.autoStFlag = ORIGINAL_STATE_B;
		   		ESCFlagC.autoStFlag = ORIGINAL_STATE_C;
		   		ESCFlagA.onceRunStartFlag = 1;
		   		ESCFlagB.onceRunStartFlag = 1;
		   		ESCFlagC.onceRunStartFlag = 1;
		   		if(ESCBYRelayCNTA != 1){
		   		    ESCFlagA.HWPowerSTOPFlag = 1;
	                ESCFlagA.ESCCntSec.HWPowerStopDelay = 0;
		   		}
                if(ESCBYRelayCNTB != 1){
                    ESCFlagB.HWPowerSTOPFlag = 1;
                    ESCFlagB.ESCCntSec.HWPowerStopDelay = 0;
                }
                if(ESCBYRelayCNTC != 1){
                    ESCFlagC.HWPowerSTOPFlag = 1;
                    ESCFlagC.ESCCntSec.HWPowerStopDelay = 0;
                }
			}
		   break;
	   case 0x8003:
		   if(DATA == 1){
               if((ESCBYRelayCNTA != 1)&&(ESCSicFaultCNTA != 1)){
                   ESCFlagA.resetFlag = 1;
               }
               if((ESCBYRelayCNTB != 1)&&(ESCSicFaultCNTB != 1)){
                   ESCFlagB.resetFlag = 1;
               }
               if((ESCBYRelayCNTC != 1)&&(ESCSicFaultCNTC != 1)){
                   ESCFlagC.resetFlag = 1;
               }

		  		cntForRepFaultA = 0;				//自启清零
		  		cntForRepFaultB = 0;
		  		cntForRepFaultC = 0;
                ESCFlagA.HWPowerFAULTFlag = 0;
                ESCFlagB.HWPowerFAULTFlag = 0;
                ESCFlagC.HWPowerFAULTFlag = 0;
			}
		   break;
	   case 0x8004:break;
	   case 0x8005:break;
	   case 0x8006:break;
	   case 0x8007:break;
	   case 0x8008:
		   if(DATA<31) 	StateFlag.RespondFlag = DATA;			//蓝牙连接后闪烁从机指示灯
		   break;
	   case 0x8009:												//界面波形显示刷新
		   if((DATA == 1)&&(ReFFTDelayTim>=30)){//故障录波后的30s内不允许FFT模式
		       FaultRecSel.ForceRecordWaveRefresh = 1;
		   }
		   break;
	   case 0x800A:break;
	   case 0x800B:break;
	   case 0x800C:break;
	   case 0x800D:
		   if(  (DATA==0x5A)&&( ((StateEventFlag_A==STATE_EVENT_STANDBY_A)||(StateEventFlag_A==STATE_EVENT_FAULT_A))||\
		                        ((StateEventFlag_B==STATE_EVENT_STANDBY_B)||(StateEventFlag_B==STATE_EVENT_FAULT_B))||\
		                        ((StateEventFlag_C==STATE_EVENT_STANDBY_C)||(StateEventFlag_C==STATE_EVENT_FAULT_C)) )  )
		   {
		       StateFlag.WatchdogFlag = 1; 		//看门狗复位,解决参数需上电重启的问题,通信应答后执行
		       CntMs.WatchdogDelay = 0;
		   }
		   break;
	   case 0x800E:												//相序、谐波校正
		   if(DATA%2)	StateFlag.SequenceAutoFlag = 1;
		   StateFlag.PowerFactor = (DATA&0xFF00)>>8;
		   break;
	   case 0x800F:												//故障录波擦除
//		   WriteFlashConfig(304);	//测试用
		   RecordFlash.UiBlockRead = DATA & 0xFF;
           if(!RecordFlash.UiBlockRead)    RecordFlash.FlashBlockRead = RecordFlash.RecordBlockNum-1;
           else                            RecordFlash.FlashBlockRead = RecordFlash.UiBlockRead-1;
           FlashDataRead();

		   if((((DATA>>8)&0x01)==1)&&((StateEventFlag_A == STATE_EVENT_STANDBY_A)|| \
		                              (StateEventFlag_B == STATE_EVENT_STANDBY_B)|| \
		                              (StateEventFlag_C == STATE_EVENT_STANDBY_C))){
		       EraseSector();
		   }
		   break;
	   case 0x8010:												//SOE擦除
  			SOE.CurrentPointerPosition = 0;
   			StateFlag.SOEFullFlag = 0;
   			FM_DatWordWrite(SOE_WRITE_ADDR_GROUP,SOE.CurrentPointerPosition);   //故障组号保存
   			FM_DatWordWrite(SOE_WRITE_ADDR_FAULTFLAG,StateFlag.SOEFullFlag);	//故障内存标志位保存
   			for(iRecord=0;iRecord<RECORD_NUMBER;iRecord++)
   				FM_StructureDatReset(EEPROM_RECORD+sizeof(IncidentRecord[0])*iRecord*2, sizeof(IncidentRecord[0]));
		   break;
	   case 0x8011:break;										//多机并联均衡系数A
	   case 0x8012:break;										//多机并联均衡系数B
	   case 0x8013:break;										//多机并联均衡系数C
	   case 0x8014:break;										//恒无功电流或无功功率值
	   case 0x8015:
		   if(DATA==1)	StateFlag.onceTimeAdcAutoAdjust = 1;	//零偏校准刷新
		   break;
	   case 0x8016:
		   if((DATA&0x01)==1)	RecordFlash.FlashState.B.RemoteControlEn = 1;	//远程控制位
		   else					RecordFlash.FlashState.B.RemoteControlEn = 0;	//远程控制位
		   break;
	   case 0x8017:
	       if(DATA==0x5A)       StateFlag.FanctrlTestFlag = 1;
	       break;
	   default:break;
	}
}

/****************************主机应用层接口**********************************************/




