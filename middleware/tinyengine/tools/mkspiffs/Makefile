# OS detection. Not used in CI builds
ifndef TARGET_OS
ifeq ($(OS),Windows_NT)
	TARGET_OS := win32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		UNAME_M := $(shell uname -m)
		ifeq ($(UNAME_M),x86_64)
			TARGET_OS := linux64
		endif
		ifeq ($(UNAME_M),i686)
			TARGET_OS := linux32
		endif
		ifeq ($(UNAME_M),armv6l)
			TARGET_OS := linux-armhf
		endif
	endif
	ifeq ($(UNAME_S),Darwin)
		TARGET_OS := osx
	endif
	ifeq ($(UNAME_S),FreeBSD)
		TARGET_OS := freebsd
	endif
endif
endif # TARGET_OS

# OS-specific settings and build flags
ifeq ($(TARGET_OS),win32)
	ARCHIVE ?= zip
	TARGET := mkspiffs.exe
	TARGET_CFLAGS = -mno-ms-bitfields
	TARGET_LDFLAGS = -Wl,-static -static-libgcc -static-libstdc++
else
	ARCHIVE ?= tar
	TARGET := mkspiffs
endif

ifeq ($(TARGET_OS),osx)
	TARGET_CFLAGS   = -mmacosx-version-min=10.7 -arch i386 -arch x86_64
	TARGET_CXXFLAGS = -mmacosx-version-min=10.7 -arch i386 -arch x86_64 -stdlib=libc++
	TARGET_LDFLAGS  = -mmacosx-version-min=10.7 -arch i386 -arch x86_64 -stdlib=libc++
endif

# Packaging into archive (for 'dist' target)
ifeq ($(ARCHIVE), zip)
	ARCHIVE_CMD := zip -r
	ARCHIVE_EXTENSION := zip
endif
ifeq ($(ARCHIVE), tar)
	ARCHIVE_CMD := tar czf
	ARCHIVE_EXTENSION := tar.gz
endif


#VERSION ?= $(shell git describe --always)
#SPIFFS_VERSION := $(shell git -C spiffs describe --tags || echo "unknown")
VERSION ?= 0.2.3-1-g011c110
SPIFFS_VERSION := 0.3.7-17-g1350deb
BUILD_CONFIG_NAME ?= -generic

OBJ		:= main.o \
		   spiffs/src/spiffs_cache.o \
		   spiffs/src/spiffs_check.o \
		   spiffs/src/spiffs_gc.o \
		   spiffs/src/spiffs_hydrogen.o \
		   spiffs/src/spiffs_nucleus.o \

INCLUDES := -Itclap -Iinclude -Ispiffs/src -I.

FILES_TO_FORMAT := $(shell find . -not -path './spiffs/*' \( -name '*.c' -o -name '*.cpp' \))

DIFF_FILES := $(addsuffix .diff,$(FILES_TO_FORMAT))

# clang doesn't seem to handle -D "ARG=\"foo bar\"" correctly, so replace spaces with \x20:
BUILD_CONFIG_STR := $(shell echo $(CPPFLAGS) | sed 's- -\\\\x20-')

override CPPFLAGS := \
	$(INCLUDES) \
	-D VERSION=\"$(VERSION)\" \
	-D SPIFFS_VERSION=\"$(SPIFFS_VERSION)\" \
	-D BUILD_CONFIG=\"$(BUILD_CONFIG_STR)\" \
	-D BUILD_CONFIG_NAME=\"$(BUILD_CONFIG_NAME)\" \
	-D __NO_INLINE__ \
	$(CPPFLAGS)

override CFLAGS := -std=gnu99 -Os -Wall $(TARGET_CFLAGS) $(CFLAGS)
override CXXFLAGS := -std=gnu++11 -Os -Wall $(TARGET_CXXFLAGS) $(CXXFLAGS)
override LDFLAGS := $(TARGET_LDFLAGS) $(LDFLAGS)

DIST_NAME := mkspiffs-$(VERSION)$(BUILD_CONFIG_NAME)-$(TARGET_OS)
DIST_DIR := $(DIST_NAME)
DIST_ARCHIVE := $(DIST_NAME).$(ARCHIVE_EXTENSION)

all: $(TARGET)

dist: $(DIST_ARCHIVE)

ifndef SKIP_TESTS
dist: test
endif

$(DIST_ARCHIVE): $(TARGET) $(DIST_DIR)
	cp $(TARGET) $(DIST_DIR)/
	$(ARCHIVE_CMD) $(DIST_ARCHIVE) $(DIST_DIR)

$(TARGET): $(OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)
	strip $(TARGET)

$(DIST_DIR):
	@mkdir -p $@

clean:
	@rm -f $(TARGET) $(OBJ) $(DIFF_FILES)

SPIFFS_TEST_FS_CONFIG := -s 0x100000 -p 512 -b 0x2000

test: $(TARGET)
	mkdir -p spiffs_t
	cp spiffs/src/*.h spiffs_t/
	cp spiffs/src/*.c spiffs_t/
	rm -rf spiffs_t/.git
	rm -f spiffs_t/.DS_Store
	ls -1 spiffs_t > out.list0
	touch spiffs_t/.DS_Store
	mkdir -p spiffs_t/.git
	touch spiffs_t/.git/foo
	./mkspiffs -c spiffs_t $(SPIFFS_TEST_FS_CONFIG) out.spiffs_t | sort | sed s/^\\/// > out.list1
	./mkspiffs -u spiffs_u $(SPIFFS_TEST_FS_CONFIG) out.spiffs_t | sort | sed s/^\\/// > out.list_u
	./mkspiffs -l $(SPIFFS_TEST_FS_CONFIG) out.spiffs_t | cut -f 2 | sort | sed s/^\\/// > out.list2
	diff --strip-trailing-cr out.list0 out.list1
	diff --strip-trailing-cr out.list0 out.list2
	rm -rf spiffs_t/.git
	rm -f spiffs_t/.DS_Store
	diff spiffs_t spiffs_u
	rm -f out.{list0,list1,list2,list_u,spiffs_t}
	rm -R spiffs_u spiffs_t

format-check: $(DIFF_FILES)
	@rm -f $(DIFF_FILES)

$(DIFF_FILES): %.diff: %
	@./format.sh < $< >$<.new
	@diff $<.new $< >$@ || ( \
		echo "File $^ not formatted correctly. Please use format.sh to re-format it." && \
		echo "Here's the diff that caused an error:" && \
		echo "" && \
		cat $@ && \
		rm $@ $<.new && \
		exit 1 )
	@rm -f $@ $<.new

.PHONY: all clean dist format-check
