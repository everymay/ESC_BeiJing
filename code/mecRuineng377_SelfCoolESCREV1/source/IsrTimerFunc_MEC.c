#include "F2837xS_device.h"     // DSP2833x Headerfile Include File
#include "F2837xS_Examples.h"   // DSP2833x Examples Include File
#include "math.h"
#include "C28x_FPU_FastRTS.h"
#include "MarcoDefine.h"
#include "VariblesRef.h"
#include "CoreCtrl.h"
#include "VarMean.h"
#include "DCL.h"
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Swi.h>
#include "VarMean.h"
#include "stdlib.h"
#include "DPLL.h"

extern const Swi_Handle FFTstart;
float32 ADBuff[6][ADTEMPBUFLEN];
int T1PRPwmFrequency = 0;

/*
 * 节拍。WY
 * 该变量在ADC-D-1的中断ISR中被使用。
 */
int ADBufPos = 0;

Uint16 FFTSrcBufp=0;
Uint16 ADGROUP_NUM=8;
Uint16 ADGROUP_MODE=2;
Uint16 WAVE_REC_FFT_MODE_DATA2=4;


float VoltSlid[VOLT_FUNDPOINT][3];
float VoltSlidA[MEANPOINTHALF],VoltSlidB[MEANPOINTHALF],VoltSlidC[MEANPOINTHALF];
float GridCurrSlidA[MEANPOINTHALF],GridCurrSlidB[MEANPOINTHALF],GridCurrSlidC[MEANPOINTHALF];
float GridCurrAF_Beta,GridCurrBF_Beta,GridCurrCF_Beta;
float GridRealCurDA2,GridRealCurDB2,GridRealCurDC2,GridRealCurQA2,GridRealCurQB2,GridRealCurQC2;
float GridRealCurErrA,GridRealCurErrB,GridRealCurErrC;

extern const Swi_Handle RMSstart;

#if TESTMODE
    float32 DbgStepPhaA=D2R(0),DbgStepPhaB=D2R(240),DbgStepPhaC=D2R(120);
#endif


#pragma	CODE_SECTION(ADCD1INT               ,"ram2func")
#if PWM_FREQUENCE_16KHZ
#pragma CODE_SECTION(PIController           ,"ram2func")
#endif

#pragma	CODE_SECTION(RMSDutyLimit            ,"ram2func")
#pragma CODE_SECTION(ReactivePowerComFUN     ,"ram2func")
#pragma CODE_SECTION(UnCurrCompFUN           ,"ram2func")

#pragma	CODE_SECTION(FaultRecordSel       ,"ram2func")
#pragma CODE_SECTION(AdRegeditOffset       ,"ram2func")

#pragma CODE_SECTION(RMSswi                 ,"ram2func")

Uint16 MeanHalfPos = 0;
inline void FaultRecordSel(void);


void getFFTData(void)
{
    ADBuff[0][FFTSrcBufp]  =LoadRealCurA*MU_MultRatio;
    ADBuff[2][FFTSrcBufp]  =LoadRealCurB*MU_MultRatio;
    ADBuff[4][FFTSrcBufp]  =LoadRealCurC*MU_MultRatio;
    ADBuff[1][FFTSrcBufp]  =ApfOutCurA;//LoadRealCurA;//;
    ADBuff[3][FFTSrcBufp]  =ApfOutCurB;//LoadRealCurB;//ApfOutCurB;//
    ADBuff[5][FFTSrcBufp]  =ApfOutCurC;//LoadRealCurC;//ApfOutCurC;//;
    if(++FFTSrcBufp >= ADTEMPBUFLEN) FFTSrcBufp=0;
}

/*
 * 功能：计算电压和电流。WY
 * 说明：该函数在ADC-D-1的中断服务函数中被调用。
 */
inline void GetVolAndInvCurr(void)
{
	VirtulADStruVAL *pAD = &VirtulADVAL; //AD真实采样值。WY
	VirtulADStruval *pID = &VirtulADval; //由触控屏下发AD零偏基准值。WY

	/*
	 * 经零偏校准后的模拟量（瞬时值）。WY
	   [0]：A相负载电压（瞬时值）
	   [1]：B相负载电压（瞬时值）
	   [2]：C相负载电压（瞬时值）

	   [3]：A相电网电压（瞬时值）
	   [4]：B相电网电压（瞬时值）
	   [5]：C相电网电压（瞬时值）

	   [6]：A相负载电流（瞬时值）
	   [7]：B相负载电流（瞬时值）
	   [8]：C相负载电流（瞬时值）

	   [9]：A相旁路电流（瞬时值）
	   [10]：B相旁路电流（瞬时值）
	   [11]：C相旁路电流（瞬时值）

	   [12]：A相直流电容电压（瞬时值）
	   [13]：B相直流电容电压（瞬时值）
	   [14]：C相直流电容电压（瞬时值）
	 */
	float Buff[15];

	float *pBuff = Buff; //指向浮点数的指针，指向经零偏校准后的模拟量。WY

	/*降压模式。WY*/
	if(StateFlag.VoltageModeFlag == 1)
	{
		*pBuff ++ = (*pAD->GridHVoltA - pID->gridHVoltA) * gridVoltRatio;                  //电网电压,电压采样系数        //电网电压采样点(运放输出点)与实际电压波形方向反向
		*pBuff ++ = (*pAD->GridHVoltB - pID->gridHVoltB) * gridVoltRatio;
		*pBuff ++ = (*pAD->GridHVoltC - pID->gridHVoltC) * gridVoltRatio;

		*pBuff ++ = (*pAD->GridLVoltA - pID->gridLVoltA) * loadVoltRatio;                  //负载电压,电压采样系数        //负载电压采样点(运放输出点)与实际电压波形方向反向
		*pBuff ++ = (*pAD->GridLVoltB - pID->gridLVoltB) * loadVoltRatio;
		*pBuff ++ = (*pAD->GridLVoltC - pID->gridLVoltC) * loadVoltRatio;

		*pBuff ++ = (*pAD->GridMainCurA - pID->gridMainCurA) * outputCurRatioCurrA;                  //电抗主电流,输出电流采样系数   //主电抗电流采样点(运放输出点)与实际电流波形方向相反(继电器板霍尔采样反了)
		*pBuff ++ = (*pAD->GridMainCurB - pID->gridMainCurB) * outputCurRatioCurrB;                  //A相电流方向与B,C相电流方向相反,需要解决?????--LJH
		*pBuff ++ = (*pAD->GridMainCurC - pID->gridMainCurC) * outputCurRatioCurrC;

		*pBuff ++ = (*pAD->GridBypassCurA - pID->gridBypassCurA) * outputCurBypassCurrA;                  //旁路电流,输出电流采样系数   //旁路电流采样点(运放输出点)与实际电流波形方向相反(需验证???)
		*pBuff ++ = (*pAD->GridBypassCurB - pID->gridBypassCurB) * outputCurBypassCurrB;                  //A相电流方向与B,C相电流方向相反,需要解决?????--LJH
		*pBuff ++ = (*pAD->GridBypassCurC - pID->gridBypassCurC) * outputCurBypassCurrC;

		*pBuff ++ = (*pAD->ADCUDCA - pID->aDCUDCA) * dcCapVoltRatio;                  //直流电容电压,电压采样系数
		*pBuff ++ = (*pAD->ADCUDCB - pID->aDCUDCB) * dcCapVoltRatio;
		*pBuff ++ = (*pAD->ADCUDCC - pID->aDCUDCC) * dcCapVoltRatio;
	}
	else if(StateFlag.VoltageModeFlag == 0) //升压模式。WY
	{
		*pBuff ++ = (*pAD->GridLVoltA - pID->gridLVoltA) * loadVoltRatio;                  //负载电压,电压采样系数        //负载电压采样点(运放输出点)与实际电压波形方向反向
		*pBuff ++ = (*pAD->GridLVoltB - pID->gridLVoltB) * loadVoltRatio;
		*pBuff ++ = (*pAD->GridLVoltC - pID->gridLVoltC) * loadVoltRatio;

		*pBuff ++ = (*pAD->GridHVoltA - pID->gridHVoltA) * gridVoltRatio;                  //电网电压,电压采样系数        //电网电压采样点(运放输出点)与实际电压波形方向反向
		*pBuff ++ = (*pAD->GridHVoltB - pID->gridHVoltB) * gridVoltRatio;
		*pBuff ++ = (*pAD->GridHVoltC - pID->gridHVoltC) * gridVoltRatio;

		*pBuff ++ = (*pAD->GridMainCurA - pID->gridMainCurA) * outputCurRatioCurrA;                  //电抗主电流,输出电流采样系数   //主电抗电流采样点(运放输出点)与实际电流波形方向相反(继电器板霍尔采样反了)
		*pBuff ++ = (*pAD->GridMainCurB - pID->gridMainCurB) * outputCurRatioCurrB;                  //A相电流方向与B,C相电流方向相反,需要解决?????--LJH
		*pBuff ++ = (*pAD->GridMainCurC - pID->gridMainCurC) * outputCurRatioCurrC;

		*pBuff ++ = (*pAD->GridBypassCurA - pID->gridBypassCurA) * outputCurBypassCurrA;                  //旁路电流,输出电流采样系数   //旁路电流采样点(运放输出点)与实际电流波形方向相反(需验证???)
		*pBuff ++ = (*pAD->GridBypassCurB - pID->gridBypassCurB) * outputCurBypassCurrB;                  //A相电流方向与B,C相电流方向相反,需要解决?????--LJH
		*pBuff ++ = (*pAD->GridBypassCurC - pID->gridBypassCurC) * outputCurBypassCurrC;

		*pBuff ++ = (*pAD->ADCUDCA - pID->aDCUDCA) * dcCapVoltRatio;                  //直流电容电压,电压采样系数
		*pBuff ++ = (*pAD->ADCUDCB - pID->aDCUDCB) * dcCapVoltRatio;
		*pBuff ++ = (*pAD->ADCUDCC - pID->aDCUDCC) * dcCapVoltRatio;
	}

	DCL_runDF22Group(VolAndInvCurrFilter, Buff, Buff, 15);  //滤波

	pBuff = Buff;

	float *pOutSrc = VoltSlid[VoltPrvPos]; //实时采样波形

	/*降压模式。WY*/
	if(StateFlag.VoltageModeFlag == 1)
	{
		*pOutSrc ++ = *pBuff ++;
		*pOutSrc ++ = *pBuff ++;
		*pOutSrc ++ = *pBuff ++;

		pOutSrc = VoltSlid[VoltPos];                  //进行电压校正

		GridVoltAF = *pOutSrc ++;                  //电网电压
		GridVoltBF = *pOutSrc ++;
		GridVoltCF = *pOutSrc ++;

		LoadVoltUF = *pBuff ++;                  //负载电压
		LoadVoltVF = *pBuff ++;
		LoadVoltWF = *pBuff ++;
	}
	else if(StateFlag.VoltageModeFlag == 0) //升压模式。WY
	{
		*pOutSrc ++ = *pBuff ++; //负载电压。WY
		*pOutSrc ++ = *pBuff ++;
		*pOutSrc ++ = *pBuff ++;

		pOutSrc = VoltSlid[VoltPos];                  //进行电压校正

		LoadVoltUF = *pOutSrc ++;
		LoadVoltVF = *pOutSrc ++;
		LoadVoltWF = *pOutSrc ++;

		GridVoltAF = *pBuff ++; //电网电压
		GridVoltBF = *pBuff ++;
		GridVoltCF = *pBuff ++;
	}

	GridCurrAF = *pBuff ++;                  //电抗主电流
	GridCurrBF = *pBuff ++;
	GridCurrCF = *pBuff ++;

	GridBPCurrAF = *pBuff ++;                  //旁路电流
	GridBPCurrBF = *pBuff ++;
	GridBPCurrCF = *pBuff ++;

	DccapVoltA = *pBuff ++;                  //直流电容电压
	DccapVoltB = *pBuff ++;
	DccapVoltC = *pBuff;

	if(StateFlag.VoltageModeFlag == 1)
	{                  //降压
		VoltInAF = GridVoltAF;
		VoltInBF = GridVoltBF;
		VoltInCF = GridVoltCF;
		VoltOutAF = LoadVoltUF;
		VoltOutBF = LoadVoltVF;
		VoltOutCF = LoadVoltWF;
	}
	else if(StateFlag.VoltageModeFlag == 0)
	{                  //升压
		VoltInAF = LoadVoltUF;
		VoltInBF = LoadVoltVF;
		VoltInCF = LoadVoltWF;
		VoltOutAF = GridVoltAF;
		VoltOutBF = GridVoltBF;
		VoltOutCF = GridVoltCF;
	}

//延时90度构造Beta    单相锁相

	VoltSlidA[MeanHalfPos] = VoltInAF;                  //grid real value
	VoltSlidB[MeanHalfPos] = VoltInBF;
	VoltSlidC[MeanHalfPos] = VoltInCF;
	GridCurrSlidA[MeanHalfPos] = GridCurrAF;
	GridCurrSlidB[MeanHalfPos] = GridCurrBF;
	GridCurrSlidC[MeanHalfPos] = GridCurrCF;

	if(MeanHalfPos == (MEANPOINTHALF - 1))
	{
		VoltInAF_Beta = VoltSlidA[0];
		VoltInBF_Beta = VoltSlidB[0];
		VoltInCF_Beta = VoltSlidC[0];
		GridCurrAF_Beta = GridCurrSlidA[0];
		GridCurrBF_Beta = GridCurrSlidB[0];
		GridCurrCF_Beta = GridCurrSlidC[0];

	}
	else
	{
		VoltInAF_Beta = VoltSlidA[MeanHalfPos + 1];
		VoltInBF_Beta = VoltSlidB[MeanHalfPos + 1];
		VoltInCF_Beta = VoltSlidC[MeanHalfPos + 1];
		GridCurrAF_Beta = GridCurrSlidA[MeanHalfPos + 1];
		GridCurrBF_Beta = GridCurrSlidB[MeanHalfPos + 1];
		GridCurrCF_Beta = GridCurrSlidC[MeanHalfPos + 1];
	}
	if(++ MeanHalfPos >= MEANPOINTHALF)
		MeanHalfPos = 0;
}

