#include <zephyr.h>
#include <drivers/gpio.h>

#define LED_BAR_NODE DT_NODELABEL(led_bar_gpio)

#if DT_NODE_HAS_STATUS(LED_BAR_NODE, okay)

#define LED_BAR_LABEL DT_LABEL(LED_BAR_NODE)
#define LED_BAR_NGPIOS DT_PROP(LED_BAR_NODE, ngpios)

#else  // DT_NODE_HAS_STATUS(LED_BAR_NODE, okay)

#error "Could not find led bar in device tree"
#define LED_BAR_LABEL ""
#define LED_BAR_NGPIOS 0

#endif  // DT_NODE_HAS_STATUS(LED_BAR_NODE, okay)

void main(void)
{
  /*
   * Initialize leds
   */
  const struct device* led_bar_dev = device_get_binding(LED_BAR_LABEL);
  if (led_bar_dev == NULL)
  {
    printk("Could not get binding for device %s\n", LED_BAR_LABEL);
  }
  else
  {
    printk("Successfully got binding for device %s\n", LED_BAR_LABEL);
  }

  for (int i = 0; i < LED_BAR_NGPIOS; ++i)
  {
    int ret =
        gpio_pin_configure(led_bar_dev, i, GPIO_OUTPUT | GPIO_ACTIVE_HIGH);
    if (ret < 0)
    {
      printk("Error while initializing led bar pin %d: %d\n", i, ret);
    }
    else
    {
      printk("Successfully initialized led bar pin %d\n", i);
    }
  }

  int led_bar_cnt = 0;

  /*
   * Main loop
   */
  struct k_timer timer;
  k_timer_init(&timer, NULL, NULL);

  k_timer_start(&timer, K_MSEC(100), K_MSEC(100));
  while (1)
  {
    k_timer_status_sync(&timer);

    gpio_pin_toggle(led_bar_dev, led_bar_cnt);

    led_bar_cnt++;
    if (led_bar_cnt >= LED_BAR_NGPIOS)
    {
      led_bar_cnt = 0;
    }
  }
}
