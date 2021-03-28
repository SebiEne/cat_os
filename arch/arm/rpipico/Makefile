SRC := $(wildcard *.c)
SRC += $(wildcard */*.c)
OBJS += $(patsubst %.c,%.o,$(SRC))
OBJS += bs2_default.o
ARCH_FLAGS:=${CFLAGS} -I$(TOPDIR)/include -I$(TOPDIR)/s_alloc -I./include/ -I$(TOPDIR)/sched/include

all: $(OBJS)
	echo "Building arch specific"
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS)

%.o : %.c
	${PREFIX}gcc $(ARCH_FLAGS) -c $< -o $@

bs2_default.o:
	${PREFIX}objcopy -I elf32-littlearm -O elf32-littlearm boot/bs2_default.elf bs2_default.o --rename-section .text=.boot2

clean:
	find . -iname "*.o" -exec rm -f {} \;
