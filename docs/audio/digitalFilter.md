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

## Design of the filter

The specifications require a filter such that :

- Useful frequencies are within +/-1.5% of the centre frequency
- Frequencies above +/-3.5% of the centre frequency should be cut off
- Maximum attenuation of useful frequencies: H1 = 0.9
- Minimum attenuation of cut-off frequencies: H2 = 0.1  

A sampling frequency of 15 kHz was chosen for the theoretical design of the analog filter.
The filter coefficients were determined with the provided python script. This script uses the signal module of the scipy library to design the filter. It starts by determining the order of the filter required (here a 4th order). The butter function is then called, which generates a numerical filter by default.  

For the filter centered around 1100 Hz :  
// insert image  
Which corresponds to this transfer function :  
// insert image

## Base principle

It is possible to turn any analog filter to a digital one, using the Tustin approximation : $$ p = \frac{2}{T_s} \times \frac{z-1}{z+1} $$.
A recurring equation is then deducted from the transfer function, which was determined in the previous part. 
In terms of maths, we have :  
$$ Y(z) = \frac{\sum a_k \times z^{-k}}{\sum b_k \times z^{-k}} \times X(z) \\ 
<=> \sum a_k \times z^{-k}  Y(z) = \sum b_k \times z^{-k}  X(z) \\
<=> \sum a_k \times z^{-k} y(n-k) = \sum b_k \times z^{-k} x(n-k)$$
