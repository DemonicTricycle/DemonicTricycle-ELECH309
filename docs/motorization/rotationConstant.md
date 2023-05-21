---
layout: default
title: Rotation constant
parent: Motorization
nav_order: 1
math: mathjax
---


<details open markdown="block">
  <summary>
    Table of contents
  </summary>
  {: .text-delta }
1. TOC
{:toc}
</details>

# K Constant for the rotation
The transfer function of the open-loop system is:  

$$BO(p) = \frac{L_e(p)}{E(p)} = \frac{k_p k_v e^{-p T_s / 2}}{(1 + p \tau)p}$$

With $$T_s = \frac{1}{100} s$$, $$k_v = 17.5 rad / s$$, and $$\tau = 0.159 s$$

Using the gain margin criteria:  

$$\phi (BO(jw_1)) = - \pi$$

$$-w_1 \frac{T_s}{2} - arctan(w_1 \tau) - \frac{\pi}{2} = - \pi$$

$$-w_1  \frac{T_s}{2}  - arctan(w_1 \tau)  = -\frac{\pi}{2}$$

$$w_1  \frac{T_s}{2}  + arctan(w_1 \tau)  = \frac{\pi}{2}$$

$$w_1 = 35.282 rad / s$$

Using -6 dB as target value : $$\| BO(jw_1) \| = -6 dB \simeq 0.5$$

$$\frac{k_{pi1} k_v}{w_1 \sqrt{1 + (w_1 \tau)^2}} = 0.5$$

$$k_{p1} = \frac{w_1 \sqrt{1 + (w_1 \tau)^2}}{2 k_v} = 5.74 rad^{-1}$$

Using the phase margin criteria:  

$$M\phi(BO(jw_2)) = \frac{-5\pi}{6}$$

$$-w_2 \frac{T_s}{2} - arctan(w_2 \tau) - \frac{\pi}{2} = \frac{-5\pi}{6}$$

$$-w_2 \frac{T_s}{2} - arctan(w_2 \tau)  = \frac{-2\pi}{6}$$

$$w_2 \frac{T_s}{2} + arctan(w_2 \tau)  = \frac{2\pi}{6}$$

$$w_2 =  9.761$$

As $$\| Bo(jw_2) \| = 1$$ :

$$\frac{k_{p_2} k_v}{w_2 \sqrt{1 + (w_2 \tau)^2}} = 1$$

$$k_{p_2} = \frac{w_2 \sqrt{1 + (w_2 \tau)^2}}{k_v} = 1. 0298 rad^{-1}$$

The gain is equal to the lowest value, giving a gain for the rotation of $$1. 0298 rad^{-1}$$.  


Test results showed that the robot performed better when using a higher rotation gain, as the friction forces stopped the robot slightly before the end of the movement. A gain equal to 3 times the calculated gain gave the best performance. 
The most likely reason is that the motors' properties changed over time (the data used for the calculation comes from tests made a few years ago).
\