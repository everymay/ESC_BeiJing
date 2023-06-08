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
int ADBufPos=0;

Uint16 FFTSrcBufp=0;
Uint16 ADGROUP_NUM=8;
Uint16 ADGROUP_MODE=2;
Uint16 WAVE_REC_FFT_MODE_DATA2=4;
//Uint16 debugPIVolChSel= 0;
//Uint16 flagA,flagB,flagC;
//float ESC_DutyDataA = 1.0f,ESC_DutyDataB = 1.0f,ESC_DutyDataC = 1.0f;

float VoltSlid[VOLT_FUNDPOINT][3];
float VoltSlidA[MEANPOINTHALF],VoltSlidB[MEANPOINTHALF],VoltSlidC[MEANPOINTHALF];
float GridCurrSlidA[MEANPOINTHALF],GridCurrSlidB[MEANPOINTHALF],GridCurrSlidC[MEANPOINTHALF];
float GridCurrAF_Beta,GridCurrBF_Beta,GridCurrCF_Beta;
float GridRealCurDA2,GridRealCurDB2,GridRealCurDC2,GridRealCurQA2,GridRealCurQB2,GridRealCurQC2;
float GridRealCurErrA,GridRealCurErrB,GridRealCurErrC;
//float VoltSlidU[MEANPOINTHALF],VoltSlidV[MEANPOINTHALF],VoltSlidW[MEANPOINTHALF];
//float GcurSlidA[MEANPOINT_QUARTER],GcurSlidB[MEANPOINT_QUARTER],GcurSlidC[MEANPOINT_QUARTER];
//float LoadSlidA[MEANPOINT_QUARTER],LoadSlidB[MEANPOINT_QUARTER],LoadSlidC[MEANPOINT_QUARTER];
//float IverSlidA[MEANPOINT_QUARTER],IverSlidB[MEANPOINT_QUARTER],IverSlidC[MEANPOINT_QUARTER];
extern const Swi_Handle RMSstart;
//int16 ZCPJudgeA(float VoltIn,float VoltOut,float Esc_Phase,float CurrIn,Uint16 TBCTR);

#if TESTMODE
    float32 DbgStepPhaA=D2R(0),DbgStepPhaB=D2R(240),DbgStepPhaC=D2R(120);
#endif

//#pragma CODE_SECTION(PRController           ,"ram2func")
#pragma	CODE_SECTION(ADCD1INT               ,"ram2func")
#if PWM_FREQUENCE_16KHZ
#pragma CODE_SECTION(PIController           ,"ram2func")
#endif
//#pragma	CODE_SECTION(StartCaluFFT           ,"ram2func")
//#pragma	CODE_SECTION(CurrRefCaul            ,"ram2func")
#pragma	CODE_SECTION(RMSDutyLimit            ,"ram2func")
#pragma CODE_SECTION(ReactivePowerComFUN     ,"ram2func")
#pragma CODE_SECTION(UnCurrCompFUN           ,"ram2func")
//#pragma	CODE_SECTION(DcVoltDiv2Loop         ,"ram2func")
#pragma	CODE_SECTION(FaultRecordSel       ,"ram2func")
#pragma CODE_SECTION(AdRegeditOffset       ,"ram2func")
//#pragma CODE_SECTION(testFunction           ,"ram2func")
//#pragma	CODE_SECTION(getFFTData             ,"ram2func")
//#pragma CODE_SECTION(InverseTransformF      ,"ram2func")
#pragma CODE_SECTION(RMSswi                 ,"ram2func")
//#pragma   CODE_SECTION(GenModulation      ,"ram2func")
//#pragma   CODE_SECTION(GetFFTData         ,"ram2func")
//#pragma   CODE_SECTION(GetVolAndInvCurr   ,"ram2func")
//#pragma   CODE_SECTION(GetGridLoadcurr    ,"ram2func")
//#pragma   CODE_SECTION(GetUdc               ,"ram2func")
//#pragma   CODE_SECTION(GetGridLoadcurrAD    ,"ram2func")
//#pragma   CODE_SECTION(GetGridLoadcurr      ,"ram2func")
//#pragma   CODE_SECTION(ADPosCnt           ,"ram2func")
//int ZCPJudgeA_ovsample(volatile Uint16 *ADResult,int ADoffset,int NumPort);

//inline void SpwmGeneration(float sa,float sb,float sc);
Uint16 MeanHalfPos = 0;
inline void FaultRecordSel(void);

//inline void StartCaluFFT()
//{
//    DMACH1ConfigAndRun(&ADBuff[FFTCalcChan][0],FFTSrcBufp); //start FFT
//}

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

//float inBuff[10],outBuff[10];
inline void GetVolAndInvCurr(void)                  //获得电压和逆变侧电流
{
#if TEST_VIRTUALSOURCE == 0
    VirtulADStruVAL *pAD = &VirtulADVAL;
    VirtulADStruval *pID = &VirtulADval;            //零偏初始值
    float Buff[15];
    float *pBuff=Buff;
    if(StateFlag.VoltageModeFlag == 1){             //降压
        *pBuff++ = (*pAD->GridHVoltA      - pID->gridHVoltA)*gridVoltRatio;          //电网电压,电压采样系数        //电网电压采样点(运放输出点)与实际电压波形方向反向
        *pBuff++ = (*pAD->GridHVoltB      - pID->gridHVoltB)*gridVoltRatio;
        *pBuff++ = (*pAD->GridHVoltC      - pID->gridHVoltC)*gridVoltRatio;
        *pBuff++ = (*pAD->GridLVoltA      - pID->gridLVoltA)*loadVoltRatio;          //负载电压,电压采样系数        //负载电压采样点(运放输出点)与实际电压波形方向反向
        *pBuff++ = (*pAD->GridLVoltB      - pID->gridLVoltB)*loadVoltRatio;
        *pBuff++ = (*pAD->GridLVoltC      - pID->gridLVoltC)*loadVoltRatio;
        *pBuff++ = (*pAD->GridMainCurA    - pID->gridMainCurA)*outputCurRatioCurrA;     //电抗主电流,输出电流采样系数   //主电抗电流采样点(运放输出点)与实际电流波形方向相反(继电器板霍尔采样反了)
        *pBuff++ = (*pAD->GridMainCurB    - pID->gridMainCurB)*outputCurRatioCurrB;     //A相电流方向与B,C相电流方向相反,需要解决?????--LJH
        *pBuff++ = (*pAD->GridMainCurC    - pID->gridMainCurC)*outputCurRatioCurrC;
        *pBuff++ = (*pAD->GridBypassCurA  - pID->gridBypassCurA)*outputCurBypassCurrA;     //旁路电流,输出电流采样系数   //旁路电流采样点(运放输出点)与实际电流波形方向相反(需验证???)
        *pBuff++ = (*pAD->GridBypassCurB  - pID->gridBypassCurB)*outputCurBypassCurrB;     //A相电流方向与B,C相电流方向相反,需要解决?????--LJH
        *pBuff++ = (*pAD->GridBypassCurC  - pID->gridBypassCurC)*outputCurBypassCurrC;
        *pBuff++ = (*pAD->ADCUDCA         - pID->aDCUDCA)*dcCapVoltRatio;                  //直流电容电压,电压采样系数
        *pBuff++ = (*pAD->ADCUDCB         - pID->aDCUDCB)*dcCapVoltRatio;
        *pBuff++ = (*pAD->ADCUDCC         - pID->aDCUDCC)*dcCapVoltRatio;
    }else if(StateFlag.VoltageModeFlag == 0){       //升压
        *pBuff++ = (*pAD->GridLVoltA      - pID->gridLVoltA)*loadVoltRatio;          //负载电压,电压采样系数        //负载电压采样点(运放输出点)与实际电压波形方向反向
        *pBuff++ = (*pAD->GridLVoltB      - pID->gridLVoltB)*loadVoltRatio;
        *pBuff++ = (*pAD->GridLVoltC      - pID->gridLVoltC)*loadVoltRatio;
        *pBuff++ = (*pAD->GridHVoltA      - pID->gridHVoltA)*gridVoltRatio;          //电网电压,电压采样系数        //电网电压采样点(运放输出点)与实际电压波形方向反向
        *pBuff++ = (*pAD->GridHVoltB      - pID->gridHVoltB)*gridVoltRatio;
        *pBuff++ = (*pAD->GridHVoltC      - pID->gridHVoltC)*gridVoltRatio;
        *pBuff++ = (*pAD->GridMainCurA    - pID->gridMainCurA)*outputCurRatioCurrA;     //电抗主电流,输出电流采样系数   //主电抗电流采样点(运放输出点)与实际电流波形方向相反(继电器板霍尔采样反了)
        *pBuff++ = (*pAD->GridMainCurB    - pID->gridMainCurB)*outputCurRatioCurrB;     //A相电流方向与B,C相电流方向相反,需要解决?????--LJH
        *pBuff++ = (*pAD->GridMainCurC    - pID->gridMainCurC)*outputCurRatioCurrC;
        *pBuff++ = (*pAD->GridBypassCurA  - pID->gridBypassCurA)*outputCurBypassCurrA;     //旁路电流,输出电流采样系数   //旁路电流采样点(运放输出点)与实际电流波形方向相反(需验证???)
        *pBuff++ = (*pAD->GridBypassCurB  - pID->gridBypassCurB)*outputCurBypassCurrB;     //A相电流方向与B,C相电流方向相反,需要解决?????--LJH
        *pBuff++ = (*pAD->GridBypassCurC  - pID->gridBypassCurC)*outputCurBypassCurrC;
        *pBuff++ = (*pAD->ADCUDCA         - pID->aDCUDCA)*dcCapVoltRatio;                  //直流电容电压,电压采样系数
        *pBuff++ = (*pAD->ADCUDCB         - pID->aDCUDCB)*dcCapVoltRatio;
        *pBuff++ = (*pAD->ADCUDCC         - pID->aDCUDCC)*dcCapVoltRatio;
    }

    DCL_runDF22Group(VolAndInvCurrFilter,Buff,Buff,15);       //滤波

    pBuff=Buff;
    float *pOutSrc = VoltSlid[VoltPrvPos];  //实时采样波形
    if(StateFlag.VoltageModeFlag == 1){
        *pOutSrc++ = *pBuff++;
        *pOutSrc++ = *pBuff++;
        *pOutSrc++ = *pBuff++;
        pOutSrc = VoltSlid[VoltPos];     //进行电压校正
        GridVoltAF = *pOutSrc++;         //电网电压
        GridVoltBF = *pOutSrc++;
        GridVoltCF = *pOutSrc++;
        LoadVoltUF = *pBuff++;           //负载电压
        LoadVoltVF = *pBuff++;
        LoadVoltWF = *pBuff++;
    }else if(StateFlag.VoltageModeFlag == 0){
        *pOutSrc++ = *pBuff++;
        *pOutSrc++ = *pBuff++;
        *pOutSrc++ = *pBuff++;
        pOutSrc = VoltSlid[VoltPos];     //进行电压校正
        LoadVoltUF = *pOutSrc++;         //电网电压
        LoadVoltVF = *pOutSrc++;
        LoadVoltWF = *pOutSrc++;
        GridVoltAF = *pBuff++;           //负载电压
        GridVoltBF = *pBuff++;
        GridVoltCF = *pBuff++;
    }
    GridCurrAF = *pBuff++;     //电抗主电流
    GridCurrBF = *pBuff++;
    GridCurrCF = *pBuff++;
    GridBPCurrAF = *pBuff++;   //旁路电流
    GridBPCurrBF = *pBuff++;
    GridBPCurrCF = *pBuff++;
    DccapVoltA = *pBuff++;     //直流电容电压
    DccapVoltB = *pBuff++;
    DccapVoltC = *pBuff;

    if(StateFlag.VoltageModeFlag == 1){ //降压
        VoltInAF = GridVoltAF;   VoltInBF = GridVoltBF;   VoltInCF = GridVoltCF;
        VoltOutAF = LoadVoltUF;  VoltOutBF = LoadVoltVF;  VoltOutCF = LoadVoltWF;
    } else if(StateFlag.VoltageModeFlag == 0){   //升压
        VoltInAF = LoadVoltUF;   VoltInBF = LoadVoltVF;   VoltInCF = LoadVoltWF;
        VoltOutAF = GridVoltAF;  VoltOutBF = GridVoltBF;  VoltOutCF = GridVoltCF;
    }

    //延时90度构造Beta    单相锁相

    VoltSlidA[MeanHalfPos]  = VoltInAF;                //grid real value
    VoltSlidB[MeanHalfPos]  = VoltInBF;
    VoltSlidC[MeanHalfPos]  = VoltInCF;
    GridCurrSlidA[MeanHalfPos] = GridCurrAF;
    GridCurrSlidB[MeanHalfPos] = GridCurrBF;
    GridCurrSlidC[MeanHalfPos] = GridCurrCF;

    if(MeanHalfPos == (MEANPOINTHALF - 1)){
        VoltInAF_Beta=VoltSlidA[0];
        VoltInBF_Beta=VoltSlidB[0];
        VoltInCF_Beta=VoltSlidC[0];
        GridCurrAF_Beta = GridCurrSlidA[0];
        GridCurrBF_Beta = GridCurrSlidB[0];
        GridCurrCF_Beta = GridCurrSlidC[0];

    }else{
        VoltInAF_Beta=VoltSlidA[MeanHalfPos+1];
        VoltInBF_Beta=VoltSlidB[MeanHalfPos+1];
        VoltInCF_Beta=VoltSlidC[MeanHalfPos+1];
        GridCurrAF_Beta = GridCurrSlidA[MeanHalfPos+1];
        GridCurrBF_Beta = GridCurrSlidB[MeanHalfPos+1];
        GridCurrCF_Beta = GridCurrSlidC[MeanHalfPos+1];
    }
    if(++MeanHalfPos >= MEANPOINTHALF)   MeanHalfPos = 0;

#if 0
    VirtulADStru *pAD = &VirtulAD;
    float sumApfOut=0,tmp1,tmp2,tmp3;
    float Buff[10];
    float *pBuff=Buff;

    *pBuff++ =        (*pAD->GridVolt1 - pAD->gridVoltAB)*gridVoltRatio;          //filter(GridVoltAF)
    *pBuff++ =        (*pAD->GridVolt2 - pAD->gridVoltBC)*gridVoltRatio;
    *pBuff++ =        (*pAD->GridVolt3 - pAD->gridVoltCA)*gridVoltRatio;
    *pBuff++ = tmp1 = (*pAD->ApfOutCur1 - pAD->apfOutputCurA)*outputCurRatio;     //filter(ApfOutCurA)
    *pBuff++ = tmp2 = (*pAD->ApfOutCur2 - pAD->apfOutputCurB)*outputCurRatio;
    *pBuff++ = tmp3 = (*pAD->ApfOutCur3 - pAD->apfOutputCurC)*outputCurRatio;
    sumApfOut += *pBuff++ = tmp1;         //filter(ApfOutCurAD)
    sumApfOut += *pBuff++ = tmp2;
    sumApfOut += *pBuff++ = tmp3;
    *pBuff++ = sumApfOut;                 //filter(ResonProtcABC)

    DCL_runDF22Group(VolAndInvCurrFilter,Buff,Buff,10);

    pBuff=Buff;
    float *pOutSrc = VoltSlid[VoltPrvPos];  //实时采样波形
    *pOutSrc++ = *pBuff++;
    *pOutSrc++ = *pBuff++;
    *pOutSrc++ = *pBuff++;
    if(!StateFlag.VoltFilterEn)    //用上周波值校正电网电压滞后的相差//是否需要电压校正?//    if(!(CLA_StateFlagm&CAPGROUPSWITCH_CAPVARUNSEAL)){
        pOutSrc=  VoltSlid[VoltPos];     //进行电压校正
    else                            //做电流前馈时,打算用dq校正电网电压滞后的相差,此时未校正
        pOutSrc=  VoltSlid[VoltPrvPos];  //不进行校正,直接将电压实时采样给 pOutSrc

    GridVoltAF = *pOutSrc++;
    GridVoltBF = *pOutSrc++;
    GridVoltCF = *pOutSrc++;
    ApfOutCurA = *pBuff++;
    ApfOutCurB = *pBuff++;
    ApfOutCurC = *pBuff++;
    ApfOutCurAD= *pBuff++;
    ApfOutCurBD= *pBuff++;
    ApfOutCurCD= *pBuff++;
    ResonProtcABC=  DCL_runDF22(&ResonancePortectABC[1],DCL_runDF22(&ResonancePortectABC[0],*pBuff)); //谐振检测
//    ApfOutCurA = tmp1;//使用滤波前的电流做运算
//    ApfOutCurB = tmp2;
//    ApfOutCurC = tmp3;
#endif

#if TEST_DEBUGFFT
    if(!StateFlag.VoltFilterEn)
        pOutSrc = VoltSlid[VoltPos];
    else
        pOutSrc=  VoltSlid[VoltPrvPos];
    DbgStepPhaA=PhaseLimit(DbgStepPhaA + PI2/FUNDPOINT);
    DbgStepPhaB=PhaseLimit(DbgStepPhaB + PI2/FUNDPOINT);
    DbgStepPhaC=PhaseLimit(DbgStepPhaC + PI2/FUNDPOINT);
    *pOutSrc++=GridVoltAF= cosf(DbgStepPhaA) *311+xrand;
    *pOutSrc++=GridVoltBF= cosf(DbgStepPhaB) *10-xrand;
    *pOutSrc++=GridVoltCF= cosf(DbgStepPhaC) *1+xrand*0.5;

    TestADTempBufp=SPLL.Theta*(GEN_FUNDPOINT/PI2);
    ApfOutCurA=ApfOutCurAD=TestLoadCurS(0);
    ApfOutCurB=ApfOutCurBD=TestLoadCurS(1);
    ApfOutCurC=ApfOutCurCD=TestLoadCurS(2);
#endif

#else
    int16 xPhaseA,xPhaseB,xPhaseC,xPhaseA1,xPhaseB1,xPhaseC1,xPhaseA2,xPhaseB2,xPhaseC2;

    DbgStepPhaA+=PI2/FUNDPOINT;
    PhaseLimit(DbgStepPhaA);
    DbgStepPhaB+=PI2/FUNDPOINT;
    PhaseLimit(DbgStepPhaB);
    DbgStepPhaC+=PI2/FUNDPOINT;
    PhaseLimit(DbgStepPhaC);
    //To calculate the fundamental power
    xPhaseA=PhaseLimitI( (DbgStepPhaA )*(PI2_SINE_LOOKTABLE/PI2) );
    xPhaseB=PhaseLimitI( (DbgStepPhaB )*(PI2_SINE_LOOKTABLE/PI2) );
    xPhaseC=PhaseLimitI( (DbgStepPhaC )*(PI2_SINE_LOOKTABLE/PI2) );
    xPhaseA1=PhaseLimitI( (DbgStepPhaA*3 )*(PI2_SINE_LOOKTABLE/PI2) );  //3次谐波
    xPhaseB1=PhaseLimitI( (DbgStepPhaB*3 )*(PI2_SINE_LOOKTABLE/PI2) );
    xPhaseC1=PhaseLimitI( (DbgStepPhaC*3 )*(PI2_SINE_LOOKTABLE/PI2) );
    xPhaseA2=PhaseLimitI( (DbgStepPhaA*5 )*(PI2_SINE_LOOKTABLE/PI2) );  //5次谐波
    xPhaseB2=PhaseLimitI( (DbgStepPhaB*5 )*(PI2_SINE_LOOKTABLE/PI2) );
    xPhaseC2=PhaseLimitI( (DbgStepPhaC*5 )*(PI2_SINE_LOOKTABLE/PI2) );

    GridVoltAF= pSineLookTab(xPhaseA) *311 + pSineLookTab(xPhaseA1) *(311*0.2) + pSineLookTab(xPhaseA2) *(311*0.3); //matrix realA*realB-imagA*imagB
//  if(debugUnbVol==0){
        GridVoltBF= pSineLookTab(xPhaseB) *311 + pSineLookTab(xPhaseB1) *(311*0.2) + pSineLookTab(xPhaseB2) *(311*0.3);
        GridVoltCF= pSineLookTab(xPhaseC) *311 + pSineLookTab(xPhaseC1) *(311*0.2) + pSineLookTab(xPhaseC2) *(311*0.3);
//  }else if(debugUnbVol==1){
//      GridVoltBF= pSineLookTab(xPhaseB) *311 + pSineLookTab(xPhaseB1) *(311*0.2) + pSineLookTab(xPhaseB2) *(311*0.3);
//      GridVoltCF=0;
//  }else{
//      GridVoltBF=0;
//      GridVoltCF=0;
//  }

    ApfOutCurA=TestLoadCurS(0);
    ApfOutCurB=TestLoadCurS(1);
    ApfOutCurC=TestLoadCurS(2);
#endif

#if TEST_NULLPULSE

    float testDcVoltF;
    static float32 DbgStepPhaA=D2R(0),DbgStepPhaB=D2R(240),DbgStepPhaC=D2R(120);
    if(dcVoltF<10)
        testDcVoltF=10;
    else
        testDcVoltF=dcVoltF;

//    DbgStepPhaA=PhaseLimit(GridVoltTheta);
//    DbgStepPhaB=PhaseLimit(GridVoltTheta+D2R(240));
//    DbgStepPhaC=PhaseLimit(GridVoltTheta+D2R(120));
    DbgStepPhaA+=PI2/(2*PWMFREQUENCY/50);
    DbgStepPhaA=PhaseLimit(DbgStepPhaA);
    DbgStepPhaB+=PI2/(2*PWMFREQUENCY/50);
    DbgStepPhaB=PhaseLimit(DbgStepPhaB);
    DbgStepPhaC+=PI2/(2*PWMFREQUENCY/50);
    DbgStepPhaC=PhaseLimit(DbgStepPhaC);
    GridVoltAF= cosf(DbgStepPhaA) *312;//testDcVoltF*0.48f;   //matrix realA*realB-imagA*imagB
    GridVoltBF= cosf(DbgStepPhaB) *312;//testDcVoltF*0.48f;
    GridVoltCF= cosf(DbgStepPhaC) *312;//testDcVoltF*0.48f;

#endif  //endif TEST_NULLPULSE

#if TEST_RUNTIME
    StateEventFlag = STATE_EVENT_RUN;   //测试区域
#endif
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
//    float32 dq2Ualpha,dq2Ubeta,tmpSin,tmpCos;
//    float32 volFundA,volFundB,volFundC;
//    //Negative inverse transform
//    if(StateFlag.negCurCompFlag == 1){
// 		DQ2UALPHA_NEG(negCurCompD,negCurCompQ);
// 		ICLACK(CurFundNgA,CurFundNgB,CurFundNgC,LoadFundaCurZ);
//    }else{
// 		CurFundNgA=0;CurFundNgB=0;CurFundNgC=0;
//    }
//    if(!StateFlag.negCurCompFlag){
//        if(StateFlag.VolSurTimeFlag)//电压补偿（Q无功补偿）
//        {
//            dq2Ualpha = currentRefD  * GridResCos - currentRefQa * GridResSin;
//            dq2Ubeta =  currentRefD  * GridResSin + currentRefQa * GridResCos;
//            CurFundA = UdcBalanceCurr + dq2Ualpha * SQRT_2DIV3;
//
//            dq2Ualpha = currentRefD  * GridResCos - currentRefQb * GridResSin;
//            dq2Ubeta =  currentRefD  * GridResSin + currentRefQb * GridResCos;
//            CurFundB = UdcBalanceCurr + dq2Ualpha * -SQRT_2DIV3_DIV2 + dq2Ubeta * SQRT2_DIV2;
//
//            dq2Ualpha = currentRefD  * GridResCos - currentRefQc * GridResSin;
//            dq2Ubeta =  currentRefD  * GridResSin + currentRefQc * GridResCos;
//            CurFundC = UdcBalanceCurr + dq2Ualpha * -SQRT_2DIV3_DIV2 + dq2Ubeta * -SQRT2_DIV2;
//        }else{
//            tmpSin = GridResSin * I_ins_index * SQRT3;
//            tmpCos = GridResCos * I_ins_index * SQRT3;
//
//            dq2Ualpha = currentRefD * GridResCos - currentRefA * tmpSin;
//            dq2Ubeta =  currentRefD * GridResSin + currentRefA * tmpCos;
//            CurFundA = UdcBalanceCurr + dq2Ualpha * SQRT_2DIV3;
//
//            dq2Ualpha = currentRefD * GridResCos - currentRefB * tmpSin;
//            dq2Ubeta =  currentRefD * GridResSin + currentRefB * tmpCos;
//            CurFundB = UdcBalanceCurr + dq2Ualpha * -SQRT_2DIV3_DIV2 + dq2Ubeta * SQRT2_DIV2;
//
//            dq2Ualpha = currentRefD * GridResCos - currentRefC * tmpSin;
//            dq2Ubeta =  currentRefD * GridResSin + currentRefC * tmpCos;
//            CurFundC = UdcBalanceCurr + dq2Ualpha * -SQRT_2DIV3_DIV2 + dq2Ubeta * -SQRT2_DIV2;
//
//            //Positive inverse transform standby
//            dq2Ualpha = -currentRefA * GridResSin * SQRT3;
//            CurFundStandbyA = UdcBalanceCurr + dq2Ualpha * SQRT_2DIV3;
//            dq2Ualpha = -currentRefB * GridResSin * SQRT3;
//            dq2Ubeta =   currentRefB * GridResCos * SQRT3;
//            CurFundStandbyB = UdcBalanceCurr + dq2Ualpha * -SQRT_2DIV3_DIV2 + dq2Ubeta * SQRT2_DIV2;
//            dq2Ualpha = -currentRefC * GridResSin * SQRT3;
//            dq2Ubeta =   currentRefC * GridResCos * SQRT3;
//            CurFundStandbyC = UdcBalanceCurr + dq2Ualpha * -SQRT_2DIV3_DIV2 + dq2Ubeta * -SQRT2_DIV2;
//        }
//    }else{
//        //Positive inverse transform
//        dq2Ualpha = currentRefD * GridResCos - currentRefQ * GridResSin * I_ins_index;
//        dq2Ubeta =  currentRefQ * GridResCos * I_ins_index + currentRefD * GridResSin;
//    //  ICLACK(CurFundA,CurFundB,CurFundC,UdcBalanceCurr);
//        CurFundA = UdcBalanceCurr + dq2Ualpha * SQRT_2DIV3;
//        CurFundB = UdcBalanceCurr + dq2Ualpha *-SQRT_2DIV3_DIV2 + dq2Ubeta * SQRT2_DIV2;
//        CurFundC = UdcBalanceCurr + dq2Ualpha *-SQRT_2DIV3_DIV2 + dq2Ubeta *-SQRT2_DIV2;
//
//        //Positive inverse transform standby
//        dq2Ualpha = -currentRefQ * GridResSin;
//        dq2Ubeta =   currentRefQ * GridResCos;
//        ICLACK(CurFundStandbyA,CurFundStandbyB,CurFundStandbyC,UdcBalanceCurr);
//    }
//    //Inverter current positive inverse transform
//	dq2Ualpha =	-InvFundaCurQ * GridResCos - InvFundaCurD * GridResSin;		//DQ axis decoupling control
//	dq2Ubeta =   InvFundaCurD * GridResCos - InvFundaCurQ * GridResSin;
//
//    volFundA = Q2D_ratio*(dq2Ualpha* SQRT_2DIV3);
//    volFundB = Q2D_ratio*(dq2Ualpha*-SQRT_2DIV3_DIV2 + dq2Ubeta * SQRT2_DIV2);
//    volFundC = Q2D_ratio*(dq2Ualpha*-SQRT_2DIV3_DIV2 + dq2Ubeta *-SQRT2_DIV2);
//
//    //Inverter current negative inverse transform
//	dq2Ualpha = InvFundaCurNQ * GridResCos - InvFundaCurND * GridResSin;	//DQ axis decoupling control
//	dq2Ubeta = -InvFundaCurND * GridResCos - InvFundaCurNQ * GridResSin;
//
//    VolFundA = volFundA + Q2D_ratio * (dq2Ualpha * SQRT_2DIV3);
//    VolFundB = volFundB + Q2D_ratio * (dq2Ualpha *-SQRT_2DIV3_DIV2 + dq2Ubeta * SQRT2_DIV2);
//    VolFundC = volFundC + Q2D_ratio * (dq2Ualpha *-SQRT_2DIV3_DIV2 + dq2Ubeta *-SQRT2_DIV2);
//
////    PRController();
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
    #if TEST_NULLPULSE
        CurrRefA=0;CurrRefB=0;CurrRefC=0;
 //       ApfOutCurA=0;ApfOutCurB=0;ApfOutCurC=0;
    #endif
}
//只有在非6.4k的整倍数频率需要用到此函数
//用于计算非整采样点间的数据,现在为1/2
void GetGridLoadcurrAD(void)
{
//    VirtulADStru *pAD = &VirtulAD;
//
//    GridCurrPrvAD[0]=(*pAD->GridCurr1 - pAD->loadCurrentA);             //AD value
//    GridCurrPrvAD[2]=(*pAD->GridCurr3 - pAD->loadCurrentC);
//    GridCurrPrvAD[1]=(FactorySet.Infer.B.DefectCT) ? -(GridCurrPrvAD[0]+GridCurrPrvAD[2]) : (*pAD->GridCurr2 - pAD->loadCurrentB);//0:3个CT,!0:B相CT缺失
//    GridCurrPrvADFlag = true;
}

void GetGridLoadcurr(void)      //12.8k
{
//#if	TEST_VIRTUALSOURCE == 0
//    VirtulADStru *pAD = &VirtulAD;
//    float tmp1=(*pAD->GridCurr1 - pAD->loadCurrentA);               //AD value
//    float tmp3=(*pAD->GridCurr3 - pAD->loadCurrentC);
//    float tmp2=(FactorySet.Infer.B.DefectCT) ? -(tmp1+tmp3) : (*pAD->GridCurr2 - pAD->loadCurrentB);//0:3个CT,!0:B相CT缺失
//
//    if(GridCurrPrvADFlag){
//        GridCurrPrvADFlag = false;
//        tmp1=(GridCurrPrvAD[0]+tmp1)*0.5;
//        tmp3=(GridCurrPrvAD[2]+tmp3)*0.5;
//        tmp2=(GridCurrPrvAD[1]+tmp2)*0.5;
//    }
//
//	if(!StateFlag.positionCT){				//电流在电网侧取样
//		if(StateFlag.EnTransformRatioMode){												//此系数为0关闭高压侧补偿
//            GridCurrHvA = tmp1*pAD->LoadCurRatioA;                  //高压侧实测网侧电流
//            GridCurrHvC = tmp3*pAD->LoadCurRatioC;
//            GridCurrHvB = tmp2*pAD->LoadCurRatioB;
//			switch(StateFlag.TransformRatioMode){
//            case 1://Dyn11
//            case 3://YNd11
//                GridCurrA= (GridCurrHvA - GridCurrHvB)*TransformCurrH2L;//(A-B)         //Dyn11方式,折算到低压侧APF的输出端.
//                GridCurrB= (GridCurrHvB - GridCurrHvC)*TransformCurrH2L;//(B-C)         //Dyn11
//                GridCurrC= (GridCurrHvC - GridCurrHvA)*TransformCurrH2L;//(C-A)         //Dyn11
//                break;
//            case 0: //Yyn0
//                GridCurrA= (GridCurrHvA )*TransformCurrH2L;                     //Yyn0方式,折算到低压侧APF的输出端.
//                GridCurrB= (GridCurrHvB )*TransformCurrH2L;                     //
//                GridCurrC= (GridCurrHvC )*TransformCurrH2L;                     //
//                break;
//            case 2://Dyn1
//                GridCurrA= (GridCurrHvA - GridCurrHvC)*TransformCurrH2L;//(A-C)         //Dyn1
//                GridCurrB= (GridCurrHvB - GridCurrHvA)*TransformCurrH2L;//(B-A)         //
//                GridCurrC= (GridCurrHvC - GridCurrHvB)*TransformCurrH2L;//(C-B)         //
//                break;
//            }
//        }else{
//            GridCurrA = tmp1*pAD->LoadCurRatioA;
//            GridCurrC = tmp3*pAD->LoadCurRatioC;
//            GridCurrB = tmp2*pAD->LoadCurRatioB;
//        }
//
//		if(StateFlag.SequenceAutoFlag){//此处是因为FFT分析的是负载电流
//            LoadRealCurA = GridCurrA;
//            LoadRealCurB = GridCurrB;
//            LoadRealCurC = GridCurrC;
//        }else{
//            LoadRealCurA= (GridCurrA - ApfOutCurA*MU_LCD_RATIO);  //loadRealCurA,因为延时不一样,得做校正
//            LoadRealCurB= (GridCurrB - ApfOutCurB*MU_LCD_RATIO);  //loadRealCurB
//            LoadRealCurC= (GridCurrC - ApfOutCurC*MU_LCD_RATIO);  //loadRealCurC
//        }
//    }else{  //电流在负载侧取样
//		if(StateFlag.EnTransformRatioMode){												//此系数为0关闭高压侧补偿
//            LoadRealCurHvA  = tmp1*pAD->LoadCurRatioA;                  //高压侧实测负载侧电流
//            LoadRealCurHvB  = tmp2*pAD->LoadCurRatioB;
//            LoadRealCurHvC  = tmp3*pAD->LoadCurRatioC;
//			switch(StateFlag.TransformRatioMode){
//            case 1://Dyn11
//            case 3://YNd11
//                LoadRealCurA= (LoadRealCurHvA - LoadRealCurHvB)*TransformCurrH2L;//(A-B)            //Dyn11方式,折算到低压侧APF的输出端.
//                LoadRealCurB= (LoadRealCurHvB - LoadRealCurHvC)*TransformCurrH2L;//(B-C)            //Dyn11
//                LoadRealCurC= (LoadRealCurHvC - LoadRealCurHvA)*TransformCurrH2L;//(C-A)            //Dyn11
//                break;
//            case 0: //Yyn0
//                LoadRealCurA= (LoadRealCurHvA )*TransformCurrH2L;                       //Yyn0方式,折算到低压侧APF的输出端.
//                LoadRealCurB= (LoadRealCurHvB )*TransformCurrH2L;                       //
//                LoadRealCurC= (LoadRealCurHvC )*TransformCurrH2L;                       //
//                break;
//            case 2://Dyn1
//                LoadRealCurA= (LoadRealCurHvA - LoadRealCurHvC)*TransformCurrH2L;//(A-C)            //Dyn1
//                LoadRealCurB= (LoadRealCurHvB - LoadRealCurHvA)*TransformCurrH2L;//(B-A)            //
//                LoadRealCurC= (LoadRealCurHvC - LoadRealCurHvB)*TransformCurrH2L;//(C-B)            //
//                break;
//            }
//        }else{
//			if(CapGroupAct.capgroupswitch.B.CapVarUnseal){			//使能电容器组投切
//                AllCapCapacityPhA=AllCapCapacityPhB=AllCapCapacityPhC=0;
//            }
//
////		    float *pPrvVolt     = VoltSlid[CapVoltPosCur];
////
////            // I = U / Z = 2πfCU//(51 = 1/(250x250x314x1e-9))
////            RUICA = AllCapCapacityPhA * *pPrvVolt++ * CAP_LOAD_RATIO;
////            RUICB = AllCapCapacityPhB * *pPrvVolt++ * CAP_LOAD_RATIO;
////            RUICC = AllCapCapacityPhC * *pPrvVolt++ * CAP_LOAD_RATIO;
//            LoadRealCurA = tmp1*pAD->LoadCurRatioA + RUICA;
//            LoadRealCurC = tmp3*pAD->LoadCurRatioC + RUICC;
//            LoadRealCurB = tmp2*pAD->LoadCurRatioB + RUICB;
//        }
//        GridCurrA = (LoadRealCurA + ApfOutCurA*MU_LCD_RATIO);
//        GridCurrB = (LoadRealCurB + ApfOutCurB*MU_LCD_RATIO);
//        GridCurrC = (LoadRealCurC + ApfOutCurC*MU_LCD_RATIO);
//    }
//    //JCLDBG
////  int16 xPhaseA,xPhaseA1,xPhaseA2;
////  static float32 DbgStepPhaA=D2R(0);
//
////  DbgStepPhaA+=PI2/FUNDPOINT;
////  CLAPhaseLimit(DbgStepPhaA);
////  //To calculate the fundamental power
////  xPhaseA =PhaseLimitI( (DbgStepPhaA*5  )*(PI2_SINE_LOOKTABLE/PI2) );
////  xPhaseA1=PhaseLimitI( (DbgStepPhaA*7  )*(PI2_SINE_LOOKTABLE/PI2) ); //3次谐波
////  xPhaseA2=PhaseLimitI( (DbgStepPhaA*19 )*(PI2_SINE_LOOKTABLE/PI2) ); //5次谐波
////
////  LoadRealCurC=LoadRealCurB=LoadRealCurA=\
////  pSineLookTab(xPhaseA) *100 + pSineLookTab(xPhaseA1) *(100*0.5) + pSineLookTab(xPhaseA2) *(100*0.25);    //matrix realA*realB-imagA*imagB
//
//#else
//	LoadRealCurA=TestLoadCurS(0);
//	LoadRealCurB=TestLoadCurS(1);
//	LoadRealCurC=TestLoadCurS(2);
//	GridCurrA=TestLoadCurS(0);
//	GridCurrB=TestLoadCurS(1);
//	GridCurrC=TestLoadCurS(2);
//#endif
//#if TEST_DEBUGFFT
//    GridCurrA=LoadRealCurA=TestLoadCurS(0);
//    GridCurrA=LoadRealCurB=TestLoadCurS(1);
//    GridCurrA=LoadRealCurC=TestLoadCurS(2);
//#endif
}

void GetUdc(void)
{
//#if TEST_VIRTUALSOURCE == 0
//    VirtulADStru *pAD = &VirtulAD;
//
//    dcVoltUp    = (ADC_RU_UDCUP - pAD->dcBusVoltUp)*(dcVoltUpRatio);    //dcVoltUp - 0.000586
//    dcVoltDn    = (ADC_RU_UDCDN - pAD->dcBusVoltDn)*(dcVoltDnRatio);    //dcVsoltDn - 0.000586
//    // 直流侧采样系数 2.67/940， 3V可以采集到1056，软件采样变换系数为1056.18/4096 = 0.257856
//    dcVoltUpF   =DCL_runDF22(&UdcUpFilter, dcVoltUp);   //sampling frequency 16kHz
//    dcVoltDnF   =DCL_runDF22(&UdcDnFilter, dcVoltDn);
//    dcVoltF=dcVoltUpF + dcVoltDnF;
//#if TEST_NULLPULSE
//    if(dcVoltF<10) dcVoltF=10;
//#endif
//    //-dcVoltF  =DCL_runDF22(&UdcFilter, dcVoltUp + dcVoltDn);
//#else
//    dcVoltDnF=dcVoltUpF=dcVoltUp=dcVoltDn=750*0.5f;
//    dcVoltF=750;
//#endif
}

/*inline void DcVoltLoop2(void)
{
	// 初始给定电网峰值+40即可。
	if(StateFlag.dcBusVtIncStart==TRUE){
		if(dcBusVtInc < dcBusVoltGiven)
			dcBusVtInc +=1000*CTRLSTEP_ALGORITHM;	//Increase per second 500V
		else{
			dcBusVtInc = dcBusVoltGiven;
			StateFlag.dcBusVtIncStart=FALSE;
		}
	}else{
		dcBusVtInc = dcBusVoltGiven;
	}

	currentRefD = (DCL_runPI(&UdcPICtrl,dcBusVtInc,dcVoltF));
}
*/
/*　 裂相电容控制策略
 * 之前两个电压霍尔的电路板，采样为直流电压正负和N负。
 *　1、直流侧N负和直流Dc/2做比较，通过PI调节输出至PWM控制电压输入，用来补偿中间电流。
 *  2、输出方向：因采集的N负电压，当电压高于0.5倍的直流电压，需要流入电流，给正N充电
 *              充电，则PWM给定负向减小，输出到比较值上应为“-”。
 *  C2-C1 = deltaV， 滤波去除1000Hz以上的谐波，输出结果进行限幅，纯比例调节。
 *  限幅：大于0.05*Vref = 0.05*700 = 35
 *  不加积分的后果：中点电压有偏移，不能完全调节回去。
 */

/*inline void DcVoltDiv2Loop(void)
{
	#if PROJ_OUYAJIXIE == 1
	UdcBalanceCurr = -DCL_runPI(&UdcPIBalance, 0,dcVoltDiv2Err);	//欧亚机械660V的带负号
	#else
	UdcBalanceCurr = DCL_runPI(&UdcPIBalance, 0,(dcVoltUpF - dcVoltDnF));		//2电平400V,120A400V定型的小单元不带负号,杭州正华及家中的三电平40A不带负号
	#endif
}*/
/*inline void testFunction(void)
{
    if((StateEventFlag == STATE_EVENT_RUN)||(StateEventFlag == STATE_EVENT_WAIT)){
        if(StateFlag.apfWiringmethod == 0){     //0：三相四线制  1：三相三线制
            DcVoltDiv2Loop();       //裂相电压的控制
        }else{
            UdcBalanceCurr = 0;
        }
        DcVoltLoop2();              //直流电压的控制
        ErrorRecord.B.REC_DC_START = StateFlag.dcBusVtIncStart;
    }else{
        UdcBalanceCurr=0;
        currentRefD=0;                                              //停机后清零,因为自动启动也需要用到逆变侧电流
    }
}*/
//inline void	ReactPowerComput(void)
//{
//	if((StateEventFlag == STATE_EVENT_RUN)||(StateEventFlag == STATE_EVENT_WAIT)){  //
////		if(StateFlag.apfWiringmethod == 0){ 	//0：三相四线制  1：三相三线制
////			DcVoltDiv2Loop();  		//裂相电压的控制
////		}else{
////			UdcBalanceCurr = 0;
////		}
////		DcVoltLoop2();     			//直流电压的控制
////		ErrorRecord.B.REC_DC_START = StateFlag.dcBusVtIncStart;
//		if(CntSec.cntForOutputCur >= 6){// 等待5秒投入补偿
//			if(I_ins_index < 1)     I_ins_index += 0.0005; 				//启动程序，切记切记，位置勿动
//			else 					I_ins_index=1;
//		}
//
//		if(StateFlag.reactPrCompFlag == 1){				//无功补偿功能代码段
//		    ReactLoadCompCurQ = reactPowerCompCurQ*CapaReactCorr*outCurTempLimit*ReactivePowerCurrCalib;
//		    ReactLoadCompCurA = ReactPowerCurQPA*CapaReactCorrA*outCurTempLimit*ReactivePowerCurrCalib;
//		    ReactLoadCompCurB = ReactPowerCurQPB*CapaReactCorrB*outCurTempLimit*ReactivePowerCurrCalib;
//		    ReactLoadCompCurC = ReactPowerCurQPC*CapaReactCorrC*outCurTempLimit*ReactivePowerCurrCalib;
//			currentRefQ = ReactLoadCompCurQ*ReactivePowerCurrCalibQ;
//			currentRefA = ReactLoadCompCurA*ReactivePowerCurrCalibA;
//			currentRefB = ReactLoadCompCurB*ReactivePowerCurrCalibB;
//			currentRefC = ReactLoadCompCurC*ReactivePowerCurrCalibC;
//			if(currentRefQ > REACTPOWERLIMIT)		currentRefQ = REACTPOWERLIMIT;
//			if(currentRefQ < -REACTPOWERLIMIT)		currentRefQ = -REACTPOWERLIMIT;
//			if(currentRefA > REACTPOWERLIMITPH)		currentRefA = REACTPOWERLIMITPH;
//			if(currentRefA < -REACTPOWERLIMITPH)	currentRefA = -REACTPOWERLIMITPH;
//			if(currentRefB > REACTPOWERLIMITPH)		currentRefB = REACTPOWERLIMITPH;
//			if(currentRefB < -REACTPOWERLIMITPH)	currentRefB = -REACTPOWERLIMITPH;
//			if(currentRefC > REACTPOWERLIMITPH)		currentRefC = REACTPOWERLIMITPH;
//			if(currentRefC < -REACTPOWERLIMITPH)	currentRefC = -REACTPOWERLIMITPH;
//		}else{
//			currentRefQ=0;
//			currentRefA = currentRefB = currentRefC = 0;
//		}
//
//		if(StateFlag.VolSurTimeFlag == 1){
//			currentRefQ=0;
//			currentRefQa = reactPowerCompCurQa;
//			if(currentRefQa > REACTPOWERLIMIT)	currentRefQa = REACTPOWERLIMIT;
//			if(currentRefQa < -REACTPOWERLIMIT)	currentRefQa = -REACTPOWERLIMIT;
//			currentRefQb = reactPowerCompCurQb;
//			if(currentRefQb > REACTPOWERLIMIT)	currentRefQb = REACTPOWERLIMIT;
//			if(currentRefQb < -REACTPOWERLIMIT)	currentRefQb = -REACTPOWERLIMIT;
//			currentRefQc = reactPowerCompCurQc;
//			if(currentRefQc > REACTPOWERLIMIT)	currentRefQc = REACTPOWERLIMIT;
//			if(currentRefQc < -REACTPOWERLIMIT)	currentRefQc = -REACTPOWERLIMIT;
//		}else{
//			currentRefQa=0;
//			currentRefQb=0;
//			currentRefQc=0;
//		}
//		if(StateFlag.apfWiringmethod == 0)  currentRefQ+=ActivePowerCurrCalib;		//基波电流静差	//0：三相四线制  1：三相三线制
//		else 					            currentRefQ+=ActivePowerCurrCalib;
//		currentRefA += ActivePowerCurrCalibPH;
//		currentRefB += ActivePowerCurrCalibPH;
//		currentRefC += ActivePowerCurrCalibPH;
//
//		if(StateFlag.negCurCompFlag == 1){ 				//不平衡补偿功能代码段
//		    UbanCompLoadCurND = -LoadFundaCurND*negCurCompPerc*NegaDPowerCalib;
//		    UbanCompLoadCurNQ = -LoadFundaCurNQ*negCurCompPerc*NegaQPowerCalib;
//			negCurCompD = -UbanCompLoadCurND*NegaDPowerCalibD*MU_MultRatio;	// 有功负序分量
//			negCurCompQ = -UbanCompLoadCurNQ*NegaQPowerCalibQ*MU_MultRatio; 	// 无功负序分量
//			if(negCurCompD > NegCurLimit)		negCurCompD = NegCurLimit;
//			if(negCurCompD < -NegCurLimit)		negCurCompD = -NegCurLimit;
//			if(negCurCompQ > NegCurLimit)		negCurCompQ = NegCurLimit;
//			if(negCurCompQ < -NegCurLimit)		negCurCompQ = -NegCurLimit;
//		}else{
//			negCurCompD = 0;
//			negCurCompQ = 0;
//		}
//		}else{
////		if(MeanInitChan==NEUTRAL_LINE_VOLT){					//Note that if you delete this statement, the initialization will fail and all mean calculation errors will occur;注意,如果删掉此语句,会出现初始化不能完成而导致所有的mean计算错误.
////			dcVoltDiv2Err =MeanInit(NEUTRAL_LINE_VOLT);
////		}
//
//		if(I_ins_index > 0.02) I_ins_index -= 0.02;    			//软关机部分，主要是用来软关除了支撑直流母线的有功电流以外的其他电流。
//
////		UdcBalanceCurr=0;
////		currentRefD=0;												//停机后清零,因为自动启动也需要用到逆变侧电流
//		currentRefQ=0;
//		currentRefQa=0;
//		currentRefQb=0;
//		currentRefQc=0;
//		negCurCompD = 0;
//		negCurCompQ = 0;
//	}
//	FaultRecordSel();
////-	TimerCounter();   				//-1.37us
////	if(++MeanPos>=MEANPOINT) MeanPos=0;	//Sliding window pointer
//	if(CntMs.StartDelay > CNT_MS(1000)){			//Power on calibration delay, thus avoiding momentary sampling error on DSP power.
//		if((StateFlag.autoAdcOffsetFlag == 1)&&(StateEventFlag != STATE_EVENT_RUN)){	//Execute only when the event state is not equal to STATE_EVENT_RUN
//			StateFlag.onceTimeAdcAutoAdjust = true;
//			StateFlag.autoAdcOffsetFlag = 0;
//		}
//
//		if(StateFlag.onceTimeAdcAutoAdjust){	//上电后自动运行一次
//			AdRegeditOffset();
//		}
//    }
//}

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

inline void  SetHeatPulse(void)
{
#if HEATPULSE==1
	TOGGLE_PULSE();
#endif
}

inline void	ADPosCnt(void)
{
	if(++ADBufPos>=ADGROUP_NUM) ADBufPos=0;
    TimeStamp=PhaseLimit(TimeStamp+IFFT_PHASE_STEP); //ifft用的时间戳产生
}

inline void VoltSlidPosCnt(void)    //构造正弦波
{
    if(++VoltPrvPos >= VOLT_FUNDPOINT)
        VoltPrvPos=0;                                               //电网电压一周波缓冲区输入指针(即经过了滤波器延时及采样延时的)//实时采样的波形(经过滤波之后)
    VoltPos = VoltPrvPos+VoltLeadPoints;                            //超前校正指针,超前了360*VoltLeadPoints/VOLT_FUNDPOINT度//电压校正

    if(VoltPos >= VOLT_FUNDPOINT)        VoltPos    -=VOLT_FUNDPOINT;                                   //环形缓冲区//VoltPos是提前做好的一个数组点数波形

    CapVoltPos = VoltPos+CAP_VOL_POINT;                             //电容电压一周波缓冲区指针,滞后270度//
    if(CapVoltPos >= VOLT_FUNDPOINT)    CapVoltPos  -=VOLT_FUNDPOINT;   //环形缓冲区

    CapVoltPosCur = VoltPos+CAP_VOL_POINT_CUR;                      //电容电压一周波缓冲区指针,滞后90度//
    if(CapVoltPosCur>=VOLT_FUNDPOINT) CapVoltPosCur -=VOLT_FUNDPOINT;
}

#if PWM_FREQUENCE_16KHZ
void PIController(void)
{
/*	float currA,currB,currC;
	currA=CurrRefA-ApfOutCurA;
	if(currA > OutCurMaxLimit)			currA = OutCurMaxLimit;
	if(currA < OutCurMaxLimitNeg)		currA = OutCurMaxLimitNeg;
	currB=CurrRefB-ApfOutCurB;
	if(currB > OutCurMaxLimit)			currB = OutCurMaxLimit;
	if(currB < OutCurMaxLimitNeg)		currB = OutCurMaxLimitNeg;
	currC=CurrRefC-ApfOutCurC;
	if(currC > OutCurMaxLimit)			currC = OutCurMaxLimit;
	if(currC < OutCurMaxLimitNeg)		currC = OutCurMaxLimitNeg;
	if(StateFlag.dcBusVtIncStart==FALSE){
		CtrlVoltA=currA*kp_CurA;
		CtrlVoltB=currB*kp_CurB;
		CtrlVoltC=currC*kp_CurC;
	}else{		//直流电压正在启动过程中...
		CtrlVoltA=currA*kp_Cur*0.02f;			//上电软起动
		CtrlVoltB=currB*kp_Cur*0.02f;
		CtrlVoltC=currC*kp_Cur*0.02f;
	}*/
}
#endif

#ifdef BOOST_DEBUG
void PRController(void)
{

/*	float currA,currB,currC;
	currA=CurrRefA-ApfOutCurA;
	if(currA > OutCurMaxLimit)			currA = OutCurMaxLimit;
	if(currA < OutCurMaxLimitNeg)		currA = OutCurMaxLimitNeg;
	currB=CurrRefB-ApfOutCurB;
	if(currB > OutCurMaxLimit)			currB = OutCurMaxLimit;
	if(currB < OutCurMaxLimitNeg)		currB = OutCurMaxLimitNeg;
	currC=CurrRefC-ApfOutCurC;
	if(currC > OutCurMaxLimit)			currC = OutCurMaxLimit;
	if(currC < OutCurMaxLimitNeg)		currC = OutCurMaxLimitNeg;
	//if((StateEventFlag == STATE_EVENT_RUN))  // 最大36us 0623
	if(StateFlag.dcBusVtIncStart==FALSE){
		#if PR_CTRL==1
		if(StateFlag.spwmFlag==0){		//PR控制开启
			DCL_runDF22Block(&PRCtrlABK[0],&ctrlVoltA,currA);	//
			DCL_runDF22Block(&PRCtrlBBK[0],&ctrlVoltB,currB);	//
			DCL_runDF22Block(&PRCtrlCBK[0],&ctrlVoltC,currC);	//
			CtrlVoltA=ctrlVoltA*ki_Cur+currA*kp_Cur;
			CtrlVoltB=ctrlVoltB*ki_Cur+currB*kp_Cur;
			CtrlVoltC=ctrlVoltC*ki_Cur+currC*kp_Cur;
		}else{					//PI控制开启
			CtrlVoltA=currA*kp_Cur;
			CtrlVoltB=currB*kp_Cur;
			CtrlVoltC=currC*kp_Cur;
		}
		#else
		CtrlVoltA=currA*kp_CurA;
		CtrlVoltB=currB*kp_CurB;
		CtrlVoltC=currC*kp_CurC;
		#endif
	}else{		//直流电压正在启动过程中...
		#if PR_CTRL==1
		if(StateFlag.spwmFlag==0){
			DCL_runDF22Block(&PRCtrlABK[0],&ctrlVoltA,currA);	//
			DCL_runDF22Block(&PRCtrlBBK[0],&ctrlVoltB,currB);	//
			DCL_runDF22Block(&PRCtrlCBK[0],&ctrlVoltC,currC);	//
		}
		#endif
		CtrlVoltA=currA*kp_CurA*0.02f;			//上电软起动
		CtrlVoltB=currB*kp_CurB*0.02f;
		CtrlVoltC=currC*kp_CurC*0.02f;
	}
*/
//	currC=CurrRefC-ApfOutCurCF;
//	if(currC>500)currC=500;	if(currC<-500)currC=-500;
//	ctrlC=currC*kp_Cur;
//	ctrlC += DCL_runDF22(&PRCtrlC[0],currC);	//1
//	ctrlC += DCL_runDF22(&PRCtrlC[1],currC);	//3
//	ctrlVoltC=ctrlC;

//    float currA,currB,currC;
//    float betaSin,alphaSin,betaCos,alphaCos;//,tmp;
//    betaSin     = ApfOutCurUVBeta   *UVresSin;
//    alphaSin    = ApfOutCurUV       *UVresSin;
//    betaCos     = ApfOutCurUVBeta   *UVresCos;
//    alphaCos    = ApfOutCurUV       *UVresCos;

//    invUVCurD=DCL_runDF22(&InvUVCurDFilter,alphaCos + betaSin);
//    invUVCurQ=DCL_runDF22(&InvUVCurQFilter,betaCos  - alphaSin);
    StateFlag.InvCurUVRmsReadyFlag = 1;
    GardVoltIntissued = 50;           //界面下发值
    GardVoltTarVal = GardVoltIntissued-gpVoltA_rms;     //目标值
    if(StateEventFlag == STATE_EVENT_RUN){
//        if((ParamEnviron.BG_MODEL == BGMODLE_3P_CURR_SOURCE)){
            if(StateFlag.InvCurUVRmsReadyFlag){
//                if(VoltHarmOver>0.1){
                    currRef_D = RunPIRE(&OutInvUVCurrRMS,GardVoltTarVal,gpVoltC_rms);
//                    currRef_D = 8;
                    CtrlVoltUV = (currRef_D *(SQRT2)* GridResCos );
//                }else{
//                    currRefUV_D =RunPI(&OutInvUVCurrD,OutInvCurrVal*(SQRT2),invUVCurD);
//                    currRefUV_Q =RunPI(&OutInvUVCurrQ,0,invUVCurQ);
//                    CtrlVoltUV = (currRefUV_D * UVresCos - currRefUV_Q * UVresSin);
//                }
//              }
//            }else{
//            OutInvUVCurrD.i10=0;
//            OutInvUVCurrD.i6=0;
//            OutInvUVCurrQ.i10=0;
//            OutInvUVCurrQ.i6=0;
//            OutInvUVCurrRMS.i10=0;
//            OutInvUVCurrRMS.i6=0;
//            CtrlVoltUV = 0;
//          }
        }else{
        OutInvUVCurrD.i10=0;
        OutInvUVCurrD.i6=0;
        OutInvUVCurrQ.i10=0;
        OutInvUVCurrQ.i6=0;
        OutInvUVCurrRMS.i10=0;
        OutInvUVCurrRMS.i6=0;
        CtrlVoltUV = 0;
    }
//#endif                                                           //PI控制开启
//        CtrlVoltA=currA*kp_Cur;
//        CtrlVoltB=currB*kp_Cur;
//        CtrlVoltC=currC*kp_Cur;
}
}

/*void LineFitLeastSquares*/
//float ratio = 0, offset = 0;
//float xAxis[10] = {0,1,2,3,4,5,6,7,8,9};
//float yAxis[10] = {100,101,102,103,104,105,106,107,108,109};
#define AD_FITING_N 3
#define FITING_A (5.0f)       //0*0+1*1+...(N-1)*(N-1)
#define FITING_B (3.0f)        //0+1+...+(N-1)
#define FITING_S  (1.0f/((AD_FITING_N*FITING_A) - (FITING_B*FITING_B)))
float debug_Y = 0,debugA_Y = 0,debugB_Y = 0,debugC_Y = 0,debugA_ratio,debugA_offset,debugB_ratio,debugB_offset;

inline ZCPJudgeA_ovsample(volatile Uint16 *ADResult,int ADoffset,int NumPort,Uint16 *pflag)
{
    Int16 i,Ci=0,Di=0;
    float ratio = 0, offset = 0;
    float C,D;
    for(i=0; i<AD_FITING_N; i++){
        Di += i* *ADResult;    //data_x[i] * data_y[i];
        Ci += *ADResult++;      //data_y[i];
    }
    C= (float)Ci;
    D= (float)Di;

    // 计算斜率a和截距b
//    if( (temp>-0.0001f) && (temp<0.0001f) ){         // 判断分母不为0
    ratio  = (AD_FITING_N*C - FITING_B*D) * FITING_S;
    offset = (FITING_A*D - FITING_B*C) * FITING_S;
//    }else{
//        ratio = 1;
//        offset = 0;
//    }

    // 计算相关系数r
/*  float Xmean, Ymean;
    Xmean = B / AD_FITING_N;
    Ymean = D / AD_FITING_N;

    float tempSumXX = 0.0, tempSumYY = 0.0;
    for (i=0; i<AD_FITING_N; i++)
    {
        tempSumXX += (data_x[i] - Xmean) * (data_x[i] - Xmean);
        tempSumYY += (data_y[i] - Ymean) * (data_y[i] - Ymean);
        E += (data_x[i] - Xmean) * (data_y[i] - Ymean);
    }
    F = sqrtf(tempSumXX) * sqrtf(tempSumYY);

    float r=E / F;
    return r;*/
    //坐标统一:过采样的x坐标是根据ACQPS=0x18,及12bitAD转换时间12点得出x坐标轴的间隔为(24+41)*SYSCLK(1/200M)=0.325us
    //T1PR*2*(1/100M)*0.5*harmCompPerc得出PWM上顶点到占空比的点的时间(us).
    float Y=(ratio*(dcVoltUpRatioVirtu_reciprocal) * T1PR * harmCompPerc + offset - ADoffset)*outputCurRatio;   //y=(kx+b-零偏)*霍尔硬件系数
    *pflag = Y>0?0:1;      //  电流方向可能有反的方向  ???
    if(NumPort == 0){
        debugA_Y = Y;    //临时测试用
        debugA_ratio = ratio;
        debugA_offset = offset;
    }else{
        debugB_Y = Y;    //临时测试用
        debugB_ratio = ratio;
        debugB_offset = offset;
    }
}
#endif

#define DEAD_CORR_HYSTERESIS 3

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
float escdebugpll=0,escdebugdata;
#define LooktablePower2Amplitude 0.05f
/*float ReactivePowerComFUN(float Esc_VoltPLL,float Q)  //ESC无功补偿函数
{
    float tmp,tmp2,pll;
    while(Esc_VoltPLL > PI){
        Esc_VoltPLL -= PI;
    }
    if(Q<0){
    tmp= sinf((2.5f/2.0f)*Esc_VoltPLL);
//        tmp2=sinf((7.0f/2.0f)*Esc_VoltPLL);
//        tmp+=tmp2*0.15;
    if(tmp<0)tmp=0;
    }else{
        //180/(2.5/2.0)=144.故超前校正的区间为0~144度.则滞后校正为36~180度.转为弧度值为0.2PI
        pll=Esc_VoltPLL-(0.2*PI);
        if(pll > 0){
            tmp= sinf((2.5f/2.0f)*pll);
//            tmp2=sinf((7.0f/2.0f)*pll);
        }else{
            tmp=0;
        }
//        tmp+=tmp2*0.15;
        if(tmp<0)tmp=0;
    }

    if(ConstantReactivePower<0.001 && ConstantReactivePower>-0.001) //界面设置为0,则开启无功补偿模式后自动补偿.
        return (tmp)*fabsf(Q);
    else    //界面下发值作为补偿
    return (tmp)*ConstantReactivePower*LooktablePower2Amplitude;
}*/

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
float  UNCurtestdata=0.0001f;
int unbalanceGenFlag =0;
int midNumber=0;
float CurMeanValue;
float CurDiffMAXValue;


void UnCurrCompFUN(void)        //ESC三相不平衡电流补偿
{
#define MAXMODE 1
#define MINMODE -1
#define NORMMODE 0

    CurMeanValue = (UnCurrData[0]+UnCurrData[1]+UnCurrData[2])/3; //三相电流平均值
    CurDiffMAXValue = Max3(UnCurrData[0],UnCurrData[1],UnCurrData[2]);
    midNumber = Max3(UnCurrData[0],UnCurrData[1],UnCurrData[2]);
    //mid=a>b ? (b>c ? b:(a>c ? c:a)):(b>c ? (a>c ? a:c):b)
    currentUnbalance = (CurDiffMAXValue-CurMeanValue)/CurMeanValue;    //三相电流不平衡度

    if(currentUnbalance > TargetCurrentUnbalance){   //不平衡度>界面设定的值,等待1秒进入不平衡调节模式
        if(ESCFlagA.ESCCntSec.UNCurrDelay1 >= CNT_SEC(1)){
            unbalanceGenFlag=TRUE;
        }else{

    }
    }else{
        ESCFlagA.ESCCntSec.UNCurrDelay1 = 0;
    }

    //不平衡度<界面设定的值-0.01退出不平衡调节模式,维持住当前的指令电压
    if(currentUnbalance < TargetCurrentUnbalance-0.01){
        if(ESCFlagA.ESCCntSec.UNCurrDelay2 >= CNT_SEC(3)){
            unbalanceGenFlag=FALSE;
    }
    }else{
        ESCFlagA.ESCCntSec.UNCurrDelay2 = 0;
}

    if(unbalanceGenFlag == TRUE){

        centreGravity=(VoltageOverflow[0]+VoltageOverflow[1]+VoltageOverflow[2]);    //用上一次的三者重心偏向上还是下.
        if(centreGravity>=2){       //电压已有两相调至最高,故重心(meanValue)得下移
            centreGravityValue-=0.00001;
        }else if(centreGravity<=-2){//电压已有两相调至最低,故重心(meanValue)得上移
            centreGravityValue+=0.00001;
        }else{
            centreGravityValue =0;  //重心维持现在的三相电流平均(mean替代)值
            }
        if(centreGravityValue<-0.2)
            centreGravityValue=-0.2;
        if(centreGravityValue>0.2)
            centreGravityValue=0.2;

        if((UnCurrData[0] > CurMeanValue*(1+centreGravityValue))){      //%50的调节裕度
            CurrentUnbalanceRegularVoltage[0] -= UNCurtestdata; //电流大,应该调低电压,即占空比趋向1
        }else if((UnCurrData[0] < CurMeanValue*(1+centreGravityValue))){
            CurrentUnbalanceRegularVoltage[0] += UNCurtestdata; //电流小,应该调高电压,即占空比趋向0.5
            }

        if(CurrentUnbalanceRegularVoltage[0]>ESCFlagA.Volttarget){
            CurrentUnbalanceRegularVoltage[0]=ESCFlagA.Volttarget;
            VoltageOverflow[0]=MAXMODE;
        }else if(CurrentUnbalanceRegularVoltage[0]<ESCFlagA.VoltThreShold){
            CurrentUnbalanceRegularVoltage[0]=ESCFlagA.VoltThreShold;
            VoltageOverflow[0]=MINMODE;
        }else{
            VoltageOverflow[0]=NORMMODE;
            }

        if((UnCurrData[1] > CurMeanValue*(1+centreGravityValue))){
            CurrentUnbalanceRegularVoltage[1] -= UNCurtestdata; //电流大,应该调低电压
        }else if((UnCurrData[1] < CurMeanValue*(1+centreGravityValue))){
            CurrentUnbalanceRegularVoltage[1] += UNCurtestdata; //电流小,应该调高电压
            }

        if(CurrentUnbalanceRegularVoltage[1]>ESCFlagB.Volttarget){
            CurrentUnbalanceRegularVoltage[1]=ESCFlagB.Volttarget;
            VoltageOverflow[1]=MAXMODE;
        }else if(CurrentUnbalanceRegularVoltage[1]<ESCFlagB.VoltThreShold){
            CurrentUnbalanceRegularVoltage[1]=ESCFlagB.VoltThreShold;
            VoltageOverflow[1]=MINMODE;
        }else{
            VoltageOverflow[1]=NORMMODE;
        }

        if((UnCurrData[2] > CurMeanValue*(1+centreGravityValue))){
            CurrentUnbalanceRegularVoltage[2] -= UNCurtestdata; //电流大,应该调低电压
        }else if((UnCurrData[2] < CurMeanValue*(1+centreGravityValue))){
            CurrentUnbalanceRegularVoltage[2] += UNCurtestdata; //电流小,应该调高电压
        }

        if(CurrentUnbalanceRegularVoltage[2]>ESCFlagC.Volttarget){
            CurrentUnbalanceRegularVoltage[2]=ESCFlagC.Volttarget;
            VoltageOverflow[2]=MAXMODE;
        }else if(CurrentUnbalanceRegularVoltage[2]<ESCFlagC.VoltThreShold){
            CurrentUnbalanceRegularVoltage[2]=ESCFlagC.VoltThreShold;
            VoltageOverflow[2]=MINMODE;
        }else{
            VoltageOverflow[2]=NORMMODE;
        }
    }
}
#else


enum ChanMaxName{
    UnCurrCompChanA=0,
    UnCurrCompChanB=1,
    UnCurrCompChanC=2,
};

void ChanMax3(float a,float b,float c,int * max ,int *min ,int *midd) {//求输入电流最大值
    if(a>b){
        if(a>c){        //A>B,A>C
            *max         = UnCurrCompChanA;

            if(b<c){    //A>B,A>C,C>B :A>C>B
                *min     = UnCurrCompChanB;
                *midd    = UnCurrCompChanC;
            }else{      //A>B,A>C,B>C :A>B>C
                *min     = UnCurrCompChanC;
                *midd    = UnCurrCompChanB;
            }

        }else{          //A>B,C>A :C>A>B
            *max         = UnCurrCompChanC;
            *min         = UnCurrCompChanB;
            *midd        = UnCurrCompChanA;
        }

    }else{      //A<B
        if(a<c){        //A<C
            *min         = UnCurrCompChanA;

            if(b<c){    //A<B,A<C,B<C :A<B<C
                *max     = UnCurrCompChanC;
                *midd    = UnCurrCompChanB;
            }else{      //A<B,A<C,C<B :A<C<B
                *max     = UnCurrCompChanB;
                *midd    = UnCurrCompChanC;
            }

        }else{          //A<B,C<A :C<A<B
            *min         = UnCurrCompChanC;
            *max         = UnCurrCompChanB;
            *midd        = UnCurrCompChanA;
        }
    }
}

int unbalanceGenFlag =0;
int maxCh,minCh,middCh;
#define UN_CURR_COMP_STEP_VOLT  0.0003f

void UnCurrCompFUN(void)        //ESC三相不平衡电流补偿
{

    float tmp = 0;
    float CurMeanValue = (UnCurrData[0]+UnCurrData[1]+UnCurrData[2])*S1DIV3; //三相电流平均值
    float CurDiffMAXValue = Max3(UnCurrData[0],UnCurrData[1],UnCurrData[2]);
    currentUnbalance = (CurDiffMAXValue - CurMeanValue)/CurMeanValue;    //三相电流不平衡度

    if(currentUnbalance > TargetCurrentUnbalance){   //不平衡度>界面设定的值,等待1秒进入不平衡调节模式
        if(CntMs.UNCurrDelay1 >= CNT_MS(1000)){
            unbalanceGenFlag = TRUE;
        }
    }else{
        CntMs.UNCurrDelay1 = 0;
    }

    //不平衡度<界面设定的值-0.01退出不平衡调节模式,维持住当前的指令电压
    if(currentUnbalance < TargetCurrentUnbalance - 0.01){
        if(CntMs.UNCurrDelay2 >= CNT_MS(20000)){
            unbalanceGenFlag = FALSE;
        }
    }else{
        CntMs.UNCurrDelay2 = 0;
    }

    if(unbalanceGenFlag == TRUE){
        ChanMax3(UnCurrData[0],UnCurrData[1],UnCurrData[2],&maxCh,&minCh,&middCh);
        CurrentUnbalanceRegularVoltage[maxCh] -= UN_CURR_COMP_STEP_VOLT;
        tmp= CurrentUnbalanceRegularVoltage[maxCh];
        CurrentUnbalanceRegularVoltage[minCh] += UN_CURR_COMP_STEP_VOLT;
        tmp+=CurrentUnbalanceRegularVoltage[minCh] ;
        float sum = ESCFlagA.Volttarget + ESCFlagB.Volttarget + ESCFlagC.Volttarget ;// (1+VolttargetCorrA)

        CurrentUnbalanceRegularVoltage[middCh] = sum - tmp ;
    };
//    else{
//        CurrentUnbalanceRegularVoltage [0] =ESCFlagA.Volttarget;
//        CurrentUnbalanceRegularVoltage [1] =ESCFlagB.Volttarget;
//        CurrentUnbalanceRegularVoltage [2] =ESCFlagC.Volttarget;
//    }
    if(CurrentUnbalanceRegularVoltage[0] > GV_RMS_OVER-25) CurrentUnbalanceRegularVoltage[0] = GV_RMS_OVER-25;
    if(CurrentUnbalanceRegularVoltage[0] < GV_RMS_UNDER+70) CurrentUnbalanceRegularVoltage[0] = GV_RMS_UNDER+70;
    if(CurrentUnbalanceRegularVoltage[1] > GV_RMS_OVER-25) CurrentUnbalanceRegularVoltage[1] = GV_RMS_OVER-25;
    if(CurrentUnbalanceRegularVoltage[1] < GV_RMS_UNDER+70) CurrentUnbalanceRegularVoltage[1] = GV_RMS_UNDER+70;
    if(CurrentUnbalanceRegularVoltage[2] > GV_RMS_OVER-25) CurrentUnbalanceRegularVoltage[2] = GV_RMS_OVER-25;
    if(CurrentUnbalanceRegularVoltage[2] < GV_RMS_UNDER+70) CurrentUnbalanceRegularVoltage[2] = GV_RMS_UNDER+70;

}


