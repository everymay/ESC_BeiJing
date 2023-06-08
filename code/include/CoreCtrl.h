/*
 * CoreCtrl.h
 *
 *  Created on: 2017-8-17
 *      Author: JCL
 */

#ifndef CORECTRL_H_
#define CORECTRL_H_
#include "VariblesRef.h"
//#include "F2837xS_device.h"
#include "Cla_data.h"
#include "CLAmath.h"

#define TESTMODE 					0	//娴嬭瘯妯″紡鎵撳紑
#if TESTMODE
#define RELESE_HIGHVOLTAGE 			0	//瀹氫箟楂樼數鍘嬫寮忓彂甯冪殑绋嬪簭
#define TEST_RUNPORCESS				0	//瀹氫箟涓嶅悎闂告姇杩愮▼搴�
#define TEST_NULLPULSE				0	//瀹氫箟绌鸿剦鍐茬▼搴�
#define TEST_IGBTFAULTDISABLE		0	//IGBT鏁呴殰鍏虫柇
#define TEST_IO						0	//瀹氫箟IO娴嬭瘯绋嬪簭
#define TEST_VIRTUALSOURCE			0	//瀹氫箟铏氭嫙婧愯緭鍏ユā寮�
#define TEST_DEBUGFFT               0   //瀹氫箟璋冭瘯FFT
#define TEST_RUNTIME				0   //娉ㄦ剰,涓婂彛缁濆涓嶈兘甯︾數.姝ゆā寮忕洿鎺ヨ繍琛孖GBT绋嬪簭.
#define DEBUG_WAVE_INNER 			0	//IO鍙ｈ緭鍑鸿剦鍐叉祴璇曡繍琛屾椂闂�0:鍏抽棴1:CPU鎺у埗 2:CLA鎺у埗
//#define DEBUG_NO_MAIN_CONTACT_ON  	0	//浣庡帇璋冭瘯闃舵涓嶆姇鍗曞厓鍐呮帴瑙﹀櫒
#endif

#define  COMMUNCICATION_VERSION		3

#define  PWM_FREQUENCE_8KHZ         0
#define  PWM_FREQUENCE_9p6KHZ       0
#define  PWM_FREQUENCE_12p8KHZ      0
#define  PWM_FREQUENCE_16KHZ        1
#define  THREE_LEVEL_PWM            1       //0:Two level 1:three level
#define  HEATPULSE                  1

#define  ESC_SINGLEPHASECTRL        0        //单相ESC设备
#define  ESC_THREEPHASECTRL         1       //三相ESC设备

#define  ESC_SINGLEPHASE            0       //单相ESC设备
#define  ESC_THREEPHASE             1      //三相ESC设备
#define  ESC_ONEINTWOPHASE          0       //两相并成一相设备,PWM输出都由A相控制
#define  ESC_ONEINTHREEPHASE        0       //三相并成一相设备,PWM输出都由A相控制

//--------------------------------渚嬬▼瀹氫箟DMA---------------------------------//
#define CALIBRATION_COEFF_LEN       15

#define GENERATOR_COEFF_LEN         9
#define SINE_LOOKTABLE_LEN          512
#define PI                          (3.141593f)
#define PI2                         (2*PI)
#define PI2_SINE_LOOKTABLE          (SINE_LOOKTABLE_LEN)
#define FFT_MAGTIUTE_COEFF          (2.0f/RFFT_SIZE)
#define RFFT_STAGES                 8
#define RFFT_SIZE                   (1 << RFFT_STAGES)              //Define the length of the FFT
#define MAGCORR(x)                  ((x)*FFT_MAGTIUTE_COEFF)
#define PWMFREQUENCY_cla            12800
#define CLAADTEMPBUFLEN             4           //姣忛殧4涓偣,鐢盋PU寰�ADBuffA缂撳啿鍖烘斁涓�娆�,鍥犱负DMA涓嶆敮鎸丆LA

extern Uint16 ADGROUP_NUM;
extern Uint16 ADGROUP_MODE;
extern Uint16 WAVE_REC_FFT_MODE_DATA2;

#define CAPPERIODVAL                1/100000000     //鏃堕挓棰戠巼鍛ㄦ湡鍊�

/*PWM强制持续输出高电平。WY*/
#define EPwmRegsAQCSFRCclear 2

/*PWM强制持续输出低电平。WY*/
#define EPwmRegsAQCSFRC   1

#define FANCtrlMaxDUTY  0.8
#define FANCtrlMinDUTY  0.2

//#if PWM_FREQUENCE_8KHZ
//    #define ADGROUP_NUM 5
//    #define ADGROUP_MODE 0
//    #define WAVE_REC_FFT_MODE_DATA2 3
//#elif PWM_FREQUENCE_9p6KHZ
//    #define ADGROUP_NUM 6
//    #define ADGROUP_MODE 1
//    #define WAVE_REC_FFT_MODE_DATA2 3
//#elif PWM_FREQUENCE_12p8KHZ
//    #define ADGROUP_NUM 8
//    #define ADGROUP_MODE 2
//    #define WAVE_REC_FFT_MODE_DATA2 4
//#elif PWM_FREQUENCE_16KHZ
//    #define ADGROUP_NUM 10
//    #define ADGROUP_MODE 3
//    #define WAVE_REC_FFT_MODE_DATA2 5
//#endif
//2:In phase carrier (three phase carrier frequency with same phase, suitable for three wire); 1: inverting carrier (carrier free frequency harmonic, suitable for four wire)
//2:鍚岀浉杞芥尝(涓夌浉鏈夊悓鐩镐綅杞芥尝棰戠巼,閫傜敤浜庝笁鐩镐笁绾�);1:鍙嶇浉杞芥尝(鏃犺浇娉㈤鐜囪皭娉�,閫傜敤浜庝笁鐩稿洓绾�)
#define  PWM_CARRIER_PHASE_OPPOSITION      1
#if  THREE_LEVEL_PWM  == 1
    #define NPC3CA10020210330REV1   1       // 浣庡帇ESC
    #define TNMD150A200415REV1      0       //S-100kva T5 涓夌數骞�
