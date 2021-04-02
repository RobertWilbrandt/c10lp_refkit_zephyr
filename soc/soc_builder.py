"""Build small SoC for zephyr to run on"""

import argparse
import os

from litex.soc.integration.builder import Builder

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

    return parser.parse_args()


def main():
    """Entry point"""
    args = parse_args()

    soc = ZephyrSoC(sys_clk_freq=SYS_CLK_FREQ)
    builder = Builder(
        soc,
        output_dir=OUTPUT_DIR,
        csr_svd=os.path.join(OUTPUT_DIR, "csr.svd"),
    )

    builder.build(run=args.build)

    if args.load:
        prog = soc.platform.create_programmer()
        prog.cable_name = "Arrow-USB-Blaster"  # Programmer used on board
        prog.load_bitstream(os.path.join(builder.gateware_dir, soc.build_name + ".sof"))


if __name__ == "__main__":
    main()
