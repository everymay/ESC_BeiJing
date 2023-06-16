
#ifndef MARCODEFINE_H_
#define MARCODEFINE_H_
#include "CoreCtrl.h"

//�������岻ͬ��·���
// #define   _NEW_BOARD_20141215   //  ͬʱ����Ҫ�ſ�main����#define _NewBoard_init
//PWM���Ϊ321������SET,����CLEAR����������ָ��IGBT�����ȡ����
//���ص�������Ϊ210
  

//#define   _LAB_50A_TEST
// ----------------------------  ���ò��������� ----------------------------------//
// 125A  ������ͬ��
// PWM���˳��321������SET,����CLEAR����������ָ��翹��������ȡ����
// ���ص�������˳��Ϊ201  DIANLIU

// ����֤��MEC ����������ָ��IGBT������ȡ����#define CUR_HUOER_DIR_IGBT      1
//

// 100KVA MEC  ����ɢ��Ƭ
// PWM���˳��Ϊ123������SET������CLEAR����������ָ��翹��������ȡ����
// ���ص�������˳��Ϊ201

/*  �ൺ APF
	PWM���˳��123������SET,����CLEAR����������ָ��翹��������ȡ����
	���ص�������˳��Ϊ201
*/
/*PWM���˳��Ϊ123������SET������CLEAR����������ָ��IGBT������ȡ����
    ���ص�������˳��Ϊ201
 */

// 150A  APF qianlong
//PWM���Ϊ321������SET,����CLEAR����������ָ��翹�������ȡ����
//���ص�������Ϊ102

// ١����MEC���ӵ�������ָ��IGBT 1111���������͵�Ԫ125Aһ��
// ����ĵ�Ԫ����������ָ��IGBT��PWM˳��ΪABC��������һ����

//#define MU_UNIT_NUM           1         // ��һ ������Ԫ���������������÷���LCD�����޸ġ�        // ������Ԫ���ڴˡ�
//#define APF_ID               0x10      // ��Ԫ��ַ,0x10 - 0x17 ֧��8����ַ

/*******************************************************************************/
#ifndef _LAB_50A_TEST
#define LAB_50A_APF             0
// �ڶ� ������������
#define CUR_HUOER_DIR_IGBT      1        // 1ָ��IGBT ��   0ָ��翹��
/*******************************************************************************/
// ����  PWM�����˳��
#define PWM_ABC_FLAG            1        // 1��ʾABC,123  0��ʾCBA 321
// ����   �ⲿ�����������Ĳ���˳����Ҫ��Է�ͷ�������������ϰ���(102)
#define EXT_CUR_SMP_ABC         1         // 1��ʾABC����201��0��ʾCBA����102
// ���� �����ű۵ķ�������Ҫ����������岻����ֱ�������ĵ�·��ġ�һ�㶼��UP_SET
#define DRIVER_UP_AQ_CLEAR      0
#endif
/*******************************************************************************/
// ��ʮһ ��ʾMEC����APF�ı�־λ��������������ʾ������
// 	��ʾMEC������������ʾ���������������Ϣ��ʾ������������ʾAPF�������Ӧ����ʾΪ��г��������
//#define DISPLAY_MEC_FLAG       1
/*******************************************************************************/
// ��ʮ�� ʵ����50A APFʵ�������Ӳ�����ò�һ�����ش����궨��
// laboratory 50AAPF-bench,�����ط���IO���ã�sample
// ʹ�ܵİ취Ϊ�ſ����Σ�ʹ����ifdef �� ifndef
//#define   _LAB_50A_TEST
/*******************************************************************************/
// ��ʮ�� ١����100KVA MEC ʹ����IO5�ڽ��з��������ת���ƣ�����������Ϊ��ת������Ϊ��ת
// ���ǵ�5���Ѿ������ⲿ����Ŀ��ƣ��������ⲿ���Ҳ��������֮ͣ�����ش˶�������
#define CONTROL_FUN_REVERSE    0
/*******************************************************************************/
// ��ʮ�� �Ƿ���Һ�����أ�û�еĻ��������޸� ����Ϊ1����Һ������Ϊ0���������Ϊ1.
//#define NO_LED_AUTO_START      1
/*******************************************************************************/

