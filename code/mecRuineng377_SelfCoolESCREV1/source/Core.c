/*
 * Core.c
 *
 *  Created on: 2016-3-3
 *      Author: HR
 */
#include "fdacoefs.h"
#include "F2837xS_device.h"
#include "VariblesRef.h"
#include "math.h"
#include "C28x_FPU_FastRTS.h"
#include "DCL.h"
#include "F2837xS_Examples.h"
#include "MarcoDefine.h"
#include "CoreCtrl.h"
#include "DSP28x_Project.h"
#include "mbport.h"
#include "Modbus.h"
#include "VarMean.h"
#include "DPLL.h"

#pragma CODE_SECTION(clock5ms, "ram2func");
#pragma CODE_SECTION(clock1ms, "ram2func");
//#pragma	CODE_SECTION(ReactivePowerGiven ,"ram2func")
#pragma CODE_SECTION(Funda3sTo2rTransf, "ram2func");
#pragma CODE_SECTION(FirstRmsCalc, "ram2func");
#pragma CODE_SECTION(SlowRmsCalc, "ram2func");
#pragma CODE_SECTION(RmsCalcIn, "ram2func");
//#pragma CODE_SECTION(MeanInit, "ram2func");
#pragma CODE_SECTION(RMSLimit, "ram2func");
#pragma CODE_SECTION(CurrLimitPri, "ram2func");
#pragma CODE_SECTION(CurrLimitZPri, "ram2func");
#pragma CODE_SECTION(GetCapaReactCorr, "ram2func");
#pragma CODE_SECTION(GetHarmLimitCorr, "ram2func");
#pragma CODE_SECTION(GetCapaUnbalCorr, "ram2func");
//#pragma CODE_SECTION(VoltageCompensation, "ram2func");
#pragma CODE_SECTION(RunPIRE, "ram2func");
#pragma CODE_SECTION(Max3, "ram2func");
#pragma CODE_SECTION(SigPhDQComput, "ram2func");


extern Event_Handle Event_SetStartSM;
extern Event_Handle Event_CapSwitchSM;
extern Event_Handle Event_Outside;

float32 CtrlVoltA=0,CtrlVoltB=0,CtrlVoltC=0;
float32 CurFundA=0,CurFundB=0,CurFundC=0,CurFundStandbyA=0,CurFundStandbyB=0,CurFundStandbyC=0;
float32 VolFundA=0,VolFundB=0,VolFundC=0;
//float32 FundwaveA=0,FundwaveB=0,FundwaveC=0;
float32 CurrRefA=0,CurrRefB=0,CurrRefC=0;
float32 CurrRefStandbyA=0,CurrRefStandbyB=0,CurrRefStandbyC=0;
float32 curFundNg;
float currRef_D = 0;
float CtrlVoltUV = 0;

#pragma DATA_SECTION(FlvSlot,"EBSS3");
#pragma DATA_SECTION(RmsSumIn,"EBSS3");
#pragma DATA_SECTION(RmsSumOut,"EBSS3");
//#pragma DATA_SECTION(RmsSumQ,"EBSS3");
float32 FlvSlot[FLV_NUM];

float32 RmsSumIn[ALL_RMSNUM], RmsSumOut[ALL_RMSNUM], RmsSumQ[RMS_CALU_SEGMENT_NUM][ALL_RMSNUM];

float32 UnUsedStoge=0;
//#pragma DATA_SECTION(DQbufIn,"EBSS3");
float DQbufIn[MEANPOINT_QUARTER][MEANPOINT_QUARTER_CHAN];

#define CLARKE2AlphaBeta(A,B,C,Z) 	\
				Z = ((A) + (B) + (C))*S1DIV3;\
				zA = (A)-(Z);\
				zB = (B)-(Z);\
				zC = (C)-(Z);\
				alpha = (SQRT2* SQRT_2DIV3) * (zA - 0.5f*zB - 0.5f*zC);\
			 	beta  = (SQRT2*SQRT2_DIV2) * (zB - zC)



inline float PARK2D(float alpha,float beta,float resSin,float resCos){
 	return (beta *resSin + alpha*resCos)*(SQRT2_DIV2);
}

inline float PARK2Q(float alpha,float beta ,float resSin,float resCos){
 	return (beta *resCos-alpha*resSin)*(SQRT2_DIV2);
}




inline float * PARK2DQ(float *p,float resSin,float resCos,float alpha,float beta)
{
    *p++=(beta *resSin + alpha*resCos)*(SQRT2_DIV2);    //D
    *p++=(alpha*resSin - beta *resCos)*(SQRT2_DIV2);    //Q
    return p;
}

float32 testout[FIFO_DQ_DATA_SIZE];

