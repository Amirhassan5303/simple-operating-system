all: os.img

boot.bin: boot/boot.asm
	nasm -f bin boot/boot.asm -o boot.bin

entry.o: kernel/kernel_entry.asm
	nasm -f elf32 kernel/kernel_entry.asm -o entry.o

kernel.o: kernel/kernel.c
	gcc -m32 -ffreestanding -nostdlib -fno-pie -fno-PIC -c kernel/kernel.c -o kernel.o

kernel.bin: entry.o kernel.o
	ld -m elf_i386 -T linker.ld --oformat binary entry.o kernel.o -o kernel.bin

os.img: boot.bin kernel.bin
	cat boot.bin kernel.bin > os.img
	truncate -s 1440K os.img

run: os.img
	qemu-system-x86_64 -drive format=raw,file=os.img

clean:
	rm -f *.bin *.o *.img

.PHONY: all run clean
