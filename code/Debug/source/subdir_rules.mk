################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
source/%.obj: ../source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -O0 --opt_for_speed=5 --fp_mode=relaxed --fp_reassoc=on --include_path="F:/Project/ESC/code/include/FlashAPI" --include_path="F:/Project/ESC/code" --include_path="E:/APP/TI/SYSBIOS/bios_6_73_01_01/packages/ti/posix/ccs" --include_path="E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/include" --include_path="F:/Project/ESC/code/include" --include_path="F:/Project/ESC/code/FreeModbus/modbus/include" --include_path="F:/Project/ESC/code/FreeModbus/port/inc" --include_path="F:/Project/ESC/code/FreeModbus/AuthorFiles" --include_path="F:/Project/ESC/code/FreeModbus/modbus/rtu" --advice:performance=all -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

source/Core.obj: ../source/Core.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -O0 --opt_for_speed=5 --fp_mode=relaxed --fp_reassoc=on --include_path="F:/Project/ESC/code/include/FlashAPI" --include_path="F:/Project/ESC/code" --include_path="E:/APP/TI/SYSBIOS/bios_6_73_01_01/packages/ti/posix/ccs" --include_path="E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/include" --include_path="F:/Project/ESC/code/include" --include_path="F:/Project/ESC/code/FreeModbus/modbus/include" --include_path="F:/Project/ESC/code/FreeModbus/port/inc" --include_path="F:/Project/ESC/code/FreeModbus/AuthorFiles" --include_path="F:/Project/ESC/code/FreeModbus/modbus/rtu" --advice:performance=all -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

source/IsrTimerFunc_MEC.obj: ../source/IsrTimerFunc_MEC.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -O0 --opt_for_speed=5 --fp_mode=relaxed --fp_reassoc=on --include_path="F:/Project/ESC/code/include/FlashAPI" --include_path="F:/Project/ESC/code" --include_path="E:/APP/TI/SYSBIOS/bios_6_73_01_01/packages/ti/posix/ccs" --include_path="E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/include" --include_path="F:/Project/ESC/code/include" --include_path="F:/Project/ESC/code/FreeModbus/modbus/include" --include_path="F:/Project/ESC/code/FreeModbus/port/inc" --include_path="F:/Project/ESC/code/FreeModbus/AuthorFiles" --include_path="F:/Project/ESC/code/FreeModbus/modbus/rtu" --advice:performance=all -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

source/OptLevelNone.obj: ../source/OptLevelNone.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -Ooff --opt_for_speed=5 --fp_mode=relaxed --fp_reassoc=on --include_path="F:/Project/ESC/code/include/FlashAPI" --include_path="F:/Project/ESC/code" --include_path="E:/APP/TI/SYSBIOS/bios_6_73_01_01/packages/ti/posix/ccs" --include_path="E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/include" --include_path="F:/Project/ESC/code/include" --include_path="F:/Project/ESC/code/FreeModbus/modbus/include" --include_path="F:/Project/ESC/code/FreeModbus/port/inc" --include_path="F:/Project/ESC/code/FreeModbus/AuthorFiles" --include_path="F:/Project/ESC/code/FreeModbus/modbus/rtu" --advice:performance=all -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

source/Spi_Fram.obj: ../source/Spi_Fram.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -O0 --opt_for_speed=5 --fp_mode=relaxed --fp_reassoc=on --include_path="F:/Project/ESC/code/include/FlashAPI" --include_path="F:/Project/ESC/code" --include_path="E:/APP/TI/SYSBIOS/bios_6_73_01_01/packages/ti/posix/ccs" --include_path="E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/include" --include_path="F:/Project/ESC/code/include" --include_path="F:/Project/ESC/code/FreeModbus/modbus/include" --include_path="F:/Project/ESC/code/FreeModbus/port/inc" --include_path="F:/Project/ESC/code/FreeModbus/AuthorFiles" --include_path="F:/Project/ESC/code/FreeModbus/modbus/rtu" --advice:performance=all -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

source/vmaxfloat.obj: ../source/vmaxfloat.cla $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -O0 --opt_for_speed=5 --fp_mode=relaxed --fp_reassoc=on --include_path="F:/Project/ESC/code/include/FlashAPI" --include_path="F:/Project/ESC/code" --include_path="E:/APP/TI/SYSBIOS/bios_6_73_01_01/packages/ti/posix/ccs" --include_path="E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/include" --include_path="F:/Project/ESC/code/include" --include_path="F:/Project/ESC/code/FreeModbus/modbus/include" --include_path="F:/Project/ESC/code/FreeModbus/port/inc" --include_path="F:/Project/ESC/code/FreeModbus/AuthorFiles" --include_path="F:/Project/ESC/code/FreeModbus/modbus/rtu" --advice:performance=all -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


