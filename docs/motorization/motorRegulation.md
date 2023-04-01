---
layout: default
title: Motor Regulation
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

# Motor Regulation
...

## Designing the regulator
The transfer function of the open-loop system is : \
BO(p) = $$\frac{L_e(p)}{E(p)} = \frac{k_p k_v e^{-p T_s / 2}}{(1 + p \tau)p}$$\
With $$T_s = \frac{1}{100} s$$, $$k_v = 17.5 rad / s$$, $$\tau = 0.159 s$$\
Using the gain margin criteria :\
$$\phi (BO(jw_1)) = - \pi$$\
$$-w_1 \frac{T_s}{2} - arctan(w_1 \tau) - \frac{\pi}{2} = - \pi$$\
$$-w_1  \frac{T_s}{2}  - arctan(w_1 \tau)  = -\frac{\pi}{2}$$\
$$w_1  \frac{T_s}{2}  + arctan(w_1 \tau)  = \frac{\pi}{2}$$\
$$w_1 = 31.282 rad / s$$\
Using -6 dB as target value : $$|| BO(jw_1)|| = -6 dB \simeq 0.5$$\
$$\frac{k_{pi1} k_v}{w_1 \sqrt{1 + (w_1 \tau)^2}} = 0.5$$\
$$k_{p1} = \frac{w_1 \sqrt{1 + (w_1 \tau)^2}}{2 k_v} = 4.535 rad^{-1}$$\
Using the phase margin criteria :\
$$M\phi(BO(jw_2)) = \frac{-5\pi}{6}$$\
$$-w_2 \frac{T_s}{2} - arctan(w_2 \tau) - \frac{\pi}{2} = \frac{-5\pi}{6}$$\
$$-w_2 \frac{T_s}{2} - arctan(w_2 \tau)  = \frac{-2\pi}{6}$$\
$$w_2 \frac{T_s}{2} + arctan(w_2 \tau)  = \frac{2\pi}{6}$$\
$$w_2 =  9.761$$\
As $$||Bo(jw_2)|| = 1$$ :\
$$\frac{k_{p_2} k_v}{w_2 \sqrt{1 + (w_2 \tau)^2}} = 1$$\
$$k_{p_2} = \frac{w_2 \sqrt{1 + (w_2 \tau)^2}}{k_v} = 1. 0298 rad^{-1}$$\
As we take the lowest value, our gain is $$1. 0298 rad^{-1}$$.\


## Speed Curve
See the **[Speed Curve](/motorization/speedCurve)** page.

## Common esgfsrgrg
...

### PWM Generation
...

### Feedback
encoders...

## Translation
![image](https://user-images.githubusercontent.com/23436953/228483207-f1e11347-ffc9-4086-a2b3-49d63ada9217.png)
...

### K Constant
...


## Rotation (Pivot)
![image](https://user-images.githubusercontent.com/23436953/228483356-38b98cfd-41e4-4f16-b560-a83ecde1d68a.png)
...

### K Constant
...