#else
    #define TNMD150A200415REV1      0       //S-100kva T5 涓夌數骞�
#endif

#if  THREE_LEVEL_PWM == 1 && NMECA161126REV4_200 == 1
	#define PROJ_OUYAJIXIE			1		//ou ya ji xie娆т簹鏈烘椤圭洰660(瑕佸湪涓夌數骞冲ぇ鏈虹涓�1鏃舵湁鏁�)
#endif

    #define NESPW20220307REV1       1       //NPC3CA10020220210REV4-鍚堟垚鐗�

//纭欢鐗堟湰鍙�(涓荤増鏈彿):
//000**~326**:绯诲垪鐗堟湰鍙�,鍦ㄦ柊绯诲垪,鏂版嫇鎵�.
//***00~***99: 鍚岀郴鍒楀洜涓洪」鐩殑涓嶅悓,鎴栫‖浠剁◢浣滀慨鏀�
#if  THREE_LEVEL_PWM  == 1
    #if NPC3CA10020210330REV1 == 1
        #define DSPSOFT_VERSION_MAJOR   (int16)1680 //(range:0~326**)
    #elif TNMD150A200415REV1 == 1
        #define DSPSOFT_VERSION_MAJOR   (int16)1680 //(range:0~326**)
    #endif
#endif
//瀛愮増鏈彿
//000**~326**:PWM棰戠巼
//***00~***99:瀛愮増鏈彿.杞欢鍙戝竷鏃朵笌涓婁竴鐗堜笉鍚�,姝ょ増鏈彿+1.
#if  THREE_LEVEL_PWM == 1
	#define VERSION_CHILD 	 		(int16)10	//(range:0~99)
#endif

#if PWM_FREQUENCE_8KHZ
#define DSPSOFT_VERSION_CHILD 	VERSION_CHILD
#define DSPSOFT_VERSION_FREQC	800
#elif PWM_FREQUENCE_9p6KHZ
#define DSPSOFT_VERSION_CHILD 	VERSION_CHILD
#define DSPSOFT_VERSION_FREQC	9600
#elif PWM_FREQUENCE_12p8KHZ
#define DSPSOFT_VERSION_CHILD 	VERSION_CHILD
#define DSPSOFT_VERSION_FREQC	12800
#elif PWM_FREQUENCE_16KHZ
#define DSPSOFT_VERSION_CHILD 	VERSION_CHILD
#define DSPSOFT_VERSION_FREQC	40000
#endif
#define DSPSOFT_VERSION_YEAR	(int16)TESTMODE*100+23
#define DSPSOFT_VERSION_MONTH	(int16)2
#define DSPSOFT_VERSION_DAY		(int16)22
#define DSPSOFT_VERSION_M_DAY	(int16)(DSPSOFT_VERSION_MONTH*100+DSPSOFT_VERSION_DAY)

//#define SPLL_3PH_DDSRF	1//1:SPLL 3ph decoupled double synchronous reference frame based

#if PWM_FREQUENCE_8KHZ
    #define PWMFREQUENCY  8000							//PWM switching frequency and Current loop frequency
	#define IFFT_PHASE_STEP PI2/(2*PWMFREQUENCY/50)
	#define VarMeanStep     (2*PWMFREQUENCY/6)
#elif PWM_FREQUENCE_9p6KHZ
    #define PWMFREQUENCY  9600							//PWM switching frequency and Current loop frequency
	#define IFFT_PHASE_STEP PI2/(2*PWMFREQUENCY/50)
	#define VarMeanStep     (2*PWMFREQUENCY/6)
#elif PWM_FREQUENCE_12p8KHZ
    #define PWMFREQUENCY  12800						    //PWM switching frequency and Current loop frequency
	#define IFFT_PHASE_STEP PI2/(2*PWMFREQUENCY/50)
	#define VarMeanStep     (2*PWMFREQUENCY/4)
#elif PWM_FREQUENCE_16KHZ
    #define PWMFREQUENCY  20000							//PWM switching frequency and Current loop frequency//寮�鍏抽鐜�40K  AD涓柇棰戠巼20K
	#define IFFT_PHASE_STEP PI2/(PWMFREQUENCY/50)
	#define VarMeanStep     (PWMFREQUENCY/2.5f)
#endif

#define POWERGRID_FREQ              50                              //Power grid frequency
#define CTRLFREQUENCY 10000								//Other module control frequency
#define WAVERECORDLEN 500								//Debug window points
#define CTRLSTEP_ALGORITHM   	(1.0f/CTRLFREQUENCY)			//Other module control step
#define CTRLSTEP_ALGORITHM_F 	(1.0f/(10000))			//Current loop control step
#define FUNDPOINT               (PWMFREQUENCY/POWERGRID_FREQ) //Fundamental point鍩烘尝鐐规暟
#define GEN_FUNDPOINT           (PWMFREQUENCY/POWERGRID_FREQ) //娉㈠舰浜х敓鍩烘尝鐐规暟
#define VOLT_FUNDPOINT			(PWMFREQUENCY/POWERGRID_FREQ)	//Fundamental point鍩烘尝鐐规暟    //鐢电綉鐢靛帇閲囨牱鐐�
#define MEANPOINTHALF           ((VOLT_FUNDPOINT/4) + 1)

