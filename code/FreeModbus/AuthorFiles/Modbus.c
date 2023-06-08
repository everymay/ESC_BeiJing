/*
 * Modbus.c
 *
 *  Created on: 2018Äê1ÔÂ29ÈÕ
 *      Author: Xu-yulin
 */
#include "Modbus.h"
#include "DSP28x_Project.h"
#include "stdlib.h"
#pragma DATA_SECTION(Modbuff,"EBSS1");
//#pragma DATA_SECTION(SCIC,"EBSS1");
//#pragma DATA_SECTION(SCIB,"EBSS1");
//#pragma DATA_SECTION(SCID,"EBSS1");
Uint16 Modbuff[3][256];
Uint16 SndbuffM[3][8];
struct ModbusSlaveStru SCIC=SCIC_DEFAULTS;
struct ModbusSlaveStru SCIB=SCIB_DEFAULTS;
struct ModbusSlaveStru SCID=SCID_DEFAULTS;
UCHAR   WrongData = 0;
UCHAR   RecdNone = 1;
UCHAR	CapCommErrCount = 0;
UCHAR   CapDataReceived=0;
const struct ModbusSlaveStru ModbusSciDefaults[3]={SCIC_DEFAULTS,SCIB_DEFAULTS,SCID_DEFAULTS};
const struct ModbusSlaveStru ModbusSciDefaultsM[3]={SCIC_M_DEFAULTS,SCIB_M_DEFAULTS,SCID_M_DEFAULTS};
//typedef enum
//{
//	COM1 = 0,
//	COM2 = 1,
//	COM3 = 2
//} COM_TypeDef;
//
//BOOL BSP_UART_Init(UART_HandleTypeDef *huart){
//
//}
//
//BOOL BSP_COM_Init(COM_TypeDef COM)
//{
//	bool err=0;
//
//	switch(){
//	case 0:
//	Uint16 *pSci1 = (Uint16 *)malloc(MB_SER_PDU_SIZE_MAX * sizeof(Uint16));
//	if(pSci1 != NULL){
//		memcpy(SCIA,ModbusSciDefaults,sizeof(SCIA));
//		ModbusSciDefaults.ModbusRcvSedBuff=pSci1;
//	}else{
//		err == true;
//	}
//#if Modbus_Master_Enable
//	PieVectTable.SCIRXINTC = &Master_serial_Rx_isr;
//	PieVectTable.SCITXINTC = &Master_serial_Tx_isr;
//#endif
// 	PieVectTable.SCITXINTA = &SCITXINTA_ISR;
//	IER |=M_INT9;
//	PieCtrlRegs.PIEIER9.bit.INTx2 = 1;
//	InitSciGpio(0);
////	SCICommuStateTransition();
//	break;
//
//	case 1:
//
//	break;
//	}
//	return err;
//}

//#if Modbus_Master_Enable
//	PieVectTable.SCIRXINTC = &Master_serial_Rx_isr;
//	PieVectTable.SCITXINTC = &Master_serial_Tx_isr;
//#elif Modbus_Slave_Enable
//	PieVectTable.SCIRXINTC = &serial_Rx_isr3;
//	PieVectTable.SCITXINTC = &serial_Tx_isr3;
////	PieVectTable.SCIRXINTA = &serial_Rx_isr1;
////	PieVectTable.SCITXINTA = &serial_Tx_isr1;
////	PieVectTable.SCIRXINTB = &serial_Rx_isr2;
////	PieVectTable.SCITXINTB = &serial_Tx_isr2;
//#endif
//
//#if INTERRUPT_SCIA_FIFO
// 	PieVectTable.SCITXINTA = &SCITXINTA_ISR;
//	IER |=M_INT9;
//	PieCtrlRegs.PIEIER9.bit.INTx2 = 1;
//#endif
//
//#if INTERRUPT_SCIB_FIFO
// 	PieVectTable.SCITXINTB = &SCITXINTB_ISR;
//	IER |=M_INT9;
//	PieCtrlRegs.PIEIER9.bit.INTx4 = 1;
//#endif
//
//#if INTERRUPT_SCIC_FIFO
// //	PieVectTable.SCITXINTC = &SCITXINTC_ISR;
//// 	PieVectTable.SCIRXINTC = &SCIRXINTC_ISR;
//	IER |=M_INT8;
//	PieCtrlRegs.PIEIER8.bit.INTx5 = 1;
//	PieCtrlRegs.PIEIER8.bit.INTx6 = 1;
//#endif
//



