---
layout: default
title: Digital Filter
parent: Audio
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

# Digital filter
...

The digital filter uses the dsPIC's built-in ADC as the input, samples the signal and then processes it to get each bit of the received message.

## Input
...  

The digital filter gets its input from the analog filter after the microphone, into the ADC.

Here is the input signal from the microphone over the whole message period.
![image](https://user-images.githubusercontent.com/23436953/233019096-c1f4d04f-3b2f-4e69-ac11-e4eb34973196.png)
- Blue: 
- Red: 

## Sampling
...
In order to get the amplitude of each signal (900Hz and 1100Hz), we need to use an ADC to get samples from these signals. This sampling will then be able to be processed to get the translated bit over a 100ms period. The sampling rate is _**TODO**_ Hz.

The dsPIC's ADC samples the received signal:
![sampling](https://user-images.githubusercontent.com/23436953/233017968-64243f65-caa4-478c-afec-c728ba670f11.png)
- Red: Sampling
- Blue: 

## Processing
...

Compares the average sample, being more at 900Hz or 1100Hz, .....

## Output
...

The digital filter sends each processed bit to the [Frame State Machine](/frameFSM) in order to compute the parameters of the binary word and check its potential errors.
