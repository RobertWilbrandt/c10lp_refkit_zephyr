#include <zephyr.h>
#include <stdio.h>
#include <drivers/gpio.h>
#include <drivers/hwinfo.h>

#include <zephyr.h>
#include "memtest.h"

#define HWINFO_NODE DT_NODELABEL(dna0)
#define LED_BAR_NODE DT_NODELABEL(led_bar)
#define SWITCHES_NODE DT_NODELABEL(switches)

/*
 * Hardware info
 */
#if !DT_NODE_HAS_STATUS(HWINFO_NODE, okay)

#error "Could not find hwinfo in device tree"

#endif  // if DT_NODE_HAS_STATUS(HWINFO_NODE, okay)

/*
 * LED bar
 */
#if DT_NODE_HAS_STATUS(LED_BAR_NODE, okay)

#define LED_BAR_LABEL DT_LABEL(LED_BAR_NODE)
#define LED_BAR_NGPIOS DT_PROP(LED_BAR_NODE, ngpios)

#else  // DT_NODE_HAS_STATUS(LED_BAR_NODE, okay)

#error "Could not find led bar in device tree"
#define LED_BAR_LABEL ""
#define LED_BAR_NGPIOS 0

#endif  // DT_NODE_HAS_STATUS(LED_BAR_NODE, okay)

/*
 * Switches
 */
#if DT_NODE_HAS_STATUS(SWITCHES_NODE, okay)

#define SWITCHES_LABEL DT_LABEL(SWITCHES_NODE)
#define SWITCHES_NGPIOS DT_PROP(SWITCHES_NODE, ngpios)

#else  // DT_NODE_HAS_STATUS(SWITCHES_NODE, okay)

#error "Could not find switches in device tree"
#define SWITCHES_LABEL ""
#define SWITCHES_NGPIOS 0

#endif  // DT_NODE_HAS_STATUS(SWITCHES_NODE, okay)

void print_hwinfo();

const struct device* init_led_bar();
const struct device* init_switches();

void main(void)
{
  print_hwinfo();
  memtest();

  const struct device* led_bar_dev = init_led_bar();
  const struct device* switches_dev = init_switches();

  const gpio_port_pins_t led_bar_port_mask = BIT_MASK(8);
  const gpio_port_value_t led_bar_pattern = BIT_MASK(4);
  int led_bar_cnt = 0;

  gpio_port_value_t last_switches = 0;

  /*
   * Main loop
   */
  struct k_timer timer;
  k_timer_init(&timer, NULL, NULL);

  k_timer_start(&timer, K_MSEC(100), K_MSEC(100));
  while (1)
  {
    k_timer_status_sync(&timer);

    // React to switches
    gpio_port_value_t switches;
    gpio_port_get(switches_dev, &switches);

    gpio_port_value_t switches_switched = switches ^ last_switches;
    if (switches_switched != 0)
    {
      for (int i = 0; i < SWITCHES_NGPIOS; ++i)
      {
        if (switches_switched & BIT(i))
        {
          if (switches & BIT(i))
          {
            printf("Switch %d pressed\n", i);
          }
          else
          {
            printf("Switch %d released\n", i);
          }
        }
      }
    }

    last_switches = switches;

    // Animate led bar
    gpio_port_set_masked(led_bar_dev, led_bar_port_mask,
                         (led_bar_pattern << led_bar_cnt) |
                             (led_bar_pattern >>
                              (8 - led_bar_cnt)));  // rotating left-shift

    led_bar_cnt++;
    if (led_bar_cnt >= LED_BAR_NGPIOS)
    {
      led_bar_cnt = 0;
    }
  }
}

void print_hwinfo()
{
  const size_t buf_len = DT_PROP_BY_IDX(HWINFO_NODE, reg, 1) / sizeof(uint32_t);
  uint8_t buf[buf_len + 1];

  ssize_t ret = hwinfo_get_device_id(buf, buf_len);
  if (ret < 0)
  {
    printf("Error while reading hardware info: %d\n", ret);
    return;
  }
  else
  {
    buf[ret] = 0;
    printf("Hardware info: %s [%d/%d]\n", buf, ret, buf_len);
  }
}

const struct device* init_led_bar()
{
  const struct device* led_bar_dev = device_get_binding(LED_BAR_LABEL);
  if (led_bar_dev == NULL)
  {
    printf("Could not get binding for device %s\n", LED_BAR_LABEL);
    return NULL;
  }
  else
  {
    printf("Successfully got binding for device %s\n", LED_BAR_LABEL);
  }

  for (int i = 0; i < LED_BAR_NGPIOS; ++i)
  {
    int ret =
        gpio_pin_configure(led_bar_dev, i, GPIO_OUTPUT | GPIO_ACTIVE_HIGH);
    if (ret < 0)
    {
      printf("Error while initializing led bar pin %d: %d\n", i, ret);
      return NULL;
    }
    else
    {
      printf("Successfully initialized led bar pin %d\n", i);
    }
  }

  return led_bar_dev;
}

const struct device* init_switches()
{
  const struct device* switches_dev = device_get_binding(SWITCHES_LABEL);
  if (switches_dev == NULL)
  {
    printf("Could not get binding for device %s\n", SWITCHES_LABEL);
    return NULL;
  }
  else
  {
    printf("Successfully got binding for device %s\n", SWITCHES_LABEL);
  }

  for (int i = 0; i < SWITCHES_NGPIOS; ++i)
  {
    int ret = gpio_pin_configure(switches_dev, i, GPIO_INPUT | GPIO_ACTIVE_LOW);
    if (ret < 0)
    {
      printf("Error while initializing switch %d: %d\n", i, ret);
      return NULL;
    }
    else
    {
      printf("Successfully initialized switch %d\n", i);
    }
  }

  return switches_dev;
}
