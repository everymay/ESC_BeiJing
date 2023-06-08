/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#include "port.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
/* ----------------------- static functions ---------------------------------*/
//static void prvvUARTTxReadyISR(void);
//static void prvvUARTRxISR(void);
//#pragma CODE_SECTION(Master_serial_Tx_isr1, "ram2func");
//#pragma CODE_SECTION(Master_serial_Rx_isr1, "ram2func");
//#pragma CODE_SECTION(Master_serial_Tx_isr2, "ram2func");
//#pragma CODE_SECTION(Master_serial_Rx_isr2, "ram2func");
//#pragma CODE_SECTION(Master_serial_Tx_isr3, "ram2func");
//#pragma CODE_SECTION(Master_serial_Rx_isr3, "ram2func");
/* ----------------------- Start implementation -----------------------------*/
void vMBMasterPortSerialEnable(struct ModbusSlaveStru *pStu, BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	if(pStu->portaddr == 2){        //后台485-1
		if (xRxEnable){             //接收使能
			RS485_Pinc = RX_EN;
			asm(" RPT	 #7 || NOP ");
		//	PieCtrlRegs.PIEIER8.bit.INTx5 = 1;  //SCI_C_Rx
		//	PieCtrlRegs.PIEACK.bit.ACK8 =1;
		}else{
			RS485_Pinc = TX_EN;
			asm(" RPT	 #7 || NOP ");
		//	PieCtrlRegs.PIEIER8.bit.INTx5 = 0;  //SCI_C_Rx
		//	PieCtrlRegs.PIEACK.bit.ACK8 =1;
		}

		if (xTxEnable){
			RS485_Pinc = TX_EN;
			asm(" RPT	 #7 || NOP ");
		//	PieCtrlRegs.PIEIER8.bit.INTx6 = 1;//SCI_C_Tx
		//	PieCtrlRegs.PIEACK.bit.ACK8 =1;
		}else{
			RS485_Pinc = RX_EN;
			asm(" RPT	 #7 || NOP ");
		//	PieCtrlRegs.PIEIER8.bit.INTx6 = 0;//SCI_C_Tx
		//	PieCtrlRegs.PIEACK.bit.ACK8 =1;
		}
	}else if(pStu->portaddr == 0){   //蓝牙
		if (xRxEnable){
		//	PieCtrlRegs.PIEIER9.bit.INTx1 = 1;//SCI_A_Rx
		//	PieCtrlRegs.PIEACK.bit.ACK9 =1;
		}else{
		//	PieCtrlRegs.PIEIER9.bit.INTx1 = 0;//SCI_A_Rx
		//	PieCtrlRegs.PIEACK.bit.ACK9 =1;
		}

		if (xTxEnable){
		//	PieCtrlRegs.PIEIER9.bit.INTx2 = 1;//SCI_A_Tx
		//	PieCtrlRegs.PIEACK.bit.ACK9 =1;
		}else{  //失能{
		//	PieCtrlRegs.PIEIER9.bit.INTx2 = 0;//SCI_A_Tx
		//	PieCtrlRegs.PIEACK.bit.ACK9 =1;
		}
	}else{       //液晶屏485-2
		if (xRxEnable){
			RS485_Pinb = RX_EN;
			asm(" RPT	 #7 || NOP ");
		//	PieCtrlRegs.PIEIER9.bit.INTx3 = 1;//SCI_B_Rx
		//	PieCtrlRegs.PIEACK.bit.ACK9 =1;
		}else{
			RS485_Pinb = TX_EN;
			asm(" RPT	 #7 || NOP ");
		//	PieCtrlRegs.PIEIER9.bit.INTx3 = 0;//SCI_B_Rx
		//	PieCtrlRegs.PIEACK.bit.ACK9 =1;
		}

		if (xTxEnable){
			RS485_Pinb = TX_EN;
			asm(" RPT	 #7 || NOP ");
		//	PieCtrlRegs.PIEIER9.bit.INTx4 = 1;//SCI_B_Tx
		//	PieCtrlRegs.PIEACK.bit.ACK9 =1;
		}else{
			RS485_Pinb = RX_EN;
			asm(" RPT	 #7 || NOP ");
		//	PieCtrlRegs.PIEIER9.bit.INTx4 = 0;//SCI_B_Tx
		//	PieCtrlRegs.PIEACK.bit.ACK9 =1;
		}
	}
}

