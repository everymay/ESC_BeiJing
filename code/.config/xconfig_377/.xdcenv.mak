#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = E:/APP/TI/SYSBIOS/bios_6_73_01_01/packages;E:/APP/TI/UIA/uia_2_30_01_02/packages;D:/TI/controlSUITE;E:/packages;C:/Users/WangYu/Desktop/mecRuineng377_SelfCoolESCREV1/.config
override XDCROOT = E:/APP/TI/CCS/xdctools_3_50_08_24_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = E:/APP/TI/SYSBIOS/bios_6_73_01_01/packages;E:/APP/TI/UIA/uia_2_30_01_02/packages;D:/TI/controlSUITE;E:/packages;C:/Users/WangYu/Desktop/mecRuineng377_SelfCoolESCREV1/.config;E:/APP/TI/CCS/xdctools_3_50_08_24_core/packages;..
HOSTOS = Windows
endif
