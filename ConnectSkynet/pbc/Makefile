CC = gcc
CFLAGS = -O2 -fPIC -Wall
AR = ar rc

BUILD = build

.PHONY : all lib clean

LIBSRCS = context.c varint.c array.c pattern.c register.c proto.c map.c alloc.c rmessage.c wmessage.c bootstrap.c stringpool.c decode.c
LIBNAME = libpbc.a


BUILD_O = $(BUILD)/o

all : lib

lib : $(LIBNAME)

clean :
	rm -rf $(BUILD)

$(BUILD) : $(BUILD_O)

$(BUILD_O) :
	mkdir -p $@

LIB_O :=

define BUILD_temp
  TAR :=  $(BUILD_O)/$(notdir $(basename $(1)))
  LIB_O := $(LIB_O) $$(TAR).o
  $$(TAR).o : | $(BUILD_O)
  -include $$(TAR).d
  $$(TAR).o : $(1)
	$(CC) $(CFLAGS) -c -I. -o $$@ -MMD $$<
endef

$(foreach s,$(LIBSRCS),$(eval $(call BUILD_temp,$(s))))

$(LIBNAME) : $(LIB_O)
	cd $(BUILD) && $(AR) $(LIBNAME) $(addprefix ../,$^)

.PHONY : all lib clean






