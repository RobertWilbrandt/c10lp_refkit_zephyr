#include <zephyr.h>
#include <drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0_LABEL DT_GPIO_LABEL(LED0_NODE, gpios)
#define LED0_PIN DT_GPIO_PIN(LED0_NODE, gpios)
#else  // DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Could not find alias led0"
#define LED0_LABEL ""
#define LED0_PIN 0
#endif  // DT_NODE_HAS_STATUS(LED0_NODE, okay)

void main(void)
{
  /*
   * Initialize led0
   */
  const struct device* led0_dev = device_get_binding(LED0_LABEL);
  if (led0_dev == NULL)
  {
    printk("Could not get binding for device %s\n", LED0_LABEL);
  }
  else
  {
    printk("Successfully got binding for device %s\n", LED0_LABEL);
  }

  int ret =
      gpio_pin_configure(led0_dev, LED0_PIN, GPIO_OUTPUT | GPIO_ACTIVE_HIGH);
  if (ret < 0)
  {
    printk("Error while initializing led0 (pin %d) gpio: %d\n", LED0_PIN, ret);
  }
  else
  {
    printk("Successfully initialized led0 on pin %d\n", LED0_PIN);
  }

  bool led0_state = false;

  /*
   * Main loop
   */
  struct k_timer timer;
  k_timer_init(&timer, NULL, NULL);

  k_timer_start(&timer, K_SECONDS(1), K_SECONDS(1));
  while (1)
  {
    k_timer_status_sync(&timer);

    led0_state = !led0_state;
    gpio_pin_set(led0_dev, LED0_PIN, (int)led0_state);
  }
}
