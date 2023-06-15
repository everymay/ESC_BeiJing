/* DCL.h - C2000 Digital Controller Library Header File */
// version 1.0, August 2015

#ifndef _C_DCL_H
#define _C_DCL_H

#include "VariblesRef.h"
#include "DCLCLA.h"
/*** PID controller ***/

typedef volatile struct {
	float Kp;		// proportional gain
	float Ki;		// integral gain
	float Kd;		// derivative gain
	float Kr;		// set point weight
	float c1;		// D filter coefficient 1
	float c2;		// D filter coefficient 2
	float d2;		// D filter storage 1
	float d3;		// D filter storage 2
	float i10;		// I storage
	float i14;		// sat storage
	float Umax;		// upper saturation limit
	float Umin;		// lower saturation limit
} PID;

#define	PID_DEFAULTS { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f }


#define GENERATOR_COEFF_DEFAULTS_1 {\
		{97.5, 8.6, 17.6, 5, 6.9, 4.6, 2.9, 3.3, 0.9},		\
		{97.5, 8.6, 17.6, 5, 6.9, 4.6, 2.9, 3.3, 0.9},		\
		{97.5, 8.6, 17.6, 5, 6.9, 4.6, 2.9, 3.3, 0.9},		\
		{D2R(12),D2R(169),D2R(93 ),D2R(75),D2R(181),D2R(158),D2R(265),D2R(243),D2R(342)},	\
		{D2R(132),D2R(51),D2R(213),D2R(314),D2R(300),D2R(37),D2R(24),D2R(122),D2R(98 )},	\
		{D2R(252),D2R(290),D2R(333),D2R(195),D2R(420),D2R(278),D2R(146),D2R(2),D2R(226)},	\
		{5       ,7       ,11       ,13      ,17      ,19      ,23      ,25      ,29}}
#define GENERATOR_COEFF_DEFAULTS_2 {\
		{93.5, 29.9, 15.9, 5.2, 7, 1.5, 4.1, 0.4, 2.8},		\
		{93.5, 29.9, 15.9, 5.2, 7, 1.5, 4.1, 0.4, 2.8},		\
		{93.5, 29.9, 15.9, 5.2, 7, 1.5, 4.1, 0.4, 2.8},		\
		{D2R(240),D2R(25 ),D2R(250),D2R(45),D2R(277),D2R(80),D2R(303),D2R(140),D2R(329)},	\
		{D2R(360),D2R(265),D2R(10),D2R(285),D2R(36 ),D2R(319),D2R(63),D2R(8),D2R(89)},	\
		{D2R(119),D2R(145),D2R(130),D2R(163),D2R(157),D2R(194),D2R(183),D2R(239),D2R(210)},	\
		{5       ,7       ,11       ,13      ,17      ,19      ,23      ,25      ,29}}
//#define GENERATOR_COEFF_DEFAULTS_3 {\
//		{83.3, 53.4, 7.7, 8.4, 6.4, 2.2, 4.1, 2.8, 1.8},		\
//		{83.3, 53.4, 7.7, 8.4, 6.4, 2.2, 4.1, 2.8, 1.8},		\
//		{83.3, 53.4, 7.7, 8.4, 6.4, 2.2, 4.1, 2.8, 1.8},		\
//		{D2R(118),D2R(204),D2R(240),D2R(52),D2R(73),D2R(205),D2R(299),D2R(36),D2R(163)},	\
//		{D2R(238),D2R(84),D2R(0),D2R(292),D2R(193),D2R(85),  D2R(58),D2R(277),D2R(281)},	\
//		{D2R(358),D2R(324),D2R(120),D2R(172),D2R(313),D2R(325),D2R(178),D2R(157),D2R(41)},	\
//		{5       ,7       ,11       ,13      ,17      ,19      ,23      ,25      ,29}}
//#define GENERATOR_COEFF_DEFAULTS_4 {\
//		{70.4, 58.4, 30.1, 20.4, 10.8, 10.1, 6.1, 4.9, 3.8},		\
//		{70.4, 58.4, 30.1, 20.4, 10.8, 10.1, 6.1, 4.9, 3.8},		\
//		{70.4, 58.4, 30.1, 20.4, 10.8, 10.1, 6.1, 4.9, 3.8},		\
//		{D2R(194),D2R(310),D2R(10),D2R(140),D2R(243),D2R(20),D2R(108),D2R(250),D2R(0  )},	\
//		{D2R(312),D2R(188),D2R(128),D2R(16),D2R(359),D2R(256),D2R(223),D2R(124),D2R(115)},	\
//		{D2R(73), D2R(69),D2R(249),D2R(258),D2R(121),D2R(139),D2R(345),D2R(9),D2R(237)},	\
//		{5       ,7       ,11       ,13      ,17      ,19      ,23      ,25      ,29}}
//#define GENERATOR_COEFF_DEFAULTS_5 {\
//		{57.7, 54.6, 40.5, 35.7, 20.1, 15.6, 5.9, 5.3, 5.3},		\
//		{57.7, 54.6, 40.5, 35.7, 20.1, 15.6, 5.9, 5.3, 5.3},		\
//		{57.7, 54.6, 40.5, 35.7, 20.1, 15.6, 5.9, 5.3, 5.3},		\
//		{D2R(14),D2R(56),D2R(323),D2R(6),D2R(277),D2R(326),D2R(263),D2R(343),D2R(294)},	\
//		{D2R(133),D2R(295),D2R(81),D2R(245),D2R(35),D2R(203),D2R(22),D2R(218),D2R(52)},	\
//		{D2R(254),D2R(177),D2R(203),D2R(128),D2R(159),D2R(88),D2R(149),D2R(107),D2R(180)},	\
//		{5       ,7       ,11       ,13      ,17      ,19      ,23      ,25      ,29}}
//#define GENERATOR_COEFF_DEFAULTS_6 {\
//        {30      ,90       ,40      ,20     ,20      ,10      ,10      ,10       ,10},    \
//        {30      ,90       ,40      ,20     ,20      ,10      ,10      ,10       ,10},    \
//        {30      ,90       ,40      ,20     ,20      ,10      ,10      ,10       ,10},    \
//        {D2R(0)  ,D2R(180),D2R(0)  ,D2R(180),D2R(0)  ,D2R(180),D2R(0)  ,D2R(180),D2R(0)},   \
//        {D2R(240),D2R(300),D2R(240),D2R(300),D2R(240),D2R(300),D2R(240),D2R(300),D2R(240)}, \
//        {D2R(120),D2R(60) ,D2R(120),D2R(60) ,D2R(120),D2R(60) ,D2R(120),D2R(60) ,D2R(120)}, \
//        {3       ,5       ,7       ,11      ,13      ,17      ,19      ,23      ,25}}
//+90
//#define GENERATOR_COEFF_DEFAULTS_7 {\
//		{93.6, 23.7, 22.1, 5.4, 10.1, 1.3, 0, 0, 0},		\
//		{93.6, 23.7, 22.1, 5.4, 10.1, 1.3, 0, 0, 0},		\
//		{93.6, 23.7, 22.1, 5.4, 10.1, 1.3, 0, 0, 0},		\
//		{D2R(66  ),D2R(264),D2R(187 ),D2R(34),D2R(317),D2R(183),D2R(90),D2R(32),D2R(226)},	\
//		{D2R(186),D2R(147),D2R(307),D2R(284),D2R(77),D2R(86),D2R(210),D2R(254),D2R(343)},	\
//		{D2R(306),D2R(26 ),D2R(65),D2R(157),D2R(195),D2R(300),D2R(329),D2R(98),D2R(103)},	\
//		{5       ,7       ,11       ,13      ,17      ,19      ,23      ,25      ,29}}
//0
//#define GENERATOR_COEFF_DEFAULTS_7 {\
//		{93.6, 23.7, 22.1, 5.4, 10.1, 1.3, 0, 0, 0},		\
//		{93.6, 23.7, 22.1, 5.4, 10.1, 1.3, 0, 0, 0},		\
//		{93.6, 23.7, 22.1, 5.4, 10.1, 1.3, 0, 0, 0},		\
//		{D2R(336),D2R(174),D2R(97 ),D2R(304),D2R(227),D2R(93),D2R(0),D2R(302),D2R(136)},	\
//		{D2R(96),D2R(57),D2R(217),D2R(194),D2R(347),D2R(356),D2R(120),D2R(164),D2R(253)},	\
//		{D2R(216),D2R(296 ),D2R(335),D2R(67),D2R(105),D2R(210),D2R(239),D2R(8),D2R(13)},	\
//		{5       ,7       ,11       ,13      ,17      ,19      ,23      ,25      ,29}}
//-90
//#define GENERATOR_COEFF_DEFAULTS_7 {\
//		{93.6, 23.7, 22.1, 5.4, 10.1, 1.3, 0, 0, 0},    \
//		{93.6, 23.7, 22.1, 5.4, 10.1, 1.3, 0, 0, 0},		\
//		{93.6, 23.7, 22.1, 5.4, 10.1, 1.3, 0, 0, 0},		\
//		{D2R(246),D2R(84),D2R(7),D2R(214),D2R(137),D2R(3),D2R(270),D2R(212),D2R(46)},	\
//		{D2R(6),D2R(327),D2R(127),D2R(104),D2R(257),D2R(266),D2R(30),D2R(74),D2R(163)},	\
//		{D2R(126),D2R(206),D2R(245),D2R(337),D2R(15),D2R(120),D2R(149),D2R(278),D2R(283)},	\
//		{5       ,7       ,11       ,13      ,17      ,19      ,23      ,25      ,29}}
//+180
//#define GENERATOR_COEFF_DEFAULTS_7 {\
//		{93.6, 23.7, 22.1, 5.4, 10.1, 1.3, 0, 0, 0},		\
//		{93.6, 23.7, 22.1, 5.4, 10.1, 1.3, 0, 0, 0},		\
//		{93.6, 23.7, 22.1, 5.4, 10.1, 1.3, 0, 0, 0},		\
//		{D2R(156),D2R(354),D2R(277 ),D2R(124),D2R(47),D2R(273),D2R(180),D2R(122),D2R(316)},	\
//		{D2R(276),D2R(237),D2R(37),D2R(14),D2R(167),  D2R(176),D2R(300),D2R(344),D2R(73)},	\
//		{D2R(36),D2R(116 ),D2R(155),D2R(247),D2R(285),D2R(30),D2R(59),D2R(188),D2R(193)},	\
//		{5       ,7       ,11       ,13      ,17      ,19      ,23      ,25      ,29},   0x0000}


