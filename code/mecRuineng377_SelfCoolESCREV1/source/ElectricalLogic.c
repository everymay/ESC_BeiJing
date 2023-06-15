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

/* ������������ֹͣ���� 0725
*  ״̬�¼�0 �� ���Ƶ�Դȡ�Ե����������ֶ�ģʽ����
*  ״̬�¼�1 �� ���������磬 ������ȷ�ϣ��Ӵ����պ�  ---- ��ת���ݵ�����ѹ��ֵ��Ϊ0
*  ״̬�¼�2 �� ����           ----- �ȴ����յ�����ָ��������г�̬״̬2 -----------ע������ȶ�ĸ�ߵ�ѹǰ��ͨ��С�޷�������
*  ״̬�¼�3 �� ����ͣ�� ----- ���յ�ͣ��ָ��״̬3����ɺ����״̬1���ȴ����ơ�-----ע����صĲ�������ѹ���͵������ĸ����ݼ�
*  ״̬�¼�8 �� ����ģʽ ----- ����������Ӳ�����ӳ���
*  ״̬�¼�9 �� ���ϴ��� ----- �ֶ���ͣͣ��������ͣ�����������״̬��
*  �ֶ���ͣ�͹��ϼ�ͣ����������ֶ���ͣͣ���󣬽���״̬2���ȴ����ƣ�����ͣ����������9�������������
*  ����ʹ�ã�����ģ�������ԣ�����ͨ����ֱ�����ѹͨ�������PWM��ͨ������������ʵ�ֿ��ٷŵ硣
*  ���簴ťֻ�������ͼ�ͣ�����źţ�����·���õ���ֱ�����ӷ�ʽ������һ��·�壩��
*  	      ��״̬�¼�2�õ���ͣ�źţ����ȵ�������5%����ȫ���Ͽ����أ�
*      ������ƻ�δֹͣ��״̬��ͣ�����������ֱ��ĸ�߾����������¿��ܻ�ը����
*      �������ʱ�ļ�ͣ���������״̬9�Զ�����PWM������˲��Ϊ�㣬ʵ�ֹ���ͣ����
*/
// �ֶ�ͣ����ֱ��ĸ�ߵ�ѹ���ڸ�ѹ��ϵͳ��ʾ��������������������������ͣ������ͣ����
// �����ֶ�ͣ������Ҫ���ޣ�ֱ�����ѹ������ˮ�����ŵ磬�ȴ������



/*
 * ���ܣ���ʼ��GPIO�����WY
 */
void OutputGpioInit_NEWBOARD(void)
{
	SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_DISABLE); //�Ͽ�A����·�ű��̵ּ�����WY
	SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_DISABLE); //�Ͽ�B����·�ű��̵ּ�����WY
	SET_MAIN_CONTACT_ACTION_C(ESC_ACTION_DISABLE); //�Ͽ�C����·�ű��̵ּ�����WY

	SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //�պ�A����·�ű��̵ּ�����WY
	SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE); //�պ�B����·�ű��̵ּ����պϡ�WY
	SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE); //�պ�C����·�ű��̵ּ����պϡ�WY

	SET_SCRA_ENABLE(ESC_ACTION_DISABLE); //�ض�A����·��բ�ܡ�WY
	SET_SCRB_ENABLE(ESC_ACTION_DISABLE); //�ض�B����·��բ�ܡ�WY
	SET_SCRC_ENABLE(ESC_ACTION_DISABLE); //�ض�C����·��բ�ܡ�WY

	SET_RUNNING_LED(0); //�豸����ָʾ����WY
	SET_FAULT_LED(0); //�豸����ָʾ����WY

	SET_PROG_RUNNING_LED(1); //�˺��������塣WY
	SET_MACHINE_RUNNING(1); //�˺��������塣WY

	SET_BOARDRUNNING_LED(1); //��������ָʾ������WY
	SET_LED_SYNC(1); //GPIO���Ų���Ӧ��WY
	SET_BOARDFAULT_LED(1); //GPIO���Ų���Ӧ��WY

	SET_IGBT_ENA(0); //ʧ��A��PWM��WY
	SET_IGBT_ENB(0); //ʧ��B��PWM��WY
	SET_IGBT_ENC(0); //ʧ��C��PWM��WY
}

#define ESC_STANDY_DELAY_TIME_POINT 100

/*
 * ���ܣ�ִ�С���ʼ��״̬������ͣ������WY
 * �����������ִ�е�X�������X��ȡֵ����Ϊ{A, B, C}��
 */
void ESCSTANDBYEVENT(ESCCTRLVALFLAG *ESCFlag)
{
	/*����A�ࡣWY*/
	if (ESCFlag->PHASE == 1)
	{
		SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //�պ�A����·�ű��̵ּ�����WY
		SET_IGBT_ENA(IGBT_DISABLE); //ʧ��A��PWM��WY
		DisablePWMA(); //����A��PWM��WY

		/*
		 * ���º������ܴ��ɡ��������£�WY
		 * GPIO79ʼ��Ϊ���ģʽ��ͨ����ȡ�����ŵ�ƽ״̬�������ű��̵ּ���״̬�����塣
		 */
		if (GET_MAIN_CONTACT_ACTION_A == ESC_ACTION_ENABLE) //A����·�ű��̵ּ����պϡ�WY
		{
			SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_DISABLE); //�Ͽ�A����·�ű��̵ּ�����WY
		}
	}

	/*����B�ࡣWY*/
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

	/*����C�ࡣWY*/
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

	/*����״̬��⡣WY*/
	if (ESCFlag->onceTimeStateMachine == 1)
	{
		/*����·�ű��̵ּ���δ�պϣ���ѭ�������·�ű��̵ּ���״̬��ֱ����������ʱʱ����5s����WY*/
		if (ESCFlag->ByPassContactFlag == 1) //��·�ű��̵ּ����պϡ�WY
		{
			/*����A�ࡣWY*/
			if (ESCFlag->PHASE == 1)
			{
				SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_DISABLE); //�Ͽ�A����·�ű��̵ּ�����WY
				SET_SCRA_ENABLE(ESC_ACTION_DISABLE); //�ض���·��բ�ܡ�WY
				ESCFlag->onceTimeStateMachine = 0; //��λ����״̬����־λ��WY
			}

			/*����B�ࡣWY*/
			if (ESCFlag->PHASE == 2)
			{
				SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_DISABLE);
				SET_SCRB_ENABLE(ESC_ACTION_DISABLE);
				ESCFlag->onceTimeStateMachine = 0;
			}

			/*����C�ࡣWY*/
			if (ESCFlag->PHASE == 3)
			{
				SET_MAIN_CONTACT_ACTION_C(ESC_ACTION_DISABLE);
				SET_SCRC_ENABLE(ESC_ACTION_DISABLE);
				ESCFlag->onceTimeStateMachine = 0;
			}
		}

		/*
		 * ����A�ࡣWY
		 * ����·�ű��̵ּ����պ��쳣������5s���������жϷ�֧��
		 * ����·�ű��̵ּ����պ����������������жϷ�֧��
		 */
		if ((ESCFlag->ESCCntMs.ContactorDelay >= CNT_MS(5000)) //������ʱ����ʱʱ����5s����WY
				&& (StateEventFlag_A == STATE_EVENT_STANDBY_A)) //A�ദ�ڳ�ʼ״̬������ͣ������WY
		{
			if (softwareFaultWord2.B.ESCBypassContNoCloseFlagA == 0) //��·�ű��̵ּ����պ�������WY
			{
				ESCFlagA.FAULTCONFIRFlag = 1; //���ڹ����źš�WY
				ESCFlagA.ESCCntMs.StartDelay = 0;
				softwareFaultWord2.B.ESCBypassContNoCloseFlagA = FaultDetect(SOE_GP_FAULT + 27, CNT_BYPASS_NOCLOSE_A, 2, ESCFlagA.PHASE);
			}
		}
		else //δ������ʱ����ʱʱ����5s����WY
		{
			SetFaultDelayCounter(CNT_BYPASS_NOCLOSE_A, 0); //����Ѽ�⵽��A����·�ű��̵ּ����պϹ����źŵĴ�����WY
		}

		/*����B�ࡣWY*/
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

		/*����C�ࡣWY*/
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
 * ���ܣ�ִ�С�Ԥ��硿״̬��WY��
 * �����������ִ�е�X�������X��ȡֵ����Ϊ{A, B, C}��
 */
