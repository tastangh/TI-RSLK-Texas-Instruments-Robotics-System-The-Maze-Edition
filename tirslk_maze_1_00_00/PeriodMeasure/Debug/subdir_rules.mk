################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Clock.obj: C:/Users/SEDAT/Desktop/New\ folder/tirslk_maze_1_00_00/inc/Clock.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP432 Compiler'
	"D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/ccs_base/arm/include" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/ccs_base/arm/include/CMSIS" --include_path="C:/Users/SEDAT/Desktop/New folder/tirslk_maze_1_00_00/PeriodMeasure" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="Clock.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

CortexM.obj: C:/Users/SEDAT/Desktop/New\ folder/tirslk_maze_1_00_00/inc/CortexM.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP432 Compiler'
	"D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/ccs_base/arm/include" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/ccs_base/arm/include/CMSIS" --include_path="C:/Users/SEDAT/Desktop/New folder/tirslk_maze_1_00_00/PeriodMeasure" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="CortexM.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

PeriodMeasure.obj: ../PeriodMeasure.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP432 Compiler'
	"D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/ccs_base/arm/include" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/ccs_base/arm/include/CMSIS" --include_path="C:/Users/SEDAT/Desktop/New folder/tirslk_maze_1_00_00/PeriodMeasure" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="PeriodMeasure.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

TA0InputCapture.obj: C:/Users/SEDAT/Desktop/New\ folder/tirslk_maze_1_00_00/inc/TA0InputCapture.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP432 Compiler'
	"D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/ccs_base/arm/include" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/ccs_base/arm/include/CMSIS" --include_path="C:/Users/SEDAT/Desktop/New folder/tirslk_maze_1_00_00/PeriodMeasure" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="TA0InputCapture.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_msp432p401r_ccs.obj: ../startup_msp432p401r_ccs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP432 Compiler'
	"D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/ccs_base/arm/include" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/ccs_base/arm/include/CMSIS" --include_path="C:/Users/SEDAT/Desktop/New folder/tirslk_maze_1_00_00/PeriodMeasure" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="startup_msp432p401r_ccs.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

system_msp432p401r.obj: ../system_msp432p401r.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP432 Compiler'
	"D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/ccs_base/arm/include" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/ccs_base/arm/include/CMSIS" --include_path="C:/Users/SEDAT/Desktop/New folder/tirslk_maze_1_00_00/PeriodMeasure" --include_path="D:/Apps/TexasInstruments/CCS7/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="system_msp432p401r.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


