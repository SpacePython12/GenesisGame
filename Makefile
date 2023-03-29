CC := gcc
LD := ld

SRCDIR := main/src/
INCDIR := main/include/
DATDIR := main/data/

NAME := game
EXT := 

CFLAGS := -O3
LIBS := -lSDL2 -lm -lzip

CSOURCES := $(shell find $(SRCDIR) -name '*.c')
ASOURCES := $(shell find $(SRCDIR) -name '*.S')
OBJECTS := $(patsubst $(SRCDIR)%.c,$(SRCDIR)%.o,$(CSOURCES)) $(patsubst $(SRCDIR)%.S,$(SRCDIR)%.o,$(ASOURCES)) 

build: $(NAME)$(EXT)

$(NAME)$(EXT): $(OBJECTS)
	$(CC) $^ -o $@ $(LIBS)
#	$(LD) $^ -o $@ --oformat=binary

$(SRCDIR)%.o: $(SRCDIR)%.c
	$(CC) -c $< -o $@ -I$(INCDIR) $(CFLAGS)
#	$(CC) -c $< -o $@.S -S -I$(INCDIR) $(CFLAGS)

$(SRCDIR)%.o: $(SRCDIR)%.S
	$(CC) -c $< -o $@ -I$(INCDIR) $(CFLAGS)



data: 
	rm -f data.pak
	$(MAKE) -C $(DATDIR) all

clean:
	rm -f $(OBJECTS) $(patsubst $(SRCDIR)%.o,$(SRCDIR)%.o.S,$(OBJECTS)) $(NAME)$(EXT)