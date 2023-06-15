//###########################################################################
//
// FILE:   F2837xS_I2C.c
//
// TITLE:  F2837xS I2C Initialization & Support Functions.
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

void InitI2C(void);
void InitI2CGpio(void);
Uint16 I2CA_WriteData(void);
Uint16 I2CA_ReadData(void);
void I2CA_StateDeal(void);
void I2CRead(Uint16 ADD,Uint16 Len);
void I2CWrite(Uint16 ADD,Uint16 Len);
void I2CA_sr(void);

#define I2C_SLAVE_ADDR        0x50  //EEPROM地址
#define I2C_WNUMBYTES         0x2   //为方便示波器观察，设置发送2字节的数据
#define I2C_RNUMBYTES         0x10
#define I2C_EEPROM_HIGH_ADDR  0x00  //数据的写入地址高位
#define I2C_EEPROM_LOW_ADDR   0x00  //数据的写入地址低位
//#pragma DATA_SECTION(I2cMsgOut1,"EBSS3");
//#pragma DATA_SECTION(I2cMsgIn1,"EBSS3");
struct I2CMSG I2cMsgOut1={I2C_MSGSTAT_INACTIVE,//初始状态为：发送带停止位数据
						  I2C_SLAVE_ADDR,
						  I2C_WNUMBYTES,
						  I2C_EEPROM_HIGH_ADDR,
						  I2C_EEPROM_LOW_ADDR,
						  0,
                          0x11,                   // Msg Byte 1
                          0x22,                   // Msg Byte 2
                          0x56,                   // Msg Byte 3
                          0x78,                   // Msg Byte 4
                          0x9A,                   // Msg Byte 5
                          0xBC,                   // Msg Byte 6
                          0xDE,                   // Msg Byte 7
                          0xF0,                   // Msg Byte 8
                          0x11,                   // Msg Byte 9
                          0x10,                   // Msg Byte a
                          0x21,                   // Msg Byte b
                          0x21,                   // Msg Byte c
                          0x32,                   // Msg Byte d
                          0x42,          		  // Msg Byte e
                          0x33,          		  // Msg Byte f
						  };
struct I2CMSG I2cMsgIn1={ I2C_MSGSTAT_INACTIVE,
						  I2C_SLAVE_ADDR,
						  I2C_RNUMBYTES,
						  I2C_EEPROM_HIGH_ADDR,
						  I2C_EEPROM_LOW_ADDR,
						  0
						};
Uint16 I2CWADD=0,I2CWLen = 4,I2CRADD=0,I2CRLen = 16;
Uint16 I2CCount=0;

//---------------------------------------------------------------------------
// InitI2C: 
//---------------------------------------------------------------------------
void InitI2C(void)
{
	Uint16 I2Ci;
	// Initialize I2C-A IO:
	InitI2CGpio();
	// Initialize I2C
	I2caRegs.I2CSAR.all = 0x0050;		// Slave address 0x50
	I2caRegs.I2CPSC.all = 249;		// Prescaler - need 7-12 Mhz on module clk,I2C分频系数,则 /10
									// LOSPCP =75M,1M speed
	I2caRegs.I2CCLKL = 10;			// NOTE: must be non zero  时钟信号的低电平时间=10
	I2caRegs.I2CCLKH = 5;			// NOTE: must be non zero  时钟信号的高电平时间=5

	I2caRegs.I2CIER.all = 0x0;		// Enable SCD & ARDY interrupts
	//总线仲裁失败中断使能位,无应答信号中断使能位,数据接收就绪中断使能位,数据发送就绪中断使能位,从机地址中断使能位
	I2caRegs.I2CMDR.all = 0x0020;	// Take I2C out of reset
	//7位地址模式,从机模式,接收模式,在总线上接收到停止位（STOP）后STP会自动清除,在总线上接收到开始位（START）后STT将自动清除,非循环模式,屏蔽自测,I2C模块使能,I2C模块起始信号无需延长,屏蔽全数据格式,8位数据
	// Stop I2C when suspended
	I2caRegs.I2CFFTX.all = 0x6000;	// Enable FIFO mode and TXFIFO
	I2caRegs.I2CFFRX.all = 0x2040;	// Enable RXFIFO, clear RXFFINT,

//	GpioDataRegs.GPACLEAR.bit.GPIO27 = 1;//关写保护
	GpioDataRegs.GPASET.bit.GPIO27 = 1;//开写保护

	for (I2Ci = 0; I2Ci < 32; I2Ci++)
	{
		I2cMsgIn1.MsgBuffer[I2Ci] = 0x0000;
	}

}	

