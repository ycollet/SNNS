#
#  Library-makefile fur submodule `history' of enzo
#



# -- Archive Configuration --

AR              = ar    # make sure this is gnu-ar (/tools/bin/ar)
ARFLAGS         = rc 



NEPOMUKDIR      = ../nepomuk
SNNSDIR      = ../../../SNNSv4.1/kernel/sources
ENZO_DIR  = ..



CC      = gcc
CFLAGS  =  -I$(ENZO_DIR) -I$(SNNSDIR) -I$(NEPOMUKDIR)
 
LIB     = libhistory.a
OBJECTS = histUtils.o histSimple.o histWeights.o histFitness.o histCross.o\
          ancestry.o histInputs.o\
          saveThem.o  Xhist.o Xgraf/graf.o



# ------------------------------------------------------------------------------

all:  opt

debug:  $(LIB)
debug:= CFLAGS += -ggdb -Wall

opt:  $(LIB)
opt:= CFLAGS += -O4

.SUFFIXES: .c 
.c.o	:
	$(CC) $(CFLAGS) -o $@ -c $*.c 


$(LIB): $(LIB)($(OBJECTS))
	$(AR) $(ARFLAGS) $@ $?


$(LIB)(%.o):%.o
	@true


#eof