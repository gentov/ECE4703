################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv7/tools/compiler/c6000_7.4.24/bin/cl6x" -mv6713 --abi=coffabi -g --include_path="R:/ECE4703_B18/Lab3Part1" --include_path="C:/ti/ccsv7/tools/compiler/c6000_7.4.24/include" --include_path="R:/ECE4703_B18/C6xCSL/include" --include_path="R:/ECE4703_B18/DSK6713/c6000/dsk6713/include" --diag_wrap=off --diag_warning=225 --display_error_number --mem_model:const=far --mem_model:data=far --preproc_with_compile --preproc_dependency="main.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

vectors.obj: ../vectors.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv7/tools/compiler/c6000_7.4.24/bin/cl6x" -mv6713 --abi=coffabi -g --include_path="R:/ECE4703_B18/Lab3Part1" --include_path="C:/ti/ccsv7/tools/compiler/c6000_7.4.24/include" --include_path="R:/ECE4703_B18/C6xCSL/include" --include_path="R:/ECE4703_B18/DSK6713/c6000/dsk6713/include" --diag_wrap=off --diag_warning=225 --display_error_number --mem_model:const=far --mem_model:data=far --preproc_with_compile --preproc_dependency="vectors.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