#endif
float Esczct_CNTA = 0,Esczct_CNTB = 0,Esczct_CNTC = 0;
float StepSize = 0.703125;
float ESC_FeedForward_DutyA = 0,ESC_FeedForward_DutyB = 0,ESC_FeedForward_DutyC = 0;
float PIOutVoltValueA = 0,PIOutVoltValueB = 0,PIOutVoltValueC = 0;
int ESCLowCurrFlagA = 0,ESCLowCurrFlagB = 0,ESCLowCurrFlagC = 0;
//float dbg_ESC_DutyDataA=0.95,dbg_ESC_DutyDataB=0.95,dbg_ESC_DutyDataC=0.95;
//float dbg_negCurCompPerc =0.025;
//int dbg_endatagen =1;
//#define LEAD_SAMPLE_INCREMENT_DUTY  0.12f
//#define LEAD_SAMPLE_DECREMENT_DUTY -0.12f

#define LEAD_SAMPLE_INCREMENT_DUTY  0.0f
#define LEAD_SAMPLE_DECREMENT_DUTY  0.0f

float DBG_leadSampleIncrementDuty = LEAD_SAMPLE_INCREMENT_DUTY;
float DBG_leadSampleDecrementDuty = LEAD_SAMPLE_DECREMENT_DUTY;

/*! 实现对调制值的限值
* @private NumeratorValue
* @private DenominatorVAL
* @private TestArithFlagA 测试用...
*
* @param[in] RMSGridInVAL 输入电压(RMS)
* @param[in] INSGridIn 输入电压瞬时值波形
* @param[in] target 目标值(RMS)
* @param[in] PIerr (目标值-输出值)的PID输出的误差值
* @param[in] LockPhAngle 电压锁相角
* @param[in] *pStorage 前馈值存储
* @param[in] ESCPHASE 三相选择,这个是权宜之计,可优化
*/
//#define RMS_CONSTANT_CURRENT_OVERLOAD_30 58
//#define RMS_CONSTANT_CURRENT_DIFF_30 13        //
//#define RMS_CONSTANT_CURRENT_RATED_30 45       //额定电流
//#define INS_CONSTANT_CURRENT_RATED_30 82
//
//#define RMS_CONSTANT_CURRENT_OVERLOAD_50 93
//#define RMS_CONSTANT_CURRENT_DIFF_50 18        //
//#define RMS_CONSTANT_CURRENT_RATED_50 75       //额定电流
//#define INS_CONSTANT_CURRENT_RATED_50 132
//
//#define RMS_CONSTANT_CURRENT_OVERLOAD_75 140
//#define RMS_CONSTANT_CURRENT_DIFF_75 23        //
//#define RMS_CONSTANT_CURRENT_RATED_75 114       //额定电流
//#define INS_CONSTANT_CURRENT_RATED_75 198

float RMSDutyLimit(float RMSGridInVAL, float INSGridIn, float target, float PIerr, float LockPhAngle, float *pStorage,int ESCPHASE)
{
//    float dutytmp;
    ArithFlagA = 2;
    ArithFlagB = 2;
    ArithFlagC = 2;
    TESTVALUE = 0;    //测试使用
    TESEPIerr = PIerr;
    testvalue1 = *pStorage;
    PhaseValue = LockPhAngle;
    TESTRMSGridInVAL = RMSGridInVAL;
    TESETarget = target;
    TESEINSGridIn = INSGridIn;
    NumeratorValue = INSGridIn-RMSGridInVAL*1.4142*sin(LockPhAngle);//电压突变值 = 当前瞬时值-上个周波的瞬时值;
    DenominatorVAL = target*1.4142*sin(LockPhAngle);

    //根据输入电压与目标值直接计算出一个大概的占空比
    if((RMSGridInVAL>10) && (target>10)){             //限制范围,防止数学上的0/0,x/0错误;


        if( ((LockPhAngle>=0.3f)          && (LockPhAngle<=(PIVAL-0.3f)))  ||  \
            ((LockPhAngle>=(PIVAL+0.3f))  && (LockPhAngle<=(2.0f*PIVAL-0.3f))) ){


            if(StateFlag.VoltageModeFlag == 0){           //升压
                if(ESCPHASE == 1){
                    //电压突变值在上下限内,或20ms都不在上下限内,
                    if(   (TestArithFlagA == 1) && \
                            (  ( (NumeratorValue < ArithVAL) && (NumeratorValue > ArithVal) ) ||\
                        (ESCFlagA.ESCCntMs.HarmDisDelay > CNT_MS(20))  )   ){
                        dutytmp = dutytmp1 = RMSGridInVAL/target;       //低压侧有效值除以目标值,得到要输出的值的前馈值
                        ArithFlagA = 1;                                 //设为正常补偿状态

                        if((NumeratorValue < ArithVAL) && (NumeratorValue > ArithVal)){ //电压突变值在上下限内,计数器清零
                            ESCFlagA.ESCCntMs.HarmDisDelay = 0;                         //计数器清零
                    }

                    //突变大于上下限,或电压有效值>过压保护值
                    }else if((NumeratorValue >= ArithVAL) || (NumeratorValue <= ArithVal) ||\
                        (TestArithFlagA == 0) || (ESCFlagA.VoltIn_rms >= GV_RMS_OVER)){

                        if((ESCFlagA.ESCCntMs.HarmDisDelay <= CNT_MS(20))){ //计数器在后台++,计数器小于20ms
                            //电压突变值除以目标值的瞬时值得到了突变比例   然后加上 实时值除以目标值 得到占空比
                            dutytmp = dutytmp1 = (RMSGridInVAL/target)+\
                                    (  (INSGridIn - RMSGridInVAL * SQRT2 * sin(LockPhAngle)) / (target * SQRT2 * sin(LockPhAngle))  );
                            ArithFlagA = 0;     //设为突变状态.
                            TestArithFlagA = 0;
                        }else{
                            TestArithFlagA = 1;
                            *pStorage = RMSGridInVAL/target;    //JCL?:长时间突变?
                        }
                    }else{
                        //null,
                        }
                }

               if(  (((NumeratorValue < ArithVAL)&&(NumeratorValue > ArithVal)) ||\
                    (ESCFlagB.ESCCntMs.HarmDisDelay > CNT_MS(20))) && (TestArithFlagB == 1)&&(ESCPHASE == 2) ){
                    dutytmp = dutytmp1 = RMSGridInVAL/target;
                    ArithFlagB = 1;
                    if((NumeratorValue < ArithVAL)&&(NumeratorValue > ArithVal)){
                        ESCFlagB.ESCCntMs.HarmDisDelay = 0;
                    }
                }else if(((NumeratorValue >= ArithVAL)||(NumeratorValue <= ArithVal)||\
                        (TestArithFlagB == 0)||(ESCFlagB.VoltIn_rms>=GV_RMS_OVER))&&(ESCPHASE == 2)){  //骤升算法
                        if((ESCFlagB.ESCCntMs.HarmDisDelay <= CNT_MS(20))){
                            dutytmp = dutytmp1 = (RMSGridInVAL/target)+((INSGridIn-RMSGridInVAL*1.4142*sin(LockPhAngle))/(target*1.4142*sin(LockPhAngle)));
                            ArithFlagB = 0;
                            TestArithFlagB = 0;
                        }
                        if(ESCFlagB.ESCCntMs.HarmDisDelay > CNT_MS(20)){
                            TestArithFlagB = 1;
                            *pStorage = RMSGridInVAL/target;
                        }
                }

              if(  (((NumeratorValue < ArithVAL)&&(NumeratorValue > ArithVal)) ||\
                   (ESCFlagC.ESCCntMs.HarmDisDelay > CNT_MS(20))) && (TestArithFlagC == 1)&&(ESCPHASE == 3) ){
                   dutytmp = dutytmp1 = RMSGridInVAL/target;
                   ArithFlagC = 1;
                   if((NumeratorValue < ArithVAL)&&(NumeratorValue > ArithVal)){
                       ESCFlagC.ESCCntMs.HarmDisDelay = 0;
                   }
               }else if(((NumeratorValue >= ArithVAL)||(NumeratorValue <= ArithVal)||\
                       (TestArithFlagC == 0)||(ESCFlagC.VoltIn_rms>=GV_RMS_OVER))&&(ESCPHASE == 3)){  //骤升算法
                       if((ESCFlagC.ESCCntMs.HarmDisDelay <= CNT_MS(20))){
                           dutytmp = dutytmp1 = (RMSGridInVAL/target)+((INSGridIn-RMSGridInVAL*1.4142*sin(LockPhAngle))/(target*1.4142*sin(LockPhAngle)));
                           ArithFlagC = 0;
                           TestArithFlagC = 0;
                       }
                       if(ESCFlagC.ESCCntMs.HarmDisDelay > CNT_MS(20)){
                           TestArithFlagC = 1;
                           *pStorage = RMSGridInVAL/target;
                       }
               }
            }else if(StateFlag.VoltageModeFlag == 1){     //降压
                dutytmp = dutytmp1 = target/RMSGridInVAL;
                if(ESCPHASE == 1)     ArithFlagA = 1;
                if(ESCPHASE == 2)     ArithFlagB = 1;
                if(ESCPHASE == 3)     ArithFlagC = 1;
            }
        }
        if(dutytmp1 > 0.96)        dutytmp1 = 0.96;       //输入电压高于目标,返回0.96,考虑到PI会自动调节故上限设为0.96
        if(dutytmp1 < 0.51)         dutytmp1 = 0.51;      //输入电压低于目标,限制到0.7~0.95区间,考虑到PI会自动调节故下限设为0.65
    }else{
          dutytmp1 = 0.96;
    }

    //幅值变化斜率控制,每秒变化率390%:=0.00001*25.6k
    if((ArithFlagA == 1)&&(ESCPHASE == 1)){ //正常状态进入斜率控制.
        //斜率限制
        if((dutytmp1 > 0.499) && (dutytmp1 < 0.961)){
           if(*pStorage > dutytmp1){
               *pStorage -= 0.0001;
           }else if(*pStorage < dutytmp1){
               *pStorage += 0.0001;
           }else{
               *pStorage = dutytmp1;
           }
        }else if(dutytmp1 > 0.962){
            *pStorage = dutytmp1 = 0.962;
        }else if(dutytmp1 < 0.497){
            *pStorage = dutytmp1 = 0.497;
        }

        if(StateFlag.VoltageModeFlag == 0){
            dutytmp1 =  *pStorage - PIerr;
        }else if(StateFlag.VoltageModeFlag == 1){
            dutytmp1 =  *pStorage + PIerr;
        }
    }


    if((ArithFlagB == 1)&&(ESCPHASE == 2)){
        //斜率限制
        if((dutytmp1 > 0.499) && (dutytmp1 < 0.961)){
           if(*pStorage > dutytmp1){
               *pStorage -= 0.0001;
           }else if(*pStorage < dutytmp1){
               *pStorage += 0.0001;
           }else{
               *pStorage = dutytmp1;
           }
        }else if(dutytmp1 > 0.962){
            *pStorage = dutytmp1 = 0.962;
        }else if(dutytmp1 < 0.497){
            *pStorage = dutytmp1 = 0.497;
        }

        if(StateFlag.VoltageModeFlag == 0){
            dutytmp1 =  *pStorage - PIerr;
        }else if(StateFlag.VoltageModeFlag == 1){
            dutytmp1 =  *pStorage + PIerr;
        }
    }

    if((ArithFlagC == 1)&&(ESCPHASE == 3)){

        if((dutytmp1 > 0.499) && (dutytmp1 < 0.961)){
           if(*pStorage > dutytmp1){
               *pStorage -= 0.0001;
           }else if(*pStorage < dutytmp1){
               *pStorage += 0.0001;
           }else{
               *pStorage = dutytmp1;
           }
        }else if(dutytmp1 > 0.962){
            *pStorage = dutytmp1 = 0.962;
        }else if(dutytmp1 < 0.497){
            *pStorage = dutytmp1 = 0.497;
        }

        if(StateFlag.VoltageModeFlag == 0){
            dutytmp1 =  *pStorage - PIerr;
        }else if(StateFlag.VoltageModeFlag == 1){
            dutytmp1 =  *pStorage + PIerr;
        }
    }

    if(dutytmp1 > 0.96){
        dutytmp1 = 0.96;
    }
    if(dutytmp1 < 0.51){
        dutytmp1 = 0.51;
    }
    return dutytmp1;
}
//float GridCurr1 = 0;
//float ConCurDutyLimit(float GridCurrF)
//{
//
//    if((GridCurrF > 106) && (GridCurrF <= 130)){
//        if(GridCurrF>GridCurr1)  GridCurr1 = GridCurrF;
//    }
//    if(GridCurr1 > 0){
//        GridCurr1 = GridCurr1-0.001;
//    }else{
//        GridCurr1 = 0;
//    }
//    VolttargetCorrA = Duty[(Uint16)(GridCurr1-106)];
//
//    if(GridCurrF > 130){
//        VolttargetCorrA = 0.5;
//    }
//    if(GridCurrF <= 106){
//        VolttargetCorrA = 1.0;
//    }
//}

float dbg_reactive;
int WUGONGlinshitest = 0,BUPINGHENGlinshitest = 0;
int T1PR_INT16 = 0;
float TIPRTEST = 0.805;
float VoltInF = 0,CurrInF = 0;
float DEBUGESC_DutyDataA = 0,DEBUGESC_DutyDataB = 0,DEBUGESC_DutyDataC = 0,DEBUGESC_DutyDataD = 0;
int UnCurrCompCnt=0;
int cntaaa=2;

//#define TAB_CONSTANT_CURR_TEMP_30_1     45.0
//#define TAB_CONSTANT_CURR_TEMP_30_K     0.001
//#define TAB_CONSTANT_CURR_TEMP_30_END   53.0
//
//#define TAB_CONSTANT_CURR_TEMP_50_1     45.0
//#define TAB_CONSTANT_CURR_TEMP_50_K     0.001
//#define TAB_CONSTANT_CURR_TEMP_50_END   53.0
//
//#define TAB_CONSTANT_CURR_TEMP_70_1     45.0
//#define TAB_CONSTANT_CURR_TEMP_70_K     0.001
//#define TAB_CONSTANT_CURR_TEMP_70_END   53.0

//ConstantCurr[0].RMS_CONSTANT_CURRENT_DIFF

////表格计算错误检查
//#if (CurTarget[2][19])>1)
//#error CurTarget_30 table comput error
//#endif
//#if ((TAB_CONSTANT_CURR_TEMP_50_1+TAB_CONSTANT_CURR_TEMP_50_K*19-TAB_CONSTANT_CURR_TEMP_50_END)>0.1)
//#error CurTarget_50 table comput error
//#endif
//#if ((TAB_CONSTANT_CURR_TEMP_70_1+TAB_CONSTANT_CURR_TEMP_70_K*19-TAB_CONSTANT_CURR_TEMP_70_END)>0.1)
//#error CurTarget_70 table comput error
//#endif

//const SMconstantCurr ConstantCurrDefault[3]={{VOLT_TARGET_CORR_DEFAULT,0.0f,TAB_CONSTANT_CURR_TEMP_30_END+5,13.0f,TAB_CONSTANT_CURR_TEMP_30_1,82.0f,0,0,0},\
//                                             {VOLT_TARGET_CORR_DEFAULT,0.0f,93.0f,18.0f,75.0f,132.0f,0,0,0},\
//                                             {VOLT_TARGET_CORR_DEFAULT,0.0f,140.0f,23.0f,114.0f,198.0f,0,0,0}}; //分设备容量(存在于flash里面,是const常数,不可修改)
//
//SMconstantCurr ConstantCurr[3]={{VOLT_TARGET_CORR_DEFAULT,0.0f,TAB_CONSTANT_CURR_TEMP_30_END+5,13.0f,TAB_CONSTANT_CURR_TEMP_30_1,82.0f,0,0,0},\
//                                {VOLT_TARGET_CORR_DEFAULT,0.0f,58.0f,13.0f,45.0f,82.0f,0,0,0},\
//                                {VOLT_TARGET_CORR_DEFAULT,0.0f,58.0f,13.0f,45.0f,82.0f,0,0,0}};  //分A/B/C三相(存在于内存中,可修改)

const SMconstantCurr ConstantCurrDefault[3]={{VOLT_TARGET_CORR_DEFAULT,0.0f,58.0f,13.0f,45.0f,82.0f,0,0,0},\
                                             {VOLT_TARGET_CORR_DEFAULT,0.0f,93.0f,18.0f,75.0f,135.0f,0,0,0},\
                                             {VOLT_TARGET_CORR_DEFAULT,0.0f,140.0f,23.0f,114.0f,198.0f,0,0,0}}; //分设备容量(存在于flash里面,是const常数,不可修改)

SMconstantCurr ConstantCurr[3]={{VOLT_TARGET_CORR_DEFAULT,0.0f,58.0f,13.0f,45.0f,82.0f,0,0,0},\
                                {VOLT_TARGET_CORR_DEFAULT,0.0f,58.0f,13.0f,45.0f,82.0f,0,0,0},\
                                {VOLT_TARGET_CORR_DEFAULT,0.0f,58.0f,13.0f,45.0f,82.0f,0,0,0}};  //分A/B/C三相(存在于内存中,可修改)

