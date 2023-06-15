#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "DSP28x_Project.h"
#include "math.h"
#include "MarcoDefine.h"
#include "VariblesRef.h"
#include "F2837xS_GlobalPrototypes.h"
#include "DCL.h"
#include "DCLCLA.h"
#include "DPLL.h"

void FunAutoControl(void);
void SetStartCtrl(void);
void PIAdjust(void);
void BlindingBluetoothConnectionLED(void);
extern Event_Handle Event_SetStartSM;
extern Event_Handle Event_FMspiWriteCtrl;
extern Event_Handle Event_Outside;

/* 电气上启动和停止控制 0725
*  状态事件0 ： 控制电源取自电网，假设手动模式，。
*  状态事件1 ： 正在软起充电， 充电完成确认，接触器闭合  ---- 跳转依据电网电压幅值不为0
*  状态事件2 ： 调制           ----- 等待接收到启动指令，或者运行常态状态2 -----------注意进入稳定母线电压前，通过小限幅来软起
*  状态事件3 ： 正常停机 ----- 接收到停机指令状态3，完成后进入状态1，等待调制。-----注意软关的操作，电压环和电流环的给定递减
*  状态事件8 ： 测试模式 ----- 添加软件测试硬件的子程序
*  状态事件9 ： 故障处理 ----- 手动急停停机、故障停机，均进入此状态。
*  手动急停和故障急停的输出区别：手动急停停机后，进入状态2，等待调制；故障停机后进入故障9，并输出报警。
*  调试使用：测试模块的完好性，可以通过将直流侧电压通过逆变器PWM开通自消耗能量，实现快速放电。
*  假如按钮只有启动和急停两种信号，充电回路采用电阻直接连接方式（三合一电路板）；
*  	      当状态事件2得到急停信号，首先电流降到5%，再全部断开开关，
*      否则控制环未停止的状态下停机，极易造成直流母线剧升，额定情况下可能会炸机。
*      假设故障时拍急停，会因故障状态9自动封锁PWM，电流瞬间为零，实现故障停机。
*/
// 手动停机，直流母线电压处于高压，系统显示：如果按启动就是启动，如果按停机就是停机。
// 假设手动停机后，需要检修，直流侧电压采样板水泥电阻放电，等待五分钟



/*
 * 功能：初始化GPIO输出。WY
 */
void OutputGpioInit_NEWBOARD(void)
{
	SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_DISABLE); //断开A相主路磁保持继电器。WY
	SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_DISABLE); //断开B相主路磁保持继电器。WY
	SET_MAIN_CONTACT_ACTION_C(ESC_ACTION_DISABLE); //断开C相主路磁保持继电器。WY

	SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //闭合A相旁路磁保持继电器。WY
	SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE); //闭合B相旁路磁保持继电器闭合。WY
	SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE); //闭合C相旁路磁保持继电器闭合。WY

	SET_SCRA_ENABLE(ESC_ACTION_DISABLE); //关断A相旁路晶闸管。WY
	SET_SCRB_ENABLE(ESC_ACTION_DISABLE); //关断B相旁路晶闸管。WY
	SET_SCRC_ENABLE(ESC_ACTION_DISABLE); //关断C相旁路晶闸管。WY

	SET_RUNNING_LED(0); //设备运行指示灯灭。WY
	SET_FAULT_LED(0); //设备故障指示灯灭。WY

	SET_PROG_RUNNING_LED(1); //此函数无意义。WY
	SET_MACHINE_RUNNING(1); //此函数无意义。WY

	SET_BOARDRUNNING_LED(1); //板载运行指示灯亮。WY
	SET_LED_SYNC(1); //GPIO引脚不对应？WY
	SET_BOARDFAULT_LED(1); //GPIO引脚不对应？WY

	SET_IGBT_ENA(0); //失能A相PWM。WY
	SET_IGBT_ENB(0); //失能B相PWM。WY
	SET_IGBT_ENC(0); //失能C相PWM。WY
}

#define ESC_STANDY_DELAY_TIME_POINT 100

/*
 * 功能：执行【初始】状态（正常停机）。WY
 * 输入参数：待执行的X相参数。X的取值集合为{A, B, C}。
 */
void ESCSTANDBYEVENT(ESCCTRLVALFLAG *ESCFlag)
{
	/*处理A相。WY*/
	if (ESCFlag->PHASE == 1)
	{
		SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //闭合A相旁路磁保持继电器。WY
		SET_IGBT_ENA(IGBT_DISABLE); //失能A相PWM。WY
		DisablePWMA(); //封锁A相PWM。WY

		/*
		 * 以下函数功能存疑。理由如下：WY
		 * GPIO79始终为输出模式。通过读取该引脚电平状态来反馈磁保持继电器状态无意义。
		 */
		if (GET_MAIN_CONTACT_ACTION_A == ESC_ACTION_ENABLE) //A相主路磁保持继电器闭合。WY
		{
			SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_DISABLE); //断开A相主路磁保持继电器。WY
		}
	}

	/*处理B相。WY*/
	if (ESCFlag->PHASE == 2)
	{
		SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE);
		SET_IGBT_ENB(IGBT_DISABLE);
		DisablePWMB();
		if (GET_MAIN_CONTACT_ACTION_B == ESC_ACTION_ENABLE)
		{
			SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_DISABLE);
		}
	}

	/*处理C相。WY*/
	if (ESCFlag->PHASE == 3)
	{
		SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE);
		SET_IGBT_ENC(IGBT_DISABLE);
		DisablePWMC();
		if (GET_MAIN_CONTACT_ACTION_C == ESC_ACTION_ENABLE)
		{
			SET_MAIN_CONTACT_ACTION_C(ESC_ACTION_DISABLE);
		}
	}

	/*单次状态检测。WY*/
	if (ESCFlag->onceTimeStateMachine == 1)
	{
		/*若旁路磁保持继电器未闭合，则循环检测旁路磁保持继电器状态，直至超过（超时时长：5s）。WY*/
		if (ESCFlag->ByPassContactFlag == 1) //旁路磁保持继电器闭合。WY
		{
			/*处理A相。WY*/
			if (ESCFlag->PHASE == 1)
			{
				SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_DISABLE); //断开A相主路磁保持继电器。WY
				SET_SCRA_ENABLE(ESC_ACTION_DISABLE); //关断旁路晶闸管。WY
				ESCFlag->onceTimeStateMachine = 0; //复位单次状态检测标志位。WY
			}

			/*处理B相。WY*/
			if (ESCFlag->PHASE == 2)
			{
				SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_DISABLE);
				SET_SCRB_ENABLE(ESC_ACTION_DISABLE);
				ESCFlag->onceTimeStateMachine = 0;
			}

			/*处理C相。WY*/
			if (ESCFlag->PHASE == 3)
			{
				SET_MAIN_CONTACT_ACTION_C(ESC_ACTION_DISABLE);
				SET_SCRC_ENABLE(ESC_ACTION_DISABLE);
				ESCFlag->onceTimeStateMachine = 0;
			}
		}

		/*
		 * 处理A相。WY
		 * 若旁路磁保持继电器闭合异常，经过5s后，则进入该判断分支；
		 * 若旁路磁保持继电器闭合正常，则跳过该判断分支。
		 */
		if ((ESCFlag->ESCCntMs.ContactorDelay >= CNT_MS(5000)) //发生超时（超时时长：5s）。WY
				&& (StateEventFlag_A == STATE_EVENT_STANDBY_A)) //A相处于初始状态（正常停机）。WY
		{
			if (softwareFaultWord2.B.ESCBypassContNoCloseFlagA == 0) //旁路磁保持继电器闭合正常。WY
			{
				ESCFlagA.FAULTCONFIRFlag = 1; //存在故障信号。WY
				ESCFlagA.ESCCntMs.StartDelay = 0;
				softwareFaultWord2.B.ESCBypassContNoCloseFlagA = FaultDetect(SOE_GP_FAULT + 27, CNT_BYPASS_NOCLOSE_A, 2, ESCFlagA.PHASE);
			}
		}
		else //未发生超时（超时时长：5s）。WY
		{
			SetFaultDelayCounter(CNT_BYPASS_NOCLOSE_A, 0); //清空已检测到的A相旁路磁保持继电器闭合故障信号的次数。WY
		}

		/*处理B相。WY*/
		if ((ESCFlag->ESCCntMs.ContactorDelay >= CNT_MS(5000)) && (StateEventFlag_B == STATE_EVENT_STANDBY_B))
		{
			if (softwareFaultWord2.B.ESCBypassContNoCloseFlagB == 0)
			{
				ESCFlagB.FAULTCONFIRFlag = 1;
				ESCFlagB.ESCCntMs.StartDelay = 0;
				softwareFaultWord2.B.ESCBypassContNoCloseFlagB = FaultDetect(SOE_GP_FAULT + 28, CNT_BYPASS_NOCLOSE_B, 2, ESCFlagB.PHASE);
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_BYPASS_NOCLOSE_B, 0);
		}

		/*处理C相。WY*/
		if ((ESCFlag->ESCCntMs.ContactorDelay >= CNT_MS(5000)) && (StateEventFlag_C == STATE_EVENT_STANDBY_C))
		{
			if (softwareFaultWord2.B.ESCBypassContNoCloseFlagC == 0)
			{
				ESCFlagC.FAULTCONFIRFlag = 1;
				ESCFlagC.ESCCntMs.StartDelay = 0;
				softwareFaultWord2.B.ESCBypassContNoCloseFlagC = FaultDetect(SOE_GP_FAULT + 29, CNT_BYPASS_NOCLOSE_C, 2, ESCFlagC.PHASE);
			}
		}
		else
		{
			SetFaultDelayCounter(CNT_BYPASS_NOCLOSE_C, 0);
		}

	}
}

/*
 * 功能：执行【预充电】状态。WY。
 * 输入参数：待执行的X相参数。X的取值集合为{A, B, C}。
 */
void ESCPRECHARGEEVENT(ESCCTRLVALFLAG *ESCFlag)
{
	ESCFlag->EscStandyDelayCnt1 = 0;
	ESCFlag->EscStandyDelayCnt2 = 0;

	if (ESCFlag->ESCCntSec.ChargingTime >= CNT_SEC(3)) //充电时长满足条件。WY
	{
		if (++ESCFlag->EscWaitRunDelayCnt >= 300) //计时单位是？作用是？WY
		{
			/*处理A相*/
			if ((ESCFlag->PHASE == 1) //A相。WY
					&&((GET_MAIN_CONTACT_ACTION_A == ESC_ACTION_DISABLE) //A相主路磁保持继电器处于断开状态。WY
					|| (ESCFlag->DelayFlag == 1))) //在特定条件下。使得本分支的判断条件恒成立。WY
			{
				ESCFlag->EscWaitRunDelayCnt = 300; //强制赋值。目的：防止该数据递增后，发生上溢。WY
				ESCFlag->DelayFlag = 1; //使得本分支的判断条件恒成立。WY

				SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //闭合A相主路磁保持继电器。WY

				/* WY
				 * 由于：1. 磁保持继电器动作需要时间；2. 对磁保持继电器的状态检测作连续采样处理；WY
				   故在发送闭合A相主路磁保持继电器的指令后，无法立即检测到A相主路磁保持继电器处于闭合状态，所以需要循环检测。
				 */
				if (ESCFlag->ContactorFeedBackFlag == 1) //A相主路磁保持继电器闭合。该标志位在AD-ISR中被修改。WY
				{
					ESCFlag->DelayFlag = 0;
					StateEventFlag_A = STATE_EVENT_WAIT_A; //切换A相状态机至：待机状态。WY
					ESCFlag->ESCCntSec.S_RSwiTiDelay = 0;
					ESCFlag->ESCCntSec.HWPowerSupDelay = 0;
				}
			}

			/*处理B相*/
			if ((ESCFlag->PHASE == 2) && ((GET_MAIN_CONTACT_ACTION_B == ESC_ACTION_DISABLE) || (ESCFlag->DelayFlag == 1)))
			{
				ESCFlag->EscWaitRunDelayCnt = 300;
				ESCFlag->DelayFlag = 1;
				SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_ENABLE);
				if (ESCFlag->ContactorFeedBackFlag == 1)
				{
					ESCFlag->DelayFlag = 0;
					StateEventFlag_B = STATE_EVENT_WAIT_B;
					ESCFlag->ESCCntSec.S_RSwiTiDelay = 0;
					ESCFlag->ESCCntSec.HWPowerSupDelay = 0;
				}
			}

			/*处理C相*/
			if ((ESCFlag->PHASE == 3) && ((GET_MAIN_CONTACT_ACTION_C == ESC_ACTION_DISABLE) || (ESCFlag->DelayFlag == 1)))//磁保持继电器开通逻辑是低电平开通;
			{
				ESCFlag->EscWaitRunDelayCnt = 300;
				ESCFlag->DelayFlag = 1;
				SET_MAIN_CONTACT_ACTION_C(ESC_ACTION_ENABLE);
				if (ESCFlag->ContactorFeedBackFlag == 1)
				{
					ESCFlag->DelayFlag = 0;
					StateEventFlag_C = STATE_EVENT_WAIT_C;
					ESCFlag->ESCCntSec.S_RSwiTiDelay = 0;
					ESCFlag->ESCCntSec.HWPowerSupDelay = 0;
				}
			}
		}
	}
	else //充电时长未满足条件。WY
	{
		ESCFlag->EscWaitRunDelayCnt = 0;
	}
}

/*
 * 功能：执行【运行】状态。WY
 * 输入参数：待执行的X相参数。X的取值集合为{A, B, C}。
 */
void ESCRUNEVENT(ESCCTRLVALFLAG *ESCFlag)
{
	if ((ESCFlag->ESCCntMs.PllDelay > CNT_MS(1000)) //为锁相环锁相分配的时长 > 设定值。WY
			&& (ESCFlag->onceTimeStateMachine == 0))
	{
		switch (StateFlag.VoltageMode)
		{
			case 0://升压。WY
			case 4:
			{
				if ((ESCFlag->VoltIn_rms > ESCFlag->StableValue)  //电网电压有效值 > 稳定值，表明：无需升压。WY
						|| (ESCFlag->gridCur_rms < LoadCurlimit)) //电网电流有效值 < 负载电流限定值，表明：存在故障。WY
				{
					if (++ESCFlag->EscStandyDelayCnt1 > ESC_STANDY_DELAY_TIME_POINT) //持续100个状态机周期？具体时长是多少？WY
					{
						ESCFlag->EscStandyDelayCnt1 = ESC_STANDY_DELAY_TIME_POINT;

						/*处理A相*/
						if (ESCFlag->PHASE == 1)
						{
							ESCFlag->PWM_ins_index = 1; //A相主路直通。WY
						}

						/*处理B相*/
						if (ESCFlag->PHASE == 2)
						{
							ESCFlag->PWM_ins_index = 1;
						}

						/*处理C相*/
						if (ESCFlag->PHASE == 3)
						{
							ESCFlag->PWM_ins_index = 1;
						}
					}
				}
				else /*需要升压，且不存在故障。WY*/
				{
					ESCFlag->EscStandyDelayCnt1 = 0;
					ESCFlag->ESCCntSec.R_SSwiTiDelay = 0;
				}

				if ((ESCFlag->VoltIn_rms < ESCFlag->StableValue)  //电网电压有效值 < 稳定值，表明：需要保持升压状态。WY
						&& (ESCFlag->gridCur_rms >= LoadCurlimit)) //电网电流有效值 > 负载电流限定值，表明：不存在故障。WY
				{
					if (++ESCFlag->EscStandyDelayCnt2 > ESC_STANDY_DELAY_TIME_POINT) //持续100个状态机周期？具体时长是多少？。WY
					{
						ESCFlag->EscStandyDelayCnt2 = ESC_STANDY_DELAY_TIME_POINT; //强制赋值。防止变量在递增时发生上溢。WY

						/*处理A相*/
						if (ESCFlag->PHASE == 1)
						{
							ESCFlag->PWM_ins_index = 0; //主路调制。WY
						}

						/*处理B相*/
						if (ESCFlag->PHASE == 2)
						{
							ESCFlag->PWM_ins_index = 0;
						}

						/*处理C相*/
						if (ESCFlag->PHASE == 3)
						{
							ESCFlag->PWM_ins_index = 0;
						}
					}
				}
				else /*无需保持升压状态。WY*/
				{
					ESCFlag->EscStandyDelayCnt2 = 0;
				}
			}
				break;

			case 1: //降压
				if ((ESCFlag->VoltIn_rms < ESCFlag->StableValue) || (ESCFlag->gridCur_rms < LoadCurlimit))
				{
					if (++ESCFlag->EscStandyDelayCnt1 > ESC_STANDY_DELAY_TIME_POINT)
					{
						ESCFlag->EscStandyDelayCnt1 = ESC_STANDY_DELAY_TIME_POINT;
						if (ESCFlag->PHASE == 1)
							ESCFlag->PWM_ins_index = 1;
						if (ESCFlag->PHASE == 2)
							ESCFlag->PWM_ins_index = 1;
						if (ESCFlag->PHASE == 3)
							ESCFlag->PWM_ins_index = 1;
					}
				}
				else
				{
					ESCFlag->EscStandyDelayCnt1 = 0;
					ESCFlag->ESCCntSec.R_SSwiTiDelay = 0;
				}
				if ((ESCFlag->VoltIn_rms > ESCFlag->StableValue) && (ESCFlag->gridCur_rms >= LoadCurlimit))
				{
					if (++ESCFlag->EscStandyDelayCnt2 > ESC_STANDY_DELAY_TIME_POINT)
					{
						ESCFlag->EscStandyDelayCnt2 = ESC_STANDY_DELAY_TIME_POINT;
						if (ESCFlag->PHASE == 1)
							ESCFlag->PWM_ins_index = 0;
						if (ESCFlag->PHASE == 2)
							ESCFlag->PWM_ins_index = 0;
						if (ESCFlag->PHASE == 3)
							ESCFlag->PWM_ins_index = 0;
					}
				}
				else
				{
					ESCFlag->EscStandyDelayCnt2 = 0;
				}
				break;

			case 2:
				break;

			case 3://不平衡电流补偿
				if ((ESCFlag->VoltIn_rms < ESCFlag->StableValue) && (ESCFlag->gridCur_rms >= LoadCurlimit))
				{//输入电压低于门限值,且负载电流大于设定值+2,置位boost状态
					if (++ESCFlag->EscStandyDelayCnt2 > ESC_STANDY_DELAY_TIME_POINT)
					{
						ESCFlag->EscStandyDelayCnt2 = ESC_STANDY_DELAY_TIME_POINT;
						if (ESCFlag->PHASE == 1)
							ESCFlag->PWM_ins_index = 0;
						if (ESCFlag->PHASE == 2)
							ESCFlag->PWM_ins_index = 0;
						if (ESCFlag->PHASE == 3)
							ESCFlag->PWM_ins_index = 0;
					}
				}
				else
				{
					ESCFlag->EscStandyDelayCnt2 = 0;
				}
				break;
		}
	}
	else
	{
		ESCFlag->PWM_ins_index = 1;//锁相未完成,置为IGBT直通状态
	}
}

/*
 * 功能：执行【运行转待机】状态。WY
 * 输入参数：待执行的X相参数。X的取值集合为{A, B, C}。
 * 说明：当设备工作在升压模式时，若当前无需升压，则进入运行转待机状态。
 */
void ESCRUNTURNWAIT(ESCCTRLVALFLAG *ESCFlag)
{
	switch (StateFlag.VoltageMode)
	{
		case 0: //升压
		case 4:
		{
			/*无需升压。WY*/
			if ((ESCFlag->VoltIn_rms > ESCFlag->StableValue) //电网电压有效值 > 稳定值。WY
					&& (ESCFlag->PWM_ins_index == 1)) //主路直通。WY
			{
				if (ESCFlag->ESCCntSec.R_SSwiTiDelay >= CNT_SEC(R_SSwitchTime)) //上述状态持续时长 > 设定值。WY
				{
					/*处理A相。WY*/
					if (ESCFlag->PHASE == 1)
					{
						SET_SCRA_ENABLE(ESC_ACTION_ENABLE); //开通A相旁路晶闸管。WY
						SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //闭合A相旁路磁保持继电器。WY
						SET_IGBT_ENA(IGBT_DISABLE); //软件失能A相PWM。WY
						DisablePWMA(); //硬件封锁A相PWM。WY
						ESCFlag->StateSkipFlag = 1; //置位跳转标志位。该标志位将在【待机状态】中被使用。WY
						StateEventFlag_A = STATE_EVENT_WAIT_A; //切换状态机至：A相待机状态。WY
						ESCFlag->ESCCntSec.HWPowerSupDelay = 0;
					}

					/*处理B相。WY*/
					if (ESCFlag->PHASE == 2)
					{
						SET_SCRB_ENABLE(ESC_ACTION_ENABLE);
						SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE);
						SET_IGBT_ENB(IGBT_DISABLE);
						DisablePWMB();
						ESCFlag->StateSkipFlag = 1;
						StateEventFlag_B = STATE_EVENT_WAIT_B;
						ESCFlag->ESCCntSec.HWPowerSupDelay = 0;
					}

					/*处理C相。WY*/
					if (ESCFlag->PHASE == 3)
					{
						SET_SCRC_ENABLE(ESC_ACTION_ENABLE);
						SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE);
						SET_IGBT_ENC(IGBT_DISABLE);
						DisablePWMC();
						ESCFlag->StateSkipFlag = 1;
						StateEventFlag_C = STATE_EVENT_WAIT_C;
						ESCFlag->ESCCntSec.HWPowerSupDelay = 0;
					}

					ESCFlag->onceTimeStateMachine = 1; //置位单次状态检测。WY
					ESCFlag->ESCCntSec.R_SSwiTiDelay = 0;
				}
			}
			else
			{
				ESCFlag->ESCCntSec.R_SSwiTiDelay = 0;
				ESCFlagA.ESCCntSec.HWPowerSupDelay = 0;
				ESCFlagB.ESCCntSec.HWPowerSupDelay = 0;
				ESCFlagC.ESCCntSec.HWPowerSupDelay = 0;
			}
		}
			break;

		case 1: //降压
			if ((ESCFlag->VoltIn_rms < ESCFlag->StableValue) && (ESCFlag->PWM_ins_index == 1))//单相
			{
				if (ESCFlag->ESCCntSec.R_SSwiTiDelay >= CNT_SEC(R_SSwitchTime))
				{//该时间需要设置长一点,防止第一版电操频繁投切.
					if (ESCFlag->PHASE == 1)
					{
						SET_SCRA_ENABLE(ESC_ACTION_ENABLE);//旁路状态
						SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE);//切旁路
						SET_IGBT_ENA(IGBT_DISABLE);
						DisablePWMA();
						ESCFlag->StateSkipFlag = 1;
						StateEventFlag_A = STATE_EVENT_WAIT_A;
						ESCFlag->ESCCntSec.HWPowerSupDelay = 0;

					}
					if (ESCFlag->PHASE == 2)
					{
						SET_SCRB_ENABLE(ESC_ACTION_ENABLE);//旁路状态
						SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE);//切旁路
						SET_IGBT_ENB(IGBT_DISABLE);
						DisablePWMB();
						ESCFlag->StateSkipFlag = 1;
						StateEventFlag_B = STATE_EVENT_WAIT_B;
						ESCFlag->ESCCntSec.HWPowerSupDelay = 0;

					}
					if (ESCFlag->PHASE == 3)
					{
						SET_SCRC_ENABLE(ESC_ACTION_ENABLE);//旁路状态
						SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE);//切旁路
						SET_IGBT_ENC(IGBT_DISABLE);
						DisablePWMC();
						ESCFlag->StateSkipFlag = 1;
						StateEventFlag_C = STATE_EVENT_WAIT_C;
						ESCFlag->ESCCntSec.HWPowerSupDelay = 0;

					}
					ESCFlag->onceTimeStateMachine = 1;//切旁路产生一次状态检测窗口
					ESCFlag->ESCCntSec.R_SSwiTiDelay = 0;
				}
			}
			else
			{
				ESCFlag->ESCCntSec.R_SSwiTiDelay = 0;
				ESCFlagA.ESCCntSec.HWPowerSupDelay = 0;
				ESCFlagB.ESCCntSec.HWPowerSupDelay = 0;
				ESCFlagC.ESCCntSec.HWPowerSupDelay = 0;
			}
			break;

		case 2:
			break;
		case 3:
			break;//不平衡方式不转待机
	}
	ESCFlag->FunContDelayFlag = 0; //无意义。WY
	ESCFlag->ESCCntSec.FunContDelay = 0; //无意义。WY
}

