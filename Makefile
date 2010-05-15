##################################################################
#                                                                #
# Makefile (generic program creation Makefile)                   #
#                                                                #
# Thomas Nemeth.                                                 #
#                                                                #
##################################################################


##################################################################
#                                                                #
#            PARTIE DES VARIABLES NON MODIFIABLES !              #
#              NO MODIFICATION BELOW THIS LINE !                 #
#                                                                #
##################################################################

include .project
PROJECT_DESC     = $(shell echo "$(PROJECT_DESCRIPTION)" | sed 's/"//g')


#*****************************************************************
# REVISON CHANGE :
#
BUILD            = $(shell date +%Y%m%d-%H%M)


#*****************************************************************
# PROJECT CONFIGURATION :
#
TEMPLATE_CFG     = doc/templates/0config.h.in
TEMPLATE_DOX     = doc/templates/doxygen.conf.in
TEMPLATE_SCR     = doc/templates/make_cfg
TEMPLATE_DOC	 = project-documentation.txt


#*****************************************************************
# SYSTEM INFORMATION :
#
ifndef DEBUG_LEVEL
  DEBUG_LEVEL    = 0
endif
ifndef CROSS_COMPILE
  CROSS_COMPILE  =
endif
ifndef DESTDIR
  DESTDIR        =
endif


#*****************************************************************
# SYSTEM DIRECTORIES :
#
INSTALLDIR       = $(DESTDIR)$(PREFIX)
BINDIR           = $(INSTALLDIR)/bin
MANDIR           = $(INSTALLDIR)/man/man1
LIBDIR           = $(INSTALLDIR)/lib
INCDIR           = $(INSTALLDIR)/include


#*****************************************************************
# FLAGS :
#
CFLAGS           = -DBUILD=\"${BUILD}\"
CPPFLAGS         = -O2 -Wall
EXTRA_CPPFLAGS   =
LDFLAGS          =


#*****************************************************************
# DEBUG MODE FLAGS :
#
DEBUG_CFLAGS     = -g3
DEBUG_CPPFLAGS   = -DDEBUG_LEVEL=${DEBUG_LEVEL}
DEBUG_LDFLAGS    = -g3


#*****************************************************************
# TARGET - SOURCES - OBJECTS :
#
ifeq ($(strip $(PROGRAM)),)
  TARGET         = noprogname
else
  TARGET         = $(PROGRAM)
endif
SRCS             = $(shell ls *.${PROJECT_LANGUAGE})
ASMS             = $(SRCS:%.${PROJECT_LANGUAGE}=%.s)
OBJS             = $(SRCS:%.${PROJECT_LANGUAGE}=%.o)


##################################################################
# GLOBAL DEFINITIONS :
#
DEFAULT_CPPFLAGS = $(CPPFLAGS) $(EXTRA_CPPFLAGS)

ALL_CFLAGS       = $(CFLAGS) $(DEBUG_CFLAGS) $(USER_CFLAGS)
ALL_CPPFLAGS     = $(DEFAULT_CPPFLAGS) $(DEBUG_CPPFLAGS) $(USER_CPPFLAGS)
ALL_LIBS         = $(LDFLAGS) $(DEBUG_LDFLAGS) $(USER_LDFLAGS)


#*****************************************************************
# COMMON COMMANDS :
#
CPP              = $(CROSS_COMPILE)cpp
ifeq ($(strip $(PROJECT_LANGUAGE)),c)
  CC             = $(CROSS_COMPILE)gcc
else
  CC             = $(CROSS_COMPILE)g++
endif
STRIP            = $(CROSS_COMPILE)strip
CP               = cp
RM               = rm
INSTALL          = install
TAR              = tar
BZIP             = bzip2
SED              = sed


##################################################################
# RULES :
#
.PHONY: all noprogname doc dep mostlyclean clean distclean mrproper strip install install-strip uninstall package help love war

all: 0config.h doc/doxygen.conf $(TARGET)

asm: 0config.h $(TARGET)-asm

noprogname:
	@echo "ERROR: No program name given."
	@echo "Change the PROGRAM variable in the .project file."
	@exit 2

0config.h: $(TEMPLATE_CFG) .project $(TEMPLATE_DOC)
	@echo "Building 0config.h..."
	@$(TEMPLATE_SCR) $< $(TEMPLATE_DOC) | $(SED) \
		-e "s/#PACKAGE#/$(PACKAGE)/" \
	        -e "s/#PROGRAM#/$(PROGRAM)/" \
	        -e "s/#VERSION#/$(VERSION)/" \
	        -e 's/#AUTHORS#/$(AUTHOR)/' > $@