//6.4k计算频率
void SigPhDQCal(void)
{
    float32 * pIn,*pfifo;
    int i;
    float32 theta,resSin,resCos,DQCalTheta;

    for(i=0;i<FIFO_DQ_BLOCK_NUM;i++){                   //输入数据缓冲无论多少点,仅输出一个点,认为DQ输出是平稳的
        if(!cbIsEmpty(&BlockFifo)){                     //判断fifo非空
            pfifo = testout;                            //得到fifo的输出指针
            pIn = rtb_Vq;                               //得到MEAN的输入指针

            cbRead(&BlockFifo,pfifo);                   //fifo中读

            DQCalTheta  = *pfifo++;                                 //0得到此组数据的相位
            *pIn++      = *pfifo++;                                 //1得到直流电压

            theta=DQCalTheta - (PI*1/180);//VoltCurrCalibrPhase;    //电网侧电压滤波器引起的相位延时校正
            if(theta < 0)   theta += PI2;                           //2*PI limiting
            sincosf(PhaseLimit(theta         ),&resSin,&resCos);
            pIn= PARK2DQ(pIn,resSin,resCos,*pfifo++,*pfifo++);      //2,3
            sincosf(PhaseLimit(theta-(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn,resSin,resCos,*pfifo++,*pfifo++);      //4,5
            sincosf(PhaseLimit(theta+(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn,resSin,resCos,*pfifo++,*pfifo++);      //6,7

            theta=DQCalTheta - LoadCurrCalibrPhase;                 //负载侧侧电流滤波器引起的相位延时校正
            if(theta < 0)   theta += PI2;                           //2*PI limiting
            sincosf(PhaseLimit(theta         ),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //8,9
            sincosf(PhaseLimit(theta-(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //10,11
            sincosf(PhaseLimit(theta+(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //12,13

            theta=DQCalTheta - GridCurrCalibrPhase;                 //电网侧电流滤波器引起的相位延时校正
            if(theta < 0)   theta += PI2;                           //2*PI limiting
            sincosf(PhaseLimit(theta         ),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //14,15
            sincosf(PhaseLimit(theta-(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //16,17
            sincosf(PhaseLimit(theta+(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //18,19


            theta =DQCalTheta - InvCurrCalibrPhase;                 //逆变侧电流滤波器引起的相位延时校正
            if(theta < 0)   theta += PI2;                           //2*PI limiting
            sincosf(PhaseLimit(theta         ),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //20,21
            sincosf(PhaseLimit(theta-(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //22,23
            sincosf(PhaseLimit(theta+(PI*2/3)),&resSin,&resCos);
            pIn= PARK2DQ(pIn, resSin,resCos,*pfifo++,*pfifo++);     //24,25

            float32 zA,zB,zC,alpha,beta;

            //Fundamental zero sequence current needs to be done after the operation of the fundamental wave FFT
            LoadFundaCurZ  =(FundwaveA + FundwaveB + FundwaveC)*S1DIV3; //The Fundamental zero sequence

            //The positive and negative d,q,zero axis of Load current
            theta = DQCalTheta;      //逆变侧电流滤波器引起的相位延时校正
            if(theta < 0)   theta += PI2;                           //2*PI limiting
            sincosf(PhaseLimit(theta         ),&resSin,&resCos);

            CLARKE2AlphaBeta(testout[8],testout[10],testout[12],LoadRealCurZ);
            pIn= PARK2DQ(pIn,  resSin, resCos,alpha,beta);         //26,27,LoadFundaCurD,LoadFundaCurQ
            pIn= PARK2DQ(pIn, -resSin, resCos,alpha,beta);         //28,29,LoadFundaCurND,LoadFundaCurNQ

            //The positive and negative d,q,zero axis of Load current ,for decoupling control
            CLARKE2AlphaBeta(testout[20],testout[22],testout[24],ApfOutCurZ);
            pIn= PARK2DQ(pIn,  resSin, resCos,alpha,beta);         //30,31,InvFundaCurD,InvFundaCurQ
            pIn= PARK2DQ(pIn, -resSin, resCos,alpha,beta);         //32,33,InvFundaCurND,InvFundaCurNQ

            //The positive and negative d,q,zero axis of grid Current
            CLARKE2AlphaBeta(testout[14],testout[16],testout[18],GridZeroCur);
            pIn= PARK2DQ(pIn, resSin,resCos,alpha,beta);          //34,35,GridFundaCurD,GridFundaCurQ

            varMean_step();                     //mean算法.21.5us@24channel
        }else{break;}
    }

    float *pOut=VarMeanOut;             //得到最后一次输出的数据

    dcVoltDiv2Err       =*pOut++;       //1
    GridVoltDA          =*pOut++;       //2
    GridVoltQA          =*pOut++;       //3
    GridVoltDB          =*pOut++;       //4
    GridVoltQB          =*pOut++;       //5
    GridVoltDC          =*pOut++;       //6
    GridVoltQC          =*pOut++;       //7

    LoadRealCurDA       =*pOut++;       //8
    LoadRealCurQA       =*pOut++;
    LoadRealCurDB       =*pOut++;
    LoadRealCurQB       =*pOut++;
    LoadRealCurDC       =*pOut++;
    LoadRealCurQC       =*pOut++;

    GridRealCurDA       =*pOut++;       //14
    GridRealCurQA       =*pOut++;
    GridRealCurDB       =*pOut++;
    GridRealCurQB       =*pOut++;
    GridRealCurDC       =*pOut++;
    GridRealCurQC       =*pOut++;

    IverRealCurDA       =*pOut++;       //20
    IverRealCurQA       =*pOut++;
    IverRealCurDB       =*pOut++;
    IverRealCurQB       =*pOut++;
    IverRealCurDC       =*pOut++;
    IverRealCurQC       =*pOut++;

    LoadFundaCurD       =*pOut++;       //26
    LoadFundaCurQ       =*pOut++;
    LoadFundaCurND      =*pOut++;
    LoadFundaCurNQ      =*pOut++;

    InvFundaCurD        =*pOut++;       //30
    InvFundaCurQ        =*pOut++;
    InvFundaCurND       =*pOut++;
    InvFundaCurNQ       =*pOut++;

    GridFundaCurD       =*pOut++;       //34
    GridFundaCurQ       =*pOut++;       //35
}


float testin[FIFO_DQ_DATA_SIZE];

void SigPhDQinput(void)
{
    float32 * pIn=testin;
    if(++CurrThisPos >= MEANPOINT_QUARTER)      CurrThisPos = 0;
    if(++CurrPrvPos >= MEANPOINT_QUARTER)       CurrPrvPos = 0;
    float *pThisSrc     = DQbufIn[CurrThisPos];
    float *pPrvSrc      = DQbufIn[CurrPrvPos];
    float *pCapAlpha    = 0;//VoltSlid[VoltPos];
    float *pCapBeta     = 0;//VoltSlid[CapVoltPos];

    *pIn++ = SPLL[0].Theta;                    //1,保存当前时刻的锁相值
    *pIn++ = DCtestMirr = dcVoltUpF - dcVoltDnF;         //2

    *pIn++= *pCapAlpha++;                   //3,电网电压A相alpha
    *pIn++= *pCapBeta++;                    //4,电网电压A相beta
    *pIn++= *pCapAlpha++;
    *pIn++= *pCapBeta++;
    *pIn++= *pCapAlpha++;
    *pIn++= *pCapBeta++;

    *pIn++= *pThisSrc++ = LoadRealCurA;     //9
    *pIn++= *pPrvSrc++;                     //10
    *pIn++= *pThisSrc++ = LoadRealCurB;
    *pIn++= *pPrvSrc++;
    *pIn++= *pThisSrc++ = LoadRealCurC;
    *pIn++= *pPrvSrc++;

    *pIn++= *pThisSrc++ = GridCurrA;        //15,放进1/4缓冲区,且给fifo
    *pIn++= *pPrvSrc++;                     //16从1/4缓冲区延时后取出给fifo
    *pIn++= *pThisSrc++ = GridCurrB;
    *pIn++= *pPrvSrc++;
    *pIn++= *pThisSrc++ = GridCurrC;
    *pIn++= *pPrvSrc++;

    *pIn++= *pThisSrc++ = ApfOutCurAD;      //21
    *pIn++= *pPrvSrc++;                     //
    *pIn++= *pThisSrc++ = ApfOutCurBD;
    *pIn++= *pPrvSrc++;
    *pIn++= *pThisSrc++ = ApfOutCurCD;
    *pIn++= *pPrvSrc++;

    cbWrite(&BlockFifo,testin);        //写入fifo
}


void SigPhDQComput(void)
{
    Uint16 temp0 = (Uint16)(Esc_VoltPhaseA*PLL_STEP);           //float convert to int
    temp0 %=  SINE_LOOKTABLE_LEN;
    float resSinA=pSineLookTab(temp0);
    float resCosA=pCoseLookTab(temp0);

    temp0 = (Uint16)(Esc_VoltPhaseB*PLL_STEP);                  //float convert to int
    temp0 %=  SINE_LOOKTABLE_LEN;
    float resSinB=pSineLookTab(temp0);
    float resCosB=pCoseLookTab(temp0);

    temp0 = (Uint16)(Esc_VoltPhaseC*PLL_STEP);                  //float convert to int
    temp0 %=  SINE_LOOKTABLE_LEN;
    float resSinC=pSineLookTab(temp0);
    float resCosC=pCoseLookTab(temp0);

    GridRealCurQA2   =-DCL_runDF22(&SinglePhaseDispFilter[0],    PARK2D(GridCurrAF,GridCurrAF_Beta,   resSinA,resCosA));
    GridRealCurQB2   =-DCL_runDF22(&SinglePhaseDispFilter[1],    PARK2D(GridCurrBF,GridCurrBF_Beta,   resSinB,resCosB));
    GridRealCurQC2   =-DCL_runDF22(&SinglePhaseDispFilter[2],    PARK2D(GridCurrCF,GridCurrCF_Beta,   resSinC,resCosC));

    GridRealCurDA2   = DCL_runDF22(&SinglePhaseDispFilter[3],    PARK2Q(GridCurrAF,GridCurrAF_Beta,   resSinA,resCosA));
    GridRealCurDB2   = DCL_runDF22(&SinglePhaseDispFilter[4],    PARK2Q(GridCurrBF,GridCurrBF_Beta,   resSinB,resCosB));
    GridRealCurDC2   = DCL_runDF22(&SinglePhaseDispFilter[5],    PARK2Q(GridCurrCF,GridCurrCF_Beta,   resSinC,resCosC));

    GridRealCurErrA =DCL_runPI(&GridCurrPICtrlA, GridRealCurQA2 , 0);
    GridRealCurErrB =DCL_runPI(&GridCurrPICtrlB, GridRealCurQB2 , 0);
    GridRealCurErrC =DCL_runPI(&GridCurrPICtrlC, GridRealCurQC2 , 0);
}


#define THDI_RMS_SQRT3CALU(chan)\
    pH->im      = (*p++)*(POWERGRID_FREQ*CTRLSTEP_ALGORITHM);\
    pH->re      = (*p++)*(POWERGRID_FREQ*CTRLSTEP_ALGORITHM);\
    pH++->rms   = *p*(POWERGRID_FREQ*CTRLSTEP_ALGORITHM);\
    (chan)= SQRT3*sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM))

#define THDI_RMS_CALU(chan)\
    pH->im      = (*p++)*(POWERGRID_FREQ*CTRLSTEP_ALGORITHM);\
    pH->re      = (*p++)*(POWERGRID_FREQ*CTRLSTEP_ALGORITHM);\
    pH++->rms   = *p*(POWERGRID_FREQ*CTRLSTEP_ALGORITHM);\
    (chan)= sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM))

#define THDI_RMS_INPUT(chan)\
    *p++ += (chan) * GridResSin;\
    *p++ += (chan) * GridResCos;\
    *p++ += (chan) * (chan)
/*
 * RmsSumIn[x]中存放的是所有要计算通道的RMS_CALU_BLOCK_NUM个点的计算和,这样做滑窗时节省运算量.因为滑窗没必要每个输入点
 * 都需要一个输出点.可比如8点输入后,输出1点.因为后端到要求速率没这么高.每个周波更新16次即可.
  ┌──────────────────────────────────────┬─────────────────────┬───────────────┬───────────────────┐
  │ApfOutCurAD[0]^2                      │ApfOutCurBD[0]^2     │    ...        │LoadRealCurZ[0]^2  │
  │     +[...n+1]                        │                     │               │                   │
  │ApfOutCurAD[RMS_CALU_SEGMENT-1]^2     │                     │               │                   │
  └──────────────────────────────────────┴─────────────────────┴───────────────┴───────────────────┘
*/

void RmsCalcIn(void)          //经过AD采样,采到的数据放在二维数组当中
{
    Uint16 i;
    float *p=RmsSumIn,*pDst;              //段首地址

    *p++ += VoltInAF * VoltInAF;          //1
    *p++ += VoltInBF * VoltInBF;
    *p++ += VoltInCF * VoltInCF;
    *p++ += VoltOutAF * VoltOutAF;        //4
    *p++ += VoltOutBF * VoltOutBF;
    *p++ += VoltOutCF * VoltOutCF;
    *p++ += GridCurrAF * GridCurrAF;      //7
    *p++ += GridCurrBF * GridCurrBF;
    *p++ += GridCurrCF * GridCurrCF;
    *p++ += GridBPCurrAF * GridBPCurrAF;      //10
    *p++ += GridBPCurrBF * GridBPCurrBF;
    *p++ += GridBPCurrCF * GridBPCurrCF;


// 电网电压三个，输出电流三个，负载电流三个，
//    THDI_RMS_INPUT(GridVoltAF);     //x[n]^2+x[n-1]^2+...+x[N-1]^2
//    THDI_RMS_INPUT(GridVoltBF);
//    THDI_RMS_INPUT(GridVoltCF);
//    THDI_RMS_INPUT(LoadVoltUF);
//    THDI_RMS_INPUT(LoadVoltVF);
//    THDI_RMS_INPUT(LoadVoltWF);
//    THDI_RMS_INPUT(GridCurrAF);
//    THDI_RMS_INPUT(GridCurrBF);
//    THDI_RMS_INPUT(GridCurrCF);

    *p++ += LoadFundaCurZ*LoadFundaCurZ;        //35    //这个值由DQ模块算出
    *p++ += CurFundNgA*CurFundNgA;              //36
    *p++ += CurFundNgB*CurFundNgB;
    *p++ += CurFundNgC*CurFundNgC;
    float temp=CurrHarmRefA +CurrHarmRefB +CurrHarmRefC;
    *p++ += temp*temp;
    *p++ += ApfOutCurZ*ApfOutCurZ;
    *p++ += GridZeroCur*GridZeroCur;            //41
    *p++ += LoadRealCurZ*LoadRealCurZ;          //42

    //段计数,段清零
    if( (++RmsCaluBlockCnt >= RMS_CALU_BLOCK_NUM) || (StateFlag.ForceRMSRefresh) )  //jcl:ForceRMSRefresh逻辑得改
    {
        RmsCaluBlockCnt=0;                      //每段的点数计数,一段满了
        p=RmsSumIn;                             //指向每段的点数之和的首地址
        pDst=RmsSumQ[RmsCaluCnt];               //放入每一段的总数
        for(i=0;i<ALL_RMSNUM;i++){
            *pDst++ = *p;                       //把RmsSum中一个周期中RMS_CALU_SEGMENT_SIZE_NUM个点数的和,保存在RmsSumQ中
            *p++=0;                             //RmsSum清零,这样RmsSumIn可以做下一段之和.
        }
        if(++RmsCaluCnt>=RMS_CALU_SEGMENT_NUM) RmsCaluCnt=0;     //循环缓冲区计数清零
    }
}

/*关于不用滑窗迭代算法计算有效值的原因是如果因为某种原因导致任何误差,都会在迭代缓冲区中一直存在而不会消除.不利于装置的健壮性,故不采用.
  下表缓为RmsSumQ[行][列]的解释
  RmsSumQ[x][0:RMS_CALU_SEGMENT]每列存一路模拟通道的数据,来自于RmsSumIn[x].
  此缓冲区为循环缓冲区.为保证数据不被异步线程的写入导致数据被刷新掉,故缓冲区加了3个.
  有效值RMS=(RmsSumQ[x][0:RMS_CALU_SEGMENT-1])的和乘以(POWERGRID_FREQ*CTRLSTEP_ALGORITHM)
  ┌────────────────────────────────┬─────────────────────────────────┬──────┬────────────────────────────────────────────────┐
  │RmsSumIn[0]t(0)                 │RmsSumIn[1]t(0)                  │ ...  │RmsSumIn[ALL_RMSNUM-1]t(0)                      │
  │                                │                                 │      │                                                │
  ├────────────────────────────────┼─────────────────────────────────┼──────┼────────────────────────────────────────────────┤
  │RmsSumIn[0]t(1)                 │RmsSumIn[1]t(1)                  │ ...  │RmsSumIn[ALL_RMSNUM-1]t(1)                      │
  │                                │                                 │      │                                                │
  ├────────────────────────────────┼─────────────────────────────────┼──────┼────────────────────────────────────────────────┤
  │    ...                         │                                 │      │                                                │
  │                                │                                 │      │                                                │
  ├────────────────────────────────┼─────────────────────────────────┼──────┼────────────────────────────────────────────────┤
  │RmsSumIn[0]t(RMS_CALU_SEGMENT)  │RmsSumIn[1]t(RMS_CALU_SEGMENT)   │ ...  │RmsSumIn[ALL_RMSNUM-1]t(RMS_CALU_SEGMENT)       │
  │                                │                                 │      │                                                │
  └────────────────────────────────┴─────────────────────────────────┴──────┴────────────────────────────────────────────────┘
 */

void SlowRmsCalc(void)         //慢速算有效值,是将整个周期分为四段,按照1/4进行取值,必须将整个周期的值都取到,才能进行有效值计算.
{
    STRU_HarmTHD *pH=(STRU_HarmTHD*) HarmTHD;    //在采集一整个周波(CTRLFREQUENCY/POWERGRID_FREQ)个点时,指向HarmTHD首地址,用于calcuFlag的下一轮计算
    float *pSrc,*p;
    Uint16 i,j;
    int16 addr;

    if((RmsCaluCnt==0) || (StateFlag.ForceRMSRefresh))          //每隔20ms刷新一次
    {
        p = &RmsSumOut[FIRST_RMSNUM];                           //获得RMS输出数组地址
        for(i=ALL_RMSNUM-FIRST_RMSNUM;i;i--)                    //从FIRST_RMSNUM~ALL_RMSNUM路输出
            *p++=0;                                             //RMS输出数组清零

        addr=RmsCaluCnt;
        for(i=0;i<(RMS_CALU_SEGMENT);i++){                      //每通道有RMS_CALU_SEGMENT个数据参与RMS算法的计算
            if(--addr<0) addr = RMS_CALU_SEGMENT_NUM-1;         //获取RMS滑动缓冲区的时间最久的地址
            p = &RmsSumOut[FIRST_RMSNUM];                       //获得RMS输出数组地址
            pSrc = &RmsSumQ[addr][FIRST_RMSNUM];
            for(j=ALL_RMSNUM-FIRST_RMSNUM;j;j--)
            {
                *p++ += *pSrc++ ;   //遍历整个环形缓冲区取和
            }
         //   *p++;                                             //指向下一个RMS输出数组
        }

        p=&RmsSumOut[FIRST_RMSNUM];                             //重获得RMS输出数组地址


        THDI_RMS_CALU(VoltInA_rms);//电网电压          谐波有效值/总的有效值=THDI;基波有效值/总的有效值=THDF;
        THDI_RMS_CALU(VoltInB_rms);
        THDI_RMS_CALU(VoltInC_rms);

        THDI_RMS_CALU(VoltOutA_rms);//负载电压
        THDI_RMS_CALU(VoltOutB_rms);
        THDI_RMS_CALU(VoltOutC_rms);

        THDI_RMS_CALU(gridCurA_rms);//电网电流
        THDI_RMS_CALU(gridCurB_rms);
        THDI_RMS_CALU(gridCurC_rms);


        StateFlag.cntForTHDi = 1;               //启动THDi的计算
        GpVoltA_rmsReciprocal=ki_Cur/(VoltInA_rms*SQRT_2DIV3);
        GpVoltB_rmsReciprocal=ki_Cur/(VoltInB_rms*SQRT_2DIV3);
        GpVoltC_rmsReciprocal=ki_Cur/(VoltInC_rms*SQRT_2DIV3);
        LoadFundaCurZRms    = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));
        CurFundNgARms       = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));
        CurFundNgBRms       = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));
        CurFundNgCRms       = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));
        CurrHarmRefZ        = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));

        ApfOutCurZRms       = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));
        GridNeutralCurRms   = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));
        LoadRealCurZRms     = sqrtf(*p++ *(POWERGRID_FREQ*CTRLSTEP_ALGORITHM));

        GridVoltRms=(VoltInA_rms+VoltInB_rms+VoltInC_rms)*S1DIV3;
        ThreeHarmGridVoltRms=GridVoltRms*(S1DIVSQRT3*SQRT2*S1DIV6);
        if(GridVoltRms>3000)            GridVoltRms=3000;
        FLVFUN(GridVoltRms_F,GridVoltRms,VOLT);

        GridCurRms = (gridCurA_rms + gridCurB_rms + gridCurC_rms)*S1DIV3;
        if(GridCurRms>10000)            GridCurRms=10000;
        FLVFUN(GridCurRms_F,GridCurRms,GRIDCURR);

        LoadVoltRms = (VoltOutA_rms + VoltOutB_rms + VoltOutC_rms)*S1DIV3;
        if(LoadVoltRms>10000)            LoadVoltRms=10000;
        FLVFUN(LoadVoltRms_F,LoadVoltRms,LOADCURR);
    }
}

/*
 * 功能：计算电压和电流有效值。WY
 */
void FirstRmsCalc(void)           //快速有效值算法,取到整个周期的1/4之后,就直接进行有效值计算.
{
	Uint16 i, j;
	int16 addr;
	float *p = &RmsSumOut[0], *pSrc; //获得RMS输出数组的首地址

	for(i = FIRST_RMSNUM; i; i --)
		*p ++ = 0; //RMS输出数组清零

	addr = RmsCaluCnt;

	for(i = RMS_CALU_SEGMENT; i; i --)
	{ //共计RMS_CALU_SEGMENT个数据计算
		if(-- addr < 0)
			addr = RMS_CALU_SEGMENT_NUM - 1; //获取RMS滑动缓冲区的时间最久的地址
		p = &RmsSumOut[0]; //获得RMS输出数组的首地址
		pSrc = &RmsSumQ[addr][0];
		for(j = FIRST_RMSNUM; j; j --) //从FIRST_RMSNUM~ALL_RMSNUM共计有ALL_RMSNUM路输出
		{
			*p ++ += *pSrc ++; //遍历整个环形缓冲区取和
		}
	}

	p = &RmsSumOut[0]; //重获得RMS输出数组的首地址

	ESCFlagA.VoltIn_rms = VoltInA_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
	ESCFlagB.VoltIn_rms = VoltInB_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
	ESCFlagC.VoltIn_rms = VoltInC_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));

	ESCFlagA.VoltOut_rms = VoltOutA_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
	ESCFlagB.VoltOut_rms = VoltOutB_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
	ESCFlagC.VoltOut_rms = VoltOutC_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));

	if(StateFlag.VoltageModeFlag == 0)
	{
		UnCurrData[0] = ESCFlagA.gridCur_rms = CURRData[0] = gridCurA_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		UnCurrData[1] = ESCFlagB.gridCur_rms = CURRData[1] = gridCurB_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		UnCurrData[2] = ESCFlagC.gridCur_rms = CURRData[2] = gridCurC_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));

		CURRData[3] = ESCFlagA.gridCurrBY_rms = gridCurrBYAF_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		CURRData[4] = ESCFlagB.gridCurrBY_rms = gridCurrBYBF_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		CURRData[5] = ESCFlagC.gridCurrBY_rms = gridCurrBYCF_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
	}
	else if(StateFlag.VoltageModeFlag == 1)
	{
		ESCFlagA.gridCur_rms = CURRData[0] = gridCurA_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		ESCFlagB.gridCur_rms = CURRData[1] = gridCurB_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		ESCFlagC.gridCur_rms = CURRData[2] = gridCurC_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));

		UnCurrData[0] = CURRData[3] = ESCFlagA.gridCurrBY_rms = gridCurrBYAF_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		UnCurrData[1] = CURRData[4] = ESCFlagB.gridCurrBY_rms = gridCurrBYBF_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
		UnCurrData[2] = CURRData[5] = ESCFlagC.gridCurrBY_rms = gridCurrBYCF_rms = sqrtf(*p ++ * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));
	}

	ResonProtcABCRms = sqrtf(*p * (POWERGRID_FREQ * CTRLSTEP_ALGORITHM));