/*
 * 功能：执行【待机】状态。WY 函数功能与函数名不符。
 * 输入参数ESCFlag：待执行的X相参数。X的取值集合为{A, B, C}。
 * 说明：当设备工作在待机状态时，若满足升压条件，则进入升压模式。
 */
void ESCWAITTURNRUN(ESCCTRLVALFLAG *ESCFlag)
{
	switch (StateFlag.VoltageModeFlag)
	{
		case 0:	//升压模式。WY
		case 3:
		case 4:
			if ((ESCFlag->VoltIn_rms < ESCFlag->VoltThreShold) //电网电压有效值 < 电压门限值。WY
					&& (ESCFlag->gridCurrBY_rms < RateLoadCurrLimit)) //旁路电流有效值 < 负载额定电流限定值。WY
			{
				SET_POWER_CTRL(1); //15V电源上电。WY
				Delayus(TIME_WRITE_15VOLT_REDAY); //延时，使得15V电源供电稳定。WY

				ESCFlagA.ESCCntSec.HWPowerSupDelay = 0; //无需关闭15V电源。WY
				ESCFlagB.ESCCntSec.HWPowerSupDelay = 0; //无需关闭15V电源。WY
				ESCFlagC.ESCCntSec.HWPowerSupDelay = 0; //无需关闭15V电源。WY

				if (ESCFlag->ESCCntSec.S_RSwiTiDelay >= CNT_SEC(S_RSwitchTime)) //电网电压欠压时长超过设定值。WY
				{//任意一相的欠压持续5000个点//约4S
					/*处理A相。WY*/
					if (ESCFlag->PHASE == 1)
					{
						ConstantCurr[0].state = SM_CONSTANT_CURR_STANDBY;
						ESCFlag->ESC_DutyData = 1.0; //占空比为100%。WY
						ESCFlag->PWM_ins_index = 1; //主路直通。WY
						Delayus(TIME_WRITE_15VOLT_REDAY); //延时2ms。WY
						EnablePWMA(); //软件解锁A相PWM。WY
						PIVolA.i10 = 0;//清理饱和
						PIVolA.i6 = 0;
						Delayus(100); //延时。为PWM持续输出高电平留有时间。WY
						SET_IGBT_ENA(IGBT_ENABLE); //硬件使能A相PWM。WY
						SET_SCRA_ENABLE(ESC_ACTION_ENABLE); //开通A相旁路晶闸管。注意：在断开磁保持继电器之前，必须开通晶闸管，防止磁保持继电器拉弧。WY
						SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_DISABLE); //断开A相旁路磁保持继电器。WY
						StateEventFlag_A = STATE_EVENT_RUN_A; //切换状态机至：A相正常运行.WY
						CurrentUnbalanceRegularVoltage[0] = ESCFlagA.VoltThreShold; //最小值  //？。WY
					}

					/*处理B相。WY*/
					if (ESCFlag->PHASE == 2)
					{
						ConstantCurr[1].state = SM_CONSTANT_CURR_STANDBY;
						ESCFlag->ESC_DutyData = 1.0;//占空比设为0.93
						ESCFlag->PWM_ins_index = 1;//IGBT直通态
						Delayus(TIME_WRITE_15VOLT_REDAY);
						EnablePWMB();
						PIVolB.i10 = 0;//清理饱和
						PIVolB.i6 = 0;
						Delayus(100);//需要加个延时时间,否则PWM的寄存器会来不及动作
						SET_IGBT_ENB(IGBT_ENABLE);//开IGBT使能
						SET_SCRB_ENABLE(ESC_ACTION_ENABLE);//开晶闸管
						SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_DISABLE);//关旁路
						StateEventFlag_B = STATE_EVENT_RUN_B;
						CurrentUnbalanceRegularVoltage[1] = ESCFlagB.VoltThreShold;
					}

					/*处理C相。WY*/
					if (ESCFlag->PHASE == 3)
					{
						ConstantCurr[2].state = SM_CONSTANT_CURR_STANDBY;
						ESCFlag->ESC_DutyData = 1.0;//占空比设为0.93
						ESCFlag->PWM_ins_index = 1;//IGBT直通态
						Delayus(TIME_WRITE_15VOLT_REDAY);
						EnablePWMC();
						PIVolC.i10 = 0;//清理饱和
						PIVolC.i6 = 0;
						Delayus(100);//需要加个延时时间,否则PWM的寄存器会来不及动作
						SET_IGBT_ENC(IGBT_ENABLE);//开IGBT使能
						SET_SCRC_ENABLE(ESC_ACTION_ENABLE);//开晶闸管
						SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_DISABLE);//关旁路
						StateEventFlag_C = STATE_EVENT_RUN_C;
						CurrentUnbalanceRegularVoltage[2] = ESCFlagC.VoltThreShold;
					}

					ESCFlag->onceTimeStateMachine = 1; //置位单次状态检测标志位。WY
					ESCFlag->ESCCntMs.ContactorDelay = 0;
					ESCFlag->ESCCntSec.S_RSwiTiDelay = 0; //清零电网电压欠压时长。WY
					ESCFlag->ESCCntSec.R_SSwiTiDelay = 0;

					CntMs.UNCurrDelay1 = 0;
					CntMs.UNCurrDelay2 = 0;
				}
			}
			else //无需升压。WY
			{
				if ((ESCFlagA.ESCCntSec.HWPowerSupDelay >= CNT_SEC(TIME_STANDBY_DELAY))  //A相所需的15V电源开启时长 > 600s。WY
						&& (ESCFlagB.ESCCntSec.HWPowerSupDelay >= CNT_SEC(TIME_STANDBY_DELAY)) //B相所需的15V电源开启时长 > 600s。WY
						&& (ESCFlagC.ESCCntSec.HWPowerSupDelay >= CNT_SEC(TIME_STANDBY_DELAY))) //C相所需的15V电源开启时长 > 600s。WY
				{
					if ((StateEventFlag_A == STATE_EVENT_WAIT_A) //A相处于待机状态
							&& (StateEventFlag_B == STATE_EVENT_WAIT_B) //B相处于待机状态
							&& (StateEventFlag_C == STATE_EVENT_WAIT_C)) //C相处于待机状态
					{
						if ((ESCBYRelayCNTA != 1)  //A相旁路磁保持继电器状态正常。WY
								&& (ESCBYRelayCNTB != 1)  //B相旁路磁保持继电器状态正常。WY
								&& (ESCBYRelayCNTC != 1)) //C相旁路磁保持继电器状态正常。WY
						{
							SET_POWER_CTRL(0); //15V电源断电。WY
						}
					}
				}

				if (ESCFlag->PHASE == 1) //处理A相。WY
				{
					ESCFlagA.ESCCntSec.S_RSwiTiDelay = 0; //清零电网欠压时长（单位：s）。WY
				}

				if (ESCFlag->PHASE == 2) //处理B相。WY
				{
					ESCFlagB.ESCCntSec.S_RSwiTiDelay = 0;
				}

				if (ESCFlag->PHASE == 3) //处理C相。WY
				{
					ESCFlagC.ESCCntSec.S_RSwiTiDelay = 0;
				}
			}
			break;

		/*降压模式*/
		case 1:
			if ((ESCFlag->VoltIn_rms > ESCFlag->VoltThreShold) && (ESCFlag->gridCurrBY_rms < RateLoadCurrLimit))//单相ESC
			{
				SET_POWER_CTRL(1);
				Delayus(TIME_WRITE_15VOLT_REDAY);
				ESCFlagA.ESCCntSec.HWPowerSupDelay = 0;
				ESCFlagB.ESCCntSec.HWPowerSupDelay = 0;
				ESCFlagC.ESCCntSec.HWPowerSupDelay = 0;
				ConstantCurrInsFlagA = 0;
				VolttargetCorrA = 0;
				if (ESCFlag->ESCCntSec.S_RSwiTiDelay >= CNT_SEC(S_RSwitchTime))
				{
					if (ESCFlag->PHASE == 1)
					{
						ESCFlag->ESC_DutyData = 1.0;//占空比设为0.93
						ESCFlag->PWM_ins_index = 1;//IGBT直通态
						Delayus(TIME_WRITE_15VOLT_REDAY);
						EnablePWMA();
						PIVolA.i10 = 0;//清理饱和
						PIVolA.i6 = 0;
						Delayus(100);//需要加个延时时间,否则PWM的寄存器会来不及动作
						SET_IGBT_ENA(IGBT_ENABLE);//开IGBT使能
						SET_SCRA_ENABLE(ESC_ACTION_ENABLE);//开晶闸管
						SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_DISABLE);//关旁路
						StateEventFlag_A = STATE_EVENT_RUN_A;
					}
					if (ESCFlag->PHASE == 2)
					{
						ESCFlag->ESC_DutyData = 1.0;//占空比设为0.93
						ESCFlag->PWM_ins_index = 1;//IGBT直通态
						Delayus(TIME_WRITE_15VOLT_REDAY);
						EnablePWMB();
						PIVolB.i10 = 0;//清理饱和
						PIVolB.i6 = 0;
						Delayus(100);//需要加个延时时间,否则PWM的寄存器会来不及动作
						SET_IGBT_ENB(IGBT_ENABLE);//开IGBT使能
						SET_SCRB_ENABLE(ESC_ACTION_ENABLE);//开晶闸管
						SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_DISABLE);//关旁路
						StateEventFlag_B = STATE_EVENT_RUN_B;
					}
					if (ESCFlag->PHASE == 3)
					{
						ESCFlag->ESC_DutyData = 1.0;//占空比设为0.93
						ESCFlag->PWM_ins_index = 1;//IGBT直通态
						Delayus(TIME_WRITE_15VOLT_REDAY);
						EnablePWMC();
						PIVolC.i10 = 0;//清理饱和
						PIVolC.i6 = 0;
						Delayus(100);//需要加个延时时间,否则PWM的寄存器会来不及动作
						SET_IGBT_ENC(IGBT_ENABLE);//开IGBT使能
						SET_SCRC_ENABLE(ESC_ACTION_ENABLE);//开晶闸管
						SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_DISABLE);//关旁路
						StateEventFlag_C = STATE_EVENT_RUN_C;
					}
					ESCFlag->onceTimeStateMachine = 1;//状态检测窗口
					ESCFlag->ESCCntMs.ContactorDelay = 0;//状态检测窗口计数器清零
					ESCFlag->ESCCntSec.S_RSwiTiDelay = 0;
					ESCFlag->ESCCntSec.R_SSwiTiDelay = 0;
				}
			}
			else
			{
				if ((ESCFlagA.ESCCntSec.HWPowerSupDelay >= CNT_SEC(TIME_STANDBY_DELAY)) && (ESCFlagB.ESCCntSec.HWPowerSupDelay >= CNT_SEC(TIME_STANDBY_DELAY))
						&& (ESCFlagC.ESCCntSec.HWPowerSupDelay >= CNT_SEC(TIME_STANDBY_DELAY)))
				{
					if ((StateEventFlag_A == STATE_EVENT_WAIT_A) && (StateEventFlag_B == STATE_EVENT_WAIT_B) && (StateEventFlag_C == STATE_EVENT_WAIT_C))
					{
						if ((ESCBYRelayCNTA != 1) && (ESCBYRelayCNTB != 1) && (ESCBYRelayCNTC != 1))
						{
							SET_POWER_CTRL(0);//必须三相都转入待机状态,再切掉15V电源
						}
					}
				}
				if (ESCFlag->PHASE == 1)
					ESCFlagA.ESCCntSec.S_RSwiTiDelay = 0;
				if (ESCFlag->PHASE == 2)
					ESCFlagB.ESCCntSec.S_RSwiTiDelay = 0;
				if (ESCFlag->PHASE == 3)
					ESCFlagC.ESCCntSec.S_RSwiTiDelay = 0;
			}
			break;
		case 2:
			break;

	}

	ESCFlag->PWM_ins_index = 1; //主路直通。WY

	if ((ESCFlag->ByPassContactFlag == 1) //旁路磁保持继电器闭合。WY
			&& (ESCFlag->StateSkipFlag == 1)) //状态跳转标志位。该标志位在【运行转待机】状态中被置位。WY
	{
		if (ESCFlag->PHASE == 1) //处理A相。
		{
			SET_SCRA_ENABLE(ESC_ACTION_DISABLE); //关断A相旁路晶闸管。WY
		}

		if (ESCFlag->PHASE == 2) //处理B相。
		{
			SET_SCRB_ENABLE(ESC_ACTION_DISABLE);
		}

		if (ESCFlag->PHASE == 3) //处理C相。
		{
			SET_SCRC_ENABLE(ESC_ACTION_DISABLE);
		}

		ESCFlag->StateSkipFlag = 0;
	}
}

/*
 * 功能：执行【主动停机】状态。WY
 * 输入参数ESCFlag：待执行的X相参数。X的取值集合为{A, B, C}。
 */
void ESCSTOPSTATE(ESCCTRLVALFLAG *ESCFlag)
{
	/*处理A相。WY*/
	if (ESCFlag->PHASE == 1)
	{
		SET_SCRA_ENABLE(ESC_ACTION_ENABLE); //开通A相旁路晶闸管。WY
		SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //闭合A相旁路磁保持继电器。WY

		SET_IGBT_ENA(IGBT_DISABLE); //硬件失能A相PWM。WY
		DisablePWMA(); //软件封锁A相PWM。WY

	}

	/*处理B相。WY*/
	if (ESCFlag->PHASE == 2)
	{
		SET_SCRB_ENABLE(ESC_ACTION_ENABLE);
		SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE);

		SET_IGBT_ENB(IGBT_DISABLE);
		DisablePWMB();

	}

	/*处理C相。WY*/
	if (ESCFlag->PHASE == 3)
	{
		SET_SCRC_ENABLE(ESC_ACTION_ENABLE);
		SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE);

		SET_IGBT_ENC(IGBT_DISABLE);
		DisablePWMC();

	}

	ESCFlag->onceTimeStateMachine = 1; //置位单次检测标志位。WY
	ESCFlag->ESCCntMs.ContactorDelay = 0;//为执行【待机】状态做好准备。该变量在【待机状态】中被使用WY
}

/*
 * 功能：执行【故障停机】状态。WY
 * 输入参数ESCFlag：待执行的X相参数。X的取值集合为{A, B, C}。
 */
void FAULTSTATE(ESCCTRLVALFLAG *ESCFlag)
{
	if ((ESCFlag->ByPassContactFlag == 1) //旁路磁保持继电器闭合。WY
			&& (ESCFlag->FaultskipFlag == 1))
	{
		if (ESCFlag->PHASE == 1) //处理A相。WY
		{
			SET_SCRA_ENABLE(ESC_ACTION_DISABLE); //关断A相旁路晶闸管。WY
		}

		if (ESCFlag->PHASE == 2) //处理B相。WY
		{
			SET_SCRB_ENABLE(ESC_ACTION_DISABLE);
		}

		if (ESCFlag->PHASE == 3) //处理C相。WY
		{
			SET_SCRC_ENABLE(ESC_ACTION_DISABLE);
		}

		ESCFlag->FaultskipFlag = 0;
	}
}


void FLAULTJUDGE(ESCCTRLVALFLAG *ESCFlag)         // 故障字判断完成后，入口
{
    if(ESCFlag->faultFlag == 1){
        #if !TEST_RUNTIME
        if(ESCFlag->PHASE == 1)        StateEventFlag_A = STATE_EVENT_FAULT_A;
        if(ESCFlag->PHASE == 2)        StateEventFlag_B = STATE_EVENT_FAULT_B;
        if(ESCFlag->PHASE == 3)        StateEventFlag_C = STATE_EVENT_FAULT_C;
        if(StateEventFlag_A == STATE_EVENT_FAULT_A){
            ESCFlagA.stopFlag = 1;
            ESCFlagA.startFlag = 0;
        }
        if(StateEventFlag_B == STATE_EVENT_FAULT_B){
            ESCFlagB.stopFlag = 1;
            ESCFlagB.startFlag = 0;
        }
        if(StateEventFlag_C == STATE_EVENT_FAULT_C){
            ESCFlagC.stopFlag = 1;
            ESCFlagC.startFlag = 0;
        }
        #endif
    }
}

/*
 * 功能：控制继电器。WY
 * 输入参数ESCFlag：待执行的X相参数。X的取值集合为{A, B, C}。
 * 说明：此函数仅在上电后2s时执行1次。
 */
void RELAYCTRL(ESCCTRLVALFLAG *ESCFlag)
{
	if ((ESCFlag->ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))  //当超时发生时，表明:此时需要检测【旁路磁保持继电器】状态。WY
			&& (ESCFlag->ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))) //当超时发生时，表明此时需要检测【主路磁保持继电器】状态。WY
	{
		if (ESCFlag->RELAYCONTROLFlag == 1) //需要操作继电器。WY
		{
			if ((ESCFlag->ByPassContactFlag == 1) //旁路磁保持继电器闭合。WY
					&& (ESCFlag->ContactorFeedBackFlag == 0)) //主路磁保持继电器断开。WY
			{
				if (ESCFlag->PHASE == 1) //处理A相。WY
				{
					SET_GV_VOL_CTRL_A(1); //闭合A相小继电器。WY
				}

				if (ESCFlag->PHASE == 2)
				{
					SET_GV_VOL_CTRL_B(1);
				}

				if (ESCFlag->PHASE == 3)
				{
					SET_GV_VOL_CTRL_C(1);
				}

				ESCFlag->RELAYCONTROLFlag = 0; //小继电器操作已完成。WY
			}
		}
	}
}

int UnitSUCCESSCNT = 0; //未使用。WY
int UnitDISSUCCESSCNT = 0; //未使用。WY
int DelayFlagA = 0,DelayFlagB = 0,DelayFlagC = 0; //未使用。WY

/*
 * 功能：状态机。WY
 */
