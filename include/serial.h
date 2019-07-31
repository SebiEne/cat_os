#ifndef __SERIAL_H
#define __SERIAL_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define UART_TX_BUFFER                      (128)
#define UART_RX_BUFFER                      (128)

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Further declaration */

struct uart_lower_s;

/* Lowerhalf callback that should be implemented by the serial driver */

typedef int (*uart_lowerhalf_open)(const struct uart_lower_s *lower);
typedef int (*uart_lowerhalf_close)(const struct uart_lower_s *lower);
typedef int (*uart_lowerhalf_write)(const struct uart_lower_s *lower,
                                    const void *ptr_data,
                                    unsigned int sz);
typedef int (*uart_lowerhalf_read)(const struct uart_lower_s *lower,
                                   void *data,
                                   unsigned int max_buf_sz);
typedef int (*uart_lowerhalf_ioctl)(const struct uart_lower_s *lower);

/* The lower half structure used by the serial driver */

struct uart_lower_s {
  void *priv;
  uart_lowerhalf_open  open_cb;
  uart_lowerhalf_close close_cb;
  uart_lowerhalf_write write_cb;
  uart_lowerhalf_read  read_cb;
  uart_lowerhalf_ioctl ioctl_cb;
};

/* The upper half structure */

struct uart_upper_s {
  uint8_t rx_buffer[UART_RX_BUFFER];
  uint8_t tx_buffer[UART_TX_BUFFER];
  const struct uart_lower_s *lower;
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int uart_low_init(void);

int uart_low_send(char *msg);

char uart_low_receive(void);

int uart_register(const char *name, const struct uart_lower_s *uart_lowerhalf);

int uart_init(void);

#endif /* __SERIAL_H */
