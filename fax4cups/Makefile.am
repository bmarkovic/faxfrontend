# $Id: Makefile.am,v 1.2 2007/03/04 17:41:25 glenn Exp $
#
bin_PROGRAMS = ffe_messagebox
ffe_messagebox_SOURCES = ffe_messagebox.cc
ffe_messagebox_CXXFLAGS = $(CXXFLAGS) $(QT_CXXFLAGS)
ffe_messagebox_LDADD = $(LDADD) $(QT_LDADD)

EXTRA_DIST = hylafax.in hylafax.ppd notes

install-data-local:
	$(mkinstalldirs) /usr/lib/cups/backend/
	$(INSTALL_SCRIPT) $(srcdir)/hylafax /usr/lib/cups/backend/hylafax
	$(mkinstalldirs) /usr/share/cups/
	$(INSTALL_DATA) $(srcdir)/hylafax.ppd /usr/share/cups/model

uninstall-local:
	-rm -f /usr/lib/cups/backend/hylafax
	-rm -f /usr/share/cups/model/hylafax.ppd
