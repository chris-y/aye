CC := gcc
CFLAGS :=
LIBS :=

OBJS := aye.o
DEPS := 

all: aye

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

aye: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