//#define	RFFT_STAGES					8
//#define	RFFT_SIZE					(1 << RFFT_STAGES)				//Define the length of the FFT
//-----------------------------------------------------------------------------
// Float32 Real FFT Definitions and Prototypes
//-----------------------------------------------------------------------------
#define FFT_PHASE_STEP  (360.0f*(1.0f/(PWMFREQUENCY*2))/0.02) 	//1.125degree@16kHz
#if PWM_FREQUENCE_8KHZ
#define HARD_PHASE_DELAY (0.0000005f/*+ other delay*/) 			//quantization error+Transformer and analog filter circuit and AD delay
#elif PWM_FREQUENCE_9p6KHZ
#define HARD_PHASE_DELAY (0.0000005f/*+ other delay*/) 			//quantization error+Transformer and analog filter circuit and AD delay
#elif PWM_FREQUENCE_12p8KHZ
#define HARD_PHASE_DELAY (0.0000005f/*+ other delay*/) 			//quantization error+Transformer and analog filter circuit and AD delay
#elif PWM_FREQUENCE_16KHZ
#define HARD_PHASE_DELAY (0.0000005f/*+ other delay*/) 			//quantization error+Transformer and analog filter circuit and AD delay
#endif

#define StandMach_ID				0x20
#define FFT_MAGTIUTE_COEFF 			(2.0f/RFFT_SIZE)
#define MAGCORR(x) 					((x)*FFT_MAGTIUTE_COEFF)
//#define CALIBRATION_COEFF_LEN 		29							//Only to open  sets of harmonic compensation.
//#define GENERATOR_COEFF_LEN 		9
#define PRCTRL_COEFF_LEN 			14							//Note that the assembly file should be modified
#define SINE_LOOKTABLE_LEN 			512
#define ADTEMPBUFLEN 				(RFFT_SIZE+14)       //AD涓存椂缂撳啿鍖�
#define PI 							(3.141593f)
#define PI2 						(2*PI)
#define PI100                       (314.1592653f)
#define PI2_SINE_LOOKTABLE 			(SINE_LOOKTABLE_LEN)
#define PLL_STEP 					(SINE_LOOKTABLE_LEN/PI2)
#define R2D(x) 						((x)*(180.0f/PI))			//The radians is converted to angle
#define D2R(x) 						((x)*(PI/180.0f))			//The angle is converted to radians
#define D2RDLY(x,pha) 				D2R( FFT_PHASE_STEP*(x)*(2)+(pha)+HARD_PHASE_DELAY )	//
#define SQRT3 						1.732051f
#define SQRT2 						1.414214f
#define SQRT_2DIV3 					0.816497f
#define SQRT_3DIV2 					1.224745f
#define SQRT2_DIV2 					0.707107f
#define S1DIVSQRT2 					0.707107f
#define S1DIVSQRT3 					0.577350f
#define SQRT_2DIV3_DIV2 			0.408248f
#define S1DIV3 						0.333333f
#define S1DIV6 						0.166667f
#define REACTLIMITPH				1.6
#define REACTLIMIT                  (REACTLIMITPH*SQRT3)
#define SQUARE(x) 					((x)*(x)*SQRT2*SQRT2)
#define DEADLIMIT 					0.975						//璋冨埗姣旈檺骞�(姝诲尯)
#define POWER_CONV_RATIO 			((1.0f/1000.0f)/SQRT3)
#define POWER_CONV_RATIO_CAP		(1.0f/1000.0f)
#define	REACTPIUPLIMIT				1.5
#define	REACTPIDNLIMIT				0.7
//#define SAN_SHI_AMPS 				30*SQRT2
///display speed////////
#define FLV_NUM						11
#define FLVSLOT_VOLT  				0
#define FLVSLOT_OUTCURR  			1
#define FLVSLOT_LOADCURR  			2
#define FLVSLOT_GRIDCURR  			3
#define FLVSLOT_CURRREF_D_CORR   	4
#define FLVSLOT_HARMLIMIT_CORR		5
#define FLVSLOT_CAPAREACT_CORR  	6
#define FLVSLOT_CAPAUNBAL_CORR  	7
#define FLVSLOT_HARMZ_LIMIT_CORR  	8
#define FLVSLOT_CAPAUNBAL_Z_CORR  	9
#define FLVSLOT_REFCURR  			10