void ESCPRECHARGEEVENT(ESCCTRLVALFLAG *ESCFlag)
{
	ESCFlag->EscStandyDelayCnt1 = 0;
	ESCFlag->EscStandyDelayCnt2 = 0;

	if (ESCFlag->ESCCntSec.ChargingTime >= CNT_SEC(3)) //���ʱ������������WY
	{
		if (++ESCFlag->EscWaitRunDelayCnt >= 300) //��ʱ��λ�ǣ������ǣ�WY
		{
			/*����A��*/
			if ((ESCFlag->PHASE == 1) //A�ࡣWY
					&&((GET_MAIN_CONTACT_ACTION_A == ESC_ACTION_DISABLE) //A����·�ű��̵ּ������ڶϿ�״̬��WY
					|| (ESCFlag->DelayFlag == 1))) //���ض������¡�ʹ�ñ���֧���ж������������WY
			{
				ESCFlag->EscWaitRunDelayCnt = 300; //ǿ�Ƹ�ֵ��Ŀ�ģ���ֹ�����ݵ����󣬷������硣WY
				ESCFlag->DelayFlag = 1; //ʹ�ñ���֧���ж������������WY

				SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //�պ�A����·�ű��̵ּ�����WY

				/* WY
				 * ���ڣ�1. �ű��̵ּ���������Ҫʱ�䣻2. �Դű��̵ּ�����״̬�����������������WY
				   ���ڷ��ͱպ�A����·�ű��̵ּ�����ָ����޷�������⵽A����·�ű��̵ּ������ڱպ�״̬��������Ҫѭ����⡣
				 */
				if (ESCFlag->ContactorFeedBackFlag == 1) //A����·�ű��̵ּ����պϡ��ñ�־λ��AD-ISR�б��޸ġ�WY
				{
					ESCFlag->DelayFlag = 0;
					StateEventFlag_A = STATE_EVENT_WAIT_A; //�л�A��״̬����������״̬��WY
					ESCFlag->ESCCntSec.S_RSwiTiDelay = 0;
					ESCFlag->ESCCntSec.HWPowerSupDelay = 0;
				}
			}

			/*����B��*/
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

			/*����C��*/
			if ((ESCFlag->PHASE == 3) && ((GET_MAIN_CONTACT_ACTION_C == ESC_ACTION_DISABLE) || (ESCFlag->DelayFlag == 1)))//�ű��̵ּ�����ͨ�߼��ǵ͵�ƽ��ͨ;
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
	else //���ʱ��δ����������WY
	{
		ESCFlag->EscWaitRunDelayCnt = 0;
	}
}

/*
 * ���ܣ�ִ�С����С�״̬��WY
 * �����������ִ�е�X�������X��ȡֵ����Ϊ{A, B, C}��
 */
void ESCRUNEVENT(ESCCTRLVALFLAG *ESCFlag)
{
	if ((ESCFlag->ESCCntMs.PllDelay > CNT_MS(1000)) //Ϊ���໷��������ʱ�� > �趨ֵ��WY
			&& (ESCFlag->onceTimeStateMachine == 0))
	{
		switch (StateFlag.VoltageMode)
		{
			case 0://��ѹ��WY
			case 4:
			{
				if ((ESCFlag->VoltIn_rms > ESCFlag->StableValue)  //������ѹ��Чֵ > �ȶ�ֵ��������������ѹ��WY
						|| (ESCFlag->gridCur_rms < LoadCurlimit)) //����������Чֵ < ���ص����޶�ֵ�����������ڹ��ϡ�WY
				{
					if (++ESCFlag->EscStandyDelayCnt1 > ESC_STANDY_DELAY_TIME_POINT) //����100��״̬�����ڣ�����ʱ���Ƕ��٣�WY
					{
						ESCFlag->EscStandyDelayCnt1 = ESC_STANDY_DELAY_TIME_POINT;

						/*����A��*/
						if (ESCFlag->PHASE == 1)
						{
							ESCFlag->PWM_ins_index = 1; //A����·ֱͨ��WY
						}

						/*����B��*/
						if (ESCFlag->PHASE == 2)
						{
							ESCFlag->PWM_ins_index = 1;
						}

						/*����C��*/
						if (ESCFlag->PHASE == 3)
						{
							ESCFlag->PWM_ins_index = 1;
						}
					}
				}
				else /*��Ҫ��ѹ���Ҳ����ڹ��ϡ�WY*/
				{
					ESCFlag->EscStandyDelayCnt1 = 0;
					ESCFlag->ESCCntSec.R_SSwiTiDelay = 0;
				}

				if ((ESCFlag->VoltIn_rms < ESCFlag->StableValue)  //������ѹ��Чֵ < �ȶ�ֵ����������Ҫ������ѹ״̬��WY
						&& (ESCFlag->gridCur_rms >= LoadCurlimit)) //����������Чֵ > ���ص����޶�ֵ�������������ڹ��ϡ�WY
				{
					if (++ESCFlag->EscStandyDelayCnt2 > ESC_STANDY_DELAY_TIME_POINT) //����100��״̬�����ڣ�����ʱ���Ƕ��٣���WY
					{
						ESCFlag->EscStandyDelayCnt2 = ESC_STANDY_DELAY_TIME_POINT; //ǿ�Ƹ�ֵ����ֹ�����ڵ���ʱ�������硣WY

						/*����A��*/
						if (ESCFlag->PHASE == 1)
						{
							ESCFlag->PWM_ins_index = 0; //��·���ơ�WY
						}

						/*����B��*/
						if (ESCFlag->PHASE == 2)
						{
							ESCFlag->PWM_ins_index = 0;
						}

						/*����C��*/
						if (ESCFlag->PHASE == 3)
						{
							ESCFlag->PWM_ins_index = 0;
						}
					}
				}
				else /*���豣����ѹ״̬��WY*/
				{
					ESCFlag->EscStandyDelayCnt2 = 0;
				}
			}
				break;

			case 1: //��ѹ
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

			case 3://��ƽ���������
				if ((ESCFlag->VoltIn_rms < ESCFlag->StableValue) && (ESCFlag->gridCur_rms >= LoadCurlimit))
				{//�����ѹ��������ֵ,�Ҹ��ص��������趨ֵ+2,��λboost״̬
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
		ESCFlag->PWM_ins_index = 1;//����δ���,��ΪIGBTֱͨ״̬
	}
}

/*
 * ���ܣ�ִ�С�����ת������״̬��WY
 * �����������ִ�е�X�������X��ȡֵ����Ϊ{A, B, C}��
 * ˵�������豸��������ѹģʽʱ������ǰ������ѹ�����������ת����״̬��
 */
void ESCRUNTURNWAIT(ESCCTRLVALFLAG *ESCFlag)
{
	switch (StateFlag.VoltageMode)
	{
		case 0: //��ѹ
		case 4:
		{
			/*������ѹ��WY*/
			if ((ESCFlag->VoltIn_rms > ESCFlag->StableValue) //������ѹ��Чֵ > �ȶ�ֵ��WY
					&& (ESCFlag->PWM_ins_index == 1)) //��·ֱͨ��WY
			{
				if (ESCFlag->ESCCntSec.R_SSwiTiDelay >= CNT_SEC(R_SSwitchTime)) //����״̬����ʱ�� > �趨ֵ��WY
				{
					/*����A�ࡣWY*/
					if (ESCFlag->PHASE == 1)
					{
						SET_SCRA_ENABLE(ESC_ACTION_ENABLE); //��ͨA����·��բ�ܡ�WY
						SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //�պ�A����·�ű��̵ּ�����WY
						SET_IGBT_ENA(IGBT_DISABLE); //���ʧ��A��PWM��WY
						DisablePWMA(); //Ӳ������A��PWM��WY
						ESCFlag->StateSkipFlag = 1; //��λ��ת��־λ���ñ�־λ���ڡ�����״̬���б�ʹ�á�WY
						StateEventFlag_A = STATE_EVENT_WAIT_A; //�л�״̬������A�����״̬��WY
						ESCFlag->ESCCntSec.HWPowerSupDelay = 0;
					}

					/*����B�ࡣWY*/
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

					/*����C�ࡣWY*/
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

					ESCFlag->onceTimeStateMachine = 1; //��λ����״̬��⡣WY
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

		case 1: //��ѹ
			if ((ESCFlag->VoltIn_rms < ESCFlag->StableValue) && (ESCFlag->PWM_ins_index == 1))//����
			{
				if (ESCFlag->ESCCntSec.R_SSwiTiDelay >= CNT_SEC(R_SSwitchTime))
				{//��ʱ����Ҫ���ó�һ��,��ֹ��һ����Ƶ��Ͷ��.
					if (ESCFlag->PHASE == 1)
					{
						SET_SCRA_ENABLE(ESC_ACTION_ENABLE);//��·״̬
						SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE);//����·
						SET_IGBT_ENA(IGBT_DISABLE);
						DisablePWMA();
						ESCFlag->StateSkipFlag = 1;
						StateEventFlag_A = STATE_EVENT_WAIT_A;
						ESCFlag->ESCCntSec.HWPowerSupDelay = 0;

					}
					if (ESCFlag->PHASE == 2)
					{
						SET_SCRB_ENABLE(ESC_ACTION_ENABLE);//��·״̬
						SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE);//����·
						SET_IGBT_ENB(IGBT_DISABLE);
						DisablePWMB();
						ESCFlag->StateSkipFlag = 1;
						StateEventFlag_B = STATE_EVENT_WAIT_B;
						ESCFlag->ESCCntSec.HWPowerSupDelay = 0;

					}
					if (ESCFlag->PHASE == 3)
					{
						SET_SCRC_ENABLE(ESC_ACTION_ENABLE);//��·״̬
						SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE);//����·
						SET_IGBT_ENC(IGBT_DISABLE);
						DisablePWMC();
						ESCFlag->StateSkipFlag = 1;
						StateEventFlag_C = STATE_EVENT_WAIT_C;
						ESCFlag->ESCCntSec.HWPowerSupDelay = 0;

					}
					ESCFlag->onceTimeStateMachine = 1;//����·����һ��״̬��ⴰ��
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
			break;//��ƽ�ⷽʽ��ת����
	}
	ESCFlag->FunContDelayFlag = 0; //�����塣WY
	ESCFlag->ESCCntSec.FunContDelay = 0; //�����塣WY
}

/*
 * ���ܣ�ִ�С�������״̬��WY ���������뺯����������
 * �������ESCFlag����ִ�е�X�������X��ȡֵ����Ϊ{A, B, C}��
 * ˵�������豸�����ڴ���״̬ʱ����������ѹ�������������ѹģʽ��
 */
void ESCWAITTURNRUN(ESCCTRLVALFLAG *ESCFlag)
{
	switch (StateFlag.VoltageModeFlag)
	{
		case 0:	//��ѹģʽ��WY
		case 3:
		case 4:
			if ((ESCFlag->VoltIn_rms < ESCFlag->VoltThreShold) //������ѹ��Чֵ < ��ѹ����ֵ��WY
					&& (ESCFlag->gridCurrBY_rms < RateLoadCurrLimit)) //��·������Чֵ < ���ض�����޶�ֵ��WY
			{
				SET_POWER_CTRL(1); //15V��Դ�ϵ硣WY
				Delayus(TIME_WRITE_15VOLT_REDAY); //��ʱ��ʹ��15V��Դ�����ȶ���WY

				ESCFlagA.ESCCntSec.HWPowerSupDelay = 0; //����ر�15V��Դ��WY
				ESCFlagB.ESCCntSec.HWPowerSupDelay = 0; //����ر�15V��Դ��WY
				ESCFlagC.ESCCntSec.HWPowerSupDelay = 0; //����ر�15V��Դ��WY

				if (ESCFlag->ESCCntSec.S_RSwiTiDelay >= CNT_SEC(S_RSwitchTime)) //������ѹǷѹʱ�������趨ֵ��WY
				{//����һ���Ƿѹ����5000����//Լ4S
					/*����A�ࡣWY*/
					if (ESCFlag->PHASE == 1)
					{
						ConstantCurr[0].state = SM_CONSTANT_CURR_STANDBY;
						ESCFlag->ESC_DutyData = 1.0; //ռ�ձ�Ϊ100%��WY
						ESCFlag->PWM_ins_index = 1; //��·ֱͨ��WY
						Delayus(TIME_WRITE_15VOLT_REDAY); //��ʱ2ms��WY
						EnablePWMA(); //�������A��PWM��WY
						PIVolA.i10 = 0;//������
						PIVolA.i6 = 0;
						Delayus(100); //��ʱ��ΪPWM��������ߵ�ƽ����ʱ�䡣WY
						SET_IGBT_ENA(IGBT_ENABLE); //Ӳ��ʹ��A��PWM��WY
						SET_SCRA_ENABLE(ESC_ACTION_ENABLE); //��ͨA����·��բ�ܡ�ע�⣺�ڶϿ��ű��̵ּ���֮ǰ�����뿪ͨ��բ�ܣ���ֹ�ű��̵ּ���������WY
						SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_DISABLE); //�Ͽ�A����·�ű��̵ּ�����WY
						StateEventFlag_A = STATE_EVENT_RUN_A; //�л�״̬������A����������.WY
						CurrentUnbalanceRegularVoltage[0] = ESCFlagA.VoltThreShold; //��Сֵ  //����WY
					}

					/*����B�ࡣWY*/
					if (ESCFlag->PHASE == 2)
					{
						ConstantCurr[1].state = SM_CONSTANT_CURR_STANDBY;
						ESCFlag->ESC_DutyData = 1.0;//ռ�ձ���Ϊ0.93
						ESCFlag->PWM_ins_index = 1;//IGBTֱ̬ͨ
						Delayus(TIME_WRITE_15VOLT_REDAY);
						EnablePWMB();
						PIVolB.i10 = 0;//������
						PIVolB.i6 = 0;
						Delayus(100);//��Ҫ�Ӹ���ʱʱ��,����PWM�ļĴ���������������
						SET_IGBT_ENB(IGBT_ENABLE);//��IGBTʹ��
						SET_SCRB_ENABLE(ESC_ACTION_ENABLE);//����բ��
						SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_DISABLE);//����·
						StateEventFlag_B = STATE_EVENT_RUN_B;
						CurrentUnbalanceRegularVoltage[1] = ESCFlagB.VoltThreShold;
					}

					/*����C�ࡣWY*/
					if (ESCFlag->PHASE == 3)
					{
						ConstantCurr[2].state = SM_CONSTANT_CURR_STANDBY;
						ESCFlag->ESC_DutyData = 1.0;//ռ�ձ���Ϊ0.93
						ESCFlag->PWM_ins_index = 1;//IGBTֱ̬ͨ
						Delayus(TIME_WRITE_15VOLT_REDAY);
						EnablePWMC();
						PIVolC.i10 = 0;//������
						PIVolC.i6 = 0;
						Delayus(100);//��Ҫ�Ӹ���ʱʱ��,����PWM�ļĴ���������������
						SET_IGBT_ENC(IGBT_ENABLE);//��IGBTʹ��
						SET_SCRC_ENABLE(ESC_ACTION_ENABLE);//����բ��
						SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_DISABLE);//����·
						StateEventFlag_C = STATE_EVENT_RUN_C;
						CurrentUnbalanceRegularVoltage[2] = ESCFlagC.VoltThreShold;
					}

					ESCFlag->onceTimeStateMachine = 1; //��λ����״̬����־λ��WY
					ESCFlag->ESCCntMs.ContactorDelay = 0;
					ESCFlag->ESCCntSec.S_RSwiTiDelay = 0; //���������ѹǷѹʱ����WY
					ESCFlag->ESCCntSec.R_SSwiTiDelay = 0;

					CntMs.UNCurrDelay1 = 0;
					CntMs.UNCurrDelay2 = 0;
				}
			}
			else //������ѹ��WY
			{
				if ((ESCFlagA.ESCCntSec.HWPowerSupDelay >= CNT_SEC(TIME_STANDBY_DELAY))  //A�������15V��Դ����ʱ�� > 600s��WY
						&& (ESCFlagB.ESCCntSec.HWPowerSupDelay >= CNT_SEC(TIME_STANDBY_DELAY)) //B�������15V��Դ����ʱ�� > 600s��WY
						&& (ESCFlagC.ESCCntSec.HWPowerSupDelay >= CNT_SEC(TIME_STANDBY_DELAY))) //C�������15V��Դ����ʱ�� > 600s��WY
				{
					if ((StateEventFlag_A == STATE_EVENT_WAIT_A) //A�ദ�ڴ���״̬
							&& (StateEventFlag_B == STATE_EVENT_WAIT_B) //B�ദ�ڴ���״̬
							&& (StateEventFlag_C == STATE_EVENT_WAIT_C)) //C�ദ�ڴ���״̬
					{
						if ((ESCBYRelayCNTA != 1)  //A����·�ű��̵ּ���״̬������WY
								&& (ESCBYRelayCNTB != 1)  //B����·�ű��̵ּ���״̬������WY
								&& (ESCBYRelayCNTC != 1)) //C����·�ű��̵ּ���״̬������WY
						{
							SET_POWER_CTRL(0); //15V��Դ�ϵ硣WY
						}
					}
				}

				if (ESCFlag->PHASE == 1) //����A�ࡣWY
				{
					ESCFlagA.ESCCntSec.S_RSwiTiDelay = 0; //�������Ƿѹʱ������λ��s����WY
				}

				if (ESCFlag->PHASE == 2) //����B�ࡣWY
				{
					ESCFlagB.ESCCntSec.S_RSwiTiDelay = 0;
				}

				if (ESCFlag->PHASE == 3) //����C�ࡣWY
				{
					ESCFlagC.ESCCntSec.S_RSwiTiDelay = 0;
				}
			}
			break;

		/*��ѹģʽ*/
		case 1:
			if ((ESCFlag->VoltIn_rms > ESCFlag->VoltThreShold) && (ESCFlag->gridCurrBY_rms < RateLoadCurrLimit))//����ESC
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
						ESCFlag->ESC_DutyData = 1.0;//ռ�ձ���Ϊ0.93
						ESCFlag->PWM_ins_index = 1;//IGBTֱ̬ͨ
						Delayus(TIME_WRITE_15VOLT_REDAY);
						EnablePWMA();
						PIVolA.i10 = 0;//������
						PIVolA.i6 = 0;
						Delayus(100);//��Ҫ�Ӹ���ʱʱ��,����PWM�ļĴ���������������
						SET_IGBT_ENA(IGBT_ENABLE);//��IGBTʹ��
						SET_SCRA_ENABLE(ESC_ACTION_ENABLE);//����բ��
						SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_DISABLE);//����·
						StateEventFlag_A = STATE_EVENT_RUN_A;
					}
					if (ESCFlag->PHASE == 2)
					{
						ESCFlag->ESC_DutyData = 1.0;//ռ�ձ���Ϊ0.93
						ESCFlag->PWM_ins_index = 1;//IGBTֱ̬ͨ
						Delayus(TIME_WRITE_15VOLT_REDAY);
						EnablePWMB();
						PIVolB.i10 = 0;//������
						PIVolB.i6 = 0;
						Delayus(100);//��Ҫ�Ӹ���ʱʱ��,����PWM�ļĴ���������������
						SET_IGBT_ENB(IGBT_ENABLE);//��IGBTʹ��
						SET_SCRB_ENABLE(ESC_ACTION_ENABLE);//����բ��
						SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_DISABLE);//����·
						StateEventFlag_B = STATE_EVENT_RUN_B;
					}
					if (ESCFlag->PHASE == 3)
					{
						ESCFlag->ESC_DutyData = 1.0;//ռ�ձ���Ϊ0.93
						ESCFlag->PWM_ins_index = 1;//IGBTֱ̬ͨ
						Delayus(TIME_WRITE_15VOLT_REDAY);
						EnablePWMC();
						PIVolC.i10 = 0;//������
						PIVolC.i6 = 0;
						Delayus(100);//��Ҫ�Ӹ���ʱʱ��,����PWM�ļĴ���������������
						SET_IGBT_ENC(IGBT_ENABLE);//��IGBTʹ��
						SET_SCRC_ENABLE(ESC_ACTION_ENABLE);//����բ��
						SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_DISABLE);//����·
						StateEventFlag_C = STATE_EVENT_RUN_C;
					}
					ESCFlag->onceTimeStateMachine = 1;//״̬��ⴰ��
					ESCFlag->ESCCntMs.ContactorDelay = 0;//״̬��ⴰ�ڼ���������
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
							SET_POWER_CTRL(0);//�������඼ת�����״̬,���е�15V��Դ
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

	ESCFlag->PWM_ins_index = 1; //��·ֱͨ��WY

	if ((ESCFlag->ByPassContactFlag == 1) //��·�ű��̵ּ����պϡ�WY
			&& (ESCFlag->StateSkipFlag == 1)) //״̬��ת��־λ���ñ�־λ�ڡ�����ת������״̬�б���λ��WY
	{
		if (ESCFlag->PHASE == 1) //����A�ࡣ
		{
			SET_SCRA_ENABLE(ESC_ACTION_DISABLE); //�ض�A����·��բ�ܡ�WY
		}

		if (ESCFlag->PHASE == 2) //����B�ࡣ
		{
			SET_SCRB_ENABLE(ESC_ACTION_DISABLE);
		}

		if (ESCFlag->PHASE == 3) //����C�ࡣ
		{
			SET_SCRC_ENABLE(ESC_ACTION_DISABLE);
		}

		ESCFlag->StateSkipFlag = 0;
	}
}

