#include <zephyr.h>

void print_msg(struct k_timer* timer);

void main(void)
{
  struct k_timer timer;
  k_timer_init(&timer, print_msg, NULL);

  k_timer_start(&timer, K_SECONDS(1), K_SECONDS(1));
  while (1) {}
}

void print_msg(struct k_timer* timer)
{
  printk("Hello World!\n");
}
