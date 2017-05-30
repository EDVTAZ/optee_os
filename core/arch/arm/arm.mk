CFG_LTC_OPTEE_THREAD ?= y
# Size of emulated TrustZone protected SRAM, 448 kB.
# Only applicable when paging is enabled.
CFG_CORE_TZSRAM_EMUL_SIZE ?= 458752
CFG_LPAE_ADDR_SPACE_SIZE ?= (1ull << 32)
CFG_TEE_COHERENT_SIZE ?= 0

ifeq ($(CFG_ARM64_core),y)
CFG_KERN_LINKER_FORMAT ?= elf64-littleaarch64
CFG_KERN_LINKER_ARCH ?= aarch64
endif
ifeq ($(CFG_ARM32_core),y)
CFG_KERN_LINKER_FORMAT ?= elf32-littlearm
CFG_KERN_LINKER_ARCH ?= arm
endif

ifeq ($(CFG_TA_FLOAT_SUPPORT),y)
# Use hard-float for floating point support in user TAs instead of
# soft-float
CFG_WITH_VFP ?= y
ifeq ($(CFG_ARM64_core),y)
# AArch64 has no fallback to soft-float
$(call force,CFG_WITH_VFP,y)
endif
ifeq ($(CFG_WITH_VFP),y)
platform-hard-float-enabled := y
endif
endif

CFG_CORE_RWDATA_NOEXEC ?= y
CFG_CORE_RODATA_NOEXEC ?= n
ifeq ($(CFG_CORE_RODATA_NOEXEC),y)
$(call force,CFG_CORE_RWDATA_NOEXEC,y)
endif
# 'y' to set the Alignment Check Enable bit in SCTLR/SCTLR_EL1, 'n' to clear it
CFG_SCTLR_ALIGNMENT_CHECK ?= y

ifeq ($(CFG_WITH_PAGER),y)
ifeq ($(CFG_CORE_SANITIZE_KADDRESS),y)
$(error Error: CFG_CORE_SANITIZE_KADDRESS not compatible with CFG_WITH_PAGER)
endif
endif

ifeq ($(CFG_ARM32_core),y)
# Configration directive related to ARMv7 optee boot arguments.
# CFG_PAGEABLE_ADDR: if defined, forces pageable data physical address.
# CFG_NS_ENTRY_ADDR: if defined, forces NS World physical entry address.
# CFG_DT_ADDR:       if defined, forces Device Tree data physical address.
endif

core-platform-cppflags	+= -I$(arch-dir)/include
core-platform-subdirs += \
	$(addprefix $(arch-dir)/, kernel mm tee pta) $(platform-dir)

ifneq ($(CFG_WITH_ARM_TRUSTED_FW),y)
core-platform-subdirs += $(arch-dir)/sm
endif

arm64-platform-cppflags += -DARM64=1 -D__LP64__=1
arm32-platform-cppflags += -DARM32=1 -D__ILP32__=1

platform-cflags-generic ?= -g -ffunction-sections -fdata-sections -pipe
platform-aflags-generic ?= -g -pipe

arm32-platform-cflags-no-hard-float ?= -mno-apcs-float -mfloat-abi=soft
arm32-platform-cflags-hard-float ?= -mfloat-abi=hard -funsafe-math-optimizations
arm32-platform-cflags-generic ?= -mthumb -mthumb-interwork \
			-fno-short-enums -fno-common -mno-unaligned-access
arm32-platform-aflags-no-hard-float ?=

arm64-platform-cflags-no-hard-float ?= -mgeneral-regs-only
arm64-platform-cflags-hard-float ?=
arm64-platform-cflags-generic ?= -mstrict-align

ifeq ($(DEBUG),1)
platform-cflags-optimization ?=  -O0
else
platform-cflags-optimization ?=  -Os
endif

platform-cflags-debug-info ?= -g3
platform-aflags-debug-info ?=

core-platform-cflags += $(platform-cflags-optimization)
core-platform-cflags += $(platform-cflags-generic)
core-platform-cflags += $(platform-cflags-debug-info)

core-platform-aflags += $(platform-aflags-generic)
core-platform-aflags += $(platform-aflags-debug-info)

