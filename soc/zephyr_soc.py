"""Small SoC definition for zephyr to run on"""
import logging
import os

from litescope import LiteScopeAnalyzer
from litex.build.generic_platform import IOStandard, Pins, Subsignal
from litex.soc.cores.gpio import GPIOIn, GPIOOut
from litex_boards.targets.c10lprefkit import BaseSoC

pmod1_uart_ios = [
    (
        "pmod1_uart",
        0,
        Subsignal("rx", Pins("P4")),
        Subsignal("tx", Pins("P6")),
        IOStandard("3.3-V LVTTL"),
    )
]


class ZephyrSoC(BaseSoC):
    def __init__(self, sys_clk_freq, output_dir, *args, **kwargs):
        self._logger = logging.getLogger("ZephyrSoC")

        super().__init__(
            cpu_type="vexriscv",
            cpu_variant="full",
            csr_data_width=8,
            integrated_rom_size=0x8000,
            *args,
            **kwargs,
        )  # TODO: Test if we actually need "full" for ecall

        self.submodules.gpio_leds = GPIOOut(self.platform.request("gpio_leds"))
        self.add_csr("gpio_leds")

        self.submodules.switches = GPIOIn(self.platform.request_all("sw"))
        self.add_csr("switches")

        self.platform.add_extension(pmod1_uart_ios)
        self.add_uartbone(name="pmod1_uart")

        analyzer_signals = [self.cpu.ibus, self.cpu.dbus]
        self.submodules.analyzer = LiteScopeAnalyzer(
            analyzer_signals,
            depth=512,
            clock_domain="sys",
            csr_csv=os.path.join(output_dir, "analyzer.csv"),
        )
