"""Small SoC definition for zephyr to run on"""
import logging

from litex.soc.cores.gpio import GPIOIn, GPIOOut
from litex_boards.targets.c10lprefkit import BaseSoC


class ZephyrSoC(BaseSoC):
    def __init__(self, sys_clk_freq, *args, **kwargs):
        self._logger = logging.getLogger("ZephyrSoC")

        self.mem_map["rom"] = 0x00000000
        self.mem_map["main_ram"] = 0x40000000
        self.mem_map["csr"] = 0xE0000000

        self.csr_map["ctrl"] = 0
        self.csr_map["uart"] = 3
        self.csr_map["timer0"] = 5
        self.csr_map["sdram"] = 6

        self.interrupt_map["timer0"] = 1
        self.interrupt_map["uart"] = 2

        kwargs["max_sdram_size"] = 0x10000000

        super().__init__(
            cpu_type="vexriscv",
            cpu_variant="full",
            csr_data_width=8,
            *args,
            **kwargs,
        )  # TODO: Test if we actually need "full" for ecall