#define FLVPRV_VOLT  			(0.975f)
#define FLVNEW_VOLT 			(1.0f-FLVPRV_VOLT)
#define FLVPRV_OUTCURR   		(0.975f)
#define FLVNEW_OUTCURR  		(1.0f-FLVPRV_OUTCURR)
#define FLVPRV_LOADCURR   		(0.975f)
#define FLVNEW_LOADCURR  		(1.0f-FLVPRV_LOADCURR)
#define FLVPRV_REFCURR   		(0.975f)
#define FLVNEW_REFCURR  		(1.0f-FLVPRV_REFCURR)
#define FLVPRV_GRIDCURR   		(0.975f)
#define FLVNEW_GRIDCURR  		(1.0f-FLVPRV_GRIDCURR)
#define FLVPRV_CURRREF_D_CORR 	(0.975f)						//CurrRefDCorr
#define FLVNEW_CURRREF_D_CORR  	(1.0f-FLVPRV_CURRREF_D_CORR)
#define FLVPRV_HARMLIMIT_CORR 	(0.975f)						//HarmLimtCorr
#define FLVNEW_HARMLIMIT_CORR  	(1.0f-FLVPRV_HARMLIMIT_CORR)
#define FLVPRV_CAPAREACT_CORR 	(0.975f)						//CapaReactCorr
#define FLVNEW_CAPAREACT_CORR  	(1.0f-FLVPRV_CAPAREACT_CORR)
#define FLVPRV_CAPAUNBAL_CORR 	(0.975f)						//CapaUnbalCorr
#define FLVNEW_CAPAUNBAL_CORR  	(1.0f-FLVPRV_CAPAUNBAL_CORR)
#define FLVPRV_HARMZ_LIMIT_CORR (0.975f)						//HarmZLimtCorr
#define FLVNEW_HARMZ_LIMIT_CORR (1.0f-FLVPRV_HARMZ_LIMIT_CORR)
#define FLVPRV_CAPAREACT_Z_CORR (0.975f)						//CapaUnbalZCorr
#define FLVNEW_CAPAREACT_Z_CORR (1.0f-FLVPRV_CAPAREACT_Z_CORR)
//out:杈撳嚭鍊� x:杈撳叆 slot:妲戒綅
#define FLVFUN(out,x,Slot)	FlvSlot[FLVSLOT_##Slot]*=FLVPRV_##Slot;\
							FlvSlot[FLVSLOT_##Slot]+=x;\
						out=FlvSlot[FLVSLOT_##Slot]*FLVNEW_##Slot;

inline float Max3(float a,float b,float c) {
    return (a>b)?(a>c?a:c):(b>c?b:c);
}


#define MEANPOINT               (CTRLFREQUENCY/POWERGRID_FREQ)
#define MEANPOINT_QUARTER       (MEANPOINT/4)
#define MEANPOINT_QUARTER_NUM   ((MEANPOINT/4)+1)
#define MEANPOINT_QUARTER_CHAN  9
//extern float GcurSlidA[MEANPOINT_QUARTER],GcurSlidB[MEANPOINT_QUARTER],GcurSlidC[MEANPOINT_QUARTER];
//extern float LoadSlidA[MEANPOINT_QUARTER],LoadSlidB[MEANPOINT_QUARTER],LoadSlidC[MEANPOINT_QUARTER];
//extern float IverSlidA[MEANPOINT_QUARTER],IverSlidB[MEANPOINT_QUARTER],IverSlidC[MEANPOINT_QUARTER];

#define EN_MEANNUM_F 0
#if EN_MEANNUM_F
#define MEANNUM     11
#define MEANPOINT           (CTRLFREQUENCY/POWERGRID_FREQ)
#define MEANSTEP    (CTRLSTEP_ALGORITHM)
#define LOAD_FUNDCURR_D     0
#define LOAD_FUNDCURR_Q     1
#define LOAD_FUNDCURR_ND    2
#define LOAD_FUNDCURR_NQ    3
#define INV_FUNDCURR_D      4
#define INV_FUNDCURR_Q      5
#define INV_FUNDCURR_ND     6
#define INV_FUNDCURR_NQ     7
#define GRID_FUNDCURR_D     8
#define GRID_FUNDCURR_Q     9
#define NEUTRAL_LINE_VOLT   10


    #define MEANNUM_F	2
	#define MEANPOINT_F	FUNDPOINT
	#define MEANSTEP_F (CTRLSTEP_ALGORITHM_F)
	#define GRID_PLLVOLT_D  0
	#define GRID_PLLVOLT_Q  1
	#define MEANCALC_F(in,out,chan)\
						MeanSum_F[chan]-=MeanSlid_F[chan][(MeanPos_F)]-(in);\
						MeanSlid_F[chan][(MeanPos_F)]=(in);\
						out=(MEANSTEP_F)*GridFreq*MeanSum_F[chan]
	extern float32 MeanSlid_F[MEANNUM_F][MEANPOINT_F];
	extern float32 MeanSum_F[MEANNUM_F];
#else
#define MEANNUM_F	0
	#define MEANCALC_F(in,out,chan)
#endif

#define DCVOLTDIV2ERR       0
#define GRIDVOLTDA          1
#define GRIDVOLTDB          2
#define GRIDVOLTDC          3
#define GRIDVOLTQA          4
#define GRIDVOLTQB          5
#define GRIDVOLTQC          6

#define LOADREALCURDA       7
#define LOADREALCURQA       8
#define LOADREALCURDB       9
#define LOADREALCURQB       10
#define LOADREALCURDC       11
#define LOADREALCURQC       12

#define GRIDREALCURDA       13
#define GRIDREALCURDB       14
#define GRIDREALCURDC       15
#define GRIDREALCURQA       16
#define GRIDREALCURQB       17
#define GRIDREALCURQC       18

#define IVERREALCURDA       19
#define IVERREALCURDB       20
#define IVERREALCURDC       21
#define IVERREALCURQA       22
#define IVERREALCURQB       23
#define IVERREALCURQC       24




#define CALIBRATION_COEFF_DEFAULTS {\
             {(1), (1.333), (1.148), (0), (0), (0), (0), (0), (0)},\
             {(1), (1.333), (1.148), (0), (0), (0), (0), (0), (0)},\
             {(1), (1.333), (1.148), (0), (0), (0), (0), (0), (0)},\
             {D2RDLY(3,0), D2RDLY(5,0), D2RDLY(7,0), D2RDLY(9,0),D2RDLY(11,0),\
              D2RDLY(13,0),D2RDLY(15,0),D2RDLY(17,0),D2RDLY(19,0)},\
             {D2RDLY(3,0), D2RDLY(5,0), D2RDLY(7,0), D2RDLY(9,0),D2RDLY(11,0),\
              D2RDLY(13,0),D2RDLY(15,0),D2RDLY(17,0),D2RDLY(19,0)},\
             {D2RDLY(3,0), D2RDLY(5,0), D2RDLY(7,0), D2RDLY(9,0),D2RDLY(11,0),\
              D2RDLY(13,0),D2RDLY(15,0),D2RDLY(17,0),D2RDLY(19,0)},\
             {SQUARE(40), SQUARE(100),SQUARE(100),SQUARE(100),SQUARE(70),\
              SQUARE(70), SQUARE(70), SQUARE(40), SQUARE(40)},\
             {SQUARE(10), SQUARE(10), SQUARE(10), SQUARE(10), SQUARE(10),\
              SQUARE(10), SQUARE(10), SQUARE(10), SQUARE(40)},\
             {3,5,7,9,11,13,0,0,0/*9,11,13,15,17,19,21,23*/}}

typedef struct {
    float  MagA[CALIBRATION_COEFF_LEN];     //1.000
    float  MagB[CALIBRATION_COEFF_LEN];     //1.000
    float  MagC[CALIBRATION_COEFF_LEN];     //1.000
    float  PhaA[CALIBRATION_COEFF_LEN];      //radian .In-place operation to
    float  PhaB[CALIBRATION_COEFF_LEN];      //radian .In-place operation to
    float  PhaC[CALIBRATION_COEFF_LEN];      //radian .In-place operation to
    float  limit[CALIBRATION_COEFF_LEN];
    float  start[CALIBRATION_COEFF_LEN];
    unsigned long   harmOrder[CALIBRATION_COEFF_LEN];    //ect...1,3,5,7...
} CALIBRATION_COEFF_STRUCT; //calibration coefficient


extern float FPUcosTable[],FPUsinTable[];
//
//#define pSineLookTabCLA(x)   FPUsinTable[x]
//#define pCoseLookTabCLA(x)   FPUcosTable[x]
#define pSineLookTab(x)  FPUsinTable[x]//*(&FPUsinTable+(x))
#define pCoseLookTab(x)  FPUcosTable[x]//*(&FPUcosTable+(x))

extern RFFT_F32_STRUCT          rfft;
//extern float LimitVal[3][GENERATOR_COEFF_LEN];
//extern unsigned int  LimitDly[3][GENERATOR_COEFF_LEN];
extern float HarmSum;
extern float HarmSumA,HarmSumB,HarmSumC;
//extern float CLACurrHarmRef[3];
extern float CurrHarmRefA,CurrHarmRefB,CurrHarmRefC;
//extern float CurrHarmRefAHold,CurrHarmRefBHold,CurrHarmRefCHold;
extern float ArithVAL,ArithVal;
extern long mdebugen;
extern long cladbg1;
extern int TurnOffPeakFaultFlagA,TurnOffPeakFaultFlagB,TurnOffPeakFaultFlagC;
extern long tempbuff;
extern float CLATestValue;
extern int PWM_ins_indexA,PWM_ins_indexB,PWM_ins_indexC;
extern float PWMcurrDirFlagA,PWMcurrDirFlagB,PWMcurrDirFlagC;
extern int ESCHighLowRelayCNTA,ESCHighLowRelayCNTB,ESCHighLowRelayCNTC;
extern int ESCSicFaultCNTA,ESCSicFaultCNTB,ESCSicFaultCNTC;
extern int ESCBYRelayCNTA,ESCBYRelayCNTB,ESCBYRelayCNTC;

extern float dbg_claWave[6];
extern float dbg_claPwmReg[8];
extern float Esc_PhaseA,Esc_PhaseB,Esc_PhaseC;
extern float VirtulAD_loadCurrentA,VirtulAD_loadCurrentB,VirtulAD_loadCurrentC;
extern long CurrDirA,CurrDirB,CurrDirC;
extern float negCurCompPerc ;
extern float ESC_DutyDataA,ESC_DutyDataB,ESC_DutyDataC;
//extern float outputCurRatio;
extern int32 T1PRmulESC_T1PR;
extern int32 T1PRmulESC_DutyDataA,T1PRmulESC_DutyDataB,T1PRmulESC_DutyDataC;
extern int32 T1PRmulESC_DutyDataAaddnegCurCompPerc,T1PRmulESC_DutyDataBaddnegCurCompPerc,T1PRmulESC_DutyDataCaddnegCurCompPerc;
extern int32 T1PRmulESC_DutyDataAsubnegCurCompPerc,T1PRmulESC_DutyDataBsubnegCurCompPerc,T1PRmulESC_DutyDataCsubnegCurCompPerc;
extern int T1PRPwmFrequency;
extern int ADBufPos;
extern float MU_MultRatio;
extern float gridVoltRatio;
extern float LoadRealCurA,LoadRealCurB,LoadRealCurC,ApfOutCurA,ApfOutCurB,ApfOutCurC;
extern float LoadRealCurAHold,LoadRealCurBHold,LoadRealCurCHold,ApfOutCurAHold,ApfOutCurBHold,ApfOutCurCHold;
//extern float RFFToutBuff[RFFT_SIZE];                    //Output of FFT here if RFFT_STAGES is EVEN
//extern float RFFTF32Coef[RFFT_SIZE];
extern float DQbufIn[MEANPOINT_QUARTER][MEANPOINT_QUARTER_CHAN];
extern CALIBRATION_COEFF_STRUCT CalibCorr;
extern float CLAStepPhaA,CLAStepPhaB,CLAStepPhaC;
extern float FundwaveA,FundwaveB,FundwaveC,CLAwave[6];
extern float TimeStamp,FFTtimeStamp;
extern unsigned long fftUsedChanA,fftUsedChanB,fftUsedChanC;//ifftUsedMagA,ifftUsedMagB,ifftUsedMagC,ifftUsedPhaA,ifftUsedPhaB,ifftUsedPhaC;
extern float32 *mpMagA,*mpMagB,*mpMagC,*mpPhaA,*mpPhaB,*mpPhaC;
extern unsigned long fftNextChan;
extern unsigned long fftUseChan,fftChan;
extern unsigned long RefreshIFFTdata;
#if TESTMODE == 1
extern float DbgStepPhaA,DbgStepPhaB,DbgStepPhaC;
#endif




typedef struct {
	float32  MagA[GENERATOR_COEFF_LEN];		//1.000
	float32  MagB[GENERATOR_COEFF_LEN];		//1.000
	float32  MagC[GENERATOR_COEFF_LEN];		//1.000
	float32  PhaA[GENERATOR_COEFF_LEN];		//radian .In-place operation to
	float32  PhaB[GENERATOR_COEFF_LEN];		//radian .In-place operation to
	float32  PhaC[GENERATOR_COEFF_LEN];		//radian .In-place operation to
	Uint16   harmOrder[GENERATOR_COEFF_LEN];	//ect...1,3,5,7...
} GENERATOR_COEFF_STRUCT;	//calibration coefficient



struct SET_FFT_STORAGE_RATIO_STRUCT{
	int  Mag;		//1.000
	float  Pha;		//radian .In-place operation to
	int  limit;
	int  start;
} ;	//calibration coefficient



#if TEST_VIRTUALSOURCE == 0
#define GENERATOR_COEFF_DEFAULTS {\
{0,	  0,    0,    0,    0,     0,     0,   0,   0},		\
{0,	  0,    0,    0,    0,     0,     0,   0,   0},		\
{0,	  0,    0,    0,    0,     0,     0,   0,   0},		\
{D2R(0  ),D2R(180),D2R(0  ),D2R(180),D2R(0  ),D2R(180),D2R(0  ),D2R(180),D2R(0  )},	\
{D2R(240),D2R(300),D2R(240),D2R(300),D2R(240),D2R(300),D2R(240),D2R(300),D2R(240)},	\
{D2R(120),D2R(60 ),D2R(120),D2R(60 ),D2R(120),D2R(60 ),D2R(120),D2R(60 ),D2R(120)},	\
{3       ,5       ,7       ,11      ,13      ,17      ,19      ,23      ,25/*9,       11,      13,    15,      17,      19,    21,  23*/}}
#else
//{0,	  90,    50,    30,    8,     8,     8,   4,   4},
//{0,	  90,    50,    30,    8,     8,     8,   4,   4},
//{0,	  90,    50,    30,    8,     8,     8,   4,   4},
#define GENERATOR_COEFF_DEFAULTS {\
{0,	  00,    0,    0,    0,     0,     0,   0,   0},		\
{0,	  00,    0,    0,    0,     0,     0,   0,   0},		\
{0,	  00,    0,    0,    0,     0,     0,   0,   0},		\
{D2R(0  ),D2R(180),D2R(0  ),D2R(180),D2R(0  ),D2R(180),D2R(0  ),D2R(180),D2R(0  )},	\
{D2R(240),D2R(300),D2R(240),D2R(300),D2R(240),D2R(300),D2R(240),D2R(300),D2R(240)},	\
{D2R(120),D2R(60 ),D2R(120),D2R(60 ),D2R(120),D2R(60 ),D2R(120),D2R(60 ),D2R(120)},	\
{3       ,5       ,7       ,11      ,13      ,17      ,19      ,23      ,25/*9,       11,      13,    15,      17,      19,    21,  23*/},   0x0000}
#endif

