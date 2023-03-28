CC := gcc
LD := ld

SRCDIR := main/src/
INCDIR := main/include/

CFLAGS := -lc -lSDL2 -lm

CSOURCES := $(shell find $(SRCDIR) -name '*.c')
ASOURCES := $(shell find $(SRCDIR) -name '*.S')
ZSOURCES := $(shell find $(SRCDIR) -name '*.s80')
OBJECTS := $(patsubst $(SRCDIR)%.c,$(SRCDIR)%.o,$(CSOURCES)) $(patsubst $(SRCDIR)%.S,$(SRCDIR)%.o,$(ASOURCES)) 
ZBINS := $(patsubst $(SRCDIR)%.s80,$(SRCDIR)%.bin,$(ZSOURCES)) 

image.o: $(OBJECTS) main/data/data.o
	$(CC) $^ -o $@ $(CFLAGS)
#	$(LD) $^ -o $@ --oformat=binary

main/data/data.o: main/data/data.S $(ZBINS)
	$(CC) -c $< -o $@ -Imain $(CFLAGS)

$(SRCDIR)%.o: $(SRCDIR)%.c
	$(CC) -c $< -o $@ -I$(INCDIR) $(CFLAGS)
#	$(CC) -c $< -o $@.S -S -I$(INCDIR) $(CFLAGS)

$(SRCDIR)%.o: $(SRCDIR)%.S
	$(CC) -c $< -o $@ -I$(INCDIR) $(CFLAGS)


clean:
	rm -f $(OBJECTS) $(patsubst $(SRCDIR)%.o,$(SRCDIR)%.o.S,$(OBJECTS)) $(ZBINS) main/image.bin main/image.o main/data/data.o