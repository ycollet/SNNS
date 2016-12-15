CC = gcc
CFLAGS = -I/tools/openwin/share/include -ggdb
LIBS   = -L/usr/lib -L/tools/openwin/lib -lxview -lolgx -lX11

all: xgraf
demo: xgraf_demo

xgraf: xgraf.o 
	$(CC) $(CFLAGS) xgraf.o $(LIBS) -o ../../../xgraf

xgraf_demo:  xgraf_demo.o graf.o graf.h
	$(CC) $(CFLAGS) xgraf_demo.o graf.o -lm -o xgraf_demo 

graf.o: graf.c graf.h

clean:
	rm -f *.o *~ core

