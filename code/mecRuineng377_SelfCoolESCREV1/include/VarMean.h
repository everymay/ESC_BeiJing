/* varMean.h */
#include <stddef.h>
#include "CoreCtrl.h"
#ifndef RTW_HEADER_varMean_h_
#define RTW_HEADER_varMean_h_

#define MEAN_N (35)       //test
#define Step                             (VarMeanStep)  // 8K,9.6K(2*PWMFREQUENCY/6);12.8k,16K(PWMFREQUENCY/6)
#define Ts                               (1.0f/Step)       //采样周期
#define Integ4_gainval_d                 (0.5*Ts)
#define Gain1_Gain_o                     (0.5)
#define Toavoiddivisionbyzero_UpperSa_o  (52.5)         //频率限定上限
#define Toavoiddivisionbyzero_LowerSa_h  (47.5)         //频率限定下限
#define Gain_Gain_e                      (Ts)
#define bufSz                            (128+2)        //缓冲区长度dvtd_buffer[68][MEAN_N]//  68=VarMeanStep/(50-fv)向上取整   fv=频率波动范围
extern float rtb_Vq[MEAN_N];
extern float VarMeanOut[MEAN_N];
//extern float dvtd_buffer[bufSz][MEAN_N];
void varMean_step(void);

typedef struct {
////////////////////////////////////////////////////////////////DW_varMean_T
    float Integ4_DSTATE[MEAN_N];                /* Expression: 0    '<S6>/Integ4' */
    float UnitDelay_DSTATE_n[MEAN_N];           /* Expression: 0    '<S18>/Unit Delay' */
//////////////////////////////////////////////////////////////////P_varMean_T_
//	float Gain1_Gain_o;                    // Expression: 0.5        Referenced by: '<S18>/Gain1'
//	float Integ4_gainval_d;                // Expression: 0.5*Ts     Referenced by: '<S6>/Integ4'
//	float Toavoiddivisionbyzero_UpperSa_o; // Expression: 1e6        Referenced by: '<S6>/To avoid division by zero'
//	float Toavoiddivisionbyzero_LowerSa_h; // Expression: 1e-6       Referenced by: '<S6>/To avoid division by zero'
//	float Gain_Gain_e;                     // Expression: Ts         Referenced by: '<S6>/Gain'
/////////////////////////////////////////////////////////////B_varMean_T
    float Integ4[MEAN_N];                       /* '<S6>/Integ4' */
    float Delay;                        /* '<S6>/Gain' */
    float SFunction[MEAN_N];                    /* '<S19>/S-Function ' */
    float dvtd_buffer[bufSz][MEAN_N];
////////////////////////////////////////////////////////////////DW_varMean_T
//  void *uBuffers;                     /* '<S19>/S-Function ' */
//    float UnitDelay_DSTATE;           /* Expression: fs   '<S1>/Unit Delay' */
//	void *uBuffers;                     /* '<S19>/S-Function ' */
//	int   bufSz;                        /* Expression: 128  '<S19>/S-Function ' */
//	int   maxDiscrDelay;                /* Expression: 127  '<S19>/S-Function  maxDiscrDelay = bufSz-1   */
    int   indBeg;                       /* Expression: 0    '<S19>/S-Function ' */
    int   indEnd;                       /* Expression: 1    '<S19>/S-Function ' */
    int Integ4_SYSTEM_ENABLE;           /* Expression: 1U   '<S6>/Integ4' */

} B_varMean_T;
//#define varMean_B_data {0,0,0,0,0,0,0,1,128,127,1U,0.5,0.5*Ts,1e6,1e-6,Ts,0,0,0,0,0,0,0,0,0}
extern B_varMean_T varMean_B;
extern void varMean_initialize(void);
extern void varMean_step(void);

/*
varMean_B.Gain_Gain_e = Ts;
varMean_B.Integ4_DSTATE[MEAN_N] = 0;
varMean_B.UnitDelay_DSTATE_n[MEAN_N] = 0;
varMean_B.Delay = 0;
varMean_B.Gain1_Gain_o = 0.5;
varMean_B.Integ4[MEAN_N] = 0;
varMean_B.Integ4_DSTATE[MEAN_N] = 0;
varMean_B.Integ4_SYSTEM_ENABLE = 1U;
varMean_B.Integ4_gainval_d = 0.5*Ts;
varMean_B.SFunction[MEAN_N] = 0;
varMean_B.Toavoiddivisionbyzero_LowerSa_h = 1e-6;
varMean_B.Toavoiddivisionbyzero_UpperSa_o = 1e6;
varMean_B.bufSz = 128;
varMean_B.maxDiscrDelay = 127;
varMean_B.indEnd = 1;
varMean_B.indBeg = 0;
*/

////fifo////////////////////////////////////////////////////////////////////////////////
#define FIFO_DQ_BLOCK_NUM 1<<1      //块必须是2的整数次幂,否则不能正常
#define FIFO_DQ_DATA_SIZE MEAN_N+1    //元素的数量,可更换为结构体等其他形式

typedef float ElemType[FIFO_DQ_BLOCK_NUM][FIFO_DQ_DATA_SIZE];
typedef struct {
    int size;            // maximum number of elements         //内存大小
    int start;           // index of oldest element            //开始指针
    int end;             // index at which to write new element//结束指针
} CircularBuffer;

extern CircularBuffer BlockFifo;
//extern float32 ELEMS_DAT[FIFO_DQ_BLOCK_NUM][FIFO_DQ_DATA_SIZE];
extern ElemType ELEMS_DAT;
void cbInit(CircularBuffer *cb, int size);
void cbWrite(CircularBuffer *cb,float *in);
void cbRead(CircularBuffer *cb,float *out);
int cbIsEmpty(CircularBuffer *cb);

////////////
#endif    /* RTW_HEADER_varMean_h_ */
