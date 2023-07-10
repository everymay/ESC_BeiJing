#include "F2837xS_device.h"     // DSP2833x Headerfile Include File
#include "F2837xS_Examples.h"   // DSP2833x Examples Include File
#include "MarcoDefine.h"
#include "VariblesRef.h"
#include "math.h"
#include "C28x_FPU_FastRTS.h"
#include "DCL.h"
#include "DCLCLA.h"
#include "Modbus.h"
#include "DPLL.h"

#pragma CODE_SECTION(PLLrun, "ram2func");
#pragma CODE_SECTION(PLLrunABC, "ram2func");

//.ebss START 注意:初始化的必须放在.ebss段中.否则不被上电初始化///////
const DF22 AllFilterCoeff[3][FILTER_MCU_DEFAULT_NUM]   ={ALL_FILTERCOFF_16KHZ,ALL_FILTERCOFF_12p8KHZ,ALL_FILTERCOFF_9p6KHZ};
DF22 ResonancePortectABC[2]             ={RESONANCE_PORTECT_ABC12p8KHZ,RESONANCE_PORTECT_ABC12p8KHZ};
DF22 VolAndInvCurrFilter[15]            ={};
DF22 UdcUpFilter                        = UDC_PNF_DEFAULT12p8KHZ;
DF22 UdcDnFilter                        = UDC_PNF_DEFAULT12p8KHZ;
DF22 bwForFreq                          = BWFILTER_FREQ_DEFAULTS12p8KHZ;
DF22 fftmag[3][CALIBRATION_COEFF_LEN]   = FFT_MAG_ALL_DEFAULT;
DF22 fftpha[3][CALIBRATION_COEFF_LEN]   = FFT_PHA_ALL_DEFAULT;
DF22 fftCalibMag[3][CALIBRATION_COEFF_LEN]   = FFT_MAG_ALL_DEFAULT;
DF22 fftCalibPha[3][CALIBRATION_COEFF_LEN]   = FFT_PHA_ALL_DEFAULT;
DF22 SinglePhaseDispFilter[6]=SINGLE_PHASE_RECTPOWER_DISP_DEFAULT;

SPLL_3ph_DDSRF SPLL[3] ={\
        {{PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ},\
        PLL_PI_CTRL_DEFAULT,\
        0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0,0,0,0,0},\
        {{PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ},\
        PLL_PI_CTRL_DEFAULT,\
        0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0,0,0,0,0},\
        {{PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ},\
        PLL_PI_CTRL_DEFAULT,\
        0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0,0,0,0,0}};  //不能初始化为0
SPLL_3ph_DDSRF SPLL_CUR = {\
        {PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ,PLL_VOLT_Q_DEFAULT12p8KHZ},\
                       PLL_PI_CTRL_DEFAULT,\
        0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0.1f,0,0,0,0,0};


PIcalc UdcPICtrl =UDC_PI_CTRL_DEFAULT;
PIcalc UdcPIBalance =UDC_PI_BALANCE_DEFAULT;

PIcalc GridCurrPICtrlA =UDC_PI_GRID_REACTIVE_DEFAULT;
PIcalc GridCurrPICtrlB =UDC_PI_GRID_REACTIVE_DEFAULT;
PIcalc GridCurrPICtrlC =UDC_PI_GRID_REACTIVE_DEFAULT;

PIcalc VolttargetCorrPIA =VOLT_TARGET_CORR_DEFAULT;
PIcalc VolttargetCorrPIB =VOLT_TARGET_CORR_DEFAULT;
PIcalc VolttargetCorrPIC =VOLT_TARGET_CORR_DEFAULT;

PIcalc PICurrInnerA =CURR_PI_INNER_DEFAULT;
PIcalc PICurrInnerB =CURR_PI_INNER_DEFAULT;
PIcalc PICurrInnerC =CURR_PI_INNER_DEFAULT;
PIcalc OutInvUVCurrD = PI_Vol_UV;
PIcalc OutInvUVCurrQ = PI_Vol_UV;
PIcalc OutInvUVCurrRMS = PI_Vol_UV;

/*
 * A相：PI控制器参数。WY
 */
PIcalc PIVolA = PI_Vol;
PIcalc PIVolB = PI_Vol;
PIcalc PIVolC = PI_Vol;

PIcalc PIFanCtl = PI_Vol;
PIcalc PIIvrA = PI_Cur;
PIcalc PIIvrB = PI_Cur;
PIcalc PIIvrC = PI_Cur;
PIcalc PIIvrQ = PI_Cur;
PIcalc PIIvrND = PI_Cur;
PIcalc PIIvrNQ = PI_Cur;
int Esc_NUM[32];
int i = 0;

/*
 * 功能：初始化PI控制器参数。WY
 * 输入参数mode：工作模式。0：上电初始化；1：通信程序初始化。
 */
void InitCtrlParam(int mode)
{
	if (mode == 0)
	{
		SPLL[0].PLLPICtrl.i10 = 2 * PI * POWERGRID_FREQ;
	}

	GridCurrPICtrlA.Kp = kp_Dc;
	GridCurrPICtrlA.Ki = ki_Dc;
	GridCurrPICtrlA.Umax = PIlim_Udc;
	GridCurrPICtrlA.Umin = -PIlim_Udc;
	GridCurrPICtrlB.Kp = kp_Dc;
	GridCurrPICtrlB.Ki = ki_Dc;
	GridCurrPICtrlB.Umax = PIlim_Udc;
	GridCurrPICtrlB.Umin = -PIlim_Udc;
	GridCurrPICtrlC.Kp = kp_Dc;
	GridCurrPICtrlC.Ki = ki_Dc;
	GridCurrPICtrlC.Umax = PIlim_Udc;
	GridCurrPICtrlC.Umin = -PIlim_Udc;

	/*A相PI控制器参数。WY*/
	ConstantCurr[0].CorrPI.Kp = kp_Dc_Div * 0.0001;
	ConstantCurr[0].CorrPI.Ki = ki_Dc_Div;
	ConstantCurr[0].CorrPI.Umax = 0;
	ConstantCurr[0].CorrPI.Umin = -(DC_ERR_LIMIT * 0.1);

	/*B相PI控制器参数。WY*/
	ConstantCurr[1].CorrPI.Kp = kp_Dc_Div*0.0001;
	ConstantCurr[1].CorrPI.Ki = ki_Dc_Div;
	ConstantCurr[1].CorrPI.Umax = 0;
	ConstantCurr[1].CorrPI.Umin = -(DC_ERR_LIMIT*0.1);

	/*C相PI控制器参数。WY*/
	ConstantCurr[2].CorrPI.Kp = kp_Dc_Div*0.0001;
	ConstantCurr[2].CorrPI.Ki = ki_Dc_Div;
	ConstantCurr[2].CorrPI.Umax = 0;
	ConstantCurr[2].CorrPI.Umin = -(DC_ERR_LIMIT*0.1);

	UdcPIBalance.Kp = kp_Dc_Div;
	UdcPIBalance.Ki = ki_Dc_Div;
	UdcPIBalance.Umax = DC_ERR_LIMIT;
	UdcPIBalance.Umin = -DC_ERR_LIMIT;

	PICurrInnerA.Kp = kp_Cur;
	PICurrInnerA.Umax = PIlim_I;
	PICurrInnerA.Umin = -PIlim_I;
	PICurrInnerB.Kp = kp_Cur;
	PICurrInnerB.Umax = PIlim_I;
	PICurrInnerB.Umin = -PIlim_I;
	PICurrInnerC.Kp = kp_Cur;
	PICurrInnerC.Umax = PIlim_I;
	PICurrInnerC.Umin = -PIlim_I;

	if(!StateFlag.spwmFlag)
	{//PR控制开启.关闭PI的I系数
		PICurrInnerA.Ki =0;//ki_Cur;
		PICurrInnerB.Ki =0;//ki_Cur;
		PICurrInnerC.Ki =0;//ki_Cur;
	}
	else
	{
		PICurrInnerA.Ki =ki_Cur;
		PICurrInnerB.Ki =ki_Cur;
		PICurrInnerC.Ki =ki_Cur;
	}

	EPwm1Regs.CMPA.bit.CMPA =SampleLead;
}

