CC := gcc
CFLAGS := 
LIBS :=

OBJS := aye.o zxay_load.o zxay_dump.o zxay_peek.o zxay_save.o zxay_misc.o zxay_poke.o zxay_data.o
DEPS := zxay.h zxay_consts.h zxay_file.h zxay_load.h zxay_dump.h zxay_peek.h zxay_save.h zxay_misc.h zxay_poke.h zxay_data.h

all: aye

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

aye: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
