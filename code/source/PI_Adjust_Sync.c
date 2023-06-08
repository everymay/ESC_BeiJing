#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "VariblesRef.h"
#include "MarcoDefine.h"
#include "mbcrc.h"
#include "DSP28x_Project.h"

#pragma	CODE_SECTION(Pi_Adjust_Sync ,"ram2func")
#pragma	CODE_SECTION(Receive_Signe ,"ram2func")
#pragma	CODE_SECTION(HardwareLeadCorr ,"ram2func")
inline void Conversion4bTo5b(Uint16 Number);

// 主机地址为0，那么需要设定屏蔽主机同步信号错误的故障。
//#define PWM_address    0

// 问题：捕获上下降沿，是否需要Ts
// 唯一单元的机型，不进入同步中断
// 状态1用于初始化，状态2调整PI输出
// 注意：不可以通过上位机修改开关频率。
// 程序直接从成世杰程序移植。
/*
 *    5单元
  				case 1:		POINT_CAP1 = T1PR*2-1125;	//(0 + 4688*(3-1))=9376-1125=8251
					break;
				case 2:		POINT_CAP1 = T1PR*4-1125;	//(0 + 4688*(5-1))=4688-1125=3563
					break;
				case 3:		POINT_CAP1 = T1PR  -1125;	//(0 + 4688*(2-1))=14064-1125=12939
					break;
				case 4:		POINT_CAP1 = T1PR*3-1125;	//(0 + 4688*(4-1))=14064-1125=12939
					break;
 *   4单元
    			case 1:		POINT_CAP1 = T1PR*2-1125;	//(0 + 4688*(3-1))=9376-1125=8251
					break;
				case 2:		POINT_CAP1 = T1PR  -1125;	//(0 + 4688*(2-1))=4688-1125=3563
					break;
				case 3:		POINT_CAP1 = T1PR*3 -1125;	//(0 + 4688*(4-1))=14064-1125=12939
					break;
 *  6 单元 ，分配为 1 3 5 2 4 6
 				case 1:		POINT_CAP1 = T1PR*2-1125;	//(0 + 4688*(3-1))=9376-1125=8251
					break;
				case 2:		POINT_CAP1 = T1PR*4 -1125;	//(0 + 4688*(5-1))=4688-1125=3563
					break;
				case 3:		POINT_CAP1 = T1PR*1 -1125;	//(0 + 4688*(2-1))=14064-1125=12939
					break;
				case 4:		POINT_CAP1 = T1PR*3-1125;	//(0 + 4688*(4-1))=14064-1125=12939
					break;
				case 5:		POINT_CAP1 = T1PR*5-1125;	//(0 + 4688*(4-1))=14064-1125=12939
					break;

 * (0 + 4688*(3-1))=9376 -1125= 8251
 * (0 + 4688*(5-1))=18752-1125=17627
 * (0 + 4688*(2-1))=12939-1125=11814
 * (0 + 4688*(4-1))=14064-1125=12939
 * (0 + 4688*(6-1))=23440-1125=22315
 */

#define S2DIV3 	0.6666667f

#define LED_SYNC_SUCCESS 0	//同步IO信号指示,0让LED亮，表示同步成功
#define LED_SYNC_FAILUER 1

#if PWM_FREQUENCE_16KHZ
	#define CAPMULT 2
	#define CNTFOR_SYNCOUT 4
#elif PWM_FREQUENCE_12p8KHZ
#define CAPMULT 2
	#define CNTFOR_SYNCOUT 4
#else
	#define CAPMULT 2
	#define CNTFOR_SYNCOUT 4
#endif

#define LOW_LEVEL 			0
#define HIGH_LEVEL 			1
#define MEDIANT 			CNTFOR_SYNCOUT/2
#define FIVE_BIT 			5
#define FOUR_BIT_NUMBER 	200
#define LOW_LEVEL_WIDTH 	5*CNTFOR_SYNCOUT-1
#define HIGH_LEVEL_WIDTH 	5*CNTFOR_SYNCOUT-3
//#pragma DATA_SECTION(SzMemory,"EBSS1");
//#pragma DATA_SECTION(Sz1,"EBSS1");
//#pragma DATA_SECTION(Sz2,"EBSS1");
//#pragma DATA_SECTION(Sz3,"EBSS1");
//#pragma DATA_SECTION(InformationMemory,"EBSS2");
float KP_CAP=0.009f;
float KI_CAP=0.003f;
Uint16 DropDeadHalt=0;
Uint16 crc4b5b =1;
Uint16 SzMemory[FOUR_BIT_NUMBER];
Uint16 Sz1[CNTFOR_SYNCOUT][FIVE_BIT];
Uint16 Sz2[FOUR_BIT_NUMBER];
Uint16 Sz3[FOUR_BIT_NUMBER/2];
Stru_InformationMemory InformationMemory[200];
/*
 * Name		 4b		 5b
 *  0		0000	11110
 *  1		0001	01001
 *  2		0010	10100
 *  3		0011	10101
 *  4		0100	01010
 *  5		0101	01011
 *  6		0110	01110
 *  7		0111	01111
 *  8		1000	10010
 *  9		1001	10011
 *  10		1010	10110
 *  11		1011	10111
 *  12		1100	11010
 *  13		1101	11011
 *  14		1110	11100
 *  15		1111	11101
 *  16	    Head1	11111
 *  17	    Head2	00000
 *  18	    Tail	11001
 */
const Uint16 Convert4bTo5b[] = {30,9,20,21,10,11,14,15,18,19,22,23,26,27,28,29,31,0,25};
const Uint16 Convert5bTo4b[] = {17,0,0,0,0,0,0,0,0,1,4,5,0,0,6,7,0,0,8,9,2,3,10,11,0,18,12,13,14,15,0,16};

float32 HardwareLeadCorr(float32 base,int16 x)
{
	float32 tmp;
	tmp= T1PR*CAPMULT*2*1.0f*(base*x+PWM_address) - SyncHardwareLead;
	if(tmp<0)
		tmp+=T1PR*CAPMULT*2;
	return tmp;
}

void Send_Order(Uint16 addr,Uint16 Number)
{
	if(StateFlag.SendFlag==0)
	{
//		Uint16 i;
		Uint16 crc1,crc2,crc3,crc4;
		SendFSM = 0;
		SyncInformation.Send4bCount = 0;
		SyncInformation.Conversion5bCount = 0;
		Conversion4bTo5b(16);
		Conversion4bTo5b(17);
		Conversion4bTo5b(16);
		Conversion4bTo5b(17);				//帧头
//		for(i=0;i<Number;i++)	Conversion8bTo5b(*(addr+i));
		Conversion8bTo5b(addr);
		Conversion8bTo5b(Number);
		crc2 = crc1 = usMBCRC16((Uint16 *)&SzMemory[0],SyncInformation.Conversion5bCount);
		crc3 = (crc1&0xFF00)>>8;
		crc4 = (crc2&0x00FF)<<8;
		crc4 = crc3|crc4;
		Conversion16bTo5b(crc4);  			//CRC
		Conversion4bTo5b(18);				//帧尾
		SyncInformation.Send5bNumber = SyncInformation.Send4bCount*FIVE_BIT;
		SyncInformation.Send5bCount = 0;
		StateFlag.SendFlag = 1;						//发送标志
	}
}

void Get_Order(void)
{
	if((Sz3[1]==1)&&(StateEventFlag==STATE_EVENT_WAIT))		StateFlag.stopwait = 1;
	if((Sz3[1]==2)&&(StateEventFlag!=STATE_EVENT_WAIT))		StateFlag.waitFlag = 1;
	if(Sz3[1]==3)
	{
		if(SubordinateStatus&&(StateEventFlag!=STATE_EVENT_WAIT))		StateFlag.waitFlag = 1;
		else if(StateEventFlag==STATE_EVENT_WAIT)			StateFlag.stopwait = 1;
	}
	StateFlag.RxcFlag = 0;
}

void Conversion8bTo5b(Uint16 Number)
{
	Uint16 c,d;
	SzMemory[SyncInformation.Conversion5bCount] = Number;
	SyncInformation.Conversion5bCount++;
	c = (Number&0x00F0)>>4;
	d = (Number&0x000F);
	Conversion4bTo5b(c);
	Conversion4bTo5b(d);
}