void HarmonicLoading(void)          //取谐波数组
{
	Uint16 i,*p,*q;
	p = (Uint16 *)&GenCorr;
	q = (Uint16 *)&GenCorrPrv[7];								//从预置谐波表取数

	for(i=0;i<sizeof(GenCorr);i++){
		*p++=*q++;														//拷贝到谐波生成数组
	}
}

 void HarmonicGeneration(void)       //将谐波数组转换成谐波信号
{
	if(CntMs.HarmonicSendCount < CNT_MS(300)){
		harmCompPerc = 1;
		StateFlag.isHarmCompensateMode = 0;
		StateFlag.harmCompEn = 1;
		TestWaveGenerator();
		if(CntMs.HarmonicSendCount>CNT_MS(30))		StateFlag.DelayCalculationFlag = 1;
	}else{
		StateFlag.harmCompEn = 0;
		StateFlag.DelayCalculationFlag = 0;
		StateFlag.MakeEnergyFlag = 0;
	}
}

void HarmnoicCorrection(void)        //补谐波
{
	Uint16 i=0;
	Uint16 j=0;
	float k;
	int *q;

	if(StateFlag.isHarmCompensateMode)						//补
	{
		CALIBRATION_COEFF_STRUCT 	*p = &CalibCorr;
		for(i=0;i<CALIBRATION_DISP_LEN;i++){
			if((Harmonic.Wave[i].MagA!=0)||(Harmonic.Wave[i].MagB!=0)||(Harmonic.Wave[i].MagC!=0))
			{
				if(j<CALIBRATION_COEFF_LEN)
				{
					q=(int*)&harmCompPercParameter+i;
					k = SETUP_RATIO_CHANGE(*q,10000);
					p->harmOrder[j]	= i+2;
					p->MagA[j] 		= SETUP_RATIO_CHANGE(Harmonic.Wave[i].MagA*k,10000);
					p->MagB[j] 		= SETUP_RATIO_CHANGE(Harmonic.Wave[i].MagB*k,10000);
					p->MagC[j] 		= SETUP_RATIO_CHANGE(Harmonic.Wave[i].MagC*k,10000);
                    p->PhaA[j]       = D2R(SETUP_RATIO_CHANGE(Harmonic.Wave[i].PhaA,100));
                    p->PhaB[j]       = D2R(SETUP_RATIO_CHANGE(Harmonic.Wave[i].PhaB,100));
                    p->PhaC[j]       = D2R(SETUP_RATIO_CHANGE(Harmonic.Wave[i].PhaC,100));
					p->limit[j] 	= SQUARE(SETUP_RATIO_CHANGE((float)Harmonic.Wave[i].limit,10));
					p->start[j] 	= SQUARE(SETUP_RATIO_CHANGE((float)Harmonic.Wave[i].start,10));

					if((i+2)%3==0)      //3、6、9、12、15、18、21、24次
					{
                        if(p->MagA[j]>1.3)              p->MagA[j]=1.3;
                        if(p->MagB[j]>1.3)              p->MagB[j]=1.3;
                        if(p->MagC[j]>1.3)              p->MagC[j]=1.3;
					}else{
						if(p->MagA[j]>1.5)				p->MagA[j]=1.5;
						if(p->MagB[j]>1.5)				p->MagB[j]=1.5;
						if(p->MagC[j]>1.5)				p->MagC[j]=1.5;
					}

					if(p->limit[j]==0)
					{
						if((i%2)&&(i<6))				p->limit[j] = 20000;//3、5、7次每个单元上限默认100A
						else if((i%2)&&(i<12))			p->limit[j] = 5000;//9、11、13次每个单元上限默认50A
						else                            p->limit[j] = 1800;//其他次每个单元上限默认30A
					}
                    if(p->start[j]==0)                  p->start[j] = 8;//大于2A开始补偿
					j++;
				}
			}
		}
		while(j<CALIBRATION_COEFF_LEN)
		{
			p->harmOrder[j]	= 0;
			p->MagA[j] 		= 0;
			p->MagB[j] 		= 0;
			p->MagC[j] 		= 0;
            p->PhaA[j]      = 0;
            p->PhaB[j]      = 0;
            p->PhaC[j]      = 0;
			p->limit[j] 	= 0;
			p->start[j] 	= 0;
			j++;
		}
	}else{
		GENERATOR_COEFF_STRUCT *p = &GenCorr;
		for(i=0;i<24;i++){
			if((Harmonic.Wave[i].MagA!=0)||(Harmonic.Wave[i].MagB!=0)||(Harmonic.Wave[i].MagC!=0))
			{
				if(j<CALIBRATION_COEFF_LEN)
				{
//					q=(int*)&harmCompPercParameter+i;
//					k = SETUP_RATIO_CHANGE(*q,10000);       发谐波幅度改为不受基本谐波设置影响
					p->harmOrder[j]	= i+2;
					p->MagA[j] 		= SETUP_RATIO_CHANGE(Harmonic.Wave[i].MagA,100);
					p->MagB[j] 		= SETUP_RATIO_CHANGE(Harmonic.Wave[i].MagB,100);
					p->MagC[j] 		= SETUP_RATIO_CHANGE(Harmonic.Wave[i].MagC,100);
					p->PhaA[j] 		= D2R(SETUP_RATIO_CHANGE(Harmonic.Wave[i].PhaA,100));
					p->PhaB[j] 		= D2R(SETUP_RATIO_CHANGE(Harmonic.Wave[i].PhaB,100));
					p->PhaC[j] 		= D2R(SETUP_RATIO_CHANGE(Harmonic.Wave[i].PhaC,100));
					j++;
				}
			}
		}
		while(j<CALIBRATION_COEFF_LEN)
		{
			p->harmOrder[j]	= 0;
			p->MagA[j] 		= 0;
			p->MagB[j] 		= 0;
			p->MagC[j] 		= 0;
			p->PhaA[j] 		= 0;
			p->PhaB[j] 		= 0;
			p->PhaC[j] 		= 0;
			j++;
		}
	}
}

#define COMMU_MAX_UNIT_NUM 8	//8个单元

void UserPreferencesRefresh(void)
{

	TargetCurrentUnbalance 	= SETUP_RATIO_CHANGE(UserSetting.TargetCurrentUnbalance,		UserParamRatio.TargetCurrentUnbalance);//0x51
	transfRatioVirtu	= SETUP_RATIO_CHANGE(UserSetting.transfRatio,			UserParamRatio.transfRatio);//0x5E
	GV_RMS_UNDER 		= SETUP_RATIO_CHANGE(UserSetting.GV_RMS_UNDER,			UserParamRatio.GV_RMS_UNDER);
	GV_RMS_OVER			= SETUP_RATIO_CHANGE(UserSetting.GV_RMS_OVER,			UserParamRatio.GV_RMS_OVER);
	ConstantReactivePower   = SETUP_RATIO_CHANGE(UserSetting.ConstantReactivePower,			UserParamRatio.ConstantReactivePower);//0x59
	Backdiffvalue		= SETUP_RATIO_CHANGE(UserSetting.Backdiffvalue,	    UserParamRatio.Backdiffvalue);//0x5A
	TransformRatioVal	= SETUP_RATIO_CHANGE(UserSetting.TransformRatioParall,	UserParamRatio.TransformRatioParall);
	ESCFlagA.Volttarget = VolttargetA = SETUP_RATIO_CHANGE(UserSetting.VolttargetA,				UserParamRatio.VolttargetA);//0x52
	ESCFlagB.Volttarget = VolttargetB = SETUP_RATIO_CHANGE(UserSetting.VolttargetB,				UserParamRatio.VolttargetB);//0x53
	ESCFlagC.Volttarget = VolttargetC = SETUP_RATIO_CHANGE(UserSetting.VolttargetC,			    UserParamRatio.VolttargetC);//0x54
	ESCFlagA.VoltThreShold = VoltThreSholdA = SETUP_RATIO_CHANGE(UserSetting.VoltThreSholdA,			UserParamRatio.VoltThreSholdA);//0x55
	ESCFlagB.VoltThreShold = VoltThreSholdB = SETUP_RATIO_CHANGE(UserSetting.VoltThreSholdB,            UserParamRatio.VoltThreSholdB);
	ESCFlagC.VoltThreShold = VoltThreSholdC = SETUP_RATIO_CHANGE(UserSetting.VoltThreSholdC,            UserParamRatio.VoltThreSholdC);
	AutoWaitCurr		= SETUP_RATIO_CHANGE(UserSetting.AutoWaitCurr,			UserParamRatio.AutoWaitCurr);
	Mode_PP				= SETUP_RATIO_CHANGE(UserSetting.Mode_PP,				UserParamRatio.Mode_PP);
	ESCFlagA.Backdiffvalue = ESCFlagB.Backdiffvalue = ESCFlagC.Backdiffvalue = Backdiffvalue;
    if(GV_RMS_UNDER>50)
        GV_RMS_UNDER_PowerOnForRecharge = GV_RMS_UNDER;
    else
        GV_RMS_UNDER_PowerOnForRecharge = 50;       //防止界面误下发0,导致没有电网电压也能启动,绕过了预充电过程

	StateFlag.apfWiringmethod 		= UserSetting.WordMode.B.apfWiringmethod;

	StateFlag.startingMethod 		= UserSetting.WordMode.B.startingMethod;
    ESCFlagA.ESCCntMs.StartDelay = 0;
    ESCFlagB.ESCCntMs.StartDelay = 0;
    ESCFlagC.ESCCntMs.StartDelay = 0;
	StateFlag.positionCT 			= UserSetting.WordMode.B.positionCT;
	StateFlag.harmCompEn 			= UserSetting.WordMode.B.harmCompEn;
	StateFlag.negCurCompFlag 		= UserSetting.WordMode.B.negCurCompFlag;
	StateFlag.AVCPrCompFlag 		= UserSetting.WordMode.B.AVCMode;
	StateFlag.constantQFlag 		= UserSetting.WordMode.B.constantQFlag;
	StateFlag.prioritizedModeFlag 	= UserSetting.WordMode.B.prioritizedModeFlag;

	StateFlag.VoltageMode           = UserSetting.WordMode.B.VoltageMode;
//	StateFlag.modeChoosedFlag       = StateFlag.VoltageMode;

	transfRatio						= transfRatioVirtu+VirtulAD.CTOutCurDevcoef;
    switch(StateFlag.VoltageMode){
    case 0:
        StateFlag.VoltageModeFlag = 0;   //升压
        ArithFlagA = 0;
        ArithFlagB = 0;
        ArithFlagC = 0;
        break;
    case 1:
        StateFlag.VoltageModeFlag = 1;   //降压
        ArithFlagA = 0;
        ArithFlagB = 0;
        ArithFlagC = 0;
        break;
    case 2: //暂不支持升降压模式.升压模式替代
        StateFlag.VoltageModeFlag = 0;   //升压
        ArithFlagA = 0;
        ArithFlagB = 0;
        ArithFlagC = 0;
        break;
    case 3:
        StateFlag.VoltageModeFlag = 0;   //升压
        ArithFlagA = 0;
        ArithFlagB = 0;
        ArithFlagC = 0;
        break;
    case 4:
        StateFlag.VoltageModeFlag = 0;   //升压
        ArithFlagA = 0;
        ArithFlagB = 0;
        ArithFlagC = 0;
        break;
    }

    if(StateFlag.VoltageModeFlag == 0){          //升压
        ESCFlagA.StableValue = ESCFlagA.VoltThreShold + ESCFlagA.Backdiffvalue;         //稳定值A=门限值+回差值;
        ESCFlagB.StableValue = ESCFlagB.VoltThreShold + ESCFlagB.Backdiffvalue;         //稳定值B=门限值+回差值;
        ESCFlagC.StableValue = ESCFlagC.VoltThreShold + ESCFlagC.Backdiffvalue;         //稳定值C=门限值+回差值;
    }else if(StateFlag.VoltageModeFlag == 1){     //降压
        ESCFlagA.StableValue = ESCFlagA.VoltThreShold - ESCFlagA.Backdiffvalue;         //稳定值A=门限值+回差值;
        ESCFlagB.StableValue = ESCFlagB.VoltThreShold - ESCFlagB.Backdiffvalue;         //稳定值B=门限值+回差值;
        ESCFlagC.StableValue = ESCFlagC.VoltThreShold - ESCFlagC.Backdiffvalue;         //稳定值C=门限值+回差值;
    }

}
void AddrConfigIOPort(void)    //地址配置输入输出端口
{
//    Uint16 IdPress=0;
//    if((StateEventFlag!=STATE_EVENT_RUN)&&(StateEventFlag!=STATE_EVENT_WAIT)){
//        IdPress  = !IDInputIoBit0<<0;
//        IdPress |= !IDInputIoBit1<<1;
//        IdPress |= !IDInputIoBit2<<2;
//        MuFaciID.APF_ID = IdPress & 0x07;
//    }
}
void IDParametersRefresh(void)
{
    AddrConfigIOPort();
	if(MuFaciID.APF_ID >= COMMU_MAX_UNIT_NUM) 	MuFaciID.APF_ID = 0x10;		//界面下发的从机地址从0~COMMU_MAX_UNIT_NUM-1,共COMMU_MAX_UNIT_NUM个单元
	MuFaciID.APF_ID = 0;
	APF_ID = MuFaciID.APF_ID + 0x10;											//单元通讯地址(ID号)从0x10~0x17
	SCIB.ucMBAddress = APF_ID;    //通讯地址
	if(MuFaciID.MU_UNIT_NUM > COMMU_MAX_UNIT_NUM || MuFaciID.MU_UNIT_NUM < 1)//单元个数
		MuFaciID.MU_UNIT_NUM = 1;
	PWM_address = MuFaciID.APF_ID;	 //同步地址										//单元地址从0~COMMU_MAX_UNIT_NUM-1,共COMMU_MAX_UNIT_NUM个单元
	if(PWM_address == 0) syncState = 3; 											//确保主机不进入同步程序
	else                    syncState = 0;
	MU_ALLCOUNT = MuFaciID.MU_UNIT_NUM  & 0xf;									//共计MU_ALLCOUNT个模块并联,此数量给脉冲分配用

	if(TestMU_RATIO>0){										//如果TestMU_RATIO不为0,则按照TestMU_RATIO的数量作为并联模块数量.
		MU_LCD_RATIO = TestMU_RATIO;
		MU_MultRatio = 1.0f /(TestMU_RATIO );				//显示的并联模块数量//均流
	}else{
		MU_LCD_RATIO = MuFaciID.MU_UNIT_NUM;			//注意:必须确保MU_UNIT_NUM被正确赋值,注意此函数的调用顺序!!!
		MU_MultRatio = 1.0f /(MuFaciID.MU_UNIT_NUM );	//显示的并联模块数量
	}
    MuFaciID.RandIDH = RandIDH  = 0;
    MuFaciID.RandIDL = RandIDL  = 0;
}

float VolKp,VolKi;
void ManufacturerParametersRefresh(void)
{
	struct Stru_FactorySet 			*pOrg= &FactorySet;
	struct Stru_FactorySet 			*pRat= &FactorySetupRatio;

	Uint16 sPWM,isharmcompensatemode,t1prtmp;//VoltLevel,FaultRecord,standbymode;
	float wholeOutCurRmsLimit,UbanCurPer;//neutralCurLimit;
//	float VolKp,VolKi;
	PhaseControl	        = SETUP_RATIO_CHANGE(pOrg->PhaseControl			,pRat->PhaseControl);    //ESC中相位控制作用.
	LoadCurlimit            = SETUP_RATIO_CHANGE(pOrg->LoadCurlimit         ,pRat->LoadCurlimit);    //ESC中负载电流控制作用.
	SyncHardwareLead		= SETUP_RATIO_CHANGE(pOrg->SyncHardwareLead		,pRat->SyncHardwareLead);
	SampleLead				= SETUP_RATIO_CHANGE(pOrg->SampleLead			,pRat->SampleLead);
	t1prtmp					= SETUP_RATIO_CHANGE(pOrg->T1PR					,pRat->T1PR);
	Curr_MinCtrlFAN         = SETUP_RATIO_CHANGE(pOrg->Curr_MinCtrlFAN      ,pRat->Curr_MinCtrlFAN); //最小负载控制风机启动
	Curr_MaxCtrlFAN         = SETUP_RATIO_CHANGE(pOrg->Curr_MaxCtrlFAN      ,pRat->Curr_MaxCtrlFAN); //最大负载控制风机全速
	Temp_MinCtrlFAN         = SETUP_RATIO_CHANGE(pOrg->Temp_MinCtrlFAN      ,pRat->Temp_MinCtrlFAN); //最小温度控制风机启动
	Temp_MaxCtrlFAN         = SETUP_RATIO_CHANGE(pOrg->Temp_MaxCtrlFAN      ,pRat->Temp_MaxCtrlFAN); //最大温度控制风机全速
	FanStartMinDUTY         = SETUP_RATIO_CHANGE(pOrg->FanStartMinDUTY      ,pRat->FanStartMinDUTY); //风机启动最小占空比
	RateLoadCurrLimit       = SETUP_RATIO_CHANGE(pOrg->RateLoadCurrLimit    ,pRat->RateLoadCurrLimit);//额定负载限流
	CapacitySelection       = SETUP_RATIO_CHANGE(pOrg->CapacitySelection    ,pRat->CapacitySelection);//容量选择(选择相应的容量(30kva/50kva/75kva)进行限流算法)
	DcCapVoltRatioVirtu		= SETUP_RATIO_CHANGE(pOrg->dcCapVoltRatio 		,pRat->dcCapVoltRatio);  //ESC--直流吸收电容电压系数
	loadVoltRatioVirtu		= SETUP_RATIO_CHANGE(pOrg->loadVoltRatio		,pRat->loadVoltRatio);   //ESC--负载电压采样系数
	gridVoltRatioVirtu		= SETUP_RATIO_CHANGE(pOrg->gridVoltRatio		,pRat->gridVoltRatio);   //ESC--电网电压系数
	outputCurRatioVirtu		= SETUP_RATIO_CHANGE(pOrg->outputCurRatio		,pRat->outputCurRatio);  //ESC--输出电流采样系数
	sPWM					= SETUP_RATIO_CHANGE(pOrg->SPWM					,pRat->SPWM);
	FaultRecSel.RecordMode	= SETUP_RATIO_CHANGE(pOrg->RecordMode			,pRat->RecordMode);
	isharmcompensatemode	= SETUP_RATIO_CHANGE(pOrg->isHarmCompensateMode	,pRat->isHarmCompensateMode);
	bypassCurrRatioVirtu	= SETUP_RATIO_CHANGE(pOrg->bypassCurrRatio		,pRat->bypassCurrRatio); //ESC--旁路电流系数
//	CLATestValue = dcBusVoltGiven;//使用CLA算法中,一个系数K
	DC_ERR_LIMIT			= SETUP_RATIO_CHANGE(pOrg->DC_ERR_LIMIT			,pRat->DC_ERR_LIMIT);  // ESC新加恒流模式下电流做PI闭环的下限
	PIlim_Udc				= SETUP_RATIO_CHANGE(pOrg->PIlim_Udc			,pRat->PIlim_Udc);  //ESC电科院做验证无功补偿模式下PI限值
	PIlim_I					= SETUP_RATIO_CHANGE(pOrg->PIlim_I				,pRat->PIlim_I);
	SPLL[0].PLLPICtrl.Umax	= SPLL[1].PLLPICtrl.Umax  = SPLL[2].PLLPICtrl.Umax    \
	                        = SETUP_RATIO_CHANGE(pOrg->phaseErrLimit        ,pRat->phaseErrLimit);
	SPLL[0].PLLPICtrl.Kp = SPLL[1].PLLPICtrl.Kp	= SPLL[2].PLLPICtrl.Kp            \
	                        = SETUP_RATIO_CHANGE(pOrg->kp_Pll				,pRat->kp_Pll);
	SPLL[0].PLLPICtrl.Ki = SPLL[1].PLLPICtrl.Ki = SPLL[2].PLLPICtrl.Ki	          \
	                        = SETUP_RATIO_CHANGE(pOrg->ki_Pll				,pRat->ki_Pll);
	kp_Dc_Div				= SETUP_RATIO_CHANGE(pOrg->kp_Dc_Div			,pRat->kp_Dc_Div);  //ESC新加恒流模式下电流做PI闭环的P系数
	ki_Dc_Div				= SETUP_RATIO_CHANGE(pOrg->ki_Dc_Div			,pRat->ki_Dc_Div);  //ESC新加恒流模式下电流做PI闭环的i系数
	kp_Dc					= SETUP_RATIO_CHANGE(pOrg->kp_Dc				,pRat->kp_Dc);  //ESC电科院做验证无功补偿模式下P系数
	ki_Dc					= SETUP_RATIO_CHANGE(pOrg->ki_Dc				,pRat->ki_Dc);  //ESC电科院做验证无功补偿模式下i系数
	kp_Cur					= SETUP_RATIO_CHANGE(pOrg->kp_Cur				,pRat->kp_Cur);
	ki_Cur					= (float)pOrg->ki_Cur*RATIO_DNLOAD1000000;
	GF_OVER					= SETUP_RATIO_CHANGE(pOrg->GF_OVER				,pRat->GF_OVER);
	GF_UNDER				= SETUP_RATIO_CHANGE(pOrg->GF_UNDER				,pRat->GF_UNDER);
	ESCDCVOLITLIMIT		    = SETUP_RATIO_CHANGE(pOrg->ESCDCVOLITLIMIT		,pRat->ESCDCVOLITLIMIT);   //ESC吸收回路直流电容电压限值
	VoltFallResponseLimit	= SETUP_RATIO_CHANGE(pOrg->VoltFallResponseLimit,pRat->VoltFallResponseLimit);
	ArithVAL = VoltFallResponseLimit;     //ESC电压跌落快速响应限制值
	ArithVal = -VoltFallResponseLimit;
	OUTCUR_OVER_INS			= SETUP_RATIO_CHANGE(pOrg->OUTCUR_OVER_INS		,pRat->OUTCUR_OVER_INS);
	OUTCUR_OVER_RMS			= SETUP_RATIO_CHANGE(pOrg->OUTCUR_OVER_RMS		,pRat->OUTCUR_OVER_RMS);
	OUT_OVER_LOAD			= SETUP_RATIO_CHANGE(pOrg->OUT_OVER_LOAD		,pRat->OUT_OVER_LOAD);
	OutCurMaxLimit			= SETUP_RATIO_CHANGE(pOrg->OutCurMaxLimit		,pRat->OutCurMaxLimit);
	WindCold.UNIT_OVER_TEMP	= SETUP_RATIO_CHANGE(pOrg->UNIT_OVER_TEMP		,pRat->UNIT_OVER_TEMP);

	reactPrCompPerc         = SETUP_RATIO_CHANGE(pOrg->reactPrCompPerc      ,pRat->reactPrCompPerc);//
	harmCompPerc            = SETUP_RATIO_CHANGE(pOrg->harmCompPerc         ,pRat->harmCompPerc);   //
	negCurCompPerc          = SETUP_RATIO_CHANGE(pOrg->negCurCompPerc       ,pRat->negCurCompPerc); //ESC中作用是测试IGBT的死区时间.
	ESCTESTDATA1            = SETUP_RATIO_CHANGE(pOrg->ESCTESTDATA1         ,pRat->ESCTESTDATA1);
	ESCTESTDATA2            = SETUP_RATIO_CHANGE(pOrg->ESCTESTDATA2         ,pRat->ESCTESTDATA2);
	ESCTESTDATA3            = SETUP_RATIO_CHANGE(pOrg->ESCTESTDATA3         ,pRat->ESCTESTDATA3);
	ESCTESTDATA4            = SETUP_RATIO_CHANGE(pOrg->ESCTESTDATA4         ,pRat->ESCTESTDATA4);
	TempEnvirProvalue       = SETUP_RATIO_CHANGE(pOrg->TempEnvirProvalue    ,pRat->TempEnvirProvalue);
	THD_K                   = SETUP_RATIO_CHANGE(pOrg->THD_K                ,pRat->THD_K);
	THD_B                   = SETUP_RATIO_CHANGE(pOrg->THD_B                ,pRat->THD_B);

	VolKp				    = SETUP_RATIO_CHANGE(pOrg->VoltProport			,pRat->VoltProport);//0xba
	VolKi				    = SETUP_RATIO_CHANGE(pOrg->VoltIntegral			,pRat->VoltIntegral);//0xbb
	VoltDerivative 			= SETUP_RATIO_CHANGE(pOrg->VoltDerivative		,pRat->VoltDerivative);//0xbc  //ESC中作用是PI限幅.

	VoltDQSource			= SETUP_RATIO_CHANGE(pOrg->VoltDQSource			,pRat->VoltDQSource);//0xbd//电网欠压延时时间
	NegaQPowerCalib			= SETUP_RATIO_CHANGE(pOrg->NegaQPowerCalib		,pRat->NegaQPowerCalib);//0xbe
	NegaDPowerCalib			= SETUP_RATIO_CHANGE(pOrg->NegaDPowerCalib		,pRat->NegaDPowerCalib);//0xbf
	ReactivePowerCurrCalib  = SETUP_RATIO_CHANGE(pOrg->ReactivePowerCurrCalib	,pRat->ReactivePowerCurrCalib);//0xc0
	ActivePowerCurrCalib	= SETUP_RATIO_CHANGE(pOrg->ActivePowerCurrCalib	,pRat->ActivePowerCurrCalib);//0xc1
	TestMU_RATIO			= SETUP_RATIO_CHANGE(pOrg->TestMU_RATIO			,pRat->TestMU_RATIO);//0xc2
	VoltLeadPoints			= SETUP_RATIO_CHANGE(pOrg->VoltLeadPoints		,pRat->VoltLeadPoints);//0xc3//电压校正延时系数//0-399
	windColdCtr				= SETUP_RATIO_CHANGE(pOrg->windColdCtr			,pRat->windColdCtr);//0xc4
	PllCalibrPhase			= D2R(pOrg->PllCalibrPhase);										//0xc5
	debugDispFlag			= SETUP_RATIO_CHANGE(pOrg->debugDispFlag		,pRat->debugDispFlag);//0xc6
	R_SSwitchTime           = SETUP_RATIO_CHANGE(pOrg->R_SSwitchTime        ,pRat->R_SSwitchTime); //      ESC--Running/standby switchover time 运行/待机切换时间
	S_RSwitchTime			= SETUP_RATIO_CHANGE(pOrg->S_RSwitchTime		,pRat->S_RSwitchTime);//0xc8   ESC--待机/运行切换时间
	wholeOutCurRmsLimit		= SETUP_RATIO_CHANGE(pOrg->WholeOutCurRmsLimit	,pRat->WholeOutCurRmsLimit);//0xc9
	StandCosFiLimit         = SETUP_RATIO_CHANGE(pOrg->StandCosFiLimit      ,pRat->StandCosFiLimit);
	UbanCurPer              = SETUP_RATIO_CHANGE(pOrg->StandUbanCurPer      ,pRat->StandUbanCurPer);
	WindCold.BOARD_OVER_TEMP= SETUP_RATIO_CHANGE(pOrg->BOARD_OVER_TEMP		,pRat->BOARD_OVER_TEMP);

    //加入代码让开关周期只能重新上电后生效,在线修改只能改EPPORM中的值.
    if(t1prtmp<1000){       //当T1PR设为小于1000的值的时候,变为如下选择方案,之前直接输入开关周期的方式废弃不用
        switch(t1prtmp){    //根据主频为200M推算得出
        case 40:    T1PR=1250;T1PRPwmFrequency=0; ADGROUP_NUM = 8; ADGROUP_MODE = 3;WAVE_REC_FFT_MODE_DATA2= 5;break;
        case 16:    T1PR=3125;T1PRPwmFrequency=1; ADGROUP_NUM = 8; ADGROUP_MODE = 3;WAVE_REC_FFT_MODE_DATA2= 5;break;
        case 128:   T1PR=3906;T1PRPwmFrequency=2; ADGROUP_NUM = 8; ADGROUP_MODE = 2;WAVE_REC_FFT_MODE_DATA2= 4;break;
        case 96:    T1PR=4883;T1PRPwmFrequency=3; ADGROUP_NUM = 6; ADGROUP_MODE = 1;WAVE_REC_FFT_MODE_DATA2= 3;break;
      //case 8:     T1PR=5860;T1PRPwmFrequency=4; ADGROUP_NUM = 4; ADGROUP_MODE = 0;WAVE_REC_FFT_MODE_DATA2= 3;break;
        default:    T1PR=3906;T1PRPwmFrequency=1; ADGROUP_NUM = 8; ADGROUP_MODE = 2;WAVE_REC_FFT_MODE_DATA2= 4;break;
        }
    }else{
	    T1PR=3906;T1PRPwmFrequency=1;     //如果界面依然按照之前的开关周期方式下发,则强行选择为12.8k,防止引起其他事故.
	                                        //并且让pOrg->T1PR的值被修改.这样再次上传时操作人员就可看出问题.
    }
    if(T1PR>13000.0f) T1PR=13000.0f;
    if(T1PR<1150.0f) T1PR=1150.0f;      //Preventing communication failure from unable interrupted
    T1PRmulESC_T1PR = ((int32)(T1PR))/*<<16*/;
    if(SyncHardwareLead>T1PR) 		SyncHardwareLead=T1PR-1;    //同步超前点数
	if(SyncHardwareLead<1.1f)		SyncHardwareLead=1.1f;
	if(SampleLead>T1PR) 			SampleLead=T1PR-3;  //采样超前点数
	if(SampleLead<3.0f)	 			SampleLead=3.0f;
	ActivePowerCurrCalibPH = ActivePowerCurrCalib*S1DIVSQRT3;
	if(reactPrCompPerc>2)           reactPrCompPerc=2;
    if(harmCompPerc > 1)              harmCompPerc = 1 ;//2;
    if(negCurCompPerc > 0.5)            negCurCompPerc = 0.5;//2;  死区最大限制20%
    else if(negCurCompPerc < -0.5)      negCurCompPerc = -0.5;

	if(TestMU_RATIO>0){										//如果TestMU_RATIO不为0,则按照TestMU_RATIO的数量作为并联模块数量.
		MU_LCD_RATIO = TestMU_RATIO;
		MU_MultRatio = 1.0f /(TestMU_RATIO );				//显示的并联模块数量
	}else{
		MU_LCD_RATIO = MuFaciID.MU_UNIT_NUM;			//注意:必须确保MU_UNIT_NUM被正确赋值,注意此函数的调用顺序!!!
		MU_MultRatio = 1.0f /(MuFaciID.MU_UNIT_NUM );	//显示的并联模块数量
	}

	FactorySet.CommunicationVersion  = COMMUNCICATION_VERSION;
	StandUbanCurPer = UbanCurPer*0.01;
	SPLL_CUR.PLLPICtrl.Umax = SPLL[0].PLLPICtrl.Umax = SPLL[1].PLLPICtrl.Umax = SPLL[2].PLLPICtrl.Umax;
	SPLL_CUR.PLLPICtrl.Kp = SPLL[0].PLLPICtrl.Kp = SPLL[1].PLLPICtrl.Kp = SPLL[2].PLLPICtrl.Kp;
	SPLL_CUR.PLLPICtrl.Ki = SPLL[0].PLLPICtrl.Ki = SPLL[1].PLLPICtrl.Ki = SPLL[2].PLLPICtrl.Ki;
	SPLL_CUR.PLLPICtrl.Umin = SPLL[0].PLLPICtrl.Umin = SPLL[1].PLLPICtrl.Umin = SPLL[2].PLLPICtrl.Umin = 48.5*PI2;
	PIVolB.Kp = PIVolC.Kp = PIVolA.Kp = VolKp*0.01;
	PIVolB.Ki = PIVolC.Ki = PIVolA.Ki = VolKi*0.01;
	PIVolB.Umax = PIVolC.Umax = PIVolA.Umax = VoltDerivative;
	PIVolB.Umin = PIVolC.Umin = PIVolA.Umin = -VoltDerivative;
	SetWaveRecordMode(FaultRecSel.RecordMode);

	OUTCUR_OVER_INS_NEG		=-OUTCUR_OVER_INS;
	OutCurMaxLimitNeg		=-OutCurMaxLimit;
	WholeOutCurRmsLimit 	= wholeOutCurRmsLimit*wholeOutCurRmsLimit;
	NegCurLimit 			= wholeOutCurRmsLimit;
	CapStartLimit 			= NegCurLimit*CapGroupAct.SvgPercent*0.0001;

	gridVoltRatio			= -gridVoltRatioVirtu;       //电网电压系数
	loadVoltRatio			= -loadVoltRatioVirtu;       //负载电压系数
	outputCurRatio			= outputCurRatioVirtu;      //主电抗电流系数
	bypassCurrRatio         = bypassCurrRatioVirtu;      //旁路电流系数
    dcCapVoltRatio          = DcCapVoltRatioVirtu;       //直流吸收电容电压系数

    outputCurRatioCurrA     = outputCurRatio;
    outputCurRatioCurrB     = outputCurRatio;
    outputCurRatioCurrC     = outputCurRatio;
    outputCurBypassCurrA    = bypassCurrRatio;
    outputCurBypassCurrB    = bypassCurrRatio;
    outputCurBypassCurrC    = bypassCurrRatio;

	if(sPWM%1000 != 0){
	   switch(sPWM%10){
		   case 0:   StateFlag.spwmFlag = 0;StateFlag.DynamicProportionEn = 0;	break;	//开启PR控制
		   case 1:   StateFlag.spwmFlag = 1;StateFlag.DynamicProportionEn = 0;	break;	//开启静态P系数
		   case 2:   StateFlag.spwmFlag = 1;StateFlag.DynamicProportionEn = 1;	break;	//开启动态P系数
		   default:  StateFlag.spwmFlag = 1;StateFlag.DynamicProportionEn = 0;	break;
	   }
	   switch((sPWM/10)%10){
		   case 0:  StateFlag.VoltFilterEn = 0;		break;
		   case 1:  StateFlag.VoltFilterEn = 1;		break;
		   default:	StateFlag.VoltFilterEn = 0;		break;
	   }
	   switch((sPWM/100)%10){
		   case 0:   StateFlag.SVPWMEn = 0;		break;
		   case 1:   StateFlag.SVPWMEn = 1;		break;
		   default:  StateFlag.SVPWMEn = 0;		break;
	   }
	}else{
	   StateFlag.SVPWMEn = 1;	StateFlag.VoltFilterEn = 0;	StateFlag.spwmFlag = 0; StateFlag.DynamicProportionEn = 0;
	}

	if((isharmcompensatemode==0)||(isharmcompensatemode>=1000)){
	    if(isharmcompensatemode>=1000){
            Uint16 i,*p,*q;
            p = (Uint16 *)&GenCorr;
            q = (Uint16 *)&GenCorrPrv[isharmcompensatemode%1000];								//从预置谐波表取数

            for(i=0;i<sizeof(GenCorr);i++){
                *p++=*q++;														//拷贝到谐波生成数组
            }
            TestWaveGenerator();
	    }
		StateFlag.isHarmCompensateMode = 0;
	}else{
		StateFlag.isHarmCompensateMode = 1;
	}

	WindCold.windColdCtrMode				=(windColdCtr/1000);
	WindCold.windColdStartTempterature		=(windColdCtr%1000);
	WindCold.windColdStopTempterature		=WindCold.windColdStartTempterature-8;
	WindCold.MotherBoardStartTempterature	=WindCold.BOARD_OVER_TEMP-10;
	WindCold.MotherBoardStopTempterature	=WindCold.MotherBoardStartTempterature-6;
	//散热器
	WindCold.ReduceCapacityHeatSink 			= WindCold.UNIT_OVER_TEMP - 2;
	if(WindCold.ReduceCapacityHeatSink<0) 		WindCold.ReduceCapacityHeatSink=0;
	WindCold.BackReduceCapacityHeatSink 		= WindCold.UNIT_OVER_TEMP - 4;
	if(WindCold.BackReduceCapacityHeatSink<0) 	WindCold.BackReduceCapacityHeatSink=0;
    //板载
    WindCold.ReduceCapacityMotherBoard             = WindCold.BOARD_OVER_TEMP - 2;
    if(WindCold.ReduceCapacityMotherBoard<0)       WindCold.ReduceCapacityMotherBoard=0;
    WindCold.BackReduceCapacityMotherBoard         = WindCold.BOARD_OVER_TEMP - 4;
    if(WindCold.BackReduceCapacityMotherBoard<0)   WindCold.BackReduceCapacityMotherBoard=0;
	CorrectingAD();							//电压相序
	CorrectingCT();							//电流相序
	DirectionCT();							//电流方向
}


void VirtuPreferencesRefresh(void)         //界面显示零偏值(程序当中使用的零偏值 = 界面下发零偏值,倍率)
{
    struct VARIZEROVAL             *pOrg= &VariZeroOffsetVAL;
    struct VARIZEROVAL             *pRat= &VariZeroOffsetRatioVAL;

    VirtulADval.gridHVoltA        = SETUP_RATIO_CHANGE(pOrg->gridHVoltA       ,pRat->gridHVoltA);
    VirtulADval.gridHVoltB        = SETUP_RATIO_CHANGE(pOrg->gridHVoltB       ,pRat->gridHVoltB);
    VirtulADval.gridHVoltC        = SETUP_RATIO_CHANGE(pOrg->gridHVoltC       ,pRat->gridHVoltC);
    VirtulAD_loadCurrentA = VirtulADval.gridLVoltA   \
                                  = SETUP_RATIO_CHANGE(pOrg->gridLVoltA       ,pRat->gridLVoltA);
    VirtulAD_loadCurrentB = VirtulADval.gridLVoltB   \
                                  = SETUP_RATIO_CHANGE(pOrg->gridLVoltB       ,pRat->gridLVoltB);
    VirtulAD_loadCurrentC = VirtulADval.gridLVoltC   \
                                  = SETUP_RATIO_CHANGE(pOrg->gridLVoltC       ,pRat->gridLVoltC);
    VirtulADval.gridMainCurA      = SETUP_RATIO_CHANGE(pOrg->gridMainCurA     ,pRat->gridMainCurA);
    VirtulADval.gridMainCurB      = SETUP_RATIO_CHANGE(pOrg->gridMainCurB     ,pRat->gridMainCurB);
    VirtulADval.gridMainCurC      = SETUP_RATIO_CHANGE(pOrg->gridMainCurC     ,pRat->gridMainCurC);
    VirtulADval.gridBypassCurA    = SETUP_RATIO_CHANGE(pOrg->gridBypassCurA   ,pRat->gridBypassCurA);
    VirtulADval.gridBypassCurB    = SETUP_RATIO_CHANGE(pOrg->gridBypassCurB   ,pRat->gridBypassCurB);
    VirtulADval.gridBypassCurC    = SETUP_RATIO_CHANGE(pOrg->gridBypassCurC   ,pRat->gridBypassCurC);
    VirtulADval.aDCUDCA           = 0;
    VirtulADval.aDCUDCB           = 0;
    VirtulADval.aDCUDCC           = 0;

    gridVoltRatio           = -gridVoltRatioVirtu;       //电网电压系数
    loadVoltRatio           = -loadVoltRatioVirtu;       //负载电压系数
    outputCurRatio          = outputCurRatioVirtu;      //主电抗电流系数
    bypassCurrRatio         = bypassCurrRatioVirtu;      //旁路电流系数
    dcCapVoltRatio          = DcCapVoltRatioVirtu;       //直流吸收电容电压系数

    outputCurRatioCurrA     = outputCurRatio;
    outputCurRatioCurrB     = outputCurRatio;
    outputCurRatioCurrC     = outputCurRatio;
    outputCurBypassCurrA    = bypassCurrRatio;
    outputCurBypassCurrB    = bypassCurrRatio;
    outputCurBypassCurrC    = bypassCurrRatio;

//    if(StateFlag.VoltageModeFlag == 0){
//        outputCurRatioCurrA     = -outputCurRatio;
//        outputCurRatioCurrB     = outputCurRatio;
//        outputCurRatioCurrC     = outputCurRatio;
//        outputCurBypassCurrA    = -dcVoltUpRatio;       //旁路电流系数A
//        outputCurBypassCurrB    = dcVoltUpRatio;
//        outputCurBypassCurrC    = dcVoltUpRatio;
//    }else if(StateFlag.VoltageModeFlag == 1){
//        outputCurRatioCurrA     = -outputCurRatio;
//        outputCurRatioCurrB     = -outputCurRatio;
//        outputCurRatioCurrC     = -outputCurRatio;
//        outputCurBypassCurrA    = -dcVoltUpRatio;       //旁路电流系数A
//        outputCurBypassCurrB    = -dcVoltUpRatio;
//        outputCurBypassCurrC    = -dcVoltUpRatio;
//    }
    transfRatio                 = transfRatioVirtu;
}

void CapParamRefresh(void)
{
	int i;
	CapStartLimit = NegCurLimit*CapGroupAct.SvgPercent*0.0001;
	CapCompenNum = CapGroupAct.CapDivdNum & 0x1F;
	CapDisperNum = (CapGroupAct.CapDivdNum>>5) & 0x1F;
	DisperACount=DisperAOffCount=(CapCompenNum);
	DisperBCount=DisperBOffCount=(CapCompenNum+1);
	DisperCCount=DisperCOffCount=(CapCompenNum+2);
	if((!StateFlag.CapStateFlag)&&(CapGroupAct.capgroupswitch.B.CapCompensation)){
	    for(i=0;i<20;i++)   CapGroupAct.PhactGroup[i]=0x10;
	}
}
void initDPLL(SPLL_3ph_DDSRF *SPLL)
{
    SPLL->Theta = 0.01f;
    SPLL->GridPLLVoltD = 0.1f;
    SPLL->GridPLLVoltDn = 0.11f;
    SPLL->GridPLLVoltQ = 0.12f;
    SPLL->GridPLLVoltQn = 0.13f;
    SPLL->PLLResCos = 0.14f;
    SPLL->PLLResSin = 0.15f;
    SPLL->PLLRes2Cos = 0.16f;
    SPLL->PLLRes2Sin = 0.17f;
    MCUfilterMemCopy(&SPLL->filter[0],                   PLL_VOLT_Q_DEFAULT_NUM);
    MCUfilterMemCopy(&SPLL->filter[1],                   PLL_VOLT_Q_DEFAULT_NUM);
    MCUfilterMemCopy(&SPLL->filter[2],                   PLL_VOLT_Q_DEFAULT_NUM);
    MCUfilterMemCopy(&SPLL->filter[3],                   PLL_VOLT_Q_DEFAULT_NUM);
    MCUfilterMemCopy(&SPLL->filter[4],                   PLL_VOLT_Q_DEFAULT_NUM);
}
//float AGCVal = 0.1f;
void PLLrunABC(SPLL_3ph_DDSRF *SPLL,float in,float delay)
{
    float32 GridVoltAlpha = in;//SQRT_2DIV3 *(GridCurrAF-0.5f*GridCurrBF - 0.5f*GridCurrCF);
    float32 GridVoltBeta =  delay;//SQRT2_DIV2 *(GridCurrBF - GridCurrCF);
    float32 gridVoltAlphaMulPLLResCos   =GridVoltAlpha* SPLL->PLLResCos; //sin*alpha
    float32 gridVoltAlphaMulPLLResSin   =GridVoltAlpha* SPLL->PLLResSin; //-cos*alpha
    float32 gridVoltBetaMulPLLResSin    =GridVoltBeta * SPLL->PLLResSin; //-cos*beta
    float32 gridVoltBetaMulPLLResCos    =GridVoltBeta * SPLL->PLLResCos; //sin*beta
    float32 *pIn  = SPLL->Buf;
    float32 *pOut = SPLL->Buf;
    *pIn++ = isqrt(SPLL->GridPLLVoltD*SPLL->GridPLLVoltD + SPLL->GridPLLVoltQ*SPLL->GridPLLVoltQ);
    *pIn++ =    gridVoltAlphaMulPLLResCos   + gridVoltBetaMulPLLResSin - SPLL->GridPLLVoltDn * SPLL->PLLRes2Cos -  SPLL->GridPLLVoltQn * SPLL->PLLRes2Sin;
    *pIn++ =  - gridVoltAlphaMulPLLResSin   + gridVoltBetaMulPLLResCos + SPLL->GridPLLVoltDn * SPLL->PLLRes2Sin -  SPLL->GridPLLVoltQn * SPLL->PLLRes2Cos;
    *pIn++ =    gridVoltAlphaMulPLLResCos   - gridVoltBetaMulPLLResSin - SPLL->GridPLLVoltD  * SPLL->PLLRes2Cos;// +  SPLL->GridPLLVoltQ  * SPLL->PLLRes2Sin;   //Q接近0
    *pIn   =    gridVoltAlphaMulPLLResSin   + gridVoltBetaMulPLLResCos - SPLL->GridPLLVoltD  * SPLL->PLLRes2Sin;// -  SPLL->GridPLLVoltQ  * SPLL->PLLRes2Cos;
    DCL_runDF22Group(SPLL->filter,SPLL->Buf,SPLL->Buf,GRIDVOL_DQFILTER_LEN);
    float AGCVal      = *pOut++;
    SPLL->GridPLLVoltD   = *pOut++;
    SPLL->GridPLLVoltQ   = *pOut++;
    SPLL->GridPLLVoltDn  = *pOut++;
    SPLL->GridPLLVoltQn  = *pOut;

//    if(AGCVal>1/(0.1f*SQRT2*SQRT_3DIV2))        AGCVal=1/(0.1f*SQRT2*SQRT_3DIV2);       //0.1A
//    if(AGCVal<1/(3000*1.2f*SQRT2*SQRT_3DIV2))    AGCVal=1/(3000*1.2f*SQRT2*SQRT_3DIV2);   //3000A
    SPLL->PllPiOutput =DCL_runPI(&SPLL->PLLPICtrl, SPLL->GridPLLVoltQ*AGCVal , 0);         //PID,AGC
    SPLL->Theta=PhaseLimit(IntergrateBackEuler(SPLL->PllPiOutput,SPLL->Theta));          //Integration and get same phase as the voltage 0 degrees.积分得到与电压0度相同的相位。

    sincosf(SPLL->Theta,&SPLL->PLLResSin,&SPLL->PLLResCos);
    sincosf(PhaseLimit(SPLL->Theta*2),&SPLL->PLLRes2Sin,&SPLL->PLLRes2Cos);
    //关于锁相的相位,见https://mp.csdn.net/mdeditor/101423306

//    float Esc_PhaseA = SPLL->Theta+PllCalibrPhase;
//    while(Esc_PhaseA > 2*PI)
//    {
//        Esc_PhaseA -= 2*PI;
//    }
}

//grid voltage mathematical model is cosine
void PLLrun(void)
{
    PLLrunABC(&SPLL[0],VoltInAF,VoltInAF_Beta);
    PLLrunABC(&SPLL[1],VoltInBF,VoltInBF_Beta);
    PLLrunABC(&SPLL[2],VoltInCF,VoltInCF_Beta);

    float32 GridCurAlpha = SQRT_2DIV3 *(GridCurrAF-0.5f*GridCurrBF - 0.5f*GridCurrCF);
    float32 GridCurBeta =  SQRT2_DIV2 *(GridCurrBF - GridCurrCF);
    float32 gridCurAlphaMulPLLResCos   =GridCurAlpha* SPLL_CUR.PLLResCos; //sin*alpha
    float32 gridCurAlphaMulPLLResSin   =GridCurAlpha* SPLL_CUR.PLLResSin; //-cos*alpha
    float32 gridCurBetaMulPLLResSin    =GridCurBeta * SPLL_CUR.PLLResSin; //-cos*beta
    float32 gridCurBetaMulPLLResCos    =GridCurBeta * SPLL_CUR.PLLResCos; //sin*beta
    float32 *pIn_Cur  = SPLL_CUR.Buf;
    float32 *pOut_Cur = SPLL_CUR.Buf;
    *pIn_Cur++ = isqrt(SPLL_CUR.GridPLLVoltD*SPLL_CUR.GridPLLVoltD + SPLL_CUR.GridPLLVoltQ*SPLL_CUR.GridPLLVoltQ);
    *pIn_Cur++ =    gridCurAlphaMulPLLResCos   + gridCurBetaMulPLLResSin - SPLL_CUR.GridPLLVoltDn * SPLL_CUR.PLLRes2Cos -  SPLL_CUR.GridPLLVoltQn * SPLL_CUR.PLLRes2Sin;
    *pIn_Cur++ =  - gridCurAlphaMulPLLResSin   + gridCurBetaMulPLLResCos + SPLL_CUR.GridPLLVoltDn * SPLL_CUR.PLLRes2Sin -  SPLL_CUR.GridPLLVoltQn * SPLL_CUR.PLLRes2Cos;
    *pIn_Cur++ =    gridCurAlphaMulPLLResCos   - gridCurBetaMulPLLResSin - SPLL_CUR.GridPLLVoltD  * SPLL_CUR.PLLRes2Cos;// +  SPLL_CUR.GridPLLVoltQ  * SPLL_CUR.PLLRes2Sin;   //Q接近0
    *pIn_Cur   =    gridCurAlphaMulPLLResSin   + gridCurBetaMulPLLResCos - SPLL_CUR.GridPLLVoltD  * SPLL_CUR.PLLRes2Sin;// -  SPLL_CUR.GridPLLVoltQ  * SPLL_CUR.PLLRes2Cos;
    DCL_runDF22Group(SPLL_CUR.filter,SPLL_CUR.Buf,SPLL_CUR.Buf,GRIDVOL_DQFILTER_LEN);
    float32 AGCVal_Cur      = *pOut_Cur++;
    SPLL_CUR.GridPLLVoltD   = *pOut_Cur++;
    SPLL_CUR.GridPLLVoltQ   = *pOut_Cur++;
    SPLL_CUR.GridPLLVoltDn  = *pOut_Cur++;
    SPLL_CUR.GridPLLVoltQn  = *pOut_Cur;

    if(AGCVal_Cur>1/(5.0f*SQRT2*SQRT_3DIV2))        AGCVal_Cur=1/(5.0f*SQRT2*SQRT_3DIV2);       //5V
    if(AGCVal_Cur<1/(690*1.2f*SQRT2*SQRT_3DIV2))    AGCVal_Cur=1/(690*1.2f*SQRT2*SQRT_3DIV2);   //220V
    SPLL_CUR.PllPiOutput =DCL_runPI(&SPLL_CUR.PLLPICtrl, SPLL_CUR.GridPLLVoltQ*AGCVal_Cur , 0);         //PID,AGC
    SPLL_CUR.Theta=PhaseLimit(IntergrateBackEuler(SPLL_CUR.PllPiOutput,SPLL_CUR.Theta));          //Integration and get same phase as the voltage 0 degrees.积分得到与电压0度相同的相位。
    //关于锁相的相位,见https://mp.csdn.net/mdeditor/101423306

    Esc_CurPhaseA = SPLL_CUR.Theta+3*PI/2+PllCalibrPhase;
    while(Esc_CurPhaseA > 2*PI)
    {
        Esc_CurPhaseA -= 2*PI;
    }
    Esc_CurPhaseB = Esc_CurPhaseA+4*PI/3;
    while(Esc_CurPhaseB > 2*PI)
    {
        Esc_CurPhaseB -= 2*PI;
    }
    Esc_CurPhaseC = Esc_CurPhaseA + 2*PI/3;
    while(Esc_CurPhaseC > 2*PI)
    {
        Esc_CurPhaseC -= 2*PI;
    }

    sincosf(SPLL_CUR.Theta,&SPLL_CUR.PLLResSin,&SPLL_CUR.PLLResCos);
    sincosf(PhaseLimit(SPLL_CUR.Theta*2),&SPLL_CUR.PLLRes2Sin,&SPLL_CUR.PLLRes2Cos);


    Esc_VoltPhaseA = SPLL[0].Theta+PI/2+PllCalibrPhase;
    while(Esc_VoltPhaseA > 2*PI)
    {
        Esc_VoltPhaseA -= 2*PI;
    }
    Esc_VoltPhaseB = SPLL[1].Theta+PI/2+PllCalibrPhase;
    while(Esc_VoltPhaseB > 2*PI)
    {
        Esc_VoltPhaseB -= 2*PI;
    }
    Esc_VoltPhaseC = SPLL[2].Theta+PI/2+PllCalibrPhase;
    while(Esc_VoltPhaseC > 2*PI)
    {
        Esc_VoltPhaseC -= 2*PI;
    }

    GridVoltTheta = Esc_VoltPhaseA;
//    GridVoltTheta = PhaseLimit(SPLL[0].Theta-PllCalibrPhase+PI2); //Corrected to the lead of the PI/2 phase-locked angle.超前90.360是为了保持界面参数前向兼容.
    sincosf(GridVoltTheta,&GridResSin,&GridResCos);               //这个相位是与实际电网对应的
    GridFreq = DCL_runDF22(&bwForFreq, SPLL[0].PllPiOutput *(1/PI2));

}

#if PR_CTRL==1
const DF22BLOCK PRCtrlSrcBK[41]         = PRCTRL_BLOCK_DEFAULT;
const float PRCtrlSrcFulc[41*2]         = PRCTRL_BLOCK_FULC;
const int PRCtrlEn[PRCTRL_COEFF_LEN]    =PRCTRL_COEFF_EN;

float PRCtrlFulcFirst[PRCTRL_COEFF_LEN*2],PRCtrlBKFirst[PRCTRL_COEFF_LEN*2];

DF22BLOCK PRCtrlABK[(PRCTRL_COEFF_LEN)],PRCtrlBBK[(PRCTRL_COEFF_LEN)],PRCtrlCBK[(PRCTRL_COEFF_LEN)];
#pragma CODE_SECTION(AdjustFulkPRcoeff, "ram2func");
void AdjustFulkPRcoeff(void)
{
    int i;
    volatile float *pA,*pB,*pC;
    volatile float const *pSrc=PRCtrlBKFirst,*pFluc=PRCtrlFulcFirst;
    float PRFreq;

#if TEST_RUNTIME
    if(1){      //测试运行时间时一直让此函数运行
        if(1){
#else
    if(!StateFlag.spwmFlag ){
        if((PrvFreq-GridFreq)>0.05 || (PrvFreq-GridFreq)<-0.05){
#endif
            PrvFreq = GridFreq;
            PRFreq=GridFreq-50;
            if(PRFreq>1)PRFreq=1;   if(PRFreq<-1)PRFreq=-1;

            pA=&PRCtrlABK[0].b1;                            //将要被修改的运行中的PR系数地址
            pB=&PRCtrlBBK[0].b1;
            pC=&PRCtrlCBK[0].b1;
            for(i=0;i<PRCTRL_COEFF_LEN;i++){
                *pC++=*pB++=*pA++=  (*pFluc++ * PRFreq)+*pSrc++;    //运行中PR系数地址=PR随电网频率变化系数+PR系数
                *pC=*pB=*pA=        (*pFluc++ * PRFreq)+*pSrc++;
                pC+=5;pB+=5;pA+=5;
            }

//-         for(i=0;i<PRCTRL_COEFF_LEN;i++){
//-             pA=&PRCtrlABK[i].b1;                            //将要被修改的运行中的PR系数地址
//-             pB=&PRCtrlBBK[i].b1;
//              pC=&PRCtrlCBK[i].b1;
//              pSrc =&PRCtrlSrcBK[PRCtrlEn[i]-1].b1;           //PR系数
//              pFluc=&PRCtrlSrcFulc[(PRCtrlEn[i]-1)<<1];       //PR随电网频率变化系数
//              *pC++=*pB++=*pA++=(*pFluc++ * PRFreq)+*pSrc++;  //运行中PR系数地址=PR随电网频率变化系数+PR系数
//              *pC=*pB=*pA=(*pFluc * PRFreq)+*pSrc;
//          }
        }
    }
    //-AdjustRCoeff=PRFreq/(12.1-11.7)//51&49Hz 2.4@117 50&2.4@121
}

void IntPRcoeff(void)
{
    volatile float *pA,*pB,*pC;
    volatile float *pBKDst,*pFlucDst;
    volatile float const *pSrc,*pFluc;
    int16 i,j;


    for(i=0;i<PRCTRL_COEFF_LEN;i++){
        pA=&PRCtrlABK[i].b0;
        pB=&PRCtrlBBK[i].b0;
        pC=&PRCtrlCBK[i].b0;
        pSrc=&PRCtrlSrcBK[PRCtrlEn[i]-1].b0;
        for(j=0;j<PRCOEFF_ELEMENUMBK;j++){
            *pC++=*pB++=*pA++=*pSrc++;
        }
    }

    //产生频率变化的高效运行的初始化表
    pBKDst=PRCtrlBKFirst;
    pFlucDst=PRCtrlFulcFirst;
    for(i=0;i<PRCTRL_COEFF_LEN;i++){
        pSrc =&PRCtrlSrcBK[PRCtrlEn[i]-1].b1;           //PR系数
        pFluc=&PRCtrlSrcFulc[(PRCtrlEn[i]-1)<<1];       //PR随电网频率变化系数
        *pFlucDst++=*pFluc++;                           //运行中PR系数地址=PR随电网频率变化系数+PR系数,第一个系数
        *pFlucDst++=*pFluc;
        *pBKDst++=*pSrc++;                              //第二组系数
        *pBKDst++=*pSrc;                                //
    }

#if ADDCAC_INV_TWO_LEVEL_PWM == 1
    //PR控制2,用于电流源输出,因为输出是准确的50Hz,故无频率变化表.
    for(i=0;i<PRCTRL_COEFF_LEN2;i++){
        pA=&PRCtrlSBK[i].b0;
        pSrc=&PRCtrlSrcBK[PRCtrlEn2[i]-1].b0;
        for(j=0;j<PRCOEFF_ELEMENUMBK;j++){
            *pA++=*pSrc++;
        }
    }
#endif
}
#else
void IntPRcoeff(void)
{;}
void AdjustFulkPRcoeff(void)
{;}
#endif


/************************** No More ********************************************/
