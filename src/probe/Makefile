SOURCES = $(ROOT_DIR)/src/probe/probe.c 

INCLUDES = -I../ -I../../deps/$(CK)/include -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SILENCED_WARNINGS = -Wno-padded \
                    -Wno-unused-parameter -Wno-unused-variable \
                    -Wno-pedantic -Wno-unknown-pragmas \
                    -Wno-undef -Wno-sign-conversion \
                    -Wno-incompatible-pointer-types-discards-qualifiers

INCLUDES += -I /usr/src/cddl/compat/opensolaris/include \
	        -I /usr/src/cddl/contrib/opensolaris/lib/libdtrace/common/ \
	        -I /usr/src/sys/cddl/compat/opensolaris \
	        -I /usr/src/sys/cddl/contrib/opensolaris/uts/common/

FLAGS = -DFREEBSD -shared -fPIC -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.so ../memory/libnxmemory.so \
      -ldtrace ../utils/libnxutils.so 

SOURCES += $(ROOT_DIR)/src/probe/probe-freebsd.c

LIB_NAME = libnxprobe.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-unused-parameter -Wno-unused-variable

FLAGS = -dynamiclib -DMAC_OSX  -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      -ldtrace ../utils/libnxutils.dylib

SOURCES += $(ROOT_DIR)/src/probe/probe-mac.c

LIB_NAME = libnxprobe.dylib

endif

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM

.PHONY: format
format:

	cd $(ROOT_DIR) && clang-format -i -style=file $(SOURCES)