#if ESC_THREEPHASECTRL
	GridVoltRms = (VoltInA_rms + VoltInB_rms + VoltInC_rms) * S1DIV3;
	ThreeHarmGridVoltRms = GridVoltRms * (S1DIVSQRT3 * SQRT2 * S1DIV6);
	if(GridVoltRms > 3000)
		GridVoltRms = 3000;
	FLVFUN(GridVoltRms_F, GridVoltRms, VOLT);

	LoadVoltRms = (VoltOutA_rms + VoltOutB_rms + VoltOutC_rms) * S1DIV3;
	if(LoadVoltRms > 10000)
		LoadVoltRms = 10000;
	FLVFUN(LoadVoltRms_F, LoadVoltRms, LOADCURR);

	GridCurRms = (gridCurA_rms + gridCurB_rms + gridCurC_rms) * S1DIV3;
	if(GridCurRms > 10000)
		GridCurRms = 10000;
	FLVFUN(GridCurRms_F, GridCurRms, GRIDCURR);

	GridCurBYRms = (gridCurrBYAF_rms + gridCurrBYBF_rms + gridCurrBYCF_rms) * S1DIV3;
	if(GridCurBYRms > 10000)
		GridCurBYRms = 10000;
	FLVFUN(GridCurBYRms_F, GridCurBYRms, GRIDCURR);
