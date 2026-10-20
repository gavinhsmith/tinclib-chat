# tinclib-chat
An example chat app using tinclib

## Setup

Install the [CE C toolchain](https://ce-programming.github.io/toolchain/) (v15.0) and put `CEdev/bin` on your `PATH`.
[tinclib](https://github.com/gavinhsmith/tinclib) (v0.6.0) is a pinned git submodule under `lib/`, with the protocol nested inside it:

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
(usbdrvce, srldrvce, fileioc) and [TINCLIBC](https://github.com/gavinhsmith/tinclib-config) for Wi-Fi setup.