#define GENERATOR_COEFF_DEFAULTS_3 {\
      {70, 60, 30, 20, 10, 10, 5, 5, 5},        \
      {70, 60, 30, 20, 10, 10, 5, 5, 5},        \
      {70, 60, 30, 20, 10, 10, 5, 5, 5},        \
      {D2R(194),D2R(310),D2R(10),D2R(140),D2R(243),D2R(20),D2R(108),D2R(250),D2R(0  )},   \
      {D2R(312),D2R(188),D2R(128),D2R(16),D2R(359),D2R(256),D2R(223),D2R(124),D2R(115)},  \
      {D2R(73), D2R(69),D2R(249),D2R(258),D2R(121),D2R(139),D2R(345),D2R(9),D2R(237)},    \
      {5       ,7       ,11       ,13      ,17      ,19      ,23      ,25      ,29}}

#define GENERATOR_COEFF_DEFAULTS_4 {\
      {100, 100, 0, 0, 0, 0, 0, 0, 0},          \
      {100, 100, 0, 0, 0, 0, 0, 0, 0},          \
      {100, 100, 0, 0, 0, 0, 0, 0, 0},          \
      {D2R(14),D2R(56),D2R(323),D2R(6),D2R(277),D2R(326),D2R(263),D2R(343),D2R(294)}, \
      {D2R(133),D2R(295),D2R(81),D2R(245),D2R(35),D2R(203),D2R(22),D2R(218),D2R(52)}, \
      {D2R(254),D2R(177),D2R(203),D2R(128),D2R(159),D2R(88),D2R(149),D2R(107),D2R(180)},  \
      {5       ,7       ,11       ,13      ,17      ,19      ,23      ,25      ,29}}

#define GENERATOR_COEFF_DEFAULTS_5 {\
      {100, 0, 0, 50, 0, 0, 0, 0, 0},          \
      {100, 0, 0, 50, 0, 0, 0, 0, 0},          \
      {100, 0, 0, 50, 0, 0, 0, 0, 0},          \
      {D2R(14),D2R(56),D2R(323),D2R(6),D2R(277),D2R(326),D2R(263),D2R(343),D2R(294)}, \
      {D2R(133),D2R(295),D2R(81),D2R(245),D2R(35),D2R(203),D2R(22),D2R(218),D2R(52)}, \
      {D2R(254),D2R(177),D2R(203),D2R(128),D2R(159),D2R(88),D2R(149),D2R(107),D2R(180)},  \
      {5       ,7       ,11       ,13      ,17      ,19      ,23      ,25      ,29}}

#define GENERATOR_COEFF_DEFAULTS_6 {\
        {30      ,90       ,40      ,20     ,20      ,10      ,10      ,10       ,10},    \
        {30      ,90       ,40      ,20     ,20      ,10      ,10      ,10       ,10},    \
        {30      ,90       ,40      ,20     ,20      ,10      ,10      ,10       ,10},    \
        {D2R(0)  ,D2R(180),D2R(0)  ,D2R(180),D2R(0)  ,D2R(180),D2R(0)  ,D2R(180),D2R(0)},   \
        {D2R(240),D2R(300),D2R(240),D2R(300),D2R(240),D2R(300),D2R(240),D2R(300),D2R(240)}, \
        {D2R(120),D2R(60) ,D2R(120),D2R(60) ,D2R(120),D2R(60) ,D2R(120),D2R(60) ,D2R(120)}, \
        {3       ,5       ,7       ,11      ,13      ,17      ,19      ,23      ,25}}
//+90
#define GENERATOR_COEFF_DEFAULTS_7 {\
      {90, 20, 20, 5, 10, 5, 0, 0, 0},        \
      {90, 20, 20, 5, 10, 5, 0, 0, 0},        \
      {90, 20, 20, 5, 10, 5, 0, 0, 0},        \
      {D2R(66  ),D2R(264),D2R(187 ),D2R(34),D2R(317),D2R(183),D2R(90),D2R(32),D2R(226)},  \
      {D2R(186),D2R(147),D2R(307),D2R(284),D2R(77),D2R(86),D2R(210),D2R(254),D2R(343)},   \
      {D2R(306),D2R(26 ),D2R(65),D2R(157),D2R(195),D2R(300),D2R(329),D2R(98),D2R(103)},   \
      {5       ,7       ,11       ,13      ,17      ,19      ,23      ,25      ,29}}

#define GENERATOR_COEFF_DEFAULTS_8 {\
        {20         ,60         ,60         ,30         ,30         ,0          ,0          ,0          ,0},    \
        {20         ,60         ,60         ,30         ,30         ,0          ,0          ,0          ,0},    \
        {20         ,60         ,60         ,30         ,30         ,0          ,0          ,0          ,0},    \
        {D2R(0)     ,D2R(180)   ,D2R(0)     ,D2R(180)   ,D2R(0)     ,D2R(180)   ,D2R(0)     ,D2R(180)   ,D2R(0)},   \
        {D2R(240)   ,D2R(300)   ,D2R(240)   ,D2R(300)   ,D2R(240)   ,D2R(300)   ,D2R(240)   ,D2R(300)   ,D2R(240)}, \
        {D2R(120)   ,D2R(60)    ,D2R(120)   ,D2R(60)    ,D2R(120)   ,D2R(60)    ,D2R(120)   ,D2R(60)    ,D2R(120)}, \
        {3      ,5      ,7      ,11     ,13     ,17     ,19     ,23     ,25}}

#define GENERATOR_COEFF_DEFAULTS_ALL {GENERATOR_COEFF_DEFAULTS_1,GENERATOR_COEFF_DEFAULTS_2,GENERATOR_COEFF_DEFAULTS_3,GENERATOR_COEFF_DEFAULTS_4,GENERATOR_COEFF_DEFAULTS_5,GENERATOR_COEFF_DEFAULTS_6,GENERATOR_COEFF_DEFAULTS_7,GENERATOR_COEFF_DEFAULTS_8}
//#define COMPENSATE_COEFF_DEFAULTS {\
//		{0		,1.15	,1.12	,1		,1		,0		,0		,0		,0},	\
//		{0		,1.15	,1.12	,1		,1		,0		,0		,0		,0},	\
//		{0		,1.15	,1.12	,1		,1		,0		,0		,0		,0},	\
//		{0		,1 		,1		,7.6	,7.5	,10.3	,12		,15		,0},	\
//		{3		,5 		,7      ,11     ,13     ,17     ,19     ,23     ,25}, \
//		{100	,120	,100	,100	,100	,50		,50		,50		,50},	\
//		{0		,0 		,0		,0		,0		,0		,0		,0		,0}}

extern float DCL_runPID(PID *p, float rk, float yk, float lk);
extern float DCL_runPIDc(PID *p, float rk, float yk, float lk);


/*** PI controller ***/

typedef volatile struct {
	float Kp;		// [0] proportional gain
 	float Ki;		// [2] integral gain
 	float i10;		// [4] I storage
	float Umax;		// [6] upper saturation limit
	float Umin;		// [8] lower saturation limit
	float i6;		// [A] saturation storage
} PIcalc;
extern PIcalc OutInvUVCurrD,OutInvUVCurrQ,OutInvUVCurrRMS;
#define	PI_DEFAULTS { 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f }