#elif ESC_SINGLEPHASECTRL
    GridVoltRms = VoltInA_rms;
    if(GridVoltRms>3000)            GridVoltRms=3000;
    FLVFUN(GridVoltRms_F,GridVoltRms,VOLT);

    LoadVoltRms = VoltOutA_rms;
    if(LoadVoltRms>10000)           LoadVoltRms=10000;
    FLVFUN(LoadVoltRms_F,LoadVoltRms,LOADCURR);

    GridCurRms = gridCurA_rms;
    if(GridCurRms>10000)            GridCurRms=10000;
    FLVFUN(GridCurRms_F,GridCurRms,GRIDCURR);

    GridCurBYRms = gridCurrBYAF_rms;
    if(GridCurBYRms>10000)          GridCurBYRms=10000;
    FLVFUN(GridCurBYRms_F,GridCurBYRms,GRIDCURR);
#endif
}
void GetCapaReactCorr(void)
{
	if(StateFlag.reactPrCompFlag){
		CapaReactCorr	=CurrLimitPri(reactPowerCompCurQ*S1DIVSQRT3);		//得到正序Q轴的限幅系数(无功输出控制轴)
	}else{
		CapaReactCorr=0;													//关闭无功补偿,则无功系数为零
	}
}

void GetPhCaReactCorr(void)
{
	WholeRmsLimitPH = WholeRmsLimit;
	if(StateFlag.reactPrCompFlag){
		CapaReactCorrA = CurrLimitPriPH(ReactPowerCurQPA);
		CapaReactCorrB = CurrLimitPriPH(ReactPowerCurQPB);
		CapaReactCorrC = CurrLimitPriPH(ReactPowerCurQPC);
	}else{
		CapaReactCorrA=CapaReactCorrB=CapaReactCorrC=0;
	}
}

void GetHarmLimitCorr(void)
{
//	if(StateFlag.harmCompEn){												//谐波补偿开启
//		harmSum	=Max3(HarmSumA,HarmSumB,HarmSumC)*(1/SQRT2);				//取三相谐波中的最大值
//		float32 harmLimtCorr	=CurrLimitPri(harmSum);								//谐波相线电流限幅系数
//		float32 harmZLimtCorr	=CurrLimitZPri(CurrHarmRefZ);						//谐波中线电流限幅系数
//		if(StateFlag.isHarmCompensateMode)									//是补谐波模式
//			HarmLimtCorr=harmLimtCorr*harmZLimtCorr*harmCompPerc;			//谐波系数=相线系数*中线系数*界面谐波系数
//		else
//			HarmLimtCorr=harmZLimtCorr*harmCompPerc;						//谐波系数=相线系数*中线系数
//	}else{
//		HarmLimtCorr=0;														//关闭谐波补偿,则谐波系数清零
//	}
}

void GetCapaUnbalCorr(void)
{
	if(StateFlag.negCurCompFlag){											//不平衡补偿开启
		curFundNg=Max3(CurFundNgARms,CurFundNgBRms,CurFundNgCRms);			//取三相中最大值
		float32 capaUnbalCorr	=CurrLimitPri(curFundNg);							//不平衡基波相线电流限幅系数
		float32 capaUnbalZCorr	=CurrLimitZPri(LoadFundaCurZRms);					//不平衡基波中线电流限幅系数
		CapaUnbalCorr =capaUnbalCorr*capaUnbalZCorr;										//不平衡系数=相线电流限幅系数*中线电流限幅系数
	}else{
		CapaUnbalCorr=0;
	}
}

void RMSLimit(void)
{
	static int16 count=0;

	WholeRmsLimit	=WholeOutCurRmsLimit;							//Saving computation, this variable is defined as RMS*RMS
//	WholeZRmsLimit	=NeutralCurLimit;

	//在此计算的值均以装置要输出的有效值电流为基准.让下一级模块乘以缩放系数实现电流自动限幅
	if( ((StateEventFlag_A == STATE_EVENT_RUN_A)||(StateEventFlag_A == STATE_EVENT_WAIT_A))|| \
	    ((StateEventFlag_B == STATE_EVENT_RUN_B)||(StateEventFlag_B == STATE_EVENT_WAIT_B))|| \
	    ((StateEventFlag_C == STATE_EVENT_RUN_C)||(StateEventFlag_C == STATE_EVENT_WAIT_C)) ){
		switch(StateFlag.prioritizedModeFlag){
		case 4:	//快速补偿(暂未实现)
		case 5:	//自动补偿(暂未实现)
		case 0:	//默认模式(与前版本兼容)
		case 1:   // 无功优先
			CurrRefDCorr	=CurrLimitPri(currentRefD*S1DIVSQRT3);					//得到正序D轴的限幅系数(直流电压控制轴)
			GetPhCaReactCorr();
			GetCapaReactCorr();
			GetHarmLimitCorr();
			GetCapaUnbalCorr();
			break;

		case 2:  // 谐波优先
			CurrRefDCorr	=CurrLimitPri(currentRefD*S1DIVSQRT3);
			GetHarmLimitCorr();
			GetCapaUnbalCorr();
			GetCapaReactCorr();
			break;

		case 3: // 不平衡优先
			CurrRefDCorr	=CurrLimitPri(currentRefD*S1DIVSQRT3);
			GetCapaUnbalCorr();
			GetHarmLimitCorr();
			GetCapaReactCorr();
			break;
		default:break;
		}
	}else{
		CapaUnbalCorr=1;
		HarmLimtCorr=1;
		CapaReactCorr=1;
	}

	if(CapaUnbalCorr>2) CapaUnbalCorr=2.0f;	if(CapaUnbalCorr<-2) CapaUnbalCorr=-2.0f;
	if(HarmLimtCorr>2)  HarmLimtCorr =2.0f;	if(HarmLimtCorr <-2) HarmLimtCorr =-2.0f;
	if(CapaReactCorr>2) CapaReactCorr=2.0f;	if(CapaReactCorr<-2) CapaReactCorr=-2.0f;
	if(count++>1600){		//0.5s刷新一次
		count=0;
		storageUsed=100*(sqrtf(WholeOutCurRmsLimit)-sqrtf(WholeRmsLimit))/sqrtf(WholeOutCurRmsLimit);
	}
}