void InitI2CGpio()
{

    EALLOW;

	GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO32 (SDAA)
	GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;	   // Enable pull-up for GPIO33 (SCLA)
	GpioCtrlRegs.GPAPUD.bit.GPIO27 = 0;    // Enable pull-up for GPIO33 (WPIO)

	GpioCtrlRegs.GPADIR.bit.GPIO27 = 1;    // GPIO33 for output(wpio)

	GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // Asynch input GPIO32 (SDAA)
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // Asynch input GPIO33 (SCLA)

	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;   // Configure GPIO32 for SDAA operation
	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;   // Configure GPIO33 for SCLA operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0;   // Configure GPIO27 for WPIO operation
	
    EDIS;
}

//最多写14个半字，多了就飞了，15个数有可能写最后一个时候，第一个已经发完了，所以有可能会实现，fifo的问题
Uint16 I2CA_WriteData(void)
{
	Uint16 I2CA_WriteC=0;

	if (I2caRegs.I2CMDR.bit.STP == 1)return I2C_STP_NOT_READY_ERROR;
	if (I2caRegs.I2CSTR.bit.BB == 1)return I2C_BUS_BUSY_ERROR;

	I2caRegs.I2CCNT = I2cMsgOut1.NumOfBytes + 2;
	I2caRegs.I2CDXR.all = I2cMsgOut1.MemoryHighAddr;
	I2caRegs.I2CDXR.all = I2cMsgOut1.MemoryLowAddr;
	if(I2cMsgOut1.NumOfBytes > 14)I2CA_WriteC = 14;
	else I2CA_WriteC = I2cMsgOut1.NumOfBytes;
	for (I2cMsgOut1.NumOfnow=0; I2cMsgOut1.NumOfnow< I2CA_WriteC; I2cMsgOut1.NumOfnow++)
		I2caRegs.I2CDXR.all = I2cMsgOut1.MsgBuffer[I2cMsgOut1.NumOfnow];
	I2caRegs.I2CMDR.all = 0x2E20;
	return I2C_SUCCESS;

}

Uint16 I2CA_ReadData(void)
{
	if (I2caRegs.I2CMDR.bit.STP == 1)
		return I2C_STP_NOT_READY_ERROR;
	if(I2cMsgIn1.MsgStatus == I2C_MSGSTAT_SEND_NOSTOP) //产生第一个START位之后发送设备地址和数据地址////
	{
		if (I2caRegs.I2CSTR.bit.BB == 1)
		  return I2C_BUS_BUSY_ERROR;
		I2caRegs.I2CCNT = 2;
		I2caRegs.I2CDXR.all = I2cMsgIn1.MemoryHighAddr;//发送要读取数据的开始地址
		I2caRegs.I2CDXR.all = I2cMsgIn1.MemoryLowAddr;
		I2caRegs.I2CMDR.all = 0x2620;     // 按以下参数配置IIC并使能IIC
	}
	else if(I2cMsgIn1.MsgStatus == I2C_MSGSTAT_RESTART)//产生第2个START位之后发送设备地址，然后开始接收存储器提供的数据//
	{
		I2caRegs.I2CCNT = I2cMsgIn1.NumOfBytes; // 设置数据接收字节数量
		I2caRegs.I2CMDR.all = 0x2C20;     // 按以下参数配置IIC并使能IIC
	}
	return I2C_SUCCESS;    //到此说明发送地址/读取数据成功
}

Uint16 I2CA_RandomRead(void)
{

	if (I2caRegs.I2CMDR.bit.STP == 1)return I2C_STP_NOT_READY_ERROR;
	if (I2caRegs.I2CSTR.bit.BB == 1)return I2C_BUS_BUSY_ERROR;

	I2caRegs.I2CCNT = 1;
	I2caRegs.I2CMDR.all = 0x2C20;

	return I2C_SUCCESS;

}

