################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
00_Test_GPIO.hex \

SECONDARY_LIST += \
00_Test_GPIO.lst \

SECONDARY_SIZE += \
00_Test_GPIO.siz \


# All Target
all: 00_Test_GPIO.elf secondary-outputs

# Tool invocations
00_Test_GPIO.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -T "E:\Project\Snowflake-6\software\CH32V003\00_Test_GPIO\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"00_Test_GPIO.map" --specs=nano.specs --specs=nosys.specs -o "00_Test_GPIO.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lprintf
	@	@
00_Test_GPIO.hex: 00_Test_GPIO.elf
	@	riscv-none-embed-objcopy -O ihex "00_Test_GPIO.elf"  "00_Test_GPIO.hex"
	@	@
00_Test_GPIO.lst: 00_Test_GPIO.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "00_Test_GPIO.elf" > "00_Test_GPIO.lst"
	@	@
00_Test_GPIO.siz: 00_Test_GPIO.elf
	@	riscv-none-embed-size --format=berkeley "00_Test_GPIO.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) 00_Test_GPIO.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
