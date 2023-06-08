#include "DSP28x_Project.h"
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "VariblesRef.h"
#include "MarcoDefine.h"
#include "mb_m.h"
#include "Modbus.h"
#include <ti/sysbios/knl/Semaphore.h>

void CapacitorSwithingCtr(void);
void CapacitorSwithingExc(void);
void CapSwitchAutoDivCtrl(void);
void CapSwitchHandCtrl(void);
void CapSwitchAutoCtrl(void);
void CapEmergStop(void);
void AutoStateReset(void);
//void CapComAction(void);
void CapComActionDown(void);
void CapDisActionDown(void);
void CapDisperActOF(Uint16 COM,Uint16 DisA,Uint16 DisB,Uint16 DisC);
void CapDisperActON(Uint16 COM,Uint16 DisA,Uint16 DisB,Uint16 DisC);
Uint16 ThrothFlag = 0xFF;
extern Event_Handle Event_CapSwitchSM;

/**************************************电容投切*******************************************/
void CapSwitchStatePoll(void)
{
//    Uint16 i;
    UInt events;
    static Uint32 CapPoll=0;
    while(1){
        // Wait for ANY of the ISR events to be posted *
        events = Event_pend(Event_CapSwitchSM, Event_Id_NONE,Event_Id_02 ,BIOS_WAIT_FOREVER);
        CapPoll++;
        if((!CapGroupAct.capgroupswitch.B.CapVarUnseal)&&(APF_ID==0x10)){
            CapEmergStop();
            switch(CapEventFlag){
                case STATE_EV_WRCTRL:
                    if(CapGroupAct.capgroupswitch.B.CapCompensation==0)
                        CapSwitchHandCtrl();
                    else if(CapGroupAct.capgroupswitch.B.CapCompensation==1){
                        if(CapGroupAct.capgroupswitch.B.CapVarConditon){
                            if(CapDisperNum){
                                CapSwitchAutoDivCtrl();
                            }else{
                                CapSwitchAutoCtrl();
                            }
                        }else{
                            if( (StateEventFlag_A == STATE_EVENT_RUN_A)|| \
                                (StateEventFlag_B == STATE_EVENT_RUN_B)|| \
                                (StateEventFlag_C == STATE_EVENT_RUN_C) )
                            {
                                if(CapDisperNum){
                                    CapSwitchAutoDivCtrl();
                                }else{
                                    CapSwitchAutoCtrl();
                                }
                            }
                        }
                    }
                    CapEventFlag = STATE_EV_WRVER;
				StateFlag.CapStateFlag = CapGroupAct.capgroupswitch.B.CapCompensation;
                break;
                case STATE_EV_FAULT:
//				if((CapGroupAct.CapComuFaultFlag==1)&&(softwareFaultWord3.B.capcomuwarning==0)){
//				    SoeRecData(131);
//				    softwareFaultWord3.B.capcomuwarning = 1;
//				}
//                if((CapGroupAct.CapComuFaultFlag==0)&&(softwareFaultWord3.B.capcomuwarning==1)){
//                    SoeRecData(130);
//                    softwareFaultWord3.B.capcomuwarning = 0;
//                }
//                    if(!CapGroupAct.capgroupswitch.B.VolHarmonicLimit){
//                        if((HarmTHD[TotalHarmDistorionVoltA].THD>CapGroupAct.HmiVolProt)||\
//                           (HarmTHD[TotalHarmDistorionVoltB].THD>CapGroupAct.HmiVolProt)||\
//                           (HarmTHD[TotalHarmDistorionVoltC].THD>CapGroupAct.HmiVolProt)){
//                        if(softwareFaultWord2.B.overVoltHarm==0){
//                            SoeRecData(132);
//                            softwareFaultWord2.B.overVoltHarm = 1;
//                            for(i=0;i<20;i++)   CapGroupAct.PhactGroup[i]=0x10;
//                            AutoStateReset();
//                        }
//                    }
//                        if((HarmTHD[TotalHarmDistorionVoltA].THD<CapGroupAct.HmiVolProt)&&\
//                           (HarmTHD[TotalHarmDistorionVoltB].THD<CapGroupAct.HmiVolProt)&&\
//                           (HarmTHD[TotalHarmDistorionVoltC].THD<CapGroupAct.HmiVolProt)){
//                            softwareFaultWord2.B.overVoltHarm = 0;
//                        }
//                    }else{
//                        softwareFaultWord2.B.overVoltHarm = 0;
//                    }
//                    for(i=0;i<18;i++){
//                        if(CapGroupAct.PhactGroup[i]==0x17){
//                            CapGroupAct.PhactGroup[18]=0x17;
//                            CapGroupAct.PhactGroup[19]=0x17;
//                            break;
//                        }else{
//                            CapGroupAct.PhactGroup[18]=0x10;
//                            CapGroupAct.PhactGroup[19]=0x10;
//                        }
//                    }
//                    CapEventFlag = STATE_EV_WRCTRL;
                break;
                case STATE_EV_WRVER:
                    CapComActionDown();
                    if(CapDisperNum != 0)
                        CapDisActionDown();
                    CapEventFlag = STATE_EV_FAULT;
                break;
            }
        }
    }
}
void CapEmergStop(void){
	if(StateFlag.CapEmergShutFlag == TRUE){
		CapEventFlag = STATE_EV_WRCTRL;
		CapGroupAct.capgroupswitch.B.CapCompensation = 0;
	}
}