void I2CA_StateDeal(void)
{
	Uint16 I2CStateC = 0,I2CStateD=0,I2CStateE=0;

	if (I2cMsgIn1.MsgStatus == I2C_MSGSTAT_INACTIVE)//读完成之前不让写
	{
		if(I2cMsgOut1.MsgStatus == I2C_MSGSTAT_SEND_WITHSTOP)
	   	{
			GpioDataRegs.GPACLEAR.bit.GPIO27=1;//关写保护
	   		asm(" RPT	 #7 || NOP ");
	  		if (I2CA_WriteData() == I2C_SUCCESS)//数据已成功放入缓存寄存器
	   			I2cMsgOut1.MsgStatus = I2C_MSGSTAT_WRITE_BUSY;//设为发送数据忙状态
	   	}  // end of write section
		else if(I2cMsgOut1.MsgStatus == I2C_MSGSTAT_WRITE_BUSY)
		{
			if(I2cMsgOut1.NumOfBytes > 14)
			{
				if(I2cMsgOut1.NumOfnow < I2cMsgOut1.NumOfBytes)
				{
					if(I2caRegs.I2CFFTX.bit.TXFFST < 16)
					{
						I2CStateD = 16 - I2caRegs.I2CFFTX.bit.TXFFST;
						I2CStateE = I2cMsgOut1.NumOfBytes - I2cMsgOut1.NumOfnow;
						if(I2CStateE < I2CStateD)
							I2CStateD = I2CStateE;
						for(I2CStateC = 0;I2CStateC<I2CStateD;I2CStateC++)
						{
							I2caRegs.I2CDXR.all = I2cMsgOut1.MsgBuffer[I2cMsgOut1.NumOfnow];
							I2cMsgOut1.NumOfnow++;
						}
					}
				}
			}
		}
		else if(I2cMsgOut1.MsgStatus == I2C_MSGSTAT_WRITE_WAIT_BUSY)
		{
			if (I2CA_RandomRead() == I2C_SUCCESS)//数据已成功放入缓存寄存器
			{
				if(I2caRegs.I2CFFRX.bit.RXFFST != 0)
				{
					I2CStateC = I2caRegs.I2CDRR.all;
					I2cMsgOut1.MsgStatus = I2C_MSGSTAT_INACTIVE;//设为发送数据忙状态
				}
			}
		}
	}

	if (I2cMsgOut1.MsgStatus == I2C_MSGSTAT_INACTIVE)//读完成之前不让写
	{
	   	if(I2cMsgIn1.MsgStatus == I2C_MSGSTAT_SEND_NOSTOP)
	   	{
	   		while(I2CA_ReadData() != I2C_SUCCESS){}
	   		I2cMsgIn1.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP_BUSY;
	   	}

	   	else if(I2cMsgIn1.MsgStatus == I2C_MSGSTAT_RESTART)//该状态是在中断服务程序中态被赋予
	   	{
	   		while(I2CA_ReadData() != I2C_SUCCESS){}
	   		I2cMsgIn1.MsgStatus = I2C_MSGSTAT_READ_BUSY;
	   	}
	   	else if (I2cMsgIn1.MsgStatus == I2C_MSGSTAT_READ_BUSY)
	    {
	   		if(I2caRegs.I2CFFRX.bit.RXFFST != 0)
	   		{
	   			I2CStateD = I2caRegs.I2CFFRX.bit.RXFFST;
	   			for(I2CStateC=0; I2CStateC < I2CStateD; I2CStateC++)
	   			{
	   				  I2cMsgIn1.MsgBuffer[I2cMsgIn1.NumOfnow] = I2caRegs.I2CDRR.all;
	   				  I2cMsgIn1.NumOfnow++;
	   			}
	   		}
	    }
	}// end of read section

	if(I2caRegs.I2CSTR.bit.SCD == 1)//停止处理
	{
	  	if (I2cMsgOut1.MsgStatus == I2C_MSGSTAT_WRITE_BUSY)
	  	{
	  		I2cMsgOut1.MsgStatus = I2C_MSGSTAT_WRITE_WAIT_BUSY;
	  		I2cMsgOut1.NumOfnow = 0;
	  		GpioDataRegs.GPASET.bit.GPIO27 = 1;//关写保护
	  		asm(" RPT	 #7 || NOP ");
	  	}
	  	else
	  	{
	  		if(I2cMsgIn1.MsgStatus == I2C_MSGSTAT_SEND_NOSTOP_BUSY)//发送无停止位地址忙
	  			I2cMsgIn1.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	  		else if (I2cMsgIn1.MsgStatus == I2C_MSGSTAT_READ_BUSY) //读数据忙状态，说明可对接收缓存器I2CDRR进行接收数据读取了
	  		{
	  			I2cMsgIn1.MsgStatus = I2C_MSGSTAT_INACTIVE;//设为未启状态动，再读数据，这样允许下一次写数据到EEPROM
	  			I2cMsgIn1.NumOfnow = 0;
	  		}
	  	}
	  	I2caRegs.I2CSTR.bit.SCD = 1;
	 }
	 else if(I2caRegs.I2CSTR.bit.ARDY == 1)//就绪处理
	 {
	  	if(I2caRegs.I2CSTR.bit.NACK == 1)//因为EEPROM处理数据忙而回复的NACK
	  	{
	  		I2caRegs.I2CMDR.bit.STP = 1;//产生一个STOP停止位，从而引发一次SCD中断。
	  		I2caRegs.I2CSTR.all = I2C_CLR_NACK_BIT; //清除NACK位
	  	}
	  	else if(I2cMsgIn1.MsgStatus == I2C_MSGSTAT_SEND_NOSTOP_BUSY)
	  		I2cMsgIn1.MsgStatus = I2C_MSGSTAT_RESTART;//更新为重发START位状态，为接收数据准备
	  	I2caRegs.I2CSTR.bit.ARDY = 1;
	 }
}
//---------------------------------------------------
//I2C e2prom读功能，可以连续读。读可以8words。
//I2C e2prom最大读写地址可以达到（0-0x20000）
//每一页256byte，不能跨页读写，多余256的数会被减少256的
//Len为多少个半字（8位的）
//读每个字的时候，由于e2prom本身的容易出错的，需要做3选2的校验
//---------------------------------------------------
void I2CRead(Uint16 ADD,Uint16 Len)
{

	if(ADD > 0xffff)
		I2cMsgIn1.SlaveAddress = 0x51;
	else
		I2cMsgIn1.SlaveAddress = 0x50;

	ADD = ADD & 0xffff;
	I2cMsgIn1.MemoryHighAddr = ADD >> 8;
	I2cMsgIn1.MemoryLowAddr = ADD & 0xff;
	I2cMsgIn1.NumOfBytes = Len;

	if(I2cMsgOut1.MsgStatus == 0 && I2cMsgIn1.MsgStatus == 0)
		I2cMsgIn1.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;

}
//---------------------------------------------------
//I2C e2prom写功能，不可以连续读。写可以最多7words
//每次写之后必须留出2ms，这个时间既不能读也不能写e2p，否则e2p读写功能都不能用了
//I2C e2prom最大读写地址可以达到（0-0x20000）
//每一页256byte，不能跨页读写，多余256的数会被减少256的
//Len为多少个半字（8位的）
//---------------------------------------------------
void I2CWrite(Uint16 ADD,Uint16 Len)
{
	if(ADD > 0xffff)
		I2cMsgOut1.SlaveAddress = 0x51;
	else
		I2cMsgOut1.SlaveAddress = 0x50;

	ADD = ADD & 0xffff;
	I2cMsgOut1.MemoryHighAddr = ADD >> 8;
	I2cMsgOut1.MemoryLowAddr = ADD & 0xff;
	I2cMsgOut1.NumOfBytes = Len;

	if(I2cMsgOut1.MsgStatus == 0 && I2cMsgIn1.MsgStatus == 0)
		I2cMsgOut1.MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;

}

