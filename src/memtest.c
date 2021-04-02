#include "memtest.h"

#include <zephyr.h>

#define MEMORY_REGIONS_NODE DT_PATH(soc, memory_regions)
#define MEMORY_REGION_LIST(node_id)                                            \
  { DT_LABEL(node_id), DT_REG_ADDR(node_id), DT_REG_SIZE(node_id) },

#define ZEPHYR_SRAM_NODE DT_CHOSEN(zephyr_sram)

struct memory_region
{
  const char* name;
  size_t addr;
  size_t size;
};

const struct memory_region memory_regions[] = { DT_FOREACH_CHILD(
    MEMORY_REGIONS_NODE, MEMORY_REGION_LIST) };

void memtest()
{
  printk("\nRunning memtest...\n");

  size_t num_memory_regions =
      sizeof(memory_regions) / sizeof(struct memory_region);
  for (size_t i = 0; i < num_memory_regions; ++i)
  {
    if (memory_regions[i].addr == DT_REG_ADDR(ZEPHYR_SRAM_NODE))
    {
      printk("[%zu/%zu] (Skipping sram %s used by Zephyr)\n", i,
             num_memory_regions, memory_regions[i].name);
      continue;
    }

    printk("[%zu/%zu] Checking region %s at 0x%08x with size 0x%x\n", i,
           num_memory_regions, memory_regions[i].name, memory_regions[i].addr,
           memory_regions[i].size);

    volatile size_t* test_base = (void*)memory_regions[i].addr;
    const size_t test_num = memory_regions[i].size / sizeof(size_t);

    printk("      Writing increasing counter...     0%%");
    for (size_t i = 0; i < test_num; ++i)
    {
      test_base[i] = i;
      if ((i % 256) == 0)
      {
        printk("\b\b\b\b%3d%%", i * 100 / test_num);
      }
    }
    printk("\b\b\b\b100%%\n");

    printk("      Checking increasing counter...    0%%");
    for (size_t i = 0; i < test_num; ++i)
    {
      size_t test_val = test_base[i];
      if (test_val != i)
      {
        printk("\nERROR! Incorrect value %zu at address 0x%08x (index %zu), "
               "expected %zu\n\n",
               test_val, (size_t)&test_base[i], i, i);
        return;
      }

      if ((i % 256) == 0)
      {
        printk("\b\b\b\b%3d%%", i * 100 / test_num);
      }
    }
    printk("\b\b\b\b100%%\n");
  }

  printk("...done\n\n");
}
