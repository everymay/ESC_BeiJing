
#ifndef MARCODEFINE_H_
#define MARCODEFINE_H_
#include "CoreCtrl.h"

//用来定义不同电路板的
// #define   _NEW_BOARD_20141215   //  同时还需要放开main里面#define _NewBoard_init
//PWM输出为321，上桥SET,下桥CLEAR。电流霍尔指向IGBT，软件取正。
//负载电流采样为210
  

//#define   _LAB_50A_TEST
// ----------------------------  常用参数定义区 ----------------------------------//
// 125A  主发送同步
// PWM输出顺序321，上桥SET,下桥CLEAR，电流霍尔指向电抗器，程序取负。
// 负载电流采样顺序为201  DIANLIU

// 做认证的MEC ，电流霍尔指向IGBT，程序取正。#define CUR_HUOER_DIR_IGBT      1
//

// 100KVA MEC  测试散热片
// PWM输出顺序为123，上桥SET，下桥CLEAR，电流霍尔指向电抗器，程序取负。
// 负载电流采样顺序为201

/*  青岛 APF
	PWM输出顺序123，上桥SET,下桥CLEAR，电流霍尔指向电抗器，程序取负。
	负载电流采样顺序为201
*/
/*PWM输出顺序为123，上桥SET，下桥CLEAR，电流霍尔指向IGBT，程序取正。
    负载电流采样顺序为201
 */

// 150A  APF qianlong
//PWM输出为321，上桥SET,下桥CLEAR。电流霍尔指向电抗器，软件取负。
//负载电流采样为102

// 佟春月MEC柜子电流霍尔指向IGBT 1111，其他都和单元125A一样
// 王款的单元，电流霍尔指向IGBT，PWM顺序为ABC，其他都一样。

//#define MU_UNIT_NUM           1         // 第一 几个单元并联，这个参数最好发到LCD可以修改。        // 几个单元，在此。
//#define APF_ID               0x10      // 单元地址,0x10 - 0x17 支持8个地址

/*******************************************************************************/
#ifndef _LAB_50A_TEST
#define LAB_50A_APF             0
// 第二 电流霍尔朝向
#define CUR_HUOER_DIR_IGBT      1        // 1指向IGBT ；   0指向电抗器
/*******************************************************************************/
// 第三  PWM的输出顺序
#define PWM_ABC_FLAG            1        // 1表示ABC,123  0表示CBA 321
// 第六   外部电流互感器的采样顺序，主要针对方头电流互感器的老板子(102)
#define EXT_CUR_SMP_ABC         1         // 1表示ABC，即201，0表示CBA，即102
// 第七 上下桥臂的方向，这主要是针对驱动板不附加直流采样的电路板的。一般都是UP_SET
#define DRIVER_UP_AQ_CLEAR      0
#endif
/*******************************************************************************/
// 第十一 显示MEC还是APF的标志位，避免主界面显示有问题
// 	显示MEC，则主界面显示电网电流，监控信息显示功率因数。显示APF，则相对应都显示为总谐波畸变率
//#define DISPLAY_MEC_FLAG       1
/*******************************************************************************/
// 第十二 实验室50A APF实验程序，因硬件配置不一样，特此做宏定义
// laboratory 50AAPF-bench,两个地方：IO配置，sample
// 使能的办法为放开屏蔽，使用了ifdef 和 ifndef
//#define   _LAB_50A_TEST
/*******************************************************************************/
// 第十三 佟春月100KVA MEC 使用了IO5口进行风机的正反转控制，正常不吸合为正转，吸合为反转
// 但是第5口已经用来外部风机的控制，启动则外部风机也启动，反之停机。特此定义区分
#define CONTROL_FUN_REVERSE    0
/*******************************************************************************/
// 第十四 是否有液晶屏呢，没有的话自启动修改 ，设为1，有液晶屏设为0，户外柜设为1.
//#define NO_LED_AUTO_START      1
/*******************************************************************************/

