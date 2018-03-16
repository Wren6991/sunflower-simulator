TREEROOT	= ../../..
include $(TREEROOT)/conf/setup.conf

PROGRAM		= pedometer
PORT		= ../port
LIBOS		= mOS
LOADADDR	= 0x08004000

TOOLSLIB	= $(TREEROOT)/tools/tools-lib
INCLUDES	= -I../port/  -I$(TREEROOT)/sys/include -I$(TREEROOT)/sys/kern/superH -I$(TREEROOT)/benchmarks/include -I$(TREEROOT)/sys/libOS/port
OPTFLAGS	= -gstabs3 -O0
CFLAGS		= $(TARGET-ARCH-FLAGS) -Wall # Do not do since we are linking mOS for libc implementation:	-nostdlib -fno-builtin 
LDFLAGS		= -Ttext $(LOADADDR) -TsuperH.ld -L$(TOOLSLIB)/$(TARGET) -L$(TREEROOT)/sys/libOS/$(LIBOS) -Map $(PROGRAM).map


OBJS	=\
	init.o\
	main.o\
	pedometer.o\


.PHONY: all clean run install

all:	$(PROGRAM) $(PROGRAM).sr

$(PROGRAM): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ -lc -lgcc -lm -l$(LIBOS)-$(TARGET)

$(PROGRAM).sr:$(PROGRAM)
	$(OBJCOPY) -O srec $(PROGRAM) $@

main.o: main.c Makefile
	$(CC) -std=c99 $(CFLAGS) $(OPTFLAGS) $(INCLUDES) -c $< -o $@

pedometer.o: pedometer.c Makefile
	$(CC) -std=c99 $(CFLAGS) $(OPTFLAGS) $(INCLUDES) -c $< -o $@

init.o: init.S
	$(CPP) init.S > init.i; $(AS) init.i -o $@

clean:
	$(RM) init.i *.o $(PROGRAM) $(PROGRAM).sr $(PROGRAM).map simlog.*

run: all
	sf run.m

install: all
	cp $(PROGRAM).sr ../../../
