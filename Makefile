SHELL = /bin/bash

CFLAGS := -g -Wall -ffreestanding -Werror -Wno-unused-variable -Wno-strict-aliasing -Wno-unused-function -fno-pic -Ikernel -Iexternal/lai/include -O2 -Wno-address-of-packed-member -fno-omit-frame-pointer
#CFLAGS += -fsanitize=undefined #TODO
LDFLAGS := -nostdlib -no-pie

INITRD_DIR := ./initrd

ifeq ($(DEBUG), 1)
	BIN_DIR := bin/DEBUG
	BUILD_DIR := build/DEBUG
else
	BIN_DIR := bin/RELEASE
	BUILD_DIR := build/RELEASE
endif

ifeq ($(TARGET), x86_64)

include configs/boot-x86_64.mk

else ifeq ($(TARGET), riscv64)

include configs/boot-riscv64.mk

else

include configs/boot-x86_64.mk

endif

include sources.mk

.PHONY: default qemu image clean clean-all # toolchain

default: image

INITRD := initrd

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
external := $(OBJS:%.o=%.d)
-include $(external)

$(BIN_DIR)/wivos.elf: $(OBJS) $(BUILD_DIR)/kernel/symbols.o
	@mkdir -p $(@D)
	clang -fuse-ld=lld $(LDFLAGS) -o $@ $(OBJS) $(BUILD_DIR)/kernel/symbols.o

$(BUILD_DIR)/kernel/symbols.o: $(OBJS) generate_symbols.py
	@mkdir -p $(@D)
	clang -fuse-ld=lld $(LDFLAGS) -o $(BUILD_DIR)/wivos.elf $(OBJS)
	nm $(BUILD_DIR)/wivos.elf -g -P | python2 generate_symbols.py > kernel/symbols.s
	clang -masm=intel -Qunused-arguments $(CFLAGS) -c kernel/symbols.s -o $@

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(@D)
	clang -MMD $(CFLAGS) -D__FILENAME__="\"$<\"" -D__MODULE__="\"$(notdir $(basename $<))\"" -c $< -o $@

$(BUILD_DIR)/%.asm.o: %.asm
	@mkdir -p $(@D)
	clang -MMD $(CFLAGS) -D__FILENAME__="\"$<\"" -D__MODULE__="\"$(notdir $(basename $<))\"" -c $< -o $@

$(BUILD_DIR)/%.S.o: %.S
	@mkdir -p $(@D)
	clang -MMD $(CFLAGS) -D__FILENAME__="\"$<\"" -D__MODULE__="\"$(notdir $(basename $<))\"" -c $< -o $@

# TODO
$(BUILD_DIR)/%.bin: %.real
	@mkdir -p $(@D)
	clang -MMD $(CFLAGS) -D__FILENAME__="\"$<\"" -D__MODULE__="\"$(notdir $(basename $<))\"" -c $< -o $@

#build-modules:
#	@make -C modules install
#
#build-libs:
#	@make -C libs install
#
#build-apps: build-libs
#	@make -C apps install

image: $(BIN_DIR)/image.hdd

$(BIN_DIR)/image.hdd: $(BIN_DIR)/wivos.elf # build-modules build-apps toolchain/sysroot/usr/lib/ld.so toolchain/sysroot/usr/lib/libc.so
# @cp -rf toolchain/sysroot/usr/lib/ld.so initrd/lib/ld.so
# @cp -rf toolchain/sysroot/usr/lib/ld.so initrd/usr/lib/ld.so
# @cp -rf toolchain/sysroot/usr/lib/libc.so initrd/lib/libc.so
# @cp -rf toolchain/sysroot/usr/lib/libc.so initrd/usr/lib/libc.so
# @cp -rf toolchain/sysroot/usr/lib initrd/usr
	@mkdir -p $(@D)
	@echo "Creating disk"
	@rm -rf $@
	@dd if=/dev/zero bs=1M count=0 seek=512 of=$@ #64
	@echo "Creating boot FAT32 partition"
	@parted -s $@ mklabel gpt
	@parted -s $@ mkpart primary 2048s 100%
	@rm -rf test_image/
	@mkdir test_image
	@sudo losetup -Pf --show $(BIN_DIR)/image.hdd > loopback_dev
	@sudo partprobe `cat loopback_dev`
	@sudo mkfs.fat -F 32 `cat loopback_dev`p1
	@sudo mount `cat loopback_dev`p1 test_image
	@sudo mkdir test_image/boot
	@sudo cp $(BIN_DIR)/wivos.elf test_image/wivos.elf
	@sudo cp boot/limine.conf test_image/boot/limine.conf
	@sudo cp boot/qemu-virt.dtb test_image/boot/qemu-virt.dtb
	@sudo mkdir -p test_image/EFI/BOOT
	@sudo cp boot/BOOTX64.EFI test_image/EFI/BOOT/
	@sudo cp boot/BOOTRISCV64.EFI test_image/EFI/BOOT/
	@sudo cp -rf $(INITRD_DIR)/. test_image/
	@sync
	@sudo umount test_image/

	@sudo losetup -d `cat loopback_dev`
	@rm -rf test_image loopback_dev

qemu-riscv64: $(BIN_DIR)/image.hdd
	source ~/.bashrc && qemu-system-riscv64 -cpu rv64 -m 1024M -M virt -device ramfb -device qemu-xhci -device usb-kbd --no-reboot --no-shutdown -drive if=pflash,unit=0,format=raw,file=boot/OVMF-riscv64.fd,readonly=on -drive file=$(BIN_DIR)/image.hdd,if=none,id=nvm -device nvme,serial=deadbeef,drive=nvm -serial mon:stdio

qemu-x86_64: $(BIN_DIR)/image.hdd
	source ~/.bashrc && qemu-system-x86_64 -enable-kvm -cpu host -m 1024M -M q35 -bios boot/OVMF-x86_64.fd -net none -smp 4 -drive file=$(BIN_DIR)/image.hdd,if=none,id=nvm -device nvme,serial=deadbeef,drive=nvm -debugcon stdio -device virtio-vga-gl,disable-legacy=on,xres=1280,yres=1024 -display sdl,gl=on -device qemu-xhci -device usb-kbd -device usb-tablet --no-reboot --no-shutdown

qemu-x86_64-debug:
	qemu-system-x86_64 -enable-kvm -cpu host -m 1024M -M q35 -bios boot/OVMF-x86_64.fd -net none -smp 4 -drive file=$(BIN_DIR)/image.hdd,if=none,id=nvm -device nvme,serial=deadbeef,drive=nvm -debugcon stdio -device virtio-vga-gl,xres=1280,yres=1024 -display sdl,gl=on --trace "virtio_gpu_cmd_*" --trace "virtio*" -device qemu-xhci -device usb-kbd --no-reboot --no-shutdown

clean:
	rm -fr build

clean-all: clean
	rm -fr bin $(BIN_DIR) $(BUILD_DIR) test_image

#toolchain:
#	@pushd toolchain/build ; \
#	DESTDIR=$(TOOLCHAIN_DESTDIR) ninja install ; \
#	popd