AM_CPPFLAGS = -Wextra -D_DEFAULT_SOURCE \
	-I$(top_srcdir) \
	-I$(top_srcdir)/hkl \
	$(DATATYPE99_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(GSL_CFLAGS)

# need access to the private part
AM_LDFLAGS = -Wl,--whole-archive,$(top_builddir)/hkl/.libs/libhkl.a,--no-whole-archive

LDADD = \
	$(GLIB_LIBS) \
	$(GSL_LIBS)

noinst_LTLIBRARIES=libhkl2.la
libhkl2_la_SOURCES = \
	hkl2.c \
	hkl2.h
