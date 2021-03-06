#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <source/ff.h>
#include <vfs.h>
#include <stdio.h>
#include <rtc.h>

#include <sensors/bme680/bme680.h>
#include <sensors/bme680/bme_680_main.h>

#ifdef CONFIG_LIBRARY_BSEC
#include <bsec/inc/bsec_interface.h>
#include "bsec_lib/bsec_integration.h"
#endif

#ifdef CONFIG_SENSOR_PMSA003
#include <sensors/pmsa003/pmsa003.h>
#endif

#define TIMER_0_BASE_ADDRESS    (0x40008000)
#define TIMER_FUNCTION_REGISTER(REG_OFFSET)    (*(volatile uint32_t *)((TIMER_0_BASE_ADDRESS) \
                                        + (REG_OFFSET)))                      \


static int g_sensor_fd;

static const char *get_name_from_iaq_index(uint32_t index)
{
  if (index >= 0 && index <= 50)
    /* Pure air; best for well-being */
    return "excellent";
  else if (index > 50 && index <= 100)
    /* No irritation or impact on well-being */
    return "good";
  else if (index > 100 && index <= 150)
    /* Reduction of well-being possible */
    return "lightly polluted";
  else if (index > 150 && index <= 200)
    /* More significant irritation possible */
    return "moderated polluted";
  else if (index > 200 && index <= 250)
    /* Exposition might lead to effects like headache depending on type
     * of VOCs */
    return "heavily polluted";
  else if (index > 250 && index <= 350)
    /* More severe health issue possible if harmful VOC present */
    return "severely polluted";
  else
    /* Headaches, additional neurotoxic effects possible */
    return "exteremely polluted";
}

#ifdef CONFIG_LIBRARY_BSEC
static void bsec_out_data(int64_t time_stamp, float iaq, uint8_t iaq_accuracy,
 float temperature, float humidity, float pressure, float raw_temperature,
 float raw_humidity, float gas, bsec_library_return_t bsec_status,
 float static_iaq, float co2_equivalent, float breath_voc_equivalent)
{
  uint32_t int_iaq = (uint32_t)iaq;
  char print_buffer[130] = {0};
  uint32_t temperature_real = temperature * 100;
  current_time_t my_time;
  uint32_t voc = breath_voc_equivalent * 100;

  int rtc_fd = open(CONFIG_RTC_PATH, 0);
  if (rtc_fd >= 0)
  {
    read(rtc_fd, &my_time, sizeof(my_time));
    close(rtc_fd);
  }

  snprintf(print_buffer, sizeof(print_buffer),
           "[%02u:%02u:%02u] %s IAQ %d, ACCURACY %d, "
           "VOC %d.%d ppm, CO2 %d ppm, "
           "Temp %d.%dC, Humidity %d, Pressure %d.%d Pa\r\n",
           my_time.g_hours, my_time.g_minute, my_time.g_second,
           get_name_from_iaq_index(int_iaq),
           int_iaq,
           iaq_accuracy,
           voc / 100, (uint32_t)(voc * 100),
           (uint32_t)(co2_equivalent),
           temperature_real / 100, temperature_real % 100,
           (uint32_t)(humidity),
           (uint32_t)pressure / 100, ((uint32_t)pressure) % 100);

  printf("%s\r\n",
         print_buffer);

#ifdef CONFIG_SENSOR_LOG
  int logger_fd = open(CONFIG_SENSOR_LOG_FILENAME, O_APPEND);
  if (logger_fd < 0) {
    printf("Error %d open LOGGER1\n", logger_fd);
    return;
  }

  write(logger_fd, print_buffer, strlen(print_buffer));
  close(logger_fd);
#endif /* CONFIG_SENSOR_LOG */
}
#endif /* CONFIG_LIBRARY_BSEC */

static uint32_t state_load(uint8_t *state_buffer, uint32_t n_buffer)
{
  return 0;
}

static void state_save(const uint8_t *state_buffer, uint32_t length)
{
}

static uint32_t config_load(uint8_t *config_buffer, uint32_t n_buffer)
{
  return 0;
}

static int64_t get_timestamp_us(void)
{
  TIMER_FUNCTION_REGISTER(0x040) = 1;
  volatile uint64_t cc = TIMER_FUNCTION_REGISTER(0x540);
  return cc;
}

static void sleep(uint32_t t_ms)
{
  usleep(100 * t_ms);
}

