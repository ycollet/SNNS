# makefile for enzo-sources


NEPOMUKDIR      = nepomuk
NEPOMUKLIB      = ${NEPOMUKDIR}/nepomuk.a
SNNSDIR         = ../../SNNSv4.1/kernel/sources
SNNSLIBS        = $(SNNSDIR)/libkernel.a $(SNNSDIR)/libfunc.a

HEADER_DIR =  -I$(SNNSDIR) -I$(NEPOMUKDIR)


CC      = gcc
CFLAGS  = $(HEADER_DIR) -I..

LDFLAGS = -lm -ll 




OBJECTS   = enzo.o m_table.o  networkData.o subUtils.o 
UTILS_OBJ = subUtils.o

SUBTARGET = 
LIBS    =     pre/libpre.a   \
              stop/libstop.a \
              selection/libselection.a \
              mutation/libmutation.a   \
              crossover/libcrossover.a \
              opt/libopt.a \
              eval/libeval.a \
              history/libhistory.a \
              survival/libsurvival.a \
              post/libpost.a






all: opt

debug opt: enzo 


.KEEP_STATE:

opt:=   SUBTARGET  += opt
opt:=   CFLAGS += -O4

debug:= SUBTARGET  += debug
debug:= CFLAGS += -ggdb -Wall


enzo:  $(OBJECTS) $(LIBS) $(NEPOMUKLIB)
	$(CC)  $(OBJECTS) $(LIBS)  $(NEPOMUKLIB) $(SNNSLIBS)    $(LDFLAGS) -o ../enzo


m_table.o: 
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(LIBS): FORCE
	cd $(@D); $(MAKE) $(SUBTARGET)

$(NEPOMUKLIB): FORCE
	cd $(NEPOMUKDIR); $(MAKE) $(SUBTARGET)



# ------------------------------------------------------------------------------

group: FORCE
	chmod -f g+w */*.o */*.a enzo

clean: FORCE
	rm -f *~ */*~ */*.o */*.a *.o   *.tmp */*.tmp


FORCE:

#eof