//Ӳ����·��IO������
#define U1X1_1 LED_SYNC				//�ⲿ��������1
#define U1X1_2 FAULT_LED			//���ϵ�
#define U1X1_3 RUNNING_LED			//���е�
#define U1X1_4 EXT_FUN_CTR			//�ⲿ���
#define U1X1_5 BYIIOout3			//�ⲿ��������2
#define U1X1_6 START_INSTRUCTION	//��ɫ����
#define U1X1_7 STOP_INSTRUCTION		//��ɫͣ��
#define U1X1_8 BYIOin1				//��·��״̬,����1
#define U1X1_9 FLback				//���׷���
#define U1X1_10 BYIOin3				//����3
#if (TNMD150A200415REV1)
	//�������
    #define SET_MAIN_CONTACT_ON(VAL)    if(VAL) GpioDataRegs.GPACLEAR.bit.GPIO4 = 1; else GpioDataRegs.GPFCLEAR.bit.GPIO163 = 1
	#define SET_MAIN_CONTACT_ON1(VAL) 	if(VAL) GpioDataRegs.GPFSET.bit.GPIO163 = 1; else GpioDataRegs.GPFCLEAR.bit.GPIO163 = 1
    #define SET_MAIN_CONTACT_ON2(VAL)   if(VAL) GpioDataRegs.GPASET.bit.GPIO4 = 1; else GpioDataRegs.GPACLEAR.bit.GPIO4 = 1
    #define GET_MAIN_CONTACT_ON1()              GpioDataRegs.GPFDAT.bit.GPIO163
    #define GET_MAIN_CONTACT_ON2()              GpioDataRegs.GPADAT.bit.GPIO4
    #define GET_MAIN_CONTACT_ON()                     //-����-��·��״̬
    #define SET_COLD_FUN_ON(VAL)        if(VAL) GpioDataRegs.GPBSET.bit.GPIO43 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO43 = 1
    #define GET_COLD_FUN_ON()                   GpioDataRegs.GPBDAT.bit.GPIO43     //-����-���13����
    #define SET_COLD_FUN_ON1(VAL)       if(VAL) GpioDataRegs.GPBSET.bit.GPIO42 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO42 = 1
    #define GET_COLD_FUN_ON1()                  GpioDataRegs.GPBDAT.bit.GPIO42     //-����-����2����
    #define SET_OUT_SYNC_SINGLE(VAL)    if(VAL) GpioDataRegs.GPASET.bit.GPIO14 = 1;  else GpioDataRegs.GPACLEAR.bit.GPIO14 = 1
    #define GET_OUT_SYNC_SINGLE()               GpioDataRegs.GPADAT.bit.GPIO14      //-ͬ���źŵķ���,�����ͬ���ź�Ҳ���Ӵ��������ź�
    #define TOGGLE_OUT_SYNC_SINGLE()            GpioDataRegs.GPATOGGLE.bit.GPIO14 = 1;
    #define SET_SYNC_SINGLE(VAL)        if(VAL) GpioDataRegs.GPASET.bit.GPIO15 = 1;  else GpioDataRegs.GPACLEAR.bit.GPIO15 = 1
    #define GET_SYNC_SINGLE()                   GpioDataRegs.GPADAT.bit.GPIO15      //-ECAP3ͬ���źŵĽ���
    #define SET_IGBT_FAULT_ACK1(VAL)    if(VAL) GpioDataRegs.GPDSET.bit.GPIO97= 1; else GpioDataRegs.GPDCLEAR.bit.GPIO97 = 1
    #define GET_IGBT_FAULT_ACK1()               GpioDataRegs.GPDDAT.bit.GPIO97  //-IGBT���Ϸ���
    #define SET_DCBUS_OV12(VAL)         if(VAL) GpioDataRegs.GPASET.bit.GPIO27 = 1; else GpioDataRegs.GPACLEAR.bit.GPIO27 = 1
    #define GET_DCBUS_OV12()                    GpioDataRegs.GPADAT.bit.GPIO27  //-��ѹ12�ϲ�,151130REV9�İ���160604REV11�İ���Ҫ�������
    #define SET_EXT_FUN_CTR(VAL)        if(VAL) GpioDataRegs.GPESET.bit.GPIO153 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO153 = 1
    #define GET_EXT_FUN_CTR()                   GpioDataRegs.GPEDAT.bit.GPIO153     //-DO2-�ⲿ����Ŀ���
    #define SET_EXT_FAULT_LED(VAL)      if(VAL) GpioDataRegs.GPESET.bit.GPIO154 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO154 = 1
    #define GET_EXT_FAULT_LED()                 GpioDataRegs.GPEDAT.bit.GPIO154     //-DO4-�ⲿ����-����ָʾ��
    #define SET_EXT_RUNNING_LED(VAL)    if(VAL) GpioDataRegs.GPESET.bit.GPIO150 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO150 = 1
    #define GET_EXT_RUNNING_LED()               GpioDataRegs.GPEDAT.bit.GPIO150     //-DO3-�ⲿ����-�豸����ָʾ��
    #define SET_PULSE(VAL)              if(VAL) GpioDataRegs.GPDSET.bit.GPIO110 = 1; else GpioDataRegs.GPDCLEAR.bit.GPIO110 = 1
    #define GET_PULSE()                         GpioDataRegs.GPDDAT.bit.GPIO110
    #define TOGGLE_PULSE()                      GpioDataRegs.GPDTOGGLE.bit.GPIO110 = 1;

    #define SET_RUNNING_LED(VAL)        if(VAL) GpioDataRegs.GPBSET.bit.GPIO59 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1
    #define GET_RUNNING_LED()                   GpioDataRegs.GPBDAT.bit.GPIO59    //-����-�豸����ָʾ��
    #define SET_FAULT_LED(VAL)          if(VAL) GpioDataRegs.GPBSET.bit.GPIO61 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1
    #define GET_FAULT_LED()                     GpioDataRegs.GPBDAT.bit.GPIO61    //-����-����ָʾ��
    #define SET_LED_SYNC(VAL)           if(VAL) GpioDataRegs.GPBSET.bit.GPIO60 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1
    #define GET_LED_SYNC()                      GpioDataRegs.GPBDAT.bit.GPIO60   //-����-ͬ��ʧЧ
    #define SET_GV_RMS_OVER_PROT_ON(VAL)
    #define GET_GV_RMS_OVER_PROT_ON()           GpioDataRegs.GPEDAT.bit.GPIO151//-DO5-������ѹ�����������

    #define SET_EXT_FUN_CTR0(VAL)       if(VAL) GpioDataRegs.GPESET.bit.GPIO151 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO151 = 1
    #define GET_EXT_FUN_CTR0()                  GpioDataRegs.GPEDAT.bit.GPIO151     //-DO0-��������0
    #define SET_EXT_FUN_CTR1(VAL)       if(VAL) GpioDataRegs.GPESET.bit.GPIO152 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO152 = 1
    #define GET_EXT_FUN_CTR1()                  GpioDataRegs.GPEDAT.bit.GPIO152     //-DO1-��������1

    #define SET_DBG_IO1(VAL)            if(VAL) GpioDataRegs.GPCSET.bit.GPIO91 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO91 = 1
    #define GET_DBG_IO1()                       GpioDataRegs.GPCDAT.bit.GPIO91      //-�������1
    #define SET_DBG_IO2(VAL)            if(VAL) GpioDataRegs.GPCSET.bit.GPIO92 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO92 = 1
    #define GET_DBG_IO2()                       GpioDataRegs.GPCDAT.bit.GPIO92      //-�������2
    #define SET_DBG_IO3(VAL)            if(VAL) GpioDataRegs.GPCSET.bit.GPIO76 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO76 = 1
    #define GET_DBG_IO3()                       GpioDataRegs.GPCDAT.bit.GPIO76      //-�������2


    #define START_INSTRUCTION           GpioDataRegs.GPCDAT.bit.GPIO77     //-DI5-��ɫ����
    #define STOP_INSTRUCTION            GpioDataRegs.GPCDAT.bit.GPIO78     //-DI4-��ɫͣ��
    #define FLback                      GpioDataRegs.GPCDAT.bit.GPIO79    //-DI2-���׷���
    #define GET_CTRL24_POWER()          0 //GpioDataRegs.GPCDAT.bit.GPIO79     //���Ƶ������
    #define IDInputIoBit0               GpioDataRegs.GPCDAT.bit.GPIO72     //��ַBit0
    #define IDInputIoBit1               GpioDataRegs.GPCDAT.bit.GPIO84     //��ַBit1
    #define IDInputIoBit2               GpioDataRegs.GPCDAT.bit.GPIO85     //��ַBit2
