CC := gcc
CFLAGS := 
LIBS :=

OBJS := aye.o zxay_load.o zxay_dump.o
DEPS := zxay.h zxay_file.h zxay_load.h zxay_dump.h

all: aye

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

aye: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
