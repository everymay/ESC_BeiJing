#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = D:/TI/bios_6_73_01_01/packages;D:/TI/uia_2_30_01_02/packages;D:/TI/controlSUITE;D:/packages;D:/WorkSpace_v8/escselfcool/.config
override XDCROOT = D:/TI/xdctools_3_50_08_24_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = D:/TI/bios_6_73_01_01/packages;D:/TI/uia_2_30_01_02/packages;D:/TI/controlSUITE;D:/packages;D:/WorkSpace_v8/escselfcool/.config;D:/TI/xdctools_3_50_08_24_core/packages;..
HOSTOS = Windows
endif