extern float DCL_runPI(PIcalc *p, float rk, float yk);
extern float DCL_runPI_Group(PIcalc *p, float *in,float *out,int N);
extern float RunPIRE(PIcalc *p, float rk, float yk);
#define PLL_PI_CTRL_DEFAULT     { 180.0f, 3200.0f, PI2*POWERGRID_FREQ, 51.5*PI2, 48.5*PI2,1}
#define UDC_PI_CTRL_DEFAULT 	{ 0.7f, 0.01f, 0, 10.0f, -10.0f,1}
#define UDC_PI_BALANCE_DEFAULT 	{ 0.3f, 0.01f, 0, 5.0f, -5.0f,1}
#define UDC_PI_GRID_REACTIVE_DEFAULT    { 0.3f, 0.01f, 0, 0.5f, -0.5f,1}
#define VOLT_TARGET_CORR_DEFAULT        { 0.003f, 0.0001f, 0, 1.0f, 0.6f,1}
#define CURR_PI_INNER_DEFAULT 	{ 0.3f, 0.000f, 0, 1.0f, -1.0f,1}
#define OUTINVCURR_DEFAULT 		{ 0.3f, 0.100f, 0, 100.0f, -100.0f,1}
#define PI_Vol 					{ 0.075f, 0.025f, 0, 250, -250,1}

#define PI_Vol_UV               { 0.075f, 0.025f, 0, 30, 0,0}

#define PI_Cur 					{ 0.015f, 0.0001f, 0, 1.5, 0.7}
#define FFT_PI_CTRL_DEFAULT     {{ 0.015f, 0.0001f, 0, 1.5, 0.7},{ 0.015f, 0.0001f, 0, 1.5, 0.7},{ 0.015f, 0.0001f, 0, 1.5, 0.7}}

typedef volatile struct {
	float Kp;		// proportional gain
 	float Ki;		// integral gain
 	float Kd;
	float Umax;		// upper saturation limit
	float Umin;		// lower saturation limit
} PIInc;

/*** Direct Form 1 - 3rd order ***/

typedef volatile struct {
	// coefficients
	float b0;	// [0] b0
	float b1;	// [2] b1
	float b2;	// [4] b2
	float b3;	// [6] b3
	float a0;	// [8] a0
	float a1;	// [A] a1
	float a2;	// [C] a2
	float a3;	// [E] a3

	//data
	float d0;	// [10] e(k)
	float d1;	// [12] e(k-1)
	float d2;	// [14] e(k-2)
	float d3;	// [16] e(k-3)
	float d4;	// [18] u(k)
	float d5;	// [1A] u(k-1)
	float d6;	// [1C] u(k-2)
	float d7;	// [1E] u(k-3)
} DF13;

#define	DF13_DEFAULTS {	0.25f, 0.25f, 0.25f, 0.25f, 0.0f, 0.0f, 0.0f, 0.0f, \
						0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f 		\
						}

extern float DCL_runDF13(DF13 *p, float ek);
extern float DCL_runDF13i(DF13 *p, float ek, float vk);
extern float DCL_runDF13p(DF13 *p, float ek, float uk);
extern float DCL_runDF13c(DF13 *p, float ek);
extern float DCL_runDF13ic(DF13 *p, float ek, float vk);
extern float DCL_runDF13pc(DF13 *p, float ek, float uk);


/*** Direct Form 2 - 2nd order ***/

typedef volatile struct {
	float b0;	// [0] b0 num
	float b1;	// [2] b1
	float b2;	// [4] b2
	float a1;	// [6] a1 denum
	float a2;	// [8] a2
	float x1;	// [A] x1
	float x2;	// [C] x2
} DF22;

typedef volatile struct {
	float b0;	// [0] b0 num
	float b1;	// [2] b1
	float a1;	// [4] a1 denum
	float a2;	// [6] a2
	float x1;	// [8] x1
	float x2;	// [A] x2
} DF22BLOCK;

#define	DF22_DEFAULTS { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
extern void DCL_runDF22Block(DF22BLOCK *p, float *,float ek);
extern void DCL_runDF22Group(DF22 *p, float *in,float *out,int N);
extern float DCL_runDF22(DF22 *p, float ek);
extern float DCL_runDF22i(DF22 *p, float ek);
extern void DCL_runDF22p(DF22 *p, float ek, float uk);
extern float DCL_runDF22c(DF22 *p, float ek);
extern float DCL_runDF22ic(DF22 *p, float ek);
extern void DCL_runDF22pc(DF22 *p, float ek, float uk);

extern void MCUfilterMemCopy(DF22 *dst,int16 num);
/*** Direct Form 2 - 3rd order ***/

typedef volatile struct {
	float b0;	// [0] b0
	float b1;	// [2] b1
	float b2;	// [4] b2
	float b3;	// [6] b3
	float a1;	// [8] a1
	float a2;	// [A] a2
	float a3;	// [C] a3
	float x1;	// [E] x1
	float x2;	// [10] x2
	float x3;	// [12] x3
} DF23;

#define	DF23_DEFAULTS { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }

extern float DCL_runDF23(DF23 *p, float ek);
extern float DCL_runDF23i(DF23 *p, float ek);
extern void DCL_runDF23p(DF23 *p, float ek, float uk);
extern float DCL_runDF23c(DF23 *p, float ek);
extern float DCL_runDF23ic(DF23 *p, float ek);
extern void DCL_runDF23pc(DF23 *p, float ek, float uk);


#define PRCOEFF_ELEMENUMBK 6
#define PRCOEFF_ELEMENUM 7

#define SINGLE_PHASE_RECTPOWER_DISP_DEFAULT {\
		RECTPOWER_DISP_DEFAULT,RECTPOWER_DISP_DEFAULT,RECTPOWER_DISP_DEFAULT,RECTPOWER_DISP_DEFAULT,RECTPOWER_DISP_DEFAULT,RECTPOWER_DISP_DEFAULT}

#define FFT_MAG_ALL_DEFAULT {\
    {   FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,\
        FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,\
        FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT},\
    {   FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,\
        FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,\
        FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT},\
    {   FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,\
        FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,\
        FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT,FFT_MAG_DEFAULT} }

#define FFT_PHA_ALL_DEFAULT {\
    {   FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,\
        FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,\
        FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT},\
    {   FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,\
        FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,\
        FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT},\
    {   FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,\
        FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,\
        FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT,FFT_PHA_DEFAULT} }

#define FILTER_MCU_DEFAULT_NUM          16
#define GRIDVOLT_DEFAULT_NUM            0
#define GRID_VOLT_LOWDEFAULT_NUM        1
#define PLL_AGC_FILTER_DEFAULT_NUM      2
#define PLL_VOLT_Q_DEFAULT_NUM          3
#define BWFILTER_FREQ_DEFAULTS_NUM      4
#define RECTPOWER_DISP_DEFAULT_NUM      5

#define APFOUT_DEFAULT_NUM              6
#define APFOUT1_DEFAULT_NUM             7
#define RESONANCE_PORTECT_ABC_NUM       8
#define UDC_PNF_DEFAULT_NUM             9

#define FFT_MAG1_ALL_DEFAULT_NUM        10      //幅度滤波第一组,系数比较偏向快速
#define FFT_MAG2_ALL_DEFAULT_NUM        11      //幅度滤波第一组,系数比较偏向快速
#define FFT_MAG3_ALL_DEFAULT_NUM        12      //幅度滤波第一组,系数比较偏向快速
#define FFT_PHA1_ALL_DEFAULT_NUM        13
#define FFT_PHA2_ALL_DEFAULT_NUM        14
#define FFT_PHA3_ALL_DEFAULT_NUM        15

#define ALL_FILTERCOFF_16KHZ    {\
                                GRID_VOLT_DEFAULT16KHZ,\
                                GRID_VOLT_LOWDEFAULT16KHZ,\
                                PLL_AGC_FILTER_DEFAULT16KHZ,\
                                PLL_VOLT_Q_DEFAULT16KHZ,\
                                BWFILTER_FREQ_DEFAULTS16KHZ,\
                                RECTPOWER_DISP_DEFAULT16KHZ,\
                                APFOUT_DEFAULT16KHZ,\
                                APFOUT1_DEFAULT16KHZ,\
                                RESONANCE_PORTECT_ABC16KHZ,\
                                UDC_PNF_DEFAULT16KHZ,\
                                FFT_MAG1_DEFAULT16KHZ,\
                                FFT_MAG2_DEFAULT16KHZ,\
                                FFT_MAG3_DEFAULT16KHZ,\
                                FFT_PHA1_DEFAULT16KHZ,\
                                FFT_PHA2_DEFAULT16KHZ,\
                                FFT_PHA3_DEFAULT16KHZ}
#define ALL_FILTERCOFF_12p8KHZ  {\
                                GRID_VOLT_DEFAULT12p8KHZ,\
                                GRID_VOLT_LOWDEFAULT12p8KHZ,\
                                PLL_AGC_FILTER_DEFAULT12p8KHZ,\
                                PLL_VOLT_Q_DEFAULT12p8KHZ,\
                                BWFILTER_FREQ_DEFAULTS12p8KHZ,\
                                RECTPOWER_DISP_DEFAULT12p8KHZ,\
                                APFOUT_DEFAULT12p8KHZ,\
                                APFOUT1_DEFAULT12p8KHZ,\
                                RESONANCE_PORTECT_ABC12p8KHZ,\
                                UDC_PNF_DEFAULT12p8KHZ,\
                                FFT_MAG1_DEFAULT12p8KHZ,\
                                FFT_MAG2_DEFAULT12p8KHZ,\
                                FFT_MAG3_DEFAULT12p8KHZ,\
                                FFT_PHA1_DEFAULT12p8KHZ,\
                                FFT_PHA2_DEFAULT12p8KHZ,\
                                FFT_PHA3_DEFAULT12p8KHZ}
