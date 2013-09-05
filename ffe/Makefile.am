# $Id: Makefile.am,v 1.1 2004/01/26 14:51:24 glenn Exp $
#
bin_PROGRAMS = faxfrontend

faxfrontend_MOC = FaxFrontEnd.moc.cc FaxFrontEndDsgn.moc.cc \
		RolodexParser.moc.cc \
		rolodex.moc.cc rolodexDsgn.moc.cc \
		faxstat.moc.cc faxstatDsgn.moc.cc

faxfrontend_UI = rolodexDsgn.cc rolodexDsgn.h \
		FaxFrontEndDsgn.cc FaxFrontEndDsgn.h \
		faxstatDsgn.cc faxstatDsgn.h

faxfrontend_SOURCES = main.cc rolodex.cc faxstat.cc FaxFrontEnd.cc \
		RolodexParser.cc FaxFrontEnd.h rolodex.h RolodexParser.h \
		faxstat.h

nodist_faxfrontend_SOURCES = $(faxfrontend_UI) $(faxfrontend_MOC)

EXTRA_DIST = FaxFrontEndDsgn.ui faxstatDsgn.ui rolodexDsgn.ui
BUILT_SOURCES = $(faxfrontend_MOC) $(faxfrontend_UI)
CLEANFILES = $(BUILT_SOURCES)
DISTCLEANFILES = $(BUILT_SOURCES)

rolodexDsgn.h: rolodexDsgn.ui
rolodexDsgn.cc : rolodexDsgn.h rolodexDsgn.ui
FaxFrontEndDsgn.h: FaxFrontEndDsgn.ui
FaxFrontEndDsgn.cc : FaxFrontEndDsgn.h FaxFrontEndDsgn.ui
faxstatDsgn.h: faxstatDsgn.ui
faxstatDsgn.cc: faxstatDsgn.h faxstatDsgn.ui

faxfrontend_LDADD   = $(LDADD) $(QT_LDADD)
faxfrontend_CXXFLAGS = $(CXXFLAGS) $(QT_CXXFLAGS)

%.moc.cc: %.h
	$(MOC) -o $@ $<
%.h: %.ui
	$(UIC) -o $@ $<
%.cc: %.ui
	$(UIC) -o $@ -impl $*.h $<

