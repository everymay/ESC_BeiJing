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
 * ���ġ�WY
 * �ñ�����ADC-D-1���ж�ISR�б�ʹ�á�
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
 * ���ܣ������ѹ�͵�����WY
 * ˵�����ú�����ADC-D-1���жϷ������б����á�
 */
inline void GetVolAndInvCurr(void)
{
	VirtulADStruVAL *pAD = &VirtulADVAL; //AD��ʵ����ֵ��WY
	VirtulADStruval *pID = &VirtulADval; //�ɴ������·�AD��ƫ��׼ֵ��WY

	/*
	 * ����ƫУ׼���ģ������˲ʱֵ����WY
	   [0]��A�ฺ�ص�ѹ��˲ʱֵ��
	   [1]��B�ฺ�ص�ѹ��˲ʱֵ��
	   [2]��C�ฺ�ص�ѹ��˲ʱֵ��

	   [3]��A�������ѹ��˲ʱֵ��
	   [4]��B�������ѹ��˲ʱֵ��
	   [5]��C�������ѹ��˲ʱֵ��

	   [6]��A�ฺ�ص�����˲ʱֵ��
	   [7]��B�ฺ�ص�����˲ʱֵ��
	   [8]��C�ฺ�ص�����˲ʱֵ��

	   [9]��A����·������˲ʱֵ��
	   [10]��B����·������˲ʱֵ��
	   [11]��C����·������˲ʱֵ��

	   [12]��A��ֱ�����ݵ�ѹ��˲ʱֵ��
	   [13]��B��ֱ�����ݵ�ѹ��˲ʱֵ��
	   [14]��C��ֱ�����ݵ�ѹ��˲ʱֵ��
	 */
	float Buff[15];

	float *pBuff = Buff; //ָ�򸡵�����ָ�룬ָ����ƫУ׼���ģ������WY

	/*��ѹģʽ��WY*/
	if(StateFlag.VoltageModeFlag == 1)
	{
		*pBuff ++ = (*pAD->GridHVoltA - pID->gridHVoltA) * gridVoltRatio;                  //������ѹ,��ѹ����ϵ��        //������ѹ������(�˷������)��ʵ�ʵ�ѹ���η�����
		*pBuff ++ = (*pAD->GridHVoltB - pID->gridHVoltB) * gridVoltRatio;
		*pBuff ++ = (*pAD->GridHVoltC - pID->gridHVoltC) * gridVoltRatio;

		*pBuff ++ = (*pAD->GridLVoltA - pID->gridLVoltA) * loadVoltRatio;                  //���ص�ѹ,��ѹ����ϵ��        //���ص�ѹ������(�˷������)��ʵ�ʵ�ѹ���η�����
		*pBuff ++ = (*pAD->GridLVoltB - pID->gridLVoltB) * loadVoltRatio;
		*pBuff ++ = (*pAD->GridLVoltC - pID->gridLVoltC) * loadVoltRatio;

		*pBuff ++ = (*pAD->GridMainCurA - pID->gridMainCurA) * outputCurRatioCurrA;                  //�翹������,�����������ϵ��   //���翹����������(�˷������)��ʵ�ʵ������η����෴(�̵����������������)
		*pBuff ++ = (*pAD->GridMainCurB - pID->gridMainCurB) * outputCurRatioCurrB;                  //A�����������B,C����������෴,��Ҫ���?????--LJH
		*pBuff ++ = (*pAD->GridMainCurC - pID->gridMainCurC) * outputCurRatioCurrC;

		*pBuff ++ = (*pAD->GridBypassCurA - pID->gridBypassCurA) * outputCurBypassCurrA;                  //��·����,�����������ϵ��   //��·����������(�˷������)��ʵ�ʵ������η����෴(����֤???)
		*pBuff ++ = (*pAD->GridBypassCurB - pID->gridBypassCurB) * outputCurBypassCurrB;                  //A�����������B,C����������෴,��Ҫ���?????--LJH
		*pBuff ++ = (*pAD->GridBypassCurC - pID->gridBypassCurC) * outputCurBypassCurrC;

		*pBuff ++ = (*pAD->ADCUDCA - pID->aDCUDCA) * dcCapVoltRatio;                  //ֱ�����ݵ�ѹ,��ѹ����ϵ��
		*pBuff ++ = (*pAD->ADCUDCB - pID->aDCUDCB) * dcCapVoltRatio;
		*pBuff ++ = (*pAD->ADCUDCC - pID->aDCUDCC) * dcCapVoltRatio;
	}
	else if(StateFlag.VoltageModeFlag == 0) //��ѹģʽ��WY
	{
		*pBuff ++ = (*pAD->GridLVoltA - pID->gridLVoltA) * loadVoltRatio;                  //���ص�ѹ,��ѹ����ϵ��        //���ص�ѹ������(�˷������)��ʵ�ʵ�ѹ���η�����
		*pBuff ++ = (*pAD->GridLVoltB - pID->gridLVoltB) * loadVoltRatio;
		*pBuff ++ = (*pAD->GridLVoltC - pID->gridLVoltC) * loadVoltRatio;

		*pBuff ++ = (*pAD->GridHVoltA - pID->gridHVoltA) * gridVoltRatio;                  //������ѹ,��ѹ����ϵ��        //������ѹ������(�˷������)��ʵ�ʵ�ѹ���η�����
		*pBuff ++ = (*pAD->GridHVoltB - pID->gridHVoltB) * gridVoltRatio;
		*pBuff ++ = (*pAD->GridHVoltC - pID->gridHVoltC) * gridVoltRatio;

		*pBuff ++ = (*pAD->GridMainCurA - pID->gridMainCurA) * outputCurRatioCurrA;                  //�翹������,�����������ϵ��   //���翹����������(�˷������)��ʵ�ʵ������η����෴(�̵����������������)
		*pBuff ++ = (*pAD->GridMainCurB - pID->gridMainCurB) * outputCurRatioCurrB;                  //A�����������B,C����������෴,��Ҫ���?????--LJH
		*pBuff ++ = (*pAD->GridMainCurC - pID->gridMainCurC) * outputCurRatioCurrC;

		*pBuff ++ = (*pAD->GridBypassCurA - pID->gridBypassCurA) * outputCurBypassCurrA;                  //��·����,�����������ϵ��   //��·����������(�˷������)��ʵ�ʵ������η����෴(����֤???)
		*pBuff ++ = (*pAD->GridBypassCurB - pID->gridBypassCurB) * outputCurBypassCurrB;                  //A�����������B,C����������෴,��Ҫ���?????--LJH
		*pBuff ++ = (*pAD->GridBypassCurC - pID->gridBypassCurC) * outputCurBypassCurrC;

		*pBuff ++ = (*pAD->ADCUDCA - pID->aDCUDCA) * dcCapVoltRatio;                  //ֱ�����ݵ�ѹ,��ѹ����ϵ��
		*pBuff ++ = (*pAD->ADCUDCB - pID->aDCUDCB) * dcCapVoltRatio;
		*pBuff ++ = (*pAD->ADCUDCC - pID->aDCUDCC) * dcCapVoltRatio;
	}

	DCL_runDF22Group(VolAndInvCurrFilter, Buff, Buff, 15);  //�˲�

	pBuff = Buff;

	float *pOutSrc = VoltSlid[VoltPrvPos]; //ʵʱ��������

	/*��ѹģʽ��WY*/
	if(StateFlag.VoltageModeFlag == 1)
	{
		*pOutSrc ++ = *pBuff ++;
		*pOutSrc ++ = *pBuff ++;
		*pOutSrc ++ = *pBuff ++;

		pOutSrc = VoltSlid[VoltPos];                  //���е�ѹУ��

		GridVoltAF = *pOutSrc ++;                  //������ѹ
		GridVoltBF = *pOutSrc ++;
		GridVoltCF = *pOutSrc ++;

		LoadVoltUF = *pBuff ++;                  //���ص�ѹ
		LoadVoltVF = *pBuff ++;
		LoadVoltWF = *pBuff ++;
	}
	else if(StateFlag.VoltageModeFlag == 0) //��ѹģʽ��WY
	{
		*pOutSrc ++ = *pBuff ++; //���ص�ѹ��WY
		*pOutSrc ++ = *pBuff ++;
		*pOutSrc ++ = *pBuff ++;

		pOutSrc = VoltSlid[VoltPos];                  //���е�ѹУ��

		LoadVoltUF = *pOutSrc ++;
		LoadVoltVF = *pOutSrc ++;
		LoadVoltWF = *pOutSrc ++;

		GridVoltAF = *pBuff ++; //������ѹ
		GridVoltBF = *pBuff ++;
		GridVoltCF = *pBuff ++;
	}

	GridCurrAF = *pBuff ++;                  //�翹������
	GridCurrBF = *pBuff ++;
	GridCurrCF = *pBuff ++;

	GridBPCurrAF = *pBuff ++;                  //��·����
	GridBPCurrBF = *pBuff ++;
	GridBPCurrCF = *pBuff ++;

	DccapVoltA = *pBuff ++;                  //ֱ�����ݵ�ѹ
	DccapVoltB = *pBuff ++;
	DccapVoltC = *pBuff;

	if(StateFlag.VoltageModeFlag == 1)
	{                  //��ѹ
		VoltInAF = GridVoltAF;
		VoltInBF = GridVoltBF;
		VoltInCF = GridVoltCF;
		VoltOutAF = LoadVoltUF;
		VoltOutBF = LoadVoltVF;
		VoltOutCF = LoadVoltWF;
	}
	else if(StateFlag.VoltageModeFlag == 0)
	{                  //��ѹ
		VoltInAF = LoadVoltUF;
		VoltInBF = LoadVoltVF;
		VoltInCF = LoadVoltWF;
		VoltOutAF = GridVoltAF;
		VoltOutBF = GridVoltBF;
		VoltOutCF = GridVoltCF;
	}

//��ʱ90�ȹ���Beta    ��������

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
//�����������(�й����ʺ��޹�����)������(�й����ʺ��޹�����)������ĺ�����
inline void InverseTransformF(void)
{
}

//function of output fundamental positive sequence (active and reactive) and negative sequence (active and reactive power) and zero sequence.
//�����������(�й����ʺ��޹�����)������(�й����ʺ��޹�����)������ĺ�����
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

/*�ú���δ���塣WY*/
void GetGridLoadcurrAD(void)
{
}

/*�ú���δ���塣WY*/
void GetGridLoadcurr(void)
{

}

/*�ú���δ���塣WY*/
void GetUdc(void)
{
}


inline void FaultRecordSel(void)
{
	struct Stru_FaultRecSel *pSel=&FaultRecSel;
	if((pSel->ForceRecordWaveRefresh)&&(GridVoltTheta<(1.5*50*PI2/CTRLFREQUENCY))){ //���沨����ʾǿ��ˢ��ͬʱ����1.5������С���㲽����Χ,��Ϊ������ǶȵĹ����
		pSel->ForceRecordWaveRefresh = FALSE;				//��0��ʼˢ��һ����������
		CntForRec = DEBUG_WAVE_LEN-1;						//ˢ��2000����
		pSel->DelayRec = DEBUG_WAVE_LEN-1;					//ˢ��2000����
		pSel->isFFTMode = TRUE;								//�л�ΪFFTģʽ
		pSel->RecordChan = 1;								//��1��̶�ΪFFTģʽ��ADͨ��
		CntSec.ForceRecordWaveRefreshCnt =0;
	}
	if(pSel->isFFTMode){ 									//FFTģʽ//ʵʱ������ʾ
		if(CntSec.ForceRecordWaveRefreshCnt>30){
			pSel->isFFTMode=0;								//��ʱ30����Զ��ر�FFTģʽ
			pSel->RecordChan = pSel->RecordChanOrg;
			CntSec.ForceRecordWaveRefreshCnt =0;
		}
	}else{													//����¼��ģʽ
		if( (!pSel->stopRecordMode)&&\
		  	(/*((StateEventFlag_A == STATE_EVENT_RUN_A) || (StateEventFlag_A == STATE_EVENT_RECHARGE_A) || (StateEventFlag_A == STATE_EVENT_WAIT_A))&& \
			 ((StateEventFlag_B == STATE_EVENT_RUN_B) || (StateEventFlag_B == STATE_EVENT_RECHARGE_B) || (StateEventFlag_B == STATE_EVENT_WAIT_B))&& \*/
			 ((StateEventFlag_C == STATE_EVENT_RUN_C) || (StateEventFlag_C == STATE_EVENT_RECHARGE_C) || (StateEventFlag_C == STATE_EVENT_WAIT_C))) ){	//ͣ��ֹͣ¼��ģʽ
			pSel->DelayRec = 50;							//ͣ����¼50����
		}else if(pSel->stopRecordMode){						//һֱ¼��ģʽ
				pSel->DelayRec = DEBUG_WAVE_LEN-1;						//һֱ¼��
		}
	}
	ErrorRecord.B.REC_FFT_MODE = pSel->isFFTMode;
}

/*
 * ���ܣ�����DSP�������塣WY
 * ˵����ͨ����תGPIO��ƽ������DSP�������塣
   �ú�����ADC-D-1���ж�ISR�б����ã�������Ƶ��ȡ����ADC-D-1���ж�Ƶ�ʡ�
 */
inline void  SetHeatPulse(void)
{
#if HEATPULSE == 1
	TOGGLE_PULSE(); //��תGPIO��ƽ��WY
#endif
}

/*
 * ���ܣ��������ġ�
 * ˵�����ú�����ADC-D-1���ж�ISR�б�����
 */
inline void ADPosCnt(void)
{
	if (++ADBufPos >= ADGROUP_NUM)
	{
		ADBufPos = 0;
	}

	TimeStamp = PhaseLimit(TimeStamp + IFFT_PHASE_STEP);//�ú��������塣WY //ifft�õ�ʱ�������
}

/*
 * ���ܣ��������Ҳ���WY
 */
inline void VoltSlidPosCnt(void)
{
	if (++VoltPrvPos >= VOLT_FUNDPOINT)
		VoltPrvPos = 0;//������ѹһ�ܲ�����������ָ��(���������˲�����ʱ��������ʱ��)//ʵʱ�����Ĳ���(�����˲�֮��)
	VoltPos = VoltPrvPos + VoltLeadPoints;//��ǰУ��ָ��,��ǰ��360*VoltLeadPoints/VOLT_FUNDPOINT��//��ѹУ��

	if (VoltPos >= VOLT_FUNDPOINT)
		VoltPos -= VOLT_FUNDPOINT;//���λ�����//VoltPos����ǰ���õ�һ�������������

	CapVoltPos = VoltPos + CAP_VOL_POINT;//���ݵ�ѹһ�ܲ�������ָ��,�ͺ�270��//
	if (CapVoltPos >= VOLT_FUNDPOINT)
		CapVoltPos -= VOLT_FUNDPOINT;//���λ�����

	CapVoltPosCur = VoltPos + CAP_VOL_POINT_CUR;//���ݵ�ѹһ�ܲ�������ָ��,�ͺ�90��//
	if (CapVoltPosCur >= VOLT_FUNDPOINT)
		CapVoltPosCur -= VOLT_FUNDPOINT;
}

#if PWM_FREQUENCE_16KHZ

/*�ú���δ���塣WY*/
void PIController(void)
{
}

#endif

#define DEAD_CORR_HYSTERESIS 3

/*�ú궨��δʹ�á�WY*/
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

float ReactivePowerComFUN(float Esc_VoltPLL,float Q)  //ESC�޹���������
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

    if(ConstantReactivePower<0.001 && ConstantReactivePower>-0.001) //��������Ϊ0,�����޹�����ģʽ���Զ�����.
        return (tmp)*fabsf(Q);
    else    //�����·�ֵ��Ϊ����
        return (tmp)*ConstantReactivePower*LooktablePower2Amplitude;
}