#define ALL_FILTERCOFF_9p6KHZ   {\
                                GRID_VOLT_DEFAULT9p6KHZ,\
                                GRID_VOLT_LOWDEFAULT9p6KHZ,\
                                PLL_AGC_FILTER_DEFAULT9p6KHZ,\
                                PLL_VOLT_Q_DEFAULT9p6KHZ,\
                                BWFILTER_FREQ_DEFAULTS9p6KHZ,\
                                RECTPOWER_DISP_DEFAULT9p6KHZ,\
                                APFOUT_DEFAULT9p6KHZ,\
                                APFOUT1_DEFAULT9p6KHZ,\
                                RESONANCE_PORTECT_ABC9p6KHZ,\
                                UDC_PNF_DEFAULT9p6KHZ,\
                                FFT_MAG1_DEFAULT9p6KHZ,\
                                FFT_MAG2_DEFAULT9p6KHZ,\
                                FFT_MAG3_DEFAULT9p6KHZ,\
                                FFT_PHA1_DEFAULT9p6KHZ,\
                                FFT_PHA2_DEFAULT9p6KHZ,\
                                FFT_PHA3_DEFAULT9p6KHZ}
//#define ALL_FILTERCOFF_8KHZ     {\
//                                GRID_VOLT_DEFAULT8KHZ,GRID_VOLT_LOWDEFAULT8KHZ,PLL_AGC_FILTER_DEFAULT8KHZ,PLL_VOLT_Q_DEFAULT8KHZ,\
//                                BWFILTER_FREQ_DEFAULTS8KHZ,RECTPOWER_DISP_DEFAULT8KHZ,APFOUT_DEFAULT8KHZ,APFOUT1_DEFAULT8KHZ,RESONANCE_PORTECT_ABC8KHZ,UDC_PNF_DEFAULT8KHZ,FFT_MAG1_DEFAULT8KHZ,FFT_MAG2_DEFAULT8KHZ,FFT_MAG3_DEFAULT8KHZ,\
//                                FFT_PHA1_DEFAULT8KHZ,FFT_PHA2_DEFAULT8KHZ,FFT_PHA3_DEFAULT8KHZ}

//cla filer coefficient

//#define CLA_FILTERCOFF_16KHZ    {}
//#define CLA_FILTERCOFF_12p8KHZ  {}
//#define CLA_FILTERCOFF_9p6KHZ   {}
//#define CLA_FILTERCOFF_8KHZ   {}



//matlab cmd1:Coeff=[G(1)*SOS(1:3),G(2)*SOS(5:6),0,0];
//matlab cmd2:fprintf('#define \t{%7.7ff,\t%7.7ff,\t%7.7ff,\t%7.7ff,\t%7.7ff,\t0.0f,\t0.0f}\t//%7.7f\n',Coeff(1),Coeff(2),Coeff(3),Coeff(4),Coeff(5),roundn(G(1),-6))

//filter common parameter
//ActivePower display Use  2 order butterworth filter,corner frequency 25Hz,sample frequency 6.4kHz
#define RECTPOWER_DISP_DEFAULT12p8KHZ 	{0.0001480f,    0.0002960f,     0.0001480f,     -1.9652934f,    0.9658855f, 0.0f,   0.0f }  //1.48e-04
#define RECTPOWER_DISP_DEFAULT16KHZ     RECTPOWER_DISP_DEFAULT12p8KHZ
#define RECTPOWER_DISP_DEFAULT9p6KHZ    RECTPOWER_DISP_DEFAULT12p8KHZ

//Udcp,UdcN Use  2 order butterworth filter,corner frequency 25Hz,sample frequency 6.4kHz
#define UDC_PNF_DEFAULT12p8KHZ 		    {0.0001480f,	0.0002960f,	    0.0001480f,	    -1.9652934f,	0.9658855f,	0.0f,	0.0f}	//1.48e-04
//#define UDC_PNF_DEFAULT16KHZ            UDC_PNF_DEFAULT12p8KHZ
#define UDC_PNF_DEFAULT9p6KHZ           UDC_PNF_DEFAULT12p8KHZ
#define UDC_BALANCE_DEFAULT12p8KHZ      UDC_PNF_DEFAULT12p8KHZ
#define UDC_BALANCE_DEFAULT16KHZ        UDC_PNF_DEFAULT12p8KHZ
#define UDC_BALANCE_DEFAULT9p6KHZ       UDC_PNF_DEFAULT12p8KHZ

//FFT filter display Use  2 order butterworth filter,corner frequency 4Hz,sample frequency 533.33kHz
#define  FFT_MAG1_DEFAULT12p8KHZ        {0.0005372f,    0.0010745f, 0.0005372f, -1.9333761f,    0.9355250f, 0.0f,   0.0f}   //0.0005370,CF:533.3Hz delay:30p
#define  FFT_MAG2_DEFAULT12p8KHZ        FFT_MAG1_DEFAULT12p8KHZ
#define  FFT_MAG3_DEFAULT12p8KHZ        FFT_MAG1_DEFAULT12p8KHZ
#define  FFT_MAG1_DEFAULT9p6KHZ         FFT_MAG1_DEFAULT12p8KHZ
#define  FFT_MAG2_DEFAULT9p6KHZ         FFT_MAG1_DEFAULT12p8KHZ
#define  FFT_MAG3_DEFAULT9p6KHZ         FFT_MAG1_DEFAULT12p8KHZ
#define  FFT_MAG1_DEFAULT16KHZ          FFT_MAG1_DEFAULT12p8KHZ
#define  FFT_MAG2_DEFAULT16KHZ          FFT_MAG1_DEFAULT12p8KHZ
#define  FFT_MAG3_DEFAULT16KHZ          FFT_MAG1_DEFAULT12p8KHZ
#define  FFT_MAG_DEFAULT                FFT_MAG1_DEFAULT12p8KHZ

#define  FFT_PHA1_DEFAULT12p8KHZ        {0.0005372f,    0.0010745f, 0.0005372f, -1.9333761f,    0.9355250f, 0.0f,   0.0f}   //0.0005370
#define  FFT_PHA2_DEFAULT12p8KHZ        FFT_PHA1_DEFAULT12p8KHZ
#define  FFT_PHA3_DEFAULT12p8KHZ        FFT_PHA1_DEFAULT12p8KHZ
#define  FFT_PHA1_DEFAULT9p6KHZ         FFT_PHA1_DEFAULT12p8KHZ
#define  FFT_PHA2_DEFAULT9p6KHZ         FFT_PHA1_DEFAULT12p8KHZ
#define  FFT_PHA3_DEFAULT9p6KHZ         FFT_PHA1_DEFAULT12p8KHZ
#define  FFT_PHA1_DEFAULT16KHZ          FFT_PHA1_DEFAULT12p8KHZ
#define  FFT_PHA2_DEFAULT16KHZ          FFT_PHA1_DEFAULT12p8KHZ
#define  FFT_PHA3_DEFAULT16KHZ          FFT_PHA1_DEFAULT12p8KHZ
#define  FFT_PHA_DEFAULT                FFT_PHA1_DEFAULT12p8KHZ


//-PWM_FREQUENCE_9p6KHZ
//ActivePower display Use  2 order butterworth filter,corner frequency 25Hz,sample frequency 10kHz
#define RECTPOWER_DISP_DEFAULT          {0.0000610f,    0.0001220f, 0.0000610f, -1.9777865f,    0.9780305f, 0.0f,   0.0f}   //0.0000610
//Use  2 order butterworth filter,corner frequency is show at the end of each line,sample frequency is 19.2kHz
#define PLL_VOLT_Q_DEFAULT9p6KHZ        { 0.0000166f,   0.0000333f,     0.0000166f,     -1.9884301f,    0.9884967f, 0.0f,   0.0f}  //1.660e-05,cf:25Hz,H1delay:50p
#define PLL_AGC_FILTER_DEFAULT9p6KHZ    PLL_VOLT_Q_DEFAULT9p6KHZ
#define BWFILTER_FREQ_DEFAULTS9p6KHZ    PLL_VOLT_Q_DEFAULT9p6KHZ
#define GRID_VOLT_LOWDEFAULT9p6KHZ      {0.3123263f,    0.6246527f,     0.3123263f,     0.0766925f,     0.1726128f, 0.0f,   0.0f}  //0.3123263,cf:5kHz,H1delay:0.662p
//#define GRID_VOLT_LOWDEFAULT9p6KHZ    {0.220195f,     0.440389f,      0.220195f,      -0.307566f,     0.188345f,  0.0f,   0.0f}  //0.220195,4kHz,delay:
#define GRID_VOLT_DEFAULT9p6KHZ         {0.1043498f,    0.2086996f,     0.1043498f,     -0.9017822f,    0.3191813f, 0.0f,   0.0f}  //0.1043498,cf:52.5kHz,H1delay::1.63p
//#define GRID_VOLT_DEFAULT9p6KHZ       {0.0216207f,    0.0432414f,     0.0216207f,     -1.5431211f,    0.6296040f, 0.0f,   0.0f}  //0.0216207,1kHz delay:5
//#define GRID_VOLT_DEFAULT9p6KHZ       {0.0112544f,    0.0225088f,     0.0112544f,     -1.6782777f,    0.7232954f, 0.0f,   0.0f}  //0.0112544,700Hz delay:7
//#define GRID_VOLT_DEFAULT9p6KHZ       {0.0010232f,    0.0020464f,     0.0010232f,     -1.9075016f,    0.9115945f, 0.0f,   0.0f}  //0.0010232,200Hz delay:23
//#define APFOUT_DEFAULT                {0.0010232f,    0.0020464f,     0.0010232f,     -1.9075016f,    0.9115945f, 0.0f,   0.0f}  //0.0010232,200Hz delay:23
#define APFOUT_DEFAULT9p6KHZ            {0.0112544f,    0.0225088f,     0.0112544f,     -1.6782777f,    0.7232954f, 0.0f,   0.0f}  //0.0112544,700Hz delay:7
#define APFOUT1_DEFAULT9p6KHZ           {0.4181633f,    0.8363267f,     0.4181633f,     0.4629380f,     0.2097154f, 0.0f,   0.0f}  //0.4181630,6000Hz delay:0.47
//RESONANCE_PORTECT_ABC Use second-order sections band-pass butterworth filter,corner frequency 2.5kHz,4.1kHz,sample frequency 19.2kHz
#define RESONANCE_PORTECT_ABC9p6KHZ     {0.2113249f,    0.0000000f,     -0.2113249f,    -0.7697876f,    0.5773503f, 0.0f,  0.0f}  //0.2113250,cf:2.5kHz~4.1kHz