void CapSwitchHandCtrl(){			//手动投切
	Uint16 i,MusData;
	float32 allCapCapacityPhA=AllCapCapacityPhA,allCapCapacityPhB=AllCapCapacityPhB,allCapCapacityPhC=AllCapCapacityPhC;
	if(CapFlagDown < 0x18){
		MusData = CapGroupAct.PhactGroup[CapFlagDown];
		if(MusData==0x17){
			if(CapFlagDown<CapCompenNum){
				allCapCapacityPhA += CapGroupAct.Capacity[CapFlagDown]*CapyRadio*0.3333;
				allCapCapacityPhB += CapGroupAct.Capacity[CapFlagDown]*CapyRadio*0.3333;
				allCapCapacityPhC += CapGroupAct.Capacity[CapFlagDown]*CapyRadio*0.3333;
			}else{
				switch((CapFlagDown-CapCompenNum)%3){
				 case 0:allCapCapacityPhA += CapGroupAct.Capacity[CapFlagDown]*CapyRadio;break;
				 case 1:allCapCapacityPhB += CapGroupAct.Capacity[CapFlagDown]*CapyRadio;break;
				 case 2:allCapCapacityPhC += CapGroupAct.Capacity[CapFlagDown]*CapyRadio;break;
				}
			}
		}
		if(MusData==0x10){
			if(CapFlagDown<CapCompenNum){
				allCapCapacityPhA -= CapGroupAct.Capacity[CapFlagDown]*CapyRadio*0.3333;
				allCapCapacityPhB -= CapGroupAct.Capacity[CapFlagDown]*CapyRadio*0.3333;
				allCapCapacityPhC -= CapGroupAct.Capacity[CapFlagDown]*CapyRadio*0.3333;
			}else{
				switch((CapFlagDown-CapCompenNum)%3){
				 case 0:allCapCapacityPhA -= CapGroupAct.Capacity[CapFlagDown]*CapyRadio;break;
				 case 1:allCapCapacityPhB -= CapGroupAct.Capacity[CapFlagDown]*CapyRadio;break;
				 case 2:allCapCapacityPhC -= CapGroupAct.Capacity[CapFlagDown]*CapyRadio;break;
				}
			}
		}
		if(MusData==0x1F)		StateFlag.CapEmergShutFlag = TRUE;
		if(allCapCapacityPhA<0)	allCapCapacityPhA = 0;
		if(allCapCapacityPhB<0)	allCapCapacityPhB = 0;
		if(allCapCapacityPhC<0)	allCapCapacityPhC = 0;
	}
	if(StateFlag.CapEmergShutFlag==TRUE){
		StateFlag.CapEmergShutFlag = FALSE;
		CapEmergTim = 0;
		for(i=0;i<24;i++)	CapGroupAct.PhactGroup[i]=0x10;
		allCapCapacityPhA=allCapCapacityPhB=allCapCapacityPhC=0;
	}
    AllCapCapacityPhA=allCapCapacityPhA;
    AllCapCapacityPhB=allCapCapacityPhB;
    AllCapCapacityPhC=allCapCapacityPhC;

	AutoStateReset();
}
void AutoStateReset(void)
{
	ThrothFlag = 0xFF;CapFlagDown = 0xFF;
	CapCount = 0;CapCountA=0;CapCountB=0;CapCountC=0;
	AutoSwitchOnCount=0;AutoSwitchOffCount=0;
	TimSwitchOnStart = 0;TimSwitchOffStart = 0;
	SwitchTimeOn = 0;SwitchTimeOff = 0;
	DisperACount=DisperAOffCount=(CapCompenNum);
	DisperBCount=DisperBOffCount=(CapCompenNum+1);
	DisperCCount=DisperCOffCount=(CapCompenNum+2);
}

