---
layout: default
title: Communication Frame
nav_order: 4
---

<details open markdown="block">
  <summary>
    Table of contents
  </summary>
  {: .text-delta }
1. TOC
{:toc}
</details>

# Communication frame

Data is transmitted via sound, from the sender (computer) to the receiver (robot). This data is contained in a custom 13bit frame.

![frame](https://user-images.githubusercontent.com/23436953/227920720-20620f49-efc8-4dc7-a9f1-b80beb2395eb.svg)

## Start bit
A correct frame always starts with a **0** as the first bit.

## Command
The next two bits describe the command sent to the robot:

- **00** - Forward
- **01** - Backward
- **10** - Turn right
- **11** - Turn left

## Parameters
The next eight bits desribe the parameters for the command sent, as an 8bit unsinged integer. If the command is _Forward_ or _Backward_, the parameters are the distance the robot must travel, in centimeters. If the command is _Turn right_ or _Turn left_, the parameters are the angle the robot must turnn in degrees. They always are an 8bit unsigned integer.

## Parity bit
The next bit describes the parity of the previous bits sent:
- **0** - even parity
- **1** - odd parity
This allows for error checking of the received frame.

## Stop bit
A correct frame always ends with a **1** as the last (13th) bit.

# Examples

Here are some examples of frames:

- 0000100101101 - Start bit correct, _Forward_ command, 75cm to travel, even parity, stop bit correct.  
  Here are the different parts of the frame: 0.00.01001011.0.1
- ...
