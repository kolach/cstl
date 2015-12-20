CFLAGS=-g -O2 -Wall -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS)

LIBS=-ldl $(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
GENERATOR_SOURCES=$(wildcard src/**/*gen.c src/*gen.c)
SOURCES:=$(filter-out $(GENERATOR_SOURCES),$(SOURCES))
VECTOR_SOURCES=$(wildcard src/**/*vector.c src/*vector.c)

OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
VECTOR_OBJECTS=$(patsubst %.c,%.o,$(VECTOR_SOURCES))

define VECTOR_O
$(1): vectorgen.c $(patsubst %.o,%c,$(1)) vectorgen.h
endef

$(foreach f,$(VECTOR_OBJECTS),$(eval $(call $(VECTOR_O),$(f))))

#$(call $(VECTOR_O), bstring_vector.o)
#$(call $(VECTOR_O), vector.o)
#$(call $(VECTOR_O), int_vector.o)
#$(call $(VECTOR_O), float_vector.o)


#bstring_vector.o: vectorgen.c bstring_vector.c vectorgen.h
#vector.o: vectorgen.c vector.c vectorgen.h
#float_vector.o: vectorgen.c float_vector.c vectorgen.h
#int_vector.o: vectorgen.c int_vector.c vectorgen.h

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

TARGET=build/libcstl.a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

# The Target Build
all: $(TARGET) $(SO_TARGET) tests

dev: CFLAGS=-g -Wall -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@
$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

build:
	@mkdir -p build
	@mkdir -p bin

# The Unit Tests
.PHONY: tests
#tests: CFLAGS += $(TARGET)
tests: LDLIBS += $(TARGET)
tests: $(TESTS)
	sh ./tests/runtests.sh

valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

# The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/”

# The Checker
check:
	@echo Files with potentially dangerous functions in $(SOURCES)
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true