#define TestLoadCurS(chan) (float)(TestLoadCur[(chan)][TestADTempBufp])
#define PRCTRL_COEFF_EN {1,2,3,5,7,    9,11,13,15,17,    19,21,23,25}	//0 indicates that PR are disable ;1 is Fundamental enable;3 is 3 harmonic enable
#define IGBTFAULT_DLYTIME(time_us) (int)((time_us)*1e-6*(2*PWMFREQUENCY))	//first protect time(us)

#define CNT_MS(time_ms) (int)((time_ms)/5)	//
#define CNT_SEC(time_sec) (int)((time_sec))		//
#define TIME_WRITE_15VOLT_REDAY      2000
#define POWERCTRLCNT_15V         600 //浣跨敤璇ュ弬鏁颁竴寮�濮嬫槸涓轰簡闃叉鎺у埗15V涓婄數鎺夌數鐨勪笁鏋佺鍦ㄩ绻佺殑鑴夊啿鍔ㄤ綔涓�,鐑ф瘉;浣嗘槸瀹為檯闂鏄笁鏋佺宸ヤ綔鍦ㄦ斁澶у尯浜�,鐜板凡璋冭瘯濂�,鍙互棰戠箒鍔ㄤ綔.
#define TIME_STANDBY_DELAY       600

