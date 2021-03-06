#ifndef __VERSATILEPB_H
#define __VERSATILEPB_H

typedef enum {
  DEFAULT = 0,
  NUM_IRQS = 255
} IRQn_Type;

/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0224i/Bbabegge.html */
#define UART0       ((volatile unsigned int*)0x101f1000)

#define UART_IRQ      (194)

/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0183g/I18381.html */
#define UARTFR        0x06 /* 0x18 bytes */
#define UARTIMSC      0x0E /* 0x38 bytes */
#define UARTICR       0x11 /* 0x44 bytes */
#define UARTFR_RXFE   0x10
#define UARTFR_TXFF   0x20
#define UARTIMSC_RXIM 0x10
#define UARTIMSC_TXIM 0x20
#define UARTICR_RXIC  0x10
#define UARTICR_TXIC  0x20

/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0271d/index.html */
#define TIMER0         ((volatile unsigned int*)0x101E2000)
#define TIMER_VALUE    0x1 /* 0x04 bytes */
#define TIMER_CONTROL  0x2 /* 0x08 bytes */
#define TIMER_INTCLR   0x3 /* 0x0C bytes */
#define TIMER_MIS      0x5 /* 0x14 bytes */
/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0271d/Babgabfg.html */
#define TIMER_EN       0x80
#define TIMER_PERIODIC 0x40
#define TIMER_INTEN    0x20
#define TIMER_32BIT    0x02
#define TIMER_ONESHOT  0x01

#define PIC_BASE          0x10140000

#define PIC_IntStatus     (*(volatile unsigned int *)(PIC_BASE))
#define PIC_IntEnable     (*(volatile unsigned int *)(PIC_BASE  + 0x10))
#define PIC_IntEnClear    (*(volatile unsigned int *)(PIC_BASE  + 0x14))

#define PIC_INT_SOURCE_UART0  (12)

/* The IRQ state */

typedef uint32_t irq_state_t;

#endif /* __VERSATILEPB_H */
