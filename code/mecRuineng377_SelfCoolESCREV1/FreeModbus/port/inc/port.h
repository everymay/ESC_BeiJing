/*
 * FreeModbus Libary: BARE Port
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
 * File: $Id: port.h ,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#ifndef _PORT_H
#define _PORT_H


#include <inttypes.h>
#include "mbconfig.h"
#include <assert.h>
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"

#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }
//#define assert(e) ((e) ? (void)0 :  1)
#define assert_param(expr) ((void)0)
void __assert(const char * x1, const char * x2, int x3);


//数据类型定义
typedef Uint16 UCHAR;
typedef int16 CHAR;
typedef bool BOOL;
typedef Uint16 USHORT;
typedef int16 SHORT;
typedef Uint32 ULONG;
typedef int32 LONG;

//485 接口宏定义
//***************************************************slave*****************************************************//
#define RS485_Pinb GpioDataRegs.GPADAT.bit.GPIO26   //后台485-2
#define RS485_Pinc GpioDataRegs.GPDDAT.bit.GPIO103  //液晶屏485-1


#define TX_EN 1
#define RX_EN 0
#define ENTER_CRITICAL_SECTION()   INTX_DISABLE()   //__set_PRIMASK(1)  //关总中断
#define EXIT_CRITICAL_SECTION()    INTX_ENABLE()    //__set_PRIMASK(0)  //开总中断
void INTX_DISABLE(void);
void  INTX_ENABLE(void);
 void serial_Rx_isr1(unsigned long arg1);
 void serial_Tx_isr1(unsigned long arg1);
 void serial_Rx_isr2(unsigned long arg1);
 void serial_Tx_isr2(unsigned long arg1);
 void serial_Rx_isr3(unsigned long arg1);
 void serial_Tx_isr3(unsigned long arg1);
 void Master_serial_Rx_isr1(unsigned long arg1);
 void Master_serial_Tx_isr1(unsigned long arg1);
 void Master_serial_Rx_isr2(unsigned long arg1);
 void Master_serial_Tx_isr2(unsigned long arg1);
 void Master_serial_Rx_isr3(unsigned long arg1);
 void Master_serial_Tx_isr3(unsigned long arg1);

/* master mode: holding register's all address */
#define          M_HD_RESERVE                     0
/* master mode: input register's all address */
#define          M_IN_RESERVE                     0
/* master mode: coil's all address */
#define          M_CO_RESERVE                     0
/* master mode: discrete's all address */
#define          M_DI_RESERVE                     0

#endif