//extern float RFFTin1Buff[RFFT_SIZE];
extern float ADBuff[6][ADTEMPBUFLEN];
extern float32 invCurrA,invCurrB,invCurrC;
extern int16 SecondReadLen;
extern Uint16 Dbg_ADTempBufp;
extern Uint16 TestADTempBufp;
extern int16 CLAFFTSrcBufp;
extern Uint16 TestWaveGenEn;
extern int16 PCalcChan;
extern unsigned long FFTCalcChan;
extern float TestLoadCur[3][GEN_FUNDPOINT];



#define ADC_RU_IMA       ( AdcaResultRegs.ADCRESULT0)          //A2  ESC涓荤數鎶楃數缃戠數娴丄
#define ADC_RU_IMB       ( AdccResultRegs.ADCRESULT0)          //C3  ESC涓荤數鎶楃數缃戠數娴丅
#define ADC_RU_IMC       ( AdcdResultRegs.ADCRESULT0)          //D5  ESC涓荤數鎶楃數缃戠數娴丆

#define ADC_RU_IBYA      ( AdcaResultRegs.ADCRESULT5)          //A0  ESC鏃佽矾鐢垫祦A(ISU)
#define ADC_RU_IBYB      ( AdcbResultRegs.ADCRESULT2)          //B2  ESC鏃佽矾鐢垫祦B(ISV)
#define ADC_RU_IBYC      ( AdcdResultRegs.ADCRESULT4)          //D4  ESC鏃佽矾鐢垫祦C(ISW)