/*
 * ���ܣ�ִ�С�����ͣ����״̬��WY
 * �������ESCFlag����ִ�е�X�������X��ȡֵ����Ϊ{A, B, C}��
 */
void ESCSTOPSTATE(ESCCTRLVALFLAG *ESCFlag)
{
	/*����A�ࡣWY*/
	if (ESCFlag->PHASE == 1)
	{
		SET_SCRA_ENABLE(ESC_ACTION_ENABLE); //��ͨA����·��բ�ܡ�WY
		SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //�պ�A����·�ű��̵ּ�����WY

		SET_IGBT_ENA(IGBT_DISABLE); //Ӳ��ʧ��A��PWM��WY
		DisablePWMA(); //�������A��PWM��WY

	}

	/*����B�ࡣWY*/
	if (ESCFlag->PHASE == 2)
	{
		SET_SCRB_ENABLE(ESC_ACTION_ENABLE);
		SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE);

		SET_IGBT_ENB(IGBT_DISABLE);
		DisablePWMB();

	}

	/*����C�ࡣWY*/
	if (ESCFlag->PHASE == 3)
	{
		SET_SCRC_ENABLE(ESC_ACTION_ENABLE);
		SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE);

		SET_IGBT_ENC(IGBT_DISABLE);
		DisablePWMC();

	}

	ESCFlag->onceTimeStateMachine = 1; //��λ���μ���־λ��WY
	ESCFlag->ESCCntMs.ContactorDelay = 0;//Ϊִ�С�������״̬����׼�����ñ����ڡ�����״̬���б�ʹ��WY
}

/*
 * ���ܣ�ִ�С�����ͣ����״̬��WY
 * �������ESCFlag����ִ�е�X�������X��ȡֵ����Ϊ{A, B, C}��
 */
void FAULTSTATE(ESCCTRLVALFLAG *ESCFlag)
{
	if ((ESCFlag->ByPassContactFlag == 1) //��·�ű��̵ּ����պϡ�WY
			&& (ESCFlag->FaultskipFlag == 1))
	{
		if (ESCFlag->PHASE == 1) //����A�ࡣWY
		{
			SET_SCRA_ENABLE(ESC_ACTION_DISABLE); //�ض�A����·��բ�ܡ�WY
		}

		if (ESCFlag->PHASE == 2) //����B�ࡣWY
		{
			SET_SCRB_ENABLE(ESC_ACTION_DISABLE);
		}

		if (ESCFlag->PHASE == 3) //����C�ࡣWY
		{
			SET_SCRC_ENABLE(ESC_ACTION_DISABLE);
		}

		ESCFlag->FaultskipFlag = 0;
	}
}


void FLAULTJUDGE(ESCCTRLVALFLAG *ESCFlag)         // �������ж���ɺ����
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
 * ���ܣ����Ƽ̵�����WY
 * �������ESCFlag����ִ�е�X�������X��ȡֵ����Ϊ{A, B, C}��
 * ˵�����˺��������ϵ��2sʱִ��1�Ρ�
 */
void RELAYCTRL(ESCCTRLVALFLAG *ESCFlag)
{
	if ((ESCFlag->ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))  //����ʱ����ʱ������:��ʱ��Ҫ��⡾��·�ű��̵ּ�����״̬��WY
			&& (ESCFlag->ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))) //����ʱ����ʱ��������ʱ��Ҫ��⡾��·�ű��̵ּ�����״̬��WY
	{
		if (ESCFlag->RELAYCONTROLFlag == 1) //��Ҫ�����̵�����WY
		{
			if ((ESCFlag->ByPassContactFlag == 1) //��·�ű��̵ּ����պϡ�WY
					&& (ESCFlag->ContactorFeedBackFlag == 0)) //��·�ű��̵ּ����Ͽ���WY
			{
				if (ESCFlag->PHASE == 1) //����A�ࡣWY
				{
					SET_GV_VOL_CTRL_A(1); //�պ�A��С�̵�����WY
				}

				if (ESCFlag->PHASE == 2)
				{
					SET_GV_VOL_CTRL_B(1);
				}

				if (ESCFlag->PHASE == 3)
				{
					SET_GV_VOL_CTRL_C(1);
				}

				ESCFlag->RELAYCONTROLFlag = 0; //С�̵�����������ɡ�WY
			}
		}
	}
}

int UnitSUCCESSCNT = 0; //δʹ�á�WY
int UnitDISSUCCESSCNT = 0; //δʹ�á�WY
int DelayFlagA = 0,DelayFlagB = 0,DelayFlagC = 0; //δʹ�á�WY

/*
 * ���ܣ�״̬����WY
 */
