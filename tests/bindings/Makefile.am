TESTS_ENVIRONMENT=env GI_TYPELIB_PATH=$(top_builddir)/hkl DATADIR=$(srcdir) $(LIBTOOL) --mode=execute -dlopen $(top_builddir)/hkl/libhkl.la
TESTS = python.py trajectory.py polarisation.py

EXTRA_DIST = $(TESTS) crystal.ini sample1.txt

ipython:
	env GI_TYPELIB_PATH=$(top_builddir)/hkl $(LIBTOOL) --mode=execute -dlopen $(top_builddir)/hkl/libhkl.la ipython3 -i -c "from gi.repository import Hkl"

gdb:
	env GI_TYPELIB_PATH=$(top_builddir)/hkl $(LIBTOOL) --mode=execute -dlopen $(top_builddir)/hkl/libhkl.la gdb python3

CLEANFILES = trajectories.pdf