inline void CapSwitchAutoDetcCap(void){
	if((-APFReactPowerOut)>CapGroupAct.Capacity[AutoSwitchOnCount]*CapSwitchVarUp){		//判断是否切换
		TimSwitchOnStart = 1;
		SwitchTimeOff = 0;
		TimSwitchOffStart = 0;
		AutoSwitchOff = 0;
	}
	if(APFReactPowerOut>2){
		TimSwitchOffStart = 1;						//切掉一个电容
		SwitchTimeOn = 0;
		TimSwitchOnStart = 0;
		AutoSwitchOn = 0;
	}
	if(SwitchTimeOn>CapGroupAct.TimSwitchOn*TmOnRadio){
		SwitchTimeOn = 0;
		if((-APFReactPowerOut)>CapGroupAct.Capacity[AutoSwitchOnCount]*CapSwitchVarUp){
			AutoSwitchOn = 1;//投
		}else{
			TimSwitchOnStart = 0;
		}
	}else if(SwitchTimeOff>CapGroupAct.TimSwitchOff*TmOfRadio){
			  SwitchTimeOff = 0;
			  if(APFReactPowerOut>2){
				 AutoSwitchOff = 1;
			  }else{
				 TimSwitchOffStart = 0;
			  }
	}
}
void CapSwitchAutoCtrl(void){
	//自动投切判断
	if(CapGroupAct.capgroupswitch.B.CapVarConditon){
			CapSwitchAutoDetcCap();
	}else{
        if( (StateEventFlag_A == STATE_EVENT_RUN_A)|| \
            (StateEventFlag_B == STATE_EVENT_RUN_B)|| \
            (StateEventFlag_C == STATE_EVENT_RUN_C) )
		{
				CapSwitchAutoDetcCap();
		}
	}
//	自动投切执行
	if(AutoSwitchOn==1){      //投
		if(CapCount < CapCompenNum){
			ThrothFlag = 1;
			CapGroupAct.PhactGroup[AutoSwitchOnCount] = 0x17;
		}
		AutoSwitchOn = 0;
	}
	if(AutoSwitchOff==1){	  //切
		if(CapCount > 0){
			ThrothFlag = 0;
			CapGroupAct.PhactGroup[AutoSwitchOffCount] = 0x10;
		}
		AutoSwitchOff = 0;
	}
}

void CapComActionDown(void)//共补确认
{
    float32 allCapCapacityPhA=AllCapCapacityPhA,allCapCapacityPhB=AllCapCapacityPhB,allCapCapacityPhC=AllCapCapacityPhC;
	if((CapGroupAct.CapStateUp[AutoSwitchOnCount]==0x07)&&(ThrothFlag==1)){	//确认已经投上
		allCapCapacityPhA += CapGroupAct.Capacity[AutoSwitchOnCount]*CapyRadio*0.3333;
		allCapCapacityPhB += CapGroupAct.Capacity[AutoSwitchOnCount]*CapyRadio*0.3333;
		allCapCapacityPhC += CapGroupAct.Capacity[AutoSwitchOnCount]*CapyRadio*0.3333;
		AutoSwitchOnCount++;CapCount++;
		if(AutoSwitchOnCount >= CapCompenNum)	AutoSwitchOnCount = 0;
		ThrothFlag = 0xFF;
	}
	if((CapGroupAct.CapStateUp[AutoSwitchOffCount]==0x0)&&(ThrothFlag==0)){	//确认已经切掉切
		allCapCapacityPhA -= CapGroupAct.Capacity[AutoSwitchOffCount]*CapyRadio*0.3333;
		allCapCapacityPhB -= CapGroupAct.Capacity[AutoSwitchOffCount]*CapyRadio*0.3333;
		allCapCapacityPhC -= CapGroupAct.Capacity[AutoSwitchOffCount]*CapyRadio*0.3333;
		AutoSwitchOffCount++;CapCount--;
		if(AutoSwitchOffCount >= CapCompenNum)	AutoSwitchOffCount = 0;
		if(allCapCapacityPhA<0)	allCapCapacityPhA = 0;
		if(allCapCapacityPhB<0)	allCapCapacityPhB = 0;
		if(allCapCapacityPhC<0)	allCapCapacityPhC = 0;
		ThrothFlag = 0xFF;
	}
    allCapCapacityPhA=AllCapCapacityPhA;
    allCapCapacityPhB=AllCapCapacityPhB;
    allCapCapacityPhC=AllCapCapacityPhC;
}