static int8_t bus_read(uint8_t dev_addr, uint8_t reg_addr,
  uint8_t *reg_data_ptr, uint16_t data_len)
{
  bme680_sensor_spi_transaction_t transaction = {
    .dev_addr = dev_addr,
    .reg_addr = reg_addr,
    .reg_data_ptr = reg_data_ptr,
    .data_len     = data_len,
  };

  int8_t ret = ioctl(g_sensor_fd, IO_BME680_BUS_READ, (unsigned long)&transaction);
  return ret;
}

static int8_t bus_write(uint8_t dev_addr, uint8_t reg_addr,
  uint8_t *reg_data_ptr, uint16_t data_len)
{
  bme680_sensor_spi_transaction_t transaction = {
    .dev_addr = dev_addr,
    .reg_addr = reg_addr,
    .reg_data_ptr = reg_data_ptr,
    .data_len     = data_len,
  };

  int8_t ret = ioctl(g_sensor_fd, IO_BME680_BUS_WRITE, (unsigned long)&transaction);
  return ret;
}

static void sensor_measure_print_usage(void)
{
  printf("Usage: sensor_measure <sensor_type>\n"
         "where sensor type: bma680 or pmsa003\n");
}

static int sensor_measure_bma680(void)
{
  int ret;
#ifdef CONFIG_LIBRARY_BSEC
  return_values_init bsec_ret;
#endif
  g_sensor_fd = open(CONFIG_SENSOR_BME680_PATH_NAME, 0);
  if (g_sensor_fd < 0) {
    printf("Error %d open\n", g_sensor_fd);
    return g_sensor_fd;
  }

#ifdef CONFIG_LIBRARY_BSEC
  bsec_ret = bsec_iot_init(BSEC_SAMPLE_RATE_LP, 0.0f, bus_write, bus_read, sleep,
    state_load, config_load);
  if (bsec_ret.bme680_status) {
    printf("Error init BSEC lib %d\n", bsec_ret.bme680_status);
    close(g_sensor_fd);
    return -EINVAL;
  }

  bsec_iot_loop(sleep, get_timestamp_us, bsec_out_data, state_save, 10000);
#endif
  close(g_sensor_fd);
  return ret;
}

static inline uint16_t convert_to_littleendian(uint16_t data)
{
  return ((data & 0xFF) << 8) | ((data & 0xFF00) >> 8);
}

static int sensor_measure_pmsa003(void)
{
  int ret = OK, fd;

#ifdef CONFIG_SENSOR_PMSA003
  pmsa003_msg_t data_sample;

  ret = open(CONFIG_SENSOR_PMSA003_PATH_NAME, 0);
  if (ret < 0) {
    printf("Error %d open pmsa003 sensor\n", ret);
    return ret;
  }

  fd = ret;

  ret = ioctl(fd, IO_PMSA003_ENTER_NORMAL, 0);
  if (ret < 0) {
    printf("Error %d cannot enter normal mode\n", ret);
    close(fd);
    return ret;
  }

  current_time_t my_time;
  int rtc_fd = open(CONFIG_RTC_PATH, 0);
  if (rtc_fd < 0)
  {
    printf("[ERROR] %d open RTC\n", rtc_fd);
  }

  for (;;) {
    ret = read(fd, &data_sample, sizeof(pmsa003_msg_t));
    if (ret < 0) {
      printf("Error %d reading pmsa003 sensor", ret);
      close(fd);
      return ret;
    }

    ret = read(rtc_fd, &my_time, sizeof(my_time));
    if (ret < 0) {
      printf("[ERROR] %d read RTC\n", ret);
      close(rtc_fd);
      close(fd);
      return ret;
    }

    printf("[%02u:%02u:%02u] pm1.0: %d pm2.5: %d pm10: %d\n",
           my_time.g_hours, my_time.g_minute, my_time.g_second,
           convert_to_littleendian(data_sample.pm1_0),
           convert_to_littleendian(data_sample.pm2_5),
           convert_to_littleendian(data_sample.pm2_5));
  }

#endif
  return ret;
}

int console_sensor_measure(int argc, const char *argv[])
{
  int ret;

  if (argc != 2) {
    sensor_measure_print_usage();
    return -1;
  }

  if (!strcmp(argv[1], "bma680")) {
    return sensor_measure_bma680();
  } else if (!strcmp(argv[1], "pmsa003")) {
    return sensor_measure_pmsa003();
  } else {
    printf("Unkown parameter: %s\n", argv[1]);
  }

  return OK;
}