void Conversion16bTo5b(Uint16 Number)
{
	Uint16 a,b,c,d;
	SzMemory[SyncInformation.Conversion5bCount] = Number;
	SyncInformation.Conversion5bCount++;
	a = (Number&0xF000)>>12;
	b = (Number&0x0F00)>>8;
	c = (Number&0x00F0)>>4;
	d = (Number&0x000F);
	Conversion4bTo5b(a);
	Conversion4bTo5b(b);
	Conversion4bTo5b(c);
	Conversion4bTo5b(d);
}

inline void Conversion4bTo5b(Uint16 Number)
{
	InformationMemory[SyncInformation.Send4bCount].all = Convert4bTo5b[Number];
	SyncInformation.Send4bCount++;
}

void Sync_OUT(char OUT)
{
	SET_OUT_SYNC_SINGLE(OUT);
	if(++SyncInformation.SendCount>=CNTFOR_SYNCOUT)
	{
		SyncInformation.SendCount = 0;
		if(SendFSM==(FIVE_BIT-1))					SyncInformation.Send5bCount++;
		if ((--SyncInformation.Send5bNumber)==0)	StateFlag.SendFlag = 0;
		else 		  				SendFSM++;
		if(SendFSM>(FIVE_BIT-1))  					SendFSM = 0;
	}
}

void Signe_Head(char Should, Uint16 Count)
{
	if(GET_SYNC_SINGLE()==Should)							SyncInformation.ReceiveCount++;
	else if((GET_SYNC_SINGLE()==(!Should))&&(SyncInformation.ReceiveCount>=Count))
	{
		SyncInformation.ReceiveCount = 0;
		ReceiveFSM++;
	}
	else
	{
		SyncInformation.ReceiveCount = 0;
		ReceiveFSM = 0;
	}
}

void Signe_Switch(void)
{
	Sz1[SyncInformation.ReceiveCount][SyncInformation.Receive5bCount] = GET_SYNC_SINGLE();

	if((SyncInformation.Receive5bCount==(FIVE_BIT-1))&&(SyncInformation.ReceiveCount==(CNTFOR_SYNCOUT-1))) 			//一组5B码接收完成
	{
		Sz2[SyncInformation.Receive4bCount] = (Sz1[MEDIANT][0]<<4)+(Sz1[MEDIANT][1]<<3)+(Sz1[MEDIANT][2]<<2)+(Sz1[MEDIANT][3]<<1)+(Sz1[MEDIANT][4]);				//5B计算
		if((SyncInformation.Receive4bCount%2)==1)
		{
			Sz3[SyncInformation.Receive8bCount] = ((Convert5bTo4b[Sz2[SyncInformation.Receive4bCount-1]])<<4)+Convert5bTo4b[Sz2[SyncInformation.Receive4bCount]];	//5B转4B 4B转8B
			SyncInformation.Receive8bCount++;
		}
		SyncInformation.Receive4bCount++;
	}

	if(++SyncInformation.ReceiveCount>=CNTFOR_SYNCOUT)
	{
		SyncInformation.ReceiveCount = 0;
		if(++SyncInformation.Receive5bCount>=FIVE_BIT)	SyncInformation.Receive5bCount = 0;
	}
}

void Receive_Signe(void)
{
	if(StateFlag.RxcFlag == 0)
	{
		switch(ReceiveFSM){
		case 0:		Signe_Head(HIGH_LEVEL,HIGH_LEVEL_WIDTH);		break;					//case0~3： 帧头检测
		case 1:		Signe_Head(LOW_LEVEL,LOW_LEVEL_WIDTH);			break;
		case 2:		Signe_Head(HIGH_LEVEL,HIGH_LEVEL_WIDTH);		break;
		case 3:
				if(GET_SYNC_SINGLE()==LOW_LEVEL)		SyncInformation.ReceiveCount++;
				else
				{
					ReceiveFSM = 0;
					SyncInformation.ReceiveCount = 0;
				}
				if(SyncInformation.ReceiveCount>=LOW_LEVEL_WIDTH)
				{
					ReceiveFSM++;
					SyncInformation.ReceiveCount = 0;
				}
		break;
		case 4:																	//单元地址检测
				Signe_Switch();
				if(SyncInformation.Receive8bCount==1)
				{
					if((Sz3[0]==0)||(Sz3[0]==PWM_address))	ReceiveFSM = 5;					//广播模式或单元地址匹配进入到下个Case
					else
					{
						SyncInformation.Receive4bCount = 0;
						SyncInformation.Receive8bCount = 0;
						ReceiveFSM = 0;
					}
				}
		break;
		case 5:																	//数据接收
				Signe_Switch();
				if((Sz2[SyncInformation.Receive4bCount-1]==Convert4bTo5b[18])||(SyncInformation.Receive4bCount>=200))	//检测帧尾
				{
					crc4b5b = usMBCRC16((Uint16 *)&Sz3[0],SyncInformation.Receive8bCount);
					if(crc4b5b==0)			StateFlag.RxcFlag = 1;												//CRC校验正确
					SyncInformation.Receive4bCount = 0;
					SyncInformation.Receive8bCount = 0;
					ReceiveFSM = 0;
				}
		break;
		}
	}
}

void Pi_Adjust_Sync(void)
{
/*	if(PWM_address == 0){
		if(StateFlag.SendFlag)
		{
			switch(SendFSM){
				case 0:		Sync_OUT(InformationMemory[SyncInformation.Send5bCount].B.Sendbuff5);	 	break;
				case 1:		Sync_OUT(InformationMemory[SyncInformation.Send5bCount].B.Sendbuff4);		break;
				case 2:		Sync_OUT(InformationMemory[SyncInformation.Send5bCount].B.Sendbuff3);		break;
				case 3:		Sync_OUT(InformationMemory[SyncInformation.Send5bCount].B.Sendbuff2);		break;
				case 4:		Sync_OUT(InformationMemory[SyncInformation.Send5bCount].B.Sendbuff1);		break;
			}
		}
		else if(++cntForSyncOut >= CNTFOR_SYNCOUT){
			cntForSyncOut = 0;
//			TOGGLE_OUT_SYNC_SINGLE();
		}
	}else{
	    capTsctr = (float)(ECap3Regs.TSCTR);   // 计数大于周期则重置。得到捕获中断周期
	}

	//注意,在非有效cap中断的时间内此程序也运行了.得改
	if(syncState == 1){
		Receive_Signe();
		while((capTsctr>(T1PR*CNTFOR_SYNCOUT*8))&&(++DropDeadHalt<8))
		{
			capTsctr-=T1PR*CNTFOR_SYNCOUT*8;
		}
		DropDeadHalt = 0;
		if((capTsctr < MAX_CAP1) && (capTsctr > MIN_CAP1)){		//误差允许范围内,正常情况下每隔6个中断进入一次
			syncErr =  POINT_CAP1 - capTsctr;
			pwmTsGiven += (syncErr - syncErrPrior)*KP_CAP + syncErr * KI_CAP;	//捕获PI控制器
	  		syncErrPrior = syncErr;

	  		if(pwmTsGiven > PWM_PERIOD_MAX)	  	pwmTsGiven = PWM_PERIOD_MAX;
	  		if(pwmTsGiven < PWM_PERIOD_MIN)	  	pwmTsGiven = PWM_PERIOD_MIN;
	  		//pwmTBPRD=T1PR_int16+DCL_runPI(&SYNC_PI_CTRL_DEFAULT, POINT_CAP1,capTsctr);
	  		pwmTBPRD = (Uint16)(pwmTsGiven);
	  		if(PrvPwmTBPRD!=pwmTBPRD){
                EPwm1Regs.TBPRD = pwmTBPRD-1;   //JCL:可能会出现多次调整后,因为载频不一样导致的采样点位置偏移,估计得开SYNCOSEL相关的硬件同步.
                EPwm4Regs.TBPRD = pwmTBPRD;
                EPwm5Regs.TBPRD = pwmTBPRD;
                EPwm6Regs.TBPRD = pwmTBPRD;
                EPwm7Regs.TBPRD = pwmTBPRD;
	  			PrvPwmTBPRD=pwmTBPRD;
	  		}
			cntForSyncErr = 0;
			//---------- 同步成功标志位  ---------------------------//
			if((syncErr <= syncErrLimit)&& (syncErr >= (-syncErrLimit))){
				if(cntSyncSucc++ >= FUNDPOINT/3){	//0.02秒指示同步成功,正常情况下每隔6个中断进入一次
					SET_LED_SYNC(LED_SYNC_SUCCESS);	//同步IO信号指示，=0亮，表示有信号！
					cntSyncSucc = FUNDPOINT/3+1;
//					pwmPeriodLimit = pwmTBPRD;
					quarterPeriod =  pwmTBPRD * 0.5;
				}else{;}
			}else{
				SET_LED_SYNC(LED_SYNC_FAILUER);
				cntSyncSucc = 0;
//				//pwmPllOk = FALSE;
                EPwm1Regs.TBPRD = pwmTBPRD-1;
                EPwm4Regs.TBPRD = pwmTBPRD;
                EPwm5Regs.TBPRD = pwmTBPRD;
                EPwm6Regs.TBPRD = pwmTBPRD;
                EPwm7Regs.TBPRD = pwmTBPRD;
 //				EPwm5Regs.CMPA.half.CMPA = SampleLead;
//				EPwm6Regs.CMPA.half.CMPA = T1PR - SampleLead;
////				pwmPeriodLimit = T1PR;
//				quarterPeriod =  T1PR * 0.5;
			}
		}else{
			if(cntForSyncErr++ >= 5*FUNDPOINT){		//0.04s指示错误并重新运行同步程序
				syncState = 0;
                EPwm1Regs.TBPRD = T1PR-1;
                EPwm4Regs.TBPRD = T1PR;
                EPwm5Regs.TBPRD = T1PR;
                EPwm6Regs.TBPRD = T1PR;
                EPwm7Regs.TBPRD = T1PR;
				// 上电同步过程中报故障，需要延时20秒进入。已观测到最大接近5秒
				if(CntMs.StartDelay > CNT_MS(35000)){	//1秒故障停机,35秒报同步失效故障
					if(softwareFaultWord2.B.pwmSyncErrFlag == 0){
						softwareFaultWord2.B.pwmSyncErrFlag = FaultDetectExtCnt(SOE_GP_FAULT+23,40*FUNDPOINT,cntForSyncErr);	// 2s
					}
					if(softwareFaultWord2.B.pwmSyncErrFlag != 0){
						SET_LED_SYNC(LED_SYNC_FAILUER);//灭
					cntForSyncErr = 0;
					syncState = 0;				//同步失败后自动重新开始同步.注掉此代码,则表示不再同步,得重新上电
					}
				}
			}
		}
	}else if(syncState == 0){
		if(cntForSyncInit <= 10){
			cntForSyncInit ++;
			// 8K : 1000000/8000 = 125 us
			// 4个单元： 125/4 --> 计数器的周期18750  18748/4=4687.5=4688
			// 125us:125/0.00666667=18750  18750/4=4688
			// 因实测用IO同步存在7.5us的延时，约1125
			// 给定在17625的时候完全重合，同步OK。
			if(MU_ALLCOUNT == 8){
				switch(PWM_address){		//角度不能顺序错开，而是要间隔错开125us/4
//				case 0:		POINT_CAP1=HardwareLeadCorr(1.0f/8,0);break;						//
				case 1:		POINT_CAP1=HardwareLeadCorr(1.0f/8,2);break;						//
				case 2:		POINT_CAP1=HardwareLeadCorr(1.0f/8,6);break;						//
				case 3:		POINT_CAP1=HardwareLeadCorr(1.0f/8,1);break;						//
				case 4:		POINT_CAP1=HardwareLeadCorr(1.0f/8,5);break;						//
				case 5:		POINT_CAP1=HardwareLeadCorr(1.0f/8,3);break;						//
				case 6:		POINT_CAP1=HardwareLeadCorr(1.0f/8,7);break;						//
				case 7:		POINT_CAP1=HardwareLeadCorr(1.0f/8,4);break;						//
				default:   	POINT_CAP1=HardwareLeadCorr(1.0f/8,0);break;
				}
			}else if(MU_ALLCOUNT == 7){		//根据仿真.7模块不能消谐,应采用4+3模式.
				switch(PWM_address){
//				case 0:		POINT_CAP1=HardwareLeadCorr(1.0f/7,0);break;						//
				case 1:		POINT_CAP1=HardwareLeadCorr(1.0f/7,2);break;						//
				case 2:		POINT_CAP1=HardwareLeadCorr(1.0f/7,4);break;						//
				case 3:		POINT_CAP1=HardwareLeadCorr(1.0f/7,6);break;						//
				case 4:		POINT_CAP1=HardwareLeadCorr(1.0f/7,1);break;						//
				case 5:		POINT_CAP1=HardwareLeadCorr(1.0f/7,3);break;						//
				case 6:		POINT_CAP1=HardwareLeadCorr(1.0f/7,5);break;						//
				default:   	POINT_CAP1=HardwareLeadCorr(1.0f/7,0);break;
				}
			}else if(MU_ALLCOUNT == 6){
				switch(PWM_address){
//				case 0:		POINT_CAP1=HardwareLeadCorr(1.0f/6,0);break;						//
				case 1:		POINT_CAP1=HardwareLeadCorr(1.0f/6,2);break;						//
				case 2:		POINT_CAP1=HardwareLeadCorr(1.0f/6,4);break;						//
				case 3:		POINT_CAP1=HardwareLeadCorr(1.0f/6,1);break;						//
				case 4:		POINT_CAP1=HardwareLeadCorr(1.0f/6,3);break;						//
				case 5:		POINT_CAP1=HardwareLeadCorr(1.0f/6,5);break;						//
				default:   	POINT_CAP1=HardwareLeadCorr(1.0f/6,0);break;
				}
			}else if(MU_ALLCOUNT == 5){		//根据仿真.5模块不能消谐,应采用2+3模式.
				switch(PWM_address){
//				case 0:		POINT_CAP1=HardwareLeadCorr(1.0f/5,0);break;						//
				case 1:		POINT_CAP1=HardwareLeadCorr(1.0f/5,2);break;						//
				case 2:		POINT_CAP1=HardwareLeadCorr(1.0f/5,4);break;						//
				case 3:		POINT_CAP1=HardwareLeadCorr(1.0f/5,1);break;						//
				case 4:		POINT_CAP1=HardwareLeadCorr(1.0f/5,3);break;						//
				default:   	POINT_CAP1=HardwareLeadCorr(1.0f/5,0);break;
				}
			}else if(MU_ALLCOUNT == 4){
				switch(PWM_address){
//				case 0:		POINT_CAP1=HardwareLeadCorr(1.0f/4,0);break;						//
				case 1:		POINT_CAP1=HardwareLeadCorr(1.0f/4,2);break;						//
				case 2:		POINT_CAP1=HardwareLeadCorr(1.0f/4,1);break;						//
				case 3:		POINT_CAP1=HardwareLeadCorr(1.0f/4,3);break;						//
				default:   	POINT_CAP1=HardwareLeadCorr(1.0f/4,0);break;
				}
			}else if(MU_ALLCOUNT == 3){		//三单元需要等分错开
				switch(PWM_address){
//				case 0:		POINT_CAP1=HardwareLeadCorr(1.0f/3,0);break;						//
				case 1:		POINT_CAP1=HardwareLeadCorr(1.0f/3,1);break;						//
				case 2:		POINT_CAP1=HardwareLeadCorr(1.0f/3,2);break;						//
				default:   	POINT_CAP1=HardwareLeadCorr(1.0f/3,0);break;
				}
			}else if(MU_ALLCOUNT == 2){
				switch(PWM_address){
//				case 0:		POINT_CAP1=HardwareLeadCorr(1.0f/2,0);break;						//
				case 1:		POINT_CAP1=HardwareLeadCorr(1.0f/2,1);break;						//
				default:   	POINT_CAP1=HardwareLeadCorr(1.0f/2,0);break;
				}
//			}else if(MU_ALLCOUNT == 1){
//				switch(PWM_address){
//				case 0:		POINT_CAP1=HardwareLeadCorr(1.0f/1,0);break;						//
//				default:   	POINT_CAP1=HardwareLeadCorr(1.0f/1,0);break;
//				}
			}else{
				POINT_CAP1 = 0;
			}

			MAX_CAP1 = POINT_CAP1 + T1PR*1.5;
			MIN_CAP1 = POINT_CAP1 - T1PR*1.5;
		}else{
			if(PWM_address == 0)	syncState = 3; 	// 确保主机不进入同步程序
			else syncState = 1;
			cntForSyncInit = 0;
			pwmTsGiven = (T1PR);
		}
	}*/
}

// ------------------------------------ NO MORE --------------------------------//