//PWM_FREQUENCE_12p8KHZ
//PLL Use  2 order butterworth filter,corner frequency 25Hz,sample frequency 25.6kHz
#define PLL_VOLT_Q_DEFAULT12p8KHZ		{0.0000094f,	0.0000187f,	    0.0000094f,	    -1.9913225f,	0.99136f,	0.0f,	0.0f}  //9.40e-06,cf:25Hz,H1delay:67.79p
#define PLL_AGC_FILTER_DEFAULT12p8KHZ   PLL_VOLT_Q_DEFAULT12p8KHZ
#define BWFILTER_FREQ_DEFAULTS12p8KHZ   PLL_VOLT_Q_DEFAULT12p8KHZ
//#define GRID_VOLT_LOWDEFAULT	        {0.264713f,	    0.529425f,	    0.264713f,	    -0.115064f,	    0.173914f,	0.0f,	0.0f}	//0.264713 6kHz
#define GRID_VOLT_LOWDEFAULT12p8KHZ	    {0.199040f,	    0.398080f,	    0.199040f,	    -0.404485f,	    0.200644f,	0.0f,	0.0f}	//0.199040,cf:5kHz,H1delay:1.00p
//#define GRID_VOLT_LOWDEFAULT	        {0.139939f,	    0.279879f,	    0.139939f,	    -0.699738f,	    0.259495f,	0.0f,	0.0f}	//0.139939 4kHz
//#define GRID_VOLT_DEFAULT12p8KHZ 	    {0.0648169f,	0.1296338f,	    0.0648169f,	    -1.1620371f,	0.4213046f,	0.0f,	0.0f}	//0.0648169,cf:2.5kHz,H1delay:2.23p
#define GRID_VOLT_DEFAULT12p8KHZ        {0.7309950f,    1.4619901f,     0.7309950f,      1.3882676f,    0.5357125f, 0.0f,   0.0f}   //11K
//#define GRID_VOLT_DEFAULT 		    {0.0127873f,	0.0255747f,	    0.0127873f,	    -1.6556077f,	0.7067571f,	0.0f,	0.0f}	//0.0127873,1kHz
//#define GRID_VOLT_DEFAULT 	        {0.0065673f,  	0.0131345f,     0.0065673f,     -1.75803f,  	0.7842991f, 0.0f,	0.0f} 	//0.0065673,700Hz
//#define APFOUT_DEFAULT12p8KHZ           {0.7309950f,    1.4619901f,     0.7309950f,      1.3882676f,    0.5357125f, 0.0f,   0.0f}   //11K  //0.0065673,cf:700Hz,H1delay:8.25p
#define APFOUT_DEFAULT12p8KHZ           {0.0065673f,    0.0131345f,     0.0065673f,     -1.75803f,      0.7842991f, 0.0f,   0.0f}   //0.0065673,cf:700Hz,H1delay:8.25p
#define APFOUT1_DEFAULT12p8KHZ          {0.264713f,     0.529425f,      0.264713f,      -0.115064f,     0.173914f,  0.0f,   0.0f}   //0.264713,cf:6000Hz,H1delay:0.78p
//RESONANCE_PORTECT_ABC Use second-order sections band-pass butterworth filter,corner frequency 3kHz,5.5kHz,sample frequency 25.6kHz
#define RESONANCE_PORTECT_ABC12p8KHZ	{0.2405824f,    0.0000000f,     -0.2405824f,    -0.8022522f,    0.5188352f, 0.0f,   0.0f}   //0.2405820,cf:3kHz~5.5kHz


//PWM_FREQUENCE_16KHZ
//PLL Use  2 order butterworth filter,corner frequency 25Hz,sample frequency 32kHz
//#define PLL_VOLT_Q_DEFAULT16KHZ		    {0.0000060f,    0.0000120f,     0.0000060f,     -1.9930580f,    0.9930820f, 0.0f,   0.0f}   //0.0000060,cf:25Hz,H1delay:81.93p
//#define PLL_AGC_FILTER_DEFAULT16KHZ 	PLL_VOLT_Q_DEFAULT16KHZ
//#define BWFILTER_FREQ_DEFAULTS16KHZ	    PLL_VOLT_Q_DEFAULT16KHZ
#define PLL_VOLT_Q_DEFAULT16KHZ         {0.000015f, 0.000031f, 0.000015f, -1.988893f, 0.988954f, 0, 0} //0.000015,corner frequency 25Hz,sample frequency 10kHz
#define PLL_AGC_FILTER_DEFAULT16KHZ 	PLL_VOLT_Q_DEFAULT16KHZ
#define BWFILTER_FREQ_DEFAULTS16KHZ	    PLL_VOLT_Q_DEFAULT16KHZ
//#define GRID_VOLT_LOWDEFAULT16KHZ	    {0.1866943f,    0.3733887f,     0.1866943f,     -0.4629380f,    0.2097154f, 0.0f,   0.0f}   //0.1866940,cf:6kHz,H1delay:1.06p
#define GRID_VOLT_LOWDEFAULT16KHZ	    {0.1399393f,    0.2798786f,     0.1399393f,     -0.6997380f,    0.2594952f, 0.0f,   0.0f}   //0.1399390,cf:5kHz,H1delay:1.32p
//#define GRID_VOLT_LOWDEFAULT16KHZ	    {0.0976311f,    0.1952621f,     0.0976311f,     -0.9428090f,    0.3333333f, 0.0f,   0.0f}   //0.0976310,cf:4kHz,H1delay:1.71p
//#define	GRID_VOLT_DEFAULT16KHZ 	    {0.0442797f,    0.0885594f,     0.0442797f,     -1.3228874f,    0.5000062f, 0.0f,   0.0f}   //0.0442800,cf:2.5kHz,H1delay:2.82p
//#define GRID_VOLT_DEFAULT16KHZ          {0.1866943f,    0.3733887f,     0.1866943f,     -0.4629380f,    0.2097154f, 0.0f,   0.0f}   //0.1866940,cf:6kHz,H1delay:1.06p
//#define GRID_VOLT_DEFAULT16KHZ          {0.292893f, 0.585786f, 0.292893f, -0.000000f, 0.171573f, 0, 0} //0.292893,5kHz,20KHZ
#define GRID_VOLT_DEFAULT16KHZ          {0.800592f, 1.601185f, 0.800592f, 1.561018f, 0.641352f, 0, 0} //0.800592,9kHz ,at sample20kHZ
//#define GRID_VOLT_DEFAULT16KHZ 	    {0.0084427f,    0.0168854f,     0.0084427f,     -1.7237762f,    0.7575469f, 0.0f,   0.0f}   //0.0084430,cf:1kHz,H1delay:7.20p
//#define GRID_VOLT_DEFAULT16KHZ 		{0.0042988f,    0.0085977f,     0.0042988f,     -1.8061542f,    0.8233496f, 0.0f,   0.0f}   //0.0042990,cf:700kHz,H1delay:10.33p
#define APFOUT_DEFAULT16KHZ             {0.0042988f,    0.0085977f,     0.0042988f,     -1.8061542f,    0.8233496f, 0.0f,   0.0f}   //0.0042990,cf:700kHz,H1delay:10.33p
//#define APFOUT1_DEFAULT16KHZ            {0.1866943f,    0.3733887f,     0.1866943f,     -0.4629380f,    0.2097154f, 0.0f,   0.0f}   //0.1866940,cf:6kHz,H1delay:1.06p
#define APFOUT1_DEFAULT16KHZ            {0.800592f, 1.601185f, 0.800592f, 1.561018f, 0.641352f, 0, 0} //0.800592,9kHz ,at sample20kHZ
#define UDC_PNF_DEFAULT16KHZ            {0.010432f, 0.020865f, 0.010432f, -1.690996f, 0.732726f, 0, 0} //0.010432,700Hz,20KHZ
//RESONANCE_PORTECT_ABC Use second-order sections band-pass butterworth filter,corner frequency 3.7kHz,6.9kHz,sample frequency 32kHz
#define RESONANCE_PORTECT_ABC16KHZ	    {0.2452373f,    0.0000000f,     -0.2452373f,    -0.8025839f,    0.5095254f, 0.0f,   0.0f}   //0.2452370,cf:3.7kHz~6.9kHz
//extern DF22 ApfOutA;
//extern DF22 ApfOutB;
//extern DF22 ApfOutC;
//extern DF22 ApfOut1A;
//extern DF22 ApfOut1B;
//extern DF22 ApfOut1C;
extern DF22 VolAndInvCurrFilter[];
extern DF22 ResonancePortectABC[];
extern DF22 UdcUpFilter;
extern DF22 UdcDnFilter;
extern DF22 gridVDForwardFilter;
extern DF22 PLLVoltQFilter;
//extern DF22 gridVoltFilterA;
//extern DF22 gridVoltFilterB;
//extern DF22 gridVoltFilterC;
//extern DF22 gridVoltFilterLowA;
//extern DF22 gridVoltFilterLowB;
//extern DF22 gridVoltFilterLowC;
//extern DF22 gridVoltPosiDFilter;
//extern DF22 gridVoltPosiQFilter;
//extern DF22 gridVoltNegDFilter;
//extern DF22 gridVoltNegQFilter;
extern DF22 bwForFreq;
extern DF22 RectPowerDispFilter;
extern DF22 ActivePowerDispFilter;
//extern DF22 PLLAGCFilter;
extern DF22 SinglePhaseDispFilter[];
extern DF22 fftmag[3][CALIBRATION_COEFF_LEN];
extern DF22 fftpha[3][CALIBRATION_COEFF_LEN];
extern const DF22 AllFilterCoeff[3][FILTER_MCU_DEFAULT_NUM];
//extern const DCL_DF22_CLA   CLAFilterCoeff[3][4];