void SetStartCtrl(void)
{
	UInt events;

	while (1)
	{
		events = Event_pend(Event_SetStartSM, Event_Id_NONE, Event_Id_01, BIOS_WAIT_FOREVER);

		if (STOP_INSTRUCTION == 0)//停机按钮被按下。WY
		{
			/*启动设备。WY*/
			if (CntMs.inputStop >= CNT_MS(6000))//启动设备的条件：停机按钮被按下的时长超过6s。WY
			{
				if ((!ESCFlagA.startFlag)//A相无需执行启动操作。WY
				&& (!ESCFlagB.startFlag)//A相无需执行启动操作。WY
						&& (!ESCFlagC.startFlag))//A相无需执行启动操作。WY
				{
					SoeRecData(SOE_GP_EVENT);//生成日志。WY
				}

				ESCFlagA.stopFlag = 0;//A相无需执行停机操作。WY
				ESCFlagB.stopFlag = 0;//B相无需执行停机操作。WY
				ESCFlagC.stopFlag = 0;//C相无需执行停机操作。WY

				ESCFlagA.startFlag = 1;//A相的启动操作等待执行。WY
				ESCFlagB.startFlag = 1;//B相的启动操作等待执行。WY
				ESCFlagC.startFlag = 1;//C相的启动操作等待执行。WY

				ESCFlagA.onceRunStartFlag = 0;
				ESCFlagB.onceRunStartFlag = 0;
				ESCFlagC.onceRunStartFlag = 0;

				ESCFlagA.autoStFlag = ORIGINAL_STATE_A;//设置A相自动重启的次数。WY
				ESCFlagB.autoStFlag = ORIGINAL_STATE_B;//设置B相自动重启的次数。WY
				ESCFlagC.autoStFlag = ORIGINAL_STATE_C;//设置C相自动重启的次数。WY

				SET_POWER_CTRL(1);//15V电源上电。WY
				Delayus(TIME_WRITE_15VOLT_REDAY);
				SET_RUNNING_LED(0);//设备运行指示灯灭。

				if ((StateEventFlag_A != STATE_EVENT_FAULT_A)//A相未处于故障停机状态。WY
				&& (StateEventFlag_B != STATE_EVENT_FAULT_B)//B相未处于故障停机状态。WY
						&& (StateEventFlag_C != STATE_EVENT_FAULT_C))//C相未处于故障停机状态。WY
				{
					SET_FAULT_LED(0);//设备故障指示灯灭。WY
				}
			}
		}
		else//停机按钮未被按下。WY
		{
			/*停止设备的条件：停机按钮被按下的时长在(0.36s, 2s)之间 。WY*/
			if ((CntMs.inputStop >= CNT_MS(360)) && (CntMs.inputStop <= CNT_MS(2000)))
			{
				if ((!ESCFlagA.stopFlag)//A相无需执行停机操作。WY
				&& (!ESCFlagB.stopFlag)//B相无需执行停机操作。WY
						&& (!ESCFlagC.stopFlag))//C相无需执行停机操作。WY
				{
					SoeRecData(SOE_GP_EVENT + 1);//生成日志。WY
				}

				ESCFlagA.stopFlag = 1;//A相等待执行停机操作。WY
				ESCFlagB.stopFlag = 1;//B相等待执行停机操作。WY
				ESCFlagC.stopFlag = 1;//C相等待执行停机操作。WY

				ESCFlagA.startFlag = 0;//A相无需执行启动操作。WY
				ESCFlagB.startFlag = 0;//B相无需执行启动操作。WY
				ESCFlagC.startFlag = 0;//C相无需执行启动操作。WY

				if (ESCBYRelayCNTA != 1)//A相旁路磁保持继电器正常。WY
				{
					ESCFlagA.HWPowerSTOPFlag = 1;
					ESCFlagA.ESCCntSec.HWPowerStopDelay = 0;
				}

				if (ESCBYRelayCNTB != 1)//B相旁路磁保持继电器正常。WY
				{
					ESCFlagB.HWPowerSTOPFlag = 1;
					ESCFlagB.ESCCntSec.HWPowerStopDelay = 0;
				}

				if (ESCBYRelayCNTC != 1)//C相旁路磁保持继电器正常。WY
				{
					ESCFlagC.HWPowerSTOPFlag = 1;
					ESCFlagC.ESCCntSec.HWPowerStopDelay = 0;
				}

				ESCFlagA.onceRunStartFlag = 1;
				ESCFlagB.onceRunStartFlag = 1;
				ESCFlagC.onceRunStartFlag = 1;

				ESCFlagA.autoStFlag = ORIGINAL_STATE_A;//设置A相自动重启的次数。WY
				ESCFlagB.autoStFlag = ORIGINAL_STATE_B;//设置B相自动重启的次数。WY
				ESCFlagC.autoStFlag = ORIGINAL_STATE_C;//设置C相自动重启的次数。WY
			}

			/*在停机按钮未被按下时，该计时器将被一直清零。WY*/
			CntMs.inputStop = 0;//清零停机按钮被按下的时长。WY
		}

		FLAULTJUDGE(&ESCFlagA);//判断A相故障。该函数存疑？WY
		FLAULTJUDGE(&ESCFlagB);//判断B相故障。该函数存疑？WY
		FLAULTJUDGE(&ESCFlagC);//判断C相故障。该函数存疑？WY

		RELAYCTRL(&ESCFlagA);//操作A相小继电器。WY
		RELAYCTRL(&ESCFlagB);//操作B相小继电器。WY
		RELAYCTRL(&ESCFlagC);//操作C相小继电器。WY

		/*A相状态机。WY*/
		switch (StateEventFlag_A)
		{
			/*A相初始状态（正常停机）。WY*/
			case STATE_EVENT_STANDBY_A:
			{
				ESCSTANDBYEVENT(&ESCFlagA); //执行初始状态(正常停机)。WY
				ESCFlagA.stopFlag = 0; //A相无需执行停机操作。WY

				if (ESCFlagA.startFlag == 1) //A相需要执行启动操作。WY
				{
					SoeRecData(SOE_GP_EVENT); //生成日志。。WY
					ESCFlagA.stopFlag = 0; //A相无需执行停机操作。WY

					if ((GET_GV_VOL_CTRL_A == 1)  //A相小继电器闭合。WY
							&& (GET_BYPASS_FEEDBACK == 1)  //A相旁路磁保持继电器状态正常。WY
							&& (ESCBYRelayCNTA != 1) //微型断路器闭合。WY
							&& (ESCSicFaultCNTA != 1)) //A相SiC管状态正常。WY
					{
						StateEventFlag_A = STATE_EVENT_RECHARGE_A; //切换状态机至：A相预充电。WY
						ESCFlagA.BYFEEDBACKFLAG = 1; //A相旁路状态正常。WY
					}

					ESCFlagA.ESCCntSec.ChargingTime = 0; //清零预充电时间。该变量将在预充电状态中被使用。WY
				}
			}
				break;

			/*A相预充电。WY*/
			case STATE_EVENT_RECHARGE_A:
			{
				SET_RUNNING_LED(1); //设备运行指示灯亮。WY
				SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //闭合A相旁路磁保持继电器。WY
				SET_IGBT_ENA(IGBT_DISABLE); //硬件失能A相PWM。WY
				DisablePWMA(); //封锁A相PWM。WY
				ESCPRECHARGEEVENT(&ESCFlagA); //执行A相预充电。WY

				/*紧急停机。WY*/
				if (ESCFlagA.stopFlag == 1)
				{
					ESCFlagA.stopFlag = 0; //A相无需执行停机操作。WY
					ESCFlagA.startFlag = FALSE; //A相无锡执行启动操作。WY
					ESCFlagA.PeakStopFlag = 1;
					StateEventFlag_A = STATE_EVENT_STOP_A; //切换状态机至：A相主动停机状态。WY
					SoeRecData(SOE_GP_EVENT + 1); //生成日志。WY
				}
			}
				break;

			/*A相运行*/
			case STATE_EVENT_RUN_A:
			{
				if (ESCFlagA.onceTimeStateMachine == 1) //单次状态检测。WY
				{
					ESCFlagA.ESCCntMs.PllDelay = 0; //清零锁相环锁相分配的时长。WY

					if (ESCFlagA.ByPassContactFlag == 0) //A相旁路磁保持继电器断开。WY
					{
						SET_SCRA_ENABLE(ESC_ACTION_DISABLE); //关断A相旁路晶闸管。WY
						ESCFlagA.onceTimeStateMachine = 0; //单次状态检测已完成。WY
					}
				}
				ESCRUNEVENT(&ESCFlagA);//A相运行。WY

				ESCRUNTURNWAIT(&ESCFlagA); //A相运行转待机。WY
				SET_RUNNING_LED(1); //设备运行指示灯亮。WY

				/*紧急停机。WY*/
				if (ESCFlagA.stopFlag == 1)
				{
					ESCFlagA.stopFlag = 0;
					ESCFlagA.startFlag = 0;

					ESCFlagA.PeakStopFlag = 1;
					SET_SCRA_ENABLE(ESC_ACTION_ENABLE);
					StateEventFlag_A = STATE_EVENT_STOP_A;
				}
			}
				break;

			/*A相主动停机状态。WY*/
			case STATE_EVENT_STOP_A:
			{
				ESCSTOPSTATE(&ESCFlagA); //执行A相主动停机状态。WY

				if (GET_BYPASS_FEEDBACK != 0)
				{
					SET_RUNNING_LED(0); //设备指示灯灭。WY
				}
#if !TEST_RUNPORCESS
				ExecuteOpenContact_A(); //断开A相主路磁保持继电器。WY
#endif
				StateEventFlag_A = STATE_EVENT_STANDBY_A; //切换状态机至：A相初始状态（正常停机）。WY

				SoeRecData(SOE_GP_EVENT + 1); //生成日志。WY

				ESCFlagA.ESCCntSec.HWPowerStopDelay = 0;
				ESCFlagA.ESCCntSec.HWPowerFaultDelay = 0;
			}
				break;

			/*A相待机状态。WY*/
			case STATE_EVENT_WAIT_A:
			{
				ESCWAITTURNRUN(&ESCFlagA); //执行A相待机操作。WY
				SET_RUNNING_LED(1); //设备运行指示灯亮。WY

				/*急停操作。WY*/
				if (ESCFlagA.stopFlag == 1)
				{
					SET_POWER_CTRL(1);
					Delayus(TIME_WRITE_15VOLT_REDAY);
					ESCFlagA.stopFlag = 0;
					ESCFlagA.startFlag = 0;
					ESCFlagA.PeakStopFlag = 1;
					StateEventFlag_A = STATE_EVENT_STOP_A;
					SoeRecData(SOE_GP_EVENT + 1);
				}
			}
				break;

			/*A相故障停机状态。WY*/
			case STATE_EVENT_FAULT_A:
			{
				ESCFlagA.startFlag = 0; //A相无需执行启动操作。WY
				ESCFlagA.stopFlag = 1; //A相等待执行停机操作。WY

				SET_RUNNING_LED(0); //设备运行指示灯灭。WY
				FAULTSTATE(&ESCFlagA); //执行A相故障停机状态。WY

				if (ESCBYRelayCNTA != 1) //A相旁路磁保持继电器状态正常。WY
				{
					ExecuteOpenContact_A();
				}

				SET_FAULT_LED(1);
			}
				break;
			default:
				break;
		}

		/*B相状态机。WY*/
		switch (StateEventFlag_B)
		{
			case STATE_EVENT_STANDBY_B:
				ESCSTANDBYEVENT(&ESCFlagB);
				ESCFlagB.stopFlag = 0;

				if (ESCFlagB.startFlag == 1)
				{
					SoeRecData(SOE_GP_EVENT);
					ESCFlagB.stopFlag = 0;
					if ((GET_GV_VOL_CTRL_B == 1) && (GET_BYPASS_FEEDBACK == 1) && (ESCBYRelayCNTB != 1) && (ESCSicFaultCNTB != 1))
					{
						StateEventFlag_B = STATE_EVENT_RECHARGE_B;
						ESCFlagB.BYFEEDBACKFLAG = 1;
					}
					ESCFlagB.ESCCntSec.ChargingTime = 0;
				}
				break;

			case STATE_EVENT_RECHARGE_B:
				SET_RUNNING_LED(1);
				SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE)
				;
				SET_IGBT_ENB(IGBT_DISABLE);
				DisablePWMB();
				ESCPRECHARGEEVENT(&ESCFlagB);
				if (ESCFlagB.stopFlag == 1)
				{
					ESCFlagB.stopFlag = 0;
					ESCFlagB.startFlag = FALSE;
					ESCFlagB.PeakStopFlag = 1;
					StateEventFlag_B = STATE_EVENT_STOP_B;
					SoeRecData(SOE_GP_EVENT + 1);
				}
				break;

			case STATE_EVENT_RUN_B:
				if (ESCFlagB.onceTimeStateMachine == 1)
				{
					ESCFlagB.ESCCntMs.PllDelay = 0;
					if (ESCFlagB.ByPassContactFlag == 0)
					{
						SET_SCRB_ENABLE(ESC_ACTION_DISABLE);
						ESCFlagB.onceTimeStateMachine = 0;
					}
				}
				ESCRUNEVENT(&ESCFlagB);
				ESCRUNTURNWAIT(&ESCFlagB);
				SET_RUNNING_LED(1);
				if (ESCFlagB.stopFlag == 1)
				{
					ESCFlagB.stopFlag = 0;
					ESCFlagB.startFlag = 0;
					ESCFlagB.PeakStopFlag = 1;
					SET_SCRB_ENABLE(ESC_ACTION_ENABLE);
					StateEventFlag_B = STATE_EVENT_STOP_B;
				}
				break;

			case STATE_EVENT_STOP_B:
				ESCSTOPSTATE(&ESCFlagB);
				if (GET_BYPASS_FEEDBACK != 0)
				{
					SET_RUNNING_LED(0);
				}
#if !TEST_RUNPORCESS
				ExecuteOpenContact_B();
#endif
				StateEventFlag_B = STATE_EVENT_STANDBY_B;
				SoeRecData(SOE_GP_EVENT + 1);
				ESCFlagB.ESCCntSec.HWPowerStopDelay = 0;
				ESCFlagB.ESCCntSec.HWPowerFaultDelay = 0;
				break;

			case STATE_EVENT_WAIT_B:
				ESCWAITTURNRUN(&ESCFlagB);
				SET_RUNNING_LED(1);
				if (ESCFlagB.stopFlag == 1)
				{
					SET_POWER_CTRL(1);
					Delayus(TIME_WRITE_15VOLT_REDAY);
					ESCFlagB.stopFlag = 0;
					ESCFlagB.startFlag = 0;
					ESCFlagB.PeakStopFlag = 1;
					StateEventFlag_B = STATE_EVENT_STOP_B;
					SoeRecData(SOE_GP_EVENT + 1);
				}
				break;

			case STATE_EVENT_FAULT_B:
				ESCFlagB.startFlag = 0;
				ESCFlagB.stopFlag = 1;
				SET_RUNNING_LED(0);
				FAULTSTATE(&ESCFlagB);
				if (ESCBYRelayCNTB != 1)
				{
					ExecuteOpenContact_B();
				}
				SET_FAULT_LED(1);
				break;
			default:
				break;
		}

		/*C相状态机。WY*/
		switch (StateEventFlag_C)
		{
			case STATE_EVENT_STANDBY_C:
				ESCSTANDBYEVENT(&ESCFlagC);
				ESCFlagC.stopFlag = 0;

				if (ESCFlagC.startFlag == 1)
				{
					SoeRecData(SOE_GP_EVENT);
					ESCFlagC.stopFlag = 0;
					if ((GET_GV_VOL_CTRL_C == 1) && (GET_BYPASS_FEEDBACK == 1) && (ESCBYRelayCNTC != 1) && (ESCSicFaultCNTC != 1))
					{
						StateEventFlag_C = STATE_EVENT_RECHARGE_C;
						ESCFlagC.BYFEEDBACKFLAG = 1;
					}
					ESCFlagC.ESCCntSec.ChargingTime = 0;
				}
				break;

			case STATE_EVENT_RECHARGE_C:
				SET_RUNNING_LED(1);
				SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE)
				;
				SET_IGBT_ENC(IGBT_DISABLE);
				DisablePWMC();
				ESCPRECHARGEEVENT(&ESCFlagC);
				if (ESCFlagC.stopFlag == 1)
				{
					ESCFlagC.stopFlag = 0;
					ESCFlagC.startFlag = FALSE;
					ESCFlagC.PeakStopFlag = 1;
					StateEventFlag_C = STATE_EVENT_STOP_C;
					SoeRecData(SOE_GP_EVENT + 1);
				}
				break;

			case STATE_EVENT_RUN_C:
				if (ESCFlagC.onceTimeStateMachine == 1)
				{
					ESCFlagC.ESCCntMs.PllDelay = 0;
					if (ESCFlagC.ByPassContactFlag == 0)
					{
						SET_SCRC_ENABLE(ESC_ACTION_DISABLE);
						ESCFlagC.onceTimeStateMachine = 0;
					}
				}
				ESCRUNEVENT(&ESCFlagC);
				ESCRUNTURNWAIT(&ESCFlagC);
				SET_RUNNING_LED(1);
				if (ESCFlagC.stopFlag == 1)
				{
					ESCFlagC.stopFlag = 0;
					ESCFlagC.startFlag = 0;
					ESCFlagC.PeakStopFlag = 1;
					SET_SCRC_ENABLE(ESC_ACTION_ENABLE);
					StateEventFlag_C = STATE_EVENT_STOP_C;
				}

				break;

			case STATE_EVENT_STOP_C:
				ESCSTOPSTATE(&ESCFlagC);
				if (GET_BYPASS_FEEDBACK != 0)
				{
					SET_RUNNING_LED(0);
				}
#if !TEST_RUNPORCESS
				ExecuteOpenContact_C();
#endif
				StateEventFlag_C = STATE_EVENT_STANDBY_C;
				SoeRecData(SOE_GP_EVENT + 1);
				ESCFlagC.ESCCntSec.HWPowerStopDelay = 0;
				ESCFlagC.ESCCntSec.HWPowerFaultDelay = 0;
				break;

			case STATE_EVENT_WAIT_C:
				ESCWAITTURNRUN(&ESCFlagC);

				SET_RUNNING_LED(1);
				if (ESCFlagC.stopFlag == 1)
				{
					SET_POWER_CTRL(1);
					Delayus(TIME_WRITE_15VOLT_REDAY);
					ESCFlagC.stopFlag = 0;
					ESCFlagC.startFlag = 0;
					ESCFlagC.PeakStopFlag = 1;
					StateEventFlag_C = STATE_EVENT_STOP_C;
					SoeRecData(SOE_GP_EVENT + 1);
				}
				break;

			case STATE_EVENT_FAULT_C:
				ESCFlagC.startFlag = 0;
				ESCFlagC.stopFlag = 1;
				SET_RUNNING_LED(0);
				FAULTSTATE(&ESCFlagC);
				if (ESCBYRelayCNTC != 1)
				{
					ExecuteOpenContact_C();
				}
				SET_FAULT_LED(1);
				break;
			default:
				break;
		}

		/*处理A相*/
		if ((ESCFlagA.resetFlag == 1) //A相需要执行复位操作。WY
				&& (StateEventFlag_A == STATE_EVENT_FAULT_A)) //故障停机状态。WY
		{
			StateEventFlag_A = STATE_EVENT_STOP_A; //切换状态机至：A相主动停机状态。WY

			ESCFlagA.faultFlag = 0;
			ESCFlagA.startFlag = 0; //A相无需执行启动操作。WY
			ESCFlagA.FAULTCONFIRFlag = 0;
			ESCFlagA.stopFlag = 1; //A相需要执行停机操作。WY
			FaultClearPWMA();
			DisablePWMA(); //封锁A相PWM。WY
			ESCFlagA.onceRunStartFlag = 0;
			ESCFlagA.HWPowerFAULTFlag = 0;
			ESCFlagA.resetFlag = 0;
			FaultReset();

			if ((StateEventFlag_A != STATE_EVENT_FAULT_A)
					&& (StateEventFlag_B != STATE_EVENT_FAULT_B)
					&& (StateEventFlag_C != STATE_EVENT_FAULT_C))
			{
				SET_FAULT_LED(0);
			}
			SoeRecData(SOE_GP_EVENT + 21);
		}
		else
		{
			ESCFlagA.resetFlag = 0;
		}

		/*处理B相*/
		if ((ESCFlagB.resetFlag == 1) && (StateEventFlag_B == STATE_EVENT_FAULT_B))
		{
			StateEventFlag_B = STATE_EVENT_STOP_B;
			ESCFlagB.faultFlag = 0;
			ESCFlagB.startFlag = 0;
			ESCFlagB.FAULTCONFIRFlag = 0;
			ESCFlagB.stopFlag = 1;
			FaultClearPWMB();
			DisablePWMB();
			ESCFlagB.onceRunStartFlag = 0;
			ESCFlagB.HWPowerFAULTFlag = 0;
			ESCFlagB.resetFlag = 0;
			FaultReset();
			if ((StateEventFlag_A != STATE_EVENT_FAULT_A) && (StateEventFlag_B != STATE_EVENT_FAULT_B) && (StateEventFlag_C != STATE_EVENT_FAULT_C))
			{
				SET_FAULT_LED(0);
			}
			SoeRecData(SOE_GP_EVENT + 21);
		}
		else
		{
			ESCFlagB.resetFlag = 0;
		}

		/*处理C相*/
		if ((ESCFlagC.resetFlag == 1) && (StateEventFlag_C == STATE_EVENT_FAULT_C))
		{
			StateEventFlag_C = STATE_EVENT_STOP_C;
			ESCFlagC.faultFlag = 0;
			ESCFlagC.startFlag = 0;
			ESCFlagC.FAULTCONFIRFlag = 0;
			ESCFlagC.stopFlag = 1;
			FaultClearPWMC();
			DisablePWMC();
			ESCFlagC.onceRunStartFlag = 0;
			ESCFlagC.HWPowerFAULTFlag = 0;
			ESCFlagC.resetFlag = 0;
			FaultReset();
			if ((StateEventFlag_A != STATE_EVENT_FAULT_A) && (StateEventFlag_B != STATE_EVENT_FAULT_B) && (StateEventFlag_C != STATE_EVENT_FAULT_C))
			{
				SET_FAULT_LED(0);
			}
			SoeRecData(SOE_GP_EVENT + 21);
		}
		else
		{
			ESCFlagC.resetFlag = 0;
		}
	}
}

int FAN_CTL_Mode = 1;
int Fan_speed_manual;
int fanIsRunFlag = 0;
int FanTempValue0,FanTempValue1,FanTempValue2;
int WindADVal0,WindADVal1,WindADVal2;
float PICTLFANVAL = 0.0;
float CTLFANDUTY = 0;
int ADINITVAL=4096;
float FANTEMPVARVAL = 0;
float CURRCTLFANDATA = 0,TEMPCTLFANDATA = 0,CURRCTLFANDATA1 = 0,TEMPCTLFANDATA1 = 0;
int CURRFUNCTRFlag = 1,TEMPFUNCTRFlag = 1;
int CURRCTRLFUNFlag = 0,TEMPCTRLFUNFlag = 0;
float CURRMaxValue = 0;

float tem2;

