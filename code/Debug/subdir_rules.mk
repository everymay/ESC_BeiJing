################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1603622319:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-1603622319-inproc

build-1603622319-inproc: ../377.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"E:/APP/TI/CCS/xdctools_3_50_08_24_core/xs" --xdcpath="E:/APP/TI/SYSBIOS/bios_6_73_01_01/packages;E:/APP/TI/UIA/uia_2_30_01_02/packages;D:/TI/controlSUITE;/packages;" xdc.tools.configuro -o configPkg -t ti.targets.C28_float -p ti.platforms.tms320x28:TMS320F28377S -r release -c "E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS" --compileOptions "-g" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-1603622319 ../377.cfg
configPkg/compiler.opt: build-1603622319
configPkg/: build-1603622319


