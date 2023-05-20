---
layout: default
title: Testing
nav_order: 11
math: mathjax
---

Testing for the project.
{: .fs-6 .fw-300 }

<details open markdown="block">
  <summary>
    Table of contents
  </summary>
  {: .text-delta }
1. TOC
{:toc}
</details>

# Kinematics Tests
During the development of the motor regulation part of the codebase, kinematics tests were conducted. This video shows the results of a looping sequence consisting of a simple straight line followed by a 180° turn to the left:

<video src="/assets/videos/video-test-mouvement.mp4" controls="controls">
</video>

## Movement
## Speed
Testing showed that the robot's speed was corresponding to the programmed speed (complying with the characteristics, which can be found in the [Project page](/project)).

## Target Distance
From the first test, target distance was observed to be accurate within defined margins.

## Pivot Accuracy
Testing found out that the pivot movement were usually overshot by 1 to 3 degrees. It allowed for fine-tuning the motor regulation regarding pivot, in order to acheive accurate turning.

## Motors Synchronization for Straight-Line Movements
From the first test, motors synchronization was observed to be correct.

## Acceleration - Wheel Slipping
With the given characteristics, which can be found in the [Project page](/project), and the programmed [parameters](/parameters), wheels were observed to not slip during testing.

# Audio Tests
Results of the audio part testing are available [here](/audio/audio_results).

# Implementation Tests for the Frame State Machine
Implementation tests were written and then conducted for the [Frame State Machine](/frameFSM). It allowed for finding bugs and validating its data output during its early development (This can be found [here](https://github.com/DemonicTricycle/DemonicTricycle-ELECH309/blob/main/test/implementation/testFrameFSM.c), but is outdated).

# Real-World Tests
Near the end of the project, real-world, full tests were conducted. These were able to confirm the correct functioning of all parts of the robot. From the microphone and its amplifier, to the motors regulation. It was possible to test different amplification parameters for various distances at which sound was sent.

Here are some videos showing these tests:

## Real-World Pivot Test Video
<video src="/assets/videos/test-real-world-pivot-1.mp4" controls="controls">
</video>

## Real-World Straight-Line Test Video
<video src="/assets/videos/real-world-test-straight-video-1.mp4" controls="controls">
</video>