ifeq ($(CFG_ARM64_core),y)
arch-bits-core := 64
core-platform-cppflags += $(arm64-platform-cppflags)
core-platform-cflags += $(arm64-platform-cflags)
core-platform-cflags += $(arm64-platform-cflags-generic)
core-platform-cflags += $(arm64-platform-cflags-no-hard-float)
core-platform-aflags += $(arm64-platform-aflags)
else
arch-bits-core := 32
core-platform-cppflags += $(arm32-platform-cppflags)
core-platform-cflags += $(arm32-platform-cflags)
core-platform-cflags += $(arm32-platform-cflags-no-hard-float)
ifeq ($(CFG_UNWIND),y)
core-platform-cflags += -funwind-tables
endif
core-platform-cflags += $(arm32-platform-cflags-generic)
core-platform-aflags += $(core_arm32-platform-aflags)
core-platform-aflags += $(arm32-platform-aflags)
endif

ifneq ($(filter ta_arm32,$(ta-targets)),)
# Variables for ta-target/sm "ta_arm32"
CFG_ARM32_ta_arm32 := y
arch-bits-ta_arm32 := 32
ta_arm32-platform-cppflags += $(arm32-platform-cppflags)
ta_arm32-platform-cflags += $(arm32-platform-cflags)
ta_arm32-platform-cflags += $(platform-cflags-optimization)
ta_arm32-platform-cflags += $(platform-cflags-debug-info)
ta_arm32-platform-cflags += -fpie
ta_arm32-platform-cflags += $(arm32-platform-cflags-generic)
ifeq ($(platform-hard-float-enabled),y)
ta_arm32-platform-cflags += $(arm32-platform-cflags-hard-float)
else
ta_arm32-platform-cflags += $(arm32-platform-cflags-no-hard-float)
endif
ifeq ($(CFG_UNWIND),y)
ta_arm32-platform-cflags += -funwind-tables
endif
ta_arm32-platform-aflags += $(platform-aflags-debug-info)
ta_arm32-platform-aflags += $(arm32-platform-aflags)

ta-mk-file-export-vars-ta_arm32 += CFG_ARM32_ta_arm32
ta-mk-file-export-vars-ta_arm32 += ta_arm32-platform-cppflags
ta-mk-file-export-vars-ta_arm32 += ta_arm32-platform-cflags
ta-mk-file-export-vars-ta_arm32 += ta_arm32-platform-aflags

ta-mk-file-export-add-ta_arm32 += CROSS_COMPILE32 ?= $$(CROSS_COMPILE)_nl_
ta-mk-file-export-add-ta_arm32 += CROSS_COMPILE_ta_arm32 ?= $$(CROSS_COMPILE32)_nl_
endif

ifneq ($(filter ta_arm64,$(ta-targets)),)
# Variables for ta-target/sm "ta_arm64"
CFG_ARM64_ta_arm64 := y
arch-bits-ta_arm64 := 64
ta_arm64-platform-cppflags += $(arm64-platform-cppflags)
ta_arm64-platform-cflags += $(arm64-platform-cflags)
ta_arm64-platform-cflags += $(platform-cflags-optimization)
ta_arm64-platform-cflags += $(platform-cflags-debug-info)
ta_arm64-platform-cflags += -fpie
ta_arm64-platform-cflags += $(arm64-platform-cflags-generic)
ifeq ($(platform-hard-float-enabled),y)
ta_arm64-platform-cflags += $(arm64-platform-cflags-hard-float)
else
ta_arm64-platform-cflags += $(arm64-platform-cflags-no-hard-float)
endif
ta_arm64-platform-aflags += $(platform-aflags-debug-info)
ta_arm64-platform-aflags += $(arm64-platform-aflags)

ta-mk-file-export-vars-ta_arm64 += CFG_ARM64_ta_arm64
ta-mk-file-export-vars-ta_arm64 += ta_arm64-platform-cppflags
ta-mk-file-export-vars-ta_arm64 += ta_arm64-platform-cflags
ta-mk-file-export-vars-ta_arm64 += ta_arm64-platform-aflags

ta-mk-file-export-add-ta_arm64 += CROSS_COMPILE64 ?= $$(CROSS_COMPILE)_nl_
ta-mk-file-export-add-ta_arm64 += CROSS_COMPILE_ta_arm64 ?= $$(CROSS_COMPILE64)_nl_
endif

# Set cross compiler prefix for each submodule
$(foreach sm, core $(ta-targets), $(eval CROSS_COMPILE_$(sm) ?= $(CROSS_COMPILE$(arch-bits-$(sm)))))
