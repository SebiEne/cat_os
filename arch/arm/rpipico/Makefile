SRC := board_initialize.c
OBJS += $(patsubst %.c,%.o,$(SRC))
OBJS += bs2_default.o
ARCH_FLAGS:=${CFLAGS} -I$(TOPDIR)/include -I$(TOPDIR)/s_alloc -I./include/ -I$(TOPDIR)/sched/include
LDFLAGS += -Llibbsp.a

all: $(OBJS) libbsp.a
	echo "Building arch specific"
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS) libbsp.a

%.o : %.c
	${PREFIX}gcc $(ARCH_FLAGS) -c $< -o $@

bs2_default.o:
	${PREFIX}objcopy -I elf32-littlearm -O elf32-littlearm boot/bs2_default.elf bs2_default.o --rename-section .text=.boot2

libbsp.a:
#	${PREFIX}objcopy -I elf32-littlearm -O elf32-littlearm boot/bs2_default.elf bs2_default.o --rename-section .text=.boot2
	mkdir build && cd build && cmake .. && make -j4 && cp libbsp.a ..
	rm -rf build

clean:
	rm -rf build
	rm -f libbsp.a
	find . -iname "*.o" -exec rm -f {} \;