void ESCfilterMemCopy(Uint16 num)
{
    int16 i;
    if(num > 2)    num = 0;
    for(i=0;i<3;i++){
        ConstantCurr[i].VolttargetCorr = ConstantCurrDefault[num].VolttargetCorr;
        ConstantCurr[i].RMS_CONSTANT_CURRENT_OVERLOAD = ConstantCurrDefault[num].RMS_CONSTANT_CURRENT_OVERLOAD;
        ConstantCurr[i].RMS_CONSTANT_CURRENT_DIFF = ConstantCurrDefault[num].RMS_CONSTANT_CURRENT_DIFF;
        ConstantCurr[i].RMS_CONSTANT_CURRENT_RATED = ConstantCurrDefault[num].RMS_CONSTANT_CURRENT_RATED;
        ConstantCurr[i].INS_CONSTANT_CURRENT_RATED = ConstantCurrDefault[num].INS_CONSTANT_CURRENT_RATED;

    }
}


float RunPIRE(PIcalc *p, float rk, float yk)
{
	float err = rk - yk;
	p->i6 += (err - p->i10) * p->Kp + err * p->Ki;
	p->i10 = err;
	if(p->i6>p->Umax)
		p->i6 = p->Umax;
	if(p->i6<p->Umin)
		p->i6 = p->Umin;
	return p->i6;
}


/*void VoltageCompensation(void)    PI运行
{
    if(StateEventFlag == STATE_EVENT_RUN)
    {
        if(UserSetting.WordMode.B.VoltageMode!=0)
        {
            if(StateEventFlag != STATE_EVENT_RUN)           CntSec.VolComDelay = 0;
            if(CntSec.VolComDelay>3)						StateFlag.StabilizeFlag = 1;		//稳定时间														//启动稳定标志
            else											StateFlag.StabilizeFlag = 0;

    //		if(LoadVoltU_rms<VolUpCo && LoadVoltU_rms>VolDownCo)
    //		if(GridVoltRms<VolUpCo&&GridVoltRms>VolDownCo)	CntMs.VolSurDelay = 0;

            if((CntMs.VolSurDelay>VolSurTime)||(UserSetting.WordMode.B.VoltageMode==2))
                StateFlag.VolSurTimeFlag = 1;		//恒电压补偿

            if(StateFlag.StabilizeFlag && StateFlag.VolSurTimeFlag){

//                if(LoadVoltU_rms     > VolUpSurCo)				  VolCorU = VolUpSurCo;	//选择电压目标值
//                else if(LoadVoltU_rms< VolDownSurCo)			      VolCorU = VolDownSurCo;
//                else				                                  VolCorU = LoadVoltU_rms;
//
//                if(LoadVoltV_rms     > VolUpSurCo)                  VolCorV = VolUpSurCo;   //选择电压目标值
//                else if(LoadVoltV_rms< VolDownSurCo)                VolCorV = VolDownSurCo;
//                else                                                VolCorV = LoadVoltV_rms;
//
//                if(LoadVoltW_rms     > VolUpSurCo)                  VolCorW = VolUpSurCo;   //选择电压目标值
//                else if(LoadVoltW_rms< VolDownSurCo)                VolCorW = VolDownSurCo;
//                else                                                VolCorW = LoadVoltW_rms;
//
//                if((LoadVoltU_rms>(VolCorU-1)) && (LoadVoltU_rms<(VolCorU+1)))  CntMs.VolSurCountU = 0;
//                if((LoadVoltV_rms>(VolCorV-1)) && (LoadVoltV_rms<(VolCorV+1)))  CntMs.VolSurCountV = 0;
//                if((LoadVoltW_rms>(VolCorW-1)) && (LoadVoltW_rms<(VolCorW+1)))  CntMs.VolSurCountW = 0;

                    VolPIOutPwmVa = DCL_runPI(&PIVolA,TargetValue,VoltOutA_rms);
                    VolPIOutPwmVb = DCL_runPI(&PIVolB,TargetValue,VoltOutB_rms);
                    VolPIOutPwmVc = DCL_runPI(&PIVolC,TargetValue,VoltOutC_rms);
            }
        }else{
            StateFlag.VolSurTimeFlag = 0;
            VolPIOutPwmVa =VolPIOutPwmVb =VolPIOutPwmVc = 1;//运行条件下,未开启补偿,应该1管要给满占空比
        }
    }
}*/

// 无功指令下发计算方法
// 功能：根据上位机下发恒无功值或者恒功率因数值，计算应该输出的无功Q值。
// 1)  P = 1/sqrt(3)*(Vd*Id + Vq*Iq + 2*V0*I0)
// 2)  Q = 1/sqrt(3)*(Vq*Id - Vd*Iq)
// 另外，关键的问题在于，计算得到的无功分量加在控制环上的容性和感性 未知。这就需要功率因数角的帮助了。
// 电流滞后电压，则得到电流的Q轴为负值，则计算得到的无功功率为负值，这样sinFai也是负的。
// 无功功率给定为正值，感性。负值，容性。恒无功  constantQFlag = 1；reactPowerGiven = （kvar）
//                                 恒功率因数 constantQFlag = 0；constantCosFai = (-1~+1)
// 成世杰程序计算方法 0922
// 计算滤波后的DQ轴的模值，电网电压有效值，负载电流有效值，输出电流有效值
// 计算无功功率和功率因数的方法：采用滤波后的数值来算有功P和无功Q，每次计算后都滤波6个周波
// 再计算视在功率  S^2 = P^2 + Q^2
// cos = P*1000/Q
// 如果Q为负值，则功率因数为正。
void ReactivePowerGiven(void)
{
	// 所有互感器接线方式为从电网进入设备视为正向，那么电流变换后感性为正，
	// 防止过补行为。这样最终负载和补偿电流和总为负几。
	// 即：-10 < ref + iqFilter_F < 0
	// ********************* 无功下发 **********************
	// 恒无功模式下，计算Q轴给定 Iq = (Vq*Id - 1.731789*Q)/Vd
	// Q = SQRT3* U*I  -> I = Q/SQRT3 /U
	// U线电压有效值    I为相电流有效值
	// 这里Vd和Vq均为相电压的幅值*1.22，所以需要变换为线电压幅值*1.22
	// 电压乘以系数1*SQRT3*SQRT3/SQRT2 = 2.2132
	// 电流乘以系数1*SQRT2*SQRT3/SQRT2 = 1.732
	if(StateFlag.reactPrCompFlag&&(StateFlag.VolSurTimeFlag==0))
	{
		switch(StateFlag.constantQFlag){
		case 0:	//恒无功功率
			if(reactPowerGiven > reactPowGivenLimit*SQRT3)  reactPowerGiven = reactPowGivenLimit*SQRT3;   //  限制在100Kvar， 上位机下发单位应该为Kvar
			if(reactPowerGiven < -reactPowGivenLimit*SQRT3) reactPowerGiven = -reactPowGivenLimit*SQRT3;
			// Vq*Id - Q = Vd*Iq
			//reactPowerCompCurQ =(GridFundaVoltQ*LoadFundaCurD* -reactPowerGiven *1000.0f*MU_MultRatio)/GridFundaVoltD;   // Q/SQRT3单位折算，这里是Var;
			reactPowerCompCurQ =(reactPowerGiven *MU_MultRatio*1000.0f)/GridFundaVoltD;   // Q/SQRT3单位折算，这里是Var;
			ReactPowerCurQPA = reactPowerCompCurQ*S1DIVSQRT3;
			ReactPowerCurQPB = ReactPowerCurQPC = ReactPowerCurQPA;
			break;
		case 1:	//恒无功电流
				ReactPowerCurQPA = restantReactCurrent*reactPrCompPerc*(MU_MultRatio);   //正是容性，负是感性
				ReactPowerCurQPB = ReactPowerCurQPC = ReactPowerCurQPA;
				reactPowerCompCurQ = ReactPowerCurQPA*SQRT3;   						 //正是容性，负是感性
				if(StateFlag.ManualUbanCurFlag==1){   //手动模式发不平衡需勾选无功补偿和无功电流模式
					ReactPowerCurQPA = manualubanlanccurA;
					ReactPowerCurQPB = manualubanlanccurB;
					ReactPowerCurQPC = manualubanlanccurC;
				}
			break;
		case 2:	//恒功率因数
			// 恒功率因数模式下，计算无功给定Q
			// 计算方法：根据负载有功分量和总视在功率，计算无功
			// S = P/cos(fai)   Q = sqrt(S^2 - P^2)
			//	if(constantCosFai >=1)		constantCosFai = 1;
			//	if(constantCosFai <= -1)	constantCosFai = -1;
	//		if(LoadFundaCurQ>2)		//cos(pha)=0.99; sin(pha)*S(30A)=0.141;S*0.141=4.23; 4.23*SQRT2*2=12.0A
	//			if(GeneratorStationFlag==1)	//发电站模式
	//				reactPowerCompCurQ = LoadFundaCurQ*constantCosFai*(MU_MultRatio);   		// 检测到感性无功,发电站模式是反的.改为发容性
	//			else						//负载模式
	//				reactPowerCompCurQ = -LoadFundaCurQ*constantCosFai*(MU_MultRatio);   		// 感性无功
	//
	//		else if(LoadFundaCurQ<-2)	//滞回特性
	//			if(GeneratorStationFlag==1)	//发电站模式
	//				reactPowerCompCurQ = LoadFundaCurQ*(2-constantCosFai)*(MU_MultRatio);		// 容性无功
	//			else
	//				reactPowerCompCurQ = -LoadFundaCurQ*(2-constantCosFai)*(MU_MultRatio);		// 容性无功
	//		else ;	//维持不变.防止在功率因数1附近切换
	//		reactPowerCompCurQ = -LoadFundaCurQ*abs(constantCosFai)*(MU_MultRatio);   		//
				if(LoadFundaCurQ>2){
					reactPowerCompCurQ = -LoadFundaCurQ*constantCosFai*reactPrCompPerc*(MU_MultRatio);   		// 感性无功
					ReactPowerCurQPA=-(LoadRealCurQA-LoadRealCurDA*PFConsin)*reactPrCompPerc*MU_MultRatio;
					ReactPowerCurQPB=-(LoadRealCurQB-LoadRealCurDB*PFConsin)*reactPrCompPerc*MU_MultRatio;
					ReactPowerCurQPC=-(LoadRealCurQC-LoadRealCurDC*PFConsin)*reactPrCompPerc*MU_MultRatio;
					CapreactPowerCurQ = Max3(ReactPowerCurQPA,ReactPowerCurQPB,ReactPowerCurQPC)*SQRT3;
					if(ReactPowerCurQPA>0)	ReactPowerCurQPA = 0;
					if(ReactPowerCurQPB>0)	ReactPowerCurQPB = 0;
					if(ReactPowerCurQPC>0)	ReactPowerCurQPC = 0;
				}else if(LoadFundaCurQ<-2){
	//				PFConsin = -PFConsin;
					reactPowerCompCurQ = -LoadFundaCurQ*reactPrCompPerc*(MU_MultRatio);	// 容性无功
					ReactPowerCurQPA = -LoadRealCurQA*reactPrCompPerc*(MU_MultRatio);
					ReactPowerCurQPB = -LoadRealCurQB*reactPrCompPerc*(MU_MultRatio);
					ReactPowerCurQPC = -LoadRealCurQC*reactPrCompPerc*(MU_MultRatio);
					CapreactPowerCurQ = reactPowerCompCurQ;
				}else{
					ReactPowerCurQPA = ReactPowerCurQPB = ReactPowerCurQPC = 0;
					CapreactPowerCurQ = reactPowerCompCurQ = 0;
				}
				break;
			default:break;
		}
	}else{
		ReactPowerCurQPA = ReactPowerCurQPB = ReactPowerCurQPC = 0;
		CapreactPowerCurQ = reactPowerCompCurQ = 0;
	}
}