//硬件电路板IO定义区
#define U1X1_1 LED_SYNC				//外部开出备用1
#define U1X1_2 FAULT_LED			//故障灯
#define U1X1_3 RUNNING_LED			//运行灯
#define U1X1_4 EXT_FUN_CTR			//外部风机
#define U1X1_5 BYIIOout3			//外部开出备用2
#define U1X1_6 START_INSTRUCTION	//绿色启动
#define U1X1_7 STOP_INSTRUCTION		//红色停机
#define U1X1_8 BYIOin1				//断路器状态,备用1
#define U1X1_9 FLback				//防雷反馈
#define U1X1_10 BYIOin3				//备用3
#if (TNMD150A200415REV1)

#elif(NPC3CA10020210330REV1)
    //板载输出
    #define SET_PULSE(VAL)                    if(VAL) GpioDataRegs.GPDSET.bit.GPIO110 = 1; else GpioDataRegs.GPDCLEAR.bit.GPIO110 = 1
    #define GET_PULSE()                               GpioDataRegs.GPDDAT.bit.GPIO110             //心跳检测

	/*
	 * 翻转GPIO电平。WY
	 */
    #define TOGGLE_PULSE()                            GpioDataRegs.GPDTOGGLE.bit.GPIO110 = 1;

    #define SET_MAIN_CONTACT_OFF(VAL)         if(VAL) GpioDataRegs.GPFSET.bit.GPIO163 = 1; else GpioDataRegs.GPFCLEAR.bit.GPIO163 = 1
	#define GET_MAIN_CONTACT_OFF()                    GpioDataRegs.GPFDAT.bit.GPIO163             //磁保持继电器分闸


	/*
	 * 设置A相主路磁保持继电器状态。WY
	 * VAL = 0，闭合；
	 * VAL = 1，断开。
	 */
	#define SET_MAIN_CONTACT_ACTION_A(VAL)    if((GET_POWER_CTRL == 0)&&(StateEventFlag_A != STATE_EVENT_STANDBY_A)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPCSET.bit.GPIO79 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO79 = 1



	/*
	 * A相主路磁保持继电器的控制引脚。WY
	 * 0，闭合；
	 * 1，断开。
	 */
	#define GET_MAIN_CONTACT_ACTION_A                 GpioDataRegs.GPCDAT.bit.GPIO79              // A相高/低压磁保持继电器

	/*
	 * 设置A相旁路磁保持继电器.WY
	 * VAL = 0，闭合；
	 * VAL = 1，断开。
	 */
    #define SET_BYPASS_CONTACT_ACTION_A(VAL)  if((GET_POWER_CTRL == 0)&&(StateEventFlag_A != STATE_EVENT_STANDBY_A)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPCSET.bit.GPIO78 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO78 = 1

	/*
	 * A相旁路磁保持继电器的控制引脚。WY
	 * 0：闭合；
	 * 1：断开。
	 */
	#define GET_BYPASS_CONTACT_ACTION_A               GpioDataRegs.GPCDAT.bit.GPIO78              // A相旁路磁保持继电器

	/*
	 * 设置B相主路磁保持继电器状态。WY
	 * VAL = 0，闭合；
	 * VAL = 1，断开。
	 */
    #define SET_MAIN_CONTACT_ACTION_B(VAL)    if((GET_POWER_CTRL == 0)&&(StateEventFlag_B != STATE_EVENT_STANDBY_B)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPESET.bit.GPIO151 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO151 = 1

	/*
	 * B相主路磁保持继电器控制引脚。WY
	 * 0：闭合；
	 * 1：断开。
	 */
	#define GET_MAIN_CONTACT_ACTION_B                 GpioDataRegs.GPEDAT.bit.GPIO151             // B相高/低压磁保持继电器


	/*
	 * 设置B相旁路磁保持继电器状态.WY
	 * VAL = 0，闭合；
	 * VAL = 1，断开。
	 */
	#define SET_BYPASS_CONTACT_ACTION_B(VAL)  if((GET_POWER_CTRL == 0)&&(StateEventFlag_B != STATE_EVENT_STANDBY_B)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPESET.bit.GPIO150 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO150 = 1

	/*
	 * B相旁路磁保持继电器的控制引脚。WY
	 * 0：闭合；
	 * 1：断开。WY*/
	#define GET_BYPASS_CONTACT_ACTION_B               GpioDataRegs.GPEDAT.bit.GPIO150             // B相旁路磁保持继电器

	/*
	 * 设置C相主路磁保持继电器状态。WY
	 * VAL = 0，闭合；
	 * VAL = 1，断开。
	 */
    #define SET_MAIN_CONTACT_ACTION_C(VAL)    if((GET_POWER_CTRL == 0)&&(StateEventFlag_C != STATE_EVENT_STANDBY_C)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPCSET.bit.GPIO77 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO77 = 1

	/*
	 * C相主路磁保持继电器控制引脚。WY
	 * 0，闭合；
	 * 1，断开。
	 */
    #define GET_MAIN_CONTACT_ACTION_C                 GpioDataRegs.GPCDAT.bit.GPIO77              // C相高/低压磁保持继电器

	/*
	 * 设置C相旁路磁保持继电器状态.WY
	 * VAL = 0，闭合；
	 * VAL = 1，断开。
	 */
    #define SET_BYPASS_CONTACT_ACTION_C(VAL)  if((GET_POWER_CTRL == 0)&&(StateEventFlag_C != STATE_EVENT_STANDBY_C)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPESET.bit.GPIO152 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO152 = 1

	/*
	 * C相旁路磁保持继电器的控制引脚。WY
	 * 0：闭合；
	 * 1：断开。WY*/
    #define GET_BYPASS_CONTACT_ACTION_C               GpioDataRegs.GPEDAT.bit.GPIO152             // C相旁路磁保持继电器

	/*
	 * 设置A相旁路晶闸管状态。WY
	 * VAL = 0：开通；
	 * VAL = 1：关断。
	 */
    #define SET_SCRA_ENABLE(VAL)              if((GET_POWER_CTRL == 0)&&(StateEventFlag_A != STATE_EVENT_STANDBY_A)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPESET.bit.GPIO154 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO154 = 1  //SCRA_EN

   	/*
	 * 设置B相旁路晶闸管状态。WY
	 * VAL = 0：开通；
	 * VAL = 1：关断。
	 */
    #define SET_SCRB_ENABLE(VAL)              if((GET_POWER_CTRL == 0)&&(StateEventFlag_B != STATE_EVENT_STANDBY_B)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPESET.bit.GPIO153 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO153 = 1  //SCRB_EN

   	/*
	 * 设置C相旁路晶闸管状态。WY
	 * VAL = 0：开通；
	 * VAL = 1：关断。
	 */
    #define SET_SCRC_ENABLE(VAL)              if((GET_POWER_CTRL == 0)&&(StateEventFlag_C != STATE_EVENT_STANDBY_C)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPESET.bit.GPIO155 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO155 = 1  //SCRC_EN

   	/*
	 * 设置设备运行指示灯状态。WY
	 * VAL = 0：灯灭；
	 * VAL = 1：灯亮。
	 */
    #define SET_RUNNING_LED(VAL)              if(VAL) GpioDataRegs.GPESET.bit.GPIO158 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO158 = 1

   	/*
	 * 设备运行指示灯状态。WY
	 * VAL = 0：灯灭；
	 * VAL = 1：灯亮。
	 */
    #define GET_RUNNING_LED                           GpioDataRegs.GPEDAT.bit.GPIO158             //设备运行指示灯

   	/*
	 * 设置设备故障指示灯状态。WY
	 * VAL = 0：灯灭；
	 * VAL = 1：灯亮。
	 */
    #define SET_FAULT_LED(VAL)                if(VAL) GpioDataRegs.GPBSET.bit.GPIO42 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO42 = 1

   	/*
	 * 设备故障指示灯状态。WY
	 * VAL = 0：灯灭；
	 * VAL = 1：灯亮。
	 */
    #define GET_FAULT_LED                             GpioDataRegs.GPBDAT.bit.GPIO42              //故障指示灯

	/*
	 * 急停按钮。WY
	 */
    #define STOP_INSTRUCTION                          GpioDataRegs.GPEDAT.bit.GPIO157             // 急停按钮

	/*
	 * 15V电源掉电检测引脚。WY
	 */
    #define GET_CTRL24_POWER                          GpioDataRegs.GPADAT.bit.GPIO1               // 15V控制电掉电检测

	/*
	 * 启停15V电源。WY
	 * 0：停止；
	 * 1：启动。
	 */
    #define SET_POWER_CTRL(VAL)               if(VAL) GpioDataRegs.GPDSET.bit.GPIO104 = 1; else GpioDataRegs.GPDCLEAR.bit.GPIO104 = 1

    #define GET_POWER_CTRL                            GpioDataRegs.GPDDAT.bit.GPIO104             // 控制15V电源供电引脚

    #define GET_GC_OVER_A                             GpioDataRegs.GPCDAT.bit.GPIO94              // A相硬件过流检测引脚
    #define GET_GC_OVER_B                             GpioDataRegs.GPCDAT.bit.GPIO85              // B相硬件过流检测引脚
    #define GET_GC_OVER_C                             GpioDataRegs.GPDDAT.bit.GPIO97              // C相硬件过流检测引脚
    #define SET_GV_VOL_CTRL_A(VAL)            if(VAL) GpioDataRegs.GPDSET.bit.GPIO105 = 1; else GpioDataRegs.GPDCLEAR.bit.GPIO105 = 1  //A相控制外加小继电器(防止在运输过程中磁保持发生误动作,上电没有预充电直接炸机,在输入磁保持和直流电容之间加一个小继电器,在上电几秒钟之后再开通)
    #define GET_GV_VOL_CTRL_A                         GpioDataRegs.GPDDAT.bit.GPIO105
    #define SET_GV_VOL_CTRL_B(VAL)            if(VAL) GpioDataRegs.GPCSET.bit.GPIO76 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO76 = 1    //B相控制外加小继电器(防止在运输过程中磁保持发生误动作,上电没有预充电直接炸机,在输入磁保持和直流电容之间加一个小继电器,在上电几秒钟之后再开通)
    #define GET_GV_VOL_CTRL_B                         GpioDataRegs.GPCDAT.bit.GPIO76
    #define SET_GV_VOL_CTRL_C(VAL)            if(VAL) GpioDataRegs.GPBSET.bit.GPIO43 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO43 = 1    //C相控制外加小继电器(防止在运输过程中磁保持发生误动作,上电没有预充电直接炸机,在输入磁保持和直流电容之间加一个小继电器,在上电几秒钟之后再开通)
    #define GET_GV_VOL_CTRL_C                         GpioDataRegs.GPBDAT.bit.GPIO43
    #define SET_FLback(VAL)                   if(VAL) GpioDataRegs.GPCSET.bit.GPIO91 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO91 = 1



	/*
	 * 防雷检测引脚。WY
	 * 0：正常；
	 * 1：异常。
	 */
    #define GET_FLback                                GpioDataRegs.GPCDAT.bit.GPIO91

    #define SET_BYPASS_FEEDBACK(VAL)          if(VAL) GpioDataRegs.GPCSET.bit.GPIO92 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO92 = 1

	/*
	 * 微型断路器检测引脚。WY
	 * 0，断开；
	 * 1，闭合。
	 */
