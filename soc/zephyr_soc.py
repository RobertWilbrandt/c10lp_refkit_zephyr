"""Small SoC definition for zephyr to run on"""
import logging

from litex.soc.cores.gpio import GPIOOut
from litex_boards.targets.c10lprefkit import BaseSoC


class ZephyrSoC(BaseSoC):
    def __init__(self, sys_clk_freq, *args, **kwargs):
        self._logger = logging.getLogger("ZephyrSoC")

        super().__init__(
            cpu_type="vexriscv",
            cpu_variant="full",
            csr_data_width=8,
            *args,
            **kwargs,
        )  # TODO: Test if we actually need "full" for ecall

        self.submodules.gpio_leds = GPIOOut(self.platform.request("gpio_leds"))
        self.add_csr("gpio_leds")