BOOL xMBMasterPortSerialInit(struct ModbusSlaveStru *pStu,ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
	//	 关于串口波特率计算的问题
	//	 BRR = ((SYS_CLK/4)/(BaudRate*8))-1
		(void)ucDataBits;  //不修改数据位长度
		(void)eParity;  //不修改检验格式
		//(void)ulBaudRate;
	//	BaudRate_Reg_temp = (Uint32)((150000000/(4*8*ulBaudRate))-1);//115200波特率计算有误
		EALLOW;
		DevCfgRegs.SOFTPRES7.bit.SCI_A       = 1;//使能时钟
//		SysCtrlRegs.PCLKCR0.bit.SCICENCLK = 1;//使能时钟
		EDIS;
		pStu->pReg->SCICCR.bit.ADDRIDLE_MODE = 0;//选择通信协议，选择空闲模式，此模式兼容RS232协议
		pStu->pReg->SCICCR.bit.LOOPBKENA = 0;//自测试模式使能位:为1使能，为0禁用
		pStu->pReg->SCICCR.bit.PARITY = 0;//奇偶校验位选择位:0为奇校验，1为偶校验
		pStu->pReg->SCICCR.bit.PARITYENA = 0;//奇偶校验位使能:为1使能，为0禁用
		pStu->pReg->SCICCR.bit.SCICHAR = 7;//该配置选择SCI的一个字符的长度可配置为1到8位(单位bit)
		pStu->pReg->SCICCR.bit.STOPBITS = 0;//停止位的配置: 0 1位停止位  1 2位停止位
		//SCIC控制寄存器的配置
	//	pStu->pReg->SCICTL1.bit.RXENA = 1;//SCI接收器使能位
	//	pStu->pReg->SCICTL1.bit.RXERRINTENA = 0;//SCI接受中断错误使能
	//	pStu->pReg->SCICTL1.bit.SLEEP = 0;//睡眠模式使能位
	//	pStu->pReg->SCICTL1.bit.SWRESET = 0;//SCI软件复位
	//	pStu->pReg->SCICTL1.bit.TXENA = 1;//SCI发送器使能位
	//	pStu->pReg->SCICTL1.bit.TXWAKE = 0;//SCI发送器唤醒模式选择位，具体的不是太明白
		pStu->pReg->SCICTL1.all =0x0003;      	// 使能发送和接收功能
		pStu->pReg->SCICTL2.all =0x0000;
		//-------------------------------------------------------------------
		if(ulBaudRate == 115200){
	        pStu->pReg->SCIHBAUD.all = 0x0000;      //波特率设置为115200 00,D8
	        pStu->pReg->SCILBAUD.all = 0x00D8;      //
		}else{
	        pStu->pReg->SCIHBAUD.all = 0x000A;      //波特率设置为9600 97,31
	        pStu->pReg->SCILBAUD.all = 0x002B;      //
		}
	//	pStu->pReg->SCIHBAUD = (BaudRate_Reg_temp>>8)&0xFF;
	//	pStu->pReg->SCILBAUD = BaudRate_Reg_temp&0xFF;
		pStu->pReg->SCICTL1.bit.SWRESET = 1;
		//以下代码是配置SCI的接收数据的FIFO和发送数据的FIFO
		//-------------------接收数据的FIFO配置
		pStu->pReg->SCIFFTX.bit.SCIRST = 0;//复位SCIC模块//复位SCI
		pStu->pReg->SCIFFTX.bit.SCIRST = 1;//复位SCIC模块//重启SCI
		pStu->pReg->SCIFFTX.bit.SCIFFENA = 1;//使能FIFO功能
		pStu->pReg->SCIFFRX.bit.RXFFIENA = 1;//使能接收中断
		pStu->pReg->SCIFFRX.bit.RXFIFORESET = 0;//复位接收器的FIFO
		pStu->pReg->SCIFFRX.bit.RXFIFORESET = 1;
		pStu->pReg->SCIFFRX.bit.RXFFIL = 0x01;//接受1个字节之后产生一个中断
		//---------------------发送数据的FIFO配置
		pStu->pReg->SCIFFTX.bit.SCIRST = 0;
		pStu->pReg->SCIFFTX.bit.SCIRST = 1;
		pStu->pReg->SCIFFTX.bit.SCIFFENA = 1;
		pStu->pReg->SCIFFTX.bit.TXFFIENA = 0;
		pStu->pReg->SCIFFTX.bit.TXFIFORESET=0;
		pStu->pReg->SCIFFTX.bit.TXFIFORESET=1;
		pStu->pReg->SCIFFTX.bit.TXFFIL = 0;//发送完一个字节产生中断
		pStu->pReg->SCIFFCT.all = 0;
		return TRUE;
}

