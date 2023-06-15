#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "VariblesRef.h"
#include "MarcoDefine.h"

// ����Ӳ���źŵ���������������Ϊ����������2.
#if (THREE_LEVEL_PWM||NMECB180709REV3_1||NMECB180202REV3)
void FaultClearPWMA(void)
{
	SET_IGBT_ENA(IGBT_ENABLE);	//����1us
	asm(" RPT #127 || NOP");
	asm(" RPT #50 || NOP");
	SET_IGBT_ENA(IGBT_FAULT);//�ر����й�}
}
void FaultClearPWMB(void)
{
    SET_IGBT_ENB(IGBT_ENABLE);  //����1us
    asm(" RPT #127 || NOP");
    asm(" RPT #50 || NOP");
    SET_IGBT_ENB(IGBT_FAULT);//�ر����й�
}
void FaultClearPWMC(void)
{
    SET_IGBT_ENC(IGBT_ENABLE);  //����1us
    asm(" RPT #127 || NOP");
    asm(" RPT #50 || NOP");
    SET_IGBT_ENC(IGBT_FAULT);//�ر����й�
}

/*
 * ���ܣ�����A��PWM��WY
 */
void DisablePWMA(void)
{
    EPwm3Regs.AQCSFRC.bit.CSFA = 2; //PWM-3-A���ָߵ�ƽ��WY
    EPwm4Regs.AQCSFRC.bit.CSFA = 2; //PWM-4-A���ָߵ�ƽ��WY

    EPwm3Regs.AQCSFRC.bit.CSFB = 2; //PWM-3-B���ָߵ�ƽ��WY
    EPwm4Regs.AQCSFRC.bit.CSFB = 2; //PWM-3-B���ָߵ�ƽ��WY

    asm(" RPT #7 || NOP");
}

/*
 * ���ܣ�����B��PWM��WY
 */
void DisablePWMB(void)
{
    EPwm5Regs.AQCSFRC.bit.CSFA = 2; //PWM-5-A���ָߵ�ƽ��WY
    EPwm6Regs.AQCSFRC.bit.CSFA = 2; //PWM-6-A���ָߵ�ƽ��WY

    EPwm5Regs.AQCSFRC.bit.CSFB = 2; //PWM-5-B���ָߵ�ƽ��WY
    EPwm6Regs.AQCSFRC.bit.CSFB = 2; //PWM-6-B���ָߵ�ƽ��WY

    asm(" RPT #7 || NOP");
}

/*
 * ���ܣ�����C��PWM��WY
 */
void DisablePWMC(void)
{
    EPwm7Regs.AQCSFRC.bit.CSFA = 2; //PWM-7-A���ָߵ�ƽ��WY
    EPwm8Regs.AQCSFRC.bit.CSFA = 2; //PWM-8-A���ָߵ�ƽ��WY

    EPwm7Regs.AQCSFRC.bit.CSFB = 2; //PWM-7-A���ָߵ�ƽ��WY
    EPwm8Regs.AQCSFRC.bit.CSFB = 2; //PWM-8-A���ָߵ�ƽ��WY

    asm(" RPT #7 || NOP");
}


void FORCEPWM(void)
{
    EPwm3Regs.AQCSFRC.bit.CSFA = 1; //ǿ������ͣ������Ϊǿ�������
    EPwm4Regs.AQCSFRC.bit.CSFA = 1;
    EPwm5Regs.AQCSFRC.bit.CSFA = 1;
    EPwm6Regs.AQCSFRC.bit.CSFA = 1; //ǿ������ͣ������Ϊǿ�������
    EPwm7Regs.AQCSFRC.bit.CSFA = 1;
    EPwm8Regs.AQCSFRC.bit.CSFA = 1;

    EPwm3Regs.AQCSFRC.bit.CSFB = 1;  //ǿ������ͣ������Ϊǿ�������
    EPwm4Regs.AQCSFRC.bit.CSFB = 1;
    EPwm5Regs.AQCSFRC.bit.CSFB = 1;
    EPwm6Regs.AQCSFRC.bit.CSFB = 1;  //ǿ������ͣ������Ϊǿ�������
    EPwm7Regs.AQCSFRC.bit.CSFB = 1;
    EPwm8Regs.AQCSFRC.bit.CSFB = 1;
}


