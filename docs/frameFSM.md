---
layout: default
title: Frame State Machine
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

# Frame State Machine

The frame analysis goes through a finite state machine, at [frameFSM.c](https://github.com/DemonicTricycle/DemonicTricycle-ELECH309/blob/main/src/frameFSM.c).

## States

- **IDLE** : Waiting for a frame (default state).
- **START** : Frame start bit correct.
- **DATA** : Comprised of two parts, command and params.
- **PARITY** : Frame parity correct.
- **STOP** : Frame stop bit correct.

Each state has its handler.

## Handlers

- **IdleHandler** : gets called when a frame has come, then goes to **START** state.
- **StartHandler** : checks if start bit is correct, starts the translation of the rest of the frame. Goes to **DATA** state if successful.
- **DataHandler** : gets the command and parameterss from the frame by getting each data bit and stores it, then goes to **PARITY** state.
- **ParityHandler** : checks if the parity of the previous processed bits against its parity bit. Goes to **STOP** state if successful.
- **StopHandler** : checks if stop bit is correct. Goes back to **IDLE** state.

If any of the handlers fails, reset back to **IDLE** state.

When the **StopHandler** has succeeded, it sends the movement order to the Motors, as the data is correct.

## Diagram

The diagram below explains it.

![frameFSM](https://github.com/DemonicTricycle/DemonicTricycle-ELECH309/assets/23436953/d4161194-2fe5-4c22-b29b-025439e4c1c2)

