SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-objc-interface-ivars \
                    -Wno-direct-ivar-access \
                    -Wno-unused-parameter
SOURCES = objc-utils.m AppDelegate.m

INCLUDES = -I../

FLAGS = -DMAC_OSX -dynamiclib -Wall -Werror -Weverything -Wpedantic -g -O3

LIB = -framework Foundation -framework AppKit

LIB_NAME = libnxobjcutils.dylib

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM