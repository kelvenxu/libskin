SUBDIRS = libskin docs tests

pcdata_DATA= libskin.pc
pcdatadir = $(libdir)/pkgconfig

EXTRA_DIST = \
	AUTHORS NEWS INSTALL COPYING README \
	ChangeLog \
	autogen.sh \
	configure.ac \
	libskin.pc.in

$(OBJECTS): libtool
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc
