all:  toolchain bootloader kernel userland image

toolchain:
	cd Toolchain; make all
	
bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all

userland:
	cd Userland; make all

image: kernel bootloader userland Toolchain/ModulePacker/mp.bin
	cd Image; make all

Toolchain/ModulePacker/mp.bin:
	cd Toolchain/ModulePacker; make all

clean:
	cd Toolchain; make clean
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean
	# Remove ELF and BIN files from the project root
	rm -f *.elf *.bin

.PHONY: bootloader image collections kernel userland all clean
