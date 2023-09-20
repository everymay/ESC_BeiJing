#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "MarcoDefine.h"
#include "VariblesRef.h"
#include "DSP28x_Project.h"
#include "CoreCtrl.h"
#include "stdio.h"
#include "VarMean.h"
#include "Flash2833x_API_Library.h"
#include "F2837xS_GlobalPrototypes.h"
#include <xdc/runtime/Log.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include "F021_F2837xS_C28x.h"
#include "DPLL.h"

#pragma	CODE_SECTION(EPWM7INT ,"ram2func")

/*
 * 文件版本：V2023.8.12
 * 修改日期：2023年8月12日
 */

/*调试程序。WY*/

void main(void)
{
	InitSysCtrl(); //初始化系统。WY

	/*将代码从Flash拷贝至RAM。WY*/
	MemCopy(&dclfuncsLoadStart, &dclfuncsLoadEnd, &dclfuncsRunStart);
	MemCopy(&mathTablesLoadStart, &mathTablesLoadEnd, &mathTablesRunStart);
	MemCopy(&Cla1funcsLoadStart, &Cla1funcsLoadEnd, &Cla1funcsRunStart);
	MemCopy(&Cla1ConstLoadStart, &Cla1ConstLoadEnd, &Cla1ConstRunStart);

    InitFlash_Bank0(); //初始化Flash。WY
    CLA_configClaMemory(); //配置CLA存储空间。注意：本项目中未使用CLA。WY
    CLA_initCpu1Cla1(); //初始化CLA。注意：本项目中未使用CLA。WY

    InitSciGpio(); //配置SCI（串口）。WY
	InitCapGroupAct(); //此函数与电容投切相关，删除？？WY
    InitECap(); //配置ECPA（脉冲捕获）。WY
    InitECapGpio(); //配置ECAP（脉冲捕获）相关引脚。WY

    SET_SPIWPn(1);
	InitSpiGpio(); //初始化SPI相关引脚。WY
    InitSpib(); //初始化SPI-B。WY
    InitSpic(); //初始化SPI-C。WY

	InitGpio_NewBoard(); //配置GPIO。WY
    InitBeforeReadROM(); //读取铁电参数前的初始化操作。WY
	OutputGpioInit_NEWBOARD(); //初始化GPIO。WY
	ReadSpiPara(); //铁电存储芯片。WY

    initDPLL(&SPLL[0]); //锁相系数初始化的位置一定要放在铁电读取数据之后.
    initDPLL(&SPLL[1]); //虚拟alfbet锁相，三相独立。
    initDPLL(&SPLL[2]);

    InitAfterReadROM(); //读取铁电后的初始化操作。WY
	InitModbusSCI(); //初始化Modbus。WY
	InitCtrlParam(0); //初始化PI控制器参数。WY
	InitParameters();	//Initialization of key parameters
	DMAInitialize(); //本项目未启用DMA。WY
    InitAdc(); //配置ADC。WY
    InitEPwmGpio(); //配置EPWM相关的引脚。WY
    InitEPwm(); //配置EPWM。WY

	Log_info1("start bios====%d", 0); //打印日志
    BIOS_start(); //启动BIOS。WY
}

/* 委托实验可以进行：1绝缘 2轻载 3负载 4保护 5温升 6输出电压变化 7输出频率变化实验（可选）
 * 					8总谐波补偿率 9输出限流能力 10损耗测试 11响应时间测试 12噪声测试
 * 					13电磁兼容实验 14外壳防护实验 15结构和外观
 */

/*此中断未使用。WY*/
void  TINT0_ISR(void)
{
    StopCpuTimer0();
}

/*此中断未使用。WY*/
void EPWM7INT(void)
{
	EPwm7Regs.ETCLR.bit.INT = 1;		//epwm4周期中断复位
}

/*此中断未使用。WY*/
interrupt void ECAP4_INT_ISR(void)
{
	ECap4Regs.ECCLR.bit.CEVT1 = 1;
	ECap4Regs.ECCLR.bit.INT = 1;
	ECap4Regs.ECCTL2.bit.REARM = 1;
}


void initCodeCopy(void)
{
    DisableDog();
    MemCopy(&knlLoadStart,&knlLoadEnd,&knlRunStart);
    MemCopy(&ram2funcLoadStart,&ram2funcLoadEnd,&ram2funcRunStart);
    MemCopy(&RamfuncsLoadStart,&RamfuncsLoadEnd,&RamfuncsRunStart);
}