void EnablePWMA(void)
{
    EPwm3Regs.AQCSFRC.bit.CSFA = 0; //������ǿ���������Ч
    EPwm4Regs.AQCSFRC.bit.CSFA = 0;
    EPwm3Regs.AQCSFRC.bit.CSFB = 0;
    EPwm4Regs.AQCSFRC.bit.CSFB = 0;
}
void EnablePWMB(void)
{
    EPwm5Regs.AQCSFRC.bit.CSFA = 0;
    EPwm6Regs.AQCSFRC.bit.CSFA = 0; //������ǿ���������Ч
    EPwm5Regs.AQCSFRC.bit.CSFB = 0;
    EPwm6Regs.AQCSFRC.bit.CSFB = 0;
}
void EnablePWMC(void)
{
    EPwm7Regs.AQCSFRC.bit.CSFA = 0;
    EPwm8Regs.AQCSFRC.bit.CSFA = 0;
    EPwm7Regs.AQCSFRC.bit.CSFB = 0;
    EPwm8Regs.AQCSFRC.bit.CSFB = 0;
}

#elif THREE_LEVEL_PWM == 0
void FaultClearPWM(void){;}			//null function
void DisablePWM(void)
{
    //ǿ�Ʒ���PWM�ķ���
	EPwm4Regs.AQCSFRC.bit.CSFA = 2; //ǿ������� �������Ϊǿ�������
	EPwm5Regs.AQCSFRC.bit.CSFA = 2;
	EPwm6Regs.AQCSFRC.bit.CSFA = 2;
}

void EnablePWM(void)
{
	EPwm4Regs.AQCSFRC.bit.CSFA = 0; //������ǿ���������Ч
	EPwm5Regs.AQCSFRC.bit.CSFA = 0;
	EPwm6Regs.AQCSFRC.bit.CSFA = 0;
}
#else
#error "Please select the THREE_LEVEL_PWM
#endif
// 1104 �޸�PWM����Ƶ�ʵķ���
/*
// �޸Ŀ���Ƶ�� 6K    150M/2��Ƶ/6k/2�� = 6250
float pwmPeriodLimit = 5304;  // 6250*0.866*0.98 = 5304.25
volatile float PIlim_I = 6365;   //  6K   6250*0.98/0.866*0.9 = 6365
float T1PR = 6250;
EPwm1Regs.TBPRD = 6250;
voltForwardRatio = 6250*0.8*0.866/700=6.2
*/
// �����ظߵ�ƽ���½��ص͵�ƽ������źŷ���
//�����������趨Ϊ�͵�ƽ���½����趨Ϊ�ߵ�ƽ��0218

