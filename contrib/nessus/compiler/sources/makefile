#*****************************************************************************
#  FILE     : Makefile
#  SHORTNAME: Makefile

#  PURPOSE  : Makefile for Nessus compiler
#  NOTES    : Don't change this file manually!!!!
#           : it is generated automatically from configure script

#  AUTHOR   : Tilman Sommer, Niels Mache, Thomas Korb
#  DATE     : 20.3.1991

#             (c) 1991 by SNNS-Group
#             Institut fuer parallele und verteilte Hoechstleistungsrechner
#             Universitaet Stuttgart
#*****************************************************************************






# Created by hummler at Thu Apr 22 14:04:41 MET DST 1993

# -- NESSUS  Installation  Directories  &  Libraries --
NESSUSDIR = /tmp_mnt/home/hummler/hiwi/NESSUSv2.1
MACHINE = sun4
CCFLAGS = -O4
FPOPTIONS = -fsingle
MACHINENAME= sun4
COMPILERBINDIR= /tmp_mnt/home/hummler/hiwi/NESSUSv2.1/compiler/bin/sun4
PROGRAM   = nessus


# -- C-Compiler Configuration --

COMPILERDEBUG=
#COMPILERDEBUG= -g  -DTEST -DDEBUG -DLOOPTEST -DSCANTEST
CFLAGS= $(COMPILERDEBUG) $(CCFLAGS) $(FPOPTIONS)
LDOPTIONS= -lm -ll
CC= cc

# Objetcs

COMPOBJECTS= co_main.o \
	co_parser.o \
	co_scanner.o \
	co_symTab.o \
	co_error.o \
	co_output.o \
	co_cFuncs.o \
	co_conns.o \
	co_strings.o \
	co_mem.o \
	co_pFuncs.o \
	co_expr.o \
	co_stmt.o \
	co_struct.o \
	co_sites.o \
	co_io.o \
	co_logical.o \
	co_params.o \
	co_loop.o

SOURCES= Tokens.l Syntax.y \
	co_constants.h co_types.h co_prototypes.h co_include.h \
	co_main.h co_main.c \
	co_parser.h co_parser.c \
	co_scanner.h co_scanner.c \
	co_symTab.h co_symTab.c \
	co_error.h co_error.c \
	co_output.h co_output.c \
	co_cFuncs.h co_cFuncs.c \
	co_conns.h co_conns.c \
	co_strings.h co_strings.c \
	co_mem.h co_mem.c \
	co_pFuncs.h co_pFuncs.c \
	co_expr.h co_expr.c \
	co_stmt.h co_stmt.c \
	co_struct.h co_struct.c \
	co_sites.h co_sites.c \
	co_io.h co_io.c \
	co_logical.h co_logical.c \
	co_params.h co_params.c \
	co_loop.h co_loop.c


# Rules

.KEEP_STATE:

default: $(PROGRAM)

all: $(PROGRAM) 

install:
		cp $(PROGRAM) $(COMPILERBINDIR)


# because of checking of timestamps this is neccessary!!!	

list:	$(SOURCES)
	cat $(SOURCES) >  compilersource.c

clean:
	rm -f $(COMPOBJECTS) $(PROGRAM) co_scanner.c co_parser.c co_pFuncs.h


$(PROGRAM): $(COMPOBJECTS)
	$(CC) -o $(PROGRAM) $(COMPOBJECTS) $(LDOPTIONS)



# dependencies for compiler 

co_main.o: co_constants.h co_types.h co_prototypes.h co_include.h co_main.c

co_parser.o: co_constants.h co_types.h co_prototypes.h co_include.h co_parser.c

co_pFuncs.o: co_pFuncs.c co_pFuncs.h

co_pFuncs.h: co_constants.h co_types.h co_prototypes.h co_include.h co_parser.c co_pfBody.h co_scanner.c
		fgrep '#define YYMAXDEPTH' co_parser.c >  temp1
		fgrep '# define YYLMAX' co_scanner.c >  temp2
		cat co_pfBody.h temp1 temp2 > co_pFuncs.h
		/bin/rm temp1 temp2

co_scanner.o: co_constants.h co_types.h co_prototypes.h co_include.h co_scanner.c

co_scanner.c: Tokens.l co_scanner.h
		lex Tokens.l 
		mv lex.yy.c co_scanner.c

#parser built by yacc, y.tab.c is modified to achieve a better error handling
#modifications are described in 
#		Schreiner, Friedman: Compiler bauen mit UNIX
#		Hanser Verlag, 1985, pp. 236-239.
co_parser.c: Syntax.y
		yacc -d Syntax.y
		cat y.tab.c | sed -f sed.parser > co_parser.c
		cat co_parser.c | sed -f sed.sparc.parser > sed.tmp.parser
		mv sed.tmp.parser co_parser.c
		rm y.tab.c

co_symTab.o: co_constants.h co_types.h co_prototypes.h co_include.h co_symTab.c

co_error.o: co_constants.h co_types.h co_prototypes.h co_include.h co_error.c

co_output.o: co_constants.h co_types.h co_prototypes.h co_include.h co_output.c

co_cFuncs.o: co_constants.h co_types.h co_prototypes.h co_include.h co_cFuncs.c

co_conns.o: co_constants.h co_types.h co_prototypes.h co_include.h co_conns.c

co_strings.o: co_constants.h co_types.h co_prototypes.h co_include.h co_strings.c co_tokPrint.h

co_mem.o: co_constants.h co_types.h co_prototypes.h co_include.h co_mem.c

co_expr.o: co_constants.h co_types.h co_prototypes.h co_include.h co_expr.c

co_stmt.o: co_constants.h co_types.h co_prototypes.h co_include.h co_stmt.c

co_struct.o: co_constants.h co_types.h co_prototypes.h co_include.h co_struct.c

co_sites.o: co_constants.h co_types.h co_prototypes.h co_include.h co_sites.c

co_io.o: co_constants.h co_types.h co_prototypes.h co_include.h co_io.c

co_logical.o: co_constants.h co_types.h co_prototypes.h co_include.h co_logical.c

co_params.o: co_constants.h co_types.h co_prototypes.h co_include.h co_params.c

co_loop.o: co_constants.h co_types.h co_prototypes.h co_include.h co_loop.c
