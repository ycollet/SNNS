# makefile for ENZO

# Targets and directories

TARGETS    = all 
SUBDIRS    = src 

XGRAF      = xgraf
XGRAFDIR   = src/history/Xgraf

DOCU       = docu
DOCUDIR    = doc/english
ALLTARGETS =  src docu xgraf

CLEAN      = clean
DEBUG      = debug

# Rules

$(TARGETS): FORCE
	$(MAKE) $(SUBDIRS) TARGET=$@

$(CLEAN): FORCE
	$(MAKE) $(ALLTARGETS)  TARGET=$@

$(DEBUG): FORCE
	$(MAKE) $(SUBDIRS) TARGET=$@

$(SUBDIRS): FORCE
	cd $@; $(MAKE) $(TARGET); 

$(XGRAF): FORCE
	cd $(XGRAFDIR); $(MAKE) $(TARGET);

$(DOCU): FORCE
	cd $(DOCUDIR); $(MAKE) $(TARGET);
FORCE:

#eof