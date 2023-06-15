/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 *
 * Generated by MATLAB(R) 7.6 and the Signal Processing Toolbox 6.9.
 *
 * Generated on: 19-Apr-2013 10:32:47
 *
 */

/*
 * Discrete-Time IIR Filter (real)
 * -------------------------------
 * Filter Structure    : Direct-Form II, Second-Order Sections
 * Number of Sections  : 1
 * Stable              : Yes
 * Linear Phase        : No
 */

/* General type conversion for MATLAB generated C-code  */
//#include "tmwtypes.h"
/* 
 * Expected path to tmwtypes.h 
 * C:\Program Files\MATLAB\R2008a\extern\include\tmwtypes.h 
 */

#define  CUTOFF_FREQUENCY_10K_20hz     0
#define  CUTOFF_FREQUENCY_10K_30hz     0
#define  CUTOFF_FREQUENCY_10K_50hz     0
#define  CUTOFF_FREQUENCY_10K_60hz     0
#define  CUTOFF_FREQUENCY_10K_80hz     0
#define  CUTOFF_FREQUENCY_10K_100hz    0

#define  CUTOFF_FREQUENCY_20K_20hz     0
#define  CUTOFF_FREQUENCY_20K_30hz     0
#define  CUTOFF_FREQUENCY_20K_50hz     0
#define  CUTOFF_FREQUENCY_20K_60hz     0
#define  CUTOFF_FREQUENCY_20K_80hz     0
#define  CUTOFF_FREQUENCY_20K_100hz    0



#define  CUTOFF_FREQUENCY_3K_25hz     0
#define  CUTOFF_FREQUENCY_3K_40hz     0
#define  CUTOFF_FREQUENCY_3K_50hz     0
#define  CUTOFF_FREQUENCY_3K_80hz     0

#define  CUTOFF_FREQUENCY_9K_25hz     0
#define  CUTOFF_FREQUENCY_9K_40hz     0
#define  CUTOFF_FREQUENCY_9K_50hz     0
#define  CUTOFF_FREQUENCY_9K_80hz     0


#define  CUTOFF_FREQUENCY_4K_20hz     0
#define  CUTOFF_FREQUENCY_4K_30hz     0
#define  CUTOFF_FREQUENCY_4K_50hz     0
#define  CUTOFF_FREQUENCY_4K_60hz     0
#define  CUTOFF_FREQUENCY_4K_80hz     0
#define  CUTOFF_FREQUENCY_4K_100hz    0

#define  CUTOFF_FREQUENCY_8K_20hz     0
#define  CUTOFF_FREQUENCY_8K_30hz     1
#define  CUTOFF_FREQUENCY_8K_50hz     0
#define  CUTOFF_FREQUENCY_8K_60hz     0
#define  CUTOFF_FREQUENCY_8K_80hz     0
#define  CUTOFF_FREQUENCY_8K_100hz    0

#define  CUTOFF_FREQUENCY_16K_20hz     0
#define  CUTOFF_FREQUENCY_16K_30hz     0
#define  CUTOFF_FREQUENCY_16K_50hz     0
#define  CUTOFF_FREQUENCY_16K_60hz     0
#define  CUTOFF_FREQUENCY_16K_80hz     0
#define  CUTOFF_FREQUENCY_16K_100hz    0


#if (CUTOFF_FREQUENCY_4K_20hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.000241359049042,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.955578240315,   0.9565436765112
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_4K_30hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.000537169774812,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,     1.93338022588,   0.9355289049792
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_4K_50hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.001460316305528,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.889033079395,   0.8948743446166
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_4K_60hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.002080567135492,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.866892279712,   0.8752145482537
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_4K_80hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.003621681514929,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.822694925196,    0.837181651256
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_4K_100hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.005542717210281,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.778631777825,   0.8008026466657
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif


#if (CUTOFF_FREQUENCY_8K_20hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  6.100617875804e-005,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.977786483777,   0.9780305084918
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_8K_30hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  0.0001365107220939,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.966681385263,   0.9672274281519
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_8K_50hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.000375069616297,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.944477657767,   0.9459779362323
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_8K_60hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.000537169774812,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,     1.93338022588,   0.9355289049792
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_8K_80hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  0.0009446918438402,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.911197067426,   0.9149758348014
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_8K_100hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.001460316305528,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.889033079395,   0.8948743446166
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_16K_20hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  1.533600836839e-005,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
		 0.9999999999979,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,      1.9888929059,   0.9889542499331
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_16K_30hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  3.441077756128e-005,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
		 0.9999999999984,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.983339569599,   0.9834772127096
	  },
	  {
					   1,                 0,                 0
	  }
	};

#endif

#if (CUTOFF_FREQUENCY_16K_50hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  9.50600294492e-005,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.972233729195,   0.9726139693131
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_16K_60hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  0.0001365107220939,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.966681385263,   0.9672274281519
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_16K_80hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.000241359049042,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.955578240315,   0.9565436765112
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_16K_100hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.000375069616297,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.944477657767,   0.9459779362323
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_9K_25hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  7.522410259769e-005,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
		  0.999999999999,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.975318547206,   0.9756194436167
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_9K_40hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  0.0001911681364122,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.960512690766,   0.9612773633119
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_9K_50hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.000297251027458,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.950644296289,   0.9518333003986
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_9K_80hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  0.0007500135506056,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.921054131671,    0.924054185873
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif



#if (CUTOFF_FREQUENCY_3K_25hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  0.0006607790982304,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.925983969732,   0.9286270861248
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif


#if (CUTOFF_FREQUENCY_3K_40hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.001655609343778,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.881650046179,   0.8882724835544
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif


#if (CUTOFF_FREQUENCY_3K_50hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.002550535158536,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.852146485396,   0.8623486260301
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif


#if (CUTOFF_FREQUENCY_3K_80hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.006263382641287,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.763977952257,   0.7890314828226
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_10K_20hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  3.913020539914e-005,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.982228929793,   0.9823854506141
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_10K_30hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  8.765554875406e-005,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
		 0.9999999999995,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.973344249781,   0.9736948719763
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_10K_50hz)
#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.000241359049042,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.955578240315,   0.9565436765112
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_10K_60hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.000346041337639,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.946697540756,   0.9480817061067
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_10K_80hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  0.0006098547187173,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.928942263252,   0.9313816821269
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_10K_100hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  0.0009446918438402,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.911197067426,   0.9149758348014
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_20K_20hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  9.825916820416e-006,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
		  1.000000000007,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.991114292202,   0.9911535958689
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_20K_30hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  2.205943646072e-005,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
		 0.9999999999985,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,   1.986671546548,   0.9867597842938
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_20K_50hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  6.100617875804e-005,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.977786483777,   0.9780305084918
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_20K_60hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  8.765554875406e-005,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
		 0.9999999999995,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.973344249781,   0.9736948719763
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_20K_80hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	  0.0001551484234757,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
		 0.9999999999997,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.964460580205,   0.9650811738991
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

#if (CUTOFF_FREQUENCY_20K_100hz)
	#define MWSPT_NSEC 3
	const int NL[MWSPT_NSEC] = { 1,3,1 };
	const float NUM[MWSPT_NSEC][3] = {
	  {
	   0.000241359049042,                 0,                 0
	  },
	  {
					   1,                 2,                 1
	  },
	  {
					   1,                 0,                 0
	  }
	};
	const int DL[MWSPT_NSEC] = { 1,3,1 };
	const float DEN[MWSPT_NSEC][3] = {
	  {
					   1,                 0,                 0
	  },
	  {
					   1,    1.955578240315,   0.9565436765112
	  },
	  {
					   1,                 0,                 0
	  }
	};
#endif

// end of .h
