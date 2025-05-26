all:  bootloader kernel userland toolchain image

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
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean
	cd Toolchain; make clean
	rm -rf Userland/native/tests/*.o
	# Remove ELF and BIN files from the project root
	rm -f *.elf *.bin
	# Clean kernel directory
	rm -f Kernel/*.elf
	# Clean userland directory
	rm -f Userland/*.elf Userland/*.bin

.PHONY: bootloader image collections kernel userland all clean