//    #define GET_BYPASS_FEEDBACK                       GpioDataRegs.GPCDAT.bit.GPIO92

	/*
	 * 微型断路器检测引脚。WY
	 * 0，断开；
	 * 1，闭合。
	 *
	 * 由于硬件中去除了微型断路器引脚检测功能，为了保证程序兼容性，将微型断路器检测引脚恒设为1，表示：微型断路器始终闭合。
	 */
	#define GET_BYPASS_FEEDBACK 1

    #define SET_BOARDRUNNING_LED(VAL)         if(VAL) GpioDataRegs.GPBSET.bit.GPIO59 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1
    #define GET_BOARDRUNNING_LED                      GpioDataRegs.GPBDAT.bit.GPIO59    //-板载-设备运行指示灯
    #define SET_BOARDFAULT_LED(VAL)           if(VAL) GpioDataRegs.GPBSET.bit.GPIO61 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1
    #define GET_BOARDFAULT_LED                        GpioDataRegs.GPBDAT.bit.GPIO61    //-板载-故障指示灯
    #define SET_LED_SYNC(VAL)                 if(VAL) GpioDataRegs.GPBSET.bit.GPIO60 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1
    #define GET_LED_SYNC                              GpioDataRegs.GPBDAT.bit.GPIO60   //-板载-同步失效