float ConstantCurrFSM(SMconstantCurr *pst,float gridCur_rms,float GridCurrF,float CurrTargetValue)
{
//    CurrTargetValueA_30 = pst->CurTarget[(Uint16)(WindCold.BOARD_OVER_TEMP-temp)];
    //第一段
    switch(pst->state)
    {
    case SM_CONSTANT_CURR_NORMAL:
        if((gridCur_rms >= pst->RMS_CONSTANT_CURRENT_OVERLOAD)||\
                (GridCurrF > pst->INS_CONSTANT_CURRENT_RATED)||\
                (GridCurrF < -pst->INS_CONSTANT_CURRENT_RATED))
        {
            pst->state = SM_CONSTANT_CURR_INS;
        }else{
            if((gridCur_rms > pst->RMS_CONSTANT_CURRENT_RATED) && (gridCur_rms < pst->RMS_CONSTANT_CURRENT_OVERLOAD))
            {
                pst->state = SM_CONSTANT_CURR_CONSTANT;
            }else if(gridCur_rms <= pst->RMS_CONSTANT_CURRENT_RATED){
                pst->state =  SM_CONSTANT_CURR_NORMAL;
            }else{
            }
        }

        break;

    case SM_CONSTANT_CURR_CONSTANT:
        if((gridCur_rms >= pst->RMS_CONSTANT_CURRENT_OVERLOAD)||\
                (GridCurrF > pst->INS_CONSTANT_CURRENT_RATED)||\
                (GridCurrF < -pst->INS_CONSTANT_CURRENT_RATED))
        {
            pst->state = SM_CONSTANT_CURR_INS;
        }else{
            if((gridCur_rms > pst->RMS_CONSTANT_CURRENT_RATED) && (gridCur_rms < pst->RMS_CONSTANT_CURRENT_OVERLOAD))
            {
                pst->state = SM_CONSTANT_CURR_CONSTANT;
            }else if(gridCur_rms <= pst->RMS_CONSTANT_CURRENT_RATED){
                pst->state =  SM_CONSTANT_CURR_NORMAL;
            }else{
            }
        }

        break;

    case SM_CONSTANT_CURR_INS:
        if((gridCur_rms >= pst->RMS_CONSTANT_CURRENT_OVERLOAD)||\
                (GridCurrF > pst->INS_CONSTANT_CURRENT_RATED)||\
                (GridCurrF < -pst->INS_CONSTANT_CURRENT_RATED))
        {
            pst->state = SM_CONSTANT_CURR_INS;
        }else{
            if((gridCur_rms > pst->RMS_CONSTANT_CURRENT_RATED) && (gridCur_rms < pst->RMS_CONSTANT_CURRENT_OVERLOAD))
            {
                pst->state = SM_CONSTANT_CURR_CONSTANT;
            }else if(gridCur_rms <= pst->RMS_CONSTANT_CURRENT_RATED){
                pst->state =  SM_CONSTANT_CURR_NORMAL;
            }else{
            }
        }

     break;

    case SM_CONSTANT_CURR_STANDBY:
    default:
        pst->Prvstate = pst->state = SM_CONSTANT_CURR_NORMAL;
        pst->CorrPI.i10=0;
    break;
    }

    //第二段
    switch(pst->state)
    {
    case SM_CONSTANT_CURR_NORMAL:
        if(pst->VolttargetCorr < 0){
            pst->VolttargetCorr += 0.00005;
        }

        if(pst->Prvstate == SM_CONSTANT_CURR_CONSTANT){
            pst->CNT1=10000;
        }
        if(pst->CNT1>=0){
            if(--pst->CNT1==0)
                pst->CorrPI.i10 = 0;
        }
        pst->Prvstate = pst->state;
        break;

    case SM_CONSTANT_CURR_CONSTANT:
        if(pst->Prvstate == SM_CONSTANT_CURR_INS){
            pst->CorrPI.i10 = pst->VolttargetCorr;
        }
        if(pst->Prvstate == SM_CONSTANT_CURR_NORMAL){
            pst->CorrPI.i10 = pst->VolttargetCorr;
        }
        pst->VolttargetCorr = DCL_runPI(&(pst->CorrPI),CurrTargetValue,gridCur_rms);
//        if(gridCur_rms>60)    SET_FLback(1);//测试debug
        pst->Prvstate = pst->state;
        break;

    case SM_CONSTANT_CURR_INS:
        pst->VolttargetCorr = -0.5;
    //                        VolttargetCorrPIA.i10 = -0.5;
        pst->Prvstate = pst->state;
        break;

    case SM_CONSTANT_CURR_STANDBY:
    default:
        pst->CorrPI.i10=0;
        break;
    }
    return pst->VolttargetCorr;
}

inline void GenModulation(void)
{
        DBG_leadSampleIncrementDuty = harmCompPerc;
        DBG_leadSampleDecrementDuty = -reactPrCompPerc;
    //    float PwmDutyA = 0,PwmDutyB = 0,PwmDutyC = 0;
//        int T1PR_INT16 = (int)T1PR;
        T1PR_INT16 = (int)T1PR;
//        int16 tmp;
//        #define GET_DUTY_LIMIT(reg,x) tmp = (int)(T1PR*(x)); (reg) = (tmp)<(T1PR_INT16)? (tmp) :(T1PR_INT16-1);   //CLA提前采样点
#if ESC_THREEPHASE
        if(((StateFlag.VoltageMode==3)||(StateFlag.VoltageMode==0))&&\
            (StateEventFlag_A == STATE_EVENT_RUN_A)&&(ESCFlagA.PWM_ins_index == 0)&&\
            (StateEventFlag_B == STATE_EVENT_RUN_B)&&(ESCFlagB.PWM_ins_index == 0)&&\
            (StateEventFlag_C == STATE_EVENT_RUN_C)&&(ESCFlagC.PWM_ins_index == 0) )//不平衡模式
        {
            UnCurrCompCnt=0;
            UnCurrCompFUN();
        }else{
            CurrentUnbalanceRegularVoltage[0] = ESCFlagA.Volttarget;
            CurrentUnbalanceRegularVoltage[1] = ESCFlagB.Volttarget;
            CurrentUnbalanceRegularVoltage[2] = ESCFlagC.Volttarget;
        }
            if(ESCFlagA.PWM_ins_index != 0){         //重要,    在待机和运行切换之间有个占空比为100的脉冲波形.
    //            ESCFlagA.PWMcurrDirFlag = 0;         //由CPU更新EPwm4,5,6Reg
                EPwm3Regs.CMPA.bit.CMPA = T1PR;     //1管直通      //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
                EPwm3Regs.CMPB.bit.CMPB = T1PR;     //2管不直通
                EPwm4Regs.CMPA.bit.CMPA = T1PR;     //3管直通
                EPwm4Regs.CMPB.bit.CMPB = T1PR;     //4管不直通
                PIVolA.i10 = 0;                  //清除PI的饱和值
                PIVolA.i6 = 0;
                GridCurrPICtrlA.i10 = 0;
                GridCurrPICtrlA.i6 = 0;
                ConstantCurr[0].state = SM_CONSTANT_CURR_STANDBY;
                ESC_FeedForward_DutyA =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
            }else if((StateEventFlag_A == STATE_EVENT_RUN_A)&&(ESCFlagA.PWM_ins_index == 0)){   //进入正式IGBT运行状态之后才进行占空比的加,减运算
    //            ESCFlagA.PWMcurrDirFlag = 1;         //由CLA更新EPwm4,5,6Reg

                switch(StateFlag.VoltageMode){
                case 0: //升压模式
                     //恒流模式

                    VolttargetCorrA = ConstantCurrFSM(&ConstantCurr[0],gridCurA_rms,GridCurrAF,CurrTargetTemper);
                    PIOutVoltValueA = DCL_runPI(&PIVolA,(CurrentUnbalanceRegularVoltage[0]*(1+VolttargetCorrA)),VoltOutA_rms);
                    ESCFlagA.ESC_DutyData = RMSDutyLimit(VoltInA_rms,VoltInAF,(CurrentUnbalanceRegularVoltage[0]*(1+VolttargetCorrA)),PIOutVoltValueA,Esc_VoltPhaseA,&ESC_FeedForward_DutyA,ESCFlagA.PHASE);

                    break;
                case 1://降压模式
                    PIOutVoltValueA = DCL_runPI(&PIVolA,ESCFlagA.Volttarget,VoltOutA_rms);
                    ESCFlagA.ESC_DutyData = RMSDutyLimit(VoltInA_rms,VoltInAF,ESCFlagA.Volttarget,PIOutVoltValueA,Esc_VoltPhaseA,&ESC_FeedForward_DutyA,ESCFlagA.PHASE);
                    break;
                case 2:break;//升降压模式
                case 3://不平衡模式
                    VolttargetCorrA = ConstantCurrFSM(&ConstantCurr[0],gridCurA_rms,GridCurrAF,CurrTargetTemper);
                    PIOutVoltValueA = DCL_runPI(&PIVolA,CurrentUnbalanceRegularVoltage[0]*(1+VolttargetCorrA),VoltOutA_rms);
                    ESCFlagA.ESC_DutyData = RMSDutyLimit(VoltInA_rms,VoltInAF,CurrentUnbalanceRegularVoltage[0],PIOutVoltValueA,Esc_VoltPhaseA,&ESC_FeedForward_DutyA,ESCFlagA.PHASE);
                    break;
                case 4://无功模式
                    PIOutVoltValueA = DCL_runPI(&PIVolA,ESCFlagA.Volttarget,VoltOutA_rms);
                    dbg_reactive=ReactivePowerComFUN(Esc_VoltPhaseA,GridRealCurErrA);
                    ESCFlagA.ESC_DutyData = RMSDutyLimit(VoltInA_rms,VoltInAF,ESCFlagA.Volttarget,PIOutVoltValueA,Esc_VoltPhaseA,&ESC_FeedForward_DutyA,ESCFlagA.PHASE)-dbg_reactive;
     //                       ReactivePowerComFUN(Esc_VoltPhaseA,GridRealCurErrA);
                    break;
            }

                T1PRmulESC_DutyDataAaddnegCurCompPerc = ((int32)(T1PR * ((ESCFlagA.ESC_DutyData + (negCurCompPerc/2))>1.0f ? 1.0f: ESCFlagA.ESC_DutyData + (negCurCompPerc/2))))/*<<16*/;
                T1PRmulESC_DutyDataAsubnegCurCompPerc = ((int32)(T1PR * ((ESCFlagA.ESC_DutyData - (negCurCompPerc/2))>1.0f ? 1.0f: ESCFlagA.ESC_DutyData - (negCurCompPerc/2))))/*<<16*/;
                T1PRmulESC_DutyDataA = ((int32)(T1PR * ESCFlagA.ESC_DutyData))/*<<16*/;
                if(VoltInAF >= 35){                 //电压正半波   UAIP+/UAIN+为1时,无互锁逻辑  //硬件新加互锁逻辑判断的电压值与软件采样的电压值在相位上相差约200us,所以这个范围值要设大一些,把硬件判断电压值包进去.
                    EPwm3Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataA;  //2
                    EPwm3Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataA;  //1,buck管
                    EPwm4Regs.CMPB.bit.CMPB = 0;                 //4,常通,续流管
                    EPwm4Regs.CMPA.bit.CMPA = T1PR;              //3,常通,续流管       //3,常关
                }else if(VoltInAF <= (-35)){        //电压负半波
                   EPwm3Regs.CMPB.bit.CMPB = 0;                 //2,常通,续流管
                   EPwm3Regs.CMPA.bit.CMPA = T1PR;              //1,常通续流管        //1,低
                   EPwm4Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataA;  //4
                   EPwm4Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataA;  //3,buck管
                }else{
                    if((VoltInAF >= 0)&&(VoltInAF < 35)){         //NESPWS20-220524REV2功率板--NPC3CA10020220518REV1合成板
                        if(GridCurrAF >= 0){
                            EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //1管
                            EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //2管
                            EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //3管
                            EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //4管
                        }else{
                            EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //1管
                            EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //2管
                            EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //3管
                            EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //4管
                        }
                    }else if((VoltInAF < 0)&&(VoltInAF > (-35))){
                        if(GridCurrAF >= 0){
                            EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //1管
                            EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //2管
                            EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //3管
                            EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //4管
                        }else{
                            EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //1管
                            EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //2管
                            EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //3管
                            EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //4管
                        }
                    }
                }
            }else{
    //            ESCFlagA.PWMcurrDirFlag = 0;
                PIVolA.i10 = 0;                 //清除PI的饱和值
                PIVolA.i6 = 0;
                GridCurrPICtrlA.i10 = 0;
                GridCurrPICtrlA.i6 = 0;
                ConstantCurr[0].state = SM_CONSTANT_CURR_STANDBY;
                EPwm3Regs.CMPA.bit.CMPA = T1PR;     //1管  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
                EPwm3Regs.CMPB.bit.CMPB = T1PR;        //2管
                EPwm4Regs.CMPA.bit.CMPA = T1PR;     //3管
                EPwm4Regs.CMPB.bit.CMPB = T1PR;        //4管
                ESC_FeedForward_DutyA =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
            }


            if(ESCFlagB.PWM_ins_index != 0){         //重要,    在待机和运行切换之间有个占空比为100的脉冲波形.
    //            ESCFlagB.PWMcurrDirFlag = 0;
                EPwm5Regs.CMPA.bit.CMPA = T1PR;     //1管直通  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
                EPwm5Regs.CMPB.bit.CMPB = T1PR;     //2管不直通
                EPwm6Regs.CMPA.bit.CMPA = T1PR;     //3管直通
                EPwm6Regs.CMPB.bit.CMPB = T1PR;     //4管不直通
                PIVolB.i10 = 0;                 //清除PI的饱和值
                PIVolB.i6 = 0;
                GridCurrPICtrlB.i10 = 0;
                GridCurrPICtrlB.i6 = 0;
                ConstantCurr[1].state = SM_CONSTANT_CURR_STANDBY;
                ESC_FeedForward_DutyB =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比

            }else if((StateEventFlag_B == STATE_EVENT_RUN_B)&&(ESCFlagB.PWM_ins_index == 0)){   //进入正式IGBT运行状态之后才进行占空比的加,减运算
    //            ESCFlagB.PWMcurrDirFlag = 1;

                switch(StateFlag.VoltageMode){
                case 0: //升压模式
//                    VolttargetCorrB = ConstantCurrFSM(&ConstantCurr[1],gridCurB_rms,GridCurrBF,CurrTargetTemper);
//                    PIOutVoltValueB = DCL_runPI(&PIVolB,(ESCFlagB.Volttarget*(1+VolttargetCorrB)),VoltOutB_rms);
//                    ESCFlagB.ESC_DutyData = RMSDutyLimit(VoltInB_rms,VoltInBF,(ESCFlagB.Volttarget*(1+VolttargetCorrB)),PIOutVoltValueB,Esc_VoltPhaseB,&ESC_FeedForward_DutyB,ESCFlagB.PHASE);
                    VolttargetCorrB = ConstantCurrFSM(&ConstantCurr[1],gridCurB_rms,GridCurrBF,CurrTargetTemper);
                    PIOutVoltValueB = DCL_runPI(&PIVolB,(CurrentUnbalanceRegularVoltage[1]*(1+VolttargetCorrB)),VoltOutB_rms);
                    ESCFlagB.ESC_DutyData = RMSDutyLimit(VoltInB_rms,VoltInBF,(CurrentUnbalanceRegularVoltage[1]*(1+VolttargetCorrB)),PIOutVoltValueB,Esc_VoltPhaseB,&ESC_FeedForward_DutyB,ESCFlagB.PHASE);
                    break;
                case 1: //升压模式
                    PIOutVoltValueB = DCL_runPI(&PIVolB,ESCFlagB.Volttarget,VoltOutB_rms);
                    ESCFlagB.ESC_DutyData = RMSDutyLimit(VoltInB_rms,VoltInBF,ESCFlagB.Volttarget,PIOutVoltValueB,Esc_VoltPhaseB,&ESC_FeedForward_DutyB,ESCFlagB.PHASE);
                    break;
                case 2:
                    break;//升降压模式
                case 3://不平衡模式
                    VolttargetCorrB = ConstantCurrFSM(&ConstantCurr[1],gridCurB_rms,GridCurrBF,CurrTargetTemper);
                    PIOutVoltValueB = DCL_runPI(&PIVolB,CurrentUnbalanceRegularVoltage[1]*(1+VolttargetCorrB),VoltOutB_rms);
                    ESCFlagB.ESC_DutyData = RMSDutyLimit(VoltInB_rms,VoltInBF,CurrentUnbalanceRegularVoltage[1],PIOutVoltValueB,Esc_VoltPhaseB,&ESC_FeedForward_DutyB,ESCFlagB.PHASE);
                    break;
                case 4://无功模式
                    PIOutVoltValueB = DCL_runPI(&PIVolB,ESCFlagB.Volttarget,VoltOutB_rms);
                    ESCFlagB.ESC_DutyData = RMSDutyLimit(VoltInB_rms,VoltInBF,ESCFlagB.Volttarget,PIOutVoltValueB,Esc_VoltPhaseB,&ESC_FeedForward_DutyB,ESCFlagB.PHASE)-\
                            ReactivePowerComFUN(Esc_VoltPhaseB,GridRealCurErrB);
                    break;
            }

                T1PRmulESC_DutyDataBaddnegCurCompPerc = ((int32)(T1PR * ((ESCFlagB.ESC_DutyData + (negCurCompPerc/2))>1.0f ? 1.0f: ESCFlagB.ESC_DutyData + (negCurCompPerc/2))))/*<<16*/;
                T1PRmulESC_DutyDataBsubnegCurCompPerc = ((int32)(T1PR * ((ESCFlagB.ESC_DutyData - (negCurCompPerc/2))>1.0f ? 1.0f: ESCFlagB.ESC_DutyData - (negCurCompPerc/2))))/*<<16*/;
                T1PRmulESC_DutyDataB = ((int32)(T1PR * ESCFlagB.ESC_DutyData))/*<<16*/;
                if(VoltInBF >= 35){                 //电压正半波
                    EPwm5Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataB;  //2
                    EPwm5Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataB;  //1,buck管
                    EPwm6Regs.CMPB.bit.CMPB = 0;                 //4,常通,续流管
                    EPwm6Regs.CMPA.bit.CMPA = T1PR;              //3,常通,续流管       //3,常关
                }else if(VoltInBF <= (-35)){         //电压负半波
                   EPwm5Regs.CMPB.bit.CMPB = 0;                 //2,常通,续流管
                   EPwm5Regs.CMPA.bit.CMPA = T1PR;              //1,常通续流管        //1,低
                   EPwm6Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataB;  //4
                   EPwm6Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataB;  //3,buck管
                }else{
                    if((VoltInBF >= 0)&&(VoltInBF < 35)){
                        if(GridCurrBF >= 0){
                            EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //1管
                            EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //2管
                            EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //3管
                            EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //4管
                        }else{
                            EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //1管
                            EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //2管
                            EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //3管
                            EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //4管
                        }
                    }else if((VoltInBF < 0)&&(VoltInBF > (-35))){
                        if(GridCurrBF >= 0){
                            EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //1管
                            EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //2管
                            EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //3管
                            EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //4管
                        }else{
                            EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //1管
                            EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //2管
                            EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //3管
                            EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //4管
                        }
                    }
                }
            }else{
    //            ESCFlagB.PWMcurrDirFlag = 0;
                PIVolB.i10 = 0;                 //清除PI的饱和值
                PIVolB.i6 = 0;
                GridCurrPICtrlB.i10 = 0;
                GridCurrPICtrlB.i6 = 0;
                ConstantCurr[1].state = SM_CONSTANT_CURR_STANDBY;
                EPwm5Regs.CMPA.bit.CMPA = T1PR;     //1管  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
                EPwm5Regs.CMPB.bit.CMPB = T1PR;        //2管
                EPwm6Regs.CMPA.bit.CMPA = T1PR;     //3管
                EPwm6Regs.CMPB.bit.CMPB = T1PR;        //4管
                ESC_FeedForward_DutyB =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
            }


            if(ESCFlagC.PWM_ins_index != 0){         //重要,    在待机和运行切换之间有个占空比为100的脉冲波形.
    //            ESCFlagC.PWMcurrDirFlag = 0;
                EPwm7Regs.CMPA.bit.CMPA = T1PR;     //1管直通  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
                EPwm7Regs.CMPB.bit.CMPB = T1PR;     //2管不直通
                EPwm8Regs.CMPA.bit.CMPA = T1PR;     //3管直通
                EPwm8Regs.CMPB.bit.CMPB = T1PR;     //4管不直通
                PIVolC.i10 = 0;                 //清除PI的饱和值
                PIVolC.i6 = 0;
                GridCurrPICtrlC.i10 = 0;
                GridCurrPICtrlC.i6 = 0;
                ConstantCurr[2].state = SM_CONSTANT_CURR_STANDBY;
                ESC_FeedForward_DutyC =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
            }else if((StateEventFlag_C == STATE_EVENT_RUN_C)&&(ESCFlagC.PWM_ins_index == 0)){   //进入正式IGBT运行状态之后才进行占空比的加,减运算
    //            ESCFlagC.PWMcurrDirFlag = 1;

                switch(StateFlag.VoltageMode){
                case 0: //升压模式
                    //恒流模式
//                    VolttargetCorrC = ConstantCurrFSM(&ConstantCurr[2],gridCurC_rms,GridCurrCF,CurrTargetTemper);
//                    PIOutVoltValueC = DCL_runPI(&PIVolC,(ESCFlagC.Volttarget*(1+VolttargetCorrC)),VoltOutC_rms);       //PI闭环跟踪
//                    ESCFlagC.ESC_DutyData = RMSDutyLimit(VoltInC_rms,VoltInCF,(ESCFlagC.Volttarget*(1+VolttargetCorrC)),PIOutVoltValueC,Esc_VoltPhaseC,&ESC_FeedForward_DutyC,ESCFlagC.PHASE);
                    VolttargetCorrC = ConstantCurrFSM(&ConstantCurr[2],gridCurC_rms,GridCurrCF,CurrTargetTemper);
                    PIOutVoltValueC = DCL_runPI(&PIVolC,(CurrentUnbalanceRegularVoltage[2]*(1+VolttargetCorrC)),VoltOutC_rms);       //PI闭环跟踪
                    ESCFlagC.ESC_DutyData = RMSDutyLimit(VoltInC_rms,VoltInCF,(CurrentUnbalanceRegularVoltage[2]*(1+VolttargetCorrC)),PIOutVoltValueC,Esc_VoltPhaseC,&ESC_FeedForward_DutyC,ESCFlagC.PHASE);
                    break;
                case 1: //降压模式
                    PIOutVoltValueC = DCL_runPI(&PIVolC,ESCFlagC.Volttarget,VoltOutC_rms);       //PI闭环跟踪
                    ESCFlagC.ESC_DutyData = RMSDutyLimit(VoltInC_rms,VoltInCF,ESCFlagC.Volttarget,PIOutVoltValueC,Esc_VoltPhaseC,&ESC_FeedForward_DutyC,ESCFlagC.PHASE);
                    break;
                case 2:
                    break;//升降压模式
                case 3://不平衡模式
                    VolttargetCorrC = ConstantCurrFSM(&ConstantCurr[2],gridCurC_rms,GridCurrCF,CurrTargetTemper);
                    PIOutVoltValueC = DCL_runPI(&PIVolC,CurrentUnbalanceRegularVoltage[2]*(1+VolttargetCorrC),VoltOutC_rms);       //PI闭环跟踪
                    ESCFlagC.ESC_DutyData = RMSDutyLimit(VoltInC_rms,VoltInCF,CurrentUnbalanceRegularVoltage[2],PIOutVoltValueC,Esc_VoltPhaseC,&ESC_FeedForward_DutyC,ESCFlagC.PHASE);
                    break;
                case 4://无功模式
                    PIOutVoltValueC = DCL_runPI(&PIVolC,ESCFlagC.Volttarget,VoltOutC_rms);       //PI闭环跟踪
                    ESCFlagC.ESC_DutyData = RMSDutyLimit(VoltInC_rms,VoltInCF,ESCFlagC.Volttarget,PIOutVoltValueC,Esc_VoltPhaseC,&ESC_FeedForward_DutyC,ESCFlagC.PHASE)-\
                            ReactivePowerComFUN(Esc_VoltPhaseC,GridRealCurErrC);
                    break;
                }

                T1PRmulESC_DutyDataCaddnegCurCompPerc = ((int32)(T1PR * ((ESCFlagC.ESC_DutyData + (negCurCompPerc/2))>1.0f ? 1.0f: ESCFlagC.ESC_DutyData + (negCurCompPerc/2))))/*<<16*/;
                T1PRmulESC_DutyDataCsubnegCurCompPerc = ((int32)(T1PR * ((ESCFlagC.ESC_DutyData - (negCurCompPerc/2))>1.0f ? 1.0f: ESCFlagC.ESC_DutyData - (negCurCompPerc/2))))/*<<16*/;
                T1PRmulESC_DutyDataC = ((int32)(T1PR * ESCFlagC.ESC_DutyData))/*<<16*/;
                if(VoltInCF >= 35){                  //电压正半波
                    EPwm7Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataC;  //2
                    EPwm7Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataC;  //1,buck管
                    EPwm8Regs.CMPB.bit.CMPB = 0;                 //4,常通,续流管
                    EPwm8Regs.CMPA.bit.CMPA = T1PR;              //3,常通,续流管       //3,常关
                }else if(VoltInCF <= (-35)){         //电压负半波
                    EPwm7Regs.CMPB.bit.CMPB = 0;                 //2,常通,续流管
                    EPwm7Regs.CMPA.bit.CMPA = T1PR;              //1,常通续流管        //1,低
                    EPwm8Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataC;  //4
                    EPwm8Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataC;  //3,buck管
                }else{
                    if((VoltInCF >= 0)&&(VoltInCF < 35)){
                        if(GridCurrCF >= 0){
                            EPwm7Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //1管
                            EPwm7Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //2管
                            EPwm8Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //3管
                            EPwm8Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //4管
                        }else{
                            EPwm7Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //1管
                            EPwm7Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //2管
                            EPwm8Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //3管
                            EPwm8Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //4管
                        }
                    }else if((VoltInCF < 0)&&(VoltInCF > (-35))){
                        if(GridCurrCF >= 0){
                            EPwm7Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //1管
                            EPwm7Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //2管
                            EPwm8Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //3管
                            EPwm8Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //4管
                        }else{
                            EPwm7Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //1管
                            EPwm7Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //2管
                            EPwm8Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //3管
                            EPwm8Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //4管
                        }
                    }
                }
            }else{
    //            ESCFlagC.PWMcurrDirFlag = 0;
                PIVolC.i10 = 0;                 //清除PI的饱和值
                PIVolC.i6 = 0;
                GridCurrPICtrlC.i10 = 0;
                GridCurrPICtrlC.i6 = 0;
                ConstantCurr[2].state = SM_CONSTANT_CURR_STANDBY;
                EPwm7Regs.CMPA.bit.CMPA = T1PR;     //1管  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
                EPwm7Regs.CMPB.bit.CMPB = T1PR;        //2管
                EPwm8Regs.CMPA.bit.CMPA = T1PR;     //3管
                EPwm8Regs.CMPB.bit.CMPB = T1PR;        //4管
                ESC_FeedForward_DutyC =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
            }
#elif ESC_SINGLEPHASE
        if((PWM_ins_indexA != 0)||(PhaseControl&0x04)){         //重要,    在待机和运行切换之间有个占空比为100的脉冲波形.
//            ESCFlagA.PWMcurrDirFlag = 0;         //由CPU更新EPwm4,5,6Reg
            EPwm3Regs.CMPA.bit.CMPA = T1PR;     //1管直通      //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
            EPwm3Regs.CMPB.bit.CMPB = T1PR;     //2管不直通
            EPwm4Regs.CMPA.bit.CMPA = T1PR;     //3管直通
            EPwm4Regs.CMPB.bit.CMPB = T1PR;     //4管不直通
            PIVolA.i10 = 0;                  //清除PI的饱和值
            PIVolA.i6 = 0;
            ESC_FeedForward_DutyA =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
        }else if((StateEventFlag_A == STATE_EVENT_RUN_A)&&(PWM_ins_indexA == 0)){   //进入正式IGBT运行状态之后才进行占空比的加,减运算
//            ESCFlagA.PWMcurrDirFlag = 1;         //由CLA更新EPwm4,5,6Reg
            PIOutVoltValueA = DCL_runPI(&PIVolA,ESCFlagA.Volttarget,VoltOutA_rms);
        if(StateFlag.VoltageModeFlag == 0){
//            ESC_DutyDataA = RMSDutyLimit(VoltInA_rms,VoltInAF,ESCFlagA.Volttarget,PIOutVoltValueA,Esc_VoltPhaseA,&ESC_FeedForward_DutyA);
            ESC_DutyDataA = TIPRTEST;
        }else if(StateFlag.VoltageModeFlag == 1){
//            ESC_DutyDataA = RMSDutyLimit(VoltInA_rms,VoltInAF,ESCFlagA.Volttarget,PIOutVoltValueA,Esc_VoltPhaseA,&ESC_FeedForward_DutyA);
            ESC_DutyDataA = TIPRTEST;
        }
            T1PRmulESC_DutyDataAaddnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataA + (negCurCompPerc/2))>1.0f ? 1.0f: ESC_DutyDataA + (negCurCompPerc/2))))/*<<16*/;
            T1PRmulESC_DutyDataAsubnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataA - (negCurCompPerc/2))>1.0f ? 1.0f: ESC_DutyDataA - (negCurCompPerc/2))))/*<<16*/;
            T1PRmulESC_DutyDataA = ((int32)(T1PR * ESC_DutyDataA))/*<<16*/;

            if(VoltInAF >= 25){                 //电压正半波
                EPwm3Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataA;  //2
                EPwm3Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataA;  //1,buck管
                EPwm4Regs.CMPB.bit.CMPB = 0;                 //4,常通,续流管
                EPwm4Regs.CMPA.bit.CMPA = T1PR;              //3,常通,续流管       //3,常关
            }else if(VoltInAF <= (-25)){         //电压负半波
               EPwm3Regs.CMPB.bit.CMPB = 0;                 //2,常通,续流管
               EPwm3Regs.CMPA.bit.CMPA = T1PR;              //1,常通续流管        //1,低
               EPwm4Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataA;  //4
               EPwm4Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataA;  //3,buck管
            }else{
                if((VoltInAF > 0)&&(VoltInAF < 25)){         //NESPWS20-220524REV2功率板--NPC3CA10020220518REV1合成板
                    if(GridCurrAF > 0){
                        EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //1管
                        EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //2管
                        EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //3管
                        EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //4管
                    }else{
                        EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //1管
                        EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //2管
                        EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //3管
                        EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //4管
                    }
                }else if((VoltInAF < 0)&&(VoltInAF > (-25))){
                    if(GridCurrAF > 0){
                        EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //1管
                        EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //2管
                        EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //3管
                        EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //4管
                    }else{
                        EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //1管
                        EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //2管
                        EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //3管
                        EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //4管
                    }
                }
            }
        }else{
//            ESCFlagA.PWMcurrDirFlag = 0;
            PIVolA.i10 = 0;                 //清除PI的饱和值
            PIVolA.i6 = 0;
            EPwm3Regs.CMPA.bit.CMPA = T1PR;     //1管  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
            EPwm3Regs.CMPB.bit.CMPB = T1PR;        //2管
            EPwm4Regs.CMPA.bit.CMPA = T1PR;     //3管
            EPwm4Regs.CMPB.bit.CMPB = T1PR;        //4管
            ESC_FeedForward_DutyA =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
        }