doc/doxygen.conf: $(TEMPLATE_DOX) .project
	@echo "Building Doxygen configuration..."
	@$(SED) -e "s/##PROJECT_NAME##/$(PROJECT_NAME)/" \
	        -e "s/##PROJECT_DESCRIPTION##/$(PROJECT_DESC)/" \
	        -e "s/##VERSION##/$(VERSION)/" $< > $@

doc: doc/doxygen.conf $(SRCS) 0config.h
	@echo "Generating documentation..."
	@$(RM) -rf ./doc/html
	@$(RM) -rf ./doc/latex
	@mkdir -p ./doc/html
	@mkdir -p ./doc/latex
	@cp doc/templates/logo.jpg ./doc/html
	@cp doc/templates/header.tex ./doc/latex
	@doxygen doc/doxygen.conf 2> /dev/null
	@echo "Building PDF file..."
	@(cd ./doc/latex ; make > /dev/null 2>&1)
	@(cd ./doc/latex ; make > /dev/null 2>&1)
	@cp ./doc/latex/refman.pdf $(PACKAGE)-refman.pdf

dep: .dependencies

$(PROGRAM): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(ALL_LIBS)

%.o: %.c
	$(CC) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $<

version.o: FORCE

$(PROGRAM)-asm: $(ASMS)

%.s: %.c
	$(CC) -S $(ALL_CFLAGS) $(ALL_CPPFLAGS) $<

version.s: FORCE

mostlyclean:
	-$(RM) -f *~ *.o
	-$(RM) -f core

clean: mostlyclean
	-$(RM) -f $(TARGET)

realclean: clean
	-$(RM) -r .dependencies

distclean: realclean
	-$(RM) -f 0config.h
	-$(RM) -f doc/doxygen.conf

mrproper: distclean
	-$(RM) -rf ./doc/html
	-$(RM) -rf ./doc/latex

strip: all
	$(STRIP) $(TARGET)

install: $(TARGET)
	$(INSTALL) -d --mode=2775 $(BINDIR)
	$(INSTALL) --mode=0775 ./$(TARGET) $(BINDIR)

install-strip: install
	$(STRIP) $(BINDIR)/$(TARGET)

uninstall:
	-$(RM) -f $(BINDIR)/$(TARGET)

package: clean
	@-$(RM) -f ../$(PROGRAM)-$(VERSION).tar.gz
	@cd .. ; \
	  $(TAR) -cf $(PROGRAM)-$(VERSION).tar $(PROGRAM) ; \
	  $(BZIP) $(PROGRAM)-$(VERSION).tar ; \
	  md5sum $(PROGRAM)-$(VERSION).tar.bz2 > $(PROGRAM)-$(VERSION).md5

help:
	@echo "Targets for building $(TARGET):"
	@echo "  all:           configure and build the program (default)"
	@echo "  doc:           build the documentation"
	@echo
	@echo "Misc. targets:"
	@echo "  dep:           rebuild the dependencies file"
	@echo "  strip:         build 'all' and strip the program"
	@echo "  install:       install $(TARGET) to $(BINDIR)"
	@echo "  install-strip: 'install' and strip $(BINDIR)/$(TARGET)"
	@echo "  uninstall:     uninstall the program"
	@echo "  package:       'clean' and create a .tar.bz2 package and its md5sum"
	@echo
	@echo "Targets for cleaning the build directory:"
	@echo "  mostlyclean:   remove built objects, backups and core files"
	@echo "  clean:         'mostlyclean' and delete $(TARGET)"
	@echo "  realclean:     'clean' and delete dependencies"
	@echo "  distclean:     'realclean' and delete automatically produced files"
	@echo "  mrproper:      'distclean' then delete the documentation"

love:
	@echo 'not war?'

war:
	@echo 'I prefer love!'

.dependencies: 0config.h
	@echo "Making dependencies in `basename ${PWD}`..."
	@-$(RM) -f .dependencies
	@touch .dependencies
	@for i in ${SRCS} ; do \
	  echo "Making dependencies for $$i" ; \
	  $(CPP) $(ALL_CPPFLAGS) -MM $$i >> .dependencies ; \
	done

FORCE:

include .dependencies
