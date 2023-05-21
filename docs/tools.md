---
layout: default
title: Tools
nav_order: 10
---

# IDE - MPLAB X
The codebase was written on Microchip's [MPLAB X](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide), a dedicated PIC MCU and dsPIC developing environment that comes with microcontroller programmming built-in.

## XC16
The compiler used for the project is Microchip's [XC16](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers), a purpose-built PIC MCU and dsPIC 16-bit compiler that is well integrated within MPLAB X.

## PICkit4
The programmer used to write the compiled program to the microcontroller is Microchip's [PICkit4](https://www.microchip.com/en-us/development-tool/PG164140), an easily operated programmer that is well integrated within MPLAB X.

# Versioning - Git(hub)
[Git](https://git-scm.com/), in conjunction with [Github](https://github.com), was used to version control the repository. This also allowed easy collaboration and synchronisation of the latest codebase.

## Documentation Building
This project's documentation is built using [Jekyll](https://jekyllrb.com/), from [Markdown](https://daringfireball.net/projects/markdown/) `.md` files, on [Cloudflare Pages](https://pages.cloudflare.com/), using the [Just The Docs](https://github.com/just-the-docs/just-the-docs) theme. This allows for easy versioning using git, and automated website builds.

# Code Tools - `tools.c`
Common code that was used globally got combined into a single [tools.c](https://github.com/DemonicTricycle/DemonicTricycle-ELECH309/blob/main/src/tools.c) file. It contains:

- UART functions (initialization, send message, send int, break line, etc)
- Integer conversions (unsigned to signed, unsigned to char array, etc)
- Absolute value of an int or float

