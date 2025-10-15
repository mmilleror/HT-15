# pico-epaper

[![builds.sr.ht status](https://builds.sr.ht/~krystianch/pico-epaper.svg)](https://builds.sr.ht/~krystianch/pico-epaper?)

Minimal epaper display library for use with the Pico SDK

## What it is

A low-level library that provides a configuration struct and a function for updating display contents from a raw buffer.

## What it isn't

A drawing library

BUT it includes example code that [draws some patterns](src/examples/draw_patterns.c) on the display.

## Documentation

See [include/pico/epaper.h](include/pico/epaper.h) for Doxygen style comments.

See [draw_patterns.c](src/examples/draw_patterns.c) for example program.

## Building the pattern drawing example

The following assumes that you have [pico-sdk](https://github.com/raspberrypi/pico-sdk) cloned to `~/.local/src`.
Substitute your own path below if your setup is different.

```bash
mkdir build
cd build
cmake -DPICO_SDK_PATH=~/.local/src/pico-sdk -DPICO_ENC28J60_EXAMPLES_ENABLED=true ..
make  # creates draw_patterns.uf2
```

## Library usage

You can add pico-epaper to your project similarly to the SDK (copying [external/pico_epaper_import.cmake](external/pico_epaper_import.cmake) into your project)
having set the `PICO_EPAPER_PATH` variable in your environment or via cmake variable.

```cmake
cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

# We also need PICO EPAPER
include(pico_epaper_import.cmake)

project(example_project)
set(CMAKE_C_STANDARD 11)
``` 
