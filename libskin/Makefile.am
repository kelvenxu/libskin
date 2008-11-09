skinincdir = $(includedir)/libskin

lib_LTLIBRARIES = libskin.la

skin-marshal.h:	marshal.list
	glib-genmarshal --prefix=skin_marshal ./marshal.list --header > skin-marshal.h

skin-marshal.c:	marshal.list
	glib-genmarshal --prefix=skin_marshal ./marshal.list --body > skin-marshal.c

skin_headers = \
	skin.h \
	skinwindow.h\
	skinbutton.h\
	skincheckbutton.h\
	skintogglebutton.h\
	skindigitaltime.h\
	skindynamictext.h\
	skinscrollbar.h\
	skinstatusbar.h\
	skinvolumebutton.h\
	skinhscale.h\
	skinvscale.h\
	skinarchive.h\
	skinbuilder.h

skininc_HEADERS = \
	$(skin_headers) \
	skin-enum-types.h

noinst_HEADERS = \
	skin-marshal.h

BUILT_SOURCES = \
	skin-enum-types.h \
	skin-enum-types.c \
	skin-marshal.h \
	skin-marshal.c

libskin_la_SOURCES = \
	skin-enum-types.c \
	skinwindow.c\
	skinbutton.c\
	skincheckbutton.c\
	skintogglebutton.c\
	skindigitaltime.c\
	skindynamictext.c\
	skinprocessbar.c\
	skinscrollbar.c\
	skinstatusbar.c\
	skinvolumebutton.c\
	skinhscale.c\
	skinvscale.c\
	skinarchive.c\
	skinbuilder.c\
	skin-marshal.c

libskin_la_LIBADD = $(PACKAGE_LIBS)
libskin_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) $(NO_UNDEFINED)

skin-enum-types.h: s-enum-types-h
	@true
s-enum-types-h: @REBUILD@ $(skin_headers) Makefile
	( cd $(srcdir) && glib-mkenums \
			--fhead "#ifndef _SKIN_ENUM_TYPES_H_\n#define _SKIN_ENUM_TYPES_H_\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
			--fprod "/* enumerations from \"@filename@\" */\n" \
			--vhead "GType @enum_name@_get_type(void);\n#define SKIN_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n"	\
			--ftail "G_END_DECLS\n\n#endif /* _SKIN_ENUM_TYPES_H_ */" \
		$(skin_headers) ) > tmp-skin-enum-types.h \
	&& (cmp -s tmp-skin-enum-types.h skin-enum-types.h || cp tmp-skin-enum-types.h skin-enum-types.h ) \
	&& rm -f tmp-skin-enum-types.h	\
	&& echo timestamp > $(@F)

skin-enum-types.c: s-enum-types-c
	@true
s-enum-types-c: @REBUILD@ $(skin_headers) Makefile
	( cd $(srcdir) && glib-mkenums \
			--fhead "#include <libskin/skin.h>" \
			--fprod "\n/* enumerations from \"@filename@\" */" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {" 	\
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(skin_headers) ) > tmp-skin-enum-types.c \
	&& (cmp -s tmp-skin-enum-types.c skin-enum-types.c || cp tmp-skin-enum-types.c skin-enum-types.c ) \
	&& rm -f tmp-skin-enum-types.c \
	&& echo timestamp > $(@F)

INCLUDES = \
	-I$(top_srcdir) \
	$(PACKAGE_CFLAGS)

EXTRA_DIST = \
	skin-enum-types.h \
	marshal.list

CLEANFILES = \
	skin-marshal.h \
	skin-marshal.c \
	skin-enum-types.h \
	s-enum-types-h \
	skin-enum-types.c \
	s-enum-types-c