//无用信号
    #define ExtCapCtrl1             SpuriousIn          //无用信号
    #define ExtCapCtrl2             SpuriousIn          //无用信号
    #define ExtCapCtrl3             SpuriousIn          //无用信号
    #define ExtCapCtrl4             SpuriousIn          //无用信号
    #define ExtCapCtrl5             SpuriousIn          //无用信号
    #define ExtCapCtrl6             SpuriousIn          //无用信号
    #define ExtCapCtrl7             SpuriousIn          //无用信号
    #define ExtCapCtrl8             SpuriousIn          //无用信号
    #define BYKIOout1               SpuriousIn          //无用信号
    #define BYKIOout3               SpuriousIn          //无用信号
    #define SET_PROG_RUNNING_LED(VAL)               //程序正常运行指示灯
    #define GET_PROG_RUNNING_LED()          SpuriousIn          //程序正常运行指示灯
    #define SET_MACHINE_RUNNING(VAL)                //设备运行指示灯
    #define GET_MACHINE_RUNNING()           SpuriousIn          //设备运行指示灯

#endif


	/*
	 * A相硬件PWM使能
     * 0：失能；
	 * 1：使能。
	 */
    #define SET_IGBT_ENA(VAL)     if(VAL) {if((GET_POWER_CTRL == 0)&&(StateEventFlag_A != STATE_EVENT_STANDBY_A)){SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                             GpioDataRegs.GPESET.bit.GPIO156 = 1; }else GpioDataRegs.GPECLEAR.bit.GPIO156 = 1

	/*
	 * B相硬件PWM使能
     * 0：失能；
	 * 1：使能。
	 */
    #define SET_IGBT_ENB(VAL)     if(VAL) {if((GET_POWER_CTRL == 0)&&(StateEventFlag_B != STATE_EVENT_STANDBY_B)){SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                             GpioDataRegs.GPFSET.bit.GPIO164 = 1; }else GpioDataRegs.GPFCLEAR.bit.GPIO164 = 1

	/*
	 * C相硬件PWM使能
     * 0：失能；
	 * 1：使能。
	 */
    #define SET_IGBT_ENC(VAL)     if(VAL) {if((GET_POWER_CTRL == 0)&&(StateEventFlag_C != STATE_EVENT_STANDBY_C)){SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                             GpioDataRegs.GPCSET.bit.GPIO93  = 1; }else GpioDataRegs.GPCCLEAR.bit.GPIO93  = 1

	/*
	 * A相硬件PWM使能检测引脚
     * 0：失能；
	 * 1：使能。
	 */
    #define GET_IGBT_ENA                          GpioDataRegs.GPEDAT.bit.GPIO156

	/*
	 * B相硬件PWM使能检测引脚
     * 0：失能；
	 * 1：使能。
	 */
    #define GET_IGBT_ENB                          GpioDataRegs.GPFDAT.bit.GPIO164

	/*
	 * C相硬件PWM使能检测引脚
     * 0：失能；
	 * 1：使能。
	 */
    #define GET_IGBT_ENC                          GpioDataRegs.GPCDAT.bit.GPIO93



