################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
FreeModbus/port/user_app/%.obj: ../FreeModbus/port/user_app/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -O0 --opt_for_speed=5 --fp_mode=relaxed --fp_reassoc=on --include_path="F:/Project/ESC/code/include/FlashAPI" --include_path="F:/Project/ESC/code" --include_path="E:/APP/TI/SYSBIOS/bios_6_73_01_01/packages/ti/posix/ccs" --include_path="E:/APP/TI/CCS/ccs/tools/compiler/ti-cgt-c2000_18.1.2.LTS/include" --include_path="F:/Project/ESC/code/include" --include_path="F:/Project/ESC/code/FreeModbus/modbus/include" --include_path="F:/Project/ESC/code/FreeModbus/port/inc" --include_path="F:/Project/ESC/code/FreeModbus/AuthorFiles" --include_path="F:/Project/ESC/code/FreeModbus/modbus/rtu" --advice:performance=all -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="FreeModbus/port/user_app/$(basename $(<F)).d_raw" --obj_directory="FreeModbus/port/user_app" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