void FunAutoControl(void)
{
    WindADVal0 = (int)ADC_RU_TEMP0;
    WindADVal1 = (int)ADC_RU_TEMP1;
//    WindADVal2 = (int)ADC_RU_TEMP2;   //未使用
    FanTempValue0 = ntc_tab[(Uint16)WindADVal0];
    FanTempValue1 = ntc_tab[(Uint16)WindADVal1];
//    FanTempValue2 = ntc_tab[(Uint16)WindADVal2];
    TempData[0] = 0.7*TempData[0] + 0.3*FanTempValue0;
    TempData[1] = 0.7*TempData[1] + 0.3*FanTempValue1;
//    TempData[2] = 0.7*TempData[2] + 0.3*FanTempValue2;
    TempMaxValue = TempCompFuntion(TempData,2);   //采样温度取最大值
    CURRMaxValue = CURRCompFuntion(CURRData,6);   //在三相电网电流和负载电流中取最大值
    WindCold.HeatSinkTempterature = 0.9*WindCold.HeatSinkTempterature + 0.1*TempMaxValue;
    if(WindCold.HeatSinkTempterature > 150)      WindCold.HeatSinkTempterature = 150;
    if(WindCold.HeatSinkTempterature < (-30))    WindCold.HeatSinkTempterature = -30;

              if(GET_CTRL24_POWER != 1){
                  EPwm1Regs.AQCSFRC.bit.CSFA = 0;
              }
              if(TempEnvirProvalue == 1){    //温升测试时使用,百分百占空比,控制全速转
                  CTLFANDUTY = FANCtrlMaxDUTY;
                  PICTLFANVAL = 0.99*PICTLFANVAL + 0.01*CTLFANDUTY;
              }else{
                  if(  (StateEventFlag_A == STATE_EVENT_RUN_A)||\
                       (StateEventFlag_B == STATE_EVENT_RUN_B)||\
                       (StateEventFlag_C == STATE_EVENT_RUN_C)  ){
//        1.载30%-70%启动,大于30%,风机40%转速40-80调节;参数界面可调    最低转速/载对应风机启和最大转速/温度对应风机启动和最大转速
//        2.60度全速80%,40度启动风机40%;参数界面可调
//        这两条得出占空比按最大值来给.
                  //CTLFANDATA[0]=(当前电流值B0-最低负载电流启动风机B1)*(最高占空比0.8-最低占空比B3)/(最高负载转入全速风机B4-最低负载启动风机B1)+最低占空比B3;
//                  if((0.8>B3)&&(B4>B1)&&(B3>0.2)&&(B1>0)){
                  if((FANCtrlMaxDUTY>FanStartMinDUTY)&&(Curr_MaxCtrlFAN>Curr_MinCtrlFAN)&&(FanStartMinDUTY>FANCtrlMinDUTY)&&(Curr_MinCtrlFAN>0)){
//                      if((B0 >= B1)&&(B0 <= B4)){
//                          CTLFANDATA[0] = (B0-B1)*(0.8-B3)/(B4-B1)+B3;
//                      }else if(B0 < B1){
//                          CTLFANDATA[0] = 0;
//                      }else if(B0 > B4){
//                          CTLFANDATA[0] = 0.8;
//                      }
                      if((CURRMaxValue >= Curr_MinCtrlFAN)&&(CURRMaxValue <= Curr_MaxCtrlFAN)){
                          CURRCTLFANDATA = (CURRMaxValue-Curr_MinCtrlFAN)*(FANCtrlMaxDUTY-FanStartMinDUTY)/(Curr_MaxCtrlFAN-Curr_MinCtrlFAN)+FanStartMinDUTY;
                          CTLFANDATA[0] = 0.99*CTLFANDATA[0] + 0.01*CURRCTLFANDATA;
                      }
                      if(CURRMaxValue < Curr_MinCtrlFAN){
                          CTLFANDATA[0] = 0;
                      }
                      if(CURRMaxValue > Curr_MaxCtrlFAN){
                          CURRCTLFANDATA1 = FANCtrlMaxDUTY;
                          CTLFANDATA[0] = 0.99*CTLFANDATA[0] + 0.01*CURRCTLFANDATA1;
                      }
                  }else{
                      CTLFANDATA[0] = 0;
                  }
                  //CTLFANDATA[1]=(当前温度值-最低温度启动风机)*(最高占空比-最低占空比)/(最高温度转入全速风机-最低温度)+最低占空比;
                  if((FANCtrlMaxDUTY>FanStartMinDUTY)&&(Temp_MaxCtrlFAN>Temp_MinCtrlFAN)&&(FanStartMinDUTY>FANCtrlMinDUTY)&&(Temp_MinCtrlFAN>0)){
                      if((WindCold.HeatSinkTempterature >= Temp_MinCtrlFAN)&&(WindCold.HeatSinkTempterature <= Temp_MaxCtrlFAN)){
                          TEMPCTLFANDATA = (WindCold.HeatSinkTempterature-Temp_MinCtrlFAN)*(FANCtrlMaxDUTY-FanStartMinDUTY)/(Temp_MaxCtrlFAN-Temp_MinCtrlFAN)+FanStartMinDUTY;
                          CTLFANDATA[1] = 0.99*CTLFANDATA[1] + 0.01*TEMPCTLFANDATA;
                      }
                      if(WindCold.HeatSinkTempterature < Temp_MinCtrlFAN){
                          CTLFANDATA[1] = 0;
                      }
                      if(WindCold.HeatSinkTempterature > Temp_MaxCtrlFAN){
                          TEMPCTLFANDATA1 = FANCtrlMaxDUTY;
                          CTLFANDATA[1] = 0.99*CTLFANDATA[1] + 0.01*TEMPCTLFANDATA1;
                      }
                  }else{
                      CTLFANDATA[1] = 0;
                  }
                      PICTLFANVAL = DUTYMUXFuntion(CTLFANDATA,2);   //取二者最大占空比最终输出
              }else{
                  CTLFANDATA[0] = 0;
                  CTLFANDATA[1] = 0;
                  PICTLFANVAL = 0;
              }
          }
          EPwm1Regs.CMPA.bit.CMPA = PICTLFANVAL*FUNT1PR;

//          tem2 = WindCold.BOARD_OVER_TEMP - TempData[0];    //tem2 = 散热器过温值80 - 散热器温度;
//          if(tem2>20){
//              tem2 = 20.001;  //温度超过20度,最大容量输出
//          }
//          if(tem2<0){
//              tem2 = 0.0;     //温度小于0度,最小容量输出
//          }
//
//          tem2 = WindCold.UNIT_OVER_TEMP - TempData[1];           //tem2 = 单元机壳内部温度过温值 70 - 单元内部温度;
//          if(tem2<5){
//              tem2 = 0.0;    //温度小于0度,最小容量输出
//          }
//          CurrTargetTemper = CurTarget[CapacitySelection][(Uint16)(tem2)];//三相共用一个温度值即可

          //tem2 = 散热器过温值80 - 散热器温度;
          if((WindCold.BOARD_OVER_TEMP - TempData[0])>20){
              tem2 = 20.001;  //温度超过20度,最大容量输出
              CurrTargetTemper = CurTarget[CapacitySelection][(Uint16)(tem2)];//三相共用一个温度值即可
          }else if((WindCold.BOARD_OVER_TEMP - TempData[0]) < 0){
              tem2 = 0.0;     //温度小于0度,最小容量输出
              CurrTargetTemper = CurTarget[CapacitySelection][(Uint16)(tem2)];//三相共用一个温度值即可
          }else{
              tem2 = WindCold.BOARD_OVER_TEMP - TempData[0];
              CurrTargetTemper = CurTarget[CapacitySelection][(Uint16)(tem2)];//三相共用一个温度值即可
          }

          //tem2 = 单元机壳内部温度过温值 70 - 单元内部温度;
          if((WindCold.UNIT_OVER_TEMP - TempData[1]) < 5){
              tem2 = 0.0;    //温度小于0度,最小容量输出
              CurrTargetTemper = CurTarget[CapacitySelection][(Uint16)(tem2)];//三相共用一个温度值即可
          }
}

void Fan_Cnt(void)
{
    if(++FAN_CTL_Mode >= /*4*/7)
        FAN_CTL_Mode=0;
}

float DUTYMUXFuntion(float CTLFANDATA[], int VAL)  //风机调速占空比比较取最大值
{
    int i;
    float DUTYMax = CTLFANDATA[0];
    for(i=1;i<VAL;i++)
    {
        if(CTLFANDATA[i] > DUTYMax)
        {
            DUTYMax = CTLFANDATA[i];
        }
    }
    if(i>=VAL)  i=1;
    return DUTYMax;
}

float CURRCompFuntion(float CURRData[], int VAL)   //三相输入/输出电流采样取最大值
{
    int i;
    float CURRMax = CURRData[0];
    for(i=1;i<VAL;i++)
    {
        if(CURRData[i] > CURRMax)
        {
            CURRMax = CURRData[i];
        }
    }
    if(i>=VAL)  i=1;
    return CURRMax;
}

float TempCompFuntion(float TempData[], int VAL)   //采样温度取最大值
{
    int i;
    float TempMax = TempData[0];
//    int TempDATA;
    for(i=1;i<VAL;i++)
    {
        if(TempData[i] > TempMax)
        {
            TempMax = TempData[i];
        }
    }
    if(i>=VAL)  i=1;
    return TempMax;
}

float UNCurrMAXCompFuntion(float UnCurrData[], int VAL)
{
    int i;
    float UNCurrMax = UnCurrData[0];
    for(i=1;i<VAL;i++)
    {
        if(UnCurrData[i] > UNCurrMax)
        {
            UNCurrMax = UnCurrData[i];
        }
    }
    if(i>=VAL)  i=1;
    return UNCurrMax;
}

float UNCurrMINCompFuntion(float UnCurrData[], int VAL)
{
    int i;
    float UNCurrMin = UnCurrData[0];
    for(i=1;i<VAL;i++)
    {
        if(UnCurrData[i] < UNCurrMin)
        {
            UNCurrMin = UnCurrData[i];
        }
    }
    if(i>=VAL)  i=1;
    return UNCurrMin;
}

float UNCurDiffMAXFuntion(float UNCurDiffData[], int VAL)
{
    int i;
    float UNCurDiffMAX = UNCurDiffData[0];
    for(i=1;i<VAL;i++)
    {
        if(UNCurDiffData[i] > UNCurDiffMAX)
        {
            UNCurDiffMAX = UNCurDiffData[i];
        }
    }
    if(i>=VAL)  i=1;
    return UNCurDiffMAX;
}


/*该函数未定义。WY*/
void MainContactConfirm(Uint16 VAL)
{
}

// 放电程序，强制进入放电程序，并且其他状态机不重叠。
void DischargingMode(void)
{
//	DINT;
//	EnablePWM();
//	SET_IGBT_EN1(IGBT_ENABLE);
//	// 开通角设定为百分之十
//    EPwm4Regs.CMPA.bit.CMPA = 500;
//    EPwm5Regs.CMPA.bit.CMPA = 500;
//    EPwm6Regs.CMPA.bit.CMPA = 500;
//	EINT;
//	StateFlag.LEDRunFlag=ITS_WORKING_HIGH;
//    // 报硬件故障 或者 直流电压降低到位。则切入故障状态。
//	if((GET_IGBT_FAULT_ACK1() == IGBT_FEEDBACK_FAULT_FLAG)||(dcVoltF <= 20))
//	{
//		StateEventFlag = STATE_EVENT_FAULT;
//	}
}

void Delayus(int16 i){ //i*5*0.001 ms
//	int j;
	for(;i>=0;i--){
		asm(" RPT #31 || NOP");
		asm(" RPT #31 || NOP");
//		asm(" RPT #31 || NOP");
//		asm(" RPT #31 || NOP");
//		for(j=10;j>=0;j--);
	}
}

#define LMT84_TEMPERATURE_COEFF_K			1.313f
#define LMT84_TEMPERATURE_COEFF_B			1853.0f
void OutsideIsrProg(void)
{
    UInt events;
    static Uint32 OutPro=0;
    while(1)
    {
        // Wait for ANY of the ISR events to be posted *
        events = Event_pend(Event_Outside, Event_Id_NONE,Event_Id_00 ,BIOS_WAIT_FOREVER);
        static int cntForRunEach=0;
//        float tempT2=0,tempT3=0,tempOnBoard=0;
//        int  temperature2,temperature3;


        OutPro++;
        if(++cntForRunEach >= 4)  // 这个位置是N，就表示main函数里轮询N-1次
            cntForRunEach = 0;

        if((SPLL[0].PllPiOutput>314.6)||(SPLL[0].PllPiOutput<313.8))													//检查锁相
        {
            if((StateFlag.SequenceAutoFlag)&&(CntSec.OverTimeCount>CNT_MS(300)))
            {
                Information_Structure.Correction.B.OverTimeFlag = 1;
                Information_Structure.Correction.B.VoltPhaseSequenFailFlag = 1;
            }
            StateFlag.PLLSafetyFlag = 0;
            CntMs.PLLSafetyCount = 0;
        }
        if(CntMs.PLLSafetyCount > CNT_MS(500))
        {
            CntSec.OverTimeCount = 0;
            StateFlag.PLLSafetyFlag = 1;	//锁相正常
        }
        if(StateFlag.SoeFlag&& !StateFlag.EEPROMResourceLock)			SOE_Write();
    //  if(RecordWaveFlag&&!StateFlag.EEPROMResourceLock&&(StateEventFlag==STATE_EVENT_FAULT))    SOE_Wave_Record();

            switch(cntForRunEach){
            case 0:		// 1  温度采样子程序
//                temperature3 = (int)(ADC_RU_TEMP0 - VirtulAD.temperature3);
//                temperature2 = (int)(ADC_RU_TEMP1 - VirtulAD.temperature2);
//
//                temperature2 = ntc_tab[temperature2];
//                tempT2 = (float)temperature2 * 0.1;			//单元测温2
//                WindCold.HeatSinkTempterature 		= 0.88*WindCold.HeatSinkTempterature + 0.12*tempT2;
//                if(WindCold.HeatSinkTempterature>200)WindCold.HeatSinkTempterature=200;
//
//                temperature3 = (int)(ADC_RU_TEMP0 - VirtulAD.temperature3);
//                temperature3 = ntc_tab[temperature3];
//                tempT3 = (float)temperature3 * 0.1;         //单元测温2
//                WindCold.EnvirTemperature       = 0.88*WindCold.EnvirTemperature + 0.12*tempT3;
//                if(WindCold.EnvirTemperature>200)WindCold.EnvirTemperature=200;
//
//                tempOnBoard =(LMT84_TEMPERATURE_COEFF_B-(LMT84_TEMPERATURE_COEFF_K*ADC_RU_DSPTEMP))* 0.1;	//JCL:T3作为增加板载温度传感
//                WindCold.MotherBoardTempterature = 0.88*WindCold.MotherBoardTempterature + 0.12*tempOnBoard;
//                if(WindCold.MotherBoardTempterature>200)WindCold.MotherBoardTempterature=200;

                FunAutoControl();// 对单元内部风机做单独的控制

                if(GET_BYPASS_FEEDBACK == 0){
                    if(CntSec.BypassSwitch > CNT_SEC(10)){
                        if(ESCFlagA.BYFEEDBACKFLAG == 1){
                            StateEventFlag_A = STATE_EVENT_STOP_A;
                            ESCFlagA.BYFEEDBACKFLAG = 0;
                        }
                        if(ESCFlagB.BYFEEDBACKFLAG == 1){
                            StateEventFlag_B = STATE_EVENT_STOP_B;
                            ESCFlagB.BYFEEDBACKFLAG = 0;
                        }
                        if(ESCFlagC.BYFEEDBACKFLAG == 1){
                            StateEventFlag_C = STATE_EVENT_STOP_C;
                            ESCFlagC.BYFEEDBACKFLAG = 0;
                        }
                        SoeRecData(SOE_GP_EVENT + 7);         //ESC旁路微断在运行过程中,被手动误操作,会在SOE中显示出来,设备转到正常停机状态.//需要改规约--旁路微断误操作警告
                        ESCFlagA.startFlag = 0;
                        ESCFlagB.startFlag = 0;
                        ESCFlagC.startFlag = 0;
                        ESCFlagA.stopFlag = 1;
                        ESCFlagB.stopFlag = 1;
                        ESCFlagC.stopFlag = 1;
                        ESCFlagA.faultFlag = 0;
                        ESCFlagB.faultFlag = 0;
                        ESCFlagC.faultFlag = 0;
                        ESCFlagA.autoStFlag = ORIGINAL_STATE_A;
                        ESCFlagB.autoStFlag = ORIGINAL_STATE_B;
                        ESCFlagC.autoStFlag = ORIGINAL_STATE_C;
                        ESCFlagA.onceRunStartFlag = 1;
                        ESCFlagB.onceRunStartFlag = 1;
                        ESCFlagC.onceRunStartFlag = 1;
                        if(ESCBYRelayCNTA != 1){
                            ESCFlagA.HWPowerSTOPFlag = 1;
                        }
                        if(ESCBYRelayCNTB != 1){
                            ESCFlagB.HWPowerSTOPFlag = 1;
                        }
                        if(ESCBYRelayCNTC != 1){
                            ESCFlagC.HWPowerSTOPFlag = 1;
                        }
                        ESCFlagA.ESCCntMs.StartDelay = 0;
                        ESCFlagB.ESCCntMs.StartDelay = 0;
                        ESCFlagC.ESCCntMs.StartDelay = 0;
                        SET_RUNNING_LED(1);
                        SET_FAULT_LED(1);
                    }else{
                        ESCFlagA.ESCCntSec.HWPowerStopDelay = 0;
                        ESCFlagB.ESCCntSec.HWPowerStopDelay = 0;
                        ESCFlagC.ESCCntSec.HWPowerStopDelay = 0;
                    }
                }else{
                    CntSec.BypassSwitch = 0;
                }

                if((ESCFlagA.HWPowerFAULTFlag == 1)&&(ESCFlagB.HWPowerFAULTFlag == 1)&&(ESCFlagC.HWPowerFAULTFlag == 1)){ 
                    if(   (ESCFlagA.ESCCntSec.HWPowerFaultDelay >= CNT_SEC(POWERCTRLCNT_15V))&&\
                          (ESCFlagB.ESCCntSec.HWPowerFaultDelay >= CNT_SEC(POWERCTRLCNT_15V))&&\
                          (ESCFlagC.ESCCntSec.HWPowerFaultDelay >= CNT_SEC(POWERCTRLCNT_15V))   ){
                        if((ESCBYRelayCNTA != 1)&&\
                                (ESCBYRelayCNTB != 1)&&\
                                (ESCBYRelayCNTC != 1)){
                            SET_POWER_CTRL(0);
                        }
                        ESCFlagA.HWPowerFAULTFlag = 0;
                        ESCFlagB.HWPowerFAULTFlag = 0;
                        ESCFlagC.HWPowerFAULTFlag = 0;
                    }
                }
                if((ESCFlagA.HWPowerSTOPFlag == 1)&&(ESCFlagB.HWPowerSTOPFlag == 1)&&(ESCFlagC.HWPowerSTOPFlag == 1)){
                    if(   (ESCFlagA.ESCCntSec.HWPowerStopDelay >= CNT_SEC(POWERCTRLCNT_15V))&&\
                          (ESCFlagB.ESCCntSec.HWPowerStopDelay >= CNT_SEC(POWERCTRLCNT_15V))&&\
                          (ESCFlagC.ESCCntSec.HWPowerStopDelay >= CNT_SEC(POWERCTRLCNT_15V))   ){
                        if((ESCBYRelayCNTA != 1)&&\
                                (ESCBYRelayCNTB != 1)&&\
                                (ESCBYRelayCNTC != 1)){
                            SET_POWER_CTRL(0);
                        }
                        ESCFlagA.HWPowerSTOPFlag = 0;
                        ESCFlagB.HWPowerSTOPFlag = 0;
                        ESCFlagC.HWPowerSTOPFlag = 0;
                    }
                }
///************加这个判断是为了在整机测试时,误操作旁路机械开关后,15V供电会先断开,然后在60s之后会再次和15V供电,而这时已经不能自动启动了,
// ************所以防止停机时,设备15V供电一直有,会有损耗,加上该函数判断,会在一定时间后再断开15V供电.************/
//                if(    (StateFlag.startingMethod == 0)&&\
//                       (GET_POWER_CTRL == 1)&&\
//                       (StateEventFlag_A == STATE_EVENT_STANDBY_A)&&\
//                       (StateEventFlag_B == STATE_EVENT_STANDBY_B)&&\
//                       (StateEventFlag_C == STATE_EVENT_STANDBY_C)   ){
//                    if(CntSec.StopDelay > CNT_SEC(60)){
//                        if((ESCBYRelayCNTA != 1)&&\
//                                (ESCBYRelayCNTB != 1)&&\
//                                (ESCBYRelayCNTC != 1)){
//                            SET_POWER_CTRL(0);
//                        }
//                        CntSec.StopDelay = 0;
//                    }
//                }
                if((ESCBYRelayCNTA == 1)||(ESCSicFaultCNTA == 1)){  //高低压磁保持继电器/sic管子损坏故障计数次数达到5次即5次之上,设备不在自动启动
                    ESCFlagA.ESCCntMs.StartDelay = 0;
                    ESCFlagA.resetFlag = 0;
                }
                if((ESCBYRelayCNTB == 1)||(ESCSicFaultCNTB == 1)){
                    ESCFlagB.ESCCntMs.StartDelay = 0;
                    ESCFlagB.resetFlag = 0;
                }
                if((ESCBYRelayCNTC == 1)||(ESCSicFaultCNTC == 1)){
                    ESCFlagC.ESCCntMs.StartDelay = 0;
                    ESCFlagC.resetFlag = 0;
                }
//该判断是用来设备在运行时发生故障,切换旁路时,旁路磁保持继电器未动作,然后检测负载电流小于1A300MS,然后使高低压磁保持继电器合闸,管子直通来保持负载不长时间断流;
//该判断使用了负载电流的判断,不知道精不精准,如果有负载本身电流就小的情况,该判断就有问题了????
//                if((gridCurrBYAF_rms < 1.0f)&&(StateEventFlag_A == STATE_EVENT_FAULT_A)){
//                    if(ESCFlagA.ESCCntMs.CutCurrDelay > CNT_MS(300)){
//                        ESCFlagA.PWM_ins_index = 1;
//                        ESCFlagA.ESC_DutyData = 1.0;
//                        SET_SCRA_ENABLE(ESC_ACTION_ENABLE);                 //开晶闸管
//                        SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_ENABLE);
//                        SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE);     //切旁路
//                        Delayus(TIME_WRITE_15VOLT_REDAY);
//                        EnablePWMA();
//                        Delayus(100);                                                      //需要加个延时时间,否则PWM的寄存器会来不及动作
//                        SET_IGBT_ENA(IGBT_ENABLE);                                         //开IGBT使能
//                        EPwm3Regs.CMPA.bit.CMPA = T1PR;     //1管直通      //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
//                        EPwm3Regs.CMPB.bit.CMPB = T1PR;     //2管不直通
//                        EPwm4Regs.CMPA.bit.CMPA = T1PR;     //3管直通
//                        EPwm4Regs.CMPB.bit.CMPB = T1PR;     //4管不直通
//                        ESCBYRelayCNTA = 1;
//                    }
//                }
//                if((gridCurrBYBF_rms < 1.0f)&&(StateEventFlag_B == STATE_EVENT_FAULT_B)){
//                    if(ESCFlagB.ESCCntMs.CutCurrDelay > CNT_MS(300)){
//                        ESCFlagB.PWM_ins_index = 1;
//                        ESCFlagB.ESC_DutyData = 1.0;
//                        SET_SCRB_ENABLE(ESC_ACTION_ENABLE);                 //开晶闸管
//                        SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_ENABLE);
//                        SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE);     //切旁路
//                        Delayus(TIME_WRITE_15VOLT_REDAY);
//                        EnablePWMB();
//                        Delayus(100);                                                      //需要加个延时时间,否则PWM的寄存器会来不及动作
//                        SET_IGBT_ENB(IGBT_ENABLE);                                         //开IGBT使能
//                        EPwm5Regs.CMPA.bit.CMPA = T1PR;     //1管直通      //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
//                        EPwm5Regs.CMPB.bit.CMPB = T1PR;     //2管不直通
//                        EPwm6Regs.CMPA.bit.CMPA = T1PR;     //3管直通
//                        EPwm6Regs.CMPB.bit.CMPB = T1PR;     //4管不直通
//                        ESCBYRelayCNTB = 1;
//                    }
//                }
//                if((gridCurrBYCF_rms < 1.0f)&&(StateEventFlag_C == STATE_EVENT_FAULT_C)){
//                    if(ESCFlagC.ESCCntMs.CutCurrDelay > CNT_MS(300)){
//                        ESCFlagC.PWM_ins_index = 1;
//                        ESCFlagC.ESC_DutyData = 1.0;
//                        SET_SCRC_ENABLE(ESC_ACTION_ENABLE);                 //开晶闸管
//                        SET_MAIN_CONTACT_ACTION_C(ESC_ACTION_ENABLE);
//                        SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE);     //切旁路
//                        Delayus(TIME_WRITE_15VOLT_REDAY);
//                        EnablePWMC();
//                        Delayus(100);                                                      //需要加个延时时间,否则PWM的寄存器会来不及动作
//                        SET_IGBT_ENC(IGBT_ENABLE);                                         //开IGBT使能
//                        EPwm7Regs.CMPA.bit.CMPA = T1PR;     //1管直通      //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
//                        EPwm7Regs.CMPB.bit.CMPB = T1PR;     //2管不直通
//                        EPwm8Regs.CMPA.bit.CMPA = T1PR;     //3管直通
//                        EPwm8Regs.CMPB.bit.CMPB = T1PR;     //4管不直通
//                        ESCBYRelayCNTC = 1;
//                    }
//                }

                OverTempLimitCur();
                break;

            case 1:
                THDiCal();
                PIAdjust();
                RemoteWriteControl(RemoteAddress);
                //FlashRecordWrite();
                break;

            case 2:
        //参数设置
            if(StateFlag.CapDataParamWrite && !StateFlag.EEPROMResourceLock)		//电容设置
            {
                StateFlag.CapDataParamWrite = false;
                CapParamSetting();
                CapParamRefresh();
            }

            if(StateFlag.ManufacturerParameters && !StateFlag.EEPROMResourceLock)		//新厂家参数
            {
                StateFlag.ManufacturerParameters = false;
                ManufacturerParametersSetting();
                ManufacturerParametersRefresh();
                InitCtrlParam(1);
                ESCfilterMemCopy(CapacitySelection);
            }

            if(StateFlag.VitruZeroParameters && !StateFlag.EEPROMResourceLock)		//零偏
            {
                StateFlag.VitruZeroParameters = false;
                VirtuPreferencesSetting();
                VirtuPreferencesRefresh();
            }

            if(StateFlag.UserPreferences && !StateFlag.EEPROMResourceLock)				//新用户参数
            {
                StateFlag.UserPreferences = false;
                UserPreferencesSetting();
                UserPreferencesRefresh();
            }

            if(StateFlag.IDPreferences && !StateFlag.EEPROMResourceLock)				//地址匹配
            {
                StateFlag.IDPreferences = false;
                IDPreferencesSetting();
                IDParametersRefresh();
            }

            if(StateFlag.harmCompPercParamRefresh&& !StateFlag.EEPROMResourceLock)		//谐波基本设置
            {
                StateFlag.harmCompPercParamRefresh = false;
                UserHarmnoicSetting();
                HarmnoicCorrection();
            }

            if(StateFlag.HarmonicWaveParamRefresh&& !StateFlag.EEPROMResourceLock)		//谐波厂家设置
            {
                StateFlag.HarmonicWaveParamRefresh = false;
                HarmnoicParamSetting();
                HarmnoicCorrection();
                #if TEST_VIRTUALSOURCE!=0
                    TestWaveGenerator();
                #else
                if((StateFlag.harmCompEn == 1)&&(StateFlag.isHarmCompensateMode == 0))
                TestWaveGenerator();
                #endif
            }
            if(StateFlag.RecordWritting&& !StateFlag.EEPROMResourceLock){
                StateFlag.RecordWritting = false;
                FlashRecordTim = 0;
                FlashPreferencesSetting();
            }
            if(CntMs.displayTimingCount>CNT_MS(500))	//监控
            {
                CntMs.displayTimingCount = 0;
                Monitor_Message();
                RemoteParamerRefresh();
			    RemoteParamerRefresh2();
//                Multiple_Parallel_Message();
            }

            if(PWM_address == 0){						//相序校正初始化
                if(StateFlag.SequenceAutoFlag)
                {
                    if(CntMs.InitializationDelay>CNT_MS(200)){
                        StateFlag.startingMethod = 0;
                    }else{
                        if((StateEventFlag_A==STATE_EVENT_RUN_A)&&(FactorySet.HarmonicInfer.B.ManualMode==0)&&(FactorySet.HarmonicInfer.B.AutomaticMode==0)){
                            StateFlag.SequenceAutoFlag = 0;                 //自动谐波校正和自动相序校正不可以同时进行
                        }else{
                            StateFlag.PhaseSequeJudSucceedFlag = 0;
                            FactorySet.Infer.B.VoltagePhaseSequence = 0;
                            FactorySet.Infer.B.PhaseSequenceCT = 0;
                            FactorySet.Infer.B.DirectionCT = 0;
                            Information_Structure.Correction.all = 0;
                            Information_Structure.Correction.B.DefectCT = FactorySet.Infer.B.DefectCT;
                            CorrectingAD();																//电压相序
                            CorrectingCT();																//电流相序
                            DirectionCT();																//电流方向
                            StateFlag.startingMethod = 1;

                            if(StateFlag.positionCT)													//负载侧
                            {
                                if((StateFlag.CurrACountFlag==0)&&(StateFlag.CurrBCountFlag==0)&&(StateFlag.CurrCCountFlag==0))
                                {
                                    if((SeqJud[0]<3)||(SeqJud[1]<3)||(SeqJud[2]<3)){	//负载基波太小 放弃自动校正
                                        StateFlag.SequenceAutoFlag = 0;
                                        Information_Structure.Correction.B.LoadingCondition = 1;
                                        Information_Structure.Correction.B.PhaseSequenResult = 1;
                                    }
                                }else{
                                    FFTDataReduction2(1,FFTCalcChan>>1);												//计算负载基波
                                }
                                StateFlag.onceRunStartFlag = 1;
                            }else{																		//电网侧
                                if((StateFlag.CurrACountFlag==0)&&(StateFlag.CurrBCountFlag==0)&&(StateFlag.CurrCCountFlag==0))
                                {
                                    if((SeqJud[0]>20)||(SeqJud[1]>20)||(SeqJud[2]>20)){	//负载干扰太大 放弃自动校正
                                        StateFlag.SequenceAutoFlag = 0;
                                        Information_Structure.Correction.B.LoadingCondition = 1;
                                        Information_Structure.Correction.B.PhaseSequenResult = 1;
                                    }
                                }else{
								    FFTDataReduction2(4,FFTCalcChan>>1);												//发四次谐波前计算负载四次谐波
                                }
                                StateFlag.onceRunStartFlag = 0;
                            }
                        }
                    }
                }else{
                    CntSec.OverTimeCount = 0;
                    CntMs.InitializationDelay = 0;
                    StateFlag.CurrACountFlag=1;
                    StateFlag.CurrBCountFlag=1;
                    StateFlag.CurrCCountFlag=1;
                }
            }
            break;

            case 3:
                if(CntMs.StartDelay >=CNT_MS(500)){		//500ms
                    FaultDetectInMainLoop();													//故障检测
//                    FaultDetectInRunning();
                }

                AutoStartInFault();
                BlindingBluetoothConnectionLED();
                SlaveStateInstruction();														//主机发送4B5B指令
                if(StateFlag.RxcFlag == 1)			// Get_Order();								//从机接收4B5B指令
                if((StateFlag.WatchdogFlag)&&(CntMs.WatchdogDelay>CNT_MS(1000)))
                {
                    StateFlag.WatchdogFlag = 0;                      //通信应答后执行复位
                    EALLOW;
                    WdRegs.SCSR.all = 0;
                    WdRegs.WDCR.all = 0x0020;
                    EDIS;
                }

                if(UserSetting.WordMode.B.StandbyModeFlag)                                  //待机功能开启
                {
//                    switch(CurrentProperty){
//                        case 0:
//                            if(MatchCondition)  AutoJudgeRms_F = MU_LCD_RATIO*Min(CurrRefARms,CurrRefBRms,CurrRefCRms);
//                            else                AutoJudgeRms_F = MU_LCD_RATIO*Max(CurrRefARms,CurrRefBRms,CurrRefCRms);
//                        break;
//                        case 1:
//                            if(MatchCondition)  AutoJudgeRms_F = Min(loadCurA_rms,loadCurB_rms,loadCurC_rms);
//                            else                AutoJudgeRms_F = Max(loadCurA_rms,loadCurB_rms,loadCurC_rms);
//                        break;
//                case 2:
//                    AutoJudgeRms_F = MU_LCD_RATIO*Max(CurrRefARms,CurrRefBRms,CurrRefCRms);
//                break;
//                        default:    AutoJudgeRms_F = CurrRefRms_F;      break;
//                    }
                }
                break;
            default:break;
        }
    }
}