float CurrLimitPri(float in){
	float capTmp=(in)*(in);
	float corr=1.0f;
	if( (WholeRmsLimit)>=(capTmp) ){
		WholeRmsLimit -= capTmp;
	}else{
		(corr)=sqrtf(WholeRmsLimit/capTmp);
		(WholeRmsLimit)=0;
	}
	return corr;
}
float CurrLimitPriPH(float in){
	float capTmp=(in)*(in);
	float corr=1.0f;
	if( (WholeRmsLimitPH)>=(capTmp) ){
		;
	}else{
		(corr)=sqrtf(WholeRmsLimitPH/capTmp);
	}
	return corr;
}

float CurrLimitZPri(float in){
	float capTmp=(in)*(in);
	float corr=1.0f;
	if( (WholeZRmsLimit)>=(capTmp) ){
		(WholeZRmsLimit)-= (capTmp);
	}else{
		(corr)=sqrtf(WholeZRmsLimit/capTmp);
		(WholeZRmsLimit)=0;
	}
	return corr;
}

void RMSAllInit(void)
{
    Uint16 *pInitVar,i;

    pInitVar = (Uint16 *)(&RmsSumIn);
    for(i=0;i<sizeof(RmsSumIn);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&RmsSumOut);
     for(i=0;i<sizeof(RmsSumOut);i++)
         *pInitVar++ = 0;

     pInitVar = (Uint16 *)(&RmsSumQ);
      for(i=0;i<sizeof(RmsSumQ);i++)
          *pInitVar++ = 0;
}


// 初始化参数使用，避免计算的时候调用无穷大
// 所有需要累加的数据，而非被替换掉的数据，比如：
// BW滤波器，滑动滤波器，均值滤波器，一阶低通滤波器
void InitParameters(void)
{
	int16 i,j;

	PWM_PERIOD_MAX = T1PR+5;
	PWM_PERIOD_MIN = T1PR-5;
	for(i=0;i<3*2;i++)
		for(j=0;j<CALIBRATION_COEFF_LEN+1;j++){
            IFFTData[i][j]=0;
	        InvFFTData[i][j]=0;
		}
	for(i=0;i<GENERATOR_COEFF_LEN;i++){
		LimitVal[0][i]=1.0f;
		LimitVal[1][i]=1.0f;
		LimitVal[2][i]=1.0f;
		LimitDly[0][i]=0;
		LimitDly[1][i]=0;
		LimitDly[2][i]=0;
	}

	CtrlVoltA=CtrlVoltB=CtrlVoltC=0;
	CurFundA=CurFundB=CurFundC=0;
	CurrHarmRefA=CurrHarmRefB=CurrHarmRefC=0;
	FundwaveA=FundwaveB=FundwaveC=0;
	CurrRefA=CurrRefB=CurrRefC=0;
	CurFundNgA=CurFundNgB=CurFundNgC=0;
	initStartVar();
}

void MCUfilterMemCopy(DF22 *dst,int16 num)
{
    int16 i;

    Uint16 *pInitVar    =(Uint16 *)dst;
    Uint16 *pSrcVar     =(Uint16 *)(&AllFilterCoeff[T1PRPwmFrequency][num]);
     for(i=0;i<sizeof(UdcUpFilter);i++)
         *pInitVar++=*pSrcVar++;
}

#define FFTMAG_SEGMENT1 5
#define FFTMAG_SEGMENT2 10
#define FFTMAG_SEGMENT3 (CALIBRATION_COEFF_LEN - FFTMAG_SEGMENT1 - FFTMAG_SEGMENT2)

