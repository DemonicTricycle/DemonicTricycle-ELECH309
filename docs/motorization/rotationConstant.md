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


Test results showed that the robot performed better when using a higher rotation gain. A gain equal to 3 times the calculated gain gave the best performance. 
It could simply be that the motors' properties changed over time (the data used for the calculation comes from tests made a few years ago), but there is another possible explaination.  

The rotation and translation regulators are used together, to keep the right direction in a translation or to avoid a translation in a rotation. 
But for the translation, the kp used in the calculations comes from a graph made with dc = 1. For the rotation, the kp was found from a graph made at dc = 0.4.  

This means that the translation gain is not valid when in a rotation, as the average dc is closer to 0.5, and that the rotation gain is not valid for the translation as the average dc is superior.
In conclusion, to get better results, when in a rotation, the robot should use a different transalation gain to reach better performances, but as the restults were aldready satisfying, it was not necessary.

