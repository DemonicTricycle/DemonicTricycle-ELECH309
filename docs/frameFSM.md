---
layout: default
title: Frame State Machine
nav_order: 3
---


# Frame State Machine

The frame analysis goes through a finite state machine, at [frameFSM.c](/frameFSM.c).

## States

- **IDLE** : Waiting for a frame (default state).
- **START** : Frame start bit correct.
- **DATA** : Comprised of two parts, command and params.
- **PARITY** : Frame parity correct.
- **STOP** : Frame stop bit correct.

Each state has its handler.

## Handlers

- _**IdleHandler** : not necessary? checks if a frame has come. Goes to **START** state if successful._
- **StartHandler** : checks if start bit is correct, starts the translation of the rest of the frame. Goes to **DATA** state if successful.
- **DataHandler** : gets the command and params from the frame. Goes to **PARITY** state if successful.
- **ParityHandler** : checks if the parity of the frame against its parity bit. Goes to **STOP** state if successful.
- **StopHandler** : checks if stop bit is correct. Goes back to **IDLE** state.

If any of the handlers fails, reset back to **IDLE** state.

When the **StopHandler** has succeeded, it sets the finished var to _true_, exits the loop and returns _0_ (_no error_) from the stateFSM function, allowing the motors to get the data received as it is correct.

## Diagram

The diagram below explains it.

![frameFSM](https://user-images.githubusercontent.com/23436953/224987794-8ba17d3b-a5c1-47aa-841a-10f06f3636d8.svg)