#define ADC_DC_UA        ( AdcdResultRegs.ADCRESULT3)          //D3  ESC A鐩哥洿娴佺數瀹圭數鍘婭OU
#define ADC_DC_UB        ( AdcaResultRegs.ADCRESULT1)          //A5  ESC B鐩哥洿娴佺數瀹圭數鍘婭OV
#define ADC_DC_UC        ( AdccResultRegs.ADCRESULT1)          //C4  ESC C鐩哥洿娴佺數瀹圭數鍘婭OW

#define ADC_RU_HVA       ( AdcaResultRegs.ADCRESULT2)          //A15 ESC楂樺帇渚N
#define ADC_RU_LVA       ( AdcdResultRegs.ADCRESULT1)          //D1  ESC浣庡帇渚N

#define ADC_RU_HVB       ( AdcbResultRegs.ADCRESULT1)          //B4  ESC楂樺帇渚N
#define ADC_RU_LVB       ( AdcbResultRegs.ADCRESULT0)          //B5  ESC浣庡帇渚N

#define ADC_RU_HVC       ( AdccResultRegs.ADCRESULT3)          //C2  ESC楂樺帇渚N
#define ADC_RU_LVC       ( AdcaResultRegs.ADCRESULT4)          //A4  ESC浣庡帇渚N

#define ADC_RU_TEMP0     ( AdcbResultRegs.ADCRESULT3)          //B1 ESC 娴嬭瘯娓╁害T1
#define ADC_RU_TEMP1     ( AdcdResultRegs.ADCRESULT2)          //D2 ESC 娴嬭瘯娓╁害T2
#define ADC_RU_TEMP2     ( AdccResultRegs.ADCRESULT2)          //C5 ESC 娴嬭瘯娓╁害T3


//#define IO_OUTPUT(IO,VAL) (IO)=(VAL);asm(" RPT #7 || NOP") ;asm(" RPT #7 || NOP")
//#define IO_STATE(IO,VAL) (IO)!=(VAL)?1:0
//
//#if DEBUG_WAVE_INNER
//	#define DBG_IOOUT(IO,VAL)  (IO)=(VAL)
//#else
//	#define DBG_IOOUT(IO,VAL)
//#endif


inline float CLAPhaseLimit(float x){
//    int tmp=CLAdiv(x,PI2);  //鍙栨暣
    return(x-PI2*(int)CLAdiv(x,PI2));
}

inline float PhaseLimit(float x){
    return(x-PI2*(int)(x/PI2)); //鍙栨暣,FS$$DIV,娉ㄦ剰x蹇呴』澶т簬0
}

inline Uint16 PhaseLimitI(Uint16 x){
	x%=PI2_SINE_LOOKTABLE;
	return x;
}


inline float32 IntergrateBackEuler(float32 u,float32 ynz1)     //绉垎鍑芥暟
{
	return ynz1+u*CTRLSTEP_ALGORITHM_F;
}
#define RATIO_UPLOAD1000000 1000000.0f+0.0000005f
#define RATIO_DNLOAD1000000 0.000001f
#define RATIO_UPLOAD100000 100000.0f+0.000005f
#define RATIO_DNLOAD100000 0.00001f
#define RATIO_UPLOAD10000 10000.0f+0.00005f
#define RATIO_DNLOAD10000 0.0001f
#define RATIO_UPLOAD1000 1000.0f+0.0005f
#define RATIO_DNLOAD1000 0.001f
#define RATIO_UPLOAD100 100.0f+0.005f
#define RATIO_DNLOAD100 0.01f
#define RATIO_UPLOAD10 10.0f+0.05f
#define RATIO_DNLOAD10 0.1f
#define RATIO_UPLOAD1 1.0f+0.5f
#define RATIO_DNLOAD1 1.0f
#define RATIO_HALVE 0.5f
#define RATIO_UPLOAD01 0.1f+0.05f
#define RATIO_DNLOAD01 10f
#define RATIO_UPLOAD001 0.01f+0.005f
#define RATIO_DNLOAD001 100f

