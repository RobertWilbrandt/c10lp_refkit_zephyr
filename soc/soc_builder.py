"""Build small SoC for zephyr to run on"""

import argparse
import os

from litex.soc.integration.builder import Builder
from litex.soc.integration.soc_sdram import soc_sdram_argdict, soc_sdram_args

from .zephyr_soc import ZephyrSoC

OUTPUT_DIR = "build_soc"

SYS_CLK_FREQ = 50 * 10 ** 6


def parse_args():
    """Parse CLI arguments"""
    parser = argparse.ArgumentParser(
        description="Demo SoC and application for the Cyclone 10 LP RefKit"
    )
    parser.add_argument("--build", action="store_true", help="Build bitstream")
    parser.add_argument("--load", action="store_true", help="Load bitstream")

    soc_sdram_args(parser)

    return parser.parse_args()


def print_soc_map(val, use_hex=False):
    entries = [i for i in val.items()]
    entries.sort(key=lambda x: x[1])
    for (name, i) in entries:
        if use_hex:
            print(f"  {name:>15}   0x{i:08x}")
        else:
            print(f"  {name:>15}   {i}")


def main():
    """Entry point"""
    args = parse_args()

    soc = ZephyrSoC(sys_clk_freq=SYS_CLK_FREQ, **soc_sdram_argdict(args))
    builder = Builder(soc, output_dir=OUTPUT_DIR)

    builder.build(run=args.build)

    print()
    print("Memory map:")
    print_soc_map(soc.mem_map, use_hex=True)

    print()
    print("CSR map:")
    print_soc_map(soc.csr_map)

    print()
    print("Interrupt map:")
    print_soc_map(soc.irq.locs)

    if args.load:
        prog = soc.platform.create_programmer()
        prog.cable_name = "Arrow-USB-Blaster"  # Programmer used on board
        prog.load_bitstream(os.path.join(builder.gateware_dir, soc.build_name + ".sof"))


if __name__ == "__main__":
    main()
