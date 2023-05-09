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

## Regulation loop
The regulation loop is made using a simple proportional regulator. 
The position of the robot is obtained using the quadrature encoders, the error is calculated, and the motor’s voltage is regulated accordingly using the PWN of the dspic.
![image](https://user-images.githubusercontent.com/23436953/236776366-69fdc0b7-26fe-4eb6-9e63-9bd44f4d96c5.png)


## Target Generation

For a translation, the speed must follow this trapezoidal curve:  

![image](https://user-images.githubusercontent.com/23436953/229224784-ba9a8556-7f18-44ae-a701-5243aa1d0371.png)

If the distance is 0.5m or less, it won't be possible to reach the desired cruise translation speed in the available time. 
In such cases, the speed will follow a triangular curve (shown in green on the illustration), and both of these scenarios need to be accounted for in the code.
To follow this curve, we wrote a function that calculates the target position based on the desired final translation and the time elapsed since the start of the movement.
The output of this script gives the following plot (for a final translation of 2m):  

![graph1](https://user-images.githubusercontent.com/23436953/236680182-96053696-b795-4aa4-b4aa-0a5f529da144.png)

The target speed can be calculated from the target position, giving:  

![graph2](https://user-images.githubusercontent.com/23436953/236680264-371f3775-b0c9-49de-9790-0c46fd52e5d2.png)

The target speed matches the specifications, and the code can be validated.  

The same principle can be used for the rotation, but the motors turn in opposite directions:  

![image](https://user-images.githubusercontent.com/23436953/229225553-d875a6fc-f2d3-48a0-a3a9-b6f54759b9f9.png)

In order to calculate the largest rotation that can be done without reaching the cruise rotation speed, we need to consider several factors. Firstly, the maximum angle that can be transmitted is 255 degrees, which is equivalent to 4.451 radians. Secondly, we have chosen a maximum rotation speed of 4 radians per second. This selection was made using the same method that was used to determine the maximum translation speed, as explained in the accompanying documents.

Additionally, we have chosen an acceleration of 3.33 radians per second squared. With these factors in mind, we can calculate the largest rotation that can be accomplished without reaching the cruise rotation speed :

$$\dot{\alpha} = 3.33 t \frac{rad}{ s^2}$$ $$\rightarrow t_{max} = \frac{\dot{\alpha}_{max }}{3.33} \frac{s^2}{rad} = \frac{4}{3.33}s $$
where $$\alpha$$ represents the angle of the robot, $$t_{max}$$ represents the time at which the cruising speed is reached.
Writing $$\alpha_{max}$$ the angle reached at this time,

$$\alpha = \frac{3.33}{2} t^2 rad / s^{2} \rightarrow \alpha_{tmax} = \frac{3.33}{2}  t_{max}^2 rad / s^{2}= \frac{3.33}{2} (\frac{4}{3.33})^2 rad = 2.4 rad$$

As the same rotation will be made when decelerating, $$\alpha_{max} = 2*2.4 rad = 4.8 rad$$  
Therefore, there is no need to implement a trapezoidal-style curve in the code as the cruising rotation speed will never be reached.  
For a rotation of 90°, the code gives the following target angle:

![graph3](https://user-images.githubusercontent.com/23436953/236680434-37b6e3f3-e92a-4576-bf1a-4b89dfd749b3.png)

The target speed can be calculated from the target angle, giving:  

![graph4](https://user-images.githubusercontent.com/23436953/236680441-1c68ecb6-01c3-4837-828b-047a7f71b89b.png)

Which follows the specification, thus validating the code.

### K Constant for the rotation
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

As we take the lowest value, our gain is $$1. 0298 rad^{-1}$$.  