//extern PIcalc PLLPICtrl;
extern PIcalc UdcPICtrl;
extern PIcalc UdcPIBalance;
extern PIcalc PICurrInnerA,PICurrInnerB,PICurrInnerC;
extern PIInc PIVol;
extern PIcalc PIFanCtl;
extern PIcalc PIVolA,PIVolB,PIVolC;
extern PIcalc PIIvrA,PIIvrB,PIIvrC,PIIvrQ,PIIvrNQ,PIIvrND;
extern PIcalc FFTPICtrl[2][CALIBRATION_COEFF_LEN];
extern PIcalc GridCurrPICtrlA,GridCurrPICtrlB,GridCurrPICtrlC;
extern PIcalc VolttargetCorrPIA,VolttargetCorrPIB,VolttargetCorrPIC;

typedef volatile struct {
    PIcalc CorrPI;
    float VolttargetCorr;
    float RMS_CONSTANT_CURRENT_OVERLOAD;
    float RMS_CONSTANT_CURRENT_DIFF;
    float RMS_CONSTANT_CURRENT_RATED;
    float INS_CONSTANT_CURRENT_RATED;
    int state;
    int Prvstate;
    int CNT1;
//    int *CurTarget;

}SMconstantCurr;
extern SMconstantCurr ConstantCurr[3];
extern const SMconstantCurr ConstantCurrDefault[3];
#define SM_CONSTANT_CURR_INS 3
#define SM_CONSTANT_CURR_CONSTANT 2
#define SM_CONSTANT_CURR_NORMAL 1
#define SM_CONSTANT_CURR_STANDBY 0
#if PR_CTRL==1
//extern DF22 PRCtrlA[(PRCTRL_COEFF_LEN)],PRCtrlB[(PRCTRL_COEFF_LEN)],PRCtrlC[(PRCTRL_COEFF_LEN)];
extern DF22BLOCK PRCtrlABK[(PRCTRL_COEFF_LEN)],PRCtrlBBK[(PRCTRL_COEFF_LEN)],PRCtrlCBK[(PRCTRL_COEFF_LEN)];

//This is PR ctrl ccs 16.00kHz @50.00Hz @2.400 @1000.00Mag coefficient.
#define PRCTRL_BLOCK_DEFAULT16KHZ { \
0.942478,-0.942296,-1.998673,0.999058,0.000000,0.000000,\
0.471239,-0.470876,-1.997517,0.999058,0.000000,0.000000,\
0.314159,-0.313615,-1.995591,0.999058,0.000000,0.000000,\
0.235619,-0.234893,-1.992896,0.999058,0.000000,0.000000,\
0.188496,-0.187588,-1.989432,0.999058,0.000000,0.000000,\
0.157080,-0.155991,-1.985201,0.999058,0.000000,0.000000,\
0.134640,-0.133370,-1.980206,0.999058,0.000000,0.000000,\
0.117810,-0.116360,-1.974446,0.999058,0.000000,0.000000,\
0.104720,-0.103089,-1.967926,0.999058,0.000000,0.000000,\
0.094248,-0.092437,-1.960647,0.999058,0.000000,0.000000,\
0.085680,-0.083690,-1.952612,0.999058,0.000000,0.000000,\
0.078540,-0.076370,-1.943824,0.999058,0.000000,0.000000,\
0.072498,-0.070150,-1.934287,0.999058,0.000000,0.000000,\
0.067320,-0.064793,-1.924004,0.999058,0.000000,0.000000,\
0.062832,-0.060127,-1.912979,0.999058,0.000000,0.000000,\
0.058905,-0.056023,-1.901217,0.999058,0.000000,0.000000,\
0.055440,-0.052381,-1.888722,0.999058,0.000000,0.000000,\
0.052360,-0.049125,-1.875499,0.999058,0.000000,0.000000,\
0.049604,-0.046193,-1.861553,0.999058,0.000000,0.000000,\
0.047124,-0.043538,-1.846889,0.999058,0.000000,0.000000,\
0.044880,-0.041120,-1.831513,0.999058,0.000000,0.000000,\
0.042840,-0.038906,-1.815431,0.999058,0.000000,0.000000,\
0.040977,-0.036871,-1.798649,0.999058,0.000000,0.000000,\
0.039270,-0.034991,-1.781174,0.999058,0.000000,0.000000,\
0.037699,-0.033249,-1.763012,0.999058,0.000000,0.000000,\
0.036249,-0.031629,-1.744170,0.999058,0.000000,0.000000,\
0.034907,-0.030117,-1.724656,0.999058,0.000000,0.000000,\
0.033660,-0.028701,-1.704477,0.999058,0.000000,0.000000,\
0.032499,-0.027373,-1.683641,0.999058,0.000000,0.000000,\
0.031416,-0.026123,-1.662156,0.999058,0.000000,0.000000,\
0.030403,-0.024944,-1.640030,0.999058,0.000000,0.000000,\
0.029452,-0.023829,-1.617272,0.999058,0.000000,0.000000,\
0.028560,-0.022773,-1.593890,0.999058,0.000000,0.000000,\
0.027720,-0.021771,-1.569894,0.999058,0.000000,0.000000,\
0.026928,-0.020817,-1.545293,0.999058,0.000000,0.000000,\
0.026180,-0.019909,-1.520096,0.999058,0.000000,0.000000,\
0.025472,-0.019043,-1.494313,0.999058,0.000000,0.000000,\
0.024802,-0.018215,-1.467953,0.999058,0.000000,0.000000,\
0.024166,-0.017422,-1.441028,0.999058,0.000000,0.000000,\
0.023562,-0.016663,-1.413547,0.999058,0.000000,0.000000,\
0.022987,-0.015934,-1.385522,0.999058,0.000000,0.000000}
#define PRCTRL_BLOCK_FULC16KHZ { \
0.000007,0.000015,\
0.000015,0.000062,\
0.000022,0.000139,\
0.000029,0.000246,\
0.000036,0.000385,\
0.000043,0.000554,\
0.000051,0.000753,\
0.000058,0.000982,\
0.000065,0.001242,\
0.000072,0.001532,\
0.000079,0.001851,\
0.000086,0.002199,\
0.000093,0.002577,\
0.000100,0.002983,\
0.000107,0.003418,\
0.000114,0.003881,\
0.000121,0.004372,\
0.000128,0.004891,\
0.000135,0.005436,\
0.000142,0.006008,\
0.000148,0.006607,\
0.000155,0.007230,\
0.000161,0.007880,\
0.000168,0.008553,\
0.000174,0.009251,\
0.000181,0.009973,\
0.000187,0.010718,\
0.000193,0.011485,\
0.000199,0.012274,\
0.000206,0.013084,\
0.000212,0.013915,\
0.000217,0.014765,\
0.000223,0.015635,\
0.000229,0.016524,\
0.000235,0.017430,\
0.000240,0.018353,\
0.000246,0.019293,\
0.000251,0.020249,\
0.000256,0.021219,\
0.000262,0.022203,\
0.000267,0.023201}

