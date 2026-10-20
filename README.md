# tinclib-chat
An example chat app using tinclib

## Setup

Install the [CE C toolchain](https://ce-programming.github.io/toolchain/) (v15.0) and put `CEdev/bin` on your `PATH`.
[tinclib](https://github.com/gavinhsmith/tinclib) (v0.6.0) and [titrmlib](https://github.com/gavinhsmith/titrmlib) (v0.4.2) are pinned git submodules under `lib/`. The protocol is nested inside tinclib:

```sh
git clone --recursive https://github.com/gavinhsmith/tinclib-chat.git
# or, in an existing clone:
git submodule update --init --recursive
```

## Build

```sh
make
```

Produces `bin/TINCCHAT.8xp`. The calculator needs the
[CE C libraries](https://github.com/CE-Programming/libraries/releases)
(usbdrvce, srldrvce, fileioc, graphx, keypadc) and [TINCLIBC](https://github.com/gavinhsmith/tinclib-config) for Wi-Fi setup.
