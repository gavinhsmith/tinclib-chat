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

## Server and web client

```sh
node server/server.js   # PORT=3000 by default; Node 18+, no dependencies
node server/test.js     # API check
```

Open `http://<your-pc>:3000` on a phone for the web client. On the calculator,
enter that same address as `host:port` (not `localhost`; the board makes the
request).

API (plain text, so the calculator doesn't parse JSON):

- `GET /messages?after=ID`: one `id<TAB>name<TAB>text` line per newer message
- `POST /messages`: body `name<LF>text` (name up to 16, text up to 200 printable ASCII characters)

The server keeps the last 100 messages in memory.

## Calculator controls

Enter the server (`host:port`, e.g. `192.168.1.5:3000`), then your name, then
type messages and press [enter] to send. The server is saved in the `TCHATSRV`
appvar and filled in next time.
[clear] empties the field, and on an empty field quits.