#define ESC_ACTION_ENABLE           0
#define ESC_ACTION_DISABLE          1

#define ESC_FANACTION_ENABLE        1
#define ESC_FANACTION_DISABLE       0
// 这个四个宏只用来给输出的IO作用，其他均不起作用
// 低电平有效, 1和0，代表了高和低
#define ITS_WORKING_LOW             1
#define ITS_NO_WORKING_HIGH         0
// 高电平有效
#define ITS_WORKING_HIGH            0
#define ITS_NO_WORKING_LOW          1
// 这个两个个宏只用来给输入的IO作用，其他均不起作用
#define GET_INPUT_WORKING_LOW       0
#define NOT_GET_INPUT_HIGH          1

#define IGBT_ENABLE                 1
#define IGBT_DISABLE                0
#define IGBT_FAULT    	 		    IGBT_DISABLE
#define IGBT_FEEDBACK_FAULT_FLAG    0

/*A相：初始状态（正常停机）。WY*/
#define STATE_EVENT_STANDBY_A         0

/*A相：预充电状态。WY*/
#define STATE_EVENT_RECHARGE_A        1

/*A相：运行状态。WY*/
#define STATE_EVENT_RUN_A             2

/*A相：主动停机状态。WY*/
#define STATE_EVENT_STOP_A            3

