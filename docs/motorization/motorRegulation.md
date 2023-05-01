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
## Designing the regulator
The regulation loop is made using a simple proportional regulator. The position of the robot is obtained using the quadrature encoders, the error is calculated, and the motor’s voltage is regulated accordingly using the PWN of the dspic.

## Speed Curve
See the **[Speed Curve](/motorization/speedCurve)** page.

## Translation
For a translation, the speed must follow this trapezoidal curve: 
![image](https://user-images.githubusercontent.com/23436953/228483207-f1e11347-ffc9-4086-a2b3-49d63ada9217.png)
But if the distance is less than or equal to 0.5m, there is not enough time to reach cruising speed and the speed follows a triangular curve (in green on the illustration).

To get the distance, a simple integration of the speed is made.


## Rotation (Pivot)
The same principle as for translation can be used for the rotation, but the motors turn in opposite directions:
![image](https://user-images.githubusercontent.com/23436953/228483356-38b98cfd-41e4-4f16-b560-a83ecde1d68a.png)
But if the distance is less than or equal to 0.5m, there is not enough time to reach cruising speed and the speed follows a triangular curve (in green on the illustration).
The same principle can be used for the rotation, but the motors turn in opposite directions:

But, taking into account that the maximum angle that can be transmitted is 255 degrees (equivalent to 4.451 rad), and that the maximum rotation speed we chose is 4 radians / s (chosen using the same method used to find the maximum translation speed, as explained in the given documents), an that the acceleration chosen is 3.33 rad / s², we can calculate the largest rotation that can be done without reaching the cruising rotation speed : 
α ̇=t*3.33  m/S^2     →t_max⁡   4/3.33 s, where t_max⁡〖  〗 represents the time at which the cruising speed is reached.
α=t^2*3.33/2→α_tmax=t_max⁡〖  〗^2*3.33/2=4^2/〖3.33〗^2 *  3.33/2=2.4 
As the same rotation will be made when decelerating, α_max=2*2.4=4.8 rad
Therefore, there is no need to implement a trapezoidal-style curve in the code as the cruising rotation speed will never be reached.

### K Constant
...
The transfer function of the open-loop system is : \
$$BO(p) = \frac{L_e(p)}{E(p)} = \frac{k_p k_v e^{-p T_s / 2}}{(1 + p \tau)p}$$\
With $$T_s = \frac{1}{100} s, k_v = 17.5 rad / s, \tau = 0.159 s$$\
Using the gain margin criteria :\
$$\phi (BO(jw_1)) = - \pi$$\
$$-w_1 \frac{T_s}{2} - arctan(w_1 \tau) - \frac{\pi}{2} = - \pi$$\
$$-w_1  \frac{T_s}{2}  - arctan(w_1 \tau)  = -\frac{\pi}{2}$$\
$$w_1  \frac{T_s}{2}  + arctan(w_1 \tau)  = \frac{\pi}{2}$$\
$$w_1 = 35.282 rad / s$$\
Using -6 dB as target value : $$|| BO(jw_1)|| = -6 dB \simeq 0.5$$\
$$\frac{k_{pi1} k_v}{w_1 \sqrt{1 + (w_1 \tau)^2}} = 0.5$$\
$$k_{p1} = \frac{w_1 \sqrt{1 + (w_1 \tau)^2}}{2 k_v} = 5.74 rad^{-1}$$\
Using the phase margin criteria :\
$$M\phi(BO(jw_2)) = \frac{-5\pi}{6}$$\
$$-w_2 \frac{T_s}{2} - arctan(w_2 \tau) - \frac{\pi}{2} = \frac{-5\pi}{6}$$\
$$-w_2 \frac{T_s}{2} - arctan(w_2 \tau)  = \frac{-2\pi}{6}$$\
$$w_2 \frac{T_s}{2} + arctan(w_2 \tau)  = \frac{2\pi}{6}$$\
$$w_2 =  9.761$$\
As $$||Bo(jw_2)|| = 1$$ :\
$$\frac{k_{p_2} k_v}{w_2 \sqrt{1 + (w_2 \tau)^2}} = 1$$\
$$k_{p_2} = \frac{w_2 \sqrt{1 + (w_2 \tau)^2}}{k_v} = 1. 0298 rad^{-1}$$\
As we take the lowest value, our gain is $$1. 0298 rad^{-1}$$.
