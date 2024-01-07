# TinyKart

<video src="http://www.iscumd.com/images/tk-run.webm" autoplay loop> </video>

This is the mono-repo for the TinyKart project, an educational autonomous RC car. It contains both a working reference
implementation of TinyKart, and a book for teaching it.

For more information on TinyKart, including setup, please view
the [book](https://andyblarblar.github.io/tinykart-academy/).

### Brief Kart Usage

To start:

1. Turn on ESC
2. Power or reset board
3. Press the blue user button to start the kart

At any time in motion, the kart can be stopped by pressing the blue button again.
The Kart will also stop automatically if it sees something directly in front of it. This will not stop it from hitting
things, but does stop it from continuing to run away.

LED codes:

- Green = Peripherals are good
- Red = Peripherals are not good
- Yellow = Kart is paused

Information on the planner will be logged to the serial port.

### Building the book

The book is written using mdbook. To build it, simply install mdbook using Rust's cargo, and then
```mdbook build```
Inside the book directory.

A GitHub action is included in this repo that deploys this to GitHub pages automatically on each push.

### For future maintainers

I have kept some personal notes in the docs directory, which build on the book in a more direct way.
The book also contains significant context that should be kept in mind when evolving this repo.