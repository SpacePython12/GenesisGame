CC := m68k-elf-gcc
AS80 := z80asm
LD := m68k-elf-ld

SRCDIR := main/src/
INCDIR := main/include/

CFLAGS := -nostdlib -fno-builtin -ffreestanding -fomit-frame-pointer

CSOURCES := $(shell find $(SRCDIR) -name '*.c')
ASOURCES := $(shell find $(SRCDIR) -name '*.S')
ZSOURCES := $(shell find $(SRCDIR) -name '*.s80')
OBJECTS := $(patsubst $(SRCDIR)%.c,$(SRCDIR)%.o,$(CSOURCES)) $(patsubst $(SRCDIR)%.S,$(SRCDIR)%.o,$(ASOURCES)) 
ZBINS := $(patsubst $(SRCDIR)%.s80,$(SRCDIR)%.bin,$(ZSOURCES)) 

image.bin: boot/header.o $(OBJECTS) main/data/data.o
	$(LD) $^ -o image.o
	$(LD) $^ -o $@ --oformat=binary

boot/header.o: boot/header.S
	$(CC) -c $< -o $@ $(CFLAGS)

main/data/data.o: main/data/data.S $(ZBINS)
	$(CC) -c $< -o $@ -Imain $(CFLAGS)

$(SRCDIR)%.o: $(SRCDIR)%.c
	$(CC) -c $< -o $@ -I$(INCDIR) $(CFLAGS)
#	$(CC) -c $< -o $@.S -S -I$(INCDIR) $(CFLAGS)

$(SRCDIR)%.o: $(SRCDIR)%.S
	$(CC) -c $< -o $@ -I$(INCDIR) $(CFLAGS)

$(SRCDIR)%.bin: $(SRCDIR)%.s80
	$(AS80) $< -o $@

clean:
	rm -f $(OBJECTS) $(patsubst $(SRCDIR)%.o,$(SRCDIR)%.o.S,$(OBJECTS)) $(ZBINS) main/image.bin main/image.o main/data/data.o