#elif ESC_ONEINTWOPHASE
        if((PWM_ins_indexA != 0)||(PhaseControl&0x04)){         //重要,    在待机和运行切换之间有个占空比为100的脉冲波形.
//            ESCFlagA.PWMcurrDirFlag = 0;         //由CPU更新EPwm4,5,6Reg
            EPwm3Regs.CMPA.bit.CMPA = T1PR;     //1管直通      //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
            EPwm3Regs.CMPB.bit.CMPB = T1PR;     //2管不直通
            EPwm4Regs.CMPA.bit.CMPA = T1PR;     //3管直通
            EPwm4Regs.CMPB.bit.CMPB = T1PR;     //4管不直通
            PIVolA.i10 = 0;                  //清除PI的饱和值
            PIVolA.i6 = 0;
            ESC_FeedForward_DutyA =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
        }else if((StateEventFlag_A == STATE_EVENT_RUN_A)&&(PWM_ins_indexA == 0)){   //进入正式IGBT运行状态之后才进行占空比的加,减运算
//            ESCFlagA.PWMcurrDirFlag = 1;         //由CLA更新EPwm4,5,6Reg
            PIOutVoltValueA = DCL_runPI(&PIVolA,ESCFlagA.Volttarget,VoltOutA_rms);
        if(StateFlag.VoltageModeFlag == 0){
//            ESC_DutyDataA = RMSDutyLimit(VoltInA_rms,VoltInAF,ESCFlagA.Volttarget,PIOutVoltValueA,Esc_VoltPhaseA,&ESC_FeedForward_DutyA);
            ESC_DutyDataA = TIPRTEST;
        }else if(StateFlag.VoltageModeFlag == 1){
//            ESC_DutyDataA = RMSDutyLimit(VoltInA_rms,VoltInAF,ESCFlagA.Volttarget,PIOutVoltValueA,Esc_VoltPhaseA,&ESC_FeedForward_DutyA);
            ESC_DutyDataA = TIPRTEST;
        }
            T1PRmulESC_DutyDataAaddnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataA + (negCurCompPerc/2))>1.0f ? 1.0f: ESC_DutyDataA + (negCurCompPerc/2))))/*<<16*/;
            T1PRmulESC_DutyDataAsubnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataA - (negCurCompPerc/2))>1.0f ? 1.0f: ESC_DutyDataA - (negCurCompPerc/2))))/*<<16*/;
            T1PRmulESC_DutyDataA = ((int32)(T1PR * ESC_DutyDataA))/*<<16*/;
            if(VoltInAF >= 25){                 //电压正半波
                EPwm3Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataA;  //2
                EPwm3Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataA;  //1,buck管
                EPwm4Regs.CMPB.bit.CMPB = 0;                 //4,常通,续流管
                EPwm4Regs.CMPA.bit.CMPA = T1PR;              //3,常通,续流管       //3,常关
            }else if(VoltInAF <= (-25)){         //电压负半波
               EPwm3Regs.CMPB.bit.CMPB = 0;                 //2,常通,续流管
               EPwm3Regs.CMPA.bit.CMPA = T1PR;              //1,常通续流管        //1,低
               EPwm4Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataA;  //4
               EPwm4Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataA;  //3,buck管
            }else{
                if((VoltInAF > 0)&&(VoltInAF < 25)){         //NESPWS20-220524REV2功率板--NPC3CA10020220518REV1合成板
                    if(GridCurrAF > 0){
                        EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //1管
                        EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //2管
                        EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //3管
                        EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //4管
                    }else{
                        EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //1管
                        EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //2管
                        EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //3管
                        EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //4管
                    }
                }else if((VoltInAF < 0)&&(VoltInAF > (-25))){
                    if(GridCurrAF > 0){
                        EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //1管
                        EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //2管
                        EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //3管
                        EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //4管
                    }else{
                        EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //1管
                        EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //2管
                        EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //3管
                        EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //4管
                    }
                }
            }
        }else{
//            ESCFlagA.PWMcurrDirFlag = 0;
            PIVolA.i10 = 0;                 //清除PI的饱和值
            PIVolA.i6 = 0;
            EPwm3Regs.CMPA.bit.CMPA = T1PR;     //1管  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
            EPwm3Regs.CMPB.bit.CMPB = T1PR;        //2管
            EPwm4Regs.CMPA.bit.CMPA = T1PR;     //3管
            EPwm4Regs.CMPB.bit.CMPB = T1PR;        //4管
            ESC_FeedForward_DutyA =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
        }

        if((PWM_ins_indexB != 0)||(PhaseControl&0x02)){         //重要,    在待机和运行切换之间有个占空比为100的脉冲波形.
//            ESCFlagB.PWMcurrDirFlag = 0;
            EPwm5Regs.CMPA.bit.CMPA = T1PR;     //1管直通  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
            EPwm5Regs.CMPB.bit.CMPB = T1PR;     //2管不直通
            EPwm6Regs.CMPA.bit.CMPA = T1PR;     //3管直通
            EPwm6Regs.CMPB.bit.CMPB = T1PR;     //4管不直通
            PIVolB.i10 = 0;                 //清除PI的饱和值
            PIVolB.i6 = 0;
            ESC_FeedForward_DutyB =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比

        }else if((StateEventFlag_B == STATE_EVENT_RUN_B)&&(PWM_ins_indexB == 0)){   //进入正式IGBT运行状态之后才进行占空比的加,减运算
//            ESCFlagB.PWMcurrDirFlag = 1;
//            PIOutVoltValueB = DCL_runPI(&PIVolB,ESCFlagB.Volttarget,VoltOutB_rms);
//        if(StateFlag.VoltageModeFlag == 0){
////            ESC_DutyDataB = RMSDutyLimit(VoltInB_rms,VoltInBF,ESCFlagB.Volttarget,PIOutVoltValueB,Esc_VoltPhaseB,&ESC_FeedForward_DutyB);
//            ESC_DutyDataB = 0.8;
//        }else if(StateFlag.VoltageModeFlag == 1){
////            ESC_DutyDataB = RMSDutyLimit(VoltInB_rms,VoltInBF,ESCFlagB.Volttarget,PIOutVoltValueB,Esc_VoltPhaseB,&ESC_FeedForward_DutyB);
//            ESC_DutyDataB = 0.8;
//        }
            T1PRmulESC_DutyDataBaddnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataA + (negCurCompPerc/2))>1.0f ? 1.0f: ESC_DutyDataA + (negCurCompPerc/2))))/*<<16*/;
            T1PRmulESC_DutyDataBsubnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataA - (negCurCompPerc/2))>1.0f ? 1.0f: ESC_DutyDataA - (negCurCompPerc/2))))/*<<16*/;
            T1PRmulESC_DutyDataB = ((int32)(T1PR * ESC_DutyDataA))/*<<16*/;
            if(VoltInAF >= 25){                 //电压正半波
                EPwm5Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataB;  //2
                EPwm5Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataB;  //1,buck管
                EPwm6Regs.CMPB.bit.CMPB = 0;                 //4,常通,续流管
                EPwm6Regs.CMPA.bit.CMPA = T1PR;              //3,常通,续流管       //3,常关
            }else if(VoltInAF <= (-25)){         //电压负半波
               EPwm5Regs.CMPB.bit.CMPB = 0;                 //2,常通,续流管
               EPwm5Regs.CMPA.bit.CMPA = T1PR;              //1,常通续流管        //1,低
               EPwm6Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataB;  //4
               EPwm6Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataB;  //3,buck管
            }else{
                if((VoltInAF > 0)&&(VoltInAF < 25)){
                    if(GridCurrAF > 0){
                        EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //1管
                        EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //2管
                        EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //3管
                        EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //4管
                    }else{
                        EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //1管
                        EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //2管
                        EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //3管
                        EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //4管
                    }
                }else if((VoltInAF < 0)&&(VoltInAF > (-25))){
                    if(GridCurrAF > 0){
                        EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //1管
                        EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //2管
                        EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //3管
                        EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //4管
                    }else{
                        EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //1管
                        EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //2管
                        EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //3管
                        EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //4管
                    }
                }
            }
        }else{
//            ESCFlagB.PWMcurrDirFlag = 0;
            PIVolB.i10 = 0;                 //清除PI的饱和值
            PIVolB.i6 = 0;
            EPwm5Regs.CMPA.bit.CMPA = T1PR;     //1管  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
            EPwm5Regs.CMPB.bit.CMPB = T1PR;        //2管
            EPwm6Regs.CMPA.bit.CMPA = T1PR;     //3管
            EPwm6Regs.CMPB.bit.CMPB = T1PR;        //4管
            ESC_FeedForward_DutyB =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
        }