void SetStartCtrl(void)
{
	UInt events;

	while (1)
	{
		events = Event_pend(Event_SetStartSM, Event_Id_NONE, Event_Id_01, BIOS_WAIT_FOREVER);

		if (STOP_INSTRUCTION == 0)//ͣ����ť�����¡�WY
		{
			/*�����豸��WY*/
			if (CntMs.inputStop >= CNT_MS(6000))//�����豸��������ͣ����ť�����µ�ʱ������6s��WY
			{
				if ((!ESCFlagA.startFlag)//A������ִ������������WY
				&& (!ESCFlagB.startFlag)//A������ִ������������WY
						&& (!ESCFlagC.startFlag))//A������ִ������������WY
				{
					SoeRecData(SOE_GP_EVENT);//������־��WY
				}

				ESCFlagA.stopFlag = 0;//A������ִ��ͣ��������WY
				ESCFlagB.stopFlag = 0;//B������ִ��ͣ��������WY
				ESCFlagC.stopFlag = 0;//C������ִ��ͣ��������WY

				ESCFlagA.startFlag = 1;//A������������ȴ�ִ�С�WY
				ESCFlagB.startFlag = 1;//B������������ȴ�ִ�С�WY
				ESCFlagC.startFlag = 1;//C������������ȴ�ִ�С�WY

				ESCFlagA.onceRunStartFlag = 0;
				ESCFlagB.onceRunStartFlag = 0;
				ESCFlagC.onceRunStartFlag = 0;

				ESCFlagA.autoStFlag = ORIGINAL_STATE_A;//����A���Զ������Ĵ�����WY
				ESCFlagB.autoStFlag = ORIGINAL_STATE_B;//����B���Զ������Ĵ�����WY
				ESCFlagC.autoStFlag = ORIGINAL_STATE_C;//����C���Զ������Ĵ�����WY

				SET_POWER_CTRL(1);//15V��Դ�ϵ硣WY
				Delayus(TIME_WRITE_15VOLT_REDAY);
				SET_RUNNING_LED(0);//�豸����ָʾ����

				if ((StateEventFlag_A != STATE_EVENT_FAULT_A)//A��δ���ڹ���ͣ��״̬��WY
				&& (StateEventFlag_B != STATE_EVENT_FAULT_B)//B��δ���ڹ���ͣ��״̬��WY
						&& (StateEventFlag_C != STATE_EVENT_FAULT_C))//C��δ���ڹ���ͣ��״̬��WY
				{
					SET_FAULT_LED(0);//�豸����ָʾ����WY
				}
			}
		}
		else//ͣ����ťδ�����¡�WY
		{
			/*ֹͣ�豸��������ͣ����ť�����µ�ʱ����(0.36s, 2s)֮�� ��WY*/
			if ((CntMs.inputStop >= CNT_MS(360)) && (CntMs.inputStop <= CNT_MS(2000)))
			{
				if ((!ESCFlagA.stopFlag)//A������ִ��ͣ��������WY
				&& (!ESCFlagB.stopFlag)//B������ִ��ͣ��������WY
						&& (!ESCFlagC.stopFlag))//C������ִ��ͣ��������WY
				{
					SoeRecData(SOE_GP_EVENT + 1);//������־��WY
				}

				ESCFlagA.stopFlag = 1;//A��ȴ�ִ��ͣ��������WY
				ESCFlagB.stopFlag = 1;//B��ȴ�ִ��ͣ��������WY
				ESCFlagC.stopFlag = 1;//C��ȴ�ִ��ͣ��������WY

				ESCFlagA.startFlag = 0;//A������ִ������������WY
				ESCFlagB.startFlag = 0;//B������ִ������������WY
				ESCFlagC.startFlag = 0;//C������ִ������������WY

				if (ESCBYRelayCNTA != 1)//A����·�ű��̵ּ���������WY
				{
					ESCFlagA.HWPowerSTOPFlag = 1;
					ESCFlagA.ESCCntSec.HWPowerStopDelay = 0;
				}

				if (ESCBYRelayCNTB != 1)//B����·�ű��̵ּ���������WY
				{
					ESCFlagB.HWPowerSTOPFlag = 1;
					ESCFlagB.ESCCntSec.HWPowerStopDelay = 0;
				}

				if (ESCBYRelayCNTC != 1)//C����·�ű��̵ּ���������WY
				{
					ESCFlagC.HWPowerSTOPFlag = 1;
					ESCFlagC.ESCCntSec.HWPowerStopDelay = 0;
				}

				ESCFlagA.onceRunStartFlag = 1;
				ESCFlagB.onceRunStartFlag = 1;
				ESCFlagC.onceRunStartFlag = 1;

				ESCFlagA.autoStFlag = ORIGINAL_STATE_A;//����A���Զ������Ĵ�����WY
				ESCFlagB.autoStFlag = ORIGINAL_STATE_B;//����B���Զ������Ĵ�����WY
				ESCFlagC.autoStFlag = ORIGINAL_STATE_C;//����C���Զ������Ĵ�����WY
			}

			/*��ͣ����ťδ������ʱ���ü�ʱ������һֱ���㡣WY*/
			CntMs.inputStop = 0;//����ͣ����ť�����µ�ʱ����WY
		}

		FLAULTJUDGE(&ESCFlagA);//�ж�A����ϡ��ú������ɣ�WY
		FLAULTJUDGE(&ESCFlagB);//�ж�B����ϡ��ú������ɣ�WY
		FLAULTJUDGE(&ESCFlagC);//�ж�C����ϡ��ú������ɣ�WY

		RELAYCTRL(&ESCFlagA);//����A��С�̵�����WY
		RELAYCTRL(&ESCFlagB);//����B��С�̵�����WY
		RELAYCTRL(&ESCFlagC);//����C��С�̵�����WY

		/*A��״̬����WY*/
		switch (StateEventFlag_A)
		{
			/*A���ʼ״̬������ͣ������WY*/
			case STATE_EVENT_STANDBY_A:
			{
				ESCSTANDBYEVENT(&ESCFlagA); //ִ�г�ʼ״̬(����ͣ��)��WY
				ESCFlagA.stopFlag = 0; //A������ִ��ͣ��������WY

				if (ESCFlagA.startFlag == 1) //A����Ҫִ������������WY
				{
					SoeRecData(SOE_GP_EVENT); //������־����WY
					ESCFlagA.stopFlag = 0; //A������ִ��ͣ��������WY

					if ((GET_GV_VOL_CTRL_A == 1)  //A��С�̵����պϡ�WY
							&& (GET_BYPASS_FEEDBACK == 1)  //A����·�ű��̵ּ���״̬������WY
							&& (ESCBYRelayCNTA != 1) //΢�Ͷ�·���պϡ�WY
							&& (ESCSicFaultCNTA != 1)) //A��SiC��״̬������WY
					{
						StateEventFlag_A = STATE_EVENT_RECHARGE_A; //�л�״̬������A��Ԥ��硣WY
						ESCFlagA.BYFEEDBACKFLAG = 1; //A����·״̬������WY
					}

					ESCFlagA.ESCCntSec.ChargingTime = 0; //����Ԥ���ʱ�䡣�ñ�������Ԥ���״̬�б�ʹ�á�WY
				}
			}
				break;

			/*A��Ԥ��硣WY*/
			case STATE_EVENT_RECHARGE_A:
			{
				SET_RUNNING_LED(1); //�豸����ָʾ������WY
				SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE); //�պ�A����·�ű��̵ּ�����WY
				SET_IGBT_ENA(IGBT_DISABLE); //Ӳ��ʧ��A��PWM��WY
				DisablePWMA(); //����A��PWM��WY
				ESCPRECHARGEEVENT(&ESCFlagA); //ִ��A��Ԥ��硣WY

				/*����ͣ����WY*/
				if (ESCFlagA.stopFlag == 1)
				{
					ESCFlagA.stopFlag = 0; //A������ִ��ͣ��������WY
					ESCFlagA.startFlag = FALSE; //A������ִ������������WY
					ESCFlagA.PeakStopFlag = 1;
					StateEventFlag_A = STATE_EVENT_STOP_A; //�л�״̬������A������ͣ��״̬��WY
					SoeRecData(SOE_GP_EVENT + 1); //������־��WY
				}
			}
				break;

			/*A������*/
			case STATE_EVENT_RUN_A:
			{
				if (ESCFlagA.onceTimeStateMachine == 1) //����״̬��⡣WY
				{
					ESCFlagA.ESCCntMs.PllDelay = 0; //�������໷��������ʱ����WY

					if (ESCFlagA.ByPassContactFlag == 0) //A����·�ű��̵ּ����Ͽ���WY
					{
						SET_SCRA_ENABLE(ESC_ACTION_DISABLE); //�ض�A����·��բ�ܡ�WY
						ESCFlagA.onceTimeStateMachine = 0; //����״̬�������ɡ�WY
					}
				}
				ESCRUNEVENT(&ESCFlagA);//A�����С�WY

				ESCRUNTURNWAIT(&ESCFlagA); //A������ת������WY
				SET_RUNNING_LED(1); //�豸����ָʾ������WY

				/*����ͣ����WY*/
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

			/*A������ͣ��״̬��WY*/
			case STATE_EVENT_STOP_A:
			{
				ESCSTOPSTATE(&ESCFlagA); //ִ��A������ͣ��״̬��WY

				if (GET_BYPASS_FEEDBACK != 0)
				{
					SET_RUNNING_LED(0); //�豸ָʾ����WY
				}
#if !TEST_RUNPORCESS
				ExecuteOpenContact_A(); //�Ͽ�A����·�ű��̵ּ�����WY
#endif
				StateEventFlag_A = STATE_EVENT_STANDBY_A; //�л�״̬������A���ʼ״̬������ͣ������WY

				SoeRecData(SOE_GP_EVENT + 1); //������־��WY

				ESCFlagA.ESCCntSec.HWPowerStopDelay = 0;
				ESCFlagA.ESCCntSec.HWPowerFaultDelay = 0;
			}
				break;

			/*A�����״̬��WY*/
			case STATE_EVENT_WAIT_A:
			{
				ESCWAITTURNRUN(&ESCFlagA); //ִ��A�����������WY
				SET_RUNNING_LED(1); //�豸����ָʾ������WY

				/*��ͣ������WY*/
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

			/*A�����ͣ��״̬��WY*/
			case STATE_EVENT_FAULT_A:
			{
				ESCFlagA.startFlag = 0; //A������ִ������������WY
				ESCFlagA.stopFlag = 1; //A��ȴ�ִ��ͣ��������WY

				SET_RUNNING_LED(0); //�豸����ָʾ����WY
				FAULTSTATE(&ESCFlagA); //ִ��A�����ͣ��״̬��WY

				if (ESCBYRelayCNTA != 1) //A����·�ű��̵ּ���״̬������WY
				{
					ExecuteOpenContact_A();
				}

				SET_FAULT_LED(1);
			}
				break;
			default:
				break;
		}

		/*B��״̬����WY*/
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

		/*C��״̬����WY*/
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

		/*����A��*/
		if ((ESCFlagA.resetFlag == 1) //A����Ҫִ�и�λ������WY
				&& (StateEventFlag_A == STATE_EVENT_FAULT_A)) //����ͣ��״̬��WY
		{
			StateEventFlag_A = STATE_EVENT_STOP_A; //�л�״̬������A������ͣ��״̬��WY

			ESCFlagA.faultFlag = 0;
			ESCFlagA.startFlag = 0; //A������ִ������������WY
			ESCFlagA.FAULTCONFIRFlag = 0;
			ESCFlagA.stopFlag = 1; //A����Ҫִ��ͣ��������WY
			FaultClearPWMA();
			DisablePWMA(); //����A��PWM��WY
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

		/*����B��*/
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

		/*����C��*/
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
//    WindADVal2 = (int)ADC_RU_TEMP2;   //δʹ��
    FanTempValue0 = ntc_tab[(Uint16)WindADVal0];
    FanTempValue1 = ntc_tab[(Uint16)WindADVal1];
//    FanTempValue2 = ntc_tab[(Uint16)WindADVal2];
    TempData[0] = 0.7*TempData[0] + 0.3*FanTempValue0;
    TempData[1] = 0.7*TempData[1] + 0.3*FanTempValue1;
//    TempData[2] = 0.7*TempData[2] + 0.3*FanTempValue2;
    TempMaxValue = TempCompFuntion(TempData,2);   //�����¶�ȡ���ֵ
    CURRMaxValue = CURRCompFuntion(CURRData,6);   //��������������͸��ص�����ȡ���ֵ
    WindCold.HeatSinkTempterature = 0.9*WindCold.HeatSinkTempterature + 0.1*TempMaxValue;
    if(WindCold.HeatSinkTempterature > 150)      WindCold.HeatSinkTempterature = 150;
    if(WindCold.HeatSinkTempterature < (-30))    WindCold.HeatSinkTempterature = -30;

              if(GET_CTRL24_POWER != 1){
                  EPwm1Regs.AQCSFRC.bit.CSFA = 0;
              }
              if(TempEnvirProvalue == 1){    //��������ʱʹ��,�ٷְ�ռ�ձ�,����ȫ��ת
                  CTLFANDUTY = FANCtrlMaxDUTY;
                  PICTLFANVAL = 0.99*PICTLFANVAL + 0.01*CTLFANDUTY;
              }else{
                  if(  (StateEventFlag_A == STATE_EVENT_RUN_A)||\
                       (StateEventFlag_B == STATE_EVENT_RUN_B)||\
                       (StateEventFlag_C == STATE_EVENT_RUN_C)  ){
//        1.��30%-70%����,����30%,���40%ת��40-80����;��������ɵ�    ���ת��/�ض�Ӧ����������ת��/�¶ȶ�Ӧ������������ת��
//        2.60��ȫ��80%,40���������40%;��������ɵ�
//        �������ó�ռ�ձȰ����ֵ����.
                  //CTLFANDATA[0]=(��ǰ����ֵB0-��͸��ص����������B1)*(���ռ�ձ�0.8-���ռ�ձ�B3)/(��߸���ת��ȫ�ٷ��B4-��͸����������B1)+���ռ�ձ�B3;
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
                  //CTLFANDATA[1]=(��ǰ�¶�ֵ-����¶��������)*(���ռ�ձ�-���ռ�ձ�)/(����¶�ת��ȫ�ٷ��-����¶�)+���ռ�ձ�;
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
                      PICTLFANVAL = DUTYMUXFuntion(CTLFANDATA,2);   //ȡ�������ռ�ձ��������
              }else{
                  CTLFANDATA[0] = 0;
                  CTLFANDATA[1] = 0;
                  PICTLFANVAL = 0;
              }
          }
          EPwm1Regs.CMPA.bit.CMPA = PICTLFANVAL*FUNT1PR;

//          tem2 = WindCold.BOARD_OVER_TEMP - TempData[0];    //tem2 = ɢ��������ֵ80 - ɢ�����¶�;
//          if(tem2>20){
//              tem2 = 20.001;  //�¶ȳ���20��,����������
//          }
//          if(tem2<0){
//              tem2 = 0.0;     //�¶�С��0��,��С�������
//          }
//
//          tem2 = WindCold.UNIT_OVER_TEMP - TempData[1];           //tem2 = ��Ԫ�����ڲ��¶ȹ���ֵ 70 - ��Ԫ�ڲ��¶�;
//          if(tem2<5){
//              tem2 = 0.0;    //�¶�С��0��,��С�������
//          }
//          CurrTargetTemper = CurTarget[CapacitySelection][(Uint16)(tem2)];//���๲��һ���¶�ֵ����

          //tem2 = ɢ��������ֵ80 - ɢ�����¶�;
          if((WindCold.BOARD_OVER_TEMP - TempData[0])>20){
              tem2 = 20.001;  //�¶ȳ���20��,����������
              CurrTargetTemper = CurTarget[CapacitySelection][(Uint16)(tem2)];//���๲��һ���¶�ֵ����
          }else if((WindCold.BOARD_OVER_TEMP - TempData[0]) < 0){
              tem2 = 0.0;     //�¶�С��0��,��С�������
              CurrTargetTemper = CurTarget[CapacitySelection][(Uint16)(tem2)];//���๲��һ���¶�ֵ����
          }else{
              tem2 = WindCold.BOARD_OVER_TEMP - TempData[0];
              CurrTargetTemper = CurTarget[CapacitySelection][(Uint16)(tem2)];//���๲��һ���¶�ֵ����
          }

          //tem2 = ��Ԫ�����ڲ��¶ȹ���ֵ 70 - ��Ԫ�ڲ��¶�;
          if((WindCold.UNIT_OVER_TEMP - TempData[1]) < 5){
              tem2 = 0.0;    //�¶�С��0��,��С�������
              CurrTargetTemper = CurTarget[CapacitySelection][(Uint16)(tem2)];//���๲��һ���¶�ֵ����
          }
}

void Fan_Cnt(void)
{
    if(++FAN_CTL_Mode >= /*4*/7)
        FAN_CTL_Mode=0;
}

float DUTYMUXFuntion(float CTLFANDATA[], int VAL)  //�������ռ�ձȱȽ�ȡ���ֵ
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

float CURRCompFuntion(float CURRData[], int VAL)   //��������/�����������ȡ���ֵ
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

float TempCompFuntion(float TempData[], int VAL)   //�����¶�ȡ���ֵ
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


/*�ú���δ���塣WY*/
void MainContactConfirm(Uint16 VAL)
{
}

// �ŵ����ǿ�ƽ���ŵ���򣬲�������״̬�����ص���
void DischargingMode(void)
{
//	DINT;
//	EnablePWM();
//	SET_IGBT_EN1(IGBT_ENABLE);
//	// ��ͨ���趨Ϊ�ٷ�֮ʮ
//    EPwm4Regs.CMPA.bit.CMPA = 500;
//    EPwm5Regs.CMPA.bit.CMPA = 500;
//    EPwm6Regs.CMPA.bit.CMPA = 500;
//	EINT;
//	StateFlag.LEDRunFlag=ITS_WORKING_HIGH;
//    // ��Ӳ������ ���� ֱ����ѹ���͵�λ�����������״̬��
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
        if(++cntForRunEach >= 4)  // ���λ����N���ͱ�ʾmain��������ѯN-1��
            cntForRunEach = 0;

        if((SPLL[0].PllPiOutput>314.6)||(SPLL[0].PllPiOutput<313.8))													//�������
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
            StateFlag.PLLSafetyFlag = 1;	//��������
        }
        if(StateFlag.SoeFlag&& !StateFlag.EEPROMResourceLock)			SOE_Write();
    //  if(RecordWaveFlag&&!StateFlag.EEPROMResourceLock&&(StateEventFlag==STATE_EVENT_FAULT))    SOE_Wave_Record();

            switch(cntForRunEach){
            case 0:		// 1  �¶Ȳ����ӳ���
//                temperature3 = (int)(ADC_RU_TEMP0 - VirtulAD.temperature3);
//                temperature2 = (int)(ADC_RU_TEMP1 - VirtulAD.temperature2);
//
//                temperature2 = ntc_tab[temperature2];
//                tempT2 = (float)temperature2 * 0.1;			//��Ԫ����2
//                WindCold.HeatSinkTempterature 		= 0.88*WindCold.HeatSinkTempterature + 0.12*tempT2;
//                if(WindCold.HeatSinkTempterature>200)WindCold.HeatSinkTempterature=200;
//
//                temperature3 = (int)(ADC_RU_TEMP0 - VirtulAD.temperature3);
//                temperature3 = ntc_tab[temperature3];
//                tempT3 = (float)temperature3 * 0.1;         //��Ԫ����2
//                WindCold.EnvirTemperature       = 0.88*WindCold.EnvirTemperature + 0.12*tempT3;
//                if(WindCold.EnvirTemperature>200)WindCold.EnvirTemperature=200;
//
//                tempOnBoard =(LMT84_TEMPERATURE_COEFF_B-(LMT84_TEMPERATURE_COEFF_K*ADC_RU_DSPTEMP))* 0.1;	//JCL:T3��Ϊ���Ӱ����¶ȴ���
//                WindCold.MotherBoardTempterature = 0.88*WindCold.MotherBoardTempterature + 0.12*tempOnBoard;
//                if(WindCold.MotherBoardTempterature>200)WindCold.MotherBoardTempterature=200;

                FunAutoControl();// �Ե�Ԫ�ڲ�����������Ŀ���

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
                        SoeRecData(SOE_GP_EVENT + 7);         //ESC��·΢�������й�����,���ֶ������,����SOE����ʾ����,�豸ת������ͣ��״̬.//��Ҫ�Ĺ�Լ--��·΢�����������
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
///************������ж���Ϊ������������ʱ,�������·��е���غ�,15V������ȶϿ�,Ȼ����60s֮����ٴκ�15V����,����ʱ�Ѿ������Զ�������,
// ************���Է�ֹͣ��ʱ,�豸15V����һֱ��,�������,���ϸú����ж�,����һ��ʱ����ٶϿ�15V����.************/
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
                if((ESCBYRelayCNTA == 1)||(ESCSicFaultCNTA == 1)){  //�ߵ�ѹ�ű��̵ּ���/sic�����𻵹��ϼ��������ﵽ5�μ�5��֮��,�豸�����Զ�����
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
//���ж��������豸������ʱ��������,�л���·ʱ,��·�ű��̵ּ���δ����,Ȼ���⸺�ص���С��1A300MS,Ȼ��ʹ�ߵ�ѹ�ű��̵ּ�����բ,����ֱͨ�����ָ��ز���ʱ�����;
//���ж�ʹ���˸��ص������ж�,��֪��������׼,����и��ر��������С�����,���жϾ���������????
//                if((gridCurrBYAF_rms < 1.0f)&&(StateEventFlag_A == STATE_EVENT_FAULT_A)){
//                    if(ESCFlagA.ESCCntMs.CutCurrDelay > CNT_MS(300)){
//                        ESCFlagA.PWM_ins_index = 1;
//                        ESCFlagA.ESC_DutyData = 1.0;
//                        SET_SCRA_ENABLE(ESC_ACTION_ENABLE);                 //����բ��
//                        SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_ENABLE);
//                        SET_BYPASS_CONTACT_ACTION_A(ESC_ACTION_ENABLE);     //����·
//                        Delayus(TIME_WRITE_15VOLT_REDAY);
//                        EnablePWMA();
//                        Delayus(100);                                                      //��Ҫ�Ӹ���ʱʱ��,����PWM�ļĴ���������������
//                        SET_IGBT_ENA(IGBT_ENABLE);                                         //��IGBTʹ��
//                        EPwm3Regs.CMPA.bit.CMPA = T1PR;     //1��ֱͨ      //����Ӳ���߼�����,CMPA,CMPB��������ֵ,�������ĸ�����ȫͨ.
//                        EPwm3Regs.CMPB.bit.CMPB = T1PR;     //2�ܲ�ֱͨ
//                        EPwm4Regs.CMPA.bit.CMPA = T1PR;     //3��ֱͨ
//                        EPwm4Regs.CMPB.bit.CMPB = T1PR;     //4�ܲ�ֱͨ
//                        ESCBYRelayCNTA = 1;
//                    }
//                }
//                if((gridCurrBYBF_rms < 1.0f)&&(StateEventFlag_B == STATE_EVENT_FAULT_B)){
//                    if(ESCFlagB.ESCCntMs.CutCurrDelay > CNT_MS(300)){
//                        ESCFlagB.PWM_ins_index = 1;
//                        ESCFlagB.ESC_DutyData = 1.0;
//                        SET_SCRB_ENABLE(ESC_ACTION_ENABLE);                 //����բ��
//                        SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_ENABLE);
//                        SET_BYPASS_CONTACT_ACTION_B(ESC_ACTION_ENABLE);     //����·
//                        Delayus(TIME_WRITE_15VOLT_REDAY);
//                        EnablePWMB();
//                        Delayus(100);                                                      //��Ҫ�Ӹ���ʱʱ��,����PWM�ļĴ���������������
//                        SET_IGBT_ENB(IGBT_ENABLE);                                         //��IGBTʹ��
//                        EPwm5Regs.CMPA.bit.CMPA = T1PR;     //1��ֱͨ      //����Ӳ���߼�����,CMPA,CMPB��������ֵ,�������ĸ�����ȫͨ.
//                        EPwm5Regs.CMPB.bit.CMPB = T1PR;     //2�ܲ�ֱͨ
//                        EPwm6Regs.CMPA.bit.CMPA = T1PR;     //3��ֱͨ
//                        EPwm6Regs.CMPB.bit.CMPB = T1PR;     //4�ܲ�ֱͨ
//                        ESCBYRelayCNTB = 1;
//                    }
//                }
//                if((gridCurrBYCF_rms < 1.0f)&&(StateEventFlag_C == STATE_EVENT_FAULT_C)){
//                    if(ESCFlagC.ESCCntMs.CutCurrDelay > CNT_MS(300)){
//                        ESCFlagC.PWM_ins_index = 1;
//                        ESCFlagC.ESC_DutyData = 1.0;
//                        SET_SCRC_ENABLE(ESC_ACTION_ENABLE);                 //����բ��
//                        SET_MAIN_CONTACT_ACTION_C(ESC_ACTION_ENABLE);
//                        SET_BYPASS_CONTACT_ACTION_C(ESC_ACTION_ENABLE);     //����·
//                        Delayus(TIME_WRITE_15VOLT_REDAY);
//                        EnablePWMC();
//                        Delayus(100);                                                      //��Ҫ�Ӹ���ʱʱ��,����PWM�ļĴ���������������
//                        SET_IGBT_ENC(IGBT_ENABLE);                                         //��IGBTʹ��
//                        EPwm7Regs.CMPA.bit.CMPA = T1PR;     //1��ֱͨ      //����Ӳ���߼�����,CMPA,CMPB��������ֵ,�������ĸ�����ȫͨ.
//                        EPwm7Regs.CMPB.bit.CMPB = T1PR;     //2�ܲ�ֱͨ
//                        EPwm8Regs.CMPA.bit.CMPA = T1PR;     //3��ֱͨ
//                        EPwm8Regs.CMPB.bit.CMPB = T1PR;     //4�ܲ�ֱͨ
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
        //��������
            if(StateFlag.CapDataParamWrite && !StateFlag.EEPROMResourceLock)		//��������
            {
                StateFlag.CapDataParamWrite = false;
                CapParamSetting();
                CapParamRefresh();
            }

            if(StateFlag.ManufacturerParameters && !StateFlag.EEPROMResourceLock)		//�³��Ҳ���
            {
                StateFlag.ManufacturerParameters = false;
                ManufacturerParametersSetting();
                ManufacturerParametersRefresh();
                InitCtrlParam(1);
                ESCfilterMemCopy(CapacitySelection);
            }

            if(StateFlag.VitruZeroParameters && !StateFlag.EEPROMResourceLock)		//��ƫ
            {
                StateFlag.VitruZeroParameters = false;
                VirtuPreferencesSetting();
                VirtuPreferencesRefresh();
            }

            if(StateFlag.UserPreferences && !StateFlag.EEPROMResourceLock)				//���û�����
            {
                StateFlag.UserPreferences = false;
                UserPreferencesSetting();
                UserPreferencesRefresh();
            }

            if(StateFlag.IDPreferences && !StateFlag.EEPROMResourceLock)				//��ַƥ��
            {
                StateFlag.IDPreferences = false;
                IDPreferencesSetting();
                IDParametersRefresh();
            }

            if(StateFlag.harmCompPercParamRefresh&& !StateFlag.EEPROMResourceLock)		//г����������
            {
                StateFlag.harmCompPercParamRefresh = false;
                UserHarmnoicSetting();
                HarmnoicCorrection();
            }

            if(StateFlag.HarmonicWaveParamRefresh&& !StateFlag.EEPROMResourceLock)		//г����������
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
            if(CntMs.displayTimingCount>CNT_MS(500))	//���
            {
                CntMs.displayTimingCount = 0;
                Monitor_Message();
                RemoteParamerRefresh();
			    RemoteParamerRefresh2();
//                Multiple_Parallel_Message();
            }

            if(PWM_address == 0){						//����У����ʼ��
                if(StateFlag.SequenceAutoFlag)
                {
                    if(CntMs.InitializationDelay>CNT_MS(200)){
                        StateFlag.startingMethod = 0;
                    }else{
                        if((StateEventFlag_A==STATE_EVENT_RUN_A)&&(FactorySet.HarmonicInfer.B.ManualMode==0)&&(FactorySet.HarmonicInfer.B.AutomaticMode==0)){
                            StateFlag.SequenceAutoFlag = 0;                 //�Զ�г��У�����Զ�����У��������ͬʱ����
                        }else{
                            StateFlag.PhaseSequeJudSucceedFlag = 0;
                            FactorySet.Infer.B.VoltagePhaseSequence = 0;
                            FactorySet.Infer.B.PhaseSequenceCT = 0;
                            FactorySet.Infer.B.DirectionCT = 0;
                            Information_Structure.Correction.all = 0;
                            Information_Structure.Correction.B.DefectCT = FactorySet.Infer.B.DefectCT;
                            CorrectingAD();																//��ѹ����
                            CorrectingCT();																//��������
                            DirectionCT();																//��������
                            StateFlag.startingMethod = 1;

                            if(StateFlag.positionCT)													//���ز�
                            {
                                if((StateFlag.CurrACountFlag==0)&&(StateFlag.CurrBCountFlag==0)&&(StateFlag.CurrCCountFlag==0))
                                {
                                    if((SeqJud[0]<3)||(SeqJud[1]<3)||(SeqJud[2]<3)){	//���ػ���̫С �����Զ�У��
                                        StateFlag.SequenceAutoFlag = 0;
                                        Information_Structure.Correction.B.LoadingCondition = 1;
                                        Information_Structure.Correction.B.PhaseSequenResult = 1;
                                    }
                                }else{
                                    FFTDataReduction2(1,FFTCalcChan>>1);												//���㸺�ػ���
                                }
                                StateFlag.onceRunStartFlag = 1;
                            }else{																		//������
                                if((StateFlag.CurrACountFlag==0)&&(StateFlag.CurrBCountFlag==0)&&(StateFlag.CurrCCountFlag==0))
                                {
                                    if((SeqJud[0]>20)||(SeqJud[1]>20)||(SeqJud[2]>20)){	//���ظ���̫�� �����Զ�У��
                                        StateFlag.SequenceAutoFlag = 0;
                                        Information_Structure.Correction.B.LoadingCondition = 1;
                                        Information_Structure.Correction.B.PhaseSequenResult = 1;
                                    }
                                }else{
								    FFTDataReduction2(4,FFTCalcChan>>1);												//���Ĵ�г��ǰ���㸺���Ĵ�г��
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
                    FaultDetectInMainLoop();													//���ϼ��
//                    FaultDetectInRunning();
                }

                AutoStartInFault();
                BlindingBluetoothConnectionLED();
                SlaveStateInstruction();														//��������4B5Bָ��
                if(StateFlag.RxcFlag == 1)			// Get_Order();								//�ӻ�����4B5Bָ��
                if((StateFlag.WatchdogFlag)&&(CntMs.WatchdogDelay>CNT_MS(1000)))
                {
                    StateFlag.WatchdogFlag = 0;                      //ͨ��Ӧ���ִ�и�λ
                    EALLOW;
                    WdRegs.SCSR.all = 0;
                    WdRegs.WDCR.all = 0x0020;
                    EDIS;
                }

                if(UserSetting.WordMode.B.StandbyModeFlag)                                  //�������ܿ���
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
//------------ ����¶ȹ��ߣ���Ҫ�ѵ����޷�-------------------//
/*	��������¼���������Ȼ���¼����ֵ����Ϊ��ߵ����޷���
*  ֻҪ��һ�������¶��������-5�����趨��ʱ������ֵΪ�޷���
*  ���¶Ȼص��¶����Ƶ�-10�����𲽷ſ������޷���
*	// �����Ӻ��趨�������0.97�����������Ӻ󻹹��£�������趨0.97����ֱ������һ���¶��ڡ���֮��Ȼ
*	�κ�ʱ�򶼿��Ե��ڣ�
*
*	��������߻������ͻ�������У���Ҫ�����µĻ���
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


/* �Զ������ֶ�У����ƫ����ֻ��ͣ����ʱ�����ʹ��
 * ֻ�м����������ʹ�ã�1�����������ƣ�2��������Ϊ���ѹ���������3����������������ߵ�ѹ��Ϊ��
 * �������������Ƶ�����£��������ȡ20���ܲ��ĺ�����Ӧ��Ϊ�㣬��ʹ����һֱ���У���������Ҳ�ǳ����ġ�
 * ���ϵ���߸�λʱУ��һ��
 * һ��У׼ʧ���ǲ�����������
 */
#define AD_REGEDIT_OFFSET_NUM 9600
void AdRegeditOffset(void) // ������ƫֵ
{
	int  i=0;
	VirtulADStruVAL *pAD = &VirtulADVAL;
	Stru_Virtu_ZeroOffSETVAL *pID = &VirtuZeroOffSETVAL;
	if(adcAutoCount < AD_REGEDIT_OFFSET_NUM)  // ȡ��
	{
		tmpOffsetValue[0]  += *pAD->GridHVoltA;  //A15 ESC��ѹ��AN      //��9600�������(ֻ�����Ҳ����ܹ�������ƫУ׼)
		tmpOffsetValue[1]  += *pAD->GridHVoltB;  //B4  ESC��ѹ��BN
		tmpOffsetValue[2]  += *pAD->GridHVoltC;  //C2  ESC��ѹ��CN
		tmpOffsetValue[3]  += *pAD->GridLVoltA;  //D1  ESC��ѹ��AN
		tmpOffsetValue[4]  += *pAD->GridLVoltB;  //B5  ESC��ѹ��BN
		tmpOffsetValue[5]  += *pAD->GridLVoltC;  //A4  ESC��ѹ��CN
		tmpOffsetValue[6]  += *pAD->GridMainCurA;//A2  ESC���翹����A
		tmpOffsetValue[7]  += *pAD->GridMainCurB;//C3  ESC���翹����B
		tmpOffsetValue[8]  += *pAD->GridMainCurC;//D5  ESC���翹����C
		tmpOffsetValue[9]  += *pAD->GridBypassCurA; //A0  ESC��·����A
		tmpOffsetValue[10] += *pAD->GridBypassCurB; //B2  ESC��·����B
		tmpOffsetValue[11] += *pAD->GridBypassCurC; //D4  ESC��·����C
        tmpOffsetValue[12] += *pAD->ADCUDCA;     //D3  ESCֱ�����ݵ�ѹA
        tmpOffsetValue[13] += *pAD->ADCUDCB;     //A5  ESCֱ�����ݵ�ѹB
        tmpOffsetValue[14] += *pAD->ADCUDCC;     //C4  ESCֱ�����ݵ�ѹC

		adcAutoCount++;
	}
	else
	{
		for(i=0;i<15;i++)
		{
			tmpOffsetValue[i] *= (1.0f/AD_REGEDIT_OFFSET_NUM);    //���ɵ���9600����ȡƽ��ֵ
		}
		pID->gridVoltHAOffset = (int16)(tmpOffsetValue[0]+0.5f);  //�������ƽ��ֵ������ƫֵ(�����У��ֵ0.5f)   //�������ƫУ׼Ϊ������ƫֵ,Ȼ�󽫼���ֵ�ϴ�������,������޸��·���ֵ��ΪVariZeroOffsetVAL.gridHVoltA.
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
			tmpOffsetValue[i] = 0;         //����
		}
		adcAutoCount = 0;
		StateFlag.onceTimeAdcAutoAdjust = false;
	}
}

// �����Ժ󣬶Զ���ֵ�����ж���ȷ���Ƿ��ں���Χ�ڣ����ⶨ��ʧ�ܡ�
// ������LCD�������Զ����깦��
// ���ܣ�ֻ�����жϽ������Ķ�����
// 20150108 DSP REV6�ĵ�·�壬������·1.5V�е�ƫ��������У׼ֵΪ2140.�޸�����2200
void ZeroOffsetJudgment(void)			//����һЩͨ��û�����
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
 * MATLAB ������������  U0 = 0��ֻҪ�����ܵ�����Чֵ�ͻ���������Чֵ���ɡ�
THD= Uhrms /U1rms
where:
Uhrms =  rms value of harmonics = sqrt( U2^2 + U3^2 + U4^2....) =  sqrt( Urms^2 -Uo^2- U1rms^2)
U1rms =  rms value of the fundamenrtal component
Uo        =  DC component = mean value
Urms  = True rms value including harmonics and DC component
*/
// ������Чֵ�ļ��㷽�������ַ� 0814
/* ����
 * ����ͬ�����εĵ�ѹ�����ڴ������ϣ���ôdeltaT��ʱ���������Ϊ U^2/R*deltaT
 * Tʱ��������󣬵�ЧΪ  Urms^2/R*T
 * �õ�Urms = sqrt(U1^2*deltaT + ....)/T
 */

void THDiCal(void)
{
	int16 i;
	float32 temp,tThd;
	STRU_HarmTHD *pC = HarmTHD;
	if(StateFlag.cntForTHDi)
	{
		for(i=0;i<HARM_CALU_NUM;i++){
			temp=2*(pC->im*pC->im + pC->re*pC->re);     //г��ƽ��+����ƽ��
			if(temp>(0.3*0.3)){		//С��0.3A��ʾΪ0
			tThd=100*sqrtf((pC->rms -temp)/temp);
			}else{
				tThd=0;
			}
			if(tThd>1000) tThd=1000;
			if(tThd<-1000) tThd=-1000;
			if((i>2)&&(i<6)&&(THD_K!=0))
			tThd = THD_K*tThd+THD_B;
			pC->THD=0.9*pC->THD + 0.1*tThd;		// ��Ϊһֱ���㣬���û����˲��ķ�ʽ���У����Ա���С����������
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
/* �������ܣ����Ϻ󣬾����Լ죬���豸Ӳ����ȫ���ҵ�����������Ҫ�����Զ�����
 * ������׼���ֳ�����ְ�أ��ش�ƫƧ����������Σ����
 * ������ʶλ��   startingMethod   0�Զ�  1�ֶ�
 * ʵ�ַ���������������ѹ�����һ���Ӻ���������
 *     �����������ڵ������ϡ�����������ֱ����ѹ��ѹ��ģ�����
 * ͣ��Ҫ��24Сʱ������������N�Σ���ȡ����������N=10
 * ����λ�ã�main
 * �����LCD�·���ͣ��ָ�����Ҫ����ͣ����
 * �и�bug�������ڵ��Թ����У����й�����ֱ���·����Զ��������Ƚ����������״̬�������ٽ��벹��ģʽ��
 */
void AutoStartInFault(void)
{
	if(StateFlag.startingMethod == 0){  	//�Զ�
		switch(ESCFlagA.autoStFlag){
		case ORIGINAL_STATE_A:			//��ʼ״̬
         if(ESCFlagA.VoltageModeFlag == 0){
             if((ESCFlagA.ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))&&(ESCFlagA.ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))){  //�ϵ�����֮��С�̵����ڶ���;
                 if(ESCFlagA.RELAYCONTROLFlag == 1){
                     if((ESCFlagA.ByPassContactFlag == 1)&&(ESCFlagA.ContactorFeedBackFlag == 0)){
                         SET_GV_VOL_CTRL_A(1);
                         ESCFlagA.RELAYCONTROLFlag = 0;
                     }
                }
             }
            if((ESCFlagA.ESCCntMs.StartDelay>CNT_MS(60000))){//60��
                /*********ƽʱ������Ҫ�������ε�,��������ٴ�***************/
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
            if((ESCFlagA.ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))&&(ESCFlagA.ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))){  //�ϵ�����֮��С�̵����ڶ���;
                if(ESCFlagA.RELAYCONTROLFlag == 1){
                    if((ESCFlagA.ByPassContactFlag == 1)&&(ESCFlagA.ContactorFeedBackFlag == 0)){
                        SET_GV_VOL_CTRL_A(1);
                        ESCFlagA.RELAYCONTROLFlag = 0;
                    }
                }
            }
                if((ESCFlagA.ESCCntMs.StartDelay>CNT_MS(60000))){//60��
                   /*********ƽʱ������Ҫ�������ε�,��������ٴ�***************/
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
		case AUTO_DETECTION_STATE_A:		//�Լ�״̬
//			if(!softwareFaultWord3.B.ESCCalibrarionFailureFlag)	//�����ƫ
//			{
				if(ESCFlagA.realFaultFlag == 1){
					cntForRepFaultA++;                   // ��ǰ�����˼���
					ESCFlagA.realFaultFlag = 0;
				}
				if((!ESCFlagA.onceRunStartFlag)||(StateEventFlag_A==STATE_EVENT_RUN_A)){
				        ESCFlagA.onceRunStartFlag = 1;
				        ESCFlagA.autoStFlag = TWEAK_STATE_A; 	//switch��ת��case:2
				}
//				StateFlag.resetFlag = 1;                    //��λ
               if((ESCBYRelayCNTA != 1)&&(ESCSicFaultCNTA != 1)){
                   ESCFlagA.resetFlag = 1;
               }
//			}
		break;
		case TWEAK_STATE_A:			//ģʽת��
		    switch(StateEventFlag_A){
			case STATE_EVENT_STANDBY_A:
			     ESCFlagA.startFlag = 1;    //���Ǳ��õ�Ԫ,�Ϳ��Զ�����
			break;
			case STATE_EVENT_RECHARGE_A:	ESCFlagA.ESCCntMs.SelfJc = 0;			 break;
            case STATE_EVENT_WAIT_A:      ESCFlagA.ESCCntMs.SelfJc = 0;            break;
			case STATE_EVENT_RUN_A:
//		    	I_ins_index = 0.005;    							//I_ins_index ���ܴ���0.01
				if(ESCFlagA.ESCCntMs.SelfJc > CNT_MS(3000))						// ��������״̬
			    {
					ESCFlagA.autoStFlag = INSPECTION_STATE_A;
			    }
		    break;
			case STATE_EVENT_FAULT_A:
			    ESCFlagA.autoStFlag = ORIGINAL_STATE_A;
				ESCFlagA.realFaultFlag = 1;
				if(ESCFlagA.ESCCntMs.StartDelay>CNT_MS(7000))	ESCFlagA.ESCCntMs.StartDelay = CNT_MS(7000);        //ͬ�����Ա�����
		    break;
		    }
		break;

		case INSPECTION_STATE_A:			// ��ʱ����״̬0����������&&���ϣ�
			if((StateEventFlag_A == STATE_EVENT_FAULT_A)||(StateEventFlag_A == STATE_EVENT_STANDBY_A))
			{
				if(StateEventFlag_A == STATE_EVENT_FAULT_A){
				    ESCFlagA.realFaultFlag = 1;
					if(ESCFlagA.ESCCntMs.StartDelay>CNT_MS(15000))	ESCFlagA.ESCCntMs.StartDelay = CNT_MS(15000);                        //ͬ�����Ա�����
			     }else{
			         ESCFlagA.realFaultFlag = 0;
			     }
				ESCFlagA.autoStFlag = ORIGINAL_STATE_A;
			}
		break;
		default:break;
		}

//		if(cntForAutoStIn30 <= 30){		//esc��������30�������ظ�������բ��
//			if(softwareFaultWord3.B.ESCCalibrarionFailureFlag == 0){
//				softwareFaultWord3.B.ESCCalibrarionFailureFlag = FaultDetectExtCnt(SOE_GP_FAULT+38,10,cntForRepFaultA);	// ����10��
//				if(softwareFaultWord3.B.ESCCalibrarionFailureFlag){
//					StateFlag.startingMethod = 1;        //�Ͻ�����������ͨ���˹��·��Զ�������ʶ���и�ֵ��
//					if(PWM_address == 0)		StateFlag.StateInstruction = 3;		//�������Ϲ��
//				}
//			}
//		}else{
//		    cntForAutoStIn30 = 0;
//			cntForRepFaultA = 0;
//		}

        switch(ESCFlagB.autoStFlag){
        case ORIGINAL_STATE_B:          //��ʼ״̬
         if(ESCFlagB.VoltageModeFlag == 0){
             if((ESCFlagB.ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))&&(ESCFlagB.ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))){  //�ϵ�����֮��С�̵����ڶ���;
                 if(ESCFlagB.RELAYCONTROLFlag == 1){
                     if((ESCFlagB.ByPassContactFlag == 1)&&(ESCFlagB.ContactorFeedBackFlag == 0)){
                         SET_GV_VOL_CTRL_B(1);
                         ESCFlagB.RELAYCONTROLFlag = 0;
                     }
                }
             }
            if((ESCFlagB.ESCCntMs.StartDelay>CNT_MS(60000))){//60��
                /*********ƽʱ������Ҫ�������ε�,��������ٴ�***************/
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
            if((ESCFlagB.ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))&&(ESCFlagB.ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))){  //�ϵ�����֮��С�̵����ڶ���;
                if(ESCFlagB.RELAYCONTROLFlag == 1){
                    if((ESCFlagB.ByPassContactFlag == 1)&&(ESCFlagB.ContactorFeedBackFlag == 0)){
                        SET_GV_VOL_CTRL_B(1);
                        ESCFlagB.RELAYCONTROLFlag = 0;
                    }
                }
            }
            if((ESCFlagB.ESCCntMs.StartDelay>CNT_MS(60000))){//60��
               /*********ƽʱ������Ҫ�������ε�,��������ٴ�***************/
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
        case AUTO_DETECTION_STATE_B:        //�Լ�״̬
//            if(!softwareFaultWord3.B.ESCCalibrarionFailureFlag)    //�����ƫ
//            {
                if(ESCFlagB.realFaultFlag == 1){
                    cntForRepFaultB++;                   // ��ǰ�����˼���
                    ESCFlagB.realFaultFlag = 0;
                }
                if((!ESCFlagB.onceRunStartFlag)||(StateEventFlag_B==STATE_EVENT_RUN_B)){
                        ESCFlagB.onceRunStartFlag = 1;
                        ESCFlagB.autoStFlag = TWEAK_STATE_B;    //switch��ת��case:2
                }
//                StateFlag.resetFlag = 1;                    //��λ
                if((ESCBYRelayCNTB != 1)&&(ESCSicFaultCNTB != 1)){
                    ESCFlagB.resetFlag = 1;
                }
//            }
        break;
        case TWEAK_STATE_B:         //ģʽת��
            switch(StateEventFlag_B){
            case STATE_EVENT_STANDBY_B:
                 ESCFlagB.startFlag = 1;    //���Ǳ��õ�Ԫ,�Ϳ��Զ�����
            break;
            case STATE_EVENT_RECHARGE_B:    ESCFlagB.ESCCntMs.SelfJc = 0;            break;
            case STATE_EVENT_WAIT_B:      ESCFlagB.ESCCntMs.SelfJc = 0;            break;
            case STATE_EVENT_RUN_B:
//              I_ins_index = 0.005;                                //I_ins_index ���ܴ���0.01
                if(ESCFlagB.ESCCntMs.SelfJc > CNT_MS(3000))                     // ��������״̬
                {
                    ESCFlagB.autoStFlag = INSPECTION_STATE_B;
                }
            break;
            case STATE_EVENT_FAULT_B:
                ESCFlagB.autoStFlag = ORIGINAL_STATE_B;
                ESCFlagB.realFaultFlag = 1;
                if(ESCFlagB.ESCCntMs.StartDelay>CNT_MS(7000))   ESCFlagB.ESCCntMs.StartDelay = CNT_MS(7000);        //ͬ�����Ա�����
            break;
            }
        break;

        case INSPECTION_STATE_B:            // ��ʱ����״̬0����������&&���ϣ�
            if((StateEventFlag_B == STATE_EVENT_FAULT_B)||(StateEventFlag_B == STATE_EVENT_STANDBY_B))
            {
                if(StateEventFlag_B == STATE_EVENT_FAULT_B){
                    ESCFlagB.realFaultFlag = 1;
                    if(ESCFlagB.ESCCntMs.StartDelay>CNT_MS(15000))  ESCFlagB.ESCCntMs.StartDelay = CNT_MS(15000);                        //ͬ�����Ա�����
                 }else{
                     ESCFlagB.realFaultFlag = 0;
                 }
                ESCFlagB.autoStFlag = ORIGINAL_STATE_B;
            }
        break;
        default:break;
        }
         switch(ESCFlagC.autoStFlag){
         case ORIGINAL_STATE_C:          //��ʼ״̬
          if(ESCFlagC.VoltageModeFlag == 0){
              if((ESCFlagC.ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))&&(ESCFlagC.ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))){  //�ϵ�����֮��С�̵����ڶ���;
                  if(ESCFlagC.RELAYCONTROLFlag == 1){
                      if((ESCFlagC.ByPassContactFlag == 1)&&(ESCFlagC.ContactorFeedBackFlag == 0)){
                          SET_GV_VOL_CTRL_C(1);
                          ESCFlagC.RELAYCONTROLFlag = 0;
                      }
                 }
              }
             if((ESCFlagC.ESCCntMs.StartDelay>CNT_MS(60000))){//60��
                 /*********ƽʱ������Ҫ�������ε�,��������ٴ�***************/
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
             if((ESCFlagC.ESCCntSec.PRECHARGEDelayBY >= CNT_SEC(2))&&(ESCFlagC.ESCCntSec.PRECHARGEDelay >= CNT_SEC(2))){  //�ϵ�����֮��С�̵����ڶ���;
                 if(ESCFlagC.RELAYCONTROLFlag == 1){
                     if((ESCFlagC.ByPassContactFlag == 1)&&(ESCFlagC.ContactorFeedBackFlag == 0)){
                         SET_GV_VOL_CTRL_C(1);
                         ESCFlagC.RELAYCONTROLFlag = 0;
                     }
                 }
             }
             if((ESCFlagC.ESCCntMs.StartDelay>CNT_MS(60000))){//60��
                /*********ƽʱ������Ҫ�������ε�,��������ٴ�***************/
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
         case AUTO_DETECTION_STATE_C:        //�Լ�״̬
//             if(!softwareFaultWord3.B.ESCCalibrarionFailureFlag)    //�����ƫ
//             {
                 if(ESCFlagC.realFaultFlag == 1){
                     cntForRepFaultC++;                   // ��ǰ�����˼���
                     ESCFlagC.realFaultFlag = 0;
                 }
                 if((!ESCFlagC.onceRunStartFlag)||(StateEventFlag_C==STATE_EVENT_RUN_C)){
                         ESCFlagC.onceRunStartFlag = 1;
                         ESCFlagC.autoStFlag = TWEAK_STATE_C;    //switch��ת��case:2
                 }
//                 StateFlag.resetFlag = 1;                    //��λ
                 if((ESCBYRelayCNTC != 1)&&(ESCSicFaultCNTC != 1)){
                     ESCFlagC.resetFlag = 1;
                 }
//             }
         break;
         case TWEAK_STATE_C:         //ģʽת��
             switch(StateEventFlag_C){
             case STATE_EVENT_STANDBY_C:
                  ESCFlagC.startFlag = 1;    //���Ǳ��õ�Ԫ,�Ϳ��Զ�����
             break;
             case STATE_EVENT_RECHARGE_C:    ESCFlagC.ESCCntMs.SelfJc = 0;            break;
             case STATE_EVENT_WAIT_C:      ESCFlagC.ESCCntMs.SelfJc = 0;            break;
             case STATE_EVENT_RUN_C:
 //              I_ins_index = 0.005;                                //I_ins_index ���ܴ���0.01
                 if(ESCFlagC.ESCCntMs.SelfJc > CNT_MS(3000))                     // ��������״̬
                 {
                     ESCFlagC.autoStFlag = INSPECTION_STATE_C;
                 }
             break;
             case STATE_EVENT_FAULT_C:
                 ESCFlagC.autoStFlag = ORIGINAL_STATE_C;
                 ESCFlagC.realFaultFlag = 1;
                 if(ESCFlagC.ESCCntMs.StartDelay>CNT_MS(7000))   ESCFlagC.ESCCntMs.StartDelay = CNT_MS(7000);        //ͬ�����Ա�����
             break;
             }
         break;

         case INSPECTION_STATE_C:            // ��ʱ����״̬0����������&&���ϣ�
             if((StateEventFlag_C == STATE_EVENT_FAULT_C)||(StateEventFlag_C == STATE_EVENT_STANDBY_C))
             {
                 if(StateEventFlag_C == STATE_EVENT_FAULT_C){
                     ESCFlagC.realFaultFlag = 1;
                     if(ESCFlagC.ESCCntMs.StartDelay>CNT_MS(15000))  ESCFlagC.ESCCntMs.StartDelay = CNT_MS(15000);                        //ͬ�����Ա�����
                  }else{
                      ESCFlagC.realFaultFlag = 0;
                  }
                 ESCFlagC.autoStFlag = ORIGINAL_STATE_C;
             }
         break;
         default:break;
         }
	}else{ 										//�ֶ�
	    ESCFlagA.autoStFlag = ORIGINAL_STATE_A;
	    ESCFlagB.autoStFlag = ORIGINAL_STATE_B;
	    ESCFlagC.autoStFlag = ORIGINAL_STATE_C;
		cntForRepFaultA = 0;
		cntForRepFaultB = 0;
		cntForRepFaultC = 0;
	}
}

/*
 * ���ܣ���A����·�ű��̵ּ����պϣ�����Ͽ���WY
 */
void ExecuteOpenContact_A(void)
{
	if (GET_MAIN_CONTACT_ACTION_A == ESC_ACTION_ENABLE)
	{
		SET_MAIN_CONTACT_ACTION_A(ESC_ACTION_DISABLE);
	}
}

/*
 * ���ܣ���B����·�ű��̵ּ����պϣ�����Ͽ���WY
 */
void ExecuteOpenContact_B(void)
{
	if (GET_MAIN_CONTACT_ACTION_B == ESC_ACTION_ENABLE)
	{
		SET_MAIN_CONTACT_ACTION_B(ESC_ACTION_DISABLE);
	}
}

/*
 * ���ܣ���C����·�ű��̵ּ����պϣ�����Ͽ���WY
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
	currentRefD=0;												//ͣ��������,��Ϊ�Զ�����Ҳ��Ҫ�õ��������
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
        pAD->GridHVoltA = (int16 *)&ADC_RU_HVA;   //GridHVoltAָ��ָ��AdcaResultRegs.ADCRESULT2�洢�����ڴ��ַ
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
        ESCFlagA.stopFlag = 1;          //ֹͣ����
        ESCFlagB.stopFlag = 1;
        ESCFlagC.stopFlag = 1;
		StateFlag.SequenceAutoFlag = 0;
		StateFlag.ManufacturerParameters = true;					//�浽EEPROM������
		UserPreferencesRefresh();									//��ԭ�û�����
	}else{
		SequenceJudgeTimes++;
		if(SequenceJudgeTimes<3){									//���Զ��У��
			Information_Structure.Correction.B.DirectionCTFailFlag=0;
			StateFlag.MakeEnergyFlag=1;
			StateFlag.CurrACountFlag=1;
			StateFlag.CurrBCountFlag=1;
			StateFlag.CurrCCountFlag=1;
			CntMs.HarmonicSendCount = 0;
		}else{														 //����У��ʧ��
            ESCFlagA.stopFlag = 1;          //ֹͣ����
            ESCFlagB.stopFlag = 1;
            ESCFlagC.stopFlag = 1;
			StateFlag.SequenceAutoFlag = 0;
			Information_Structure.Correction.B.PhaseSequenResult = 1;
			Information_Structure.Correction.B.PhaseSequenCTFailFlag = ~StateFlag.PhaseSequeJudSucceedFlag;
			UserPreferencesRefresh();								 //��ԭ�û�����
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
		StateFlag.ManufacturerParameters = true;					//�浽EEPROM������
		UserPreferencesRefresh();									//��ԭ�û�����
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
			UserPreferencesRefresh();								//��ԭ�û�����
		}
	}
}

void AngleJudgePowerGrid(int N1,int N2,int N3,Uint16 number)	//������
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

void AngleJudgeLoad(int N1,int N2,int N3,Uint16 number)		//���ز๦����������0.5
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

void AngleJudgeInductanceLoad(int N1,int N2,int N3,Uint16 number)	//���ز๦������С��0.5
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

void AngleJudgeCapacitanceLoad(int N1,int N2,int N3,Uint16 number)	//���ز����Ը���(-0.5,0)
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
		APFReactPowerOutA= (GridVoltDA*CapReactPowerCurQPA*POWER_CONV_RATIO_CAP)/(MU_MultRatio);//APFӦ�÷������޹� = ���ݺ͸��ص��޹���
		APFReactPowerOutB= (GridVoltDB*CapReactPowerCurQPB*POWER_CONV_RATIO_CAP)/(MU_MultRatio);
		APFReactPowerOutC= (GridVoltDC*CapReactPowerCurQPC*POWER_CONV_RATIO_CAP)/(MU_MultRatio);
		APFReactPowerOut = (CapreactPowerCurQ * GridFundaVoltD * POWER_CONV_RATIO_CAP)/(MU_MultRatio);//APF�������޹���//ָ��
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
//	�������޹�
	GridActPower  =-( GridFundaVoltD*GridFundaCurD+GridFundaVoltQ*GridFundaCurQ )*(POWER_CONV_RATIO*SQRT3);
	GridReactPower=-( GridFundaVoltQ*GridFundaCurD-GridFundaVoltD*GridFundaCurQ )*(POWER_CONV_RATIO*SQRT3);
	GridApparentPower = sqrt(GridActPower*GridActPower+GridReactPower*GridReactPower);
//	������
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

	//���ݺ͸��ز�
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

	//��ƽ��ȼ���
//	MinVolRms    = Min(gpVoltA_rms,gpVoltB_rms,gpVoltC_rms);
	MinLoadCosFi = Min(LoadCosFiPhA,LoadCosFiPhB,LoadCosFiPhC);
	MaxGridCurRms = Max(gridCurA_rms,gridCurB_rms,gridCurC_rms);
	MaxLoadCurRms = Max(loadCurA_rms,loadCurB_rms,loadCurC_rms);
	GridCurRmsUnban = (MaxGridCurRms-GridCurRms_F)/GridCurRms_F;
	LoadCurRmsUnban = (MaxLoadCurRms-LoadCurRms_F)/LoadCurRms_F;
	//״̬����
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
//	Choose5=(int)(DccapVoltA*10);//����5ʼ����ʾ �����¶�
	switch(debugDispFlag){
	  case  0:
		  Choose1=GpioDataRegs.GPCDAT.bit.GPIO79;		Choose2=GpioDataRegs.GPCDAT.bit.GPIO78;        //A��ߵ�ѹ�ű��� ��·�ű���
		  Choose3=GpioDataRegs.GPEDAT.bit.GPIO156;		Choose4=GpioDataRegs.GPDDAT.bit.GPIO104;
		  break;
	  case  2:
		  Choose1=GpioDataRegs.GPEDAT.bit.GPIO151;			Choose2=GpioDataRegs.GPEDAT.bit.GPIO150;   //B��ߵ�ѹ�ű��� ��·�ű���
		  Choose3=GpioDataRegs.GPFDAT.bit.GPIO164;			Choose4=GpioDataRegs.GPDDAT.bit.GPIO104;
		  break;
	  case  3:
		  Choose1=GpioDataRegs.GPCDAT.bit.GPIO77;			Choose2=GpioDataRegs.GPEDAT.bit.GPIO152;   //C��ߵ�ѹ�ű��� ��·�ű���
		  Choose3=GpioDataRegs.GPCDAT.bit.GPIO93;			Choose4=GpioDataRegs.GPDDAT.bit.GPIO104;
		  break;
	  case  4:
		  Choose1=(int)(TempData[0]*10);			                Choose2=(int)(TempData[1]*10);     //ESC �����¶�--ɢ�����¶Ⱥ͵�Ԫ�ڲ��¶�
		  Choose3=(int)(WindCold.HeatSinkTempterature*10);			Choose4=0;
		  break;
	  case  5:
		  Choose1=(int)(StateEventFlag_A);				Choose2=(int)(StateEventFlag_B);               //ESC ����״̬ ��·΢�Ϸ���
		  Choose3=(int)(StateEventFlag_C);				Choose4=GpioDataRegs.GPCDAT.bit.GPIO92 ;
		  break;
	  case  6:
		  Choose1=(int)(ESCFlagA.ESC_DutyData*100);     Choose2=(int)(VoltInA_rms*100);                //ESC-A�����ռ�ձ�
		  Choose3=(int)(VoltOutA_rms*100);              Choose4=(int)(PIOutVoltValueA*10000);
		  break;
	  case  7:
		  Choose1=(int)(ESCFlagB.ESC_DutyData*100);		Choose2=(int)(VoltInB_rms*100);                //ESC-B�����ռ�ձ�
		  Choose3=(int)(VoltOutB_rms*100);				Choose4=(int)(PIOutVoltValueB*10000);
		  break;
	  case 8:
	      Choose1=(int)(ESCFlagC.ESC_DutyData*100);     Choose2=(int)(VoltInC_rms*100);                //ESC-C�����ռ�ձ�
	      Choose3=(int)(VoltOutC_rms*100);              Choose4=(int)(PIOutVoltValueC*10000);
		  break;
	  case 9:
		  Choose1=(int)(ESCBYRelayCNTA*10);			    Choose2=(int)(ESCBYRelayCNTB*10);              //ESC ��·�ű��̵ּ������߸ߵ�ѹ�ű��̵ּ����𻵹��ϱ�־λ
		  Choose3=(int)(ESCBYRelayCNTC*10);				Choose4=0;
		  break;
	  case 10:
		  Choose1=(int)(DccapVoltA*10);			Choose2=(int)(DccapVoltB*10);                          //ESC���յ��ݵ��ݵ�ѹ
		  Choose3=(int)(DccapVoltC*10);			Choose4=(int)(DccapVoltA*10);
		  break;
	  case 11:
          Choose1=(int)(gridCurA_rms*10);         Choose2=(int)(gridCurB_rms*10);                      //ESC-��������
          Choose3=(int)(gridCurC_rms*10);         Choose4=0;
		  break;
	  case 12:
		  Choose1=(int)(gridCurrBYAF_rms*10);     Choose2=(int)(gridCurrBYBF_rms*10);                  //ESC-��·����
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
          Choose1=(int)(PIOutVoltValueA*1000);           Choose2=(int)(PIOutVoltValueB*1000);             //boost��ѹPI�ջ���ǰ��ֵ
          Choose3=(int)(PIOutVoltValueC*1000);           Choose4=(int)(dutytmp);
          break;

      case 22:
          Choose1=(int)(ESCFlagA.PWM_ins_index);          Choose2=(int)(ESCFlagB.PWM_ins_index);             //����ģʽ��C�����Ŀ��ֵ
          Choose3=(int)(ESCFlagC.PWM_ins_index);          Choose4=0;
          break;

	  case 24:
		  Choose1 = (int)(VolttargetCorrA*1000);			Choose2 = (int)(VolttargetCorrB*1000);     //����ģʽ�����������PI��ĵ�ѹĿ��ֵϵ��
		  Choose3 = (int)(VolttargetCorrC*1000);			Choose4 = (int)(CurrTargetTemper*100);
		  break;
	  case 25:
//		  Choose1=(int)(SPLL[0].PllPiOutput*10);    Choose2=(int)(SPLL[0].Theta*1000);
//		  Choose3=(int)(SPLL[0].PLLResSin*1000);    Choose4=(int)(SPLL[0].PLLResCos*1000);
          Choose1=(int)(Esc_VoltPhaseA*100);    Choose2=(int)(Esc_VoltPhaseB*100);                     //ESC ������ѹ��������Ƕ�
          Choose3=(int)(Esc_VoltPhaseC*100);    Choose4=0;
		  break;
	  case 26:
          Choose1=(Uint16)(SPLL[0].PllPiOutput*100);                                                    //ESC ������ѹ��������ֵ
          Choose2=(Uint16)(SPLL[1].PllPiOutput*100);
          Choose3=(Uint16)(SPLL[2].PllPiOutput*100);
          Choose4=(int)(GridVoltRms*10);
		  break;
	  case 27:
	      Choose1=(int)(ESCFlagA.resetFlag*10);         Choose2=(int)(ESCFlagB.resetFlag*10);
	      Choose3=(int)(ESCFlagC.resetFlag*10);         Choose4=0;
	      break;
      case 28:
          Choose1=(int)(ESCSicFaultCNTA*10);            Choose2=(int)(ESCSicFaultCNTB*10);              //ESC SIC�����𻵱�־λ
          Choose3=(int)(ESCSicFaultCNTC*10);            Choose4=0;
          break;
	  case 29:
	      Choose1=(int)(ESCFlagA.HWPowerFAULTFlag);                                                     //ESC ���ϱ�־λ
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
          Choose1=(int)(ESCFlagA.HWPowerSTOPFlag);          Choose2=(int)(ESCFlagB.HWPowerSTOPFlag);     //ESC ͣ����־λ
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
	if(0){//((StateEventFlag == STATE_EVENT_RUN)&&(PIAdjustTim>=10)&&(PIAutoJust==0)){//��������ʱ10s,��ֹ��������
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
                   StateFlag.StartFlashFlag = 1;  //�ж��Ƿ�ʹ��Զ����д
                   RemoteStateCtrl.RemoteStart=0;
        }
                break;
            case 0x0001:
                if(RemoteStateCtrl.RemoteStop==0xFF00){
            if((!ESCFlagA.stopFlag)&&(!ESCFlagB.stopFlag)&&(!ESCFlagC.stopFlag))		SoeRecData(SOE_GP_EVENT+1);
                ESCFlagA.stopFlag = 1;			//ֹͣ����
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
//                    StateFlag.resetFlag = 1;		//��λ����
                    ESCFlagA.resetFlag = 1;
                    ESCFlagB.resetFlag = 1;
                    ESCFlagC.resetFlag = 1;
                    cntForRepFaultA = 0;				//��������
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


/* �൥Ԫ������Ƚ����⣺��λ��ȥ�������߼�����Ԫ������������ָ��ɡ�
 * 1����Ԫ֮�䲢�޹�����LCD�������߼����Ƶ����ã�������ʾ��������Ҫ��LCDʵ�ֹ��ܻ�����
 * 1118 ���Ӷ൥Ԫ��������������
 *
	�൥Ԫ��ѯ��
	�ļ����漰��ѯ��
	------------------------------------------------------------
	�����ˣ��м����� = ĳ����Ԫ������
	Ƶ����������־λ���м����� = ĳ����ԪƵ����������־λ
		�޹��ϱ�־ ���м�����= ĳ����Ԫ�޹��ϱ�־

		�����������ùܶ����Ԫ��ֱ���趨���ɡ�

	 //   ��Ԫ����Զ���ģʽ����Ԫ����һ���Ӻ��Զ���λ���ȴ�����ָ�
	--------------------------------------------------------------
	������״̬����
	if(������ == 1��
		���Ϻ����200���ʱ�� ++��
	else
		���Ϻ����200���ʱ�� = 0

	------------------------------------------------------------
	״̬0��if(״̬ == 0)
	if���������� == 1��&&��Ƶ����������־λ == 0����
		�ܽ�����������
		״̬ = 1��
	else ���������� == 1��&&��Ƶ����������־λ == 1����
		״̬ = 3��
		״̬1��if(״̬ == 1)
		��������־λ =1��
		״̬ = 2��
		��ʱ�� = 0��
	״̬2��if(״̬ == 2)
		if((�޹��ϱ�־ == 1)&(���Ϻ����200���ʱ�� >= 100))
		{
			��������ʱ��++��
			if����������ʱ��>= 40)&&(���Ϻ����200���ʱ�� >= 200)
			�������ӣ������·���ȫ����Ԫ������ָ�
			״̬0 = 1��
		}
		else  // �������ϸ�λ����������40���Թ���
		{
			״̬0 = 1��
		}
	״̬3��Ƶ���������ˣ��ȴ���ת״̬0
		   if����Ƶ����������־λ == 0��&&��״̬3��ʱ�� >= 30�룩
	{
	״̬0 = 1��
	}
 */

//*********************** NO MORE **************************//



//===========================================================================
// No more.
//===========================================================================

