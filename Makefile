GCC = i686-elf-gcc
GCCFLAGS = -ffreestanding -nostdlib -Iinclude
GDB = gdb

C_SOURCE = ${wildcard device/*.c cpu/*.c cpu/*.s lib/*.c}

all: build/myos.bin

image: build/myos.img

build/myos.img: build/myos.bin
	dd if=$< of=$@ conv=notrunc

build/kernel.bin: boot/head.s kernel/kernel.c ${C_SOURCE}
	$(GCC) $(GCCFLAGS) -Wl,-Ttext=0x0,--oformat=binary  $^ -o $@

build/kernel.elf: boot/head.s kernel/kernel.c ${C_SOURCE}
	$(GCC) $(GCCFLAGS) -g -Wl,-Ttext=0x0  $^ -o $@

build/myos.bin: build/bootsect.bin build/kernel.bin
	dd if=build/bootsect.bin of=$@
	dd if=build/kernel.bin of=$@ seek=1

build/bootsect.bin: boot/bootsect.s 
	$(GCC) $(GCCFLAGS) -Wl,-Ttext=0x0,--oformat=binary  $^ -o $@

run: build/myos.img
	qemu-system-i386 -hda $<

debug: build/myos.img build/kernel.elf
	qemu-system-i386 -s -S -hda build/myos.img -m 16M & \
	${GDB} \
	-ex "target remote localhost:1234" \
	-ex "symbol-file build/kernel.elf" \
	-ex "b *0x7c00" \
	-ex "c"


clean:
	rm build/*.bin build/*.elf