void CapSwitchAutoDivCtrl(void)
{
	/***********************************************************************************************
		电容混合补偿控制策略:
		共补电容判断:取三相无功缺口最小的作为三相共补的判断
		优先使用共补电容:优先投共补,优先切分补
		分补判断:每一相的无功缺口作为每一相电容的补偿目标值,电容补偿之后由SVG的三相不平衡来补偿
		注意事项:
		混合补偿由每一组电容容量来控制
		必须下发24组电容的容量值
		为了防止 投切 震荡 , 下发的容量应大于实际产生的容量  此外程序上做了	CapSwitchVarUp*100 倍的扩限
		去掉了分补电容和共补电容之间的切换 原因:
		当分补和共补同时存在或者每路电容容量不同的时候  这种替换策略有上万种
	************************************************************************************************/
	Uint16 CapCompenFind=0;Uint16 CapDisperAFind=0;Uint16 CapDisperBFind=0;Uint16 CapDisperCFind=0;
	float32 APFReactPowerOutON=0;float32 APFReactPowerOutOF=0;
	if((-APFReactPowerOutA>0.7)&&(-APFReactPowerOutB>0.7)&&(-APFReactPowerOutC>0.7)){//三相全需要补容性,判断共补是否符合要求
		APFReactPowerOutON = Max(APFReactPowerOutA,APFReactPowerOutB,APFReactPowerOutC)*3;
	}
	if((APFReactPowerOutA>0.7)&&(APFReactPowerOutB>0.7)&&(APFReactPowerOutC>0.7)){//三相全需要补感性,判断共补是否可以切除
		APFReactPowerOutOF = Max(APFReactPowerOutA,APFReactPowerOutB,APFReactPowerOutC)*3;
	}
	/**************************************************投切判断***************************************/
	if(((-APFReactPowerOutON)>CapGroupAct.Capacity[AutoSwitchOnCount]*CapSwitchVarUp)&&(CapCount<CapCompenNum)){//投判断
		TimSwitchOnStart = 1;
		SwitchTimeOff = 0;
		TimSwitchOffStart = 0;
		AutoSwitchOff = 0;
		CapCompenFind = 1;
	}else if(((-APFReactPowerOutA)>CapGroupAct.Capacity[DisperACount]*CapSwitchVarUp)&&(CapCountA<CapDisperNum)){
		TimSwitchOnStart = 1;
		SwitchTimeOff = 0;
		TimSwitchOffStart = 0;
		AutoSwitchOff = 0;
		CapDisperAFind = 1;
	}else if(((-APFReactPowerOutB)>CapGroupAct.Capacity[DisperBCount]*CapSwitchVarUp)&&(CapCountB<CapDisperNum)){
		TimSwitchOnStart = 1;
		SwitchTimeOff = 0;
		TimSwitchOffStart = 0;
		AutoSwitchOff = 0;
		CapDisperBFind = 1;
	}else if(((-APFReactPowerOutC)>CapGroupAct.Capacity[DisperCCount]*CapSwitchVarUp)&&(CapCountC<CapDisperNum)){
		TimSwitchOnStart = 1;
		SwitchTimeOff = 0;
		TimSwitchOffStart = 0;
		AutoSwitchOff = 0;
		CapDisperCFind = 1;
	}else{
		TimSwitchOnStart = 0;
		SwitchTimeOn = 0;
	}

	if((APFReactPowerOutA>0.7)&&(CapCountA>0)){//切判断
		TimSwitchOffStart = 1;
		SwitchTimeOn = 0;
		TimSwitchOnStart = 0;
		AutoSwitchOn = 0;
		CapDisperAFind = 1;
	}else if((APFReactPowerOutB>0.7)&&(CapCountB>0)){
		TimSwitchOffStart = 1;
		SwitchTimeOn = 0;
		TimSwitchOnStart = 0;
		AutoSwitchOn = 0;
		CapDisperBFind = 1;
	}else if((APFReactPowerOutC>0.7)&&(CapCountC>0)){
		TimSwitchOffStart = 1;
		SwitchTimeOn = 0;
		TimSwitchOnStart = 0;
		AutoSwitchOn = 0;
		CapDisperCFind = 1;
	}else if((APFReactPowerOutOF>2.1)&&(CapCount>0)){
		TimSwitchOffStart = 1;
		SwitchTimeOn = 0;
		TimSwitchOnStart = 0;
		AutoSwitchOn = 0;
		CapCompenFind = 1;
	}else{
		TimSwitchOffStart = 0;
		SwitchTimeOff = 0;
	}
/******************滞回判断 防止电网波动导致误投***********************************************/
///投滞回
	if(SwitchTimeOn>CapGroupAct.TimSwitchOn*TmOnRadio){
		SwitchTimeOn = 0;
		if(CapCompenFind == 1){
			if(((-APFReactPowerOutON)>CapGroupAct.Capacity[AutoSwitchOnCount]*CapSwitchVarUp)&&(CapCount<CapCompenNum)){
				AutoSwitchOn = 1;//投
			}else{
				TimSwitchOnStart = 0;
			}
		}
		if(CapDisperAFind == 1){
			if(((-APFReactPowerOutA)>CapGroupAct.Capacity[DisperACount]*CapSwitchVarUp)&&(CapCountA<CapDisperNum)){
				AutoSwitchOn = 1;//投
			}else{
				TimSwitchOnStart = 0;
			}
		}
		if(CapDisperBFind == 1){
			if(((-APFReactPowerOutB)>CapGroupAct.Capacity[DisperBCount]*CapSwitchVarUp)&&(CapCountB<CapDisperNum)){
				AutoSwitchOn = 1;//投
			}else{
				TimSwitchOnStart = 0;
			}
		}
		if(CapDisperCFind == 1){
			if(((-APFReactPowerOutC)>CapGroupAct.Capacity[DisperCCount]*CapSwitchVarUp)&&(CapCountC<CapDisperNum)){
				AutoSwitchOn = 1;//投
			}else{
				TimSwitchOnStart = 0;
			}
		}
	}
///切滞回
	if(SwitchTimeOff>CapGroupAct.TimSwitchOff*TmOnRadio){
		SwitchTimeOff = 0;
		if(CapCompenFind == 1){
			if((APFReactPowerOutOF>2.1)&&(CapCount>0)){
				AutoSwitchOff = 1;
			}else{
				TimSwitchOffStart = 0;
			}
		}
		if(CapDisperAFind == 1){
			if((APFReactPowerOutA>0.7)&&(CapCountA>0)){
				AutoSwitchOff = 1;
			}else{
				TimSwitchOffStart = 0;
			}
		}
		if(CapDisperBFind == 1){
			if((APFReactPowerOutB>0.7)&&(CapCountB>0)){
				AutoSwitchOff = 1;
			}else{
				TimSwitchOffStart = 0;
			}
		}
		if(CapDisperCFind == 1){
			if((APFReactPowerOutC>0.7)&&(CapCountC>0)){
				AutoSwitchOff = 1;
			}else{
				TimSwitchOffStart = 0;
			}
		}
	}
/********************************************投切执行****************************************/
	CapDisperActON(CapCompenFind,CapDisperAFind,CapDisperBFind,CapDisperCFind);
	CapDisperActOF(CapCompenFind,CapDisperAFind,CapDisperBFind,CapDisperCFind);
}

void CapDisperActOF(Uint16 COM,Uint16 DisA,Uint16 DisB,Uint16 DisC)
{
	if(AutoSwitchOff==1){
		if(COM==1){
			if(CapCount>0){
				ThrothFlag = 0;
				CapGroupAct.PhactGroup[AutoSwitchOffCount] = 0x10;
			}else{
				ThrothFlag = 0xFF;
			}
		}
		if(DisA==1){
			if(CapCountA>0){
				ThrothFlag = 5;
				CapGroupAct.PhactGroup[DisperAOffCount] = 0x10;
			}else{
				ThrothFlag = 0xFF;
			}
		}
		if(DisB==1){
			if(CapCountB>0){
				ThrothFlag = 6;
				CapGroupAct.PhactGroup[DisperBOffCount] = 0x10;
			}else{
				ThrothFlag = 0xFF;
			}
		}
		if(DisC==1){
			if(CapCountC>0){
				ThrothFlag = 7;
				CapGroupAct.PhactGroup[DisperCOffCount] = 0x10;
			}else{
				ThrothFlag = 0xFF;
			}
		}
		AutoSwitchOff =0;
	}
}

void CapDisperActON(Uint16 COM,Uint16 DisA,Uint16 DisB,Uint16 DisC)
{
	if(AutoSwitchOn==1){
		if(COM==1){
			if(CapCount < CapCompenNum){
				ThrothFlag = 1;
				CapGroupAct.PhactGroup[AutoSwitchOnCount] = 0x17;
			}else{
				ThrothFlag = 0xFF;
			}
		}
		if(DisA==1){
			if(CapCountA < CapDisperNum){
				ThrothFlag = 2;
				CapGroupAct.PhactGroup[DisperACount] = 0x17;
			}else{
				ThrothFlag = 0xFF;
			}
		}
		if(DisB==1){
			if(CapCountB < CapDisperNum){
				ThrothFlag = 3;
				CapGroupAct.PhactGroup[DisperBCount] = 0x17;
			}else{
				ThrothFlag = 0xFF;
			}
		}
		if(DisC==1){
			if(CapCountC < CapDisperNum){
				ThrothFlag = 4;
				CapGroupAct.PhactGroup[DisperCCount] = 0x17;
			}else{
				ThrothFlag = 0xFF;
			}
		}
		AutoSwitchOn =0;
	}
}

