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

# Numerical simulation

##  Before optimizing

The filter around 1100 Hz was first simulated on MATLAB in order to take advantage of the graphical tools and the workspace system to debug the code.
The first stage was first implemented separately (in blue, the theoretical filter and in orange, the numerically simulated stage) :  
// insert image  
The method of simulation is to simply generate Q sines at the input, with frequencies equidistant between $$ ]0, fs/2\[ $$
The output of the system is then observed in regime to determine the gain.
It was then necessary to determine whether the 4 stages of the filter were in parallel (sum of the responses of each filter), or in series (the output of one stage becomes the input of another). By simulation, it was determined that the filters were in series.  
Here is a simulation of the 4 stages of the filter centered around 1100 Hz for an input frequency of a sin of 1100 Hz :  
// insert image
And for an input frequency of 1150 Hz :  
// insert image  
This was done to check if the filter attenuates correctly the frequencies.

## Optimizing memory

As there are 4 stages + the output, and the stages are in series, a 5x12 table was used to optimize memory :  
// insert image  

## Optimizing compute ressources

As floats aren't native to the microprocessor architecture, fixed point compute was used. The data type that will be used are therefore integers, stored on 32 bits. To determine how many bits to use for the integer and the fractional part, numerical simulation in C of the filters was used. The notation used is QX:Y, X being the number of bits used for the integer part, and Y for the fractional part ($$ X + Y = 32 $$).  
Between Q12:20 and Q17:15, for all values :  
// insert image  
For the most important values :  
// insert image  
For Q18:14, the maximal values of int32_t's was reached, which causes overflow :  
// insert image  
Q17:15 was chosen, but any repartition that doesn't ovoerflow would have been fine, as the performance doesn't seem too much affected.

# Implementation on the microcontroller
// je dois peut être trouver une meilleure terminologie que sampling timer psq c'est pas précis
The chip is overclocked to 39.5 MHz, to allow sufficiently fast sampling.
The code starts by resetting the values in the array to 0 :  
// insert code

An infinite loop waits for the conversion of the adc (15 kHz), then calculates the output of the stages. The pointers to the array are updated :  
// insert code

In parallel, at 1000 Hz, an interrupt timer is used to analyse the array values. While this code is running, sampling does not take place. The maximum value of the two arrays (900 and 1100Hz) is recovered from the previous 18 samples (which gives at least one period of the signal). If one of the two values exceeds an experimentally determined threshold (around 300), one of the filters has detected a signal.

When a signal is detected, the chip goes into listening mode. Each time the timer is interrupted at 1000 Hz, the average_sample variable will be incremented if the 1100 Hz filter has detected a signal, and decremented if it is the 900 Hz filter. Once 99 samples have been received (normally 100 but the frequency is not exactly 1000 Hz), a bit has been received. This bit is deducted from the value of average_sample, and is sent to the FSM. If this bit is the last one of the frame, everything is reset.

Small details: due to a delay in the detection of the signal at the first bit, caused by the digital filter transient, the first bit is detected after 74 samples and not 99. Also, as can be seen in the figure (insert figure with filter stages), again due to the filter transient, some short sounds (composed of many frequencies, such as a clap) are not attenuated fast enough. During sampling, the filters then detect a signal. In order to overcome this problem, a "fluke" detection system has been implemented. A noise_counter variable counts the number of times that none of the filters detected a signal during the first bit. If this variable reaches 15/75, it is probably a fluke and the system resets.

Full code of the sampling timer : 
// insert code
