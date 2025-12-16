FILES = ./build/kernel.asm.o ./build/kernel.o \
        ./build/disk/disk.o ./build/disk/streamer.o \
        ./build/fs/pparser.o ./build/fs/file.o ./build/fs/fat/fat16.o \
        ./build/string/string.o ./build/timer/timer.o ./build/keyboard/keyboard.o \
        ./build/idt/idt.asm.o ./build/idt/idt.o \
        ./build/memory/memory.o \
		./build/libc/ctype.o ./build/stdio/stdio_impl.o \
		./build/stdio/stdio.o ./build/stdlib/stdlib.o \
        ./build/io/io.asm.o ./build/graphics/vga.o \
        ./build/gdt/gdt.o ./build/gdt/gdt.asm.o \
        ./build/memory/heap/heap.o ./build/memory/heap/kheap.o \
        ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/errno.o \
		./build/breakout/breakout_audio.o \
		./build/breakout/breakout_graphics.o \
		./build/breakout/breakout_main.o \
		./build/breakout/breakout_menu.o \
		./build/breakout/breakout_particles.o \
		./build/breakout/breakout_physics.o \
		./build/breakout/breakout_powerups.o \
		./build/breakout/breakout_ui.o

INCLUDES = -I./src -I./src/stdlib -I./src/stdio -I./src/string
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops \
        -fstrength-reduce -fomit-frame-pointer -finline-functions \
        -fno-builtin -nostdlib -nostartfiles -nodefaultlibs \
        -Wall -O0 -Iinc \
        -Wno-unused-function -Wno-unused-label -Wno-unused-parameter \
        -Wno-unused-variable -Wno-cpp -Wno-implicit-function-declaration

WAD_PATH := src/doomgeneric/Doom_UserFiles/doom1.wad

all: ./bin/boot.bin ./bin/kernel.bin
	rm -f ./bin/os.bin
	truncate -s 128M ./bin/os.bin
	dd if=./bin/boot.bin   of=./bin/os.bin bs=512 conv=notrunc
	dd if=./bin/kernel.bin of=./bin/os.bin bs=512 seek=1 conv=notrunc

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

./build/kernel.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o

./build/idt/idt.asm.o: ./src/idt/idt.asm
	nasm -f elf -g ./src/idt/idt.asm -o ./build/idt/idt.asm.o

./build/idt/idt.o: ./src/idt/idt.c
	i686-elf-gcc $(INCLUDES) -I./src/idt $(FLAGS) -std=gnu99 -c ./src/idt/idt.c -o ./build/idt/idt.o

./build/gdt/gdt.o: ./src/gdt/gdt.c
	i686-elf-gcc $(INCLUDES) -I./src/gdt $(FLAGS) -std=gnu99 -c ./src/gdt/gdt.c -o ./build/gdt/gdt.o

./build/gdt/gdt.asm.o: ./src/gdt/gdt.asm
	nasm -f elf -g ./src/gdt/gdt.asm -o ./build/gdt/gdt.asm.o

./build/memory/memory.o: ./src/memory/memory.c
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c ./src/memory/memory.c -o ./build/memory/memory.o

./build/io/io.asm.o: ./src/io/io.asm
	nasm -f elf -g ./src/io/io.asm -o ./build/io/io.asm.o

./build/memory/heap/heap.o: ./src/memory/heap/heap.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/heap.c -o ./build/memory/heap/heap.o

./build/memory/heap/kheap.o: ./src/memory/heap/kheap.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/kheap.c -o ./build/memory/heap/kheap.o

./build/memory/paging/paging.o: ./src/memory/paging/paging.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/paging $(FLAGS) -std=gnu99 -c ./src/memory/paging/paging.c -o ./build/memory/paging/paging.o

./build/memory/paging/paging.asm.o: ./src/memory/paging/paging.asm
	nasm -f elf -g ./src/memory/paging/paging.asm -o ./build/memory/paging/paging.asm.o

./build/disk/disk.o: ./src/disk/disk.c
	i686-elf-gcc $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/disk.c -o ./build/disk/disk.o

./build/disk/streamer.o: ./src/disk/streamer.c
	i686-elf-gcc $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/streamer.c -o ./build/disk/streamer.o

./build/fs/fat/fat16.o: ./src/fs/fat/fat16.c
	i686-elf-gcc $(INCLUDES) -I./src/fs -I./src/fat $(FLAGS) -std=gnu99 -c ./src/fs/fat/fat16.c -o ./build/fs/fat/fat16.o

./build/fs/file.o: ./src/fs/file.c
	i686-elf-gcc $(INCLUDES) -I./src/fs $(FLAGS) -std=gnu99 -c ./src/fs/file.c -o ./build/fs/file.o

./build/fs/pparser.o: ./src/fs/pparser.c
	i686-elf-gcc $(INCLUDES) -I./src/fs $(FLAGS) -std=gnu99 -c ./src/fs/pparser.c -o ./build/fs/pparser.o

./build/string/string.o: ./src/string/string.c
	i686-elf-gcc $(INCLUDES) -I./src/string $(FLAGS) -std=gnu99 -c ./src/string/string.c -o ./build/string/string.o

./build/timer/timer.o: ./src/timer/timer.c
	i686-elf-gcc $(INCLUDES) -I./src/timer $(FLAGS) -std=gnu99 -c ./src/timer/timer.c -o ./build/timer/timer.o

./build/keyboard/keyboard.o: ./src/keyboard/keyboard.c
	i686-elf-gcc $(INCLUDES) -I./src/keyboard $(FLAGS) -std=gnu99 -c ./src/keyboard/keyboard.c -o ./build/keyboard/keyboard.o

./build/graphics/vga.o: ./src/graphics/vga.c
	i686-elf-gcc $(INCLUDES) -I./src/graphics $(FLAGS) -std=gnu99 -c ./src/graphics/vga.c -o ./build/graphics/vga.o

./build/errno.o: ./src/errno.c
	i686-elf-gcc $(INCLUDES) -I./src $(FLAGS) -std=gnu99 -c ./src/errno.c -o ./build/errno.o
# ADDED: ctype implementation
./build/libc/ctype.o: ./src/libc/ctype.c
	mkdir -p ./build/libc
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/libc/ctype.c -o ./build/libc/ctype.o

# ADDED: stdio implementation  
./build/stdio/stdio_impl.o: ./src/stdio/stdio_impl.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/stdio/stdio_impl.c -o ./build/stdio/stdio_impl.o

./build/stdio/stdio.o: ./src/stdio/stdio.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/stdio/stdio.c -o ./build/stdio/stdio.o

./build/stdlib/stdlib.o: ./src/stdlib/stdlib.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/stdlib/stdlib.c -o ./build/stdlib/stdlib.o

./build/breakout/%.o: ./src/breakout/%.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@
	
clean:
	rm -rf ./bin/boot.bin
	rm -rf ./bin/kernel.bin
	rm -rf ./bin/os.bin
	rm -rf ${FILES}
	rm -rf ./build/kernelfull.o