BOOL xMBMasterPortSerialPutByte(UCHAR ucByte)
{
	 //发送一个字节
	ScicRegs.SCITXBUF.all = ucByte;
	return TRUE;
}
BOOL xMBMasterPortSerialGetByte( struct ModbusSlaveStru *pStu,UCHAR * pucByte )
{
	*pucByte= pStu->pReg->SCIRXBUF.all;
	return TRUE;
}

/*******************************************************************************
* @Name   : SCIC中断服务函数
*
* @Brief  : none
*
* @Input  : none
*
* @Output : none
*
* @Return : none
*******************************************************************************/
 void Master_serial_Rx_isr3(unsigned long arg1)
{
	pxMBMasterFrameCBByteReceived(&SCIC);
	ScicRegs.SCIFFRX.bit.RXFFINTCLR = 1;
	ScicRegs.SCIFFRX.bit.RXFFOVRCLR = 1;
//	PieCtrlRegs.PIEACK.bit.ACK8 =1;
}

 void Master_serial_Tx_isr3(unsigned long arg1)
{
	pxMBMasterFrameCBTransmitterEmpty(&SCIC);
	ScicRegs.SCIFFTX.bit.TXFFINTCLR = 1;
//	PieCtrlRegs.PIEACK.bit.ACK8 = 1;
}

 void Master_serial_Rx_isr1(unsigned long arg1)
{
	pxMBMasterFrameCBByteReceived(&SCIC);
	ScicRegs.SCIFFRX.bit.RXFFINTCLR = 1;
	ScicRegs.SCIFFRX.bit.RXFFOVRCLR = 1;
//	PieCtrlRegs.PIEACK.bit.ACK9 =1;
}

 void Master_serial_Tx_isr1(unsigned long arg1)
{
	pxMBMasterFrameCBTransmitterEmpty(&SCIC);
	ScicRegs.SCIFFTX.bit.TXFFINTCLR = 1;
//	PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}

 void Master_serial_Rx_isr2(unsigned long arg1)
{
	pxMBMasterFrameCBByteReceived(&SCIB);
	ScibRegs.SCIFFRX.bit.RXFFINTCLR = 1;
	ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1;
//	PieCtrlRegs.PIEACK.bit.ACK9 =1;
}

 void Master_serial_Tx_isr2(unsigned long arg1)
{
	pxMBMasterFrameCBTransmitterEmpty(&SCID);
	ScidRegs.SCIFFTX.bit.TXFFINTCLR = 1;
//	PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}

