
SOURCES = $(ROOT_DIR)/src/concurrent/concurrent.c

INCLUDES = -I../ -I../../deps/$(CK)/include \
           -isystem ../../deps/$(CK)/include

OPERATING_SYSTEM = $(shell uname)

# FreeBSD build options.
ifeq ($(OPERATING_SYSTEM), FreeBSD)

SOURCES += $(ROOT_DIR)/src/concurrent/concurrent-freebsd.c

SILENCED_WARNINGS = -Wno-padded \
                    -Wno-format-nonliteral -Wno-gnu-statement-expression \
                    -Wno-unused-parameter -Wno-undef -Wno-unused-parameter

FLAGS = -DFREEBSD -shared -fPIC -Wall -Werror -Weverything -g -O3 -std=c99

LIB = ../../deps/$(CK)/src/libck.so ../io/libnxio.so

LIB_NAME = libnxconcurrent.so

endif

# Mac OSX build options.
ifeq ($(OPERATING_SYSTEM), Darwin)

SOURCES += $(ROOT_DIR)/src/concurrent/concurrent-mac.c

SILENCED_WARNINGS = -Wno-reserved-id-macro -Wno-padded \
                    -Wno-format-nonliteral -Wno-gnu-statement-expression \
                    -Wno-unused-parameter

FLAGS = -dynamiclib -DMAC_OSX -Wall -Werror -Weverything -g -O3 -std=c99

LIB = ../../deps/$(CK)/src/libck.so ../io/libnxio.dylib

LIB_NAME = libnxconcurrent.dylib

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
