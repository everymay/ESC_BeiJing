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
 * File: $Id: portserial.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

#include "port.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "MarcoDefine.h"
/* ----------------------- static functions ---------------------------------*/
//void prvvUARTTxReadyISR( void );
//void prvvUARTRxISR( void );
#pragma	CODE_SECTION(serial_Rx_isr1 ,"ram2func")
#pragma	CODE_SECTION(serial_Tx_isr1 ,"ram2func")
#pragma	CODE_SECTION(serial_Rx_isr2 ,"ram2func")
#pragma	CODE_SECTION(serial_Tx_isr2 ,"ram2func")
#pragma	CODE_SECTION(serial_Rx_isr3 ,"ram2func")
#pragma	CODE_SECTION(serial_Tx_isr3 ,"ram2func")
/* ----------------------- Start implementation -----------------------------*/
void vMBPortSerialEnable(struct ModbusSlaveStru *pStu, BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     * ���xRXEnable���ô��н����жϡ����xTxENable���÷��������жϡ�
     */
    if(pStu->portaddr == 2){        //��̨485-1
        if (xRxEnable){             //����ʹ��
            RS485_Pinc = RX_EN;
            asm(" RPT    #7 || NOP ");
          PieCtrlRegs.PIEIER8.bit.INTx7 = 1;
          PieCtrlRegs.PIEACK.bit.ACK8 =1;
        }else{
            RS485_Pinc = TX_EN;
            asm(" RPT    #7 || NOP ");
          PieCtrlRegs.PIEIER8.bit.INTx7 = 0;
          PieCtrlRegs.PIEACK.bit.ACK8 =1;
        }

        if (xTxEnable){
            RS485_Pinc = TX_EN;
            asm(" RPT    #7 || NOP ");
          PieCtrlRegs.PIEIER8.bit.INTx8 = 1;
          PieCtrlRegs.PIEACK.bit.ACK8 =1;
        }else{
            RS485_Pinc = RX_EN;
            asm(" RPT    #7 || NOP ");
          PieCtrlRegs.PIEIER8.bit.INTx8 = 0;
          PieCtrlRegs.PIEACK.bit.ACK8 =1;
        }
    }else if(pStu->portaddr == 0){   //����
        if (xRxEnable){
          PieCtrlRegs.PIEIER8.bit.INTx5 = 1;
          PieCtrlRegs.PIEACK.bit.ACK8 =1;
        }else{
          PieCtrlRegs.PIEIER8.bit.INTx5 = 0;
          PieCtrlRegs.PIEACK.bit.ACK8 =1;
        }

        if (xTxEnable){
          PieCtrlRegs.PIEIER8.bit.INTx6 = 1;
          PieCtrlRegs.PIEACK.bit.ACK8 =1;
        }else{  //ʧ��{
          PieCtrlRegs.PIEIER8.bit.INTx6 = 0;
          PieCtrlRegs.PIEACK.bit.ACK8 =1;
        }
    }else{       //Һ����485-2
        if (xRxEnable){
            RS485_Pinb = RX_EN;
            asm(" RPT    #7 || NOP ");
          PieCtrlRegs.PIEIER9.bit.INTx3 = 1;
          PieCtrlRegs.PIEACK.bit.ACK9 =1;
        }else{
            RS485_Pinb = TX_EN;
            asm(" RPT    #7 || NOP ");
          PieCtrlRegs.PIEIER9.bit.INTx3 = 0;
          PieCtrlRegs.PIEACK.bit.ACK9 =1;
        }

        if (xTxEnable){
            RS485_Pinb = TX_EN;
            asm(" RPT    #7 || NOP ");
          PieCtrlRegs.PIEIER9.bit.INTx4 = 1;//SCI_B_Tx
          PieCtrlRegs.PIEACK.bit.ACK9 =1;
        }else{
            RS485_Pinb = RX_EN;
            asm(" RPT    #7 || NOP ");
          PieCtrlRegs.PIEIER9.bit.INTx4 = 0;//SCI_B_Tx
          PieCtrlRegs.PIEACK.bit.ACK9 =1;
        }
    }
}