#ifdef unblance_gravity
#else

/*
 * �ڷ������������ƽ��ʱ�����ڱ�ʶ������Чֵ��С����WY
 */
enum ChanMaxName
{
	UnCurrCompChanA = 0, //A��
	UnCurrCompChanB = 1, //B��
	UnCurrCompChanC = 2, //C��
};

/*
 * ���ܣ����������������Чֵ���ս�������WY
 *
 * �������a��A�����������Чֵ
 * �������b��B�����������Чֵ
 * �������c��C�����������Чֵ
 * �������max���������������Чֵ�����ֵ���
 * �������min���������������Чֵ����Сֵ���
 * �������midd���������������Чֵ�ľ���ֵ���
 */
void ChanMax3(float a, float b, float c, int *max, int *min, int *midd)
{
	if (a > b)
	{
		if (a > c)
		{
			*max = UnCurrCompChanA; //A�������ЧֵΪ�����е����ֵ��WY

			if (b < c)
			{
				*min = UnCurrCompChanB; //B�������ЧֵΪ�����е���Сֵ��WY
				*midd = UnCurrCompChanC; //C�������ЧֵΪ�����еľ���ֵ��WY
			}
			else
			{
				*min = UnCurrCompChanC; //C�������ЧֵΪ�����е���Сֵ��WY
				*midd = UnCurrCompChanB; //B�������ЧֵΪ�����еľ���ֵ��WY
			}
		}
		else
		{
			*max = UnCurrCompChanC; //C�������ЧֵΪ�����е����ֵ��WY
			*min = UnCurrCompChanB; //B�������ЧֵΪ�����е���Сֵ��WY
			*midd = UnCurrCompChanA; //A�������ЧֵΪ�����еľ���ֵ��WY
		}
	}
	else
	{
		if (a < c)
		{
			*min = UnCurrCompChanA; //A�������ЧֵΪ�����е���Сֵ��WY

			if (b < c)
			{
				*max = UnCurrCompChanC; //C�������ЧֵΪ�����е����ֵ��WY
				*midd = UnCurrCompChanB; //B�������ЧֵΪ�����еľ���ֵ��WY
			}
			else
			{
				*max = UnCurrCompChanB; //B�������ЧֵΪ�����е����ֵ��WY
				*midd = UnCurrCompChanC; //C�������ЧֵΪ�����еľ���ֵ��WY
			}
		}
		else
		{
			*min = UnCurrCompChanC; //C�������ЧֵΪ�����е���Сֵ��WY
			*max = UnCurrCompChanB; //B�������ЧֵΪ�����е����ֵ��WY
			*midd = UnCurrCompChanA; //A�������ЧֵΪ�����еľ���ֵ��WY
		}
	}
}