/* I2C 写入EE注意事项：renjg 1223
 * 1、每次可写最大254个8bit一页，不超254页
 * 2、延时完成后，再进入读写的状态。
 * 3、上电的时候需要先处于读状态，读完数据并且校验完成后，再进入写状态。
 * 4、读完后，再进入写状态，跳入写状态的标识，需要参数下发通信中给出。
 * 5、通信中下发写EE的标志位，要在写完、延时、校验后，才可以进行清零。
 * 6、除非重新上电，I2C将一直处于等待写状态。
 */

void I2CA_sr(void)
{
	if(I2cMsgOut1.MsgStatus == 0 && I2cMsgIn1.MsgStatus == 0)
	{
		// 加进入读写的标识，状态存在等待读和等待写。
		if(I2CCount == 0)
		{
			// 写入工作
			I2cMsgOut1.MsgBuffer[0] = 0x1234 >> 8;
			I2cMsgOut1.MsgBuffer[1] = 0x1234 & 0xff;
			I2cMsgOut1.MsgBuffer[2] = 0x5678 >> 8;
			I2cMsgOut1.MsgBuffer[3] = 0x5678 & 0xff;
			I2cMsgOut1.MsgBuffer[4] = 0x9012 >> 8;
			I2cMsgOut1.MsgBuffer[5] = 0x9012 & 0xff;
			I2cMsgOut1.MsgBuffer[6] = 0x3456 >> 8;
			I2cMsgOut1.MsgBuffer[7] = 0x3456 & 0xff;
			I2CWrite(I2CWADD,I2CWLen);
			// 增加跳转条件
			I2CCount = 1;
		}
		else
		{

			I2CRead(I2CRADD,I2CRLen);
			// 读取工作

			// 增加跳转条件
			I2CCount = 0;
		}

	}
}

