rwildcard = $(foreach d, $(wildcard $1*), $(filter $(subst *, %, $2), $d) $(call rwildcard, $d/, $2))

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/base_rules

################################################################################

IPL_LOAD_ADDR := 0x40008000
VERSION := $(shell cat VERSION)

################################################################################

TARGET := omninx-installer
OUTPUT_NAME := OmniNX-Installer.bin
BUILDDIR := build
OUTPUTDIR := output
SOURCEDIR := source
BDKDIR := bdk
BDKINC := -I./$(BDKDIR)
VPATH = $(dir ./$(SOURCEDIR)/) $(dir $(wildcard ./$(SOURCEDIR)/*/)) $(dir $(wildcard ./$(SOURCEDIR)/*/*/))
VPATH += $(dir $(wildcard ./$(BDKDIR)/)) $(dir $(wildcard ./$(BDKDIR)/*/)) $(dir $(wildcard ./$(BDKDIR)/*/*/))

# All source files
OBJS = $(patsubst $(SOURCEDIR)/%.S, $(BUILDDIR)/$(TARGET)/%.o, \
		$(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/$(TARGET)/%.o, \
		$(call rwildcard, $(SOURCEDIR), *.S *.c)))
OBJS += $(patsubst $(BDKDIR)/%.S, $(BUILDDIR)/$(TARGET)/%.o, \
		$(patsubst $(BDKDIR)/%.c, $(BUILDDIR)/$(TARGET)/%.o, \
		$(call rwildcard, $(BDKDIR), *.S *.c)))

GFX_INC   := '"../$(SOURCEDIR)/gfx.h"'
FFCFG_INC := '"../$(SOURCEDIR)/libs/fatfs/ffconf.h"'

################################################################################

CUSTOMDEFINES := -DIPL_LOAD_ADDR=$(IPL_LOAD_ADDR)
CUSTOMDEFINES += -DGFX_INC=$(GFX_INC) -DFFCFG_INC=$(FFCFG_INC)
CUSTOMDEFINES += -DVERSION='"$(VERSION)"'

ARCH := -march=armv4t -mtune=arm7tdmi -mthumb -mthumb-interwork
CFLAGS = $(ARCH) -Os -nostdlib -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-inline -std=gnu11 -Wall -Wno-missing-braces $(CUSTOMDEFINES)
LDFLAGS = $(ARCH) -nostartfiles -lgcc -Wl,--nmagic,--gc-sections -Xlinker --defsym=IPL_LOAD_ADDR=$(IPL_LOAD_ADDR)

################################################################################

.PHONY: all clean release

all: $(OUTPUTDIR)/$(OUTPUT_NAME)
	$(eval BIN_SIZE = $(shell wc -c < $(OUTPUTDIR)/$(OUTPUT_NAME)))
	@echo "Payload size is $(BIN_SIZE) bytes"
	@echo "Max size is 126296 bytes."

clean:
	@rm -rf $(BUILDDIR)
	@rm -rf $(OUTPUTDIR)
	@rm -rf release
	@rm -f $(TARGET)-*.zip

$(OUTPUTDIR)/$(OUTPUT_NAME): $(BUILDDIR)/$(TARGET)/$(TARGET).elf
	@mkdir -p "$(@D)"
	$(OBJCOPY) -S -O binary $< $(OUTPUTDIR)/$(TARGET).bin
	@mv $(OUTPUTDIR)/$(TARGET).bin $(OUTPUTDIR)/$(OUTPUT_NAME)

$(BUILDDIR)/$(TARGET)/$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) -T $(SOURCEDIR)/link.ld $^ -o $@

$(BUILDDIR)/$(TARGET)/%.o: $(SOURCEDIR)/%.c
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) $(BDKINC) -I$(SOURCEDIR) -c $< -o $@

$(BUILDDIR)/$(TARGET)/%.o: $(SOURCEDIR)/%.S
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/$(TARGET)/%.o: $(BDKDIR)/%.c
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) $(BDKINC) -I$(SOURCEDIR) -c $< -o $@

$(BUILDDIR)/$(TARGET)/%.o: $(BDKDIR)/%.S
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -c $< -o $@

release: $(OUTPUTDIR)/$(OUTPUT_NAME)
	@mkdir -p release/bootloader/payloads
	@cp $(OUTPUTDIR)/$(OUTPUT_NAME) release/bootloader/payloads/$(OUTPUT_NAME)
	@cd release && zip -r ../$(TARGET)-$(VERSION).zip bootloader
	@echo "Release package created: $(TARGET)-$(VERSION).zip"