/*A相：待机状态。WY*/
#define STATE_EVENT_WAIT_A            4

#define STATE_EVENT_BEFORERUN_A       5               //正常运行前的准备状态.
#define STATE_EVENT_DISCHARGE_A       6               //放电状态

/*A相：故障停机状态。WY*/
#define STATE_EVENT_FAULT_A           7

#define STATE_EVENT_WARNING_A         8              //告警状态
#define STATE_EVENT_ALARM_A           9              //警报状态
#define ORIGINAL_STATE_A              10              //自动复位_初始状态
#define AUTO_DETECTION_STATE_A        11              //自动复位_自检状态
#define TWEAK_STATE_A                 12              //自动复位_调整状态
#define INSPECTION_STATE_A            13              //自动复位_正常状态

/*B相：初始状态（正常停机）。WY*/
#define STATE_EVENT_STANDBY_B         0               //初始状态(正常停机)

/*B相：预充电状态。WY*/
#define STATE_EVENT_RECHARGE_B        1               //充电状态

/*B相：运行状态。WY*/
#define STATE_EVENT_RUN_B             2               //正常运行

/*B相：主动停机状态。WY*/
#define STATE_EVENT_STOP_B            3               //正常停机

/*B相：待机状态。WY*/
#define STATE_EVENT_WAIT_B            4               //待机状态
#define STATE_EVENT_BEFORERUN_B       5               //正常运行前的准备状态.
#define STATE_EVENT_DISCHARGE_B       6               //放电状态

/*B相：故障停机状态。WY*/
#define STATE_EVENT_FAULT_B           7               //故障停机
#define STATE_EVENT_WARNING_B         8              //告警状态
#define STATE_EVENT_ALARM_B           9              //警报状态
#define ORIGINAL_STATE_B              10              //自动复位_初始状态
#define AUTO_DETECTION_STATE_B        11              //自动复位_自检状态
#define TWEAK_STATE_B                 12              //自动复位_调整状态
#define INSPECTION_STATE_B            13              //自动复位_正常状态

/*C相：初始状态（正常停机）。WY*/
#define STATE_EVENT_STANDBY_C         0               //初始状态(正常停机)

/*C相：预充电状态。WY*/
#define STATE_EVENT_RECHARGE_C        1               //充电状态

/*C相：运行状态。WY*/
#define STATE_EVENT_RUN_C             2               //正常运行

/*C相：主动停机状态。WY*/
#define STATE_EVENT_STOP_C            3               //正常停机

/*C相：待机状态。WY*/
#define STATE_EVENT_WAIT_C            4               //待机状态
#define STATE_EVENT_BEFORERUN_C       5               //正常运行前的准备状态.
#define STATE_EVENT_DISCHARGE_C       6               //放电状态

/*C相：故障停机状态。WY*/
#define STATE_EVENT_FAULT_C           7               //故障停机

#define STATE_EVENT_WARNING_C         8              //告警状态
#define STATE_EVENT_ALARM_C           9              //警报状态
#define ORIGINAL_STATE_C              10              //自动复位_初始状态
#define AUTO_DETECTION_STATE_C        11              //自动复位_自检状态
#define TWEAK_STATE_C                 12              //自动复位_调整状态
#define INSPECTION_STATE_C            13              //自动复位_正常状态

#endif /* MARCODEFINE_H_ */