//
//---------------------------------------------------------------------------
// Example: I2cAGpioConfig(), I2cBGpioConfig()
//---------------------------------------------------------------------------
// These functions configures GPIO pins to function as I2C pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.
//

#ifdef CPU1
//
// I2cAGpioConfig - Configure I2CA GPIOs
//                  'I2caDataClkPin' should be assign with one of the
//                   possible I2C_A SDA - SDL GPIO pin Use defined Macros from
//                   "F2837xS_I2c_defines.h" for  assignment
//
void I2cAGpioConfig(Uint16 I2caDataClkPin)
{
    EALLOW;

    switch(I2caDataClkPin)
    {
        case I2C_A_GPIO0_GPIO1:

            //
            // Enable internal pull-up for the selected I2C pins
            // Enable pull-up for GPIO0 (SDAA)
            // Enable pull-up for GPIO1 (SDLA)
            //
            GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;
            GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;

            //
            // Set qualification for the selected I2C pins
            // Async/no qualification (I/ps sync to SYSCLKOUT by default)
            //
            GpioCtrlRegs.GPAQSEL1.bit.GPIO0 = 3;
            GpioCtrlRegs.GPAQSEL1.bit.GPIO1 = 3;

            //
            // Configure which of the possible GPIO pins will be I2C_A pins
            // using GPIO regs
            // Configure GPIO0 for SDAA operation
            // Configure GPIO0 for SDAA operation
            //
            GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 1;
            GpioCtrlRegs.GPAMUX1.bit.GPIO0  = 2;

            //
            // Configure GPIO1 for SDLA operation
            // Configure GPIO1 for SDLA operation
            //
            GpioCtrlRegs.GPAGMUX1.bit.GPIO1 = 1;
            GpioCtrlRegs.GPAMUX1.bit.GPIO1  = 2;

            break;

        case I2C_A_GPIO32_GPIO33:
            //
            // Enable internal pull-up for the selected I2C pins
            //
            GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;
            GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;

            //
            // Set qualification for the selected I2C pins
            //
            GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;
            GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;

            //
            // Configure which of the possible GPIO pins will be I2C_A pins
            // using GPIO regs
            //
            GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;
            GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;

            break;

        case I2C_A_GPIO42_GPIO43:
            //
            // Enable internal pull-up for the selected I2C pins
            //

            //
            // Set qualification for the selected I2C pins
            //

            //
            // Configure which of the possible GPIO pins will be I2C_A pins
            // using GPIO regs
            //
            break;

        case I2C_A_GPIO91_GPIO92:
            //
            // Enable internal pull-up for the selected I2C pins
            //
            GpioCtrlRegs.GPCPUD.bit.GPIO91 = 0;
            GpioCtrlRegs.GPCPUD.bit.GPIO92 = 0;

            //
            // Set qualification for the selected I2C pins
            //
               GpioCtrlRegs.GPCQSEL2.bit.GPIO91 = 3;
               GpioCtrlRegs.GPCQSEL2.bit.GPIO92 = 3;

            //
            // Configure which of the possible GPIO pins will be I2C_A pins
            // using GPIO regs
            //
               GpioCtrlRegs.GPCGMUX2.bit.GPIO91 = 1;
            GpioCtrlRegs.GPCMUX2.bit.GPIO91 = 2;
               GpioCtrlRegs.GPCGMUX2.bit.GPIO92 = 1;
            GpioCtrlRegs.GPCMUX2.bit.GPIO92 = 2;
            break;

        case I2C_A_GPIO63104_GPIO105:
            //
            // Enable internal pull-up for the selected I2C pins
            //

            //
            // Set qualification for the selected I2C pins
            //

            //
            // Configure which of the possible GPIO pins will be I2C_A pins
            // using GPIO regs
            //
            break;

        default:

            break;

    } // End of Switch
    EDIS;
}