BOOL xMBPortSerialInit( struct ModbusSlaveStru *pStu, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
//	 ���ڴ��ڲ����ʼ��������
//	 BRR = ((SYS_CLK/4)/(BaudRate*8))-1
	(void)ucDataBits;  //���޸�����λ����
	(void)eParity;  //���޸ļ����ʽ
//  (void)ulBaudRate;
//	BaudRate_Reg_temp = (Uint32)((150000000/(4*8*ulBaudRate))-1);//115200�����ʼ�������
//	EALLOW;
//  DevCfgRegs.SOFTPRES7.bit.SCI_A       = 1;//��λģ��
//  SysCtrlRegs.PCLKCR0.bit.SCICENCLK   = 1;//ʹ��ʱ��
//	EDIS;
	pStu->pReg->SCICCR.bit.ADDRIDLE_MODE = 0;//ѡ��ͨ��Э�飬ѡ�����ģʽ����ģʽ����RS232Э��
	pStu->pReg->SCICCR.bit.LOOPBKENA = 0;//�Բ���ģʽʹ��λ:Ϊ1ʹ�ܣ�Ϊ0����
	pStu->pReg->SCICCR.bit.PARITY = 0;//��żУ��λѡ��λ:0Ϊ��У�飬1ΪżУ��
	pStu->pReg->SCICCR.bit.PARITYENA = 0;//��żУ��λʹ��:Ϊ1ʹ�ܣ�Ϊ0����
	pStu->pReg->SCICCR.bit.SCICHAR = 7;//������ѡ��SCI��һ���ַ��ĳ��ȿ�����Ϊ1��8λ(��λbit)
	pStu->pReg->SCICCR.bit.STOPBITS = 0;//ֹͣλ������: 0 1λֹͣλ  1 2λֹͣλ
	//SCIC���ƼĴ���������
//	pStu->pReg->SCICTL1.bit.RXENA = 1;//SCI������ʹ��λ
//	pStu->pReg->SCICTL1.bit.RXERRINTENA = 0;//SCI�����жϴ���ʹ��
//	pStu->pReg->SCICTL1.bit.SLEEP = 0;//˯��ģʽʹ��λ
//	pStu->pReg->SCICTL1.bit.SWRESET = 0;//SCI�����λ
//	pStu->pReg->SCICTL1.bit.TXENA = 1;//SCI������ʹ��λ
//	pStu->pReg->SCICTL1.bit.TXWAKE = 0;//SCI����������ģʽѡ��λ������Ĳ���̫����
	pStu->pReg->SCICTL1.all =0x0003;      	// ʹ�ܷ��ͺͽ��չ���
	pStu->pReg->SCICTL2.all =0x0000;
	//-------------------------------------------------------------------
    if(ulBaudRate == 115200){
        pStu->pReg->SCIHBAUD.all = 0x0000;      //����������Ϊ115200 00,D8     ����ʹ�õ�wifi������
        pStu->pReg->SCILBAUD.all = 0x00D8;      //
    }else if(ulBaudRate == 9600){
        pStu->pReg->SCIHBAUD.all = 0x000A;      //����������Ϊ9600 0A,2B ����
        pStu->pReg->SCILBAUD.all = 0x002B;      //
    }else{
        pStu->pReg->SCIHBAUD.all = 0x0001;      //����������Ϊ57600(����wifi) 01,B2 WIFI  //BAUD=LSPCLK/((BRR+1)*8)=200000000/((57600+1)*8)=BEBC200/70808=1B2;
        pStu->pReg->SCILBAUD.all = 0x00B2;      //
    }
//	pStu->pReg->SCIHBAUD = (BaudRate_Reg_temp>>8)&0xFF;
//	pStu->pReg->SCILBAUD = BaudRate_Reg_temp&0xFF;
	pStu->pReg->SCICTL1.bit.SWRESET = 1;
	//���´���������SCI�Ľ������ݵ�FIFO�ͷ������ݵ�FIFO
	//-------------------�������ݵ�FIFO����
	pStu->pReg->SCIFFTX.bit.SCIRST = 0;//��λSCICģ��
	pStu->pReg->SCIFFTX.bit.SCIRST = 1;//��λSCICģ��
	pStu->pReg->SCIFFTX.bit.SCIFFENA = 1;//ʹ��FIFO����
	pStu->pReg->SCIFFRX.bit.RXFFIENA = 1;//ʹ�ܽ����ж�
	pStu->pReg->SCIFFRX.bit.RXFIFORESET = 0;//��λ��������FIFO
	pStu->pReg->SCIFFRX.bit.RXFIFORESET = 1;
	pStu->pReg->SCIFFRX.bit.RXFFIL = 0x01;//����1���ֽ�֮�����һ���ж�
	//---------------------�������ݵ�FIFO����
	pStu->pReg->SCIFFTX.bit.SCIRST = 0;
	pStu->pReg->SCIFFTX.bit.SCIRST = 1;
	pStu->pReg->SCIFFTX.bit.SCIFFENA = 1;
	pStu->pReg->SCIFFTX.bit.TXFFIENA = 0;
	pStu->pReg->SCIFFTX.bit.TXFIFORESET=0;
	pStu->pReg->SCIFFTX.bit.TXFIFORESET=1;
	pStu->pReg->SCIFFTX.bit.TXFFIL = 0;//������һ���ֽڲ����ж�
	pStu->pReg->SCIFFCT.all = 0;
	return TRUE;
}
//BOOL xMBPortSerialPutByte( CHAR ucByte )
//{
//	 //����һ���ֽ�
//	ScicRegs.SCITXBUF = ucByte;
//	return TRUE;
//}
//inline BOOL xMBPortSerialGetByte( struct ModbusSlaveStru *pStu,CHAR * pucByte )
//{
//	*pucByte= pStu->pReg->SCIRXBUF.bit.RXDT;
//	return TRUE;
//}
/*******************************************************************************
* @Name   : SCIC�жϷ�����
*
* @Brief  : none
*
* @Input  : none
*
* @Output : none
*
* @Return : none
*******************************************************************************/
void serial_Rx_isr3(unsigned long arg1)
{
	pxMBFrameCBByteReceived(&SCID);
	ScidRegs.SCIFFRX.bit.RXFFINTCLR = 1;        //scic������
	ScidRegs.SCIFFRX.bit.RXFFOVRCLR = 1;
	PieCtrlRegs.PIEACK.bit.ACK8 =1;
}
void serial_Tx_isr3(unsigned long arg1)
{
	pxMBFrameCBTransmitterEmpty(&SCID);
	ScidRegs.SCIFFTX.bit.TXFFINTCLR = 1;
	PieCtrlRegs.PIEACK.bit.ACK8 = 1;
}
void serial_Rx_isr1(unsigned long arg1)
{
	pxMBFrameCBByteReceived(&SCIC);
	ScicRegs.SCIFFRX.bit.RXFFINTCLR = 1;        //
	ScicRegs.SCIFFRX.bit.RXFFOVRCLR = 1;
	PieCtrlRegs.PIEACK.bit.ACK8 =1;
}
void serial_Tx_isr1(unsigned long arg1)
{
	pxMBFrameCBTransmitterEmpty(&SCIC);
	ScicRegs.SCIFFTX.bit.TXFFINTCLR = 1;
	PieCtrlRegs.PIEACK.bit.ACK8 = 1;
}
void serial_Rx_isr2(unsigned long arg1)
{
	pxMBFrameCBByteReceived(&SCIB);
	ScibRegs.SCIFFRX.bit.RXFFINTCLR = 1;
	ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1;
	PieCtrlRegs.PIEACK.bit.ACK9 =1;
}
void serial_Tx_isr2(unsigned long arg1)
{
	pxMBFrameCBTransmitterEmpty(&SCIB);
	ScibRegs.SCIFFTX.bit.TXFFINTCLR = 1;
	PieCtrlRegs.PIEACK.bit.ACK9 = 1;
}