void InitMCUFilter(void) //选定开关频率
{
    int i;
    MCUfilterMemCopy(&VolAndInvCurrFilter[0],           GRIDVOLT_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[1],           GRIDVOLT_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[2],           GRIDVOLT_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[3],           GRIDVOLT_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[4],           GRIDVOLT_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[5],           GRIDVOLT_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[6],           APFOUT1_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[7],           APFOUT1_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[8],           APFOUT1_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[9],           APFOUT1_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[10],          APFOUT1_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[11],          APFOUT1_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[12],          UDC_PNF_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[13],          UDC_PNF_DEFAULT_NUM);
    MCUfilterMemCopy(&VolAndInvCurrFilter[14],          UDC_PNF_DEFAULT_NUM);
    MCUfilterMemCopy(&ResonancePortectABC[0],           RESONANCE_PORTECT_ABC_NUM);
    MCUfilterMemCopy(&ResonancePortectABC[1],           RESONANCE_PORTECT_ABC_NUM);
    MCUfilterMemCopy(&UdcUpFilter,                      UDC_PNF_DEFAULT_NUM);
    MCUfilterMemCopy(&UdcDnFilter,                      UDC_PNF_DEFAULT_NUM);
    MCUfilterMemCopy(&bwForFreq,                        BWFILTER_FREQ_DEFAULTS_NUM);

    for(i=0;i<FFTMAG_SEGMENT1;i++)
    {
        MCUfilterMemCopy(&fftmag[0][i],              FFT_MAG1_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftmag[1][i],              FFT_MAG1_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftmag[2][i],              FFT_MAG1_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[0][i],              FFT_PHA1_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[1][i],              FFT_PHA1_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[2][i],              FFT_PHA1_ALL_DEFAULT_NUM);
    }

    for(i=FFTMAG_SEGMENT1;i<FFTMAG_SEGMENT2;i++)
    {
        MCUfilterMemCopy(&fftmag[0][i],              FFT_MAG2_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftmag[1][i],              FFT_MAG2_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftmag[2][i],              FFT_MAG2_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[0][i],              FFT_PHA2_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[1][i],              FFT_PHA2_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[2][i],              FFT_PHA2_ALL_DEFAULT_NUM);
    }

    for(i=FFTMAG_SEGMENT2;i<FFTMAG_SEGMENT3;i++)
    {
        MCUfilterMemCopy(&fftmag[0][i],              FFT_MAG3_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftmag[1][i],              FFT_MAG3_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftmag[2][i],              FFT_MAG3_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[0][i],              FFT_PHA3_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[1][i],              FFT_PHA3_ALL_DEFAULT_NUM);
        MCUfilterMemCopy(&fftpha[2][i],              FFT_PHA3_ALL_DEFAULT_NUM);
    }
    //    MCUfilterMemCopy(&gridVoltFilterLowA,           GRID_VOLT_LOWDEFAULT_NUM);
    //    MCUfilterMemCopy(&gridVoltFilterLowB,           GRID_VOLT_LOWDEFAULT_NUM);
    //    MCUfilterMemCopy(&gridVoltFilterLowC,           GRID_VOLT_LOWDEFAULT_NUM);
    //    MCUfilterMemCopy(&PLLAGCFilter,                 PLL_AGC_FILTER_DEFAULT_NUM);
    //    MCUfilterMemCopy(&gridVoltPosiDFilter,          PLL_VOLT_Q_DEFAULT_NUM);
    //    MCUfilterMemCopy(&gridVoltPosiQFilter,          PLL_VOLT_Q_DEFAULT_NUM);
    //    MCUfilterMemCopy(&gridVoltNegDFilter,           PLL_VOLT_Q_DEFAULT_NUM);
    //    MCUfilterMemCopy(&gridVoltNegQFilter,           PLL_VOLT_Q_DEFAULT_NUM);
    //    MCUfilterMemCopy(&RectPowerDispFilter,          RECTPOWER_DISP_DEFAULT_NUM);
    //    MCUfilterMemCopy(&ActivePowerDispFilter,        RECTPOWER_DISP_DEFAULT_NUM);
}

void CLAfilterMemCopy(DCL_DF22_CLA *dst,int16 num)
{
//    int16 i;
//
//    Uint32 *pInitVar    =(Uint32 *)(&ApfOutA);
//    Uint32 *pSrcVar     =(Uint32 *)(&CLAFilterCoeff[T1PRPwmFrequency][num]);
//    for(i=0;i<sizeof(ApfOutA);i++)
//        *pInitVar=*pSrcVar;
}

void InitCLAFilter(void)
{
}

void InitAfterReadROM(void)
{
    InitCLAFilter();
    InitMCUFilter();
    StateFlag.RecordWritting=1;
//    cbInit(&FlashWaveFifo,FLASHWAVE_FIFO_NUM);
}

