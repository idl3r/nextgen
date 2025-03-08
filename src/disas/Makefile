SOURCES = $(ROOT_DIR)/src/disas/disas.c

INCLUDES = -I../ -I../../deps/$(CK)/include \
           -I../../deps/$(CAPSTONE)/include \
           -isystem ../../deps/$(CAPSTONE)/include \
           -isystem ../../deps/$(CK)/include

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SILENCED_WARNINGS = -Wno-padded \
                    -Wno-unused-parameter

FLAGS = -DFREEBSD -shared -fPIC -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.so ../memory/libnxmemory.so \
      ../file/libnxfile.so ../../deps/capstone-3.0.4/libcapstone.a

LIB_NAME = libnxdisas.so

SOURCES += $(ROOT_DIR)/src/disas/disas-freebsd.c

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-unused-parameter

FLAGS = -dynamiclib -DMAC_OSX -Wall -Werror -Weverything -Wpedantic -g -O3 -std=c99

LIB = ../io/libnxio.dylib ../memory/libnxmemory.dylib \
      ../file/libnxfile.dylib

LIB_NAME = libnxdisas.dylib ../../deps/capstone-3.0.4/libcapstone.a

SOURCES += $(ROOT_DIR)/src/disas/disas-mac.c

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