//This is PR ctrl ccs 19.20kHz @50.00Hz @2.400 @1000.00Mag coefficient.
#define PRCTRL_BLOCK_DEFAULT9p6KHZ { \
0.785398,-0.785293,-1.998947,0.999215,0.000000,0.000000,\
0.392699,-0.392489,-1.998145,0.999215,0.000000,0.000000,\
0.261799,-0.261484,-1.996807,0.999215,0.000000,0.000000,\
0.196350,-0.195929,-1.994934,0.999215,0.000000,0.000000,\
0.157080,-0.156554,-1.992528,0.999215,0.000000,0.000000,\
0.130900,-0.130270,-1.989588,0.999215,0.000000,0.000000,\
0.112200,-0.111465,-1.986116,0.999215,0.000000,0.000000,\
0.098175,-0.097335,-1.982111,0.999215,0.000000,0.000000,\
0.087266,-0.086322,-1.977576,0.999215,0.000000,0.000000,\
0.078540,-0.077491,-1.972512,0.999215,0.000000,0.000000,\
0.071400,-0.070247,-1.966920,0.999215,0.000000,0.000000,\
0.065450,-0.064193,-1.960801,0.999215,0.000000,0.000000,\
0.060415,-0.059054,-1.954157,0.999215,0.000000,0.000000,\
0.056100,-0.054635,-1.946989,0.999215,0.000000,0.000000,\
0.052360,-0.050791,-1.939301,0.999215,0.000000,0.000000,\
0.049087,-0.047415,-1.931093,0.999215,0.000000,0.000000,\
0.046200,-0.044425,-1.922369,0.999215,0.000000,0.000000,\
0.043633,-0.041755,-1.913129,0.999215,0.000000,0.000000,\
0.041337,-0.039356,-1.903378,0.999215,0.000000,0.000000,\
0.039270,-0.037186,-1.893117,0.999215,0.000000,0.000000,\
0.037400,-0.035214,-1.882349,0.999215,0.000000,0.000000,\
0.035700,-0.033412,-1.871077,0.999215,0.000000,0.000000,\
0.034148,-0.031759,-1.859304,0.999215,0.000000,0.000000,\
0.032725,-0.030235,-1.847034,0.999215,0.000000,0.000000,\
0.031416,-0.028825,-1.834269,0.999215,0.000000,0.000000,\
0.030208,-0.027516,-1.821013,0.999215,0.000000,0.000000,\
0.029089,-0.026297,-1.807269,0.999215,0.000000,0.000000,\
0.028050,-0.025158,-1.793041,0.999215,0.000000,0.000000,\
0.027083,-0.024091,-1.778334,0.999215,0.000000,0.000000,\
0.026180,-0.023089,-1.763150,0.999215,0.000000,0.000000,\
0.025335,-0.022146,-1.747494,0.999215,0.000000,0.000000,\
0.024544,-0.021256,-1.731371,0.999215,0.000000,0.000000,\
0.023800,-0.020415,-1.714784,0.999215,0.000000,0.000000,\
0.023100,-0.019617,-1.697738,0.999215,0.000000,0.000000,\
0.022440,-0.018861,-1.680237,0.999215,0.000000,0.000000,\
0.021817,-0.018141,-1.662286,0.999215,0.000000,0.000000,\
0.021227,-0.017455,-1.643891,0.999215,0.000000,0.000000,\
0.020668,-0.016801,-1.625055,0.999215,0.000000,0.000000,\
0.020138,-0.016176,-1.605784,0.999215,0.000000,0.000000,\
0.019635,-0.015579,-1.586084,0.999215,0.000000,0.000000,\
0.019156,-0.015006,-1.565959,0.999215,0.000000,0.000000}
#define PRCTRL_BLOCK_FULC9p6KHZ { \
0.000004,0.000011,\
0.000008,0.000043,\
0.000013,0.000096,\
0.000017,0.000171,\
0.000021,0.000267,\
0.000025,0.000385,\
0.000029,0.000523,\
0.000034,0.000683,\
0.000038,0.000864,\
0.000042,0.001066,\
0.000046,0.001288,\
0.000050,0.001532,\
0.000054,0.001796,\
0.000058,0.002080,\
0.000062,0.002385,\
0.000067,0.002709,\
0.000071,0.003054,\
0.000075,0.003418,\
0.000079,0.003803,\
0.000083,0.004206,\
0.000087,0.004629,\
0.000091,0.005070,\
0.000094,0.005530,\
0.000098,0.006009,\
0.000102,0.006506,\
0.000106,0.007020,\
0.000110,0.007552,\
0.000114,0.008102,\
0.000117,0.008669,\
0.000121,0.009252,\
0.000125,0.009852,\
0.000128,0.010468,\
0.000132,0.011099,\
0.000136,0.011746,\
0.000139,0.012408,\
0.000143,0.013085,\
0.000146,0.013776,\
0.000150,0.014481,\
0.000153,0.015199,\
0.000156,0.015931,\
0.000160,0.016675}

//This is PR ctrl ccs 25.60kHz @50.00Hz @2.400 @1000.00Mag coefficient.
#define PRCTRL_BLOCK_DEFAULT12p8KHZ { \
0.589049,-0.589004,-1.999261,0.999411,0.000000,0.000000,\
0.294524,-0.294436,-1.998809,0.999411,0.000000,0.000000,\
0.196350,-0.196217,-1.998056,0.999411,0.000000,0.000000,\
0.147262,-0.147085,-1.997003,0.999411,0.000000,0.000000,\
0.117810,-0.117588,-1.995648,0.999411,0.000000,0.000000,\
0.098175,-0.097909,-1.993994,0.999411,0.000000,0.000000,\
0.084150,-0.083840,-1.992039,0.999411,0.000000,0.000000,\
0.073631,-0.073277,-1.989783,0.999411,0.000000,0.000000,\
0.065450,-0.065051,-1.987229,0.999411,0.000000,0.000000,\
0.058905,-0.058462,-1.984375,0.999411,0.000000,0.000000,\
0.053550,-0.053063,-1.981222,0.999411,0.000000,0.000000,\
0.049087,-0.048556,-1.977771,0.999411,0.000000,0.000000,\
0.045311,-0.044736,-1.974021,0.999411,0.000000,0.000000,\
0.042075,-0.041456,-1.969975,0.999411,0.000000,0.000000,\
0.039270,-0.038607,-1.965632,0.999411,0.000000,0.000000,\
0.036816,-0.036108,-1.960993,0.999411,0.000000,0.000000,\
0.034650,-0.033899,-1.956059,0.999411,0.000000,0.000000,\
0.032725,-0.031930,-1.950830,0.999411,0.000000,0.000000,\
0.031003,-0.030164,-1.945307,0.999411,0.000000,0.000000,\
0.029452,-0.028570,-1.939491,0.999411,0.000000,0.000000,\
0.028050,-0.027124,-1.933384,0.999411,0.000000,0.000000,\
0.026775,-0.025805,-1.926985,0.999411,0.000000,0.000000,\
0.025611,-0.024598,-1.920295,0.999411,0.000000,0.000000,\
0.024544,-0.023487,-1.913317,0.999411,0.000000,0.000000,\
0.023562,-0.022462,-1.906051,0.999411,0.000000,0.000000,\
0.022656,-0.021512,-1.898497,0.999411,0.000000,0.000000,\
0.021817,-0.020630,-1.890658,0.999411,0.000000,0.000000,\
0.021037,-0.019808,-1.882534,0.999411,0.000000,0.000000,\
0.020312,-0.019039,-1.874126,0.999411,0.000000,0.000000,\
0.019635,-0.018320,-1.865436,0.999411,0.000000,0.000000,\
0.019002,-0.017643,-1.856465,0.999411,0.000000,0.000000,\
0.018408,-0.017007,-1.847215,0.999411,0.000000,0.000000,\
0.017850,-0.016406,-1.837686,0.999411,0.000000,0.000000,\
0.017325,-0.015839,-1.827881,0.999411,0.000000,0.000000,\
0.016830,-0.015302,-1.817801,0.999411,0.000000,0.000000,\
0.016362,-0.014792,-1.807446,0.999411,0.000000,0.000000,\
0.015920,-0.014307,-1.796820,0.999411,0.000000,0.000000,\
0.015501,-0.013846,-1.785923,0.999411,0.000000,0.000000,\
0.015104,-0.013407,-1.774757,0.999411,0.000000,0.000000,\
0.014726,-0.012988,-1.763323,0.999411,0.000000,0.000000,\
0.014367,-0.012587,-1.751624,0.999411,0.000000,0.000000}
#define PRCTRL_BLOCK_FULC12p8KHZ { \
0.000002,0.000006,\
0.000004,0.000024,\
0.000005,0.000054,\
0.000007,0.000096,\
0.000009,0.000150,\
0.000011,0.000217,\
0.000012,0.000295,\
0.000014,0.000385,\
0.000016,0.000487,\
0.000018,0.000601,\
0.000019,0.000726,\
0.000021,0.000864,\
0.000023,0.001013,\
0.000025,0.001175,\
0.000026,0.001347,\
0.000028,0.001532,\
0.000030,0.001728,\
0.000032,0.001935,\
0.000033,0.002154,\
0.000035,0.002385,\
0.000037,0.002626,\
0.000039,0.002879,\
0.000040,0.003144,\
0.000042,0.003419,\
0.000044,0.003705,\
0.000045,0.004002,\
0.000047,0.004310,\
0.000049,0.004629,\
0.000050,0.004958,\
0.000052,0.005298,\
0.000054,0.005649,\
0.000055,0.006009,\
0.000057,0.006380,\
0.000059,0.006761,\
0.000060,0.007152,\
0.000062,0.007553,\
0.000063,0.007964,\
0.000065,0.008384,\
0.000067,0.008814,\
0.000068,0.009253,\
0.000070,0.009701}
#endif

