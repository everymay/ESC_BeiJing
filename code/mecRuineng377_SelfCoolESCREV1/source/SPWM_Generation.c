#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "VariblesRef.h"
#include "MarcoDefine.h"

// 根据硬件信号的输出来看，如果高为封锁，则置2.
#if (THREE_LEVEL_PWM||NMECB180709REV3_1||NMECB180202REV3)
void FaultClearPWMA(void)
{
	SET_IGBT_ENA(IGBT_ENABLE);	//保持1us
	asm(" RPT #127 || NOP");
	asm(" RPT #50 || NOP");
	SET_IGBT_ENA(IGBT_FAULT);//关闭所有管}
}
void FaultClearPWMB(void)
{
    SET_IGBT_ENB(IGBT_ENABLE);  //保持1us
    asm(" RPT #127 || NOP");
    asm(" RPT #50 || NOP");
    SET_IGBT_ENB(IGBT_FAULT);//关闭所有管
}
void FaultClearPWMC(void)
{
    SET_IGBT_ENC(IGBT_ENABLE);  //保持1us
    asm(" RPT #127 || NOP");
    asm(" RPT #50 || NOP");
    SET_IGBT_ENC(IGBT_FAULT);//关闭所有管
}

/*
 * 功能：封锁A相PWM。WY
 */
void DisablePWMA(void)
{
    EPwm3Regs.AQCSFRC.bit.CSFA = 2; //PWM-3-A保持高电平。WY
    EPwm4Regs.AQCSFRC.bit.CSFA = 2; //PWM-4-A保持高电平。WY

    EPwm3Regs.AQCSFRC.bit.CSFB = 2; //PWM-3-B保持高电平。WY
    EPwm4Regs.AQCSFRC.bit.CSFB = 2; //PWM-3-B保持高电平。WY

    asm(" RPT #7 || NOP");
}

/*
 * 功能：封锁B相PWM。WY
 */
void DisablePWMB(void)
{
    EPwm5Regs.AQCSFRC.bit.CSFA = 2; //PWM-5-A保持高电平。WY
    EPwm6Regs.AQCSFRC.bit.CSFA = 2; //PWM-6-A保持高电平。WY

    EPwm5Regs.AQCSFRC.bit.CSFB = 2; //PWM-5-B保持高电平。WY
    EPwm6Regs.AQCSFRC.bit.CSFB = 2; //PWM-6-B保持高电平。WY

    asm(" RPT #7 || NOP");
}

/*
 * 功能：封锁C相PWM。WY
 */
void DisablePWMC(void)
{
    EPwm7Regs.AQCSFRC.bit.CSFA = 2; //PWM-7-A保持高电平。WY
    EPwm8Regs.AQCSFRC.bit.CSFA = 2; //PWM-8-A保持高电平。WY

    EPwm7Regs.AQCSFRC.bit.CSFB = 2; //PWM-7-A保持高电平。WY
    EPwm8Regs.AQCSFRC.bit.CSFB = 2; //PWM-8-A保持高电平。WY

    asm(" RPT #7 || NOP");
}


void FORCEPWM(void)
{
    EPwm3Regs.AQCSFRC.bit.CSFA = 1; //强制输出低，反向后为强制输出高
    EPwm4Regs.AQCSFRC.bit.CSFA = 1;
    EPwm5Regs.AQCSFRC.bit.CSFA = 1;
    EPwm6Regs.AQCSFRC.bit.CSFA = 1; //强制输出低，反向后为强制输出高
    EPwm7Regs.AQCSFRC.bit.CSFA = 1;
    EPwm8Regs.AQCSFRC.bit.CSFA = 1;

    EPwm3Regs.AQCSFRC.bit.CSFB = 1;  //强制输出低，反向后为强制输出高
    EPwm4Regs.AQCSFRC.bit.CSFB = 1;
    EPwm5Regs.AQCSFRC.bit.CSFB = 1;
    EPwm6Regs.AQCSFRC.bit.CSFB = 1;  //强制输出低，反向后为强制输出高
    EPwm7Regs.AQCSFRC.bit.CSFB = 1;
    EPwm8Regs.AQCSFRC.bit.CSFB = 1;
}


void EnablePWMA(void)
{
    EPwm3Regs.AQCSFRC.bit.CSFA = 0; //不允许强制输出，无效
    EPwm4Regs.AQCSFRC.bit.CSFA = 0;
    EPwm3Regs.AQCSFRC.bit.CSFB = 0;
    EPwm4Regs.AQCSFRC.bit.CSFB = 0;
}
void EnablePWMB(void)
{
    EPwm5Regs.AQCSFRC.bit.CSFA = 0;
    EPwm6Regs.AQCSFRC.bit.CSFA = 0; //不允许强制输出，无效
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
    //强制封锁PWM的方法
	EPwm4Regs.AQCSFRC.bit.CSFA = 2; //强制输出高 ，反向后为强制输出低
	EPwm5Regs.AQCSFRC.bit.CSFA = 2;
	EPwm6Regs.AQCSFRC.bit.CSFA = 2;
}

void EnablePWM(void)
{
	EPwm4Regs.AQCSFRC.bit.CSFA = 0; //不允许强制输出，无效
	EPwm5Regs.AQCSFRC.bit.CSFA = 0;
	EPwm6Regs.AQCSFRC.bit.CSFA = 0;
}
#else
#error "Please select the THREE_LEVEL_PWM
#endif
// 1104 修改PWM开关频率的方法
/*
// 修改开关频率 6K    150M/2分频/6k/2倍 = 6250
float pwmPeriodLimit = 5304;  // 6250*0.866*0.98 = 5304.25
volatile float PIlim_I = 6365;   //  6K   6250*0.98/0.866*0.9 = 6365
float T1PR = 6250;
EPwm1Regs.TBPRD = 6250;
voltForwardRatio = 6250*0.8*0.866/700=6.2
*/
// 上升沿高电平，下降沿低电平，输出信号反向。
//　则上升沿设定为低电平，下降沿设定为高电平　0218

// T1=SQRT3*Ts*Ualpha/Udc = SQRT3*6250*Ualpha/Udc = 4058.0*Ualpha/Udc
/*
void PWMsector(void)
{
    float T1,T2,Tmp;
    float temp_32 =0;  //T1PR 是定时器的周期，修改这块即可使用。
	if(pwmPllOk == 0)
	{
		EPwm1Regs.TBPRD = T1PR;
		EPwm2Regs.TBPRD = T1PR;
		EPwm3Regs.TBPRD = T1PR;
		EPwm5Regs.TBPRD = T1PR;
		EPwm6Regs.TBPRD = T1PR;
	}


    // 以下变换经过验证是正确的。
//    Ur1 = dq2Ubeta* SQRT_2DIV3;
//    Ur2 = -0.4082 * dq2Ubeta + dq2Ualpha * 0.707;
//-   Ur3 = -(Ur1 + Ur2);
//    Ur3 = -0.4082 * dq2Ubeta - dq2Ualpha * 0.707;

	dq2Ualpha *= udcForwardRatio;
	dq2Ubeta  *= udcForwardRatio;
    // 实际变换为
    Ur1 = dq2Ubeta;
    Ur2 = 0.866*dq2Ualpha - 0.5*dq2Ubeta;
    Ur3 = -(Ur1 + Ur2);
    // 因变换后与实际反向，即 nPhaseCtrlVolt
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