void BlindingBluetoothConnectionLED(void)
{
//    int flag = CntSec.RespondCount%2;
//	if(StateFlag.RespondFlag != 0)
//	{
//        SET_RUNNING_LED(flag);
//        SET_EXT_RUNNING_LED(flag);
//		if(CntSec.RespondCount>StateFlag.RespondFlag*2)	StateFlag.RespondFlag = 0;
//	}else{
//		SET_RUNNING_LED(StateFlag.LEDRunFlag);
//        SET_EXT_RUNNING_LED(StateFlag.LEDRunFlag);
//		CntSec.RespondCount = 0;
//	}
}

void SlaveStateInstruction(void)
{
	if((StateFlag.StateInstruction!=0)&&(CntMs.SendOrderDelay>CNT_MS(60)))
	{
		CntMs.SendOrderDelay = 0;
		SendOrderCount++;
		// Send_Order(0,StateFlag.StateInstruction);
	}
	if(SendOrderCount>2)
	{
		SendOrderCount = 0;
		StateFlag.StateInstruction = 0;
	}
}
//------------ 如果温度过高，需要把电流限幅-------------------//
/*	方法：记录过温情况，然后记录电流值，作为最高电流限幅。
*  只要有一个超过温度最高限制-5，则设定此时电流峰值为限幅。
*  若温度回到温度限制的-10，则逐步放开电流限幅。
*	// 两分钟后，设定额定电流的0.97倍，若两分钟后还过温，则继续设定0.97倍，直至保持一定温度内。反之亦然
*	任何时候都可以调节，
*
*	如果测温线坏掉，就会空载运行，需要增加新的机制
*/
void OverTempLimitCur(void)
{
	if(StateEventFlag_A == STATE_EVENT_RUN_A){
	        switch(DropLetMarkFlag){
            case 0:
                outCurSinkTempLimit = 1;
                if(WindCold.HeatSinkTempterature > WindCold.BackReduceCapacityHeatSink)      DropLetMarkFlag = 1;
            break;
            case 1:
                outCurSinkTempLimit = 0.8;
                if(WindCold.HeatSinkTempterature > WindCold.ReduceCapacityHeatSink)          DropLetMarkFlag = 2;
                if(WindCold.HeatSinkTempterature < (WindCold.BackReduceCapacityHeatSink-4))  DropLetMarkFlag = 0;
            break;
            case 2:
                outCurSinkTempLimit = 0.6;
                if(WindCold.HeatSinkTempterature < (WindCold.ReduceCapacityHeatSink-3))      DropLetMarkFlag = 1;
            break;
		}
    #if NMECB201030REV62
            if(WindCold.EnvirTemperature > WindCold.ReduceCapacityMotherBoard)                  outCurBoardTempLimit = 0.6;
            else                                                                                outCurBoardTempLimit = 1;
    #else
            if(WindCold.MotherBoardTempterature > WindCold.ReduceCapacityMotherBoard)           outCurBoardTempLimit = 0.6;
            else                                                                                outCurBoardTempLimit = 1;
    #endif
        if(outCurSinkTempLimit<outCurBoardTempLimit)                                        outCurTempLimit = outCurSinkTempLimit;
        else                                                                                outCurTempLimit = outCurBoardTempLimit;
	}else{
		outCurTempLimit = 1;
	}
}


/* 自动或者手动校验零偏量、只有停机的时候可以使用
 * 只有几种情况可以使用：1、三相三线制，2、采样点为零电压或零电流，3、三相采样电流或者电压和为零
 * 假设三相四线制的情况下，单相电流取20个周波的和依旧应该为零，即使负载一直运行，采样定标也是成立的。
 * 仅上电或者复位时校验一次
 * 一旦校准失败是不允许启动的
 */
#define AD_REGEDIT_OFFSET_NUM 9600
void AdRegeditOffset(void) // 计算零偏值
{
	int  i=0;
	VirtulADStruVAL *pAD = &VirtulADVAL;
	Stru_Virtu_ZeroOffSETVAL *pID = &VirtuZeroOffSETVAL;
	if(adcAutoCount < AD_REGEDIT_OFFSET_NUM)  // 取整
	{
		tmpOffsetValue[0]  += *pAD->GridHVoltA;  //A15 ESC高压侧AN      //采9600个点相加(只有正弦波才能够进行零偏校准)
		tmpOffsetValue[1]  += *pAD->GridHVoltB;  //B4  ESC高压侧BN
		tmpOffsetValue[2]  += *pAD->GridHVoltC;  //C2  ESC高压侧CN
		tmpOffsetValue[3]  += *pAD->GridLVoltA;  //D1  ESC低压侧AN
		tmpOffsetValue[4]  += *pAD->GridLVoltB;  //B5  ESC低压侧BN
		tmpOffsetValue[5]  += *pAD->GridLVoltC;  //A4  ESC低压侧CN
		tmpOffsetValue[6]  += *pAD->GridMainCurA;//A2  ESC主电抗电流A
		tmpOffsetValue[7]  += *pAD->GridMainCurB;//C3  ESC主电抗电流B
		tmpOffsetValue[8]  += *pAD->GridMainCurC;//D5  ESC主电抗电流C
		tmpOffsetValue[9]  += *pAD->GridBypassCurA; //A0  ESC旁路电流A
		tmpOffsetValue[10] += *pAD->GridBypassCurB; //B2  ESC旁路电流B
		tmpOffsetValue[11] += *pAD->GridBypassCurC; //D4  ESC旁路电流C
        tmpOffsetValue[12] += *pAD->ADCUDCA;     //D3  ESC直流电容电压A
        tmpOffsetValue[13] += *pAD->ADCUDCB;     //A5  ESC直流电容电压B
        tmpOffsetValue[14] += *pAD->ADCUDCC;     //C4  ESC直流电容电压C

		adcAutoCount++;
	}
	else
	{
		for(i=0;i<15;i++)
		{
			tmpOffsetValue[i] *= (1.0f/AD_REGEDIT_OFFSET_NUM);    //将采到的9600个点取平均值
		}
		pID->gridVoltHAOffset = (int16)(tmpOffsetValue[0]+0.5f);  //将计算的平均值给到零偏值(须加上校正值0.5f)   //界面的零偏校准为计算零偏值,然后将计算值上传到界面,如果不修改下发该值即为VariZeroOffsetVAL.gridHVoltA.
		pID->gridVoltHBOffset = (int16)(tmpOffsetValue[1]+0.5f);
		pID->gridVoltHCOffset = (int16)(tmpOffsetValue[2]+0.5f);
		pID->gridVoltLAOffset = (int16)(tmpOffsetValue[3]+0.5f);
		pID->gridVoltLBOffset = (int16)(tmpOffsetValue[4]+0.5f);
		pID->gridVoltLCOffset = (int16)(tmpOffsetValue[5]+0.5f);
		pID->gridMainIAOffset = (int16)(tmpOffsetValue[6]+0.5f);
		pID->gridMainIBOffset = (int16)(tmpOffsetValue[7]+0.5f);
		pID->gridMainICOffset = (int16)(tmpOffsetValue[8]+0.5f);
		pID->gridBypassIAOffset = (int16)(tmpOffsetValue[9]+0.5f);
		pID->gridBypassIBOffset = (int16)(tmpOffsetValue[10]+0.5f);
		pID->gridBypassICOffset = (int16)(tmpOffsetValue[11]+0.5f);
        pID->aDCUDCA = 0;
        pID->aDCUDCB = 0;
        pID->aDCUDCC = 0;


		for(i=0;i<15;i++)
		{
			tmpOffsetValue[i] = 0;         //清零
		}
		adcAutoCount = 0;
		StateFlag.onceTimeAdcAutoAdjust = false;
	}
}

// 定标以后，对定标值进行判定，确定是否在合理范围内，避免定标失败。
// 背景：LCD增加了自动定标功能
// 功能：只用来判断交流量的定标结果
// 20150108 DSP REV6的电路板，采样电路1.5V中点偏差，输出电流校准值为2140.修改门限2200
void ZeroOffsetJudgment(void)			//还有一些通道没有添加
{
    Stru_Virtu_ZeroOffSETVAL *pAD = &VirtuZeroOffSETVAL;

	if(   (pAD->gridVoltHAOffset 		> zeroOffsetUpLimit)	\
		||(pAD->gridVoltHBOffset 		> zeroOffsetUpLimit)	\
		||(pAD->gridVoltHCOffset 		> zeroOffsetUpLimit)	\
		||(pAD->gridVoltLAOffset 	    > zeroOffsetUpLimit)	\
		||(pAD->gridVoltLBOffset 	    > zeroOffsetUpLimit)	\
		||(pAD->gridVoltLCOffset 	    > zeroOffsetUpLimit)	\
		||(pAD->gridMainIAOffset 		> zeroOffsetUpLimit)	\
		||(pAD->gridMainIBOffset 		> zeroOffsetUpLimit)	\
		||(pAD->gridMainICOffset 		> zeroOffsetUpLimit) 	\
		||(pAD->gridBypassIAOffset		< zeroOffsetDownLimit)	\
		||(pAD->gridBypassIBOffset 		< zeroOffsetDownLimit)	\
		||(pAD->gridBypassICOffset 		< zeroOffsetDownLimit)	 ){
		if(softwareFaultWord3.B.ESCCalibrarionFailureFlag == 0){
//			softwareFaultWord2.B.calibrarionFailureFlag = FaultDetect(SOE_GP_FAULT+21,CNT_CALIB_ZERO_SHIFT,0);
		}
	}else{
		SetFaultDelayCounter(CNT_CALIB_ZERO_SHIFT,0);
	}
}

/*
 * MATLAB 库中这样计算  U0 = 0；只要计算总电流有效值和基波电流有效值即可。
THD= Uhrms /U1rms
where:
Uhrms =  rms value of harmonics = sqrt( U2^2 + U3^2 + U4^2....) =  sqrt( Urms^2 -Uo^2- U1rms^2)
U1rms =  rms value of the fundamenrtal component
Uo        =  DC component = mean value
Urms  = True rms value including harmonics and DC component
*/
// 电流有效值的计算方法：积分法 0814
/* 解释
 * 假设同样波形的电压，接在纯电阻上，那么deltaT的时间产生热量为 U^2/R*deltaT
 * T时间加起来后，等效为  Urms^2/R*T
 * 得到Urms = sqrt(U1^2*deltaT + ....)/T
 */

void THDiCal(void)
{
	int16 i;
	float32 temp,tThd;
	STRU_HarmTHD *pC = HarmTHD;
	if(StateFlag.cntForTHDi)
	{
		for(i=0;i<HARM_CALU_NUM;i++){
			temp=2*(pC->im*pC->im + pC->re*pC->re);     //谐波平方+基波平方
			if(temp>(0.3*0.3)){		//小于0.3A显示为0
			tThd=100*sqrtf((pC->rms -temp)/temp);
			}else{
				tThd=0;
			}
			if(tThd>1000) tThd=1000;
			if(tThd<-1000) tThd=-1000;
			if((i>2)&&(i<6)&&(THD_K!=0))
			tThd = THD_K*tThd+THD_B;
			pC->THD=0.9*pC->THD + 0.1*tThd;		// 改为一直计算，采用滑动滤波的方式进行，可以避免小数来回跳动
			pC++;
		}
		temp=S1DIV3*(HarmTHD[TotalHarmDistorionGridCurrA].THD+HarmTHD[TotalHarmDistorionGridCurrB].THD+\
					 HarmTHD[TotalHarmDistorionGridCurrC].THD);
		if(temp > 1000)	temp = 1000;		if(temp <-1000)	temp = -1000;
		TotalHarmDistorionGridCurr = 0.9 * TotalHarmDistorionGridCurr + 0.1 * temp;
		TotalHarmDistorionLoadCurr = S1DIV3*(HarmTHD[TotalHarmDistorionLoadCurrA].THD+HarmTHD[TotalHarmDistorionLoadCurrB].THD+HarmTHD[TotalHarmDistorionLoadCurrC].THD);
		TotalHarmDistorionVolt = S1DIV3*(HarmTHD[TotalHarmDistorionVoltA].THD+HarmTHD[TotalHarmDistorionVoltB].THD+HarmTHD[TotalHarmDistorionVoltC].THD);
		StateFlag.cntForTHDi = 0;
	}
}
///*********************************************************************************************///
/* 函数功能：故障后，经过自检，若设备硬件安全，且电网符合启动要求，则自动启动
 * 自启标准：现场无人职守，地处偏僻，自启动无危害。
 * 启动标识位：   startingMethod   0自动  1手动
 * 实现方法：仅检测电网电压情况，一分钟后自启动。
 *     条件：限制于电网故障、电流过流、直流电压过压、模块过温
 * 停机要求：24小时内自启动超过N次，则取消自启动。N=10
 * 调用位置：main
 * 如果是LCD下发的停机指令，则需要立即停机。
 * 有个bug，仅存在调试过程中，运行过程中直接下发“自动”，会先进入空载运行状态，而后再进入补偿模式。
 */
void AutoStartInFault(void)
{
	if(StateFlag.startingMethod == 0){  	//自动
		switch(ESCFlagA.autoStFlag){
		case ORIGINAL_STATE_A:			//初始状态
         if(ESCFlagA.VoltageModeFlag == 0){
             if((ESCFlagA.ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))&&(ESCFlagA.ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))){  //上电两秒之后小继电器在动作;
                 if(ESCFlagA.RELAYCONTROLFlag == 1){
                     if((ESCFlagA.ByPassContactFlag == 1)&&(ESCFlagA.ContactorFeedBackFlag == 0)){
                         SET_GV_VOL_CTRL_A(1);
                         ESCFlagA.RELAYCONTROLFlag = 0;
                     }
                }
             }
            if((ESCFlagA.ESCCntMs.StartDelay>CNT_MS(60000))){//60秒
                /*********平时调试需要将其屏蔽掉,整体调试再打开***************/
                if((GET_BYPASS_FEEDBACK == 1)&&(GET_GV_VOL_CTRL_A == 1)&&(ESCBYRelayCNTA != 1)&&(ESCSicFaultCNTA != 1))
                {
                    ESCFlagA.BYFEEDBACKFLAG = 1;
                    SET_POWER_CTRL(1);
                    Delayus(TIME_WRITE_15VOLT_REDAY);
                    CntSec.StopDelay = 0;
                    ESCFlagA.autoStFlag = AUTO_DETECTION_STATE_A;
//                    SET_RUNNING_LED(0);
//                    if(  (StateEventFlag_A != STATE_EVENT_FAULT_A)&&\
//                         (StateEventFlag_B != STATE_EVENT_FAULT_B)&&\
//                         (StateEventFlag_C != STATE_EVENT_FAULT_C)  )
//                    {
//                        SET_FAULT_LED(0);
//                    }
                }
             }
        }else if(ESCFlagA.VoltageModeFlag == 1){
            if((ESCFlagA.ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))&&(ESCFlagA.ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))){  //上电两秒之后小继电器在动作;
                if(ESCFlagA.RELAYCONTROLFlag == 1){
                    if((ESCFlagA.ByPassContactFlag == 1)&&(ESCFlagA.ContactorFeedBackFlag == 0)){
                        SET_GV_VOL_CTRL_A(1);
                        ESCFlagA.RELAYCONTROLFlag = 0;
                    }
                }
            }
                if((ESCFlagA.ESCCntMs.StartDelay>CNT_MS(60000))){//60秒
                   /*********平时调试需要将其屏蔽掉,整体调试再打开***************/
                    if((GET_BYPASS_FEEDBACK == 1)&&(GET_GV_VOL_CTRL_A == 1)&&(ESCBYRelayCNTA != 1)&&(ESCSicFaultCNTA != 1))
                    {
                        ESCFlagA.BYFEEDBACKFLAG = 1;
                        SET_POWER_CTRL(1);
                        Delayus(TIME_WRITE_15VOLT_REDAY);
                        CntSec.StopDelay = 0;
                        ESCFlagA.autoStFlag = AUTO_DETECTION_STATE_A;
//                        SET_RUNNING_LED(0);
//                        if(  (StateEventFlag_A != STATE_EVENT_FAULT_A)&&\
//                             (StateEventFlag_B != STATE_EVENT_FAULT_B)&&\
//                             (StateEventFlag_C != STATE_EVENT_FAULT_C)  )
//                        {
//                            SET_FAULT_LED(0);
//                        }
                    }
             }
        }
		break;
		case AUTO_DETECTION_STATE_A:		//自检状态
//			if(!softwareFaultWord3.B.ESCCalibrarionFailureFlag)	//检测零偏
//			{
				if(ESCFlagA.realFaultFlag == 1){
					cntForRepFaultA++;                   // 当前重启了几次
					ESCFlagA.realFaultFlag = 0;
				}
				if((!ESCFlagA.onceRunStartFlag)||(StateEventFlag_A==STATE_EVENT_RUN_A)){
				        ESCFlagA.onceRunStartFlag = 1;
				        ESCFlagA.autoStFlag = TWEAK_STATE_A; 	//switch跳转到case:2
				}