void InitBeforeReadROM(void)
{
	int16 i;
	Uint16 *pInitVar;

	pInitVar = (Uint16 *)(&StateFlag);
	for(i=0;i<sizeof(StateFlag);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(tmpOffsetValue);
	for(i=0;i<sizeof(tmpOffsetValue);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(FlvSlot);
	for(i=0;i<sizeof(FlvSlot);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&CntMs);
	for(i=0;i<sizeof(CntMs);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&CntSec);
	for(i=0;i<sizeof(CntSec);i++)
		*pInitVar++ = 0;

    pInitVar = (Uint16 *)(&ESCFlagA);
    for(i=0;i<sizeof(ESCFlagA);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&ESCFlagB);
    for(i=0;i<sizeof(ESCFlagB);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&ESCFlagC);
    for(i=0;i<sizeof(ESCFlagC);i++)
        *pInitVar++ = 0;

	pInitVar = (Uint16 *)HarmTHD;
	for(i=0;i<sizeof(HarmTHD);i++)
		*pInitVar++=0;

	pInitVar = (Uint16 *)(&CntFaultDelay);
	for(i=0;i<sizeof(CntFaultDelay);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&CntFaultDelayLong);
	for(i=0;i<sizeof(CntFaultDelayLong);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&SyncInformation);
	for(i=0;i<sizeof(SyncInformation);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)&IncidentRecord;
	for(i=0;i<sizeof(IncidentRecord);i++)
		*pInitVar++ = 0;

    pInitVar = (Uint16 *)(&VoltSlid);
    for(i=0;i<sizeof(VoltSlid);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&VoltSlidA);
	for(i=0;i<sizeof(VoltSlidA);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&VoltSlidB);
	for(i=0;i<sizeof(VoltSlidB);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&VoltSlidC);
	for(i=0;i<sizeof(VoltSlidC);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&GridCurrSlidA);
    for(i=0;i<sizeof(GridCurrSlidA);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&GridCurrSlidB);
    for(i=0;i<sizeof(GridCurrSlidA);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&GridCurrSlidC);
    for(i=0;i<sizeof(GridCurrSlidA);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&RmsSumIn);
    for(i=0;i<sizeof(RmsSumIn);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&RmsSumOut);
    for(i=0;i<sizeof(RmsSumOut);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&RmsSumQ);
    for(i=0;i<sizeof(RmsSumQ);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&DQbufIn);
    for(i=0;i<sizeof(DQbufIn);i++)
        *pInitVar++ = 0;

//	pInitVar = (Uint16 *)&GcurSlidA[0];
//	for(i=0;i<sizeof(GcurSlidA);i++)
//		*pInitVar++ = 0;
//
//	pInitVar = (Uint16 *)&GcurSlidB[0];
//	for(i=0;i<sizeof(GcurSlidB);i++)
//		*pInitVar++ = 0;
//
//	pInitVar = (Uint16 *)&GcurSlidC[0];
//	for(i=0;i<sizeof(GcurSlidC);i++)
//		*pInitVar++ = 0;
//
//	pInitVar = (Uint16 *)&LoadSlidA[0];
//	for(i=0;i<sizeof(LoadSlidA);i++)
//		*pInitVar++ = 0;
//
//	pInitVar = (Uint16 *)&LoadSlidB[0];
//	for(i=0;i<sizeof(LoadSlidB);i++)
//		*pInitVar++ = 0;
//
//	pInitVar = (Uint16 *)&LoadSlidC[0];
//	for(i=0;i<sizeof(LoadSlidC);i++)
//		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&harmCompPercParameter);
	for(i=0;i<sizeof(harmCompPercParameter);i++)
//		*pInitVar++ = 10000;
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&Harmonic);
	for(i=0;i<sizeof(Harmonic);i++)
		*pInitVar++ = 0;

	pInitVar = (Uint16 *)(&SOE);
	for(i=0;i<sizeof(SOE);i++)
		*pInitVar++ = 0;

    pInitVar = (Uint16 *)(&Modbuff[0][0]);
    for(i=0;i<sizeof(Modbuff[0][0]);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&Modbuff[1][0]);
    for(i=0;i<sizeof(Modbuff[1][0]);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&Modbuff[2][0]);
    for(i=0;i<sizeof(Modbuff[2][0]);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(&RecordFlash);
    for(i=0;i<sizeof(RecordFlash);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)&ErrorRecord;
    for(i=0;i<sizeof(ErrorRecord);i++)
        *pInitVar++ = 0;

    pInitVar = (Uint16 *)(TestLoadCur);
    for(i=0;i<sizeof(TestLoadCur);i++)
        *pInitVar++ = 0;


//	pInitVar = (Uint16 *)(&GenFFTCoeff);
//	for(i=0;i<sizeof(GenFFTCoeff);i++)
//		*pInitVar++ = 0;
    Information_Structure.Correction.all = 0;
	RMSAllInit();
//	MeanAllInit();
    cbInit(&BlockFifo,FIFO_DQ_BLOCK_NUM);
    varMean_initialize();
	FM_Write_Status(UNLOCK);				//StateFlag初始化后解锁
    FM_DatWordWrite(0x1FFE,63);
    FM_DatWordWrite(0x7FFE,255);
    Uint16 TypeJudgment1,TypeJudgment2;
    TypeJudgment1 = FM_DatReadWord(0x1FFE);
    TypeJudgment2 = FM_DatReadWord(0x7FFE);
    if((TypeJudgment1==63)&&(TypeJudgment2==255))
    {
        StorageTypeFlag = 1;
//        FM_DatRead16(WAVE_RECORD_START_ADDR,sizeof(debugW),&debugW[0][0]);
    }

	SaveDataEead();
	SetResetExecute();  					//Fault flag reset
	StateFlag.onceTimeAdcAutoAdjust = 1;	//上电,零偏校准一次
	ESCFlagA.autoStFlag = ORIGINAL_STATE_A;  //上电,复位自启动状态机
	ESCFlagB.autoStFlag = ORIGINAL_STATE_B;
	ESCFlagC.autoStFlag = ORIGINAL_STATE_C;
	StateEventFlag_A = STATE_EVENT_STANDBY_A;   //上电,复位状态机
	StateEventFlag_B = STATE_EVENT_STANDBY_B;
	StateEventFlag_C = STATE_EVENT_STANDBY_C;
	ESCFlagA.FaultJudgeFlag = 0;  //上电初始化为0
	ESCFlagB.FaultJudgeFlag = 0;
	ESCFlagC.FaultJudgeFlag = 0;
	StateFlag.ManufacturerParameters = true;
    CurrThisPos = 0;
    CntSec.StopDelay = 0;
    ESCFlagA.BYFEEDBACKFLAG = 1;
    ESCFlagB.BYFEEDBACKFLAG = 1;
    ESCFlagC.BYFEEDBACKFLAG = 1;

    ESCFlagA.onceRunStartFlag = 0;
    ESCFlagB.onceRunStartFlag = 0;
    ESCFlagC.onceRunStartFlag = 0;

    ESCFlagA.onceTimeStateMachine = 1;     //上电,延时200ms置为导通晶闸管状态
    ESCFlagB.onceTimeStateMachine = 1;
    ESCFlagC.onceTimeStateMachine = 1;
    ESCFlagA.ESCCntSec.PRECHARGEDelayBY = 0;//防止上电因运输工程中旁路磁保持误动作而炸机,复位初始化
    ESCFlagB.ESCCntSec.PRECHARGEDelayBY = 0;
    ESCFlagC.ESCCntSec.PRECHARGEDelayBY = 0;
    ESCFlagA.ESCCntSec.PRECHARGEDelay = 0;//防止上电因运输工程中高/低压磁保持误动作而炸机.复位初始化
    ESCFlagB.ESCCntSec.PRECHARGEDelay = 0;
    ESCFlagC.ESCCntSec.PRECHARGEDelay = 0;
    ESCFlagA.RELAYCONTROLFlag = 1;//上电继电器控制标志位,复位初始化
    ESCFlagB.RELAYCONTROLFlag = 1;
    ESCFlagC.RELAYCONTROLFlag = 1;
    ESCFlagA.FAULTCONFIRFlag = 0; //用来确认是哪一相发生故障,复位初始化
    ESCFlagB.FAULTCONFIRFlag = 0;
    ESCFlagC.FAULTCONFIRFlag = 0;
    ESCHighLowRelayCNTA = 0;
    ESCHighLowRelayCNTB = 0;
    ESCHighLowRelayCNTC = 0;
    ESCSicFaultCNTA = 0;
    ESCSicFaultCNTB = 0;
    ESCSicFaultCNTC = 0;
    ESCBYRelayCNTA = 0;
    ESCBYRelayCNTB = 0;
    ESCBYRelayCNTC = 0;
    ESCFlagA.PHASE = 1;             //A相
    ESCFlagB.PHASE = 2;             //B相
    ESCFlagC.PHASE = 3;             //C相
    SET_POWER_CTRL(1);  //初始化上电,给15V供电
    Delayus(TIME_WRITE_15VOLT_REDAY);

    CurrPrvPos= (MEANPOINT_QUARTER_NUM-MEANPOINT_QUARTER);
//    SET_FLback(0);//测试debug
//    SET_BYPASS_FEEDBACK(0);//测试debug
}

void clock1ms(void)
{
	ServiceDog();
	if (++Time.MicroSec >= 999)//1秒
		Time.MicroSec = 0;

	if (SCIC.SendTrsformDelayTim)
	{
		if (--SCIC.SendTrsformDelayTim == 0)
			vMBPortSerialEnable(&SCIC, TRUE, FALSE);
	}

	if (SCIB.SendTrsformDelayTim)
	{
		if (--SCIB.SendTrsformDelayTim == 0)
			vMBPortSerialEnable(&SCIB, TRUE, FALSE);
	}

	if (SCID.SendTrsformDelayTim)
	{
		if (--SCID.SendTrsformDelayTim == 0)
			vMBPortSerialEnable(&SCID, TRUE, FALSE);
	}

	if (SCICMASTERENABLE == 1)
		ModBusMasterTimeCal(&SCIC);
	else
		ModBusSlaveTimeCal(&SCIC);

	ModBusSlaveTimeCal(&SCIB);

	if (SCIDMASTERENABLE == 1)
		ModBusMasterTimeCal(&SCID);
	else
		ModBusSlaveTimeCal(&SCID);
	if (++CapEmergTim >= 4500)
		CapEmergTim = 4500;
	if (++RandTimCount >= 60)
		RandTimCount = 60;
}

/*
 * 功能：节拍产生。WY
 *
 * 说明：该函数的执行周期为5ms。
 */
void clock5ms(void)
{
	static int j = 2;
	int16 *pCnt;
	Uint16 i;

	pCnt = (int16*) &CntMs;
	for (i = sizeof(CntMs); i != 0; i--)
	{
		if ((*pCnt)++ > 30000)
			(*pCnt) = 30000;
		pCnt++;
	}

	pCnt = (int16*) &ESCFlagA.ESCCntMs;
	for (i = sizeof(ESCFlagA.ESCCntMs); i != 0; i--)
	{
		if ((*pCnt)++ > 30000)
			(*pCnt) = 30000;
		pCnt++;
	}

	pCnt = (int16*) &ESCFlagB.ESCCntMs;
	for (i = sizeof(ESCFlagB.ESCCntMs); i != 0; i--)
	{
		if ((*pCnt)++ > 30000)
			(*pCnt) = 30000;
		pCnt++;
	}

	pCnt = (int16*) &ESCFlagC.ESCCntMs;
	for (i = sizeof(ESCFlagC.ESCCntMs); i != 0; i--)
	{
		if ((*pCnt)++ > 30000)
			(*pCnt) = 30000;
		pCnt++;
	}

	if (j-- <= 0)
	{
		Event_post(Event_SetStartSM, Event_Id_01);
		Event_post(Event_CapSwitchSM, Event_Id_02);
		Event_post(Event_Outside, Event_Id_00);
		Event_post(Event_SCI_Reveived, Event_Id_00 | Event_Id_01 | Event_Id_02);
		j = 1;
	}
	MainContactConfirm(25); //该函数未定义。WY
}

void clock1Second(void)
{
	int16 *pCnt;
	Uint16 i;

	if (TimSwitchOnStart == 1)
	{
		if (++SwitchTimeOn > 600)
			SwitchTimeOn = 600;
	}
	if (TimSwitchOffStart == 1)
	{
		if (++SwitchTimeOff > 600)
			SwitchTimeOff = 600;
	}
	if (++FlashRecordTim > 10)
		StateFlag.RecordWritting = 1;
	if (++ReFFTDelayTim > 40)
		ReFFTDelayTim = 40;
	if (++CpuHeatDelayTime > 15)
		CpuHeatDelayTime = 15;

	pCnt = &CntSec.MeasureDelay;
	for (i = 0; i < sizeof(CntSec); i++)
	{
		if ((*pCnt)++ > 3600)
			(*pCnt) = 3600;
		pCnt++;
	}
	pCnt = &ESCFlagA.ESCCntSec.ChargingTime;
	for (i = 0; i < sizeof(ESCFlagA.ESCCntSec); i++)
	{
		if ((*pCnt)++ > 3600)
			(*pCnt) = 3600;
		pCnt++;
	}
	pCnt = &ESCFlagB.ESCCntSec.ChargingTime;
	for (i = 0; i < sizeof(ESCFlagB.ESCCntSec); i++)
	{
		if ((*pCnt)++ > 3600)
			(*pCnt) = 3600;
		pCnt++;
	}
	pCnt = &ESCFlagC.ESCCntSec.ChargingTime;
	for (i = 0; i < sizeof(ESCFlagC.ESCCntSec); i++)
	{
		if ((*pCnt)++ > 3600)
			(*pCnt) = 3600;
		pCnt++;
	}

	Fan_Cnt();
	if (++Time.Second >= 60)
	{//1分钟定时计数
		Time.Second = 0;
		cntForAutoStIn30++;
		if (++Time.Minute >= 60)
		{// 1小时定时计数
			Time.Minute = 0;

			if (++Time.Hour >= 24)
			{
				Time.Hour = 0;
				if (++Time.Day > Monthnum(Time.Year, Time.Month))//加万年历程序
				{
					Time.Day = 1;
					if (++Time.Month > 12)
					{
						Time.Month = 1;
						Time.Year++;
					}
				}
			}
		}
	}
}

//2 功能控制模块
//功能控制模块包括每月天数判定功能、每年天数判定功能、星期计算功能。分别由函数Monthnum()、Yearnum()、IsWeek（）和*getWeek()
//（1）   int Monthnum(inty,int m)，判定每月天数。
//（2）   int Yearnum(int y)，判定第年天数。
//（3）   int isWeek(inty,int m)，根据给定年月计算该月第一天对应的星期。
//（4）   char *getWeek(inty,int m,int d)，根据给定年月日计算其对应的星期。
//#define LeapYear (y) (y%4==0&&y%100!=0||y%400==0)?1:0

int Monthnum(unsigned char y,unsigned char m)
{
	static int month[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	if(y%4==0)
		month[2]=29;//闰年二月29天，平年28天
	return month[m];
}