/*************************************���º��İ�IO��δ����**********************************************/
	#define BYIOin1				    GpioDataRegs.GPCDAT.bit.GPIO81		//-DI3-�ֶ���·��״̬
	#define BYIOin4					0  //GpioDataRegs.GPCDAT.bit.GPIO79  	//-DI1-����ڹ���(����������)
	#define BYIOin5					GpioDataRegs.GPCDAT.bit.GPIO78		//-DI0-���뱸��0
	#define TEMP1   				GpioDataRegs.GPCDAT.bit.GPIO92		//-����-�¶Ƚڵ�1
	#define TEMP2   				GpioDataRegs.GPCDAT.bit.GPIO91		//-����-�¶Ƚڵ�2
	//�����ź�
	#define ExtCapCtrl1 			SpuriousIn       	//�����ź�
	#define ExtCapCtrl2 			SpuriousIn      	//�����ź�
	#define ExtCapCtrl3 			SpuriousIn      	//�����ź�
	#define ExtCapCtrl4 			SpuriousIn      	//�����ź�
	#define ExtCapCtrl5 			SpuriousIn      	//�����ź�
	#define ExtCapCtrl6				SpuriousIn      	//�����ź�
	#define ExtCapCtrl7				SpuriousIn      	//�����ź�
	#define ExtCapCtrl8				SpuriousIn      	//�����ź�
	#define BYKIOout1 				SpuriousIn      	//�����ź�
	#define BYKIOout3 				SpuriousIn      	//�����ź�
	#define SET_PROG_RUNNING_LED(VAL)  		      	//������������ָʾ��
	#define GET_PROG_RUNNING_LED()  		SpuriousIn      	//������������ָʾ��
	#define SET_MACHINE_RUNNING(VAL)   		      	//�豸����ָʾ��
	#define GET_MACHINE_RUNNING()   		SpuriousIn      	//�豸����ָʾ��
#elif(NPC3CA10020210330REV1)
    //�������
    #define SET_PULSE(VAL)                    if(VAL) GpioDataRegs.GPDSET.bit.GPIO110 = 1; else GpioDataRegs.GPDCLEAR.bit.GPIO110 = 1
    #define GET_PULSE()                               GpioDataRegs.GPDDAT.bit.GPIO110             //�������
    #define TOGGLE_PULSE()                            GpioDataRegs.GPDTOGGLE.bit.GPIO110 = 1;
    #define SET_MAIN_CONTACT_OFF(VAL)         if(VAL) GpioDataRegs.GPFSET.bit.GPIO163 = 1; else GpioDataRegs.GPFCLEAR.bit.GPIO163 = 1
	#define GET_MAIN_CONTACT_OFF()                    GpioDataRegs.GPFDAT.bit.GPIO163             //�ű��̵ּ�����բ


	/*
	 * ����A����·�ű��̵ּ���״̬��WY
	 * VAL = 0���պϣ�
	 * VAL = 1���Ͽ���
	 */
	#define SET_MAIN_CONTACT_ACTION_A(VAL)    if((GET_POWER_CTRL == 0)&&(StateEventFlag_A != STATE_EVENT_STANDBY_A)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPCSET.bit.GPIO79 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO79 = 1



	/*
	 * A����·�ű��̵ּ����Ŀ������š�WY
	 * 0���պϣ�
	 * 1���Ͽ���
	 */
	#define GET_MAIN_CONTACT_ACTION_A                 GpioDataRegs.GPCDAT.bit.GPIO79              // A���/��ѹ�ű��̵ּ���

	/*
	 * ����A����·�ű��̵ּ���.WY
	 * VAL = 0���պϣ�
	 * VAL = 1���Ͽ���
	 */
    #define SET_BYPASS_CONTACT_ACTION_A(VAL)  if((GET_POWER_CTRL == 0)&&(StateEventFlag_A != STATE_EVENT_STANDBY_A)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPCSET.bit.GPIO78 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO78 = 1

	/*
	 * A����·�ű��̵ּ����Ŀ������š�WY
	 * 0���պϣ�
	 * 1���Ͽ���
	 */
	#define GET_BYPASS_CONTACT_ACTION_A               GpioDataRegs.GPCDAT.bit.GPIO78              // A����·�ű��̵ּ���

	/*
	 * ����B����·�ű��̵ּ���״̬��WY
	 * VAL = 0���պϣ�
	 * VAL = 1���Ͽ���
	 */
    #define SET_MAIN_CONTACT_ACTION_B(VAL)    if((GET_POWER_CTRL == 0)&&(StateEventFlag_B != STATE_EVENT_STANDBY_B)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPESET.bit.GPIO151 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO151 = 1

	/*
	 * B����·�ű��̵ּ����������š�WY
	 * 0���պϣ�
	 * 1���Ͽ���
	 */
	#define GET_MAIN_CONTACT_ACTION_B                 GpioDataRegs.GPEDAT.bit.GPIO151             // B���/��ѹ�ű��̵ּ���


	/*
	 * ����B����·�ű��̵ּ���״̬.WY
	 * VAL = 0���պϣ�
	 * VAL = 1���Ͽ���
	 */
	#define SET_BYPASS_CONTACT_ACTION_B(VAL)  if((GET_POWER_CTRL == 0)&&(StateEventFlag_B != STATE_EVENT_STANDBY_B)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPESET.bit.GPIO150 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO150 = 1

	/*
	 * B����·�ű��̵ּ����Ŀ������š�WY
	 * 0���պϣ�
	 * 1���Ͽ���WY*/
	#define GET_BYPASS_CONTACT_ACTION_B               GpioDataRegs.GPEDAT.bit.GPIO150             // B����·�ű��̵ּ���

	/*
	 * ����C����·�ű��̵ּ���״̬��WY
	 * VAL = 0���պϣ�
	 * VAL = 1���Ͽ���
	 */
    #define SET_MAIN_CONTACT_ACTION_C(VAL)    if((GET_POWER_CTRL == 0)&&(StateEventFlag_C != STATE_EVENT_STANDBY_C)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPCSET.bit.GPIO77 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO77 = 1

	/*
	 * C����·�ű��̵ּ����������š�WY
	 * 0���պϣ�
	 * 1���Ͽ���
	 */
    #define GET_MAIN_CONTACT_ACTION_C                 GpioDataRegs.GPCDAT.bit.GPIO77              // C���/��ѹ�ű��̵ּ���

	/*
	 * ����C����·�ű��̵ּ���״̬.WY
	 * VAL = 0���պϣ�
	 * VAL = 1���Ͽ���
	 */
    #define SET_BYPASS_CONTACT_ACTION_C(VAL)  if((GET_POWER_CTRL == 0)&&(StateEventFlag_C != STATE_EVENT_STANDBY_C)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPESET.bit.GPIO152 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO152 = 1

	/*
	 * C����·�ű��̵ּ����Ŀ������š�WY
	 * 0���պϣ�
	 * 1���Ͽ���WY*/
    #define GET_BYPASS_CONTACT_ACTION_C               GpioDataRegs.GPEDAT.bit.GPIO152             // C����·�ű��̵ּ���

	/*
	 * ����A����·��բ��״̬��WY
	 * VAL = 0����ͨ��
	 * VAL = 1���ضϡ�
	 */
    #define SET_SCRA_ENABLE(VAL)              if((GET_POWER_CTRL == 0)&&(StateEventFlag_A != STATE_EVENT_STANDBY_A)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPESET.bit.GPIO154 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO154 = 1  //SCRA_EN

   	/*
	 * ����B����·��բ��״̬��WY
	 * VAL = 0����ͨ��
	 * VAL = 1���ضϡ�
	 */
    #define SET_SCRB_ENABLE(VAL)              if((GET_POWER_CTRL == 0)&&(StateEventFlag_B != STATE_EVENT_STANDBY_B)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPESET.bit.GPIO153 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO153 = 1  //SCRB_EN

   	/*
	 * ����C����·��բ��״̬��WY
	 * VAL = 0����ͨ��
	 * VAL = 1���ضϡ�
	 */
    #define SET_SCRC_ENABLE(VAL)              if((GET_POWER_CTRL == 0)&&(StateEventFlag_C != STATE_EVENT_STANDBY_C)){ SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                                 if(VAL) GpioDataRegs.GPESET.bit.GPIO155 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO155 = 1  //SCRC_EN

   	/*
	 * �����豸����ָʾ��״̬��WY
	 * VAL = 0������
	 * VAL = 1��������
	 */
    #define SET_RUNNING_LED(VAL)              if(VAL) GpioDataRegs.GPESET.bit.GPIO158 = 1; else GpioDataRegs.GPECLEAR.bit.GPIO158 = 1

   	/*
	 * �豸����ָʾ��״̬��WY
	 * VAL = 0������
	 * VAL = 1��������
	 */
    #define GET_RUNNING_LED                           GpioDataRegs.GPEDAT.bit.GPIO158             //�豸����ָʾ��

   	/*
	 * �����豸����ָʾ��״̬��WY
	 * VAL = 0������
	 * VAL = 1��������
	 */
    #define SET_FAULT_LED(VAL)                if(VAL) GpioDataRegs.GPBSET.bit.GPIO42 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO42 = 1

   	/*
	 * �豸����ָʾ��״̬��WY
	 * VAL = 0������
	 * VAL = 1��������
	 */
    #define GET_FAULT_LED                             GpioDataRegs.GPBDAT.bit.GPIO42              //����ָʾ��

	/*
	 * ��ͣ��ť��WY
	 */
    #define STOP_INSTRUCTION                          GpioDataRegs.GPEDAT.bit.GPIO157             // ��ͣ��ť

	/*
	 * 15V��Դ��WY
	 */
    #define GET_CTRL24_POWER                          GpioDataRegs.GPADAT.bit.GPIO1               // 15V���Ƶ������
    #define SET_POWER_CTRL(VAL)               if(VAL) GpioDataRegs.GPDSET.bit.GPIO104 = 1; else GpioDataRegs.GPDCLEAR.bit.GPIO104 = 1
    #define GET_POWER_CTRL                            GpioDataRegs.GPDDAT.bit.GPIO104             // ����15V��Դ��������

    #define GET_GC_OVER_A                             GpioDataRegs.GPCDAT.bit.GPIO94              // A��Ӳ�������������
    #define GET_GC_OVER_B                             GpioDataRegs.GPCDAT.bit.GPIO85              // B��Ӳ�������������
    #define GET_GC_OVER_C                             GpioDataRegs.GPDDAT.bit.GPIO97              // C��Ӳ�������������
    #define SET_GV_VOL_CTRL_A(VAL)            if(VAL) GpioDataRegs.GPDSET.bit.GPIO105 = 1; else GpioDataRegs.GPDCLEAR.bit.GPIO105 = 1  //A��������С�̵���(��ֹ����������дű��ַ�������,�ϵ�û��Ԥ���ֱ��ը��,������ű��ֺ�ֱ������֮���һ��С�̵���,���ϵ缸����֮���ٿ�ͨ)
    #define GET_GV_VOL_CTRL_A                         GpioDataRegs.GPDDAT.bit.GPIO105
    #define SET_GV_VOL_CTRL_B(VAL)            if(VAL) GpioDataRegs.GPCSET.bit.GPIO76 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO76 = 1    //B��������С�̵���(��ֹ����������дű��ַ�������,�ϵ�û��Ԥ���ֱ��ը��,������ű��ֺ�ֱ������֮���һ��С�̵���,���ϵ缸����֮���ٿ�ͨ)
    #define GET_GV_VOL_CTRL_B                         GpioDataRegs.GPCDAT.bit.GPIO76
    #define SET_GV_VOL_CTRL_C(VAL)            if(VAL) GpioDataRegs.GPBSET.bit.GPIO43 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO43 = 1    //C��������С�̵���(��ֹ����������дű��ַ�������,�ϵ�û��Ԥ���ֱ��ը��,������ű��ֺ�ֱ������֮���һ��С�̵���,���ϵ缸����֮���ٿ�ͨ)
    #define GET_GV_VOL_CTRL_C                         GpioDataRegs.GPBDAT.bit.GPIO43
    #define SET_FLback(VAL)                   if(VAL) GpioDataRegs.GPCSET.bit.GPIO91 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO91 = 1
    #define GET_FLback                                GpioDataRegs.GPCDAT.bit.GPIO91              // ���׷���
    #define SET_BYPASS_FEEDBACK(VAL)          if(VAL) GpioDataRegs.GPCSET.bit.GPIO92 = 1; else GpioDataRegs.GPCCLEAR.bit.GPIO92 = 1
    #define GET_BYPASS_FEEDBACK                       GpioDataRegs.GPCDAT.bit.GPIO92              // ��·΢�Ϸ���

    #define SET_BOARDRUNNING_LED(VAL)         if(VAL) GpioDataRegs.GPBSET.bit.GPIO59 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1
    #define GET_BOARDRUNNING_LED                      GpioDataRegs.GPBDAT.bit.GPIO59    //-����-�豸����ָʾ��
    #define SET_BOARDFAULT_LED(VAL)           if(VAL) GpioDataRegs.GPBSET.bit.GPIO61 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1
    #define GET_BOARDFAULT_LED                        GpioDataRegs.GPBDAT.bit.GPIO61    //-����-����ָʾ��
    #define SET_LED_SYNC(VAL)                 if(VAL) GpioDataRegs.GPBSET.bit.GPIO60 = 1; else GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1
    #define GET_LED_SYNC                              GpioDataRegs.GPBDAT.bit.GPIO60   //-����-ͬ��ʧЧ


//�����ź�
    #define ExtCapCtrl1             SpuriousIn          //�����ź�
    #define ExtCapCtrl2             SpuriousIn          //�����ź�
    #define ExtCapCtrl3             SpuriousIn          //�����ź�
    #define ExtCapCtrl4             SpuriousIn          //�����ź�
    #define ExtCapCtrl5             SpuriousIn          //�����ź�
    #define ExtCapCtrl6             SpuriousIn          //�����ź�
    #define ExtCapCtrl7             SpuriousIn          //�����ź�
    #define ExtCapCtrl8             SpuriousIn          //�����ź�
    #define BYKIOout1               SpuriousIn          //�����ź�
    #define BYKIOout3               SpuriousIn          //�����ź�
    #define SET_PROG_RUNNING_LED(VAL)               //������������ָʾ��
    #define GET_PROG_RUNNING_LED()          SpuriousIn          //������������ָʾ��
    #define SET_MACHINE_RUNNING(VAL)                //�豸����ָʾ��
    #define GET_MACHINE_RUNNING()           SpuriousIn          //�豸����ָʾ��

#endif

//    #define SET_IGBT_EN1(VAL)    \
//    if(VAL == 0){          \
//        ErrorRecord.B.REC_IGBT_EN = 0;\
//        GpioDataRegs.GPECLEAR.bit.GPIO156 = 1;\
//        GpioDataRegs.GPFCLEAR.bit.GPIO164 = 1;\
//        GpioDataRegs.GPCCLEAR.bit.GPIO93 = 1;\
//    }else{\
//        ErrorRecord.B.REC_IGBT_EN = 1;\
//        GpioDataRegs.GPESET.bit.GPIO156 = 1;\
//        GpioDataRegs.GPFSET.bit.GPIO164 = 1;\
//        GpioDataRegs.GPCSET.bit.GPIO93 = 1;\
//    }
    #define SET_IGBT_ENA(VAL)     if(VAL) {if((GET_POWER_CTRL == 0)&&(StateEventFlag_A != STATE_EVENT_STANDBY_A)){SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                             GpioDataRegs.GPESET.bit.GPIO156 = 1; }else GpioDataRegs.GPECLEAR.bit.GPIO156 = 1
    #define SET_IGBT_ENB(VAL)     if(VAL) {if((GET_POWER_CTRL == 0)&&(StateEventFlag_B != STATE_EVENT_STANDBY_B)){SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                             GpioDataRegs.GPFSET.bit.GPIO164 = 1; }else GpioDataRegs.GPFCLEAR.bit.GPIO164 = 1
    #define SET_IGBT_ENC(VAL)     if(VAL) {if((GET_POWER_CTRL == 0)&&(StateEventFlag_C != STATE_EVENT_STANDBY_C)){SET_POWER_CTRL(1); Delayus(TIME_WRITE_15VOLT_REDAY);}\
                                             GpioDataRegs.GPCSET.bit.GPIO93  = 1; }else GpioDataRegs.GPCCLEAR.bit.GPIO93  = 1

    #define GET_IGBT_ENA                          GpioDataRegs.GPEDAT.bit.GPIO156       //A��PWMʹ��
    #define GET_IGBT_ENB                          GpioDataRegs.GPFDAT.bit.GPIO164       //B��PWMʹ��
    #define GET_IGBT_ENC                          GpioDataRegs.GPCDAT.bit.GPIO93        //C��PWMʹ��



#define ESC_ACTION_ENABLE           0
#define ESC_ACTION_DISABLE          1

#define ESC_FANACTION_ENABLE        1
#define ESC_FANACTION_DISABLE       0
// ����ĸ���ֻ�����������IO���ã���������������
// �͵�ƽ��Ч, 1��0�������˸ߺ͵�
#define ITS_WORKING_LOW             1
#define ITS_NO_WORKING_HIGH         0
// �ߵ�ƽ��Ч
#define ITS_WORKING_HIGH            0
#define ITS_NO_WORKING_LOW          1
// �����������ֻ�����������IO���ã���������������
#define GET_INPUT_WORKING_LOW       0
#define NOT_GET_INPUT_HIGH          1

#define IGBT_ENABLE                 1
#define IGBT_DISABLE                0
#define IGBT_FAULT    	 		    IGBT_DISABLE
#define IGBT_FEEDBACK_FAULT_FLAG    0

// bit7:debug;      bit6:warning;       bit5:reserve;       bit4:fault;     bit3~0:state
//#define STATE_EVENT_STANDBY		    0               //��ʼ״̬(����ͣ��)
//#define STATE_EVENT_RECHARGE 	    1               //���״̬
//#define STATE_EVENT_RUN 		    2               //��������
//#define STATE_EVENT_STOP		    3               //����ͣ��
//#define STATE_EVENT_WAIT 		    5               //����״̬
//#define STATE_EVENT_BEFORERUN	    6               //��������ǰ��׼��״̬.
//#define STATE_EVENT_DISCHARGE	    8               //�ŵ�״̬
//#define STATE_EVENT_FAULT 		    9               //����ͣ��
//#define STATE_EVENT_WARNING		    10              //�澯״̬
//#define STATE_EVENT_ALARM           11              //����״̬
//#define ORIGINAL_STATE			    12              //�Զ���λ_��ʼ״̬
//#define AUTO_DETECTION_STATE 	    13              //�Զ���λ_�Լ�״̬
//#define TWEAK_STATE	 			    14              //�Զ���λ_����״̬
//#define INSPECTION_STATE		    15              //�Զ���λ_����״̬