//				StateFlag.resetFlag = 1;                    //复位
               if((ESCBYRelayCNTA != 1)&&(ESCSicFaultCNTA != 1)){
                   ESCFlagA.resetFlag = 1;
               }
//			}
		break;
		case TWEAK_STATE_A:			//模式转换
		    switch(StateEventFlag_A){
			case STATE_EVENT_STANDBY_A:
			     ESCFlagA.startFlag = 1;    //不是备用单元,就可自动运行
			break;
			case STATE_EVENT_RECHARGE_A:	ESCFlagA.ESCCntMs.SelfJc = 0;			 break;
            case STATE_EVENT_WAIT_A:      ESCFlagA.ESCCntMs.SelfJc = 0;            break;
			case STATE_EVENT_RUN_A:
//		    	I_ins_index = 0.005;    							//I_ins_index 不能大于0.01
				if(ESCFlagA.ESCCntMs.SelfJc > CNT_MS(3000))						// 调入正常状态
			    {
					ESCFlagA.autoStFlag = INSPECTION_STATE_A;
			    }
		    break;
			case STATE_EVENT_FAULT_A:
			    ESCFlagA.autoStFlag = ORIGINAL_STATE_A;
				ESCFlagA.realFaultFlag = 1;
				if(ESCFlagA.ESCCntMs.StartDelay>CNT_MS(7000))	ESCFlagA.ESCCntMs.StartDelay = CNT_MS(7000);        //同步可以报故障
		    break;
		    }
		break;

		case INSPECTION_STATE_A:			// 何时调入状态0，（自启动&&故障）
			if((StateEventFlag_A == STATE_EVENT_FAULT_A)||(StateEventFlag_A == STATE_EVENT_STANDBY_A))
			{
				if(StateEventFlag_A == STATE_EVENT_FAULT_A){
				    ESCFlagA.realFaultFlag = 1;
					if(ESCFlagA.ESCCntMs.StartDelay>CNT_MS(15000))	ESCFlagA.ESCCntMs.StartDelay = CNT_MS(15000);                        //同步可以报故障
			     }else{
			         ESCFlagA.realFaultFlag = 0;
			     }
				ESCFlagA.autoStFlag = ORIGINAL_STATE_A;
			}
		break;
		default:break;
		}

//		if(cntForAutoStIn30 <= 30){		//esc自启动后30分钟内重复故障跳闸。
//			if(softwareFaultWord3.B.ESCCalibrarionFailureFlag == 0){
//				softwareFaultWord3.B.ESCCalibrarionFailureFlag = FaultDetectExtCnt(SOE_GP_FAULT+38,10,cntForRepFaultA);	// 共计10次
//				if(softwareFaultWord3.B.ESCCalibrarionFailureFlag){
//					StateFlag.startingMethod = 1;        //严禁自启动，可通过人工下发自动启动标识进行赋值。
//					if(PWM_address == 0)		StateFlag.StateInstruction = 3;		//主机故障广告
//				}
//			}
//		}else{
//		    cntForAutoStIn30 = 0;
//			cntForRepFaultA = 0;
//		}

        switch(ESCFlagB.autoStFlag){
        case ORIGINAL_STATE_B:          //初始状态
         if(ESCFlagB.VoltageModeFlag == 0){
             if((ESCFlagB.ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))&&(ESCFlagB.ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))){  //上电两秒之后小继电器在动作;
                 if(ESCFlagB.RELAYCONTROLFlag == 1){
                     if((ESCFlagB.ByPassContactFlag == 1)&&(ESCFlagB.ContactorFeedBackFlag == 0)){
                         SET_GV_VOL_CTRL_B(1);
                         ESCFlagB.RELAYCONTROLFlag = 0;
                     }
                }
             }
            if((ESCFlagB.ESCCntMs.StartDelay>CNT_MS(60000))){//60秒
                /*********平时调试需要将其屏蔽掉,整体调试再打开***************/
                if((GET_BYPASS_FEEDBACK == 1)&&(GET_GV_VOL_CTRL_B == 1)&&(ESCBYRelayCNTB != 1)&&(ESCSicFaultCNTB != 1))
                {
                    ESCFlagB.BYFEEDBACKFLAG = 1;
                    SET_POWER_CTRL(1);
                    Delayus(TIME_WRITE_15VOLT_REDAY);
                    CntSec.StopDelay = 0;
                    ESCFlagB.autoStFlag = AUTO_DETECTION_STATE_B;
//                        SET_RUNNING_LED(0);
//                        if(  (StateEventFlag_A != STATE_EVENT_FAULT_A)&&\
//                             (StateEventFlag_B != STATE_EVENT_FAULT_B)&&\
//                             (StateEventFlag_C != STATE_EVENT_FAULT_C)  )
//                        {
//                            SET_FAULT_LED(0);
//                        }
                }
             }
        }else if(ESCFlagB.VoltageModeFlag == 1){
            if((ESCFlagB.ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))&&(ESCFlagB.ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))){  //上电两秒之后小继电器在动作;
                if(ESCFlagB.RELAYCONTROLFlag == 1){
                    if((ESCFlagB.ByPassContactFlag == 1)&&(ESCFlagB.ContactorFeedBackFlag == 0)){
                        SET_GV_VOL_CTRL_B(1);
                        ESCFlagB.RELAYCONTROLFlag = 0;
                    }
                }
            }
            if((ESCFlagB.ESCCntMs.StartDelay>CNT_MS(60000))){//60秒
               /*********平时调试需要将其屏蔽掉,整体调试再打开***************/
                if((GET_BYPASS_FEEDBACK == 1)&&(GET_GV_VOL_CTRL_B == 1)&&(ESCBYRelayCNTB != 1)&&(ESCSicFaultCNTB != 1))
                {
                    ESCFlagB.BYFEEDBACKFLAG = 1;
                    SET_POWER_CTRL(1);
                    Delayus(TIME_WRITE_15VOLT_REDAY);
                    CntSec.StopDelay = 0;
                    ESCFlagB.autoStFlag = AUTO_DETECTION_STATE_B;
//                    SET_RUNNING_LED(0);
//                    if(  (StateEventFlag_A != STATE_EVENT_FAULT_A)&&\
//                         (StateEventFlag_B != STATE_EVENT_FAULT_B)&&\
//                         (StateEventFlag_C != STATE_EVENT_FAULT_C)  )
//                    {
//                        SET_FAULT_LED(0);
//                    }
                }
            }
         }
        break;
        case AUTO_DETECTION_STATE_B:        //自检状态
//            if(!softwareFaultWord3.B.ESCCalibrarionFailureFlag)    //检测零偏
//            {
                if(ESCFlagB.realFaultFlag == 1){
                    cntForRepFaultB++;                   // 当前重启了几次
                    ESCFlagB.realFaultFlag = 0;
                }
                if((!ESCFlagB.onceRunStartFlag)||(StateEventFlag_B==STATE_EVENT_RUN_B)){
                        ESCFlagB.onceRunStartFlag = 1;
                        ESCFlagB.autoStFlag = TWEAK_STATE_B;    //switch跳转到case:2
                }
//                StateFlag.resetFlag = 1;                    //复位
                if((ESCBYRelayCNTB != 1)&&(ESCSicFaultCNTB != 1)){
                    ESCFlagB.resetFlag = 1;
                }
//            }
        break;
        case TWEAK_STATE_B:         //模式转换
            switch(StateEventFlag_B){
            case STATE_EVENT_STANDBY_B:
                 ESCFlagB.startFlag = 1;    //不是备用单元,就可自动运行
            break;
            case STATE_EVENT_RECHARGE_B:    ESCFlagB.ESCCntMs.SelfJc = 0;            break;
            case STATE_EVENT_WAIT_B:      ESCFlagB.ESCCntMs.SelfJc = 0;            break;
            case STATE_EVENT_RUN_B:
//              I_ins_index = 0.005;                                //I_ins_index 不能大于0.01
                if(ESCFlagB.ESCCntMs.SelfJc > CNT_MS(3000))                     // 调入正常状态
                {
                    ESCFlagB.autoStFlag = INSPECTION_STATE_B;
                }
            break;
            case STATE_EVENT_FAULT_B:
                ESCFlagB.autoStFlag = ORIGINAL_STATE_B;
                ESCFlagB.realFaultFlag = 1;
                if(ESCFlagB.ESCCntMs.StartDelay>CNT_MS(7000))   ESCFlagB.ESCCntMs.StartDelay = CNT_MS(7000);        //同步可以报故障
            break;
            }
        break;

        case INSPECTION_STATE_B:            // 何时调入状态0，（自启动&&故障）
            if((StateEventFlag_B == STATE_EVENT_FAULT_B)||(StateEventFlag_B == STATE_EVENT_STANDBY_B))
            {
                if(StateEventFlag_B == STATE_EVENT_FAULT_B){
                    ESCFlagB.realFaultFlag = 1;
                    if(ESCFlagB.ESCCntMs.StartDelay>CNT_MS(15000))  ESCFlagB.ESCCntMs.StartDelay = CNT_MS(15000);                        //同步可以报故障
                 }else{
                     ESCFlagB.realFaultFlag = 0;
                 }
                ESCFlagB.autoStFlag = ORIGINAL_STATE_B;
            }
        break;
        default:break;
        }
         switch(ESCFlagC.autoStFlag){
         case ORIGINAL_STATE_C:          //初始状态
          if(ESCFlagC.VoltageModeFlag == 0){
              if((ESCFlagC.ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))&&(ESCFlagC.ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))){  //上电两秒之后小继电器在动作;
                  if(ESCFlagC.RELAYCONTROLFlag == 1){
                      if((ESCFlagC.ByPassContactFlag == 1)&&(ESCFlagC.ContactorFeedBackFlag == 0)){
                          SET_GV_VOL_CTRL_C(1);
                          ESCFlagC.RELAYCONTROLFlag = 0;
                      }
                 }
              }
             if((ESCFlagC.ESCCntMs.StartDelay>CNT_MS(60000))){//60秒
                 /*********平时调试需要将其屏蔽掉,整体调试再打开***************/
                 if((GET_BYPASS_FEEDBACK == 1)&&(GET_GV_VOL_CTRL_C == 1)&&(ESCBYRelayCNTC != 1)&&(ESCSicFaultCNTC != 1))
                 {
                     ESCFlagC.BYFEEDBACKFLAG = 1;
                     SET_POWER_CTRL(1);
                     Delayus(TIME_WRITE_15VOLT_REDAY);
                     CntSec.StopDelay = 0;
                     ESCFlagC.autoStFlag = AUTO_DETECTION_STATE_C;
//                     SET_RUNNING_LED(0);
//                     if(  (StateEventFlag_A != STATE_EVENT_FAULT_A)&&\
//                          (StateEventFlag_B != STATE_EVENT_FAULT_B)&&\
//                          (StateEventFlag_C != STATE_EVENT_FAULT_C)  )
//                     {
//                         SET_FAULT_LED(0);
//                     }
                 }
              }
         }else if(ESCFlagC.VoltageModeFlag == 1){
             if((ESCFlagC.ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))&&(ESCFlagC.ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))){  //上电两秒之后小继电器在动作;
                 if(ESCFlagC.RELAYCONTROLFlag == 1){
                     if((ESCFlagC.ByPassContactFlag == 1)&&(ESCFlagC.ContactorFeedBackFlag == 0)){
                         SET_GV_VOL_CTRL_C(1);
                         ESCFlagC.RELAYCONTROLFlag = 0;
                     }
                 }
             }
             if((ESCFlagC.ESCCntMs.StartDelay>CNT_MS(60000))){//60秒
                /*********平时调试需要将其屏蔽掉,整体调试再打开***************/
                 if((GET_BYPASS_FEEDBACK == 1)&&(GET_GV_VOL_CTRL_C == 1)&&(ESCBYRelayCNTC != 1)&&(ESCSicFaultCNTC != 1))
                 {
                     ESCFlagC.BYFEEDBACKFLAG = 1;
                     SET_POWER_CTRL(1);
                     Delayus(TIME_WRITE_15VOLT_REDAY);
                     CntSec.StopDelay = 0;
                     ESCFlagC.autoStFlag = AUTO_DETECTION_STATE_C;
//                     SET_RUNNING_LED(0);
//                     if(  (StateEventFlag_A != STATE_EVENT_FAULT_A)&&\
//                          (StateEventFlag_B != STATE_EVENT_FAULT_B)&&\
//                          (StateEventFlag_C != STATE_EVENT_FAULT_C)  )
//                     {
//                         SET_FAULT_LED(0);
//                     }
                 }
              }
         }
         break;
         case AUTO_DETECTION_STATE_C:        //自检状态
//             if(!softwareFaultWord3.B.ESCCalibrarionFailureFlag)    //检测零偏
//             {
                 if(ESCFlagC.realFaultFlag == 1){
                     cntForRepFaultC++;                   // 当前重启了几次
                     ESCFlagC.realFaultFlag = 0;
                 }
                 if((!ESCFlagC.onceRunStartFlag)||(StateEventFlag_C==STATE_EVENT_RUN_C)){
                         ESCFlagC.onceRunStartFlag = 1;
                         ESCFlagC.autoStFlag = TWEAK_STATE_C;    //switch跳转到case:2
                 }
//                 StateFlag.resetFlag = 1;                    //复位
                 if((ESCBYRelayCNTC != 1)&&(ESCSicFaultCNTC != 1)){
                     ESCFlagC.resetFlag = 1;
                 }
//             }
         break;
         case TWEAK_STATE_C:         //模式转换
             switch(StateEventFlag_C){
             case STATE_EVENT_STANDBY_C:
                  ESCFlagC.startFlag = 1;    //不是备用单元,就可自动运行
             break;
             case STATE_EVENT_RECHARGE_C:    ESCFlagC.ESCCntMs.SelfJc = 0;            break;
             case STATE_EVENT_WAIT_C:      ESCFlagC.ESCCntMs.SelfJc = 0;            break;
             case STATE_EVENT_RUN_C:
 //              I_ins_index = 0.005;                                //I_ins_index 不能大于0.01
                 if(ESCFlagC.ESCCntMs.SelfJc > CNT_MS(3000))                     // 调入正常状态
                 {
                     ESCFlagC.autoStFlag = INSPECTION_STATE_C;
                 }
             break;
             case STATE_EVENT_FAULT_C:
                 ESCFlagC.autoStFlag = ORIGINAL_STATE_C;
                 ESCFlagC.realFaultFlag = 1;
                 if(ESCFlagC.ESCCntMs.StartDelay>CNT_MS(7000))   ESCFlagC.ESCCntMs.StartDelay = CNT_MS(7000);        //同步可以报故障
             break;
             }
         break;

         case INSPECTION_STATE_C:            // 何时调入状态0，（自启动&&故障）
             if((StateEventFlag_C == STATE_EVENT_FAULT_C)||(StateEventFlag_C == STATE_EVENT_STANDBY_C))
             {
                 if(StateEventFlag_C == STATE_EVENT_FAULT_C){
                     ESCFlagC.realFaultFlag = 1;
                     if(ESCFlagC.ESCCntMs.StartDelay>CNT_MS(15000))  ESCFlagC.ESCCntMs.StartDelay = CNT_MS(15000);                        //同步可以报故障
                  }else{
                      ESCFlagC.realFaultFlag = 0;
                  }
                 ESCFlagC.autoStFlag = ORIGINAL_STATE_C;
             }
         break;
         default:break;
         }
	}else{ 										//手动
	    ESCFlagA.autoStFlag = ORIGINAL_STATE_A;
	    ESCFlagB.autoStFlag = ORIGINAL_STATE_B;
	    ESCFlagC.autoStFlag = ORIGINAL_STATE_C;
		cntForRepFaultA = 0;
		cntForRepFaultB = 0;
		cntForRepFaultC = 0;
	}
}

/*
 * 功能：若A相主路磁保持继电器闭合，则将其断开。WY
 */
void ExecuteOpenContact_A(void)
{
	if (GET_MAIN_CONTACT_ACTION_A == ESC_ACTION_ENABLE)
	{
		SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_DISABLE);
	}
}

/*
 * 功能：若B相主路磁保持继电器闭合，则将其断开。WY
 */
void ExecuteOpenContact_B(void)
{
	if (GET_MAIN_CONTACT_ACTION_B == ESC_ACTION_ENABLE)
	{
		SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_DISABLE);
	}
}

/*
 * 功能：若C相主路磁保持继电器闭合，则将其断开。WY
 */
void ExecuteOpenContact_C(void)
{
	if (GET_MAIN_CONTACT_ACTION_C == ESC_ACTION_ENABLE)
	{
		SET_MAIN_CONTACT_ACTION_C(ESC_ACTION_DISABLE);
	}
}


void initStartVar(void){
	PIVolA.i6 = 0;
	PIVolA.i10 = 0;
	PIVolB.i6 = 0;
	PIVolB.i10 = 0;
	PIVolC.i6 = 0;
	PIVolC.i10 = 0;
	UdcPICtrl.i10 = 0;
	UdcPIBalance.i10 = 0;
    GridCurrPICtrlA.i10     =0;
    GridCurrPICtrlB.i10     =0;
    GridCurrPICtrlC.i10     =0;
    VolttargetCorrPIA.i10     =0;
    VolttargetCorrPIB.i10     =0;
    VolttargetCorrPIC.i10     =0;
//	I_ins_index = 0;
	UdcBalanceCurr=0;
	currentRefD=0;												//停机后清零,因为自动启动也需要用到逆变侧电流
	currentRefQ=0;
	currentRefQa=0;
	currentRefQb=0;
	currentRefQc=0;
	negCurCompD = 0;
	negCurCompQ = 0;
	reactPowerCompCurQa = 0;
	reactPowerCompCurQb = 0;
	reactPowerCompCurQc = 0;
	CntSec.VolComDelay = 0;
	CntSec.WaitDelay = 0;
}

void SetResetExecute(void)
{
	softwareFaultWord1.all = 0;
	softwareFaultWord2.all = 0;
	softwareFaultWord3.all = 0;
	softwareFaultWord4.all = 0;
	softwareFaultWord5.all = 0;

	StateFlag.isFault  = 0;
	StateEventFlag_A = STATE_EVENT_STANDBY_A;
	StateEventFlag_B = STATE_EVENT_STANDBY_B;
	StateEventFlag_C = STATE_EVENT_STANDBY_C;
}

void CorrectingAD(void)
{
        VirtulADStruVAL *pAD = &VirtulADVAL;
        pAD->GridHVoltA = (int16 *)&ADC_RU_HVA;   //GridHVoltA指针指向AdcaResultRegs.ADCRESULT2存储器的内存地址
        pAD->GridHVoltB = (int16 *)&ADC_RU_HVB;
        pAD->GridHVoltC = (int16 *)&ADC_RU_HVC;
        pAD->GridLVoltA = (int16 *)&ADC_RU_LVA;
        pAD->GridLVoltB = (int16 *)&ADC_RU_LVB;
        pAD->GridLVoltC = (int16 *)&ADC_RU_LVC;
        pAD->ADCUDCA    = (int16 *)&ADC_DC_UA;
        pAD->ADCUDCB    = (int16 *)&ADC_DC_UB;
        pAD->ADCUDCC    = (int16 *)&ADC_DC_UC;

		pPwmV1  = &PwmVa;
		pPwmV2  = &PwmVb;
		pPwmV3  = &PwmVc;
		pPwmVN1 = &PwmVaN;
		pPwmVN2 = &PwmVbN;
		pPwmVN3 = &PwmVcN;

}

void CorrectingCT(void)
{
    VirtulADStruVAL *pAD = &VirtulADVAL;
    pAD->GridMainCurA    = (int16 *)&ADC_RU_IMA;
    pAD->GridMainCurB    = (int16 *)&ADC_RU_IMB;
    pAD->GridMainCurC    = (int16 *)&ADC_RU_IMC;
    pAD->GridBypassCurA  = (int16 *)&ADC_RU_IBYA;
    pAD->GridBypassCurB  = (int16 *)&ADC_RU_IBYB;
    pAD->GridBypassCurC  = (int16 *)&ADC_RU_IBYC;

}

void DirectionCT(void)
{
	if((FactorySet.Infer.B.DirectionCT/4)==1)		VirtulAD.LoadCurRatioA =  transfRatio * 0.003233651 ;   //A
	else											VirtulAD.LoadCurRatioA = -transfRatio * 0.003233651 ;  //-A
	if((FactorySet.Infer.B.DirectionCT%4/2)==1)		VirtulAD.LoadCurRatioB =  transfRatio * 0.003233651 ;   //B
	else											VirtulAD.LoadCurRatioB = -transfRatio * 0.003233651 ;  //-B
	if((FactorySet.Infer.B.DirectionCT%2)==1)		VirtulAD.LoadCurRatioC =  transfRatio * 0.003233651 ;   //C
	else											VirtulAD.LoadCurRatioC = -transfRatio * 0.003233651 ;  //-C
}

void JudgePowerGridCT(void)
{
	AngleJudgePowerGrid(0,1,2,0);
	if(!StateFlag.PhaseSequeJudSucceedFlag)		AngleJudgePowerGrid(0,2,1,1);
	if(!StateFlag.PhaseSequeJudSucceedFlag) 	AngleJudgePowerGrid(1,0,2,2);
	if(!StateFlag.PhaseSequeJudSucceedFlag)		AngleJudgePowerGrid(2,0,1,3);
	if(!StateFlag.PhaseSequeJudSucceedFlag)		AngleJudgePowerGrid(1,2,0,4);
	if(!StateFlag.PhaseSequeJudSucceedFlag)		AngleJudgePowerGrid(2,1,0,5);

	if(StateFlag.PhaseSequeJudSucceedFlag && (!Information_Structure.Correction.B.DirectionCTFailFlag)){
		Uint16 i,*p;
		p = (Uint16 *)&GenCorr;
		for(i=0;i<sizeof(GenCorr);i++)			*p++=0;
		Information_Structure.Correction.B.PhaseSequenResult = 2;
		Information_Structure.Correction.B.VoltPhaseSequen = FactorySet.Infer.B.VoltagePhaseSequence;
		Information_Structure.Correction.B.PhaseSequenCT = FactorySet.Infer.B.PhaseSequenceCT;
		Information_Structure.Correction.B.DirectionCT = FactorySet.Infer.B.DirectionCT;
        ESCFlagA.stopFlag = 1;          //停止命令
        ESCFlagB.stopFlag = 1;
        ESCFlagC.stopFlag = 1;
		StateFlag.SequenceAutoFlag = 0;
		StateFlag.ManufacturerParameters = true;					//存到EEPROM并更新
		UserPreferencesRefresh();									//还原用户设置
	}else{
		SequenceJudgeTimes++;
		if(SequenceJudgeTimes<3){									//尝试多次校正
			Information_Structure.Correction.B.DirectionCTFailFlag=0;
			StateFlag.MakeEnergyFlag=1;
			StateFlag.CurrACountFlag=1;
			StateFlag.CurrBCountFlag=1;
			StateFlag.CurrCCountFlag=1;
			CntMs.HarmonicSendCount = 0;
		}else{														 //相序校正失败
            ESCFlagA.stopFlag = 1;          //停止命令
            ESCFlagB.stopFlag = 1;
            ESCFlagC.stopFlag = 1;
			StateFlag.SequenceAutoFlag = 0;
			Information_Structure.Correction.B.PhaseSequenResult = 1;
			Information_Structure.Correction.B.PhaseSequenCTFailFlag = ~StateFlag.PhaseSequeJudSucceedFlag;
			UserPreferencesRefresh();								 //还原用户设置
		}
	}
}

