---
layout: default
title: Data Transmission
nav_order: 3
---

<details open markdown="block">
  <summary>
    Table of contents
  </summary>
  {: .text-delta }
1. TOC
{:toc}
</details>

# Data transmission

Data is transmitted via sound, from the sender (computer) to the receiver (robot).  
See [Communication Frame](/communicationFrame) for details about the data frame.

## Sender
Any sound emitting device can be used to send the data.  
Either a smartphone, or a laptop was used to play premade .wav files[^1] containing the command that the robot must execute. These commands can be "Go forward 50cm", "Turn left 180°", etc.

## Receiver
The robot receives these audio commands using its onboard microphone. This sound is then amplified, filtered and translated by the robot.  
See [Audio](/audio) for details about the audio processing.

## Acoustic transmission
Data is transmitted via frequency modulated sound. A 13bit word is sent, each bit is represented by a 100ms long sound note:
- **900** Hz - Represents a **0** bit
- **1100** Hz - Represents a **1** bit

See [Communication Frame](/communicationFrame) for details about the data frame.

### Example
Here is an audio file of an example frame (_Turn Left_ 90°):

<audio controls src="/assets/audio/left90.mp3">
</audio>

## Diagram

This diagram explains it.

![dataTransmission](https://user-images.githubusercontent.com/23436953/225017515-a9b6e247-f674-4453-ae4a-4dbe4b0e50b5.svg)

----

[^1]: These .wav files can be found at [the project's Github repository in `comms_sender/Audio commands examples/`](https://github.com/DemonicTricycle/DemonicTricycle-ELECH309/tree/main/comms_sender/Audio%20commands%20examples)
