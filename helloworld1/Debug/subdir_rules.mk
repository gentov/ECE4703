################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
hello.obj: ../hello.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv7/tools/compiler/c6000_7.4.24/bin/cl6x" --abi=coffabi -g --include_path="R:/ECE4703_B18/helloworld1" --include_path="C:/ti/ccsv7/tools/compiler/c6000_7.4.24/include" --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="hello.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


