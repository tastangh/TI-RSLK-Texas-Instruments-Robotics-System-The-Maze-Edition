################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Lab16_Tachmain.obj: C:/Users/SEDAT/Desktop/New\ folder\ (2)/tirslk_maze_1_00_01/tirslk_maze_1_00_00/Lab16_Tach/Lab16_Tachmain.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv4 --code_state=32 --include_path="C:/Users/SEDAT/Desktop/New folder/tirslk_maze_1_00_00/pi" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="Lab16_Tachmain.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv4 --code_state=32 --include_path="C:/Users/SEDAT/Desktop/New folder/tirslk_maze_1_00_00/pi" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="main.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


