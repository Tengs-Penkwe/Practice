CFLAGS  += -std=gnu11 -g -I./uthreads
UNAME = $(shell uname)
ifeq ($(UNAME), Linux)
LDFLAGS += -pthread 
endif
EXES = sRead aRead tRead

all: $(EXES)

clean:
	rm -f *.o $(EXES); rm -rf *.dSYM
tidy:
	rm -f *.o; rm -rf *.dSYM

sRead: sRead.o disk.o uthread.o
aRead: aRead.o disk.o uthread.o
tRead: tRead.o disk.o uthread.o