#define STATE_EVENT_STANDBY_A         0               //��ʼ״̬(����ͣ��)
#define STATE_EVENT_RECHARGE_A        1               //���״̬
#define STATE_EVENT_RUN_A             2               //��������
#define STATE_EVENT_STOP_A            3               //����ͣ��
#define STATE_EVENT_WAIT_A            4               //����״̬
#define STATE_EVENT_BEFORERUN_A       5               //��������ǰ��׼��״̬.
#define STATE_EVENT_DISCHARGE_A       6               //�ŵ�״̬
#define STATE_EVENT_FAULT_A           7               //����ͣ��
#define STATE_EVENT_WARNING_A         8              //�澯״̬
#define STATE_EVENT_ALARM_A           9              //����״̬
#define ORIGINAL_STATE_A              10              //�Զ���λ_��ʼ״̬
#define AUTO_DETECTION_STATE_A        11              //�Զ���λ_�Լ�״̬
#define TWEAK_STATE_A                 12              //�Զ���λ_����״̬
#define INSPECTION_STATE_A            13              //�Զ���λ_����״̬

#define STATE_EVENT_STANDBY_B         0               //��ʼ״̬(����ͣ��)
#define STATE_EVENT_RECHARGE_B        1               //���״̬
#define STATE_EVENT_RUN_B             2               //��������
#define STATE_EVENT_STOP_B            3               //����ͣ��
#define STATE_EVENT_WAIT_B            4               //����״̬
#define STATE_EVENT_BEFORERUN_B       5               //��������ǰ��׼��״̬.
#define STATE_EVENT_DISCHARGE_B       6               //�ŵ�״̬
#define STATE_EVENT_FAULT_B           7               //����ͣ��
#define STATE_EVENT_WARNING_B         8              //�澯״̬
#define STATE_EVENT_ALARM_B           9              //����״̬
#define ORIGINAL_STATE_B              10              //�Զ���λ_��ʼ״̬
#define AUTO_DETECTION_STATE_B        11              //�Զ���λ_�Լ�״̬
#define TWEAK_STATE_B                 12              //�Զ���λ_����״̬
#define INSPECTION_STATE_B            13              //�Զ���λ_����״̬

#define STATE_EVENT_STANDBY_C         0               //��ʼ״̬(����ͣ��)
#define STATE_EVENT_RECHARGE_C        1               //���״̬
#define STATE_EVENT_RUN_C             2               //��������
#define STATE_EVENT_STOP_C            3               //����ͣ��
#define STATE_EVENT_WAIT_C            4               //����״̬
#define STATE_EVENT_BEFORERUN_C       5               //��������ǰ��׼��״̬.
#define STATE_EVENT_DISCHARGE_C       6               //�ŵ�״̬
#define STATE_EVENT_FAULT_C           7               //����ͣ��
#define STATE_EVENT_WARNING_C         8              //�澯״̬
#define STATE_EVENT_ALARM_C           9              //����״̬
#define ORIGINAL_STATE_C              10              //�Զ���λ_��ʼ״̬
#define AUTO_DETECTION_STATE_C        11              //�Զ���λ_�Լ�״̬
#define TWEAK_STATE_C                 12              //�Զ���λ_����״̬
#define INSPECTION_STATE_C            13              //�Զ���λ_����״̬

#endif /* MARCODEFINE_H_ */
