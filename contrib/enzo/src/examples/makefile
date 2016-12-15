#
#  Library-makefile fur submodule `examples' of enzo
#



# -- Archive Configuration --

AR              = ar    # make sure this is gnu-ar (/tools/bin/ar)
ARFLAGS         = rc 



NEPOMUKDIR      = ../nepomuk
SNNSDIR      = ../../../SNNSv4.1/kernel/sources
ENZO_DIR  = ..




CC     = gcc
CFLAGS =  -I$(SNNSDIR) -I$(NEPOMUKDIR) -I$(ENZO_DIR) 



LIB     = libexamples.a
OBJECTS = my_module.o



# ------------------------------------------------------------------------------

all:  opt

debug:  $(LIB)
debug:= CFLAGS += -ggdb -Wall

opt:  $(LIB)
opt:= CFLAGS += -O4


$(LIB): $(LIB)($(OBJECTS))
	$(AR) $(ARFLAGS) $@ $?


$(LIB)(%.o):%.o
	@true


#eof