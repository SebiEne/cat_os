#include <console_main.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

/* Shutdown flag */

static bool g_is_shutdown_set;

/*
 * console_main - console application entry point
 *
 */
void console_main(void)
{
  char cmd_buffer[CONFIG_CMD_BUFER_LEN];
  int uart_fd = open(CONFIG_CONSOLE_UART_PATH, 0);
  if (uart_fd < 0)
  {
    return;
  }

  int len = 0;
  g_is_shutdown_set = false;

  do {
    ssize_t sz = read(uart_fd, cmd_buffer + len, 1);
    if (sz > 0)
    {
      len += sz;
    }

    /* If the character was a terminator interpret the command */

  } while (g_is_shutdown_set);

  /* This app will exit on a reboot/shutdown command */

  close(uart_fd);
}