void JudgeLoadCT(void)
{
	switch(StateFlag.PowerFactor){
		case 0:
		AngleJudgeLoad(0,1,2,0);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeLoad(0,2,1,1);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeLoad(1,0,2,2);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeLoad(2,0,1,3);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeLoad(1,2,0,4);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeLoad(2,1,0,5);
		break;
		case 1:
		AngleJudgeInductanceLoad(0,1,2,0);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeInductanceLoad(0,2,1,1);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeInductanceLoad(1,0,2,2);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeInductanceLoad(2,0,1,3);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeInductanceLoad(1,2,0,4);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeInductanceLoad(2,1,0,5);
		break;
		case 2:
		AngleJudgeCapacitanceLoad(0,1,2,0);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeCapacitanceLoad(0,2,1,1);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeCapacitanceLoad(1,0,2,2);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeCapacitanceLoad(2,0,1,3);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeCapacitanceLoad(1,2,0,4);
		if(!StateFlag.PhaseSequeJudSucceedFlag)	AngleJudgeCapacitanceLoad(2,1,0,5);
		break;
		default:break;
	}

	if(StateFlag.PhaseSequeJudSucceedFlag){
		Information_Structure.Correction.B.VoltPhaseSequen = FactorySet.Infer.B.VoltagePhaseSequence;
		Information_Structure.Correction.B.PhaseSequenCT = FactorySet.Infer.B.PhaseSequenceCT;
		Information_Structure.Correction.B.DirectionCT = FactorySet.Infer.B.DirectionCT;
		Information_Structure.Correction.B.PhaseSequenResult = 2;
		StateFlag.SequenceAutoFlag = 0;
		StateFlag.ManufacturerParameters = true;					//存到EEPROM并更新
		UserPreferencesRefresh();									//还原用户设置
	}else{
		SequenceJudgeTimes++;
		if(SequenceJudgeTimes<9){
			StateFlag.CurrACountFlag=1;
			StateFlag.CurrBCountFlag=1;
			StateFlag.CurrCCountFlag=1;
		}else{
			StateFlag.SequenceAutoFlag = 0;
			Information_Structure.Correction.B.PhaseSequenResult = 1;
			Information_Structure.Correction.B.PhaseSequenCTFailFlag = ~StateFlag.PhaseSequeJudSucceedFlag;
			UserPreferencesRefresh();								//还原用户设置
		}
	}
}

void AngleJudgePowerGrid(int N1,int N2,int N3,Uint16 number)	//电网侧
{
    float coefficient1,coefficient2,coefficient3;
    coefficient1 = SeqJud[N2]/SeqJud[N3];
    coefficient2 = SeqJud[N1]/SeqJud[N3];
    coefficient3 = SeqJud[N1]/SeqJud[N2];
	switch(FactorySet.Infer.B.DefectCT){
	case 0:
        if((coefficient1>1.5)&&(coefficient1)&&(coefficient2>2.5)&&(coefficient2<3.5))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT=number;
			if((DirJud[N1]<120)&&(DirJud[N1]>60))			FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 3;
			else if((DirJud[N1]<300)&&(DirJud[N1]>240)) 	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 4;
			else	 Information_Structure.Correction.B.DirectionCTFailFlag = 1;

			if((DirJud[N2]<240)&&(DirJud[N2]>180))			FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 5;
			else if((DirJud[N2]<60)&&(DirJud[N2]>0))		FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 2;
			else	 Information_Structure.Correction.B.DirectionCTFailFlag = 1;

			if((DirJud[N3]<360)&&(DirJud[N3]>300))			FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 6;
			else if((DirJud[N3]<180)&&(DirJud[N3]>120))		FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 1;
			else	 Information_Structure.Correction.B.DirectionCTFailFlag = 1;
		}
	break;
	case 1:
        if((coefficient3>1.25)&&(coefficient3<1.75))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT=number;
			if((DirJud[N1]<120)&&(DirJud[N1]>60))			FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 3;
			else if((DirJud[N1]<300)&&(DirJud[N1]>240)) 	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 4;
			else	 Information_Structure.Correction.B.DirectionCTFailFlag = 1;

			if((DirJud[N2]<240)&&(DirJud[N2]>180))			FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 5;
			else if((DirJud[N2]<60)&&(DirJud[N2]>0))		FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 2;
			else	 Information_Structure.Correction.B.DirectionCTFailFlag = 1;
		}
	break;
	case 2:
	    if((coefficient2>2.5)&&(coefficient2<3.5))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT=number;
			if((DirJud[N1]<120)&&(DirJud[N1]>60))			FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 3;
			else if((DirJud[N1]<300)&&(DirJud[N1]>240)) 	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 4;
			else	 Information_Structure.Correction.B.DirectionCTFailFlag = 1;

			if((DirJud[N3]<360)&&(DirJud[N3]>300))			FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 6;
			else if((DirJud[N3]<180)&&(DirJud[N3]>120))		FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 1;
			else	 Information_Structure.Correction.B.DirectionCTFailFlag = 1;
		}
	break;
	case 3:
	    if((coefficient1>1.5)&&(coefficient1<2.5))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT=number;
			if((DirJud[N2]<240)&&(DirJud[N2]>180))			FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 5;
			else if((DirJud[N2]<60)&&(DirJud[N2]>0))		FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 2;
			else	 Information_Structure.Correction.B.DirectionCTFailFlag = 1;

			if((DirJud[N3]<360)&&(DirJud[N3]>300))			FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 6;
			else if((DirJud[N3]<180)&&(DirJud[N3]>120))		FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 1;
			else	 Information_Structure.Correction.B.DirectionCTFailFlag = 1;
		}
	break;
	default:break;
	}
}

void AngleJudgeLoad(int N1,int N2,int N3,Uint16 number)		//负载侧功率因数大于0.5
{
	switch(FactorySet.Infer.B.DefectCT){
	case 0:
		if((((DirJud[N1]<360)&&(DirJud[N1]>290))||((DirJud[N1]<10)&&(DirJud[N1]>=0))||((DirJud[N1]<190)&&(DirJud[N1]>110)))\
			&&(((DirJud[N2]<250)&&(DirJud[N2]>170))||((DirJud[N2]<70)&&(DirJud[N2]>=0))||((DirJud[N2]<360)&&(DirJud[N2]>350)))\
			&&(((DirJud[N3]<130)&&(DirJud[N3]>50))||((DirJud[N3]<310)&&(DirJud[N3]>230))))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT = number;
			if((DirJud[N1]<190)&&(DirJud[N1]>110)) 	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 4;
			else									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 3;
			if((DirJud[N2]<250)&&(DirJud[N2]>170))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 5;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 2;
			if((DirJud[N3]<130)&&(DirJud[N3]>50))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 6;
			else									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 1;
		}
	break;
	case 1:
		if((((DirJud[N1]<360)&&(DirJud[N1]>290))||((DirJud[N1]<10)&&(DirJud[N1]>=0))||((DirJud[N1]<190)&&(DirJud[N1]>110)))\
			&&(((DirJud[N2]<250)&&(DirJud[N2]>170))||((DirJud[N2]<70)&&(DirJud[N2]>=0))||((DirJud[N2]<360)&&(DirJud[N2]>350))))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT = number;
			if((DirJud[N1]<190)&&(DirJud[N1]>110)) 	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 4;
			else								   	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 3;
			if((DirJud[N2]<250)&&(DirJud[N2]>170))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 5;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 2;
		}
	break;
	case 2:
		if((((DirJud[N1]<360)&&(DirJud[N1]>290))||((DirJud[N1]<10)&&(DirJud[N1]>=0))||((DirJud[N1]<190)&&(DirJud[N1]>110)))\
			&&(((DirJud[N3]<130)&&(DirJud[N3]>50))||((DirJud[N3]<310)&&(DirJud[N3]>230))))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT = number;
			if((DirJud[N1]<190)&&(DirJud[N1]>110)) 	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 4;
			else								   	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 3;
			if((DirJud[N3]<130)&&(DirJud[N3]>50))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 6;
			else									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 1;
		}
	break;
	case 3:
		if((((DirJud[N2]<250)&&(DirJud[N2]>170))||((DirJud[N2]<70)&&(DirJud[N2]>=0))||((DirJud[N2]<360)&&(DirJud[N2]>350)))\
			&&(((DirJud[N3]<130)&&(DirJud[N3]>50))||((DirJud[N3]<310)&&(DirJud[N3]>230))))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT = number;
			if((DirJud[N2]<250)&&(DirJud[N2]>170))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 5;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 2;
			if((DirJud[N3]<130)&&(DirJud[N3]>50))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 6;
			else									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 1;
		}
	break;
	default:break;
	}
}

void AngleJudgeInductanceLoad(int N1,int N2,int N3,Uint16 number)	//负载侧功率因数小于0.5
{
	switch(FactorySet.Infer.B.DefectCT){
	case 0:
		if((((DirJud[N1]<310)&&(DirJud[N1]>260))||((DirJud[N1]<130)&&(DirJud[N1]>80)))\
			&&(((DirJud[N2]<190)&&(DirJud[N2]>140))||((DirJud[N2]<360)&&(DirJud[N2]>320))||((DirJud[N2]<10)&&(DirJud[N2]>=0)))\
			&&(((DirJud[N3]<70)&&(DirJud[N3]>20))||((DirJud[N3]<250)&&(DirJud[N3]>200))))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT = number;
			if((DirJud[N1]<310)&&(DirJud[N1]>260))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 3;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 4;
			if((DirJud[N2]<190)&&(DirJud[N2]>140))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 5;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 2;
			if((DirJud[N3]<70)&&(DirJud[N3]>20))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 6;
			else									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 1;
		}
	break;
	case 1:
		if((((DirJud[N1]<310)&&(DirJud[N1]>260))||((DirJud[N1]<130)&&(DirJud[N1]>80)))\
			&&(((DirJud[N2]<190)&&(DirJud[N2]>140))||((DirJud[N2]<360)&&(DirJud[N2]>320))||((DirJud[N2]<10)&&(DirJud[N2]>=0))))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT = number;
			if((DirJud[N1]<310)&&(DirJud[N1]>260))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 3;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 4;
			if((DirJud[N2]<190)&&(DirJud[N2]>140))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 5;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 2;
		}
	break;
	case 2:
		if((((DirJud[N1]<310)&&(DirJud[N1]>260))||((DirJud[N1]<130)&&(DirJud[N1]>80)))\
			&&(((DirJud[N3]<70)&&(DirJud[N3]>20))||((DirJud[N3]<250)&&(DirJud[N3]>200))))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT = number;
			if((DirJud[N1]<310)&&(DirJud[N1]>260))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 3;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 4;
			if((DirJud[N3]<70)&&(DirJud[N3]>20))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 6;
			else									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 1;
		}
	break;
	case 3:
		if((((DirJud[N2]<190)&&(DirJud[N2]>140))||((DirJud[N2]<360)&&(DirJud[N2]>320))||((DirJud[N2]<10)&&(DirJud[N2]>=0)))\
			&&(((DirJud[N3]<70)&&(DirJud[N3]>20))||((DirJud[N3]<250)&&(DirJud[N3]>200))))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT = number;
			if((DirJud[N2]<190)&&(DirJud[N2]>140))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 5;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 2;
			if((DirJud[N3]<70)&&(DirJud[N3]>20))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 6;
			else									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 1;
		}
	break;
	default:break;
	}
}

void AngleJudgeCapacitanceLoad(int N1,int N2,int N3,Uint16 number)	//负载侧容性负载(-0.5,0)
{
	switch(FactorySet.Infer.B.DefectCT){
	case 0:
		if((((DirJud[N1]<100)&&(DirJud[N1]>30))||((DirJud[N1]<280)&&(DirJud[N1]>210)))\
			&&(((DirJud[N2]<340)&&(DirJud[N2]>270))||((DirJud[N2]<160)&&(DirJud[N2]>90)))\
			&&(((DirJud[N3]<220)&&(DirJud[N3]>150))||((DirJud[N3]<40)&&(DirJud[N3]>=0))||((DirJud[N3]<360)&&(DirJud[N3]>330))))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT = number;
			if((DirJud[N1]<100)&&(DirJud[N1]>30))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 3;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 4;
			if((DirJud[N2]<340)&&(DirJud[N2]>270))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 5;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 2;
			if((DirJud[N3]<220)&&(DirJud[N3]>150))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 6;
			else									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 1;
		}
	break;
	case 1:
		if((((DirJud[N1]<100)&&(DirJud[N1]>30))||((DirJud[N1]<280)&&(DirJud[N1]>210)))\
			&&(((DirJud[N2]<340)&&(DirJud[N2]>270))||((DirJud[N2]<160)&&(DirJud[N2]>90))))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT = number;
			if((DirJud[N1]<100)&&(DirJud[N1]>30))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 3;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 4;
			if((DirJud[N2]<340)&&(DirJud[N2]>270))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 5;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 2;
		}
	break;
	case 2:
		if((((DirJud[N1]<100)&&(DirJud[N1]>30))||((DirJud[N1]<280)&&(DirJud[N1]>210)))\
			&&(((DirJud[N3]<220)&&(DirJud[N3]>150))||((DirJud[N3]<40)&&(DirJud[N3]>=0))||((DirJud[N3]<360)&&(DirJud[N3]>330))))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT = number;
			if((DirJud[N1]<100)&&(DirJud[N1]>30))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 3;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 4;
			if((DirJud[N3]<220)&&(DirJud[N3]>150))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 6;
			else									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 1;
		}
	break;
	case 3:
		if((((DirJud[N2]<340)&&(DirJud[N2]>270))||((DirJud[N2]<160)&&(DirJud[N2]>90)))\
			&&(((DirJud[N3]<220)&&(DirJud[N3]>150))||((DirJud[N3]<40)&&(DirJud[N3]>=0))||((DirJud[N3]<360)&&(DirJud[N3]>330))))
		{
			StateFlag.PhaseSequeJudSucceedFlag = 1;
			FactorySet.Infer.B.PhaseSequenceCT = number;
			if((DirJud[N2]<340)&&(DirJud[N2]>270))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 5;
			else 									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 2;
			if((DirJud[N3]<220)&&(DirJud[N3]>150))	FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  & 6;
			else									FactorySet.Infer.B.DirectionCT = FactorySet.Infer.B.DirectionCT  | 1;
		}
	break;
	default:break;
	}
}


