cmake_minimum_required(VERSION 3.13.1)

list(APPEND BOARD_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND DTS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/dts/riscv/c10lp_refkit_soc")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(c10lp_refkit_zephyr)

target_sources(
  app
  PRIVATE
    src/main.c
)