//
// I2cBGpioConfig - Configure I2CB GPIOs
//                  'I2cbDataClkPin' should be assign with one of the possible
//                  I2C_B SDA - SDL GPIO pin Use defined Macros from
//                  "F2837xS_I2c_defines.h" for assignment
//
void I2cBGpioConfig(Uint16 I2cbDataClkPin)
{
    EALLOW;

    switch(I2cbDataClkPin)
    {
        case I2C_B_GPIO2_GPIO3:
            //
            // Enable internal pull-up for the selected I2C pins
            // Enable pull-up for GPIO2 (SDAB)
            // Enable pull-up for GPIO3 (SDLB)
            //
            GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;
            GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;

            //
            // Set qualification for the selected I2C pins
            // Async/no qualification (I/ps sync to SYSCLKOUT by default)
            //
            GpioCtrlRegs.GPAQSEL1.bit.GPIO2 = 3;
            GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 3;

            //
            // Configure which of the possible GPIO pins will be I2C_B pins
            // using GPIO regs
            // Configure GPIO2 for SDAB operation
            // Configure GPIO3 for SDAB operation
            // Configure GPIO1 for SDLB operation
            // Configure GPIO1 for SDLB operation
            //
            GpioCtrlRegs.GPAGMUX1.bit.GPIO2 = 1;
            GpioCtrlRegs.GPAMUX1.bit.GPIO2  = 2;

            GpioCtrlRegs.GPAGMUX1.bit.GPIO3 = 1;
            GpioCtrlRegs.GPAMUX1.bit.GPIO3  = 2;

            break;

        case I2C_B_GPIO134_GPIO35:
            //
            // Enable internal pull-up for the selected I2C pins
            //

            //
            // Set qualification for the selected I2C pins
            //

            //
            // Configure which of the possible GPIO pins will be I2C_B pins
            // using GPIO regs
            //
            break;

        case I2C_B_GPIO40_GPIO41:
            //
            // Enable internal pull-up for the selected I2C pins
            //

            //
            // Set qualification for the selected I2C pins
            //

            //
            // Configure which of the possible GPIO pins will be I2C_B pins
            // using GPIO regs
            //
            break;

        case I2C_B_GPIO66_GPIO69:
            //
            // Enable internal pull-up for the selected I2C pins
            //
            GpioCtrlRegs.GPCPUD.bit.GPIO66 = 0;    //SDAB
            GpioCtrlRegs.GPCPUD.bit.GPIO69 = 0;    //SCLB

            //
            // Set qualification for the selected I2C pins
            //
            GpioCtrlRegs.GPCQSEL1.bit.GPIO66 = 3;
            GpioCtrlRegs.GPCQSEL1.bit.GPIO69 = 3;

            //
            // Configure which of the possible GPIO pins will be I2C_B pins
            // using GPIO regs
            //
            GpioCtrlRegs.GPCGMUX1.bit.GPIO66 = 1;    //0x6
            GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 2;

            GpioCtrlRegs.GPCGMUX1.bit.GPIO69 = 1;    //0x6
            GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 2;
            break;

        default:
            break;

    }
    EDIS;
}

#endif


//
// End of file
//
