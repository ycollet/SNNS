# makefile f"ur enzo-doku


TEX_SOURCES = enzo.tex  userman.tex devman.tex design.tex          \
              pre_evolution.tex  selection.tex   crossover.tex      \
              mutation.tex       opt.tex         evaluation.tex     \
              history.tex        survival.tex    stop_evolution.tex \
              post_evolution.tex




.KEEP_STATE:

all: enzo

docu: enzo

touchps:
	touch ../enzo.ps
	
enzo:  touchps
	/bin/rm *.aux enzo.i*
	/bin/cp enzo.dummy enzo.index
	latex enzo.tex
	echo # ---- making index -------- #
	makeindex -o enzo.index -p odd enzo
	echo # ---- making bibtex ------- #
	bibtex enzo
	echo # ---- making latex -------- #
	latex enzo.tex
	echo # ---- making latex agein -- #
	latex enzo.tex
	echo # ---- making ps-file  ----- #
	dvips -o ../enzo.ps enzo.dvi


clean:  FORCE
	rm -f *.dvi *.ps *.log *~ *.aux  *.bbl *.blg *.toc

FORCE:


#eof


