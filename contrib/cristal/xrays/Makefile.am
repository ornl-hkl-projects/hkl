# Process this file with automake to produce Makefile.in

AM_CFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/hkl \
	-I.. \
	-DPKGDATA=\""$(pkgdatadir)"\" \
	-fPIC -DPIC \
	$(CGLM_CFLAGS) \
	$(DATATYPE99_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(HDF5_CFLAGS)

AM_LDFLAGS = -version-info 0:0:0 \
	$(CGLM_LIBS) \
	$(GLIB_LIBS) \
	$(HDF5_LIBS) -lm

noinst_LTLIBRARIES = libhkl-xrays.la
libhkl_xrays_la_SOURCES = \
	usage.c \
	xrays-droplet.c \
	xrays-droplet.h \
	xrays-image.c \
	xrays-image.h \
	xrays-image-spe.c \
	xrays-image-spe.h \
	xrays-image-dat.c \
	xrays-macros.h

# Support for GNU Flymake, in Emacs.
check-syntax: AM_CFLAGS += -fsyntax-only -pipe
check-syntax:
	test -z "$(CHK_SOURCES)" || $(COMPILE) $(CHK_SOURCES)

.PHONY: check-syntax