extern int maxCh,minCh;
extern float UnCurrData[];
void PowerReactStateRefresh(void)
{
    float MaxGridCurRms,MaxLoadCurRms;
	if(StateFlag.reactPrCompFlag&&(StateFlag.VolSurTimeFlag==0)){
		APFReactPowerOutA= (GridVoltDA*CapReactPowerCurQPA*POWER_CONV_RATIO_CAP)/(MU_MultRatio);//APF应该发出的无功 = 电容和负载的无功差
		APFReactPowerOutB= (GridVoltDB*CapReactPowerCurQPB*POWER_CONV_RATIO_CAP)/(MU_MultRatio);
		APFReactPowerOutC= (GridVoltDC*CapReactPowerCurQPC*POWER_CONV_RATIO_CAP)/(MU_MultRatio);
		APFReactPowerOut = (CapreactPowerCurQ * GridFundaVoltD * POWER_CONV_RATIO_CAP)/(MU_MultRatio);//APF发出的无功量//指令
		CapStartCapcity = (CapStartLimit*(SQRT3) * GridFundaVoltD * POWER_CONV_RATIO_CAP)/(MU_MultRatio);
	}else if((!StateFlag.reactPrCompFlag)&&(StateFlag.VolSurTimeFlag==0)){
		APFReactPowerOutA = -LoadRealCurQA*GridVoltDA*POWER_CONV_RATIO_CAP;
		APFReactPowerOutB = -LoadRealCurQB*GridVoltDB*POWER_CONV_RATIO_CAP;
		APFReactPowerOutC = -LoadRealCurQC*GridVoltDC*POWER_CONV_RATIO_CAP;
		APFReactPowerOut  = -LoadFundaCurQ*GridFundaVoltD*POWER_CONV_RATIO_CAP;
	}else{
		APFReactPowerOutA = APFReactPowerOutB = APFReactPowerOutC = 0;
		APFReactPowerOut = CapStartCapcity = 0;
	}
//	电网总无功
	GridActPower  =-( GridFundaVoltD*GridFundaCurD+GridFundaVoltQ*GridFundaCurQ )*(POWER_CONV_RATIO*SQRT3);
	GridReactPower=-( GridFundaVoltQ*GridFundaCurD-GridFundaVoltD*GridFundaCurQ )*(POWER_CONV_RATIO*SQRT3);
	GridApparentPower = sqrt(GridActPower*GridActPower+GridReactPower*GridReactPower);
//	电网侧
	GridActPowerPhA  = GridVoltDA*GridRealCurDA*POWER_CONV_RATIO_CAP;
	GridReactPowerPhA= GridVoltDA*GridRealCurQA*POWER_CONV_RATIO_CAP;
	GridApparentPowerPhA = sqrt(GridActPowerPhA*GridActPowerPhA+GridReactPowerPhA*GridReactPowerPhA);

	GridActPowerPhB  = GridVoltDB*GridRealCurDB*POWER_CONV_RATIO_CAP;
	GridReactPowerPhB= GridVoltDB*GridRealCurQB*POWER_CONV_RATIO_CAP;
	GridApparentPowerPhB = sqrt(GridActPowerPhB*GridActPowerPhB+GridReactPowerPhB*GridReactPowerPhB);

	GridActPowerPhC  = GridVoltDC*GridRealCurDC*POWER_CONV_RATIO_CAP;
	GridReactPowerPhC= GridVoltDC*GridRealCurQC*POWER_CONV_RATIO_CAP;
	GridApparentPowerPhC = sqrt(GridActPowerPhC*GridActPowerPhC+GridReactPowerPhC*GridReactPowerPhC);

	GridCosFi = GridActPower/GridApparentPower;
	GridCosFiPhA = GridActPowerPhA/GridApparentPowerPhA;
	GridCosFiPhB = GridActPowerPhB/GridApparentPowerPhB;
	GridCosFiPhC = GridActPowerPhC/GridApparentPowerPhC;
	if(GridCosFi >=1)			GridCosFi = 1;
	if(GridCosFi <= -1)			GridCosFi = -1;
	if(GridReactPower<-10)		GridCosFi=-GridCosFi;
	if(GridCosFiPhA >=1)		GridCosFiPhA = 1;
	if(GridCosFiPhA <= -1)		GridCosFiPhA = -1;
	if(GridReactPowerPhA<-5)	GridCosFiPhA=-GridCosFiPhA;
	if(GridCosFiPhB >=1)		GridCosFiPhB = 1;
	if(GridCosFiPhB <= -1)		GridCosFiPhB = -1;
	if(GridReactPowerPhB<-5)	GridCosFiPhB=-GridCosFiPhB;
	if(GridCosFiPhC >=1)		GridCosFiPhC = 1;
	if(GridCosFiPhC <= -1)		GridCosFiPhC = -1;
	if(GridReactPowerPhC<-5)	GridCosFiPhC=-GridCosFiPhC;

	//电容和负载侧
	LoadActPowerPhA  = (GridVoltDA*LoadRealCurDA+GridVoltQA*LoadRealCurQA)*POWER_CONV_RATIO;
	LoadReactPowerPhA= (GridVoltDA*LoadRealCurDA-GridVoltQA*LoadRealCurQA)*POWER_CONV_RATIO;
	LoadApparentPowerPhA = sqrtf(LoadActPowerPhA*LoadActPowerPhA+LoadReactPowerPhA*LoadReactPowerPhA);

    LoadActPowerPhB  = (GridVoltDB*LoadRealCurDB+GridVoltQB*LoadRealCurQB)*POWER_CONV_RATIO;
    LoadReactPowerPhB= (GridVoltDB*LoadRealCurDB-GridVoltQB*LoadRealCurQB)*POWER_CONV_RATIO;
	LoadApparentPowerPhB = sqrtf(LoadActPowerPhB*LoadActPowerPhB+LoadReactPowerPhB*LoadReactPowerPhB);

    LoadActPowerPhC  = (GridVoltDC*LoadRealCurDC+GridVoltQC*LoadRealCurQC)*POWER_CONV_RATIO;
    LoadReactPowerPhC= (GridVoltDC*LoadRealCurDC-GridVoltQC*LoadRealCurQC)*POWER_CONV_RATIO;
	LoadApparentPowerPhC = sqrtf(LoadActPowerPhC*LoadActPowerPhC+LoadReactPowerPhC*LoadReactPowerPhC);

	LoadCosFiPhA = LoadActPowerPhA/LoadApparentPowerPhA;
	LoadCosFiPhB = LoadActPowerPhB/LoadApparentPowerPhB;
	LoadCosFiPhC = LoadActPowerPhC/LoadApparentPowerPhC;
	if(LoadCosFiPhA >=1)		LoadCosFiPhA = 1;
	if(LoadCosFiPhA <= -1)		LoadCosFiPhA = -1;
	if(LoadReactPowerPhA<-5)	LoadCosFiPhA=-LoadCosFiPhA;
	if(LoadCosFiPhB >=1)		LoadCosFiPhB = 1;
	if(LoadCosFiPhB <= -1)		LoadCosFiPhB = -1;
	if(LoadReactPowerPhB<-5)	LoadCosFiPhB=-LoadCosFiPhB;
	if(LoadCosFiPhC >=1)		LoadCosFiPhC = 1;
	if(LoadCosFiPhC <= -1)		LoadCosFiPhC = -1;
	if(LoadReactPowerPhC<-5)	LoadCosFiPhC=-LoadCosFiPhC;

	//不平衡度计算
//	MinVolRms    = Min(gpVoltA_rms,gpVoltB_rms,gpVoltC_rms);
	MinLoadCosFi = Min(LoadCosFiPhA,LoadCosFiPhB,LoadCosFiPhC);
	MaxGridCurRms = Max(gridCurA_rms,gridCurB_rms,gridCurC_rms);
	MaxLoadCurRms = Max(loadCurA_rms,loadCurB_rms,loadCurC_rms);
	GridCurRmsUnban = (MaxGridCurRms-GridCurRms_F)/GridCurRms_F;
	LoadCurRmsUnban = (MaxLoadCurRms-LoadCurRms_F)/LoadCurRms_F;
	//状态更新
	StateEventFlag1.B.RunState_A = StateEventFlag_A;
	StateEventFlag1.B.RunState_B = StateEventFlag_B;
	StateEventFlag1.B.RunState_C = StateEventFlag_C;
	StateEventFlag1.B.AutoRecordReadFlag = RecordFlash.FlashState.B.AutoRecordReadFlag;
	if((StateEventFlag_A==STATE_EVENT_RUN_A)&&(StateEventFlag_B==STATE_EVENT_RUN_B)&&(StateEventFlag_C==STATE_EVENT_RUN_C))
	    RemoteStateRunStop = 1;
	else
	    RemoteStateRunStop = 0;
	if( ((StateEventFlag_A==STATE_EVENT_WAIT_A)||(StateEventFlag_A==STATE_EVENT_STOP_A)\
	        ||(StateEventFlag_A==STATE_EVENT_DISCHARGE_A)||(StateEventFlag_A==STATE_EVENT_RECHARGE_A))\
	        &&((StateEventFlag_B==STATE_EVENT_WAIT_B)||(StateEventFlag_B==STATE_EVENT_STOP_B)\
	                ||(StateEventFlag_B==STATE_EVENT_DISCHARGE_B)||(StateEventFlag_B==STATE_EVENT_RECHARGE_B))\
	                &&((StateEventFlag_C==STATE_EVENT_WAIT_C)||(StateEventFlag_C==STATE_EVENT_STOP_C)\
	                        ||(StateEventFlag_C==STATE_EVENT_DISCHARGE_C)||(StateEventFlag_C==STATE_EVENT_RECHARGE_C)) )
	    RemoteStateWait = 1;
	else
	    RemoteStateWait = 0;
    if((StateEventFlag_A==STATE_EVENT_FAULT_A)&&(StateEventFlag_B==STATE_EVENT_FAULT_B)&&(StateEventFlag_C==STATE_EVENT_FAULT_C))
        RemoteStateFault = 1;
    else
        RemoteStateFault = 0;
    if(StateFlag.constantQFlag==0)
        RemoteReactpowerOrder = reactPowerGiven*10;
    if(StateFlag.constantQFlag==1)
        RemoteReactpowerOrder = restantReactCurrent*10;
    if(StateFlag.constantQFlag==2)
        RemoteReactpowerOrder = constantCosFai*100;
	IOstate1  = \
            shift(PWM_ins_indexA,0)                |shift(PWM_ins_indexB,1)                    |shift(PWM_ins_indexA,2)                |shift(0,3)\
           |shift(0,4)     |shift(StateFlag.onceTimeStateMachine,5)    |shift(StateFlag.MainContactEnanle,6)   |shift(0,7)\
           |shift(GET_MAIN_CONTACT_ACTION_A,8)         |shift(GET_BYPASS_CONTACT_ACTION_A,9)              |shift(0,10)               |shift(0,11)  ;
	IOstate2  =\
			 shift(0,0)	|shift(0,1)				|shift(0,2)					|shift(0,3)\
			|shift(0,4)	|shift(0,5)	|shift(0,6)	|shift(0,7)\
			|shift(0,8)		|shift(0,9)	|shift(0,10)					|shift(0,11)\
			|shift(0,12)					|shift(0,13)				|shift(0,14)					|shift(0,15);
//	Choose5=(int)(DccapVoltA*10);//备用5始终显示 主板温度
	switch(debugDispFlag){
	  case  0:
		  Choose1=GpioDataRegs.GPCDAT.bit.GPIO79;		Choose2=GpioDataRegs.GPCDAT.bit.GPIO78;        //A相高低压磁保持 旁路磁保持
		  Choose3=GpioDataRegs.GPEDAT.bit.GPIO156;		Choose4=GpioDataRegs.GPDDAT.bit.GPIO104;
		  break;
	  case  2:
		  Choose1=GpioDataRegs.GPEDAT.bit.GPIO151;			Choose2=GpioDataRegs.GPEDAT.bit.GPIO150;   //B相高低压磁保持 旁路磁保持
		  Choose3=GpioDataRegs.GPFDAT.bit.GPIO164;			Choose4=GpioDataRegs.GPDDAT.bit.GPIO104;
		  break;
	  case  3:
		  Choose1=GpioDataRegs.GPCDAT.bit.GPIO77;			Choose2=GpioDataRegs.GPEDAT.bit.GPIO152;   //C相高低压磁保持 旁路磁保持
		  Choose3=GpioDataRegs.GPCDAT.bit.GPIO93;			Choose4=GpioDataRegs.GPDDAT.bit.GPIO104;
		  break;
	  case  4:
		  Choose1=(int)(TempData[0]*10);			                Choose2=(int)(TempData[1]*10);     //ESC 采样温度--散热器温度和单元内部温度
		  Choose3=(int)(WindCold.HeatSinkTempterature*10);			Choose4=0;
		  break;
	  case  5:
		  Choose1=(int)(StateEventFlag_A);				Choose2=(int)(StateEventFlag_B);               //ESC 三相状态 旁路微断反馈
		  Choose3=(int)(StateEventFlag_C);				Choose4=GpioDataRegs.GPCDAT.bit.GPIO92 ;
		  break;
	  case  6:
		  Choose1=(int)(ESCFlagA.ESC_DutyData*100);     Choose2=(int)(VoltInA_rms*100);                //ESC-A相输出占空比
		  Choose3=(int)(VoltOutA_rms*100);              Choose4=(int)(PIOutVoltValueA*10000);
		  break;
	  case  7:
		  Choose1=(int)(ESCFlagB.ESC_DutyData*100);		Choose2=(int)(VoltInB_rms*100);                //ESC-B相输出占空比
		  Choose3=(int)(VoltOutB_rms*100);				Choose4=(int)(PIOutVoltValueB*10000);
		  break;
	  case 8:
	      Choose1=(int)(ESCFlagC.ESC_DutyData*100);     Choose2=(int)(VoltInC_rms*100);                //ESC-C相输出占空比
	      Choose3=(int)(VoltOutC_rms*100);              Choose4=(int)(PIOutVoltValueC*10000);
		  break;
	  case 9:
		  Choose1=(int)(ESCBYRelayCNTA*10);			    Choose2=(int)(ESCBYRelayCNTB*10);              //ESC 旁路磁保持继电器或者高低压磁保持继电器损坏故障标志位
		  Choose3=(int)(ESCBYRelayCNTC*10);				Choose4=0;
		  break;
	  case 10:
		  Choose1=(int)(DccapVoltA*10);			Choose2=(int)(DccapVoltB*10);                          //ESC吸收电容电容电压
		  Choose3=(int)(DccapVoltC*10);			Choose4=(int)(DccapVoltA*10);
		  break;
	  case 11:
          Choose1=(int)(gridCurA_rms*10);         Choose2=(int)(gridCurB_rms*10);                      //ESC-电网电流
          Choose3=(int)(gridCurC_rms*10);         Choose4=0;
		  break;
	  case 12:
		  Choose1=(int)(gridCurrBYAF_rms*10);     Choose2=(int)(gridCurrBYBF_rms*10);                  //ESC-旁路电流
		  Choose3=(int)(gridCurrBYCF_rms*10);     Choose4=0;
		  break;
	  case 13:
	      Choose1=(int)(ESCFlagA.ESCCntSec.HWPowerStopDelay);           Choose2=(int)(ESCFlagB.ESCCntSec.HWPowerStopDelay);
	      Choose3=(int)(ESCFlagC.ESCCntSec.HWPowerStopDelay);           Choose4=0;
	      break;
	  case 14:
	      Choose1=(int)(ESCFlagA.ESCCntSec.HWPowerFaultDelay);           Choose2=(int)(ESCFlagB.ESCCntSec.HWPowerFaultDelay);
	      Choose3=(int)(ESCFlagC.ESCCntSec.HWPowerFaultDelay);           Choose4=0;
	      break;
	  case 20:
		  Choose1=(int)((VoltInA_rms/VoltOutA_rms)*1000);			Choose2=(int)((gridCurrBYAF_rms/gridCurA_rms)*1000);
		  Choose3=(int)((VoltInB_rms/VoltOutB_rms)*1000);			Choose4=(int)((gridCurrBYBF_rms/gridCurB_rms)*1000);
		  break;
      case 21:
          Choose1=(int)(PIOutVoltValueA*1000);           Choose2=(int)(PIOutVoltValueB*1000);             //boost升压PI闭环和前馈值
          Choose3=(int)(PIOutVoltValueC*1000);           Choose4=(int)(dutytmp);
          break;

      case 22:
          Choose1=(int)(ESCFlagA.PWM_ins_index);          Choose2=(int)(ESCFlagB.PWM_ins_index);             //恒流模式下C相电流目标值
          Choose3=(int)(ESCFlagC.PWM_ins_index);          Choose4=0;
          break;

	  case 24:
		  Choose1 = (int)(VolttargetCorrA*1000);			Choose2 = (int)(VolttargetCorrB*1000);     //恒流模式下由输入电流PI算的电压目标值系数
		  Choose3 = (int)(VolttargetCorrC*1000);			Choose4 = (int)(CurrTargetTemper*100);
		  break;
	  case 25:
//		  Choose1=(int)(SPLL[0].PllPiOutput*10);    Choose2=(int)(SPLL[0].Theta*1000);
//		  Choose3=(int)(SPLL[0].PLLResSin*1000);    Choose4=(int)(SPLL[0].PLLResCos*1000);
          Choose1=(int)(Esc_VoltPhaseA*100);    Choose2=(int)(Esc_VoltPhaseB*100);                     //ESC 电网电压单向锁相角度
          Choose3=(int)(Esc_VoltPhaseC*100);    Choose4=0;
		  break;
	  case 26:
          Choose1=(Uint16)(SPLL[0].PllPiOutput*100);                                                    //ESC 电网电压单相锁相值
          Choose2=(Uint16)(SPLL[1].PllPiOutput*100);
          Choose3=(Uint16)(SPLL[2].PllPiOutput*100);
          Choose4=(int)(GridVoltRms*10);
		  break;
	  case 27:
	      Choose1=(int)(ESCFlagA.resetFlag*10);         Choose2=(int)(ESCFlagB.resetFlag*10);
	      Choose3=(int)(ESCFlagC.resetFlag*10);         Choose4=0;
	      break;
      case 28:
          Choose1=(int)(ESCSicFaultCNTA*10);            Choose2=(int)(ESCSicFaultCNTB*10);              //ESC SIC管子损坏标志位
          Choose3=(int)(ESCSicFaultCNTC*10);            Choose4=0;
          break;
	  case 29:
	      Choose1=(int)(ESCFlagA.HWPowerFAULTFlag);                                                     //ESC 故障标志位
	      Choose2=(int)(ESCFlagB.HWPowerFAULTFlag);
          Choose3=(int)(ESCFlagC.HWPowerFAULTFlag);
          Choose4=0;
		  break;
	  case 30:
		  Choose1=(int16)(ESC_FeedForward_DutyA*1000);      Choose2=(int16)(ESC_FeedForward_DutyB*1000);
		  Choose3=(int16)(ESC_FeedForward_DutyC*1000);		Choose4=(int)(VoltOutA_rms*10);
		  Choose5=(int)(VoltInA_rms*10);
		  break;
	  case 31:
          Choose1=(int)(ESCFlagA.HWPowerSTOPFlag);          Choose2=(int)(ESCFlagB.HWPowerSTOPFlag);     //ESC 停机标志位
          Choose3=(int)(ESCFlagC.HWPowerSTOPFlag);          Choose4=0;
          Choose5=0;
		  break;
      case 32:
          Choose1=(int)(ESCFlagA.ESCCntMs.StartDelay);          Choose2=(int)(ESCFlagB.ESCCntMs.StartDelay);
          Choose3=(int)(ESCFlagC.ESCCntMs.StartDelay);          Choose4=0;
          break;
      case 34:
          Choose1=(int)(ConstantCurr[0].state);         Choose2=(int)(ConstantCurr[0].CNT1);
          Choose3=(int)(VolttargetCorrA*10000);         Choose4=(int)(ConstantCurr[0].CorrPI.i10*10000);
          Choose5=(int)(gridCurA_rms*10);
          break;
      case 35:
          Choose1=(int)(ConstantCurr[1].state);         Choose2=(int)(ConstantCurr[1].CNT1);
          Choose3=(int)(VolttargetCorrB*10000);         Choose4=(int)(ConstantCurr[1].CorrPI.i10*10000);
          Choose5=(int)(gridCurB_rms*10);
          break;
      case 36:
          Choose1=(int)(ConstantCurr[2].state);         Choose2=(int)(ConstantCurr[2].CNT1);
          Choose3=(int)(VolttargetCorrC*10000);         Choose4=(int)(ConstantCurr[2].CorrPI.i10*10000);
          Choose5=(int)(gridCurC_rms*10);
          break;
      case 37:
          Choose1=(int)(CurrentUnbalanceRegularVoltage[0]*10);         Choose2=(int)(CurrentUnbalanceRegularVoltage[1]*10);
          Choose3=(int)(CurrentUnbalanceRegularVoltage[2]*10);         Choose4=(int)(maxCh<<4|minCh);
          Choose5=(int)(UnCurrData[0]*10);
          break;
      case 38:
          Choose1=(int)(CurrentUnbalanceRegularVoltage[0]*10);         Choose2=(int)(UnCurrData[0]*10);
          Choose3=(int)(UnCurrData[1]*10);         Choose4=(UnCurrData[2]*10);
          Choose5=(int)(int)(maxCh<<4|minCh);
          break;

	  default :
		  Choose1=0;Choose2=0;Choose3=0;Choose4=0;
	  break;
	}
}

void PIAdjust(void)
{
	float OutPrecent,OutPrecentA,OutPrecentB,OutPrecentC;
	float OutPrecentND,OutPrecentNQ;
	if(0){//((StateEventFlag == STATE_EVENT_RUN)&&(PIAdjustTim>=10)&&(PIAutoJust==0)){//启动后延时10s,防止电流过冲
		if(StateFlag.reactPrCompFlag == 1){
			if((fabsf(ReactLoadCompCurA)>10)&&(fabsf(ReactLoadCompCurB)>10)&&(fabsf(ReactLoadCompCurC)>10)){
				OutPrecentA= IverRealCurQA/ReactLoadCompCurA;
				OutPrecentB= IverRealCurQB/ReactLoadCompCurB;
				OutPrecentC= IverRealCurQC/ReactLoadCompCurC;
				if(OutPrecentA>0.5){
					if(fabsf(IverRealCurQA-ReactLoadCompCurA)>0.5)
						ReactivePowerCurrCalibA = RunPIRE(&PIIvrA,1,OutPrecentA);
				}else{
					ReactivePowerCurrCalibA = 1.1;
				}
				if(OutPrecentB>0.5){
					if(fabsf(IverRealCurQB-ReactLoadCompCurB)>0.5)
						ReactivePowerCurrCalibB = RunPIRE(&PIIvrB,1,OutPrecentB);
				}else{
					ReactivePowerCurrCalibB = 1.1;
				}
				if(OutPrecentC>0.5){
					if(fabsf(IverRealCurQC-ReactLoadCompCurC)>0.5)
						ReactivePowerCurrCalibC = RunPIRE(&PIIvrC,1,OutPrecentC);
				}else{
					ReactivePowerCurrCalibC = 1.1;
				}
			}else{
				ReactivePowerCurrCalibA=ReactivePowerCurrCalibB=ReactivePowerCurrCalibC=1.2;
			}

	        if(fabsf(ReactLoadCompCurQ)>17){
				OutPrecent= InvFundaCurQ/ReactLoadCompCurQ;
				if(OutPrecent>0.5){
					if(fabsf(InvFundaCurQ-ReactLoadCompCurQ)>1)
						ReactivePowerCurrCalibQ = RunPIRE(&PIIvrQ,1,OutPrecent);
				}else{
					ReactivePowerCurrCalibQ = 1.1;
				}
			}else{
				ReactivePowerCurrCalibQ = 1.1;
		}
		}
		if((fabsf(UbanCompLoadCurND)>10)&&(StateFlag.negCurCompFlag == 1)){
			OutPrecentND= InvFundaCurND/UbanCompLoadCurND;
			if(OutPrecentND>0){
				if(fabsf(InvFundaCurND-UbanCompLoadCurND)>0.5)
					NegaDPowerCalibD = RunPIRE(&PIIvrND,1,OutPrecentND);
			}else{
				NegaDPowerCalibD = 1.1;
			}
		}else{
			NegaDPowerCalibD = 1.1;
		}

		if((fabsf(UbanCompLoadCurNQ)>10)&&(StateFlag.negCurCompFlag == 1)){
			OutPrecentNQ= InvFundaCurNQ/UbanCompLoadCurNQ;
			if(OutPrecentNQ>0){
				if(fabsf(InvFundaCurNQ-UbanCompLoadCurNQ)>0.5)
					NegaQPowerCalibQ = RunPIRE(&PIIvrNQ,1,OutPrecentNQ);
			}else{
				NegaQPowerCalibQ = 1.1;
			}
		}else{
			NegaQPowerCalibQ = 1.1;
		}
	}else{
	    ReactivePowerCurrCalibA=ReactivePowerCurrCalibB=ReactivePowerCurrCalibC=1.1;
	    ReactivePowerCurrCalibQ = NegaQPowerCalibQ = NegaDPowerCalibD = 1.1;
	    PIIvrA.i6 = 1.1; PIIvrA.i10 = 0;
	    PIIvrB.i6 = 1.1; PIIvrB.i10 = 0;
	    PIIvrC.i6 = 1.1; PIIvrC.i10 = 0;
	    PIIvrQ.i6 = 1.1; PIIvrQ.i10 = 0;
	    PIIvrND.i6 = 1.1;PIIvrND.i10 = 0;
	    PIIvrNQ.i6 = 1.1;PIIvrNQ.i10 = 0;
	}
}

void RemoteWriteControl(Uint16 usAddress) //0x06  0x05
{
    if((StateFlag.RemoteWriteCoilFlag)&&(RecordFlash.FlashState.B.RemoteControlEn)){  //0x05
        switch(usAddress){
            case 0x0000:
                if(RemoteStateCtrl.RemoteStart==0xFF00){
           if((!ESCFlagA.startFlag)&&(!ESCFlagB.startFlag)&&(!ESCFlagC.startFlag))			SoeRecData(SOE_GP_EVENT);
                   FlashStartTim = 0;
                   StateFlag.StartFlashFlag = 1;  //判断是否使用远程烧写
                   RemoteStateCtrl.RemoteStart=0;
        }
                break;
            case 0x0001:
                if(RemoteStateCtrl.RemoteStop==0xFF00){
            if((!ESCFlagA.stopFlag)&&(!ESCFlagB.stopFlag)&&(!ESCFlagC.stopFlag))		SoeRecData(SOE_GP_EVENT+1);
                ESCFlagA.stopFlag = 1;			//停止命令
                ESCFlagB.stopFlag = 1;
                ESCFlagC.stopFlag = 1;
                ESCFlagA.autoStFlag = ORIGINAL_STATE_A;
                ESCFlagB.autoStFlag = ORIGINAL_STATE_B;
                ESCFlagC.autoStFlag = ORIGINAL_STATE_C;
                RemoteStateCtrl.RemoteStop=0;
        }
                break;
            case 0x0002:
                if(RemoteStateCtrl.RemoteReset==0xFF00){
//                    StateFlag.resetFlag = 1;		//复位命令
                    ESCFlagA.resetFlag = 1;
                    ESCFlagB.resetFlag = 1;
                    ESCFlagC.resetFlag = 1;
                    cntForRepFaultA = 0;				//自启清零
                    cntForRepFaultB = 0;
                    cntForRepFaultC = 0;
                    RemoteStateCtrl.RemoteReset=0;
        }
                break;
            default:break;
        }
    }

    if((StateFlag.RemoteWriteHoldFlag)&&(RecordFlash.FlashState.B.RemoteControlEn)){  //0x06
        switch(usAddress){
            case 0x0000:
//                if((StateFlag.constantQFlag==0)&&(StateFlag.AVCPrCompFlag))
//                    reactPowerGiven = RemoteInstructCtrl.ReactpowerOrder*0.1;
//                if((StateFlag.constantQFlag==1)&&(StateFlag.AVCPrCompFlag))
//                    restantReactCurrent = RemoteInstructCtrl.ReactpowerOrder*0.1;
//                if((StateFlag.constantQFlag==2)&&(StateFlag.AVCPrCompFlag)){
//                    if((((fabs(RemoteInstructCtrl.ReactpowerOrder))<=100))&&(RemoteInstructCtrl.ReactpowerOrder!=0)){
//                        constantCosFai = RemoteInstructCtrl.ReactpowerOrder*0.01;
//                        PFConsin = sqrt(1 - constantCosFai*constantCosFai)/constantCosFai;
//                    }
//                }
                    VolttargetA = RemoteInstructCtrl.VolttargetA*0.001;
                break;
            case 0x0001:
                    VolttargetB = RemoteInstructCtrl.VolttargetB*0.001;
                break;
            case 0x0002:
                    VolttargetC = RemoteInstructCtrl.VolttargetC*0.001;
                break;
            case 0x0003:
                    VoltThreSholdA = RemoteInstructCtrl.VoltThreSholdA*0.001;
                break;
            case 0x0004:
                    VoltThreSholdB = RemoteInstructCtrl.VoltThreSholdB*0.001;
                break;
            case 0x0005:
                    VoltThreSholdC = RemoteInstructCtrl.VoltThreSholdC*0.001;
                break;
            case 0x0006:
                    Backdiffvalue = RemoteInstructCtrl.Backdiffvalue*0.001;
                break;
            default:break;
    }
    }
    StateFlag.RemoteWriteHoldFlag = 0;
    StateFlag.RemoteWriteCoilFlag = 0;
}


/* 多单元的情况比较特殊：上位机去做控制逻辑，单元仅仅接收启动指令即可。
 * 1、单元之间并无关联，LCD并不起逻辑控制的作用，仅供显示。这里需要和LCD实现功能互补。
 * 1118 增加多单元的自启动方案。
 *
	多单元轮询：
	哪几个涉及轮询：
	------------------------------------------------------------
	故障了（中间量） = 某个单元故障了
	频繁自启动标志位（中间量） = 某个单元频繁自启动标志位
		无故障标志 （中间量）= 某个单元无故障标志

		其它变量不用管多个单元，直接设定即可。

	 //   单元里：“自动”模式，单元故障一分钟后，自动复位，等待启动指令。
	--------------------------------------------------------------
	自启动状态机：
	if(故障了 == 1）
		故障后记满200秒计时器 ++；
	else
		故障后记满200秒计时器 = 0

	------------------------------------------------------------
	状态0：if(状态 == 0)
	if（（故障了 == 1）&&（频繁自启动标志位 == 0））
		能进入自启动；
		状态 = 1；
	else （（故障了 == 1）&&（频繁自启动标志位 == 1））
		状态 = 3；
		状态1：if(状态 == 1)
		自启动标志位 =1；
		状态 = 2；
		计时器 = 0；
	状态2：if(状态 == 2)
		if((无故障标志 == 1)&(故障后记满200秒计时器 >= 100))
		{
			自启动计时器++；
			if（自启动计时器>= 40)&&(故障后记满200秒计时器 >= 200)
			满两分钟，可以下发“全部单元”启动指令；
			状态0 = 1；
		}
		else  // 其它故障复位不掉，记满40秒仍故障
		{
			状态0 = 1；
		}
	状态3：频繁自启动了，等待跳转状态0
		   if（（频繁自启动标志位 == 0）&&（状态3计时器 >= 30秒）
	{
	状态0 = 1；
	}
 */

//*********************** NO MORE **************************//



//===========================================================================
// No more.
//===========================================================================

