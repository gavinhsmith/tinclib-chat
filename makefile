# ----------------------------
# Makefile Options
# ----------------------------

NAME = TINCCHAT
ICON = icon.png
DESCRIPTION = "tinclib chat"
COMPRESSED = NO

# tinclib and titrmlib are git submodules under lib/ (CEdev on Windows can't
# build sources reached through `..`). The protocol comes from tinclib's nested submodule.
PROTO = lib/tinclib/external/tinclib-protocol
INCLUDES = -Ilib/titrmlib/src -Ilib/tinclib/src -I$(PROTO)
CFLAGS = -Wall -Wextra -Oz $(INCLUDES)
CXXFLAGS = -Wall -Wextra -Oz $(INCLUDES)
EXTRA_C_SOURCES = $(wildcard lib/titrmlib/src/*.c) $(wildcard lib/tinclib/src/*.c) $(PROTO)/crc16.c $(PROTO)/tinc_frame.c

# ----------------------------

include $(shell cedev-config --makefile)