int unbalanceGenFlag = 0; //���಻ƽ���־λ��0������ƽ�⣨Ĭ�ϣ���1�����಻ƽ�⡣WY

int maxCh; //���������Чֵ����ߵ���š�WY
int minCh; //���������Чֵ��С�ߵ���š�WY
int middCh; //���������Чֵ�����ߵ���š�WY

/*
 * �������������ƽ��ʱ�Ĳ�������λ��V����WY
 */
#define UN_CURR_COMP_STEP_VOLT  0.0003f

/*
 * ���ܣ�������������������Чֵ����ƽ�⣬�ݴ��������ฺ�ص�ѹ����Чֵ��Ŀ��ֵ��WY
 */
void UnCurrCompFUN(void)
{
	float tmp = 0; //��ʱ������WY

	float CurMeanValue = (UnCurrData[0] + UnCurrData[1] + UnCurrData[2]) * S1DIV3;//������������������Чֵ��ƽ��ֵ��WY
	float CurDiffMAXValue = Max3(UnCurrData[0], UnCurrData[1], UnCurrData[2]); //������������������Чֵ�������ֵ��WY
	currentUnbalance = (CurDiffMAXValue - CurMeanValue) / CurMeanValue; //������������������Чֵ����ƽ��ȡ�WY

	if (currentUnbalance > TargetCurrentUnbalance) //���������������Чֵ���Ĳ�ƽ��� > ���������������Чֵ���Ĳ�ƽ������ޡ�WY
	{
		if (CntMs.UNCurrDelay1 >= CNT_MS(1000)) //���������������Чֵ����ƽ�����ʱ�䳬��1000ms��WY
		{
			unbalanceGenFlag = TRUE; //�豸�������������������Чֵ����ƽ��״̬��WY
		}
	}
	else
	{
		CntMs.UNCurrDelay1 = 0; //�����ʱ����WY
	}

	if (currentUnbalance < TargetCurrentUnbalance - 0.01) //���������������Чֵ����ƽ��� < (���������������Чֵ����ƽ������� - 0.01)��Ŀ�ģ��趨������ֵ����ֹϵͳ������WY
	{
		if (CntMs.UNCurrDelay2 >= CNT_MS(20000)) //����ʱ�䳬��20000ms��Ŀ�ģ���ֹϵͳ������WY
		{
			unbalanceGenFlag = FALSE; //�豸�������������������Чֵ��ƽ��״̬��WY
		}
	}
	else
	{
		CntMs.UNCurrDelay2 = 0; //�����ʱ����WY
	}

	/*���������������Чֵ����ƽ�⡣WY*/
	if (unbalanceGenFlag == TRUE)
	{
		ChanMax3(UnCurrData[0], UnCurrData[1], UnCurrData[2], &maxCh, &minCh, &middCh); //�����������������Чֵ�����ս�������WY
		CurrentUnbalanceRegularVoltage[maxCh] -= UN_CURR_COMP_STEP_VOLT; //���͵�����������Чֵ��������Ӧ�ĸ��ص�ѹ����Чֵ��Ŀ��ֵ��WY
		tmp = CurrentUnbalanceRegularVoltage[maxCh];
		CurrentUnbalanceRegularVoltage[minCh] += UN_CURR_COMP_STEP_VOLT; //��ߵ�����������Чֵ����С���Ӧ�ĸ��ص�ѹ����Чֵ��Ŀ��ֵ��WY
		tmp += CurrentUnbalanceRegularVoltage[minCh];
		float sum = ESCFlagA.Volttarget + ESCFlagB.Volttarget + ESCFlagC.Volttarget; //�û��趨�ĸ��ص�ѹ����Чֵ��Ŀ��ֵ�ĺ͡�WY

		CurrentUnbalanceRegularVoltage[middCh] = sum - tmp; //�������������������Чֵ�������߶�Ӧ�ĵ�ѹ����Чֵ����WY
	};

	/*����A�ࣺ���ص�ѹ����Чֵ��Ŀ��ֵ�޷�*/
	if (CurrentUnbalanceRegularVoltage[0] > GV_RMS_OVER - 25) //A�ฺ�ص�ѹ����Чֵ��Ŀ��ֵ > ����ֵ��WY
	{
		CurrentUnbalanceRegularVoltage[0] = GV_RMS_OVER - 25; //�޷����޶�A�ฺ�ص�ѹ����Чֵ��Ŀ��ֵ�����ޡ�WY

	}
	if (CurrentUnbalanceRegularVoltage[0] < GV_RMS_UNDER + 40) //A�ฺ�ص�ѹ����Чֵ��Ŀ��ֵ < ����ֵ��WY
	{
		CurrentUnbalanceRegularVoltage[0] = GV_RMS_UNDER + 40; //�޷����޶�A�ฺ�ص�ѹ����Чֵ��Ŀ��ֵ�����ޡ�WY
	}

	/*����B�ࣺ���ص�ѹ����Чֵ��Ŀ��ֵ�޷�*/
	if (CurrentUnbalanceRegularVoltage[1] > GV_RMS_OVER - 25)
	{
		CurrentUnbalanceRegularVoltage[1] = GV_RMS_OVER - 25;
	}
	if (CurrentUnbalanceRegularVoltage[1] < GV_RMS_UNDER + 40)
	{
		CurrentUnbalanceRegularVoltage[1] = GV_RMS_UNDER + 40;
	}

	/*����C�ࣺ���ص�ѹ����Чֵ��Ŀ��ֵ�޷�*/
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

float ESC_FeedForward_DutyA = 0; //A��PWMռ�ձ�ǰ��ֵ��WY
float ESC_FeedForward_DutyB = 0; //B��PWMռ�ձ�ǰ��ֵ��WY
float ESC_FeedForward_DutyC = 0; //C��PWMռ�ձ�ǰ��ֵ��WY

float PIOutVoltValueA = 0; //A��PI�����������ֵ��WY
float PIOutVoltValueB = 0; //B��PI�����������ֵ��WY
float PIOutVoltValueC = 0; //C��PI�����������ֵ��WY

int ESCLowCurrFlagA = 0,ESCLowCurrFlagB = 0,ESCLowCurrFlagC = 0; //�ñ���δʹ�á�WY

#define LEAD_SAMPLE_INCREMENT_DUTY  0.0f //�ú궨��δʹ�á�WY
#define LEAD_SAMPLE_DECREMENT_DUTY  0.0f //�ú궨��δʹ�á�WY

float DBG_leadSampleIncrementDuty = LEAD_SAMPLE_INCREMENT_DUTY; //�ñ���δʹ�á�WY
float DBG_leadSampleDecrementDuty = LEAD_SAMPLE_DECREMENT_DUTY; //�ñ���δʹ�á�WY

/*
 * ���ܣ����ݵ�����ѹ�Ƿ����ͻ�䣬���PWMռ�ձȡ�WY
 *
 * �������RMSGridInVAL��������ѹ����Чֵ��
 * �������INSGridIn��������ѹ��˲ʱֵ��
 * �������target�����ص�ѹ����Чֵ��Ŀ��ֵ
 * �������PIerr��PI�����������ֵ
 * �������LockPhAngle�������໷�õ�����λ
 * �������pStorage��PWMռ�ձ�Ԥ��ֵ����������ѹδ����ͻ��ʱ��ʹ��PWMռ�ձ�Ԥ��ֵ�����ƽ�PWMǰ��ֵ��ʵ���ȶ�ϵͳ�񵴵����á�
 * �������ESCPHASE����λѡ��
 *
 * ����ֵ��PWMռ�ձ�
 */
float RMSDutyLimit(float RMSGridInVAL, //������ѹ����Чֵ��
				   float INSGridIn, //������ѹ��˲ʱֵ��
				   float target, //���ص�ѹ����Чֵ��Ŀ��ֵ
				   float PIerr, //PI�����������ֵ(��������PWMռ�ձȵ�ǰ��ֵ)
				   float LockPhAngle, //�����໷�õ�����λ����������ѹδ����ͻ��ʱ��ʹ��PWMռ�ձ�Ԥ��ֵ�����ƽ�PWMǰ��ֵ��ʵ���ȶ�ϵͳ�񵴵����á�
				   float *pStorage, //PWMռ�ձ�Ԥ��ֵ��
				   int ESCPHASE) //��λѡ��
{
	ArithFlagA = 2; //A�������ѹͻ���־λ��0������ͻ�䣻1��������ͻ�䡣WY
	ArithFlagB = 2; //B�������ѹͻ���־λ��0������ͻ�䣻1��������ͻ�䡣WY
	ArithFlagC = 2; //C�������ѹͻ���־λ��0������ͻ�䣻1��������ͻ�䡣WY

	TESTVALUE = 0; //�ñ���δʹ�á�WY
	TESEPIerr = PIerr; //�ñ���δʹ�á�WY
	testvalue1 = *pStorage; //�ñ���δʹ�á�WY
	PhaseValue = LockPhAngle; //�ñ���δʹ�á�WY
	TESTRMSGridInVAL = RMSGridInVAL; //�ñ���δʹ�á�WY
	TESETarget = target; //�ñ���δʹ�á�WY
	TESEINSGridIn = INSGridIn; //�ñ���δʹ�á�WY

	NumeratorValue = INSGridIn - RMSGridInVAL * 1.4142 * sin(LockPhAngle); //������ѹ��˲ʱֵ����ͻ��ֵ = ������ѹ��˲ʱֵ�� - �ϸ��ܲ�������ѹ��˲ʱֵ����WY
	DenominatorVAL = target * 1.4142 * sin(LockPhAngle); //�ñ���δʹ�á�WY

	if ((RMSGridInVAL > 10) //������ѹ����Чֵ�� > 10V��WY
			&& (target > 10)) //���ص�ѹ����Чֵ��Ŀ��ֵ > 10V��
	{
		 /*������ѹ���ڹ���㸽����WY*/
		if (((LockPhAngle >= 0.3f)
				&& (LockPhAngle <= (PIVAL - 0.3f)))
				|| ((LockPhAngle >= (PIVAL + 0.3f))
				&& (LockPhAngle <= (2.0f * PIVAL - 0.3f))))
		{
			/*��ѹģʽ��WY*/
			if (StateFlag.VoltageModeFlag == 0)
			{
				/*����A��*/
				if (ESCPHASE == 1)
				{
					/*��ѹ������ͻ�䡣WY*/
					if ((TestArithFlagA == 1) //����״̬��WY
							&& (((NumeratorValue < ArithVAL) //������ѹ��˲ʱֵ��ͻ��ֵ < ������ѹ��˲ʱֵ����ͻ��ֵ���ޡ�WY
							&& (NumeratorValue > ArithVal)) //������ѹ��˲ʱֵ��ͻ��ֵ > ������ѹ��˲ʱֵ����ͻ��ֵ���ޡ�WY
							|| (ESCFlagA.ESCCntMs.HarmDisDelay > CNT_MS(20)))) //������ѹ��˲ʱֵ��ͻ���ʱ > 20ms��WY
					{
						dutytmp = dutytmp1 = RMSGridInVAL / target; //PWMռ�ձ� = ������ѹ����Чֵ�� / ���ص�ѹ����Чֵ��Ŀ��ֵ��WY
						ArithFlagA = 1; //A�������ѹ������ͻ�䡣WY

						if ((NumeratorValue < ArithVAL) //������ѹͻ��ֵ < ������ѹͻ��ֵ���ޡ�WY
								&& (NumeratorValue > ArithVal)) //������ѹͻ��ֵ > ������ѹͻ��ֵ���ޡ�WY
						{
							ESCFlagA.ESCCntMs.HarmDisDelay = 0; //���������ѹͻ�����ʱ�䡣WY
						}
					}
					else if
					/*��ѹ����ͻ�䡣WY*/
					((NumeratorValue >= ArithVAL) //������ѹ��˲ʱֵ��ͻ��ֵ > ������ѹ��˲ʱֵ��ͻ��ֵ���ޡ�WY
							|| (NumeratorValue <= ArithVal) //������ѹ��˲ʱֵ��ͻ��ֵ < ������ѹ��˲ʱֵ��ͻ��ֵ���ޡ�WY
							|| (TestArithFlagA == 0) //����״̬��WY
							|| (ESCFlagA.VoltIn_rms >= GV_RMS_OVER)) //������ѹ����Чֵ�� > ������ѹ����Чֵ�����ޡ�WY
					{

						if ((ESCFlagA.ESCCntMs.HarmDisDelay <= CNT_MS(20))) //������ѹͻ�����ʱ�� < 20ms��WY
						{
							dutytmp = dutytmp1 = (RMSGridInVAL / target)  //������ѹ����Чֵ�� / ���ص�ѹ����Чֵ����Ŀ��ֵ��WY
									+ ((INSGridIn - RMSGridInVAL * SQRT2 * sin(LockPhAngle)) / (target * SQRT2 * sin(LockPhAngle))); //��ѹͻ����� = (������ѹ˲ʱֵ - �ϸ��ܲ�������ѹ��˲ʱֵ) / ���ص�ѹ˲ʱֵ��Ŀ��ֵ��WY
							ArithFlagA = 0; //A�������ѹ����ͻ�䡣WY
							TestArithFlagA = 0; //������WY
						}
						else //������ѹͻ���ʱ > 20ms��WY
						{
							TestArithFlagA = 1; //������WY
							*pStorage = RMSGridInVAL / target; //PWMռ�ձ� = ��ǰ������ѹ(��Чֵ) / ���ص�ѹ(��Чֵ)Ŀ��ֵ��WY
						}
					}
				}

				/*����B��*/
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

				/*����C��*/
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
			/*��ѹģʽ��WY*/
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

		/*�޶�PWMռ�ձ�����*/
		if (dutytmp1 > 0.96)
		{
			dutytmp1 = 0.96;
		}

		/*�޶�PWMռ�ձ�����*/
		if (dutytmp1 < 0.51)
		{
			dutytmp1 = 0.51;
		}
	}
	else //(������ѹ��Чֵ < 10V)��(���ص�ѹ��Чֵ��Ŀ��ֵ < 10V)��WY
	{
		dutytmp1 = 0.96;
	}

//��ֵ�仯б�ʿ���,ÿ��仯��390%:=0.00001*25.6k

	/*����A��*/
	if ((ArithFlagA == 1) //A�������ѹ������ͻ�䡣WY
			&& (ESCPHASE == 1)) //A�ࡣWY
	{
		if ((dutytmp1 > 0.499) && (dutytmp1 < 0.961)) //PWMռ�ձȵ�ǰ��ֵ�ڷ�Χ�ڡ�WY
		{
			if (*pStorage > dutytmp1) //PWMռ�ձȵ�Ԥ��ֵ > PWMռ�ձȵ�ǰ��ֵ��WY
			{
				*pStorage -= 0.0001; //΢��PWMռ�ձȵ�Ԥ��ֵ��ʹ�仺���ƽ�PWMռ�ձȵ�ǰ��ֵ��WY
			}
			else if (*pStorage < dutytmp1) //PWMռ�ձȵ�ǰ��ֵ < PWMռ�ձȡ�WY
			{
				*pStorage += 0.0001; //΢��PWMռ�ձȵ�Ԥ��ֵ��ʹ�仺���ƽ�PWMռ�ձȵ�ǰ��ֵ��WY
			}
			else //�������㲻������ԭ���޷���������������Ƚϡ�WY
			{
				*pStorage = dutytmp1; //����΢��΢��PWMռ�ձȵ�Ԥ��ֵ��WY
			}
		}
		else if (dutytmp1 > 0.962) //PWMռ�ձȵ�ǰ��ֵ��ע�⣺�������㲻������WY
		{
			*pStorage = dutytmp1 = 0.962; //�޶�PWMռ�ձȵ�ǰ��ֵ�����ޡ�WY
		}
		else if (dutytmp1 < 0.497) //PWMռ�ձȵ�ǰ��ֵ��ע�⣺�������㲻������WY
		{
			*pStorage = dutytmp1 = 0.497; //�޶�PWMռ�ձȵ�ǰ��ֵ�����ޡ�WY
		}

		/*��ѹģʽ��WY*/
		if (StateFlag.VoltageModeFlag == 0)
		{
			dutytmp1 = *pStorage - PIerr; //PWMռ�ձȵ�Ԥ��ֵ - PI�����������ֵ��ע�⣺����PI�������������󣬵ó�PWMռ�ձȵ�����ֵ��WY
		}
		/*��ѹģʽ��WY*/
		else if (StateFlag.VoltageModeFlag == 1)
		{
			dutytmp1 = *pStorage + PIerr;
		}
	}

	/*����B��*/
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
			dutytmp1 = *pStorage - PIerr;//*pStorageǰ��ֵ��PIerr����ֵ
		}
		else if (StateFlag.VoltageModeFlag == 1)
		{
			dutytmp1 = *pStorage + PIerr;
		}
	}

	/*����C��*/
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

	/*�޶�PWMռ�ձ����ޡ�WY*/
	if (dutytmp1 > 0.96)
	{
		dutytmp1 = 0.96;
	}

	/*�޶�PWMռ�ձ����ޡ�WY*/
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

int UnCurrCompCnt = 0; //�ñ���δʹ�á�WY

int cntaaa=2;

/*
 * ��ͬϵ�л��͵�����������WY
 */
const SMconstantCurr ConstantCurrDefault[3] =
{
	{
	  VOLT_TARGET_CORR_DEFAULT, //PI������������WY
	  0.0f, //���ص�ѹ��Чֵ��Ŀ��ֵ������ϵ����WY
	  58.0f, //����������Чֵ�Ĺ���ֵ��WY
	  13.0f, //�ó�Աδʹ�á�WY
	  45.0f, //����������Чֵ�Ķֵ��WY
	  82.0f, // ��������˲ʱֵ���޶�ֵ��WY
	  0, //״̬��״̬������������������״̬����WY
	  0, //ǰһ�ε�������״̬��WY
	  0 //��������Ŀ�ģ���ֹ״̬��Ƶ���л�����������WY
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
 * ABC����PI������������WY
 *
 * �ò����������³�����
   ���ݵ�������״̬�����㸺�ص�ѹ����Чֵ��Ŀ��ֵ������ϵ����
 *
 * ע�⣺�ò������ܻᱻ�����������¸�ֵ��
 */
SMconstantCurr ConstantCurr[3] =
{
 	 /*A�����*/
	{
	  VOLT_TARGET_CORR_DEFAULT,
	  0.0f, //���ص�ѹ��Чֵ��Ŀ��ֵ������ϵ������Χ��-0.6 ~ 0���ɷ������۲�ó�������WY
	  58.0f, //������������Чֵ������ֵ��WY
	  13.0f, //�ó�Աδʹ�á�WY
	  45.0f, //������������Чֵ���ֵ��WY
	  82.0f, // ����������˲ʱֵ���ֵ��WY
	  0, //״̬��״̬����������������������WY
	  0, //ǰһ��״̬��״̬��WY
	  0 //��������Ŀ�ģ���ֹ״̬��Ƶ���л�����������WY
	},

	/*B�����*/
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

	/*C�����*/
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
 * ���ܣ����ݵ�������״̬�����㸺�ص�ѹ����Чֵ��Ŀ��ֵ������ϵ����WY
 * ˵�������ݵ��������Ĵ�С����3��״̬���ֱ�Ϊ��
         1. ������������ģʽ��
         2. ���������㶨ģʽ��
         3. ��������ͻ��ģʽ��
 *
 * �������SMconstantCurr���豸������ز���
 * �������gridCur_rms��������������Чֵ��
 * �������GridCurrF������������˲ʱֵ��
 * �������CurrTargetValue����ͬ�����¶��¶�Ӧ�ĵ�����������Чֵ��Ŀ��ֵ
 *
 * ����ֵ�����ص�ѹ����Чֵ��Ŀ��ֵ������ϵ��
 */
float ConstantCurrFSM(SMconstantCurr *pst, //�豸������ز�����WY
					  float gridCur_rms, //������������Чֵ����WY
					  float GridCurrF, //����������˲ʱֵ����WY
					  float CurrTargetValue //��ͬ�����¶��¶�Ӧ�ĵ�����������Чֵ��Ŀ��ֵ��WY
					  )
{
	/*״̬����ת�߼���WY*/
	switch (pst->state)
	{
		 /*������������ģʽ��WY*/
		case SM_CONSTANT_CURR_NORMAL:
		{
			if ((gridCur_rms >= pst->RMS_CONSTANT_CURRENT_OVERLOAD) //������������Чֵ ��> ������������Чֵ������ֵ��WY
					|| (GridCurrF > pst->INS_CONSTANT_CURRENT_RATED) //����������˲ʱֵ�� > ����������˲ʱֵ���ֵ���ޡ�WY
					|| (GridCurrF < -pst->INS_CONSTANT_CURRENT_RATED)) //����������˲ʱֵ�� < ����������˲ʱֵ���ֵ���ޡ�WY
			{
				pst->state = SM_CONSTANT_CURR_INS; //�л�״̬��������������˲ʱģʽ��WY
			}
			else
			{
				if ((gridCur_rms > pst->RMS_CONSTANT_CURRENT_RATED) //������������Чֵ �� > ������������Чֵ���ֵ��WY
						&& (gridCur_rms < pst->RMS_CONSTANT_CURRENT_OVERLOAD)) //������������Чֵ�� < ������������Чֵ������ֵ��WY
				{
					pst->state = SM_CONSTANT_CURR_CONSTANT; //�л�״̬���������������㶨ģʽ��WY
				}
				else if (gridCur_rms <= pst->RMS_CONSTANT_CURRENT_RATED) //������������Чֵ�� < ������������Чֵ���ֵ��WY
				{
					pst->state = SM_CONSTANT_CURR_NORMAL; //�л�״̬������������������ģʽ��WY
				}
			}
		}
			break;

		/*���������㶨ģʽ��WY*/
		case SM_CONSTANT_CURR_CONSTANT:
		{
			if ((gridCur_rms >= pst->RMS_CONSTANT_CURRENT_OVERLOAD) //������������Чֵ ��> ������������Чֵ������ֵ��WY
					|| (GridCurrF > pst->INS_CONSTANT_CURRENT_RATED) //����������˲ʱֵ�� > ����������˲ʱֵ���ֵ���ޡ�WY
					|| (GridCurrF < -pst->INS_CONSTANT_CURRENT_RATED)) //����������˲ʱֵ�� < ����������˲ʱֵ���ֵ���ޡ�WY
			{
				pst->state = SM_CONSTANT_CURR_INS; //�л�״̬��������������˲ʱģʽ��WY
			}
			else
			{
				if ((gridCur_rms > pst->RMS_CONSTANT_CURRENT_RATED) //������������Чֵ �� > ������������Чֵ���ֵ��WY
						&& (gridCur_rms < pst->RMS_CONSTANT_CURRENT_OVERLOAD)) //������������Чֵ�� < ������������Чֵ������ֵ��WY
				{
					pst->state = SM_CONSTANT_CURR_CONSTANT; //�л�״̬���������������㶨ģʽ��WY
				}
				else if (gridCur_rms <= pst->RMS_CONSTANT_CURRENT_RATED) //������������Чֵ�� < ������������Чֵ���ֵ��WY
				{
					pst->state = SM_CONSTANT_CURR_NORMAL; //�л�״̬������������������ģʽ��WY
				}
			}
		}
			break;

		/*��������˲ʱģʽ��WY*/
		case SM_CONSTANT_CURR_INS:
		{
			if ((gridCur_rms >= pst->RMS_CONSTANT_CURRENT_OVERLOAD) //������������Чֵ�� > ������������Чֵ������ֵ��WY
					|| (GridCurrF > pst->INS_CONSTANT_CURRENT_RATED) //����������˲ʱֵ�� > ����������˲ʱֵ�����ޡ�WY
					|| (GridCurrF < -pst->INS_CONSTANT_CURRENT_RATED)) //����������˲ʱֵ�� < ����������˲ʱֵ�����ޡ�WY
			{
				pst->state = SM_CONSTANT_CURR_INS; //�л�״̬��������������˲ʱģʽ��WY
			}
			else
			{
				if ((gridCur_rms > pst->RMS_CONSTANT_CURRENT_RATED) //������������Чֵ > ������������Чֵ���ֵ��WY
						&& (gridCur_rms < pst->RMS_CONSTANT_CURRENT_OVERLOAD)) //������������Чֵ ��< ������������Чֵ������ֵ��WY
				{
					pst->state = SM_CONSTANT_CURR_CONSTANT; //�л�״̬���������������㶨ģʽ��WY
				}
				else if (gridCur_rms <= pst->RMS_CONSTANT_CURRENT_RATED) //������������Чֵ�� < ������������Чֵ���ֵ��WY
				{
					pst->state = SM_CONSTANT_CURR_NORMAL; //�л�״̬������������������ģʽ��WY
				}
			}
		}
			break;

		case SM_CONSTANT_CURR_STANDBY: //Ĭ�����á�WY
		default:
		{
			pst->Prvstate = pst->state = SM_CONSTANT_CURR_NORMAL; //�л�״̬��������������������WY
			pst->CorrPI.i10 = 0;
		}
			break;
	}

	/*״̬������Ϊ��WY*/
	switch (pst->state)
	{
		/*������������ģʽ��WY*/
		case SM_CONSTANT_CURR_NORMAL:
		{
			if (pst->VolttargetCorr < 0)
			{
				pst->VolttargetCorr += 0.00005; //�������ص�ѹ����Чֵ��Ŀ��ֵ����ϵ����WY
			}

			/*��δ���������ʵ�����á�WY*/
			{
			if (pst->Prvstate == SM_CONSTANT_CURR_CONSTANT) //��һ��״̬���¼�Ϊ�����������㶨ģʽ��WY
			{
				pst->CNT1 = 10000; //��ֵ���ڼ���������ֹ�����������������ʵ�����á�WYWY
			}
			if (pst->CNT1 >= 0)
			{
				if (--pst->CNT1 == 0)
				{
					pst->CorrPI.i10 = 0; //�����������ʵ�����á�WY
				}
			}
			}

			pst->Prvstate = pst->state; //��¼״̬��״̬��WY
		}
		break;

		/*���������㶨ģʽ��WY*/
		case SM_CONSTANT_CURR_CONSTANT:
		{
			if (pst->Prvstate == SM_CONSTANT_CURR_INS) //��һ��״̬���¼�Ϊ����������˲ʱģʽ��WY
			{
				pst->CorrPI.i10 = pst->VolttargetCorr;
			}

			if (pst->Prvstate == SM_CONSTANT_CURR_NORMAL) //��һ��״̬���¼�Ϊ��������������ģʽ��WY
			{
				pst->CorrPI.i10 = pst->VolttargetCorr;
			}

			pst->VolttargetCorr = DCL_runPI(&(pst->CorrPI), CurrTargetValue, gridCur_rms); //���㸺�ص�ѹ����Чֵ��Ŀ��ֵ������ϵ����WY

			pst->Prvstate = pst->state; //��¼״̬��״̬��WY
		}
			break;

		/*��������˲ʱģʽ*/
		case SM_CONSTANT_CURR_INS:
		{
			pst->VolttargetCorr = -0.5; //����ѹ��

			pst->Prvstate = pst->state; //��¼״̬��״̬��WY
		}
			break;

		/*����������ʼ*/
		case SM_CONSTANT_CURR_STANDBY:
		default:
		{
			pst->CorrPI.i10 = 0; //����PI�������Ļ����WY
			break;
		}
	}

	return pst->VolttargetCorr; //���ظ��ص�ѹ����Чֵ��Ŀ��ֵ������ϵ����WY
}


/*
 * ���ܣ�PWMռ�ձȵ��ơ�WY
 * ע�⣺�ú�����ADC-D-1��ISR�б����á�
 */
inline void GenModulation(void)
{
	DBG_leadSampleIncrementDuty = harmCompPerc; //�ñ���δʹ�á�WY
	DBG_leadSampleDecrementDuty = -reactPrCompPerc; //�ñ���δʹ�á�WY
	T1PR_INT16 = (int) T1PR; //�ñ���δʹ�á�WY

#if ESC_THREEPHASE
	if ((StateFlag.VoltageMode == 3) //��ƽ��ģʽ��WY
			&& (StateEventFlag_A == STATE_EVENT_RUN_A) //A�ദ������״̬��WY
			&& (ESCFlagA.PWM_ins_index == 0) //A����·����PWM����״̬��WY
			&& (StateEventFlag_B == STATE_EVENT_RUN_B) //B�ദ������״̬��WY
			&& (ESCFlagB.PWM_ins_index == 0) //B����·����PWM����״̬��WY
			&& (StateEventFlag_C == STATE_EVENT_RUN_C) //C�ദ������״̬��WY
			&& (ESCFlagC.PWM_ins_index == 0)) //C����·����PWM����״̬��WY
	{
		UnCurrCompCnt = 0; //�ñ���δʹ�á�WY
		UnCurrCompFUN(); //������������������Чֵ����ƽ�⣬�ݴ��������ฺ�ص�ѹ����Чֵ��Ŀ��ֵ��WY
	}
	else
	{
		CurrentUnbalanceRegularVoltage[0] = ESCFlagA.Volttarget; //A�ฺ�ص�ѹ��Чֵ��Ŀ��ֵ��WY
		CurrentUnbalanceRegularVoltage[1] = ESCFlagB.Volttarget; //B�ฺ�ص�ѹ��Чֵ��Ŀ��ֵ��WY
		CurrentUnbalanceRegularVoltage[2] = ESCFlagC.Volttarget; //C�ฺ�ص�ѹ��Чֵ��Ŀ��ֵ��WY
	}

	/*����A��*/
	if (ESCFlagA.PWM_ins_index != 0) //A����·����PWMֱͨ״̬��WY
	{
		EPwm3Regs.CMPA.bit.CMPA = T1PR;//1��ֱͨ
		EPwm3Regs.CMPB.bit.CMPB = T1PR;//2�ܲ�ֱͨ
		EPwm4Regs.CMPA.bit.CMPA = T1PR;//3��ֱͨ
		EPwm4Regs.CMPB.bit.CMPB = T1PR;//4�ܲ�ֱͨ

		PIVolA.i10 = 0;//���PI�ı���ֵ
		PIVolA.i6 = 0;

		GridCurrPICtrlA.i10 = 0;
		GridCurrPICtrlA.i6 = 0;

		ConstantCurr[0].state = SM_CONSTANT_CURR_STANDBY; //��ʼ��״̬����WY

		ESC_FeedForward_DutyA = 1; //A��PWMռ�ձ�ǰ��ֵ��WY
	}
	else if ((StateEventFlag_A == STATE_EVENT_RUN_A) //A�ദ������״̬��WY
			&& (ESCFlagA.PWM_ins_index == 0)) //A����·����PWM����״̬��WY
	{
		switch (StateFlag.VoltageMode) //ѡ����ģʽ��WY
		{
			case 0: //��ѹģʽ��WY
			{
				VolttargetCorrA = ConstantCurrFSM(&ConstantCurr[0], gridCurA_rms, GridCurrAF, CurrTargetTemper); //���ݵ�������״̬�����㸺�ص�ѹ����Чֵ��Ŀ��ֵ������ϵ����WY
				PIOutVoltValueA = DCL_runPI(&PIVolA, (CurrentUnbalanceRegularVoltage[0] * (1 + VolttargetCorrA)), VoltOutA_rms); //���PI�����������ֵ��WY
				ESCFlagA.ESC_DutyData = RMSDutyLimit(VoltInA_rms, VoltInAF, (CurrentUnbalanceRegularVoltage[0] * (1 + VolttargetCorrA)), PIOutVoltValueA,
														Esc_VoltPhaseA, &ESC_FeedForward_DutyA, ESCFlagA.PHASE);
			}
				break;

			case 1: //��ѹģʽ��WY
			{
				PIOutVoltValueA = DCL_runPI(&PIVolA, ESCFlagA.Volttarget, VoltOutA_rms);
				ESCFlagA.ESC_DutyData = RMSDutyLimit(VoltInA_rms, VoltInAF, ESCFlagA.Volttarget, PIOutVoltValueA, Esc_VoltPhaseA, &ESC_FeedForward_DutyA,
														ESCFlagA.PHASE);
			}
				break;

			case 2: //����ѹģʽ��WY
				break;

			case 3: //��ƽ��ģʽ��WY
			{
				VolttargetCorrA = ConstantCurrFSM(&ConstantCurr[0], gridCurA_rms, GridCurrAF, CurrTargetTemper);
				PIOutVoltValueA = DCL_runPI(&PIVolA, CurrentUnbalanceRegularVoltage[0] * (1 + VolttargetCorrA), VoltOutA_rms);
				ESCFlagA.ESC_DutyData = RMSDutyLimit(VoltInA_rms, VoltInAF, CurrentUnbalanceRegularVoltage[0], PIOutVoltValueA, Esc_VoltPhaseA,
														&ESC_FeedForward_DutyA, ESCFlagA.PHASE);
			}
				break;

			case 4: //�޹�����ģʽ��WY
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
		{//��ѹ���벨   UAIP+/UAIN+Ϊ1ʱ,�޻����߼�  //Ӳ���¼ӻ����߼��жϵĵ�ѹֵ����������ĵ�ѹֵ����λ�����Լ200us,���������ΧֵҪ���һЩ,��Ӳ���жϵ�ѹֵ����ȥ.
			EPwm3Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataA;//2
			EPwm3Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataA;//1,buck��
			EPwm4Regs.CMPB.bit.CMPB = 0;//4,��ͨ,������
			EPwm4Regs.CMPA.bit.CMPA = T1PR;//3,��ͨ,������       //3,����
		}
		else if (VoltInAF <= (-35))
		{//��ѹ���벨
			EPwm3Regs.CMPB.bit.CMPB = 0;//2,��ͨ,������
			EPwm3Regs.CMPA.bit.CMPA = T1PR;//1,��ͨ������        //1,��
			EPwm4Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataA;//4
			EPwm4Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataA;//3,buck��
		}
		else
		{
			if ((VoltInAF >= 0) && (VoltInAF < 35))
			{//NESPWS20-220524REV2���ʰ�--NPC3CA10020220518REV1�ϳɰ�
				if (GridCurrAF >= 0)
				{
					EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//1��
					EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//2��
					EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//3��
					EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//4��
				}
				else
				{
					EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//1��
					EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//2��
					EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//3��
					EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//4��
				}
			}
			else if ((VoltInAF < 0) && (VoltInAF > (-35)))
			{
				if (GridCurrAF >= 0)
				{
					EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//1��
					EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//2��
					EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//3��
					EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//4��
				}
				else
				{
					EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//1��
					EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc);//2��
					EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//3��
					EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc);//4��
				}
			}
		}
	}
	else //A�ദ�ڷ�����״̬��WY
	{
		PIVolA.i10 = 0;//���PI�ı���ֵ
		PIVolA.i6 = 0;
		GridCurrPICtrlA.i10 = 0;
		GridCurrPICtrlA.i6 = 0;
		ConstantCurr[0].state = SM_CONSTANT_CURR_STANDBY;
		EPwm3Regs.CMPA.bit.CMPA = T1PR;//1��  //����Ӳ���߼�����,CMPA,CMPB��������ֵ,�������ĸ�����ȫͨ.
		EPwm3Regs.CMPB.bit.CMPB = T1PR;//2��
		EPwm4Regs.CMPA.bit.CMPA = T1PR;//3��
		EPwm4Regs.CMPB.bit.CMPB = T1PR;//4��
		ESC_FeedForward_DutyA = 1;//����������,δ��������,Ӧ��1��Ҫ����ռ�ձ�,��ʼ״̬�������ó�100%ռ�ձ�
	}

	/*����B��*/
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

	/*����C��*/
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
 * ADC-D-1���жϷ�������WY
 * ˵�����ж�Ƶ��20K���ж�����50us��
 */
void ADCD1INT(void)
{
	FaultFastDetectInInt(); //���ٹ��ϼ�⡣WY
	VoltSlidPosCnt(); //�������Ҳ���WY
	GenModulation(); //PWM���ơ�WY
	ADPosCnt(); //�������ġ�WY
	GetVolAndInvCurr(); //�����ѹ�͵�����WY

	switch (T1PRPwmFrequency)
	{
		case 0:
		{
			/*
			 * ���ս��ģ����ִ�в�ͬ���ܵĺ�����WY
			 * Ŀ�ģ����ͺ����ĵ���Ƶ�ʣ�ѹ��AD�жϷ�������ִ��ʱ�䡣
			 */
			switch (ADBufPos)
			{
				case 0:
				{
					SetHeatPulse(); //����DSP�������塣WY
					PLLrun(); //����  10K
					SigPhDQComput();//�޹����� 10k
				}
					break;

				case 1:
				{
					Swi_post(RMSstart); //�ͷ�RTOS�ź�����WY
					RmsCalcIn();//10K
					FirstRmsCalc();//5K
				}
					break;

				case 2:
				{
					StateFeedBackJudge();
					SetHeatPulse(); //����DSP�������塣WY
					PLLrun();//����
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
					SetHeatPulse(); //����DSP�������塣WY
					PLLrun();//����
					SigPhDQComput();
				}
					break;

				case 5:
				{
					Swi_post(RMSstart); //�ͷ�RTOS�ź�����WY
					FirstRmsCalc();//
					RmsCalcIn();
				}
					break;

				case 6:
				{
					SetHeatPulse(); //����DSP�������塣WY
					PLLrun();//����
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

	FaultRecordProg(); //����¼����WY
	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //���ADC-D-1���жϱ�־λ��WY
}

void RMSswi(void)
{
    FaultDetectInInt();             //0.61us
    FaultRecordSel();
    if (StateFlag.onceTimeAdcAutoAdjust)
    {    //�ϵ���Զ�����һ��
        AdRegeditOffset();
    }
}