#endif // _C_DCL_H


/* /- PWM_FREQUENCE_8KHZ
//Udcp,UdcN Use  2 order butterworth filter,corner frequency 25Hz,sample frequency 6.4kHz
#define UDC_PNF_DEFAULT8KHZ         {0.0001480f,    0.0002960f, 0.0001480f, -1.9652934f,    0.9658855f, 0.0f,   0.0f }  //1.48e-04
#define UDC_BALANCE_DEFAULT8KHZ     UDC_PNF_DEFAULT8KHZ
//PLL Use  2 order butterworth filter,corner frequency 25Hz,sample frequency 16kHz
#define PLL_VOLT_Q_DEFAULT8KHZ      {0.0000239f,    0.0000479f, 0.0000239f, -1.9861162f,    0.9862119f, 0.0f,   0.0f } //2.39e-05
#define PLL_AGC_FILTER_DEFAULT8KHZ  PLL_VOLT_Q_DEFAULT8KHZ
#define BWFILTER_FREQ_DEFAULTS8KHZ  PLL_VOLT_Q_DEFAULT8KHZ
//GRID_VOLT Use  2 order butterworth filter,corner frequency 2500Hz,sample frequency 16kHz
//#define   GRID_VOLT_DEFAULT       {0.1399393f,    0.2798786f, 0.1399393f, -0.6997380f,    0.2594952f, 0.0f,   0.0f }  //0.1399393,2.5kHz delay:2
#define GRID_VOLT_DEFAULT8KHZ       {0.0299546f,    0.0599092f, 0.0299546f, -1.4542436f,    0.5740619f, 0.0f,   0.0f }  //0.0299546,1kHz delay:4
//#define   GRID_VOLT_DEFAULT       {0.0157495f,    0.0314989f, 0.0157495f, -1.6149426f,    0.6779405f, 0.0f,   0.0f }  //0.0157495,700Hz delay:6
#define APFOUT_DEFAULT8KHZ        {0.0112544f,    0.0225088f,     0.0112544f,     -1.6782777f,    0.7232954f, 0.0f,   0.0f }  //0.0112544,700Hz delay:7

//RESONANCE_PORTECT_ABC Use second-order sections band-pass butterworth filter,corner frequency 3kHz,5kHz,sample frequency 19.2kHz
#define RESONANCE_PORTECT_ABC8KHZ {0.253427f, 0.000000f,  -0.253427f, -0.408113f, 0.493145f,  0,  0}  //0.253427
//FFT filter display Use  2 order butterworth filter,corner frequency 25Hz,sample frequency 3.2kHzJCL:要改
#define  FFT_MAG1_DEFAULT8KHZ  {0.000833f, 0.001665f,  0.000833f,  -1.916741f, 0.920071f,  0,  0}  //0.000833
#define  FFT_MAG2_DEFAULT8KHZ FFT_MAG1_DEFAULT8KHZ
#define  FFT_MAG3_DEFAULT8KHZ FFT_MAG1_DEFAULT8KHZ
#define  FFT_MAG_DEFAULT FFT_MAG1_DEFAULT8KHZ
#define  FFT_PHA1_DEFAULT8KHZ   {0.000833f, 0.001665f,  0.000833f,  -1.916741f, 0.920071f,  0,  0}  //0.000833
#define  FFT_PHA2_DEFAULT8KHZ FFT_PHA1_DEFAULT8KHZ
#define  FFT_PHA3_DEFAULT8KHZ FFT_PHA1_DEFAULT8KHZ
#define  FFT_PHA_DEFAULT FFT_PHA1_DEFAULT8KHZ
//This is PR ctrl ccs 8.00kHz @50.00Hz @2.400 @1000.00Mag coefficient.
#define PRCTRL_BLOCK_DEFAULT8KHZ { \
0.942478,-0.942296,-1.998673,0.999058,0.000000,0.000000,\
0.471239,-0.470876,-1.997517,0.999058,0.000000,0.000000,\
0.314159,-0.313615,-1.995591,0.999058,0.000000,0.000000,\
0.235619,-0.234893,-1.992896,0.999058,0.000000,0.000000,\
0.188496,-0.187588,-1.989432,0.999058,0.000000,0.000000,\
0.157080,-0.155991,-1.985201,0.999058,0.000000,0.000000,\
0.134640,-0.133370,-1.980206,0.999058,0.000000,0.000000,\
0.117810,-0.116360,-1.974446,0.999058,0.000000,0.000000,\
0.104720,-0.103089,-1.967926,0.999058,0.000000,0.000000,\
0.094248,-0.092437,-1.960647,0.999058,0.000000,0.000000,\
0.085680,-0.083690,-1.952612,0.999058,0.000000,0.000000,\
0.078540,-0.076370,-1.943824,0.999058,0.000000,0.000000,\
0.072498,-0.070150,-1.934287,0.999058,0.000000,0.000000,\
0.067320,-0.064793,-1.924004,0.999058,0.000000,0.000000,\
0.062832,-0.060127,-1.912979,0.999058,0.000000,0.000000,\
0.058905,-0.056023,-1.901217,0.999058,0.000000,0.000000,\
0.055440,-0.052381,-1.888722,0.999058,0.000000,0.000000,\
0.052360,-0.049125,-1.875499,0.999058,0.000000,0.000000,\
0.049604,-0.046193,-1.861553,0.999058,0.000000,0.000000,\
0.047124,-0.043538,-1.846889,0.999058,0.000000,0.000000,\
0.044880,-0.041120,-1.831513,0.999058,0.000000,0.000000,\
0.042840,-0.038906,-1.815431,0.999058,0.000000,0.000000,\
0.040977,-0.036871,-1.798649,0.999058,0.000000,0.000000,\
0.039270,-0.034991,-1.781174,0.999058,0.000000,0.000000,\
0.037699,-0.033249,-1.763012,0.999058,0.000000,0.000000,\
0.036249,-0.031629,-1.744170,0.999058,0.000000,0.000000,\
0.034907,-0.030117,-1.724656,0.999058,0.000000,0.000000,\
0.033660,-0.028701,-1.704477,0.999058,0.000000,0.000000,\
0.032499,-0.027373,-1.683641,0.999058,0.000000,0.000000,\
0.031416,-0.026123,-1.662156,0.999058,0.000000,0.000000,\
0.030403,-0.024944,-1.640030,0.999058,0.000000,0.000000,\
0.029452,-0.023829,-1.617272,0.999058,0.000000,0.000000,\
0.028560,-0.022773,-1.593890,0.999058,0.000000,0.000000,\
0.027720,-0.021771,-1.569894,0.999058,0.000000,0.000000,\
0.026928,-0.020817,-1.545293,0.999058,0.000000,0.000000,\
0.026180,-0.019909,-1.520096,0.999058,0.000000,0.000000,\
0.025472,-0.019043,-1.494313,0.999058,0.000000,0.000000,\
0.024802,-0.018215,-1.467953,0.999058,0.000000,0.000000,\
0.024166,-0.017422,-1.441028,0.999058,0.000000,0.000000,\
0.023562,-0.016663,-1.413547,0.999058,0.000000,0.000000,\
0.022987,-0.015934,-1.385522,0.999058,0.000000,0.000000}
#define PRCTRL_BLOCK_FULC8KHZ { \
0.000007,0.000015,\
0.000015,0.000062,\
0.000022,0.000139,\
0.000029,0.000246,\
0.000036,0.000385,\
0.000043,0.000554,\
0.000051,0.000753,\
0.000058,0.000982,\
0.000065,0.001242,\
0.000072,0.001532,\
0.000079,0.001851,\
0.000086,0.002199,\
0.000093,0.002577,\
0.000100,0.002983,\
0.000107,0.003418,\
0.000114,0.003881,\
0.000121,0.004372,\
0.000128,0.004891,\
0.000135,0.005436,\
0.000142,0.006008,\
0.000148,0.006607,\
0.000155,0.007230,\
0.000161,0.007880,\
0.000168,0.008553,\
0.000174,0.009251,\
0.000181,0.009973,\
0.000187,0.010718,\
0.000193,0.011485,\
0.000199,0.012274,\
0.000206,0.013084,\
0.000212,0.013915,\
0.000217,0.014765,\
0.000223,0.015635,\
0.000229,0.016524,\
0.000235,0.017430,\
0.000240,0.018353,\
0.000246,0.019293,\
0.000251,0.020249,\
0.000256,0.021219,\
0.000262,0.022203,\
0.000267,0.023201}
*/
/* end of file */
