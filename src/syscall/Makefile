SOURCES = $(ROOT_DIR)/src/syscall/syscall.c \
          $(ROOT_DIR)/src/syscall/arg_types.c \
          $(ROOT_DIR)/src/syscall/generate.c \
          $(ROOT_DIR)/src/syscall/signals.c

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

ENTRY_SOURCES := $(wildcard freebsd/*.c)

INCLUDES = -I../ -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include -Ifreebsd
           
SILENCED_WARNINGS = -Wno-padded \
                    -Wno-used-but-marked-unused -Wno-cast-align \
                    -Wno-missing-noreturn -Wno-unused-parameter \
                    -Wno-deprecated-declarations -Wno-pedantic \
                    -Wno-unknown-pragmas -Wno-sign-conversion

INCLUDES += -I/usr/src/cddl/compat/opensolaris/include \
            -I/usr/src/cddl/contrib/opensolaris/lib/libdtrace/common \
            -I/usr/src/sys/cddl/compat/opensolaris \
            -I/usr/src/sys/cddl/contrib/opensolaris/uts/common

FLAGS = -DFREEBSD -shared -fPIC -Wall -Werror -Weverything -g -O3 -std=c99

LIB = ../io/libnxio.so ../memory/libnxmemory.so \
      ../crypto/libnxcrypto.so ../concurrent/libnxconcurrent.so \
      ../network/libnxnetwork.so ../utils/libnxutils.so \
      ../probe/libnxprobe.so ../resource/libnxresource.so \
      ../mutate/libnxmutate.so ../log/libnxlog.so 

LIB_NAME = libnxsyscall.so

SOURCES += $(ROOT_DIR)/src/syscall/syscall-freebsd.c

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

ENTRY_SOURCES := $(wildcard mac/*.c)

INCLUDES = -I../ -I../../deps/ck-0.5.0/include -Imac
           
SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-used-but-marked-unused -Wno-cast-align \
                    -Wno-missing-noreturn -Wno-unused-parameter \
                    -Wno-deprecated-declarations -Wno-undef

FLAGS = -DMAC_OSX -dynamiclib -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../crypto/libnxcrypto.dylib ../concurrent/libnxconcurrent.dylib \
      ../network/libnxnetwork.dylib ../utils/libnxutils.dylib \
      ../probe/libnxprobe.dylib ../resource/libnxresource.dylib \
      ../mutate/libnxmutate.dylib ../log/libnxlog.dylib 

LIB_NAME = libnxsyscall.dylib

SOURCES += $(ROOT_DIR)/src/syscall/syscall-mac.c

endif

all:

	$(CC) $(FLAGS) $(SILENCED_WARNINGS) $(INCLUDES) $(SOURCES) $(ENTRY_SOURCES) $(LIB) -o $(LIB_NAME)

.PHONY: clean
clean:

	rm -rf $(LIB_NAME)
	rm -rf *.dSYM

.PHONY: format
format:

	cd $(ROOT_DIR) && clang-format -i -style=file $(SOURCES)