void CapDisActionDown(void)//分补确认
{
    float32 allCapCapacityPhA=AllCapCapacityPhA,allCapCapacityPhB=AllCapCapacityPhB,allCapCapacityPhC=AllCapCapacityPhC;

	if((CapGroupAct.CapStateUp[DisperACount]==0x07)&&(ThrothFlag==2)){	//A相确认已经投上
		allCapCapacityPhA += CapGroupAct.Capacity[DisperACount]*CapyRadio;
		DisperACount = DisperACount + 3;
		AutoSwitchOn = 0;
		CapCountA++;
		ThrothFlag = 0xFF;
		if(DisperACount > (CapCompenNum+(CapDisperNum-1)*3))	DisperACount = CapCompenNum;
	}
	if((CapGroupAct.CapStateUp[DisperAOffCount]==0x00)&&(ThrothFlag==5)){	//A相确认已经切掉
		allCapCapacityPhA -= CapGroupAct.Capacity[DisperAOffCount]*CapyRadio;
		DisperAOffCount = DisperAOffCount + 3;
		AutoSwitchOff = 0;
		CapCountA--;
		ThrothFlag = 0xFF;
		if(DisperAOffCount > (CapCompenNum+(CapDisperNum-1)*3))	DisperAOffCount = CapCompenNum;
	}
	if((CapGroupAct.CapStateUp[DisperBCount]==0x07)&&(ThrothFlag==3)){	//B相确认已经投上
		allCapCapacityPhB += CapGroupAct.Capacity[DisperBCount]*CapyRadio;
		DisperBCount = DisperBCount + 3;
		AutoSwitchOn = 0;
		CapCountB++;
		ThrothFlag = 0xFF;
		if(DisperBCount > (CapCompenNum+1+(CapDisperNum-1)*3))	DisperBCount = CapCompenNum+1;
	}
	if((CapGroupAct.CapStateUp[DisperBOffCount]==0x00)&&(ThrothFlag==6)){	//B相确认已经切掉
		allCapCapacityPhB -= CapGroupAct.Capacity[DisperBOffCount]*CapyRadio;
		DisperBOffCount = DisperBOffCount + 3;
		AutoSwitchOff = 0;
		CapCountB--;
		ThrothFlag = 0xFF;
		if(DisperBOffCount > (CapCompenNum+1+(CapDisperNum-1)*3))	DisperBOffCount = CapCompenNum+1;
	}
	if((CapGroupAct.CapStateUp[DisperCCount]==0x07)&&(ThrothFlag==4)){	//C相确认已经投上
		allCapCapacityPhC += CapGroupAct.Capacity[DisperCCount]*CapyRadio;
		DisperCCount = DisperCCount + 3;
		AutoSwitchOn = 0;
		CapCountC++;
		ThrothFlag = 0xFF;
		if(DisperCCount > (CapCompenNum+2+(CapDisperNum-1)*3))	DisperCCount = CapCompenNum+2;
	}
	if((CapGroupAct.CapStateUp[DisperCOffCount]==0x00)&&(ThrothFlag==7)){	//C相确认已经切掉
		allCapCapacityPhC -= CapGroupAct.Capacity[DisperCOffCount]*CapyRadio;
		DisperCOffCount = DisperCOffCount + 3;
		AutoSwitchOff = 0;
		CapCountC--;
		ThrothFlag = 0xFF;
		if(DisperCOffCount > (CapCompenNum+2+(CapDisperNum-1)*3))	DisperCOffCount = CapCompenNum+2;
	}
    AllCapCapacityPhA=allCapCapacityPhA;
    AllCapCapacityPhB=allCapCapacityPhB;
    AllCapCapacityPhC=allCapCapacityPhC;
}

/*
 * 功能：电容投切？？WY
 */
void InitCapGroupAct(void)
{
	Uint16 *tmp, i;

	tmp = (Uint16*) &CapGroupAct;

	for (i = 0; i < sizeof(CapGroupAct); i++)
	{
		*tmp++ = 0;
	}

	for (i = 0; i < 24; i++)
	{
		CapGroupAct.PhactGroup[i] = 0x10;
	}
	CapFlagDown = 0xFF;
}

#if FIXED_CAP_EN == 1

//电容投切参数
//volatile float CSReactPower = 0,CSReactPowerave = 0;
int CSRPIncount = 0,CSRPcount = 0,CSRPIF = 0;
int CSRPswitchC = 20,CSRPswitchT = -15;

void CapacitorSwithingCtr(void)//电容切换控制
{
	static int CapaSwithIF=0;
	if(CntSec.CapaSwithCount>=7){
		CapaSwithIF=1;
//		CntSec.CapaSwithCount=0;
	}

	if(  (EnCapSwitch.B.mode) ){
		if(CapaSwithIF){
			if(CSReactPowerave > CSRPswitchC){										//判断是否切换
				CSRPIF = 1;															//投一个电容
			}else if(CSReactPowerave < CapRunSwitchBack){
				CSRPIF = 2;															//切掉一个电容
			}else
				CSRPIF = 0;
	}
	}else if(!EnCapSwitch.B.mode){
		if(StateEventFlag == STATE_EVENT_RUN){//运行时和不运行时切换条件不一样
			if(CapaSwithIF){
				if(currentRefQ*-0.4 > CSRPswitchC){										//判断MEC容性无功功率>电容额定容量.
					CSRPIF = 1;															//强制投入一组电容,让MEC自动降低容性无功
				}else {
					if(CSReactPowerave > CSRPswitchC){									//判断是否切换
			CSRPIF = 1;//投一个电容
					}else if(CSReactPowerave < CapRunSwitchBack){	//
			CSRPIF = 2;//切掉一个电容
					}else
						CSRPIF = 0;
				}
			}
		}else{
			if(CSReactPowerave < CapRunSwitchBack){	//
				CSRPIF = 2;														//切掉一个电容
			}else
				CSRPIF = 0;
		}
	}
	//故障判断，过电压1.05倍不投，1.1倍立即跳
	if(!EnCapSwitch.B.overVoltFaltEn){
	if(      gpVoltA_rms > 440 || gpVoltB_rms > 440 || gpVoltC_rms > 440){	//全部切除
		CSRPIF = 2;
	}else if(gpVoltA_rms > 425 || gpVoltB_rms > 425 || gpVoltC_rms > 425){	//不动作
		if(CSRPIF == 1)
			CSRPIF = 0;
	}else if(gpVoltA_rms < 40  || gpVoltB_rms < 40  || gpVoltC_rms < 40) {	//全部切除
		CSRPIF = 2;
		}
	}

	if(!EnCapSwitch.B.compenEn){
		//if(SciCStatus.flag.B.ideal){						//通信链路空闲
		//	CapcitorCommAct();								//
		//}

/*		if(!EnCapSwitch.B.commFaltEn){						//故障检测开启
			if(CntMs.StartDelay > (CNT_MS(1100)){
				if(cntForCapaDisconn>=10){
					cntForCapaDisconn=10;
					if(softwareFaultWord2.B.fixedCapComm == 0){	//14位，Bit14:电容器组通信失败(吉林特纳普)
						softwareFaultWord2.B.fixedCapComm = FaultDetect(SOE_GP_FAULT+30,30,10);//连续10秒得不到数据帧，通信故障
					}else{
						SetFaultDelayCounter( ,0);
					}
				}
			}
	}
*/
		if(!EnCapSwitch.B.ctrl){
			if((!EnCapSwitch.B.handMode)&&(CapaSwithIF)){
			//	CommCapacSwithingExc();							//通信的方式的电容切换执行
				CapaSwithIF = 0;
			}
		}else{
		#if COMM_FIXED_CAP == 0
			if((EnCapSwitch.B.IOMode==3)||(EnCapSwitch.B.IOMode==1))
				CapacitorSwithingExc();												//电容切换执行
		#endif
		;
}
	}
}