// T1=SQRT3*Ts*Ualpha/Udc = SQRT3*6250*Ualpha/Udc = 4058.0*Ualpha/Udc
/*
void PWMsector(void)
{
    float T1,T2,Tmp;
    float temp_32 =0;  //T1PR �Ƕ�ʱ�������ڣ��޸���鼴��ʹ�á�
	if(pwmPllOk == 0)
	{
		EPwm1Regs.TBPRD = T1PR;
		EPwm2Regs.TBPRD = T1PR;
		EPwm3Regs.TBPRD = T1PR;
		EPwm5Regs.TBPRD = T1PR;
		EPwm6Regs.TBPRD = T1PR;
	}


    // ���±任������֤����ȷ�ġ�
//    Ur1 = dq2Ubeta* SQRT_2DIV3;
//    Ur2 = -0.4082 * dq2Ubeta + dq2Ualpha * 0.707;
//-   Ur3 = -(Ur1 + Ur2);
//    Ur3 = -0.4082 * dq2Ubeta - dq2Ualpha * 0.707;

	dq2Ualpha *= udcForwardRatio;
	dq2Ubeta  *= udcForwardRatio;
    // ʵ�ʱ任Ϊ
    Ur1 = dq2Ubeta;
    Ur2 = 0.866*dq2Ualpha - 0.5*dq2Ubeta;
    Ur3 = -(Ur1 + Ur2);
    // ��任����ʵ�ʷ��򣬼� nPhaseCtrlVolt
 //     Ur1 += nPhaseCtrlVolt;
 //    Ur2 += nPhaseCtrlVolt;
 //    Ur3 += nPhaseCtrlVolt;
	if(Ur1 >= 0)                  //sector selection
	{
		if(Ur2 >= 0)		//Ur3 = -(Ur1+Ur2)
		{   sector = 1;              //sector1
		  	T1 = Ur2;
		    T2 = Ur1;
		}
		else
		{
			if(Ur3 >= 0)
			{	sector = 3;              //sector3
				T1 = Ur1;
				T2 = Ur3;
			}
			else
			{ 	sector = 2;              //sector2
				T1 = -Ur2;
				T2 = -Ur3;
			}
		}
	}
	else
	{
		if(Ur2 >= 0)
		{
		   if(Ur3 >= 0)
		   {  sector = 5;              //sector5
		   	  T1 = Ur3;
		      T2 = Ur2;
		   }
		   else
		   {  sector = 6;		      //sector6
		      T1 = -Ur3;
		      T2 = -Ur1;
		   }
		}
		else
		{  	sector = 4;                //sector4
		   	T1 = -Ur1;
		   	T2 = -Ur2;
		}
	}

	Tmp = T1 + T2;


	if(Tmp > T1PR - T_restrict)              //  if over  mudulation  happens
	{
	  	temp_32 = T1;
	  	temp_32 = temp_32 * (T1PR - T_restrict);
	  	T1 = temp_32 / Tmp; 		//restrict the minimum time of 000 and 111
	  	T2 = T1PR - T_restrict - T1;		//T2_new = T2_old*(Ts-200)/Tmp = Ts - 200 - T1_new;
	}

	T_Data1 = T1;
	T_Data2 = T2;


	Tu = (Uint16)(T1PR - T1 - T2);
	Tu = Tu >> 1;                      // the first
	Tv= (Uint16)(T1) + Tu;                // the second
	Tw= (Uint16)(T2) + Tv;               // the third


	Tu -= nPhaseCtrlVolt;
	Tv -= nPhaseCtrlVolt;
	Tw -= nPhaseCtrlVolt;

	if(PWM_ABC_FLAG == 1)
	{
		if(sector == 1)
		{
			 EPwm1Regs.CMPA.half.CMPA = Tu;
			 EPwm2Regs.CMPA.half.CMPA = Tv;
			 EPwm3Regs.CMPA.half.CMPA = Tw;
		}
		else if(sector == 2)
		{
			 EPwm1Regs.CMPA.half.CMPA = Tv;
			 EPwm2Regs.CMPA.half.CMPA = Tu;
			 EPwm3Regs.CMPA.half.CMPA = Tw;
		}
		else if(sector == 3)
		{
			 EPwm1Regs.CMPA.half.CMPA = Tw;
			 EPwm2Regs.CMPA.half.CMPA = Tu;
			 EPwm3Regs.CMPA.half.CMPA = Tv;
		}
		else if(sector == 4)
		{
			 EPwm1Regs.CMPA.half.CMPA = Tw;
			 EPwm2Regs.CMPA.half.CMPA = Tv;
			 EPwm3Regs.CMPA.half.CMPA = Tu;
		}
		else if(sector == 5)
		{
			 EPwm1Regs.CMPA.half.CMPA = Tv;
			 EPwm2Regs.CMPA.half.CMPA = Tw;
			 EPwm3Regs.CMPA.half.CMPA = Tu;
		}
		else
		{
			 EPwm1Regs.CMPA.half.CMPA = Tu;
			 EPwm2Regs.CMPA.half.CMPA = Tw;
			 EPwm3Regs.CMPA.half.CMPA = Tv;
		}
	}
	else
	{
		if(sector == 1)
		{
			 EPwm3Regs.CMPA.half.CMPA = Tu;
			 EPwm2Regs.CMPA.half.CMPA = Tv;
			 EPwm1Regs.CMPA.half.CMPA = Tw;
		}
		else if(sector == 2)
		{
			 EPwm3Regs.CMPA.half.CMPA = Tv;
			 EPwm2Regs.CMPA.half.CMPA = Tu;
			 EPwm1Regs.CMPA.half.CMPA = Tw;
		}
		else if(sector == 3)
		{
			 EPwm3Regs.CMPA.half.CMPA = Tw;
			 EPwm2Regs.CMPA.half.CMPA = Tu;
			 EPwm1Regs.CMPA.half.CMPA = Tv;
		}
		else if(sector == 4)
		{
			 EPwm3Regs.CMPA.half.CMPA = Tw;
			 EPwm2Regs.CMPA.half.CMPA = Tv;
			 EPwm1Regs.CMPA.half.CMPA = Tu;
		}
		else if(sector == 5)
		{
			 EPwm3Regs.CMPA.half.CMPA = Tv;
			 EPwm2Regs.CMPA.half.CMPA = Tw;
			 EPwm1Regs.CMPA.half.CMPA = Tu;
		}
		else
		{
			 EPwm3Regs.CMPA.half.CMPA = Tu;
			 EPwm2Regs.CMPA.half.CMPA = Tw;
			 EPwm1Regs.CMPA.half.CMPA = Tv;
		}
	}
}
*/
// ---------------------------- NO MORE --------------------------//

