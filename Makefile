CC := gcc
CFLAGS := 
LIBS :=

OBJS := aye.o zxay_read.o
DEPS := zxay.h zxay_read.h

all: aye

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

aye: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