extern float32 CtrlVoltA,CtrlVoltB,CtrlVoltC;
extern float32 CurFundA,CurFundB,CurFundC,CurFundStandbyA,CurFundStandbyB,CurFundStandbyC;
extern float32 VolFundA,VolFundA1,VolFundA2,VolFundB,VolFundB1,VolFundB2,VolFundC,VolFundC1,VolFundC2;
extern float32 CurrHarmRefA,CurrHarmRefB,CurrHarmRefC,FundwaveA,FundwaveB,FundwaveC,CLAwave[6];
extern float32 CurFundNgA,CurFundNgB,CurFundNgC,CurFundNg;
extern float32 CurrRefA,CurrRefB,CurrRefC;
extern float32 CurrRefStandbyA,CurrRefStandbyB,CurrRefStandbyC;
extern float32 UdcBalanceCurr;
extern float32 AutoCurrSharing;

extern float32 currRef_D;
extern float32 CtrlVoltUV;

//extern CALIBRATION_COEFF_STRUCT CalibCorr;
extern GENERATOR_COEFF_STRUCT GenCorr;
//extern struct SET_FFT_COEFF_STRUCT SetFFTCoeff;
//extern struct SET_FFT_COEFF_STRUCT GenFFTCoeff;
extern const GENERATOR_COEFF_STRUCT GenCorrPrv[GENERATOR_COEFF_LEN];
//extern const CALIBRATION_COEFF_STRUCT CalibCorrPrv;
extern float IFFTData[(3+1)*2][CALIBRATION_COEFF_LEN+1];
extern float InvFFTData[3*2][CALIBRATION_COEFF_LEN+1];
extern Uint16  LimitDly[(3+1)][CALIBRATION_COEFF_LEN];
extern float32 LimitVal[(3+1)][CALIBRATION_COEFF_LEN];
extern float32 FlvSlot[FLV_NUM];
extern float32 HarmSumA,HarmSumB,HarmSumC;
extern Uint16  FFTSrcBufp;

//extern float VoltSlidA[VOLT_FUNDPOINT],VoltSlidB[VOLT_FUNDPOINT],VoltSlidC[VOLT_FUNDPOINT];
extern float VoltSlid[VOLT_FUNDPOINT][3];
extern float VoltSlidA[MEANPOINTHALF],VoltSlidB[MEANPOINTHALF],VoltSlidC[MEANPOINTHALF];
//extern float VoltSlidU[MEANPOINTHALF],VoltSlidV[MEANPOINTHALF],VoltSlidW[MEANPOINTHALF];
extern float GridCurrSlidA[MEANPOINTHALF],GridCurrSlidB[MEANPOINTHALF],GridCurrSlidC[MEANPOINTHALF];
extern float GridRealCurDA2,GridRealCurDB2,GridRealCurDC2,GridRealCurQA2,GridRealCurQB2,GridRealCurQC2;
extern float GridCurrAF_Beta,GridCurrBF_Beta,GridCurrCF_Beta;
extern float GridRealCurErrA,GridRealCurErrB,GridRealCurErrC;
void FFTDataReduction(Uint16 chan);
void FFTDataReduction2(Uint16 harm,Uint16 chan);
void TestFFT(void);
void waveRecord(void);
void TestWaveGenerator(void);
void DMACH1ConfigAndRun(float32 *src,int16 ADPos);
void SecondDMACH1ConfigAndRun(void);
void FFTInit(void);
void DMAInitialize(void);
void profileDisp(void);
void ADBufftoFFT(void);
void HarmCurGnManul(void);
void InitCtrlParam(int mode);
void HarmonicLoading(void);
void HarmonicGeneration(void);
void CorrectingAD(void);
void CorrectingCT(void);
void DirectionCT(void);
void JudgePowerGridCT(void);
void JudgeLoadCT(void);
void AngleJudgePowerGrid(int N1,int N2,int N3,Uint16 number);
void AngleJudgeLoad(int N1,int N2,int N3,Uint16 number);
void AngleJudgeInductanceLoad(int N1,int N2,int N3,Uint16 number);
void AngleJudgeCapacitanceLoad(int N1,int N2,int N3,Uint16 number);
void InitMainCtrlParam(void);
void UserParamRefresh(void);
void FactoryParamRefresh(void);
void HarmnoicParamRefresh(void);
//void HarmnoicCorrection(void);
void SetHarmnoicParam_harmOrder(int16 rx34);
void SetHarmnoicParam_MagPha(int16 rx34,int16 chan);
void SetHarmnoicParam_startLimit(int16 rx34,int16 chan);
void SetHarmnoicParam_thisGroup(int16 rx34);

float MeanInit(Uint16 chan);
float CurrLimitZPri(float in);
float CurrLimitPri(float in);
float CurrLimitPriPH(float in);
void PRController(void);
void AdjustFulkPRcoeff(void);
extern float32 StepPhaA,StepPhaB,StepPhaC;


typedef struct {
    long fftRefreshChan;
    float *pDstMag;
    float *pDstPha;
    float *pMagA;
    float *pPhaA;
    float *pMagB;
    float *pPhaB;
    float *pMagC;
    float *pPhaC;
}struFFTData;
extern struFFTData FFTData;

#endif /* CORECTRL_H_ */
