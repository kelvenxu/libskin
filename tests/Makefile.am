noinst_PROGRAMS = \
	test-skin-window \
	test-skin-archive\
	test-skin-builder\
	test-skin-hscale


common_ldflags = \
	$(top_builddir)/libskin/libskin.la \
	$(PACKAGE_LIBS)

test_skin_window_SOURCES = test-skin-window.c
test_skin_window_LDADD   = $(common_ldflags)

test_skin_archive_SOURCES = test-skin-archive.c
test_skin_archive_LDADD   = $(common_ldflags)

test_skin_builder_SOURCES = test-skin-builder.c
test_skin_builder_LDADD   = $(common_ldflags)

test_skin_hscale_SOURCES = test-skin-hscale.c
test_skin_hscale_LDADD   = $(common_ldflags)

INCLUDES = \
	-I$(top_srcdir) \
	$(PACKAGE_CFLAGS)