#define DQ2UALPHA_NEG(D,Q)  dq2Ualpha = (D)*GridResCos + (Q)*GridResSin;\
                            dq2Ubeta = -(D)*GridResSin + (Q)*GridResCos
#define DQ2UALPHA_POS(D,Q)  dq2Ualpha = (D)*GridResCos - (Q)*GridResSin;\
                            dq2Ubeta =  (D)*GridResSin + (Q)*GridResCos
#define ICLACK(A,B,C,Z)     A = (Z)+dq2Ualpha* SQRT_2DIV3;\
                            B = (Z)+dq2Ualpha*-SQRT_2DIV3_DIV2 + dq2Ubeta * SQRT2_DIV2;\
                            C = (Z)+dq2Ualpha*-SQRT_2DIV3_DIV2 + dq2Ubeta *-SQRT2_DIV2
//function of output fundamental positive sequence (active and reactive) and negative sequence (active and reactive power) and zero sequence.
//输出基本正序(有功功率和无功功率)、负序(有功功率和无功功率)和零序的函数。
inline void InverseTransformF(void)
{
}

//function of output fundamental positive sequence (active and reactive) and negative sequence (active and reactive power) and zero sequence.
//输出基本正序(有功功率和无功功率)、负序(有功功率和无功功率)和零序的函数。
void CurrRefCaul(void)
{
    InverseTransformF();            //1.26us,Fundamental wave inverse transform
    float tmp,currHarmRefA,currHarmRefB,currHarmRefC;
    float corr1 =CapaUnbalCorr*outCurTempLimit;
    float corr2 =HarmLimtCorr*outCurTempLimit;
    if(!StateFlag.isHarmCompensateMode){
        TestADTempBufp=SPLL[0].Theta*(GEN_FUNDPOINT/PI2);
        currHarmRefA=TestLoadCurS(0);                   //
        currHarmRefB=TestLoadCurS(1);                   //
        currHarmRefC=TestLoadCurS(2);                   //
        FundwaveA=0;FundwaveB=0;FundwaveC=0;
    }else{
        currHarmRefA = CurrHarmRefA;
        currHarmRefB = CurrHarmRefB;
        currHarmRefC = CurrHarmRefC;
    }

    tmp=CurFundNgA*corr1+currHarmRefA*corr2;
    CurrRefStandbyA =CurFundStandbyA-tmp;
    CurrRefA        =CurFundA       -tmp;
    if(CurrRefA>OutCurMaxLimit)     CurrRefA=OutCurMaxLimit;
    if(CurrRefA<OutCurMaxLimitNeg)  CurrRefA=OutCurMaxLimitNeg;

    tmp=CurFundNgB*corr1+currHarmRefB*corr2;
    CurrRefStandbyB =CurFundStandbyB-tmp;
    CurrRefB        =CurFundB       -tmp;
    if(CurrRefB>OutCurMaxLimit)     CurrRefB=OutCurMaxLimit;
    if(CurrRefB<OutCurMaxLimitNeg)  CurrRefB=OutCurMaxLimitNeg;

    tmp=CurFundNgC*corr1+currHarmRefC*corr2;
    CurrRefStandbyC =CurFundStandbyC-tmp;
    CurrRefC        =CurFundC       -tmp;

    if(CurrRefC>OutCurMaxLimit)     CurrRefC=OutCurMaxLimit;
    if(CurrRefC<OutCurMaxLimitNeg)  CurrRefC=OutCurMaxLimitNeg;
}

/*该函数未定义。WY*/
void GetGridLoadcurrAD(void)
{
}

/*该函数未定义。WY*/
void GetGridLoadcurr(void)
{

}

/*该函数未定义。WY*/
void GetUdc(void)
{
}


inline void FaultRecordSel(void)
{
	struct Stru_FaultRecSel *pSel=&FaultRecSel;
	if((pSel->ForceRecordWaveRefresh)&&(GridVoltTheta<(1.5*50*PI2/CTRLFREQUENCY))){ //界面波形显示强制刷新同时落入1.5倍的最小运算步长范围,认为是锁相角度的过零点
		pSel->ForceRecordWaveRefresh = FALSE;				//从0开始刷新一整个缓冲区
		CntForRec = DEBUG_WAVE_LEN-1;						//刷新2000个点
		pSel->DelayRec = DEBUG_WAVE_LEN-1;					//刷新2000个点
		pSel->isFFTMode = TRUE;								//切换为FFT模式
		pSel->RecordChan = 1;								//第1组固定为FFT模式的AD通道
		CntSec.ForceRecordWaveRefreshCnt =0;
	}
	if(pSel->isFFTMode){ 									//FFT模式//实时波形显示
		if(CntSec.ForceRecordWaveRefreshCnt>30){
			pSel->isFFTMode=0;								//延时30秒后自动关闭FFT模式
			pSel->RecordChan = pSel->RecordChanOrg;
			CntSec.ForceRecordWaveRefreshCnt =0;
		}
	}else{													//故障录波模式
		if( (!pSel->stopRecordMode)&&\
		  	(/*((StateEventFlag_A == STATE_EVENT_RUN_A) || (StateEventFlag_A == STATE_EVENT_RECHARGE_A) || (StateEventFlag_A == STATE_EVENT_WAIT_A))&& \
			 ((StateEventFlag_B == STATE_EVENT_RUN_B) || (StateEventFlag_B == STATE_EVENT_RECHARGE_B) || (StateEventFlag_B == STATE_EVENT_WAIT_B))&& \*/
			 ((StateEventFlag_C == STATE_EVENT_RUN_C) || (StateEventFlag_C == STATE_EVENT_RECHARGE_C) || (StateEventFlag_C == STATE_EVENT_WAIT_C))) ){	//停机停止录波模式
			pSel->DelayRec = 50;							//停机后录50个点
		}else if(pSel->stopRecordMode){						//一直录波模式
				pSel->DelayRec = DEBUG_WAVE_LEN-1;						//一直录波
		}
	}
	ErrorRecord.B.REC_FFT_MODE = pSel->isFFTMode;
}

/*
 * 功能：产生DSP心跳脉冲。WY
 * 说明：通过翻转GPIO电平，产生DSP心跳脉冲。
   该函数在ADC-D-1的中断ISR中被调用，故脉冲频率取决于ADC-D-1的中断频率。
 */
inline void  SetHeatPulse(void)
{
#if HEATPULSE == 1
	TOGGLE_PULSE(); //翻转GPIO电平。WY
#endif
}

/*
 * 功能：产生节拍。
 * 说明：该函数在ADC-D-1的中断ISR中被调用
 */
inline void ADPosCnt(void)
{
	if (++ADBufPos >= ADGROUP_NUM)
	{
		ADBufPos = 0;
	}

	TimeStamp = PhaseLimit(TimeStamp + IFFT_PHASE_STEP);//该函数无意义。WY //ifft用的时间戳产生
}

/*
 * 功能：构造正弦波。WY
 */
inline void VoltSlidPosCnt(void)
{
	if (++VoltPrvPos >= VOLT_FUNDPOINT)
		VoltPrvPos = 0;//电网电压一周波缓冲区输入指针(即经过了滤波器延时及采样延时的)//实时采样的波形(经过滤波之后)
	VoltPos = VoltPrvPos + VoltLeadPoints;//超前校正指针,超前了360*VoltLeadPoints/VOLT_FUNDPOINT度//电压校正

	if (VoltPos >= VOLT_FUNDPOINT)
		VoltPos -= VOLT_FUNDPOINT;//环形缓冲区//VoltPos是提前做好的一个数组点数波形

	CapVoltPos = VoltPos + CAP_VOL_POINT;//电容电压一周波缓冲区指针,滞后270度//
	if (CapVoltPos >= VOLT_FUNDPOINT)
		CapVoltPos -= VOLT_FUNDPOINT;//环形缓冲区

	CapVoltPosCur = VoltPos + CAP_VOL_POINT_CUR;//电容电压一周波缓冲区指针,滞后90度//
	if (CapVoltPosCur >= VOLT_FUNDPOINT)
		CapVoltPosCur -= VOLT_FUNDPOINT;
}

#if PWM_FREQUENCE_16KHZ

/*该函数未定义。WY*/
void PIController(void)
{
}

#endif

#define DEAD_CORR_HYSTERESIS 3

/*该宏定义未使用。WY*/
#define GetVlotFeedback()\
    V_dq2Ualpha = SPLL.GridPLLVoltD  * GridResCos - SPLL.GridPLLVoltQ  * GridResSin;\
    V_dq2Ubeta =  SPLL.GridPLLVoltD  * GridResSin + SPLL.GridPLLVoltQ  * GridResCos;\
    VolforwardA = V_dq2Ualpha*  SQRT_2DIV3;\
    VolforwardB = V_dq2Ualpha*(-SQRT_2DIV3_DIV2) + V_dq2Ubeta *  SQRT2_DIV2;\
    VolforwardC = V_dq2Ualpha*(-SQRT_2DIV3_DIV2) + V_dq2Ubeta *(-SQRT2_DIV2);\
    \
    V_dq2Ualpha = SPLL.GridPLLVoltDn * GridResCos + SPLL.GridPLLVoltQn * GridResSin;\
    V_dq2Ubeta = -SPLL.GridPLLVoltDn * GridResSin + SPLL.GridPLLVoltQn * GridResCos;\
    VolforwardA += V_dq2Ualpha*  SQRT_2DIV3;\
    VolforwardB += V_dq2Ualpha*(-SQRT_2DIV3_DIV2) + V_dq2Ubeta *  SQRT2_DIV2;\
    VolforwardC += V_dq2Ualpha*(-SQRT_2DIV3_DIV2) + V_dq2Ubeta *(-SQRT2_DIV2);

float escdebugpll = 0, escdebugdata;
#define LooktablePower2Amplitude 0.05f

float ReactivePowerComFUN(float Esc_VoltPLL,float Q)  //ESC无功补偿函数
{
    float tmp;
    while(Esc_VoltPLL > PI){
        Esc_VoltPLL -= PI;
    }

    if(Q>0){
        tmp= sinf((2.0f)*Esc_VoltPLL);
    }else{
        tmp= -sinf((2.0f)*Esc_VoltPLL);
    }

    if(ConstantReactivePower<0.001 && ConstantReactivePower>-0.001) //界面设置为0,则开启无功补偿模式后自动补偿.
        return (tmp)*fabsf(Q);
    else    //界面下发值作为补偿
        return (tmp)*ConstantReactivePower*LooktablePower2Amplitude;
}

#ifdef unblance_gravity
#else

/*
 * 在分析三相电流不平衡时，用于标识电流有效值大小排序。WY
 */
enum ChanMaxName
{
	UnCurrCompChanA = 0, //A相
	UnCurrCompChanB = 1, //B相
	UnCurrCompChanC = 2, //C相
};

/*
 * 功能：将三相电网电流有效值按照降序排序。WY
 *
 * 输入参数a：A相电网电流有效值
 * 输入参数b：B相电网电流有效值
 * 输入参数c：C相电网电流有效值
 * 输出参数max：三相电网电流有效值的最大值序号
 * 输出参数min：三相电网电流有效值的最小值序号
 * 输出参数midd：三相电网电流有效值的居中值序号
 */
void ChanMax3(float a, float b, float c, int *max, int *min, int *midd)
{
	if (a > b)
	{
		if (a > c)
		{
			*max = UnCurrCompChanA; //A相电流有效值为三者中的最大值。WY

			if (b < c)
			{
				*min = UnCurrCompChanB; //B相电流有效值为三者中的最小值。WY
				*midd = UnCurrCompChanC; //C相电流有效值为三者中的居中值。WY
			}
			else
			{
				*min = UnCurrCompChanC; //C相电流有效值为三者中的最小值。WY
				*midd = UnCurrCompChanB; //B相电流有效值为三者中的居中值。WY
			}
		}
		else
		{
			*max = UnCurrCompChanC; //C相电流有效值为三者中的最大值。WY
			*min = UnCurrCompChanB; //B相电流有效值为三者中的最小值。WY
			*midd = UnCurrCompChanA; //A相电流有效值为三者中的居中值。WY
		}
	}
	else
	{
		if (a < c)
		{
			*min = UnCurrCompChanA; //A相电流有效值为三者中的最小值。WY

			if (b < c)
			{
				*max = UnCurrCompChanC; //C相电流有效值为三者中的最大值。WY
				*midd = UnCurrCompChanB; //B相电流有效值为三者中的居中值。WY
			}
			else
			{
				*max = UnCurrCompChanB; //B相电流有效值为三者中的最大值。WY
				*midd = UnCurrCompChanC; //C相电流有效值为三者中的居中值。WY
			}
		}
		else
		{
			*min = UnCurrCompChanC; //C相电流有效值为三者中的最小值。WY
			*max = UnCurrCompChanB; //B相电流有效值为三者中的最大值。WY
			*midd = UnCurrCompChanA; //A相电流有效值为三者中的居中值。WY
		}
	}
}

int unbalanceGenFlag = 0; //三相不平衡标志位。0：三相平衡（默认）；1：三相不平衡。WY

int maxCh; //三相电流有效值最大者的序号。WY
int minCh; //三相电流有效值最小者的序号。WY
int middCh; //三相电流有效值居中者的序号。WY

/*
 * 调节三相电流不平衡时的步长（单位：V）。WY
 */
#define UN_CURR_COMP_STEP_VOLT  0.0003f

/*
 * 功能：检测三相电网电流（有效值）不平衡，据此修正三相负载电压（有效值）目标值。WY
 */
void UnCurrCompFUN(void)
{
	float tmp = 0; //临时变量。WY

	float CurMeanValue = (UnCurrData[0] + UnCurrData[1] + UnCurrData[2]) * S1DIV3;//求解三相电网电流（有效值）平均值。WY
	float CurDiffMAXValue = Max3(UnCurrData[0], UnCurrData[1], UnCurrData[2]); //求解三相电网电流（有效值）的最大值。WY
	currentUnbalance = (CurDiffMAXValue - CurMeanValue) / CurMeanValue; //求解三相电网电流（有效值）不平衡度。WY

	if (currentUnbalance > TargetCurrentUnbalance) //三相电网电流（有效值）的不平衡度 > 三相电网电流（有效值）的不平衡度上限。WY
	{
		if (CntMs.UNCurrDelay1 >= CNT_MS(1000)) //三相电网电流（有效值）不平衡持续时间超过1000ms。WY
		{
			unbalanceGenFlag = TRUE; //设备处于三相电网电流（有效值）不平衡状态。WY
		}
	}
	else
	{
		CntMs.UNCurrDelay1 = 0; //清零计时器。WY
	}

	if (currentUnbalance < TargetCurrentUnbalance - 0.01) //三相电网电流（有效值）不平衡度 < (三相电网电流（有效值）不平衡的上限 - 0.01)。目的：设定调节阈值，防止系统抖动。WY
	{
		if (CntMs.UNCurrDelay2 >= CNT_MS(20000)) //持续时间超过20000ms。目的：防止系统抖动。WY
		{
			unbalanceGenFlag = FALSE; //设备处于三相电网电流（有效值）平衡状态。WY
		}
	}
	else
	{
		CntMs.UNCurrDelay2 = 0; //清零计时器。WY
	}

	/*三相电网电流（有效值）不平衡。WY*/
	if (unbalanceGenFlag == TRUE)
	{
		ChanMax3(UnCurrData[0], UnCurrData[1], UnCurrData[2], &maxCh, &minCh, &middCh); //将三相电网电流（有效值）按照降序排序。WY
		CurrentUnbalanceRegularVoltage[maxCh] -= UN_CURR_COMP_STEP_VOLT; //降低电网电流（有效值）最大相对应的负载电压（有效值）目标值。WY
		tmp = CurrentUnbalanceRegularVoltage[maxCh];
		CurrentUnbalanceRegularVoltage[minCh] += UN_CURR_COMP_STEP_VOLT; //提高电网电流（有效值）最小相对应的负载电压（有效值）目标值。WY
		tmp += CurrentUnbalanceRegularVoltage[minCh];
		float sum = ESCFlagA.Volttarget + ESCFlagB.Volttarget + ESCFlagC.Volttarget; //用户设定的负载电压（有效值）目标值的和。WY

		CurrentUnbalanceRegularVoltage[middCh] = sum - tmp; //调节三相电网电流（有效值）居中者对应的电压（有效值）。WY
	};

	/*处理A相：负载电压（有效值）目标值限幅*/
	if (CurrentUnbalanceRegularVoltage[0] > GV_RMS_OVER - 25) //A相负载电压（有效值）目标值 > 上限值。WY
	{
		CurrentUnbalanceRegularVoltage[0] = GV_RMS_OVER - 25; //限幅：限定A相负载电压（有效值）目标值的上限。WY

	}
	if (CurrentUnbalanceRegularVoltage[0] < GV_RMS_UNDER + 40) //A相负载电压（有效值）目标值 < 下限值。WY
	{
		CurrentUnbalanceRegularVoltage[0] = GV_RMS_UNDER + 40; //限幅：限定A相负载电压（有效值）目标值的下限。WY
	}

	/*处理B相：负载电压（有效值）目标值限幅*/
	if (CurrentUnbalanceRegularVoltage[1] > GV_RMS_OVER - 25)
	{
		CurrentUnbalanceRegularVoltage[1] = GV_RMS_OVER - 25;
	}
	if (CurrentUnbalanceRegularVoltage[1] < GV_RMS_UNDER + 40)
	{
		CurrentUnbalanceRegularVoltage[1] = GV_RMS_UNDER + 40;
	}

	/*处理C相：负载电压（有效值）目标值限幅*/
	if (CurrentUnbalanceRegularVoltage[2] > GV_RMS_OVER - 25)
	{
		CurrentUnbalanceRegularVoltage[2] = GV_RMS_OVER - 25;
	}
	if (CurrentUnbalanceRegularVoltage[2] < GV_RMS_UNDER + 40)
	{
		CurrentUnbalanceRegularVoltage[2] = GV_RMS_UNDER + 40;
	}
}

#endif

float ESC_FeedForward_DutyA = 0; //A相PWM占空比前馈值。WY
float ESC_FeedForward_DutyB = 0; //B相PWM占空比前馈值。WY
float ESC_FeedForward_DutyC = 0; //C相PWM占空比前馈值。WY

float PIOutVoltValueA = 0; //A相PI调节器的输出值。WY
float PIOutVoltValueB = 0; //B相PI调节器的输出值。WY
float PIOutVoltValueC = 0; //C相PI调节器的输出值。WY

int ESCLowCurrFlagA = 0,ESCLowCurrFlagB = 0,ESCLowCurrFlagC = 0; //该变量未使用。WY

#define LEAD_SAMPLE_INCREMENT_DUTY  0.0f //该宏定义未使用。WY
#define LEAD_SAMPLE_DECREMENT_DUTY  0.0f //该宏定义未使用。WY

float DBG_leadSampleIncrementDuty = LEAD_SAMPLE_INCREMENT_DUTY; //该变量未使用。WY
float DBG_leadSampleDecrementDuty = LEAD_SAMPLE_DECREMENT_DUTY; //该变量未使用。WY

/*
 * 功能：根据电网电压是否存在突变，求解PWM占空比。WY
 *
 * 输入参数RMSGridInVAL：电网电压（有效值）
 * 输入参数INSGridIn：电网电压（瞬时值）
 * 输入参数target：负载电压（有效值）目标值
 * 输入参数PIerr：PI调节器的输出值
 * 输入参数LockPhAngle：经锁相环得到的相位
 * 输出参数pStorage：PWM占空比预设值。当电网电压未发生突变时，使得PWM占空比预设值缓慢逼近PWM前馈值，实现稳定系统振荡的作用。
 * 输入参数ESCPHASE：相位选择
 *
 * 返回值：PWM占空比
 */
float RMSDutyLimit(float RMSGridInVAL, //电网电压（有效值）
				   float INSGridIn, //电网电压（瞬时值）
				   float target, //负载电压（有效值）目标值
				   float PIerr, //PI调节器的输出值(用于修正PWM占空比的前馈值)
				   float LockPhAngle, //经锁相环得到的相位。当电网电压未发生突变时，使得PWM占空比预设值缓慢逼近PWM前馈值，实现稳定系统振荡的作用。
				   float *pStorage, //PWM占空比预设值。
				   int ESCPHASE) //相位选择
{
	ArithFlagA = 2; //A相电网电压突变标志位。0，存在突变；1，不存在突变。WY
	ArithFlagB = 2; //B相电网电压突变标志位。0，存在突变；1，不存在突变。WY
	ArithFlagC = 2; //C相电网电压突变标志位。0，存在突变；1，不存在突变。WY

	TESTVALUE = 0; //该变量未使用。WY
	TESEPIerr = PIerr; //该变量未使用。WY
	testvalue1 = *pStorage; //该变量未使用。WY
	PhaseValue = LockPhAngle; //该变量未使用。WY
	TESTRMSGridInVAL = RMSGridInVAL; //该变量未使用。WY
	TESETarget = target; //该变量未使用。WY
	TESEINSGridIn = INSGridIn; //该变量未使用。WY

	NumeratorValue = INSGridIn - RMSGridInVAL * 1.4142 * sin(LockPhAngle); //电网电压（瞬时值）的突变值 = 电网电压（瞬时值） - 上个周波电网电压（瞬时值）。WY
	DenominatorVAL = target * 1.4142 * sin(LockPhAngle); //该变量未使用。WY

	if ((RMSGridInVAL > 10) //电网电压（有效值） > 10V。WY
			&& (target > 10)) //负载电压（有效值）目标值 > 10V。
	{
		 /*电网电压不在过零点附近。WY*/
		if (((LockPhAngle >= 0.3f)
				&& (LockPhAngle <= (PIVAL - 0.3f)))
				|| ((LockPhAngle >= (PIVAL + 0.3f))
				&& (LockPhAngle <= (2.0f * PIVAL - 0.3f))))
		{
			/*升压模式。WY*/
			if (StateFlag.VoltageModeFlag == 0)
			{
				/*处理A相*/
				if (ESCPHASE == 1)
				{
					/*电压不存在突变。WY*/
					if ((TestArithFlagA == 1) //解锁状态。WY
							&& (((NumeratorValue < ArithVAL) //电网电压（瞬时值）突变值 < 电网电压（瞬时值）的突变值上限。WY
							&& (NumeratorValue > ArithVal)) //电网电压（瞬时值）突变值 > 电网电压（瞬时值）的突变值下限。WY
							|| (ESCFlagA.ESCCntMs.HarmDisDelay > CNT_MS(20)))) //电网电压（瞬时值）突变计时 > 20ms。WY
					{
						dutytmp = dutytmp1 = RMSGridInVAL / target; //PWM占空比 = 电网电压（有效值） / 负载电压（有效值）目标值。WY
						ArithFlagA = 1; //A相电网电压不存在突变。WY

						if ((NumeratorValue < ArithVAL) //电网电压突变值 < 电网电压突变值上限。WY
								&& (NumeratorValue > ArithVal)) //电网电压突变值 > 电网电压突变值下限。WY
						{
							ESCFlagA.ESCCntMs.HarmDisDelay = 0; //清零电网电压突变持续时间。WY
						}
					}
					else if
					/*电压存在突变。WY*/
					((NumeratorValue >= ArithVAL) //电网电压（瞬时值）突变值 > 电网电压（瞬时值）突变值上限。WY
							|| (NumeratorValue <= ArithVal) //电网电压（瞬时值）突变值 < 电网电压（瞬时值）突变值下限。WY
							|| (TestArithFlagA == 0) //上锁状态。WY
							|| (ESCFlagA.VoltIn_rms >= GV_RMS_OVER)) //电网电压（有效值） > 电网电压（有效值）上限。WY
					{

						if ((ESCFlagA.ESCCntMs.HarmDisDelay <= CNT_MS(20))) //电网电压突变持续时间 < 20ms。WY
						{
							dutytmp = dutytmp1 = (RMSGridInVAL / target)  //电网电压（有效值） / 负载电压（有效值）的目标值。WY
									+ ((INSGridIn - RMSGridInVAL * SQRT2 * sin(LockPhAngle)) / (target * SQRT2 * sin(LockPhAngle))); //电压突变比例 = (电网电压瞬时值 - 上个周波电网电压的瞬时值) / 负载电压瞬时值的目标值。WY
							ArithFlagA = 0; //A相电网电压存在突变。WY
							TestArithFlagA = 0; //上锁。WY
						}
						else //电网电压突变计时 > 20ms。WY
						{
							TestArithFlagA = 1; //解锁。WY
							*pStorage = RMSGridInVAL / target; //PWM占空比 = 当前电网电压(有效值) / 负载电压(有效值)目标值。WY
						}
					}
				}

				/*处理B相*/
				if ((((NumeratorValue < ArithVAL) && (NumeratorValue > ArithVal)) || (ESCFlagB.ESCCntMs.HarmDisDelay > CNT_MS(20))) && (TestArithFlagB == 1)
						&& (ESCPHASE == 2))
				{
					dutytmp = dutytmp1 = RMSGridInVAL / target;
					ArithFlagB = 1;
					if ((NumeratorValue < ArithVAL) && (NumeratorValue > ArithVal))
					{
						ESCFlagB.ESCCntMs.HarmDisDelay = 0;
					}
				}
				else if (((NumeratorValue >= ArithVAL) || (NumeratorValue <= ArithVal) || (TestArithFlagB == 0) || (ESCFlagB.VoltIn_rms >= GV_RMS_OVER))
						&& (ESCPHASE == 2))
				{
					if ((ESCFlagB.ESCCntMs.HarmDisDelay <= CNT_MS(20)))
					{
						dutytmp = dutytmp1 = (RMSGridInVAL / target)
								+ ((INSGridIn - RMSGridInVAL * 1.4142 * sin(LockPhAngle)) / (target * 1.4142 * sin(LockPhAngle)));
						ArithFlagB = 0;
						TestArithFlagB = 0;
					}
					if (ESCFlagB.ESCCntMs.HarmDisDelay > CNT_MS(20))
					{
						TestArithFlagB = 1;
						*pStorage = RMSGridInVAL / target;
					}
				}

				/*处理C相*/
				if ((((NumeratorValue < ArithVAL) && (NumeratorValue > ArithVal)) || (ESCFlagC.ESCCntMs.HarmDisDelay > CNT_MS(20))) && (TestArithFlagC == 1)
						&& (ESCPHASE == 3))
				{
					dutytmp = dutytmp1 = RMSGridInVAL / target;
					ArithFlagC = 1;
					if ((NumeratorValue < ArithVAL) && (NumeratorValue > ArithVal))
					{
						ESCFlagC.ESCCntMs.HarmDisDelay = 0;
					}
				}
				else if (((NumeratorValue >= ArithVAL) || (NumeratorValue <= ArithVal) || (TestArithFlagC == 0) || (ESCFlagC.VoltIn_rms >= GV_RMS_OVER))
						&& (ESCPHASE == 3))
				{
					if ((ESCFlagC.ESCCntMs.HarmDisDelay <= CNT_MS(20)))
					{
						dutytmp = dutytmp1 = (RMSGridInVAL / target)
								+ ((INSGridIn - RMSGridInVAL * 1.4142 * sin(LockPhAngle)) / (target * 1.4142 * sin(LockPhAngle)));
						ArithFlagC = 0;
						TestArithFlagC = 0;
					}
					if (ESCFlagC.ESCCntMs.HarmDisDelay > CNT_MS(20))
					{
						TestArithFlagC = 1;
						*pStorage = RMSGridInVAL / target;
					}
				}
			}
			/*降压模式。WY*/
			else if (StateFlag.VoltageModeFlag == 1)
			{
				dutytmp = dutytmp1 = target / RMSGridInVAL;
				if (ESCPHASE == 1)
				{
					ArithFlagA = 1;
				}
				if (ESCPHASE == 2)
				{
					ArithFlagB = 1;
				}
				if (ESCPHASE == 3)
				{
					ArithFlagC = 1;
				}
			}
		}

		/*限定PWM占空比上限*/
		if (dutytmp1 > 0.96)
		{
			dutytmp1 = 0.96;
		}

		/*限定PWM占空比下限*/
		if (dutytmp1 < 0.51)
		{
			dutytmp1 = 0.51;
		}
	}
	else //(电网电压有效值 < 10V)或(负载电压有效值的目标值 < 10V)。WY
	{
		dutytmp1 = 0.96;
	}

//幅值变化斜率控制,每秒变化率390%:=0.00001*25.6k

	/*处理A相*/
	if ((ArithFlagA == 1) //A相电网电压不存在突变。WY
			&& (ESCPHASE == 1)) //A相。WY
	{
		if ((dutytmp1 > 0.499) && (dutytmp1 < 0.961)) //PWM占空比的前馈值在范围内。WY
		{
			if (*pStorage > dutytmp1) //PWM占空比的预设值 > PWM占空比的前馈值。WY
			{
				*pStorage -= 0.0001; //微调PWM占空比的预设值，使其缓慢逼近PWM占空比的前馈值。WY
			}
			else if (*pStorage < dutytmp1) //PWM占空比的前馈值 < PWM占空比。WY
			{
				*pStorage += 0.0001; //微调PWM占空比的预设值，使其缓慢逼近PWM占空比的前馈值。WY
			}
			else //该条件恒不成立。原因：无法将两个浮点数相比较。WY
			{
				*pStorage = dutytmp1; //无需微调微调PWM占空比的预设值。WY
			}
		}
		else if (dutytmp1 > 0.962) //PWM占空比的前馈值。注意：该条件恒不成立。WY
		{
			*pStorage = dutytmp1 = 0.962; //限定PWM占空比的前馈值的上限。WY
		}
		else if (dutytmp1 < 0.497) //PWM占空比的前馈值。注意：该条件恒不成立。WY
		{
			*pStorage = dutytmp1 = 0.497; //限定PWM占空比的前馈值的下限。WY
		}

		/*升压模式。WY*/
		if (StateFlag.VoltageModeFlag == 0)
		{
			dutytmp1 = *pStorage - PIerr; //PWM占空比的预设值 - PI调节器的输出值。注意：经过PI调节器的修正后，得出PWM占空比的最终值。WY
		}
		/*降压模式。WY*/
		else if (StateFlag.VoltageModeFlag == 1)
		{
			dutytmp1 = *pStorage + PIerr;
		}
	}

	/*处理B相*/
	if ((ArithFlagB == 1) && (ESCPHASE == 2))
	{
		if ((dutytmp1 > 0.499) && (dutytmp1 < 0.961))
		{
			if (*pStorage > dutytmp1)
			{
				*pStorage -= 0.0001;
			}
			else if (*pStorage < dutytmp1)
			{
				*pStorage += 0.0001;
			}
			else
			{
				*pStorage = dutytmp1;
			}
		}
		else if (dutytmp1 > 0.962)
		{
			*pStorage = dutytmp1 = 0.962;
		}
		else if (dutytmp1 < 0.497)
		{
			*pStorage = dutytmp1 = 0.497;
		}

		if (StateFlag.VoltageModeFlag == 0)
		{
			dutytmp1 = *pStorage - PIerr;//*pStorage前馈值，PIerr反馈值
		}
		else if (StateFlag.VoltageModeFlag == 1)
		{
			dutytmp1 = *pStorage + PIerr;
		}
	}

	/*处理C相*/
	if ((ArithFlagC == 1) && (ESCPHASE == 3))
	{

		if ((dutytmp1 > 0.499) && (dutytmp1 < 0.961))
		{
			if (*pStorage > dutytmp1)
			{
				*pStorage -= 0.0001;
			}
			else if (*pStorage < dutytmp1)
			{
				*pStorage += 0.0001;
			}
			else
			{
				*pStorage = dutytmp1;
			}
		}
		else if (dutytmp1 > 0.962)
		{
			*pStorage = dutytmp1 = 0.962;
		}
		else if (dutytmp1 < 0.497)
		{
			*pStorage = dutytmp1 = 0.497;
		}

		if (StateFlag.VoltageModeFlag == 0)
		{
			dutytmp1 = *pStorage - PIerr;
		}
		else if (StateFlag.VoltageModeFlag == 1)
		{
			dutytmp1 = *pStorage + PIerr;
		}
	}

	/*限定PWM占空比上限。WY*/
	if (dutytmp1 > 0.96)
	{
		dutytmp1 = 0.96;
	}

	/*限定PWM占空比下限。WY*/
	if (dutytmp1 < 0.51)
	{
		dutytmp1 = 0.51;
	}

	return dutytmp1;
}


float dbg_reactive;
int WUGONGlinshitest = 0,BUPINGHENGlinshitest = 0;
int T1PR_INT16 = 0;
float TIPRTEST = 0.805;
float VoltInF = 0,CurrInF = 0;
float DEBUGESC_DutyDataA = 0,DEBUGESC_DutyDataB = 0,DEBUGESC_DutyDataC = 0,DEBUGESC_DutyDataD = 0;

int UnCurrCompCnt = 0; //该变量未使用。WY

int cntaaa=2;

/*
 * 不同系列机型的容量参数。WY
 */
const SMconstantCurr ConstantCurrDefault[3] =
{
	{
	  VOLT_TARGET_CORR_DEFAULT, //PI调节器参数。WY
	  0.0f, //负载电压有效值的目标值的修正系数。WY
	  58.0f, //电网电流有效值的过载值。WY
	  13.0f, //该成员未使用。WY
	  45.0f, //电网电流有效值的额定值。WY
	  82.0f, // 电网电流瞬时值的限定值。WY
	  0, //状态机状态（用于描述电网电流状态）。WY
	  0, //前一次电网电流状态。WY
	  0 //计数器。目的：防止状态机频繁切换引发抖动。WY
	},

	{
	  VOLT_TARGET_CORR_DEFAULT,
	  0.0f,
	  93.0f,
	  18.0f,
	  75.0f,
	  135.0f,
	  0,
	  0,
	  0
	},

	{
	  VOLT_TARGET_CORR_DEFAULT,
	  0.0f,
	  140.0f,
	  23.0f,
	  114.0f,
	  198.0f,
	  0,
	  0,
	  0
	}
};

/*
 * ABC三相PI控制器参数。WY
 *
 * 该参数用于如下场景：
   根据电网电流状态，计算负载电压（有效值）目标值的修正系数。
 *
 * 注意：该参数可能会被其他函数重新赋值。
 */
SMconstantCurr ConstantCurr[3] =
{
 	 /*A相参数*/
	{
	  VOLT_TARGET_CORR_DEFAULT,
	  0.0f, //负载电压有效值的目标值的修正系数（范围：-0.6 ~ 0（由仿真器观测得出））。WY
	  58.0f, //电网电流（有效值）过载值。WY
	  13.0f, //该成员未使用。WY
	  45.0f, //电网电流（有效值）额定值。WY
	  82.0f, // 电网电流（瞬时值）额定值。WY
	  0, //状态机状态（用于描述电网电流）。WY
	  0, //前一次状态机状态。WY
	  0 //计数器。目的：防止状态机频繁切换引发抖动。WY
	},

	/*B相参数*/
	{
	  VOLT_TARGET_CORR_DEFAULT,
	  0.0f,
	  58.0f,
	  13.0f,
	  45.0f,
	  82.0f,
	  0,
	  0,
	  0
	},

	/*C相参数*/
	{
	  VOLT_TARGET_CORR_DEFAULT,
	  0.0f,
	  58.0f,
	  13.0f,
	  45.0f,
	  82.0f,
	  0,
	  0,
	  0
	}
};


/*
 * 功能：根据电网电流状态，计算负载电压（有效值）目标值的修正系数。WY
 * 说明：根据电网电流的大小划分3种状态，分别为：
         1. 电网电流正常模式；
         2. 电网电流恒定模式；
         3. 电网电流突变模式。
 *
 * 输入参数SMconstantCurr：设备容量相关参数
 * 输入参数gridCur_rms：电网电流（有效值）
 * 输入参数GridCurrF：电网电流（瞬时值）
 * 输入参数CurrTargetValue：不同环境温度下对应的电网电流（有效值）目标值
 *
 * 返回值：负载电压（有效值）目标值的修正系数
 */
float ConstantCurrFSM(SMconstantCurr *pst, //设备容量相关参数。WY
					  float gridCur_rms, //电网电流（有效值）。WY
					  float GridCurrF, //电网电流（瞬时值）。WY
					  float CurrTargetValue //不同环境温度下对应的电网电流（有效值）目标值。WY
					  )
{
	/*状态机跳转逻辑。WY*/
	switch (pst->state)
	{
		 /*电网电流正常模式。WY*/
		case SM_CONSTANT_CURR_NORMAL:
		{
			if ((gridCur_rms >= pst->RMS_CONSTANT_CURRENT_OVERLOAD) //电网电流（有效值 ）> 电网电流（有效值）过载值。WY
					|| (GridCurrF > pst->INS_CONSTANT_CURRENT_RATED) //电网电流（瞬时值） > 电网电流（瞬时值）额定值上限。WY
					|| (GridCurrF < -pst->INS_CONSTANT_CURRENT_RATED)) //电网电流（瞬时值） < 电网电流（瞬时值）额定值下限。WY
			{
				pst->state = SM_CONSTANT_CURR_INS; //切换状态机至：电网电流瞬时模式。WY
			}
			else
			{
				if ((gridCur_rms > pst->RMS_CONSTANT_CURRENT_RATED) //电网电流（有效值 ） > 电网电流（有效值）额定值。WY
						&& (gridCur_rms < pst->RMS_CONSTANT_CURRENT_OVERLOAD)) //电网电流（有效值） < 电网电流（有效值）过载值。WY
				{
					pst->state = SM_CONSTANT_CURR_CONSTANT; //切换状态机至：电网电流恒定模式。WY
				}
				else if (gridCur_rms <= pst->RMS_CONSTANT_CURRENT_RATED) //电网电流（有效值） < 电网电流（有效值）额定值。WY
				{
					pst->state = SM_CONSTANT_CURR_NORMAL; //切换状态机至：电网电流正常模式。WY
				}
			}
		}
			break;

		/*电网电流恒定模式。WY*/
		case SM_CONSTANT_CURR_CONSTANT:
		{
			if ((gridCur_rms >= pst->RMS_CONSTANT_CURRENT_OVERLOAD) //电网电流（有效值 ）> 电网电流（有效值）过载值。WY
					|| (GridCurrF > pst->INS_CONSTANT_CURRENT_RATED) //电网电流（瞬时值） > 电网电流（瞬时值）额定值上限。WY
					|| (GridCurrF < -pst->INS_CONSTANT_CURRENT_RATED)) //电网电流（瞬时值） < 电网电流（瞬时值）额定值下限。WY
			{
				pst->state = SM_CONSTANT_CURR_INS; //切换状态机至：电网电流瞬时模式。WY
			}
			else
			{
				if ((gridCur_rms > pst->RMS_CONSTANT_CURRENT_RATED) //电网电流（有效值 ） > 电网电流（有效值）额定值。WY
						&& (gridCur_rms < pst->RMS_CONSTANT_CURRENT_OVERLOAD)) //电网电流（有效值） < 电网电流（有效值）过载值。WY
				{
					pst->state = SM_CONSTANT_CURR_CONSTANT; //切换状态机至：电网电流恒定模式。WY
				}
				else if (gridCur_rms <= pst->RMS_CONSTANT_CURRENT_RATED) //电网电流（有效值） < 电网电流（有效值）额定值。WY
				{
					pst->state = SM_CONSTANT_CURR_NORMAL; //切换状态机至：电网电流正常模式。WY
				}
			}
		}
			break;

		/*电网电流瞬时模式：WY*/
		case SM_CONSTANT_CURR_INS:
		{
			if ((gridCur_rms >= pst->RMS_CONSTANT_CURRENT_OVERLOAD) //电网电流（有效值） > 电网电流（有效值）过载值。WY
					|| (GridCurrF > pst->INS_CONSTANT_CURRENT_RATED) //电网电流（瞬时值） > 电网电流（瞬时值）上限。WY
					|| (GridCurrF < -pst->INS_CONSTANT_CURRENT_RATED)) //电网电流（瞬时值） < 电网电流（瞬时值）下限。WY
			{
				pst->state = SM_CONSTANT_CURR_INS; //切换状态机至：电网电流瞬时模式。WY
			}
			else
			{
				if ((gridCur_rms > pst->RMS_CONSTANT_CURRENT_RATED) //电网电流（有效值 > 电网电流（有效值）额定值。WY
						&& (gridCur_rms < pst->RMS_CONSTANT_CURRENT_OVERLOAD)) //电网电流（有效值 ）< 电网电流（有效值）过载值。WY
				{
					pst->state = SM_CONSTANT_CURR_CONSTANT; //切换状态机至：电网电流恒定模式。WY
				}
				else if (gridCur_rms <= pst->RMS_CONSTANT_CURRENT_RATED) //电网电流（有效值） < 电网电流（有效值）额定值。WY
				{
					pst->state = SM_CONSTANT_CURR_NORMAL; //切换状态机至：电网电流正常模式。WY
				}
			}
		}
			break;

		case SM_CONSTANT_CURR_STANDBY: //默认设置。WY
		default:
		{
			pst->Prvstate = pst->state = SM_CONSTANT_CURR_NORMAL; //切换状态机至：电网电流正常。WY
			pst->CorrPI.i10 = 0;
		}
			break;
	}

	/*状态机的行为。WY*/
	switch (pst->state)
	{
		/*电网电流正常模式。WY*/
		case SM_CONSTANT_CURR_NORMAL:
		{
			if (pst->VolttargetCorr < 0)
			{
				pst->VolttargetCorr += 0.00005; //递增负载电压（有效值）目标值修正系数。WY
			}

			/*这段代码疑似无实际作用。WY*/
			{
			if (pst->Prvstate == SM_CONSTANT_CURR_CONSTANT) //上一次状态机事件为：电网电流恒定模式。WY
			{
				pst->CNT1 = 10000; //赋值周期计数器，防止抖动？该语句疑似无实际作用。WYWY
			}
			if (pst->CNT1 >= 0)
			{
				if (--pst->CNT1 == 0)
				{
					pst->CorrPI.i10 = 0; //该语句疑似无实际作用。WY
				}
			}
			}

			pst->Prvstate = pst->state; //记录状态机状态。WY
		}
		break;

		/*电网电流恒定模式。WY*/
		case SM_CONSTANT_CURR_CONSTANT:
		{
			if (pst->Prvstate == SM_CONSTANT_CURR_INS) //上一次状态机事件为：电网电流瞬时模式。WY
			{
				pst->CorrPI.i10 = pst->VolttargetCorr;
			}

			if (pst->Prvstate == SM_CONSTANT_CURR_NORMAL) //上一次状态机事件为：电网电流正常模式。WY
			{
				pst->CorrPI.i10 = pst->VolttargetCorr;
			}

			pst->VolttargetCorr = DCL_runPI(&(pst->CorrPI), CurrTargetValue, gridCur_rms); //计算负载电压（有效值）目标值的修正系数。WY

			pst->Prvstate = pst->state; //记录状态机状态。WY
		}
			break;

		/*电网电流瞬时模式*/
		case SM_CONSTANT_CURR_INS:
		{
			pst->VolttargetCorr = -0.5; //不升压。

			pst->Prvstate = pst->state; //记录状态机状态。WY
		}
			break;

		/*电网电流初始*/
		case SM_CONSTANT_CURR_STANDBY:
		default:
		{
			pst->CorrPI.i10 = 0; //清零PI控制器的积分项。WY
			break;
		}
	}

	return pst->VolttargetCorr; //返回负载电压（有效值）目标值的修正系数。WY
}


/*
 * 功能：PWM占空比调制。WY
 * 注意：该函数在ADC-D-1的ISR中被调用。
 */
inline void GenModulation(void)
{
	DBG_leadSampleIncrementDuty = harmCompPerc; //该变量未使用。WY
	DBG_leadSampleDecrementDuty = -reactPrCompPerc; //该变量未使用。WY
	T1PR_INT16 = (int) T1PR; //该变量未使用。WY

#if ESC_THREEPHASE
	if ((StateFlag.VoltageMode == 3) //不平衡模式。WY
			&& (StateEventFlag_A == STATE_EVENT_RUN_A) //A相处于运行状态。WY
			&& (ESCFlagA.PWM_ins_index == 0) //A相主路处于PWM调制状态。WY
			&& (StateEventFlag_B == STATE_EVENT_RUN_B) //B相处于运行状态。WY
			&& (ESCFlagB.PWM_ins_index == 0) //B相主路处于PWM调制状态。WY
			&& (StateEventFlag_C == STATE_EVENT_RUN_C) //C相处于运行状态。WY
			&& (ESCFlagC.PWM_ins_index == 0)) //C相主路处于PWM调制状态。WY
	{
		UnCurrCompCnt = 0; //该变量未使用。WY
		UnCurrCompFUN(); //检测三相电网电流（有效值）不平衡，据此修正三相负载电压（有效值）目标值。WY
	}
	else
	{
		CurrentUnbalanceRegularVoltage[0] = ESCFlagA.Volttarget; //A相负载电压有效值的目标值。WY
		CurrentUnbalanceRegularVoltage[1] = ESCFlagB.Volttarget; //B相负载电压有效值的目标值。WY
		CurrentUnbalanceRegularVoltage[2] = ESCFlagC.Volttarget; //C相负载电压有效值的目标值。WY
	}

	/*处理A相*/
	if (ESCFlagA.PWM_ins_index != 0) //A相主路处于PWM直通状态。WY
	{
		EPwm3Regs.CMPA.bit.CMPA = T1PR;//1管直通
		EPwm3Regs.CMPB.bit.CMPB = T1PR;//2管不直通
		EPwm4Regs.CMPA.bit.CMPA = T1PR;//3管直通
		EPwm4Regs.CMPB.bit.CMPB = T1PR;//4管不直通

		PIVolA.i10 = 0;//清除PI的饱和值
		PIVolA.i6 = 0;

		GridCurrPICtrlA.i10 = 0;
		GridCurrPICtrlA.i6 = 0;

		ConstantCurr[0].state = SM_CONSTANT_CURR_STANDBY; //初始化状态机。WY

		ESC_FeedForward_DutyA = 1; //A相PWM占空比前馈值。WY
	}
	else if ((StateEventFlag_A == STATE_EVENT_RUN_A) //A相处于运行状态。WY
			&& (ESCFlagA.PWM_ins_index == 0)) //A相主路处于PWM调制状态。WY
	{
		switch (StateFlag.VoltageMode) //选择工作模式。WY
		{
			case 0: //升压模式。WY
			{
				VolttargetCorrA = ConstantCurrFSM(&ConstantCurr[0], gridCurA_rms, GridCurrAF, CurrTargetTemper); //根据电网电流状态，计算负载电压（有效值）目标值的修正系数。WY
				PIOutVoltValueA = DCL_runPI(&PIVolA, (CurrentUnbalanceRegularVoltage[0] * (1 + VolttargetCorrA)), VoltOutA_rms); //求解PI控制器的输出值。WY
				ESCFlagA.ESC_DutyData = RMSDutyLimit(VoltInA_rms, VoltInAF, (CurrentUnbalanceRegularVoltage[0] * (1 + VolttargetCorrA)), PIOutVoltValueA,
														Esc_VoltPhaseA, &ESC_FeedForward_DutyA, ESCFlagA.PHASE);
			}
				break;

			case 1: //降压模式。WY
			{
				PIOutVoltValueA = DCL_runPI(&PIVolA, ESCFlagA.Volttarget, VoltOutA_rms);
				ESCFlagA.ESC_DutyData = RMSDutyLimit(VoltInA_rms, VoltInAF, ESCFlagA.Volttarget, PIOutVoltValueA, Esc_VoltPhaseA, &ESC_FeedForward_DutyA,
														ESCFlagA.PHASE);
			}
				break;

			case 2: //升降压模式。WY
				break;

			case 3: //不平衡模式。WY
			{
				VolttargetCorrA = ConstantCurrFSM(&ConstantCurr[0], gridCurA_rms, GridCurrAF, CurrTargetTemper);
				PIOutVoltValueA = DCL_runPI(&PIVolA, CurrentUnbalanceRegularVoltage[0] * (1 + VolttargetCorrA), VoltOutA_rms);
				ESCFlagA.ESC_DutyData = RMSDutyLimit(VoltInA_rms, VoltInAF, CurrentUnbalanceRegularVoltage[0], PIOutVoltValueA, Esc_VoltPhaseA,
														&ESC_FeedForward_DutyA, ESCFlagA.PHASE);
			}
				break;

			case 4: //无功补偿模式。WY
			{
				PIOutVoltValueA = DCL_runPI(&PIVolA, ESCFlagA.Volttarget, VoltOutA_rms);
				dbg_reactive = ReactivePowerComFUN(Esc_VoltPhaseA, GridRealCurErrA);
				ESCFlagA.ESC_DutyData = RMSDutyLimit(VoltInA_rms, VoltInAF, ESCFlagA.Volttarget, PIOutVoltValueA, Esc_VoltPhaseA, &ESC_FeedForward_DutyA,
														ESCFlagA.PHASE) - dbg_reactive;
			}
				break;
		}

		T1PRmulESC_DutyDataAaddnegCurCompPerc = ((int32) (T1PR * ((ESCFlagA.ESC_DutyData + (negCurCompPerc / 2)) > 1.0f ?
				1.0f : ESCFlagA.ESC_DutyData + (negCurCompPerc / 2))));

		T1PRmulESC_DutyDataAsubnegCurCompPerc = ((int32) (T1PR * ((ESCFlagA.ESC_DutyData - (negCurCompPerc / 2)) > 1.0f ?
				1.0f : ESCFlagA.ESC_DutyData - (negCurCompPerc / 2))));

		T1PRmulESC_DutyDataA = ((int32) (T1PR * ESCFlagA.ESC_DutyData));

		if (VoltInAF >= 35)
		{//电压正半波   UAIP+/UAIN+为1时,无互锁逻辑  //硬件新加互锁逻辑判断的电压值与软件采样的电压值在相位上相差约200us,所以这个范围值要设大一些,把硬件判断电压值包进去.
			EPwm3Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataA;//2
			EPwm3Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataA;//1,buck管
			EPwm4Regs.CMPB.bit.CMPB = 0;//4,常通,续流管
			EPwm4Regs.CMPA.bit.CMPA = T1PR;//3,常通,续流管       //3,常关
		}
		else if (VoltInAF <= (-35))
		{//电压负半波
			EPwm3Regs.CMPB.bit.CMPB = 0;//2,常通,续流管
			EPwm3Regs.CMPA.bit.CMPA = T1PR;//1,常通续流管        //1,低
			EPwm4Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataA;//4
			EPwm4Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataA;//3,buck管
		}
		else
		{
			if ((VoltInAF >= 0) && (VoltInAF < 35))
			{//NESPWS20-220524REV2功率板--NPC3CA10020220518REV1合成板
				if (GridCurrAF >= 0)
				{
					EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//1管
					EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//2管
					EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//3管
					EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//4管
				}
				else
				{
					EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//1管
					EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//2管
					EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//3管
					EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//4管
				}
			}
			else if ((VoltInAF < 0) && (VoltInAF > (-35)))
			{
				if (GridCurrAF >= 0)
				{
					EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//1管
					EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//2管
					EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//3管
					EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//4管
				}
				else
				{
					EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//1管
					EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//2管
					EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//3管
					EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//4管
				}
			}
		}
	}
	else //A相处于非运行状态。WY
	{
		PIVolA.i10 = 0;//清除PI的饱和值
		PIVolA.i6 = 0;
		GridCurrPICtrlA.i10 = 0;
		GridCurrPICtrlA.i6 = 0;
		ConstantCurr[0].state = SM_CONSTANT_CURR_STANDBY;
		EPwm3Regs.CMPA.bit.CMPA = T1PR;//1管  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
		EPwm3Regs.CMPB.bit.CMPB = T1PR;//2管
		EPwm4Regs.CMPA.bit.CMPA = T1PR;//3管
		EPwm4Regs.CMPB.bit.CMPB = T1PR;//4管
		ESC_FeedForward_DutyA = 1;//运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
	}

	/*处理B相*/
	if (ESCFlagB.PWM_ins_index != 0)
	{
		EPwm5Regs.CMPA.bit.CMPA = T1PR;
		EPwm5Regs.CMPB.bit.CMPB = T1PR;
		EPwm6Regs.CMPA.bit.CMPA = T1PR;
		EPwm6Regs.CMPB.bit.CMPB = T1PR;
		PIVolB.i10 = 0;
		PIVolB.i6 = 0;
		GridCurrPICtrlB.i10 = 0;
		GridCurrPICtrlB.i6 = 0;
		ConstantCurr[1].state = SM_CONSTANT_CURR_STANDBY;
		ESC_FeedForward_DutyB = 1;

	}
	else if ((StateEventFlag_B == STATE_EVENT_RUN_B) && (ESCFlagB.PWM_ins_index == 0))
	{
		switch (StateFlag.VoltageMode)
		{
			case 0:
				VolttargetCorrB = ConstantCurrFSM(&ConstantCurr[1], gridCurB_rms, GridCurrBF, CurrTargetTemper);
				PIOutVoltValueB = DCL_runPI(&PIVolB, (CurrentUnbalanceRegularVoltage[1] * (1 + VolttargetCorrB)), VoltOutB_rms);
				ESCFlagB.ESC_DutyData = RMSDutyLimit(VoltInB_rms, VoltInBF, (CurrentUnbalanceRegularVoltage[1] * (1 + VolttargetCorrB)), PIOutVoltValueB,
														Esc_VoltPhaseB, &ESC_FeedForward_DutyB, ESCFlagB.PHASE);
				break;
			case 1:
				PIOutVoltValueB = DCL_runPI(&PIVolB, ESCFlagB.Volttarget, VoltOutB_rms);
				ESCFlagB.ESC_DutyData = RMSDutyLimit(VoltInB_rms, VoltInBF, ESCFlagB.Volttarget, PIOutVoltValueB, Esc_VoltPhaseB, &ESC_FeedForward_DutyB,
														ESCFlagB.PHASE);
				break;
			case 2:
				break;
			case 3:
				VolttargetCorrB = ConstantCurrFSM(&ConstantCurr[1], gridCurB_rms, GridCurrBF, CurrTargetTemper);
				PIOutVoltValueB = DCL_runPI(&PIVolB, CurrentUnbalanceRegularVoltage[1] * (1 + VolttargetCorrB), VoltOutB_rms);
				ESCFlagB.ESC_DutyData = RMSDutyLimit(VoltInB_rms, VoltInBF, CurrentUnbalanceRegularVoltage[1], PIOutVoltValueB, Esc_VoltPhaseB,
														&ESC_FeedForward_DutyB, ESCFlagB.PHASE);
				break;
			case 4:
				PIOutVoltValueB = DCL_runPI(&PIVolB, ESCFlagB.Volttarget, VoltOutB_rms);
				ESCFlagB.ESC_DutyData = RMSDutyLimit(VoltInB_rms, VoltInBF, ESCFlagB.Volttarget, PIOutVoltValueB, Esc_VoltPhaseB, &ESC_FeedForward_DutyB,
														ESCFlagB.PHASE) -\
 ReactivePowerComFUN(Esc_VoltPhaseB, GridRealCurErrB);
				break;
		}

		T1PRmulESC_DutyDataBaddnegCurCompPerc = ((int32) (T1PR * ((ESCFlagB.ESC_DutyData + (negCurCompPerc / 2)) > 1.0f ?
				1.0f : ESCFlagB.ESC_DutyData + (negCurCompPerc / 2))));
		T1PRmulESC_DutyDataBsubnegCurCompPerc = ((int32) (T1PR * ((ESCFlagB.ESC_DutyData - (negCurCompPerc / 2)) > 1.0f ?
				1.0f : ESCFlagB.ESC_DutyData - (negCurCompPerc / 2))));
		T1PRmulESC_DutyDataB = ((int32) (T1PR * ESCFlagB.ESC_DutyData));
		if (VoltInBF >= 35)
		{
			EPwm5Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataB;
			EPwm5Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataB;
			EPwm6Regs.CMPB.bit.CMPB = 0;
			EPwm6Regs.CMPA.bit.CMPA = T1PR;
		}
		else if (VoltInBF <= (-35))
		{
			EPwm5Regs.CMPB.bit.CMPB = 0;
			EPwm5Regs.CMPA.bit.CMPA = T1PR;
			EPwm6Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataB;
			EPwm6Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataB;
		}
		else
		{
			if ((VoltInBF >= 0) && (VoltInBF < 35))
			{
				if (GridCurrBF >= 0)
				{
					EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc);
					EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc);
					EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc);
					EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc);
				}
				else
				{
					EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc);
					EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc);
					EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc);
					EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc);
				}
			}
			else if ((VoltInBF < 0) && (VoltInBF > (-35)))
			{
				if (GridCurrBF >= 0)
				{
					EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc);
					EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc);
					EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc);
					EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc);
				}
				else
				{
					EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc);
					EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc);
					EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc);
					EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc);
				}
			}
		}
	}
	else
	{
		PIVolB.i10 = 0;
		PIVolB.i6 = 0;
		GridCurrPICtrlB.i10 = 0;
		GridCurrPICtrlB.i6 = 0;
		ConstantCurr[1].state = SM_CONSTANT_CURR_STANDBY;
		EPwm5Regs.CMPA.bit.CMPA = T1PR;
		EPwm5Regs.CMPB.bit.CMPB = T1PR;
		EPwm6Regs.CMPA.bit.CMPA = T1PR;
		EPwm6Regs.CMPB.bit.CMPB = T1PR;
		ESC_FeedForward_DutyB = 1;
	}

	/*处理C相*/
	if (ESCFlagC.PWM_ins_index != 0)
	{
		EPwm7Regs.CMPA.bit.CMPA = T1PR;
		EPwm7Regs.CMPB.bit.CMPB = T1PR;
		EPwm8Regs.CMPA.bit.CMPA = T1PR;
		EPwm8Regs.CMPB.bit.CMPB = T1PR;
		PIVolC.i10 = 0;
		PIVolC.i6 = 0;
		GridCurrPICtrlC.i10 = 0;
		GridCurrPICtrlC.i6 = 0;
		ConstantCurr[2].state = SM_CONSTANT_CURR_STANDBY;
		ESC_FeedForward_DutyC = 1;
	}
	else if ((StateEventFlag_C == STATE_EVENT_RUN_C) && (ESCFlagC.PWM_ins_index == 0))
	{
		switch (StateFlag.VoltageMode)
		{
			case 0:
				VolttargetCorrC = ConstantCurrFSM(&ConstantCurr[2], gridCurC_rms, GridCurrCF, CurrTargetTemper);
				PIOutVoltValueC = DCL_runPI(&PIVolC, (CurrentUnbalanceRegularVoltage[2] * (1 + VolttargetCorrC)), VoltOutC_rms);
				ESCFlagC.ESC_DutyData = RMSDutyLimit(VoltInC_rms, VoltInCF, (CurrentUnbalanceRegularVoltage[2] * (1 + VolttargetCorrC)), PIOutVoltValueC,
														Esc_VoltPhaseC, &ESC_FeedForward_DutyC, ESCFlagC.PHASE);
				break;
			case 1:
				PIOutVoltValueC = DCL_runPI(&PIVolC, ESCFlagC.Volttarget, VoltOutC_rms);
				ESCFlagC.ESC_DutyData = RMSDutyLimit(VoltInC_rms, VoltInCF, ESCFlagC.Volttarget, PIOutVoltValueC, Esc_VoltPhaseC, &ESC_FeedForward_DutyC,
														ESCFlagC.PHASE);
				break;
			case 2:
				break;
			case 3:
				VolttargetCorrC = ConstantCurrFSM(&ConstantCurr[2], gridCurC_rms, GridCurrCF, CurrTargetTemper);
				PIOutVoltValueC = DCL_runPI(&PIVolC, CurrentUnbalanceRegularVoltage[2] * (1 + VolttargetCorrC), VoltOutC_rms);
				ESCFlagC.ESC_DutyData = RMSDutyLimit(VoltInC_rms, VoltInCF, CurrentUnbalanceRegularVoltage[2], PIOutVoltValueC, Esc_VoltPhaseC,
														&ESC_FeedForward_DutyC, ESCFlagC.PHASE);
				break;
			case 4:
				PIOutVoltValueC = DCL_runPI(&PIVolC, ESCFlagC.Volttarget, VoltOutC_rms);
				ESCFlagC.ESC_DutyData = RMSDutyLimit(VoltInC_rms, VoltInCF, ESCFlagC.Volttarget, PIOutVoltValueC, Esc_VoltPhaseC, &ESC_FeedForward_DutyC,
														ESCFlagC.PHASE) -\
 ReactivePowerComFUN(Esc_VoltPhaseC, GridRealCurErrC);
				break;
		}

		T1PRmulESC_DutyDataCaddnegCurCompPerc = ((int32) (T1PR * ((ESCFlagC.ESC_DutyData + (negCurCompPerc / 2)) > 1.0f ?
				1.0f : ESCFlagC.ESC_DutyData + (negCurCompPerc / 2))));
		T1PRmulESC_DutyDataCsubnegCurCompPerc = ((int32) (T1PR * ((ESCFlagC.ESC_DutyData - (negCurCompPerc / 2)) > 1.0f ?
				1.0f : ESCFlagC.ESC_DutyData - (negCurCompPerc / 2))));
		T1PRmulESC_DutyDataC = ((int32) (T1PR * ESCFlagC.ESC_DutyData));
		if (VoltInCF >= 35)
		{
			EPwm7Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataC;
			EPwm7Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataC;
			EPwm8Regs.CMPB.bit.CMPB = 0;
			EPwm8Regs.CMPA.bit.CMPA = T1PR;
		}
		else if (VoltInCF <= (-35))
		{
			EPwm7Regs.CMPB.bit.CMPB = 0;
			EPwm7Regs.CMPA.bit.CMPA = T1PR;
			EPwm8Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataC;
			EPwm8Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataC;
		}
		else
		{
			if ((VoltInCF >= 0) && (VoltInCF < 35))
			{
				if (GridCurrCF >= 0)
				{
					EPwm7Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCsubnegCurCompPerc);
					EPwm7Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCsubnegCurCompPerc);
					EPwm8Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCaddnegCurCompPerc);
					EPwm8Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCaddnegCurCompPerc);
				}
				else
				{
					EPwm7Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCaddnegCurCompPerc);
					EPwm7Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCaddnegCurCompPerc);
					EPwm8Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCsubnegCurCompPerc);
					EPwm8Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCsubnegCurCompPerc);
				}
			}
			else if ((VoltInCF < 0) && (VoltInCF > (-35)))
			{
				if (GridCurrCF >= 0)
				{
					EPwm7Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCsubnegCurCompPerc);
					EPwm7Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCsubnegCurCompPerc);
					EPwm8Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCaddnegCurCompPerc);
					EPwm8Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCaddnegCurCompPerc);
				}
				else
				{
					EPwm7Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCaddnegCurCompPerc);
					EPwm7Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCaddnegCurCompPerc);
					EPwm8Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCsubnegCurCompPerc);
					EPwm8Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCsubnegCurCompPerc);
				}
			}
		}
	}
	else
	{
		PIVolC.i10 = 0;
		PIVolC.i6 = 0;
		GridCurrPICtrlC.i10 = 0;
		GridCurrPICtrlC.i6 = 0;
		ConstantCurr[2].state = SM_CONSTANT_CURR_STANDBY;
		EPwm7Regs.CMPA.bit.CMPA = T1PR;
		EPwm7Regs.CMPB.bit.CMPB = T1PR;
		EPwm8Regs.CMPA.bit.CMPA = T1PR;
		EPwm8Regs.CMPB.bit.CMPB = T1PR;
		ESC_FeedForward_DutyC = 1;
	}
#endif
}

/*
 * ADC-D-1的中断服务函数。WY
 * 说明：中断频率20K，中断周期50us。
 */
void ADCD1INT(void)
{
	FaultFastDetectInInt(); //快速故障检测。WY
	VoltSlidPosCnt(); //构造正弦波。WY
	GenModulation(); //PWM调制。WY
	ADPosCnt(); //产生节拍。WY
	GetVolAndInvCurr(); //计算电压和电流。WY

	switch (T1PRPwmFrequency)
	{
		case 0:
		{
			/*
			 * 按照节拍，间隔执行不同功能的函数。WY
			 * 目的：降低函数的调用频率，压缩AD中断服务函数的执行时间。
			 */
			switch (ADBufPos)
			{
				case 0:
				{
					SetHeatPulse(); //产生DSP心跳脉冲。WY
					PLLrun(); //锁相  10K
					SigPhDQComput();//无功计算 10k
				}
					break;

				case 1:
				{
					Swi_post(RMSstart); //释放RTOS信号量。WY
					RmsCalcIn();//10K
					FirstRmsCalc();//5K
				}
					break;

				case 2:
				{
					StateFeedBackJudge();
					SetHeatPulse(); //产生DSP心跳脉冲。WY
					PLLrun();//锁相
					SigPhDQComput();
				}
					break;

				case 3:
				{
					RmsCalcIn();
				}
					break;

				case 4:
				{
					SetHeatPulse(); //产生DSP心跳脉冲。WY
					PLLrun();//锁相
					SigPhDQComput();
				}
					break;

				case 5:
				{
					Swi_post(RMSstart); //释放RTOS信号量。WY
					FirstRmsCalc();//
					RmsCalcIn();
				}
					break;

				case 6:
				{
					SetHeatPulse(); //产生DSP心跳脉冲。WY
					PLLrun();//锁相
					SigPhDQComput();
				}
					break;

				case 7:
				{
					RmsCalcIn();
				}
					break;
			}
		}
			break;
	}

	FaultRecordProg(); //故障录波。WY
	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //清除ADC-D-1的中断标志位。WY
}

void RMSswi(void)
{
    FaultDetectInInt();             //0.61us
    FaultRecordSel();
    if (StateFlag.onceTimeAdcAutoAdjust)
    {    //上电后自动运行一次
        AdRegeditOffset();
    }
}


