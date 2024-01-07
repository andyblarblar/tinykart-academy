# Software Overview

Now that we have the codebase setup, let's briefly introduce what we're working with.

## Arduino

Tinykart's embedded software is based on Arduino. If you somehow haven't heard of it, Arduino is an extremely high level
(as in simple) embedded programming framework. It's designed to allow for beginners and hobbyists to engage with
embedded
programming without spending a semester reading docs. If you've worked with Arduino before, you've probably worked with
_the_ Arduino, the UNO v3:

<img src="https://store.arduino.cc/cdn/shop/products/A000066_03.front_571x429.jpg?v=1629815860"/>

What most don't know is that Arduino is actually completely independent of the UNO, mega, and other "Arduino" branded
dev-boards. In this respect, Arduino is simply a C++ library, similar to something like ROS.

So how does this work then? Well, Arduino can be ported between boards using what are called "Cores". Cores are
basically
just implementations of the Arduino API (think `writeDigital()`, `readAnalog()` etc.) using different, lower-level
libraries specific to some board or vendor. This way, you can use Arduino the same way across boards, while the actual
implementation is free to change (encapsulation, for those in SWE).

Because we are using the STM32H723GZ, we will be using [STM32duino](https://github.com/stm32duino/Arduino_Core_STM32),
an
implementation of Arduino for STM32 MCU. This Arduino core is based on the STM32Cube HAL, something the more experienced
among you may have seen before. While not required, this does mean that we can actually use the STM HAL _alongside_
Arduino,
something that is used considerably behind the scenes, although you won't need to touch it.

So what do we have available to us?

- C++17
- The Arduino API
- The STM32 HAL
- The C standard library, via newlib
- The C++ standard library (or at least large portions of it), via newlib

## PlatformIO

PlatformIO is a build system and project configuration platform that works at a level above frameworks like Arduino or
the HAL.
While I won't get too far into the details here, PIO generally allows for embedded projects to be much more flexible by
allowing
the use of different tools like IDEs, while also providing a dependency manager similar to the Arduino IDE.

One of the strongest features of PIO is its project structure, which allows for much more flexible projects than Arduino
IDE. This section will describe how to navigate this project structure.

As of the time of writing, the tinykart repo looks like this:

```
├── docs
│   ├── io_setup.md
│   ├── pinout.md
│   ├── references.md
│   └── refrence.pdf
├── include
│   ├── dma.hpp
│   ├── logger.hpp
│   ├── pins.hpp
│   └── uart.hpp
├── lib
│   ├── gap_follow
│   │   ├── common.hpp
│   │   ├── f1tenth_gap_follow.cpp
│   │   ├── f1tenth_gap_follow.hpp
│   │   ├── naive_gap_follow.cpp
│   │   └── naive_gap_follow.hpp
│   ├── ld06
│   │   ├── ld06.cpp
│   │   └── ld06.hpp
│   ├── pure_pursuit
│   │   └── pure_pursuit.hpp
│   ├── README
│   └── tinykart
│       ├── esc.hpp
│       └── kart.hpp
├── platformio.ini
├── README.md
├── src
│   ├── dma.cpp
│   ├── main.cpp
│   └── stm32h7xx_hal_msp.c
└── test
    └── README
```

- **docs**: Contains documentation about the implementation of tinykart, and other useful references.
- **include**: Contains all headers to be only used with tinykart.
- **lib**: Contains all local libraries. Allows for us to extract code that could work outside of this tinykart
  implementation, making things more extensible and clean.
    - **gap_follow**: My reference autonomous routines. You will be designing your own implementation of these.
    - **ldo6**: A driver for the LD06 LiDAR. This is provided for your convience, and you are not expected to know how
      this
      works.
    - **pure_pursuit**: Another reference implementation for you, to be redone for your own implementation.
    - **tinykart**: Utilities for accessing the RC car hardware. While you will not be implementing these, they will be
      discussed.
- **platformio.ini**: PIO's configuration file. While you will not be required to touch this, for your own
  implementation
  this
  is where you would add additional libraries or board configs.
- **src**: tinykart implementation files, using the include headers.
    - **dma.cpp**: DMA reader implementation. Don't worry about this
    - **main.cpp**: Arduino main file. This is where the magic happens.
    - **stm32h7xx_hal_msp.c**: Handles configuring the HAL for areas where we can't use Arduino, like the LiDAR. Don't
      worry
      about this, unless you are adding more sensors and also cannot use Arduino.
- **test**: Unit tests go here, although these are TBD.

## TinyKart "Extensions"

To make things easier for you, we provide a bit of a framework for TinyKart, containing implementations of the harder
parts of using Arduino to interface with the required hardware. This section will contain a brief explanation of these
components.

### TinyKart struct

The `TinyKart` struct, implemented in the tinykart local lib, is an abstraction for working with the RC car hardware. It
lives in global scope:

```c++
{{ #include ../../src/main.cpp:11:12 }}
```

You can use it to ex. move the steering to a certain angle, or set the throttle or brake:

```c++
{{ #include ../../src/main.cpp:102:104 }}
```

### LiDAR utilities

The `LD06` class is a driver for the LD06 Lidar. `ScanBuilder` is a class for working with the
raw data returned from the driver. These both live in global scope:

```c++
{{ #include ../../src/main.cpp:15:18 }}
```

These will be introduced further in the next chapter.

### Logger

> NOTE: Because of newlib restrictions, attempting to print a floating point number will hardfault

Because we need to use the UART interrupt for the LiDAR driver, we are unable to use the Serial library. Because print
debugging is quite useful, we reimplement a basic equivalent. The equivalent to `Serial` is now a global `logger`
defined
in logger.hpp.

Ex. to printf:

```c++
{{ #include ../../src/main.cpp:89:92 }}
```

Unlike the Serial library, this printf is actually non-blocking, and uses interrupts to process the message behind the
scenes. This means that there will be some lag before the message is printed, as it needs to be queued for
synchronisation
purposes.