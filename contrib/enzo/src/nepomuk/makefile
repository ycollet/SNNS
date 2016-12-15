



ORGSOURCE   = ../../../SNNSv4.1/kernel/sources





# C-Compiler Configuration 

CC     = gcc
CFLAGS = -D__ENZO__ -I$(ORGSOURCE) 
OUTPUT_OPTION = -o $(@:%.c=%.o)






# nepomuk objekt-dateien

NEPOMUK_OBJECTS = nepomuk.o kr_shell.o
NEPOMUK         = nepomuk.a


# -- Archive Configuration --

AR              = ar    # make sure this is gnu-ar (/tools/bin/ar)
ARFLAGS 	= src 

# Rules

.KEEP_STATE:

all: opt


debug:  $(NEPOMUK)
debug:= CFLAGS += -ggdb -Wall

opt:  $(NEPOMUK)
opt:= CFLAGS += -O4



$(NEPOMUK): $(NEPOMUK)($(NEPOMUK_OBJECTS))
	$(AR) $(ARFLAGS)  $@ $?

$(NEPOMUK)(%.o): %.o
	@true

clean: FORCE
	rm -f *~  *.a *.o 

FORCE:


# eof






