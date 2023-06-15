#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "CoreCtrl.h"
#include "VariblesRef.h"
#include "stdio.h"
#include "F2837xS_device.h"
#include "math.h"
#include "C28x_FPU_FastRTS.h"
#include "float.h"
#include "fpu_rfft.h"
#include "stdlib.h"

//#pragma CODE_SECTION(TestWaveGenerator, "ramfuncs");
float xRandSet = 10;

void TestWaveGenerator(void){      //谐波生成
	float32 *pCMagA=GenCorr.MagA, *pCMagB=GenCorr.MagB, *pCMagC=GenCorr.MagC;
	float32 *pCPhaA=GenCorr.PhaA, *pCPhaB=GenCorr.PhaB, *pCPhaC=GenCorr.PhaC;
	Uint16	*pCOrd=GenCorr.harmOrder;

	float32 stepPha=0;							//Generating inverter side compensation current
	Uint16 i,j;
	float32 y0,y1,y2;
	int16 xRand=0;


	for(j=0;j<GEN_FUNDPOINT;j++){
//	    srand(Seed);
		stepPha += (PI2/(GEN_FUNDPOINT));		//step=2*pi*(i-1)/waveLen
		y0=0;y1=0;y2=0;
		pCOrd=GenCorr.harmOrder;
		pCMagA=GenCorr.MagA; pCMagB=GenCorr.MagB; pCMagC=GenCorr.MagC;
		pCPhaA=GenCorr.PhaA; pCPhaB=GenCorr.PhaB; pCPhaC=GenCorr.PhaC;

		for(i=0;i<GENERATOR_COEFF_LEN;i++){
	        xRand=( xRandSet*(1.0f/32768))*rand();   //加入最大值为10A的白噪声
		    y0+=*pCMagA * sin(PhaseLimit( *pCOrd * stepPha + *pCPhaA));//+xRand;
			y1+=*pCMagB * sin(PhaseLimit( *pCOrd * stepPha + *pCPhaB));//+xRand;
			y2+=*pCMagC * sin(PhaseLimit( *pCOrd * stepPha + *pCPhaC));//+xRand;
			pCOrd++; pCPhaA++;pCPhaB++;pCPhaC++; pCMagA++;pCMagB++;pCMagC++;
		}
		TestLoadCur[0][j]=y0*reactPowGivenLimit*(SQRT2/100); //扩大10倍
		TestLoadCur[1][j]=y1*reactPowGivenLimit*(SQRT2/100);
		TestLoadCur[2][j]=y2*reactPowGivenLimit*(SQRT2/100);
	}
}
//===========================================================================
// End of File
//===========================================================================

