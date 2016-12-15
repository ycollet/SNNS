CC = gcc 
CFLAGS = -I/usr/local/X11R5/include   
LFLAGS = -L/usr/local/X11R5/lib -lfwf -lDir -lXaw -lXt -lXmu -lXext -lX11 -lm
# Verzeichnisse der SUN-Libraries 
LIBDIRS = -L/usr/local/as/lib.sun4  
# Verzeichnisse der DEC-Libraries
# LIBDIRS = -L/usr/local/as/lib.decstation

all: vistra

vistra: format.o patterns.o globals.o coll.o eh.o symtab.o vector.o veccoll.o vistra.o misc.o batch.o makefile
	$(CC) -o $@ format.o patterns.o globals.o coll.o eh.o symtab.o vector.o veccoll.o vistra.o misc.o batch.o $(LIBDIRS) $(LFLAGS)

batch.o: xvis.h batch.c
	$(CC) $(CFLAGS) -c batch.c

format.o: xvis.h format.c
	$(CC) $(CFLAGS) -c format.c

patterns.o: xvis.h patterns.c fin.c fout.c n01.c lvq.c 
	$(CC) $(CFLAGS) -c patterns.c

globals.o: xvis.h globals.c
	$(CC) $(CFLAGS) -c globals.c

coll.o: xvis.h coll.c
	$(CC) $(CFLAGS) -c coll.c

eh.o: xvis.h eh.c
	$(CC) $(CFLAGS) -c eh.c

symtab.o: xvis.h symtab.c
	$(CC) $(CFLAGS) -c symtab.c

vector.o: xvis.h vector.c
	$(CC) $(CFLAGS) -c vector.c 

veccoll.o: xvis.h veccoll.c
	$(CC) $(CFLAGS) -c veccoll.c

# Falls keine FWF-Widgets verfuegbar, ist -DXFWF_FILE_SELECTION zu entfernen !
# Es wird dann eine eigene Dialogbox zur Datei-Auswahl verwendet !
vistra.o: xvis.h def.h vistra.c crtwdgts.c uif.c int2.c init.c draw.c gw.c ssw.c
	$(CC) $(CFLAGS) -DXFWF_FILE_SELECTION -I/usr/local/as/include -c vistra.c 

misc.o: xvis.h misc.c
	$(CC) $(CFLAGS) -c misc.c

clean:
	touch *.c
