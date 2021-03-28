

MEMORY {
  FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2048k
  RAM(rwx) : ORIGIN =  0x20000000, LENGTH = 256k
  SCRATCH_X(rwx) : ORIGIN = 0x20040000, LENGTH = 4k
  SCRATCH_Y(rwx) : ORIGIN = 0x20041000, LENGTH = 4k
}

SECTIONS {
        /* Second stage bootloader is prepended to the image. It must be 256 bytes big
         * and checksummed. It is usually built by the boot_stage2 target
         * in the Raspberry Pi Pico SDK
         */

        .flash_begin : {
          __flash_binary_start = .;
        } > FLASH

        .boot2 : {
          __boot2_start__ = .;
          KEEP (*(.boot2))
          __boot2_end__ = .;
        } > FLASH
/*
 *	ASSERT(__boot2_end__ - __boot2_start__ == 256,
 *              "ERROR: Pico second stage bootloader must be 256 bytes in size")
 */
        /* The second stage will always enter the image at the start of .text.
         * The debugger will use the ELF entry point, which is the _entry_point
         * symbol if present, otherwise defaults to start of .text.
         * This can be used to transfer control back to the bootrom on debugger
         * launches only, to perform proper flash setup.
         */

        .text :  {
                . = ALIGN(4);
                *(.text)                /* Program code */
                *(.text.*)
                . = ALIGN(4);
        } >FLASH                        /* this section is code, hence gos to FLASH */

        .rodata : {
          . = ALIGN(4);
          _srodata = .;
          *(.rodata)
          *(.rodata.*)
          . = ALIGN(4);
        } >FLASH

        _etext = .;             /* _etext is end of program code */
        .data : {                       /* AT specifies the LMA (load memory address) */
                . = ALIGN(4);           /* also, this section is for initialized
                                             global/static variables */
                _sdata = .;
                *(.data)
                *(.data.*)
                . = ALIGN(4);
                _edata = .;
        } >RAM AT> FLASH                     /* this section go into this RAM */

        .bss : {                        /* this section is for uninitialized global/static
                                        variables, which we set to zero */
                . = ALIGN(4);
                _sbss = .;              /* the . sets the symbol to value of current counter */
                *(.bss)
                *(COMMON)
                . = ALIGN(4);
                _ebss = .;
        } >RAM

        .stack : {
                 KEEP(*(.stack*))
                . = ALIGN(8);
                _sstack = .;
                . = . + 4096;
                . = ALIGN(8);
                _estack = .;
       } >RAM

       .irq_stack : {
                 KEEP(*(.irq_stack*))
                . = ALIGN(8);
                _sirq_stack = .;
                . = . + 2048;
                . = ALIGN(8);
                _eirq_stack = .;
       } >RAM

        .heap : {
                KEEP(*(.heap*))
                . = ALIGN(4);
                _sheap = .;
        } >RAM

      _eheap = ORIGIN(RAM) + LENGTH(RAM);
}