//------------电容切换执行---------------------------------
void CapacitorSwithingExc(void)
{
	/*
	switch(CSRPIF)
	{
	case 1:
		if(CSRPIncount == 0)
		{
			if(ExtCapCtrl1 && CSRPCap1 == 0)
				{ExtCapCtrl1 = 0;CSRPCap1 = CapacitorCloseTime;}
			else CSRPIncount++;
		}
		if(CSRPIncount == 1)
		{
			if(ExtCapCtrl2 && CSRPCap2 == 0)
				{ExtCapCtrl2 = 0;CSRPCap2 = CapacitorCloseTime;}
			else CSRPIncount++;
		}
		if(CSRPIncount == 2)
		{
			if(ExtCapCtrl3 && CSRPCap3 == 0)
				{ExtCapCtrl3 = 0;CSRPCap3 = CapacitorCloseTime;}
			else CSRPIncount++;
		}
		if(CSRPIncount == 3)
		{
			if(ExtCapCtrl4 && CSRPCap4 == 0)
				{ExtCapCtrl4 = 0;CSRPCap4 = CapacitorCloseTime;}
			else CSRPIncount++;
		}
		if(CSRPIncount == 4)
		{
			if(ExtCapCtrl5 && CSRPCap5 == 0)
				{ExtCapCtrl5 = 0;CSRPCap5 = CapacitorCloseTime;}
			else CSRPIncount++;
		}
		if(CSRPIncount == 5)
		{
			if(ExtCapCtrl6 == 0 && CSRPCap6 == 0)
				{ExtCapCtrl6 = 1;CSRPCap6 = CapacitorCloseTime;}
			else CSRPIncount++;
		}
		if(CSRPIncount == 6)
		{
			if(ExtCapCtrl7 == 0 && CSRPCap7 == 0)
				{ExtCapCtrl7 = 1;CSRPCap7 = CapacitorCloseTime;}
			else CSRPIncount++;
		}
		if(CSRPIncount == 7)
		{
			if(ExtCapCtrl8 == 0 && CSRPCap8 == 0)
				{ExtCapCtrl8 = 1;CSRPCap8 = CapacitorCloseTime;}
			else CSRPIncount++;
		}
		CSRPIncount++;
		if(CSRPIncount>FIXED_IOCAP_NUM)CSRPIncount=0;
		break;//切入一个电容
	case 2:
		if(CSRPDecount == 0)
		{
			if(ExtCapCtrl1 == 0 && CSRPCap1 == 0)
				{ExtCapCtrl1 = 1;CSRPCap1 = CapacitorTripTime;}
			else CSRPDecount++;
		}
		if(CSRPDecount == 1)
		{
			if(ExtCapCtrl2 == 0 && CSRPCap2 == 0)
				{ExtCapCtrl2 = 1;CSRPCap2 = CapacitorTripTime;}
			else CSRPDecount++;
		}
		if(CSRPDecount == 2)
		{
			if(ExtCapCtrl3 == 0 && CSRPCap3 == 0)
				{ExtCapCtrl3 = 1;CSRPCap3 = CapacitorTripTime;}
			else CSRPDecount++;
		}
		if(CSRPDecount == 3)
		{
			if(ExtCapCtrl4 == 0 && CSRPCap4 == 0)
				{ExtCapCtrl4 = 1;CSRPCap4 = CapacitorTripTime;}
			else CSRPDecount++;
		}
		if(CSRPDecount == 4)
		{
			if(ExtCapCtrl5 == 0 && CSRPCap5 == 0)
				{ExtCapCtrl5 = 1;CSRPCap5 = CapacitorTripTime;}
			else CSRPDecount++;
		}
		if(CSRPDecount == 5)
		{
			if(ExtCapCtrl6 == 1 && CSRPCap6 == 0)
				{ExtCapCtrl6 = 0;CSRPCap6 = CapacitorTripTime;}
			else CSRPDecount++;
		}
		if(CSRPDecount == 6)
		{
			if(ExtCapCtrl7 == 1 && CSRPCap7 == 0)
				{ExtCapCtrl7 = 0;CSRPCap7 = CapacitorTripTime;}
			else CSRPDecount++;
		}
		if(CSRPDecount == 7)
		{
			if(ExtCapCtrl8 == 1 && CSRPCap8 == 0)
				{ExtCapCtrl8 = 0;CSRPCap8 = CapacitorTripTime;}
			else CSRPDecount++;
		}
		CSRPDecount++;
		if(CSRPDecount>FIXED_IOCAP_NUM)CSRPDecount=0;
		break;//切出一个电容
	default:
		break;//不动作
	}
	CSRPIF = 0;
cntForCapaSwithIF =0;*/
}
#else
void CapacitorSwithingCtr(void){;}//电容切换控制
void CapacitorSwithingExc(void){;}
#endif

