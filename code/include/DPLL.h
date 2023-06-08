#ifndef DPLL_H_
#define DPLL_H_

#include "CoreCtrl.h"
#include "DCL.h"

#define GRIDVOL_DQFILTER_LEN 5
typedef struct{
    DF22 filter[GRIDVOL_DQFILTER_LEN];
    PIcalc PLLPICtrl;
    float32 PLLResSin;
    float32 PLLResCos;
    float32 PLLRes2Sin;
    float32 PLLRes2Cos;
    float32 GridPLLVoltDn;
    float32 GridPLLVoltQn;
    float32 GridPLLVoltD;
    float32 GridPLLVoltQ;
    float32 Theta;
    float32 PllPiOutput;
    float32 Buf[GRIDVOL_DQFILTER_LEN];
}SPLL_3ph_DDSRF;
extern SPLL_3ph_DDSRF SPLL[3];
extern SPLL_3ph_DDSRF SPLL_CUR;
void initDPLL(SPLL_3ph_DDSRF *SPLL);
void DPLL(void);
#endif