#elif ESC_ONEINTHREEPHASE
        if((PWM_ins_indexA != 0)||(PhaseControl&0x04)){         //重要,    在待机和运行切换之间有个占空比为100的脉冲波形.
//            ESCFlagA.PWMcurrDirFlag = 0;         //由CPU更新EPwm4,5,6Reg
            EPwm3Regs.CMPA.bit.CMPA = T1PR;     //1管直通      //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
            EPwm3Regs.CMPB.bit.CMPB = T1PR;     //2管不直通
            EPwm4Regs.CMPA.bit.CMPA = T1PR;     //3管直通
            EPwm4Regs.CMPB.bit.CMPB = T1PR;     //4管不直通
            PIVolA.i10 = 0;                  //清除PI的饱和值
            PIVolA.i6 = 0;
            ESC_FeedForward_DutyA =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
        }else if((StateEventFlag_A == STATE_EVENT_RUN_A)&&(PWM_ins_indexA == 0)){   //进入正式IGBT运行状态之后才进行占空比的加,减运算
//            ESCFlagA.PWMcurrDirFlag = 1;         //由CLA更新EPwm4,5,6Reg
            PIOutVoltValueA = DCL_runPI(&PIVolA,ESCFlagA.Volttarget,VoltOutA_rms);
        if(StateFlag.VoltageModeFlag == 0){
//            ESC_DutyDataA = RMSDutyLimit(VoltInA_rms,VoltInAF,ESCFlagA.Volttarget,PIOutVoltValueA,Esc_VoltPhaseA,&ESC_FeedForward_DutyA);
            ESC_DutyDataA = TIPRTEST;
        }else if(StateFlag.VoltageModeFlag == 1){
//            ESC_DutyDataA = RMSDutyLimit(VoltInA_rms,VoltInAF,ESCFlagA.Volttarget,PIOutVoltValueA,Esc_VoltPhaseA,&ESC_FeedForward_DutyA);
            ESC_DutyDataA = TIPRTEST;
        }
            T1PRmulESC_DutyDataAaddnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataA + (negCurCompPerc/2))>1.0f ? 1.0f: ESC_DutyDataA + (negCurCompPerc/2))))/*<<16*/;
            T1PRmulESC_DutyDataAsubnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataA - (negCurCompPerc/2))>1.0f ? 1.0f: ESC_DutyDataA - (negCurCompPerc/2))))/*<<16*/;
            T1PRmulESC_DutyDataA = ((int32)(T1PR * ESC_DutyDataA))/*<<16*/;
            if(VoltInAF >= 25){                 //电压正半波
                EPwm3Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataA;  //2
                EPwm3Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataA;  //1,buck管
                EPwm4Regs.CMPB.bit.CMPB = 0;                 //4,常通,续流管
                EPwm4Regs.CMPA.bit.CMPA = T1PR;              //3,常通,续流管       //3,常关
            }else if(VoltInAF <= (-25)){         //电压负半波
               EPwm3Regs.CMPB.bit.CMPB = 0;                 //2,常通,续流管
               EPwm3Regs.CMPA.bit.CMPA = T1PR;              //1,常通续流管        //1,低
               EPwm4Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataA;  //4
               EPwm4Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataA;  //3,buck管
            }else{
                if((VoltInAF > 0)&&(VoltInAF < 25)){         //NESPWS20-220524REV2功率板--NPC3CA10020220518REV1合成板
                    if(GridCurrAF > 0){
                        EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //1管
                        EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //2管
                        EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //3管
                        EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //4管
                    }else{
                        EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //1管
                        EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //2管
                        EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //3管
                        EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //4管
                    }
                }else if((VoltInAF < 0)&&(VoltInAF > (-25))){
                    if(GridCurrAF > 0){
                        EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //1管
                        EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //2管
                        EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //3管
                        EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //4管
                    }else{
                        EPwm3Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //1管
                        EPwm3Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAaddnegCurCompPerc); //2管
                        EPwm4Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //3管
                        EPwm4Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataAsubnegCurCompPerc); //4管
                    }
                }
            }
        }else{
//            ESCFlagA.PWMcurrDirFlag = 0;
            PIVolA.i10 = 0;                 //清除PI的饱和值
            PIVolA.i6 = 0;
            EPwm3Regs.CMPA.bit.CMPA = T1PR;     //1管  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
            EPwm3Regs.CMPB.bit.CMPB = T1PR;        //2管
            EPwm4Regs.CMPA.bit.CMPA = T1PR;     //3管
            EPwm4Regs.CMPB.bit.CMPB = T1PR;        //4管
            ESC_FeedForward_DutyA =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
        }

        if((PWM_ins_indexB != 0)||(PhaseControl&0x02)){         //重要,    在待机和运行切换之间有个占空比为100的脉冲波形.
//            ESCFlagB.PWMcurrDirFlag = 0;
            EPwm5Regs.CMPA.bit.CMPA = T1PR;     //1管直通  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
            EPwm5Regs.CMPB.bit.CMPB = T1PR;     //2管不直通
            EPwm6Regs.CMPA.bit.CMPA = T1PR;     //3管直通
            EPwm6Regs.CMPB.bit.CMPB = T1PR;     //4管不直通
            PIVolB.i10 = 0;                 //清除PI的饱和值
            PIVolB.i6 = 0;
            ESC_FeedForward_DutyB =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比

        }else if((StateEventFlag_B == STATE_EVENT_RUN_B)&&(PWM_ins_indexB == 0)){   //进入正式IGBT运行状态之后才进行占空比的加,减运算
//            ESCFlagB.PWMcurrDirFlag = 1;
//            PIOutVoltValueB = DCL_runPI(&PIVolB,ESCFlagB.Volttarget,VoltOutB_rms);
//        if(StateFlag.VoltageModeFlag == 0){
////            ESC_DutyDataB = RMSDutyLimit(VoltInB_rms,VoltInBF,ESCFlagB.Volttarget,PIOutVoltValueB,Esc_VoltPhaseB,&ESC_FeedForward_DutyB);
//            ESC_DutyDataB = 0.8;
//        }else if(StateFlag.VoltageModeFlag == 1){
////            ESC_DutyDataB = RMSDutyLimit(VoltInB_rms,VoltInBF,ESCFlagB.Volttarget,PIOutVoltValueB,Esc_VoltPhaseB,&ESC_FeedForward_DutyB);
//            ESC_DutyDataB = 0.8;
//        }
//            T1PRmulESC_DutyDataBaddnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataB + negCurCompPerc)>1.0f ? 1.0f: ESC_DutyDataB + negCurCompPerc)));
//            T1PRmulESC_DutyDataBsubnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataB - negCurCompPerc)>1.0f ? 1.0f: ESC_DutyDataB - negCurCompPerc)));
            T1PRmulESC_DutyDataBaddnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataA + (negCurCompPerc/2))>1.0f ? 1.0f: ESC_DutyDataA + (negCurCompPerc/2))))/*<<16*/;
            T1PRmulESC_DutyDataBsubnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataA - (negCurCompPerc/2))>1.0f ? 1.0f: ESC_DutyDataA - (negCurCompPerc/2))))/*<<16*/;
            T1PRmulESC_DutyDataB = ((int32)(T1PR * ESC_DutyDataA))/*<<16*/;
            if(VoltInAF >= 25){                 //电压正半波
                EPwm5Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataB;  //2
                EPwm5Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataB;  //1,buck管
                EPwm6Regs.CMPB.bit.CMPB = 0;                 //4,常通,续流管
                EPwm6Regs.CMPA.bit.CMPA = T1PR;              //3,常通,续流管       //3,常关
            }else if(VoltInAF <= (-25)){         //电压负半波
               EPwm5Regs.CMPB.bit.CMPB = 0;                 //2,常通,续流管
               EPwm5Regs.CMPA.bit.CMPA = T1PR;              //1,常通续流管        //1,低
               EPwm6Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataB;  //4
               EPwm6Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataB;  //3,buck管
            }else{
                if((VoltInAF > 0)&&(VoltInAF < 25)){
                    if(GridCurrAF > 0){
                        EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //1管
                        EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //2管
                        EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //3管
                        EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //4管
                    }else{
                        EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //1管
                        EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //2管
                        EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //3管
                        EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //4管
                    }
                }else if((VoltInAF < 0)&&(VoltInAF > (-25))){
                    if(GridCurrAF > 0){
                        EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //1管
                        EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //2管
                        EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //3管
                        EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //4管
                    }else{
                        EPwm5Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //1管
                        EPwm5Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBaddnegCurCompPerc); //2管
                        EPwm6Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //3管
                        EPwm6Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataBsubnegCurCompPerc); //4管
                    }
                }
            }
        }else{
//            ESCFlagB.PWMcurrDirFlag = 0;
            PIVolB.i10 = 0;                 //清除PI的饱和值
            PIVolB.i6 = 0;
            EPwm5Regs.CMPA.bit.CMPA = T1PR;     //1管  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
            EPwm5Regs.CMPB.bit.CMPB = T1PR;        //2管
            EPwm6Regs.CMPA.bit.CMPA = T1PR;     //3管
            EPwm6Regs.CMPB.bit.CMPB = T1PR;        //4管
            ESC_FeedForward_DutyB =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
        }

        if((PWM_ins_indexC != 0)||(PhaseControl&0x01)){         //重要,    在待机和运行切换之间有个占空比为100的脉冲波形.
//            ESCFlagC.PWMcurrDirFlag = 0;
            EPwm7Regs.CMPA.bit.CMPA = T1PR;     //1管直通  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
            EPwm7Regs.CMPB.bit.CMPB = T1PR;     //2管不直通
            EPwm8Regs.CMPA.bit.CMPA = T1PR;     //3管直通
            EPwm8Regs.CMPB.bit.CMPB = T1PR;     //4管不直通
            PIVolC.i10 = 0;                 //清除PI的饱和值
            PIVolC.i6 = 0;
            ESC_FeedForward_DutyC =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比

        }else if((StateEventFlag_C == STATE_EVENT_RUN_C)&&(PWM_ins_indexC == 0)){   //进入正式IGBT运行状态之后才进行占空比的加,减运算
//            ESCFlagC.PWMcurrDirFlag = 1;
//            PIOutVoltValueC = DCL_runPI(&PIVolC,ESCFlagC.Volttarget,VoltOutC_rms);       //PI闭环跟踪
//            if(StateFlag.VoltageModeFlag == 0){   //升压
////                ESC_DutyDataC = RMSDutyLimit(VoltInC_rms,VoltInCF,ESCFlagC.Volttarget,PIOutVoltValueC,Esc_VoltPhaseC,&ESC_FeedForward_DutyC);
//                ESC_DutyDataC = 0.8;
//            }else if(StateFlag.VoltageModeFlag == 1){   //降压
////                ESC_DutyDataC = RMSDutyLimit(VoltInC_rms,VoltInCF,ESCFlagC.Volttarget,PIOutVoltValueC,Esc_VoltPhaseC,&ESC_FeedForward_DutyC);
//                ESC_DutyDataC = 0.8;
//            }
            T1PRmulESC_DutyDataCaddnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataA + (negCurCompPerc/2))>1.0f ? 1.0f: ESC_DutyDataA + (negCurCompPerc/2))))/*<<16*/;
            T1PRmulESC_DutyDataCsubnegCurCompPerc = ((int32)(T1PR * ((ESC_DutyDataA - (negCurCompPerc/2))>1.0f ? 1.0f: ESC_DutyDataA - (negCurCompPerc/2))))/*<<16*/;
            T1PRmulESC_DutyDataC = ((int32)(T1PR * ESC_DutyDataA))/*<<16*/;

            if(VoltInAF >= 25){                 //电压正半波
                EPwm7Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataC;  //2
                EPwm7Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataC;  //1,buck管
                EPwm8Regs.CMPB.bit.CMPB = 0;                 //4,常通,续流管
                EPwm8Regs.CMPA.bit.CMPA = T1PR;              //3,常通,续流管       //3,常关
           }else if(VoltInAF <= (-25)){         //电压负半波
               EPwm7Regs.CMPB.bit.CMPB = 0;                 //2,常通,续流管
               EPwm7Regs.CMPA.bit.CMPA = T1PR;              //1,常通续流管        //1,低
               EPwm8Regs.CMPB.bit.CMPB = T1PRmulESC_DutyDataC;  //4
               EPwm8Regs.CMPA.bit.CMPA = T1PRmulESC_DutyDataC;  //3,buck管
            }else{
                if((VoltInAF > 0)&&(VoltInAF < 25)){
                    if(GridCurrAF > 0){
                        EPwm7Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //1管
                        EPwm7Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //2管
                        EPwm8Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //3管
                        EPwm8Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //4管
                    }else{
                        EPwm7Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //1管
                        EPwm7Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //2管
                        EPwm8Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //3管
                        EPwm8Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //4管
                    }
                }else if((VoltInAF < 0)&&(VoltInAF > (-25))){
                    if(GridCurrAF > 0){
                        EPwm7Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //1管
                        EPwm7Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //2管
                        EPwm8Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //3管
                        EPwm8Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //4管
                    }else{
                        EPwm7Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //1管
                        EPwm7Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCaddnegCurCompPerc); //2管
                        EPwm8Regs.CMPA.bit.CMPA = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //3管
                        EPwm8Regs.CMPB.bit.CMPB = (T1PRmulESC_DutyDataCsubnegCurCompPerc); //4管
                    }
                }
            }
        }else{
//            ESCFlagC.PWMcurrDirFlag = 0;
            PIVolC.i10 = 0;                 //清除PI的饱和值
            PIVolC.i6 = 0;
            EPwm7Regs.CMPA.bit.CMPA = T1PR;     //1管  //自冷硬件逻辑限制,CMPA,CMPB都给周期值,并不是四个管子全通.
            EPwm7Regs.CMPB.bit.CMPB = T1PR;        //2管
            EPwm8Regs.CMPA.bit.CMPA = T1PR;     //3管
            EPwm8Regs.CMPB.bit.CMPB = T1PR;        //4管
            ESC_FeedForward_DutyC =1;        //运行条件下,未开启补偿,应该1管要给满占空比,初始状态必须设置成100%占空比
        }

#endif

}

/*
 * ADC1-ISR中断服务函数。WY
 */
void ADCD1INT(void) //50us
{
	FaultFastDetectInInt();//20K
	VoltSlidPosCnt();//20K
	GenModulation();//20K
	ADPosCnt();
	GetVolAndInvCurr();//20K

	switch (T1PRPwmFrequency)
	{
		case 0:
			switch (ADBufPos)
			{
				case 0:
				{
					SetHeatPulse();//心跳  10K
					PLLrun();//锁相  10K
					SigPhDQComput();//无功计算 10k
				}
					break;

				case 1:
				{
					Swi_post(RMSstart);
					RmsCalcIn();//10K
					FirstRmsCalc();//5K
				}
					break;

				case 2:
				{
					StateFeedBackJudge();
					SetHeatPulse();//心跳
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
					SetHeatPulse();//心跳
					PLLrun();//锁相
					SigPhDQComput();
				}
					break;

				case 5:
				{
					Swi_post(RMSstart);
					FirstRmsCalc();//
					RmsCalcIn();

				}
					break;

				case 6:
				{
					SetHeatPulse();//心跳
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
			break;
	}

	FaultRecordProg();//20K

	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
}

void RMSswi(void)
{
    FaultDetectInInt();             //0.61us
    FaultRecordSel();
    if(StateFlag.onceTimeAdcAutoAdjust){    //上电后自动运行一次
      AdRegeditOffset();
    }
}

