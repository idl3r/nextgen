SOURCES = $(ROOT_DIR)/src/resource/resource.c

INCLUDES = -I../ -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include
           
OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SILENCED_WARNINGS = -Wno-padded \
                    -Wno-used-but-marked-unused \
                    -Wno-incompatible-pointer-types-discards-qualifiers 

FLAGS = -DFREEBSD -shared -fPIC -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.so ../memory/libnxmemory.so \
      ../crypto/libnxcrypto.so ../concurrent/libnxconcurrent.so \
      ../network/libnxnetwork.so ../utils/libnxutils.so

LIB_NAME = libnxresource.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-used-but-marked-unused

FLAGS = -dynamiclib -DMAC_OSX  -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../crypto/libnxcrypto.dylib ../concurrent/libnxconcurrent.dylib \
      ../network/libnxnetwork.dylib ../utils/libnxutils.dylib

LIB_NAME = libnxresource.dylib

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
