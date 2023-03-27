---
layout: default
title: Data Transmission
nav_order: 2
---

<details open markdown="block">
  <summary>
    Table of contents
  </summary>
  {: .text-delta }
1. TOC
{:toc}
</details>

# Data transmission

Data is transmitted via sound, from the sender (computer) to the receiver (robot).  
See [Communication Frame](/communicationFrame) for details about the data frame.

## Sender
TODO

## Receiver
TODO

## Acoustic transmission
Data is transmitted via frequency modulated sound. A 13bit word is sent, each bit is a 200ms sound note:
- **900** Hz - Represents a **0** bit
- **1100** Hz - Represents a **1** bit

See [Communication Frame](/communicationFrame) for details about the data frame.

## Diagram

This diagram explains it.

![dataTransmission](https://user-images.githubusercontent.com/23436953/225017515-a9b6e247-f674-4453-ae4a-4dbe4b0e50b5.svg)
