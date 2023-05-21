---
layout: default
title: Project
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

# Project Goal
This project was completed in the context of the ELEC-H309 Projet Intégré course at the [École Polytechnique de Bruxelles (ULB)](https://polytech.ulb.be/en).  
Its purpose is to implement a sound operated robot control system. An acquisition chain had to be designed, as well as a motor regulation system.  
This project is a way for electronics engineering students to:

- use concepts taught throughout the curriculum,
- successfully carry out a large-scale multidisciplinary project,
- learn about the methodology used to approach an engineering project,
- analyze and comply with a set of specifications.

# Methodology

## Specifications Analysis
This project is defined around these specifications:

- Build a robot control system.
- Use a provided rolling base (with motors, encoders and battery) with fixed geometry.
- Develop in the C programming language.
- Use a provided [Microchip dsPIC33FJ128MC802](https://www.microchip.com/en-us/product/dsPIC33FJ128MC802) microcontroller.
- Robot must precisely move a certain amount (in a straight line or pivot).
- Receive and translate audio command signals using a microphone.
- Audio must be a sequence of 900 (representing a binary 0) and 1100 (representing a binary 1) Hz frequencies.
- Execute received commands.
- Use a [predefined data frame protocol](/communicationFrame).
- Robot must be capable of moving on solid, flat and horizontal terrrain.
- Robot must function in a calm environment (indoors, without background noise).
- Reasonable speed (maximum 1.5 to 2m/s) and acceleration (maximum 0.5m/s²).
- Robot must be easily carried.
- Regulate motors using the dsPIC's PWM signal generator.

## Breakdown into Modules
Dividing the project into multiple modules was necessary, each responsible for a part of the robot operation. The project consists of three main components:

- Data transmission over audio
- Logic processing of the received data
- Motor regulation to execute the command

This breakdown will ease the writing of each module's code, and improve its readability and access for developpers and integrators. It will also facilitate the code optimization that can be done in order to stay within the limits of our microcontroller.

The data transmission over audio can also be broken down into two submodules: Analog acquisition chain & Digital demodulation.

## Technical Solutions
In order to build each module, it was needed to decide how everything will work. Starting from already given information from the project's guidelines[^1], as well as ideas about the functionning of each part, tests and simulations were conducted to confirm the chosen path. If successful, or sufficient enough, developement can then be pursued.

A choice needed to be taken between using two microcontrollers (with a custom communication protocal between them) or only one microcontroller (limited in processing power and timings). Depending on each modules performance impact, it can be determines if the use of two dsPICs is necessary.

## Testing Strategies of Each Module
During the separate development of each module, it was needed to test their function and experiment with different parameters.

After testing, it was observed that using only one microcontroller was possible because the robot does not need to listen to new commands while moving, which would have required fast timings or the use of a second dsPIC.

## Assembly of Modules
When every module has been thoroughly tested and fine tuned, as well as confirmed working, assembling all of them into a single program can be conducted, which will be written onto the microcontroller. Each module should be able to communicate with the other ones seamlessly.

## Final Testing and Results
Final tests were conclusive. The robot worked as it should: easy, reproduceable and accurate. Some real-world test videos are available [here](/testing#real-world-tests).

# Conclusion
In the end, a fully functionning robot that can be controlled over sound from a user's device was successfully built. This robot is accurate and precise within margins, and can be easily reproduced by others (with similar hardware).

----

[^1]: See the project's guidelines on its [Gitlab repository](https://gitlab.com/mosee/elech309-2023)

Most of the details on this page come from the [ELEC-H309 project guidelines on Gitlab](https://gitlab.com/mosee/elech309-2023/-/tree/main/Description%20du%20projet).
