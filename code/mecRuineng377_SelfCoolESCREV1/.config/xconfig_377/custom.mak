## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,28FP linker.cmd package/cfg/377_p28FP.o28FP

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/377_p28FP.xdl
	$(SED) 's"^\"\(package/cfg/377_p28FPcfg.cmd\)\"$""\"F:/Project/ESC/code/mecRuineng377_SelfCoolESCREV1/.config/xconfig_377/\1\""' package/cfg/377_p28FP.xdl > $@
	-$(SETDATE) -r:max package/cfg/377_p28FP.h compiler.opt compiler.opt.defs
