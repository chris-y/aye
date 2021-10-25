CC := gcc
CFLAGS := 
LIBS :=

OBJS := aye.o zxay_load.o
DEPS := zxay.h zxay_file.h zxay_load.h

all: aye

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

aye: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
